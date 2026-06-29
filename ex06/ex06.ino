// 警车双闪灯效（反相渐变呼吸）
const int ledPin_r = 4;   // 红色 LED (或灯A)
const int ledPin_y = 5;   // 黄色 LED (或灯B)

const int freq = 5000;
const int resolution = 8; // 8位分辨率 (0-255)

void setup() {
  Serial.begin(115200);

  // 绑定引脚到LEDC PWM
  ledcAttach(ledPin_r, freq, resolution);
  ledcAttach(ledPin_y, freq, resolution);
}

void loop() {
  // 阶段1：灯A从0逐渐变亮，灯B从255逐渐变暗（反相）
  for (int dutyA = 0, dutyB = 255; dutyA <= 255; dutyA++, dutyB--) {
    ledcWrite(ledPin_r, dutyA);
    ledcWrite(ledPin_y, dutyB);
    delay(10);
  }

  // 阶段2：灯A从255逐渐变暗，灯B从0逐渐变亮
  for (int dutyA = 255, dutyB = 0; dutyA >= 0; dutyA--, dutyB++) {
    ledcWrite(ledPin_r, dutyA);
    ledcWrite(ledPin_y, dutyB);
    delay(10);
  }
}
