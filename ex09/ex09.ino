#include <WiFi.h>
#include <WebServer.h>

const char* ssid = "iQOO 12";
const char* password = "qweasdfg";

const int TOUCH_PIN = 4;     // 触摸传感器引脚 T0 (GPIO4)

WebServer server(80);

// 生成网页（仅显示实时触摸传感器数值）
String makePage() {
  String html = R"rawliteral(
<!DOCTYPE html>
<html lang="zh-CN">
<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <title>实时传感器仪表盘</title>
  <style>
    body { font-family: Arial; text-align: center; margin-top: 80px; }
    .sensor-box {
      font-size: 6em; font-weight: bold; margin: 30px 0;
      color: #2196F3; background: #f0f0f0; display: inline-block;
      padding: 30px 60px; border-radius: 20px;
      box-shadow: 0 4px 12px rgba(0,0,0,0.2);
    }
    h1 { color: #333; }
    .desc { color: #888; margin-top: 30px; }
  </style>
</head>
<body>
  <h1>触摸传感器实时数值</h1>
  <div class="sensor-box" id="sensorValue">--</div>
  <p class="desc">手靠近或触摸 GPIO4 (T0)，数值会实时变小</p>

  <script>
    // 定时拉取触摸传感器数值
    function fetchSensor() {
      fetch('/touch')
        .then(response => response.text())
        .then(data => {
          document.getElementById('sensorValue').innerText = data;
        })
        .catch(err => console.error('传感器数据获取失败:', err));
    }
    setInterval(fetchSensor, 200); // 每200毫秒更新一次
    fetchSensor(); // 页面加载后立即获取一次
  </script>
</body>
</html>
)rawliteral";
  return html;
}

// 首页
void handleRoot() {
  server.send(200, "text/html; charset=UTF-8", makePage());
}

// 返回触摸传感器实时数值（纯文本）
void handleTouch() {
  int touchValue = touchRead(TOUCH_PIN);
  server.send(200, "text/plain", String(touchValue));
}

void setup() {
  Serial.begin(115200);

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

  // 注册 URL 处理函数（仅首页和传感器数据接口）
  server.on("/", handleRoot);
  server.on("/touch", handleTouch);
  server.begin();
}

void loop() {
  server.handleClient();
  Serial.print("访问地址: http://");
  Serial.println(WiFi.localIP());
}