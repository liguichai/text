// 定义LED引脚，ESP32通常板载LED连接在GPIO 2
const int ledPin = 2; 

void setup() {
  // 初始化串口通信，设置波特率为115200
  Serial.begin(115200);
  // 将LED引脚设置为输出模式
  pinMode(ledPin, OUTPUT);
}

// 基于millis()的非阻塞实现
unsigned long previousMillis = 0;
const long interval = 500;

void loop() {
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    digitalWrite(ledPin, !digitalRead(ledPin));  // 状态翻转
  }
}
