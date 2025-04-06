#include <Adafruit_NeoPixel.h>

#define PIN_P1 6
#define PIN_P2 7
#define NUMPIXELS 8
#define BTN_PIN 8

Adafruit_NeoPixel stripP1(NUMPIXELS, PIN_P1, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel stripP2(NUMPIXELS, PIN_P2, NEO_GRB + NEO_KHZ800);

String input = "";
int brightness1 = 0;
int brightness2 = 0;
bool Auto = true;

void setup() {
  Serial.begin(9600);  // RX(0) ← UNO의 TX(1)
  pinMode(BTN_PIN, INPUT_PULLUP);

  stripP1.begin();
  stripP2.begin();
  stripP1.show();
  stripP2.show();
}

void loop() {
  // 버튼 처리
  static bool lastBtn = HIGH;
  bool btn = digitalRead(BTN_PIN);
  if (btn == LOW && lastBtn == HIGH) {
    Auto = !Auto;
    delay(5);
  }
  lastBtn = btn;

  // 데이터 수신
  while (Serial.available() > 0) {
    char c = Serial.read();
    if (c == '\n') {
      parseData(input);
      input = "";
    } else {
      input += c;
    }
  }

  // 네오픽셀 제어
  if (Auto) {
    for (int i = 0; i < NUMPIXELS; i++) {
      stripP1.setPixelColor(i, stripP1.Color(brightness1, brightness1, brightness1));
      stripP2.setPixelColor(i, stripP2.Color(brightness2, brightness2, brightness2));
    }
  }
  else {
    for (int i = 0; i < NUMPIXELS; i++) {
      stripP1.setPixelColor(i, 0);
      stripP2.setPixelColor(i, 0);
    }
  }

  stripP1.show();
  stripP2.show();
  delay(50);
}

void parseData(String data) {
  int commaIndex = data.indexOf(',');
  if (commaIndex > 0) {
    brightness1 = data.substring(0, commaIndex).toInt();

    // B_raw 값 (어두울수록 값 커짐)
    brightness2 = data.substring(commaIndex + 1).toInt();

    // b1과 b2 값 출력
    Serial.print("b1: ");
    Serial.print(brightness1);
    Serial.print(" b2: ");
    Serial.println(brightness2);
  }
}
