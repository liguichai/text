#include <WiFi.h>
#include <WebServer.h>

const char* ssid = "iQOO 12";
const char* password = "qweasdfg";

const int LED_PIN = 2;      // 报警指示灯
const int TOUCH_PIN = 4;    // 触摸引脚 T0 (GPIO4)

WebServer server(80);

// 安防系统全局状态
bool armed = false;          // 布防状态
bool alarmActive = false;    // 报警状态（LED 闪烁）

// 触摸边缘检测与防抖
bool lastTouchState = false;
unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 200;  // 防抖时间 200ms
const int touchThreshold = 400;           // 触摸阈值（需根据实际调整）

// LED 闪烁控制（非阻塞）
unsigned long lastBlinkTime = 0;
const unsigned long blinkInterval = 80;   // 闪烁间隔（毫秒），越小越急促
bool ledState = false;

// 生成网页
String makePage() {
  String statusText;
  if (alarmActive) {
    statusText = "⚠️ 报警中！";
  } else if (armed) {
    statusText = "🔒 已布防";
  } else {
    statusText = "🔓 已撤防";
  }

  String html = R"rawliteral(
<!DOCTYPE html>
<html lang="zh-CN">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>安防报警器</title>
  <style>
    body { font-family: Arial; text-align: center; margin-top: 50px; }
    .status { font-size: 1.5em; margin: 20px 0; }
    button { padding: 12px 24px; margin: 10px; font-size: 1em; }
  </style>
</head>
<body>
  <h1>物联网安防主机</h1>
  <div class="status">当前状态：<b>)rawliteral" + statusText + R"rawliteral(</b></div>
  <div>
    <a href="/arm"><button>🔒 布防</button></a>
    <a href="/disarm"><button>🔓 撤防</button></a>
  </div>
  <p style="margin-top:30px; color:#888;">布防后触摸引脚将触发报警，需网页撤防才能关闭</p>
</body>
</html>
)rawliteral";
  return html;
}

// 首页
void handleRoot() {
  server.send(200, "text/html; charset=UTF-8", makePage());
}

// 布防请求：仅在未报警时生效，报警状态下无效
void handleArm() {
  if (!alarmActive) {
    armed = true;
    alarmActive = false;
    ledcWrite(LED_PIN, 0);       // 确保 LED 熄灭
  }
  // 重定向回首页
  server.sendHeader("Location", "/");
  server.send(303);
}

// 撤防请求：总是生效，清除布防和报警状态，熄灭 LED
void handleDisarm() {
  armed = false;
  alarmActive = false;
  ledcWrite(LED_PIN, 0);
  server.sendHeader("Location", "/");
  server.send(303);
}

void setup() {
  Serial.begin(115200);

  // 初始化 LED 为 PWM 输出（频率 5000Hz，8位分辨率）
  ledcAttach(LED_PIN, 5000, 8);
  ledcWrite(LED_PIN, 0);

  // 连接 WiFi
  WiFi.begin(ssid, password);
  Serial.print("连接WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\n连接成功");
  Serial.print("访问地址: http://");
  Serial.println(WiFi.localIP());

  // 配置 Web 服务器路由
  server.on("/", handleRoot);
  server.on("/arm", handleArm);
  server.on("/disarm", handleDisarm);
  server.begin();
}

void loop() {
  server.handleClient();

  // --- 触摸检测与报警触发逻辑 ---
  int touchValue = touchRead(TOUCH_PIN);
  bool touchNow = (touchValue < touchThreshold);  // 触摸时值变小

  // 边缘检测（从未触摸变为触摸）+ 防抖
  if (touchNow && !lastTouchState) {
    if (millis() - lastDebounceTime > debounceDelay) {
      // 仅在布防状态且未报警时，触摸才触发报警
      if (armed && !alarmActive) {
        alarmActive = true;
        Serial.println("警报！触摸触发，进入报警状态");
      }
      lastDebounceTime = millis();
    }
  }
  lastTouchState = touchNow;

  // 如果撤防，自动关闭报警（与按钮逻辑一致）
  if (!armed && alarmActive) {
    alarmActive = false;
    ledcWrite(LED_PIN, 0);
    ledState = false;
  }

  // --- 报警时 LED 高频闪烁（非阻塞） ---
  if (alarmActive) {
    unsigned long now = millis();
    if (now - lastBlinkTime >= blinkInterval) {
      ledState = !ledState;
      ledcWrite(LED_PIN, ledState ? 255 : 0);
      lastBlinkTime = now;
    }
  }
}