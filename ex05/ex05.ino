// 多档位触摸调速呼吸灯
#define TOUCH_PIN 4     // 触摸引脚 T0 (GPIO4)
#define LED_PIN 2       // 板载 LED
#define THRESHOLD 400   // 触摸阈值，低于此值视为触摸（需根据硬件调整）

// PWM 参数
const int freq = 5000;
const int resolution = 8;

// 三档呼吸速度对应的延时 (ms)，数值越小呼吸越快
const int speedDelays[3] = {15, 7, 3}; // 1 慢速，2 中速，3 快速

// 中断相关变量
volatile int speedLevel = 1;            // 当前档位 1/2/3
volatile bool lastTouchState = false;   // 上一次触摸状态（边缘检测）
volatile unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 150;
volatile bool levelChanged = false;     // 通知主循环档位已更新

// 中断服务函数声明
void IRAM_ATTR gotTouch();

void setup() {
  Serial.begin(115200);
  delay(1000);

  // 初始化 PWM
  ledcAttach(LED_PIN, freq, resolution);   // 新版 ESP32 Arduino 核心用法

  // 绑定触摸中断
  touchAttachInterrupt(TOUCH_PIN, gotTouch, THRESHOLD);

  Serial.println("多档位触摸调速呼吸灯已启动");
  Serial.println("触摸 T0 引脚切换速度：1(慢) -> 2(中) -> 3(快) -> 1...");
}

void loop() {
  // 打印档位变化
  if (levelChanged) {
    Serial.print("当前档位: ");
    Serial.println(speedLevel);
    levelChanged = false;
  }

  // 取当前档位对应的延时
  int stepDelay = speedDelays[speedLevel - 1];

  // 渐亮
  for (int duty = 0; duty <= 255; duty++) {
    ledcWrite(LED_PIN, duty);
    delay(stepDelay);
    checkTouchRelease();   // 检测手指是否离开，更新释放状态
  }

  // 渐暗
  for (int duty = 255; duty >= 0; duty--) {
    ledcWrite(LED_PIN, duty);
    delay(stepDelay);
    checkTouchRelease();
  }
}

// 检测触摸释放：当手指离开时重置 lastTouchState，为下一次边缘检测做准备
void checkTouchRelease() {
  if (touchRead(TOUCH_PIN) >= THRESHOLD) {
    lastTouchState = false;
  }
}

// 触摸中断服务函数
void IRAM_ATTR gotTouch() {
  unsigned long now = millis();
  // 软件防抖
  if (now - lastDebounceTime < debounceDelay) return;

  // 再次确认触摸值仍然低于阈值
  if (touchRead(TOUCH_PIN) < THRESHOLD) {
    // 边缘检测：从未触摸到触摸的瞬间
    if (!lastTouchState) {
      // 档位循环：1→2→3→1
      speedLevel = speedLevel % 3 + 1;
      levelChanged = true;               // 通知主循环
    }
    lastTouchState = true;               // 标记为已触摸
  }
  lastDebounceTime = now;
}