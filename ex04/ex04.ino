// 定义触摸引脚 (T0对应GPIO4)
#define TOUCH_PIN 4
// 定义LED引脚 (ESP32 DevKit板载LED通常是GPIO2)
#define LED_PIN 2
// 中断模式设置：1 使用中断模式
#define EXT_ISR_MODE 1

// 阈值，需要通过串口监视器观察并调整
int threshold = 400;

// LED 状态变量
volatile bool ledState = false;
// 上一次触摸状态（供边缘检测使用）
volatile bool lastTouchState = false;
// 上一次有效中断的时间戳（防抖用）
volatile unsigned long lastDebounceTime = 0;
// 软件防抖延时（毫秒）
const unsigned long debounceDelay = 150;

// 中断服务函数声明
void gotTouch();

void setup() {
  Serial.begin(115200);
  delay(1000); // 等待串口稳定

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, ledState); // 初始状态为熄灭

  // 绑定触摸中断，阈值使用 threshold 变量
  touchAttachInterrupt(TOUCH_PIN, gotTouch, threshold);

  Serial.println("触摸中断模式已启动，摸一下切换LED状态");
}

void loop() {
  // 读取当前触摸值，用于释放检测和调试
  int touchValue = touchRead(TOUCH_PIN);
  Serial.print("Touch Value: ");
  Serial.println(touchValue);

  // 当手指离开时，将 lastTouchState 标记为 false，
  // 以便下次触摸能正确识别为“按下瞬间”
  if (touchValue >= threshold) {
    lastTouchState = false;
  }

  delay(100); // 维持原代码的读取周期
}

// 中断服务函数 (ISR) —— 仅在触摸值穿越阈值时被调用
void gotTouch() {
  unsigned long now = millis();

  // 软件防抖：如果距离上次有效中断时间太短，忽略本次中断
  if (now - lastDebounceTime < debounceDelay) {
    return;
  }

  // 再次确认当前触摸值确实低于阈值（避免误触发）
  if (touchRead(TOUCH_PIN) < threshold) {
    // 边缘检测：只有上一次未触摸，本次才视为“按下瞬间”
    if (!lastTouchState) {
      ledState = !ledState;               // 翻转 LED 状态
      digitalWrite(LED_PIN, ledState);    // 更新硬件引脚
    }
    // 标记当前已触摸，防止按住期间重复触发翻转
    lastTouchState = true;
  }

  // 更新防抖时间戳
  lastDebounceTime = now;
}