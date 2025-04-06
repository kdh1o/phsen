#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

int phsen[4] = {0,};  // A0 ~ A3
const int abtn = 7;   // 자동/수동 버튼
bool Auto = true;
int hour = 12;
String sss = "";

int prevA = -1;
int prevB = -1;

const int cmp[6] =    {100, 200, 300, 350, 400, 500};
const int cmp2[6] = {0, 33, 66, 99, 132, 200};
const int bright[6] = {0, 10, 30, 60, 90, 120};

void setup() {
  Serial.begin(9600);  //Serial이 TX=1 사용
  lcd.init();
  lcd.backlight();
  pinMode(abtn, INPUT_PULLUP);
}

void loop() {
  // 시간 입력
  while (Serial.available() > 0) {
    char ch = Serial.read();
    if (ch == '\n' || ch == '\r') {
      if (sss.length() > 0) {
        int h = sss.toInt();
        if (h >= 0 && h < 24) hour = h;
        sss = "";
      }
    } else {
      sss += ch;
    }
  }

  // 가중치
  float w1, w2, w3;
  if (hour >= 8 && hour < 11) {
    w1 = 0.6; w2 = 0.3; w3 = 0.1;
  }
  else if (hour >= 11 && hour < 14) {
    w1 = 0.3; w2 = 0.5; w3 = 0.2;
  }
  else if (hour >= 14 && hour < 16) {
    w1 = 0.2; w2 = 0.5; w3 = 0.3;
  }
  else {
    w1 = 0.33; w2 = 0.33; w3 = 0.34;
  }

  phsen[0] = analogRead(A0);  // A1
  phsen[1] = analogRead(A1);  // A2
  phsen[2] = analogRead(A2);  // A3
  phsen[3] = analogRead(A3);  // 반사 조도 (B)

  int A = phsen[0] * w1 + phsen[1] * w2 + phsen[2] * w3;
  int B = map(phsen[3], 0, 1023, 0, 128);  // 줄임!

// 값을 0~255 범위로 제한 (이미 map에서 처리되지만 안전장치로 추가)
  B = map(phsen[3], 0, 1023, 0, 255);  // 조도센서 값 매핑

  Serial.print("DBG A: ");
  Serial.print(A);
  Serial.print(" B: ");
  Serial.println(B);
    
  static int b1 = 0;
  if (prevA == -1 || abs(A - prevA) > 40) {
    prevA = A;

    for (int i = 0; i < 6; i++) {
      if (A <= cmp[i]) {
        b1 = bright[i];
        break;
      }
    }
  }
  
  static int b2 = 0;

  for (int i = 0; i < 6; i++) {
    if (B <= cmp2[i]) {
      b2 = bright[i];
      break;
    }
  }

  // Serial 통해 나노에 전송
  Serial.print(b1);
  Serial.print(",");
  Serial.println(b2);  // (\n 전송됨)

  // LCD 표시
  lcd.setCursor(0, 0);
  lcd.print("Time: ");
  if (hour < 10) lcd.print("0");
  lcd.print(hour);
  lcd.print("h   ");
  
  lcd.setCursor(0, 1);
  lcd.print("W: ");
  lcd.print(w1,1);
  lcd.print("  ");
  lcd.print(w2,1);
  lcd.print("  ");
  lcd.print(w3,1);
  lcd.print(""); 

  delay(100);
}
