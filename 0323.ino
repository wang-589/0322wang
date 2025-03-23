#include <Servo.h>

// 定義針腳
const int ledPins[] = {2, 3, 4, 5, 6};  // 5個 LED 的針腳
const int buzzerPin = 8;                 // 蜂鳴器針腳
const int potPin = A0;                   // 可變電阻針腳
const int servoPin = 11;                 // 伺服馬達針腳

// 定義 LED 延遲時間範圍
const int minDelay = 200;    // 最快閃爍速度
const int maxDelay = 1600;   // 最慢閃爍速度

// 定義伺服馬達控制參數
const int SERVO_MIN_ANGLE = 0;    // 最小角度
const int SERVO_MAX_ANGLE = 180;  // 最大角度
const int SERVO_START_ANGLE = 0;  // 起始角度

// 定義救護車警報旋律（相對頻率）
const int melody[] = {
  1200,  // 高音
  600,   // 低音
  1200,  // 高音
  600,   // 低音
  1200,  // 高音
  600    // 低音
};

// 定義每個音符的持續時間（毫秒）
const int noteDurations[] = {
  300,  // 高音持續時間
  300,  // 低音持續時間
  300,  // 高音持續時間
  300,  // 低音持續時間
  300,  // 高音持續時間
  300   // 低音持續時間
};

// 伺服馬達控制變數
Servo myServo;
int currentAngle = 0;
bool isMovingForward = true;  // true: 向前轉動, false: 返回
unsigned long lastMoveTime = 0;  // 上次移動時間
int moveInterval = 20;  // 移動間隔（毫秒）

void setup() {
  // 初始化 LED 針腳
  for(int i = 0; i < 5; i++) {
    pinMode(ledPins[i], OUTPUT);
  }
  
  // 初始化蜂鳴器針腳
  pinMode(buzzerPin, OUTPUT);
  
  // 初始化伺服馬達
  myServo.attach(servoPin);
  myServo.write(SERVO_START_ANGLE);
  currentAngle = SERVO_START_ANGLE;
  
  // 初始化序列通訊
  Serial.begin(9600);
}

void updateServo() {
  // 讀取可變電阻的值並映射到移動間隔（10-100毫秒）
  int potValue = analogRead(potPin);
  moveInterval = map(potValue, 0, 1023, 10, 300);  // 可變電阻控制速度
  
  // 檢查是否到達移動時間
  unsigned long currentTime = millis();
  if (currentTime - lastMoveTime >= moveInterval) {
    // 更新移動時間
    lastMoveTime = currentTime;
    
    if (isMovingForward) {
      // 直接轉動到180度
      currentAngle = SERVO_MAX_ANGLE;
      myServo.write(currentAngle);
      isMovingForward = false;
    } else {
      // 直接返回0度
      currentAngle = SERVO_MIN_ANGLE;
      myServo.write(currentAngle);
      isMovingForward = true;
    }
  }
}

void loop() {
  // 持續更新伺服馬達位置
  updateServo();
  
  // 讀取可變電阻的值並映射到延遲時間
  int potValue = analogRead(potPin);
  int ledDelay = map(potValue, 0, 1023, minDelay, maxDelay);
  
  // 計算音高調整係數（0.5 到 1.5 之間）
  float pitchFactor = map(potValue, 0, 1023, 50, 150) / 100.0;
  
  // 播放調整後的旋律
  for(int i = 0; i < 6; i++) {
    // 計算調整後的頻率
    int adjustedFreq = melody[i] * pitchFactor;
    
    // 播放音符
    tone(buzzerPin, adjustedFreq);
    delay(noteDurations[i]);
    
    // LED 閃爍效果
    for(int j = 0; j < 5; j++) {
      digitalWrite(ledPins[j], HIGH);
      delay(ledDelay/10);
      digitalWrite(ledPins[j], LOW);
    }
  }
  
  // 短暫停頓
  noTone(buzzerPin);
  delay(200);
  
  // 輸出除錯資訊
  Serial.print("Pitch Factor: ");
  Serial.print(pitchFactor);
  Serial.print(" | LED Delay: ");
  Serial.print(ledDelay);
  Serial.print(" | Servo Angle: ");
  Serial.println(currentAngle);
} 