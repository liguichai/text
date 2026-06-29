#include <WiFi.h>
#include <WebServer.h>

const char* ssid = "iQOO 12";
const char* password = "qweasdfg";
const int LED_PIN = 2;

WebServer server(80);
int currentDuty = 0;  // 当前占空比 (0-255)

String makePage() {
  // 读取当前占空比（确保滑动条与状态同步）
  String html = R"rawliteral(
<!DOCTYPE html>
<html lang="zh-CN">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>无极调光器</title>
  <style>
    body { font-family: Arial; text-align: center; margin-top: 50px; }
    input[type=range] { width: 80%; margin: 20px 0; }
    #val { font-weight: bold; font-size: 1.2em; }
  </style>
</head>
<body>
  <h1>Web 无极调光器</h1>
  <p>当前亮度：<span id="val">)rawliteral" + String(currentDuty) + R"rawliteral(</span></p>
  <input type="range" id="slider" min="0" max="255" value=")rawliteral" + String(currentDuty) + R"rawliteral(" oninput="updateLED(this.value)">
  
  <p>快捷按钮</p>
  <a href="/on"><button>点亮 (100%)</button></a>
  <a href="/off"><button>熄灭 (0%)</button></a>

  <script>
    function updateLED(val) {
      document.getElementById('val').innerText = val;
      // 发送GET请求更新PWM值
      fetch('/set?value=' + val)
        .then(response => console.log('亮度已设为: ' + val))
        .catch(err => console.error('请求失败:', err));
    }
  </script>
</body>
</html>
)rawliteral";
  return html;
}

void handleRoot() {
  server.send(200, "text/html; charset=UTF-8", makePage());
}

void handleOn() {
  currentDuty = 255;
  ledcWrite(LED_PIN, currentDuty);
  server.sendHeader("Location", "/");
  server.send(303);
}

void handleOff() {
  currentDuty = 0;
  ledcWrite(LED_PIN, currentDuty);
  server.sendHeader("Location", "/");
  server.send(303);
}

// 处理滑动条GET请求：/set?value=xxx
void handleSet() {
  if (server.hasArg("value")) {
    int val = server.arg("value").toInt();
    val = constrain(val, 0, 255);
    currentDuty = val;
    ledcWrite(LED_PIN, currentDuty);
  }
  server.send(200, "text/plain", "OK");
}

void setup() {
  Serial.begin(115200);
  
  // 配置LEDC PWM（8位分辨率，频率5000Hz）
  ledcAttach(LED_PIN, 5000, 8);
  ledcWrite(LED_PIN, currentDuty);
  
  WiFi.begin(ssid, password);
  Serial.print("连接WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\n连接成功");
  Serial.print("访问地址: http://");
  Serial.println(WiFi.localIP());

  server.on("/", handleRoot);
  server.on("/on", handleOn);
  server.on("/off", handleOff);
  server.on("/set", handleSet);
  server.begin();
}

void loop() {
  server.handleClient();
  Serial.print("访问地址: http://");
  Serial.println(WiFi.localIP());
}