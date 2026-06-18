// 定义LED引脚，ESP32通常板载LED连接在GPIO 2
const int ledPin = 2;

// SOS序列的步骤：LED状态和持续时间(ms)
const int steps = 21;
const int ledStates[steps] = {
  HIGH, LOW, HIGH, LOW, HIGH, LOW,    // S: 3短闪 (ON 200, OFF 200) ×3
  LOW,                                // 中间间隔 500ms (LED灭)
  HIGH, LOW, HIGH, LOW, HIGH, LOW,    // O: 3长闪 (ON 600, OFF 200) ×3
  LOW,                                // 中间间隔 500ms (LED灭)
  HIGH, LOW, HIGH, LOW, HIGH, LOW,    // S: 3短闪 (ON 200, OFF 200) ×3
  LOW                                 // 单次循环间隔 2000ms (LED灭)
};
const int intervals[steps] = {
  200, 200, 200, 200, 200, 200,       // S短闪
  500,                                // 中间间隔
  600, 200, 600, 200, 600, 200,      // O长闪
  500,                                // 中间间隔
  200, 200, 200, 200, 200, 200,      // S短闪
  2000                                // 单次循环间隔
};

int currentStep = 0;
unsigned long previousMillis = 0;

void setup() {
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);
  // 初始化第一个步骤的状态
  digitalWrite(ledPin, ledStates[currentStep]);
  previousMillis = millis();
}

void loop() {
  unsigned long currentMillis = millis();
  
  // 检查当前步骤持续时间是否已到
  if (currentMillis - previousMillis >= intervals[currentStep]) {
    // 移动到下一个步骤
    currentStep = (currentStep + 1) % steps;
    digitalWrite(ledPin, ledStates[currentStep]);
    previousMillis = currentMillis;
  }
}