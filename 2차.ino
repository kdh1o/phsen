#include <Wire.h>
#include <LiquidCrystal_I2C.h>

// LCD 설정
LiquidCrystal_I2C lcd(0x27, 16, 2); 

// 조도센서 핀 설정
const int cmp[12] = {100, 200, 250, 300, 350, 370, 400, 430, 500, 600, 800};
const int bright[12] = {0, 25, 51, 76, 100, 120, 135, 160, 200, 255, 255, 255};  // 각 단계에 맞는 LED 밝기

int phsen[4] = {0,};  // A1, A2, A3, B 센서 값

// LED 핀 설정
const int p1[] = {9, 10, 11};  // 창가 조명 (P1)
const int p2[] = {3, 5, 6};  // 반사 조명 (P2)

// 버튼 설정
const int abtn = 2;   // 자동 조정 버튼 핀
bool Auto = true;  // 자동 조정 모드 상태
bool ledstate = false;  // 수동 모드 LED 상태

// 현재 시간 입력값 (0~23)
int hour = 12;  

void setup() {
    Serial.begin(9600);
    pinMode(abtn, INPUT_PULLUP);

    // LED 핀 출력 설정
    for (int i = 0; i < 3; i++) {
        pinMode(p1[i], OUTPUT);
        pinMode(p2[i], OUTPUT);
    }

    // LCD 초기화
    lcd.init();  // LCD초기 설정             
    lcd.backlight(); // LCD초기 설정  

    Serial.println("시간을 입력(0~23): ");
}

void loop() {
    if (Serial.available()) {
        int inputHour = Serial.parseInt();
        if (inputHour >= 0 && inputHour < 24) {
            hour = inputHour;
            Serial.print("입력된 시간: ");
            Serial.println(hour);
        }
    }

    // 태양에 따른 밝기 가중치 설정
    float weightA1, weightA2, weightA3;

    if (hour >= 6 && hour < 12){ // 오전 A1 비중 
        weightA1 = 0.5; weightA2 = 0.3; weightA3 = 0.2;
    }
    else if (hour >= 12 && hour < 15){   // 정오 A2 중심
        weightA1 = 0.3; weightA2 = 0.5; weightA3 = 0.2;
    }
    else if (hour >= 15 && hour < 18){
      // 오후 A3 중심
      weightA1 = 0.2; weightA2 = 0.3; weightA3 = 0.5;
    }
        
    else{// 야간: 비슷비슷
        weightA1 = 0.33; weightA2 = 0.33; weightA3 = 0.34;
    }

    // 버튼 상태
    bool autostate = digitalRead(abtn);

    // 자동 조정 버튼 눌림 상태 확인
    static bool lastauto = HIGH;
    if (autostate == LOW && lastauto == HIGH) {
        Auto = !Auto;  // 자동 조정 모드 전환
        delay(5);
    }
    lastauto = autostate;

    if (Auto) {
        // 조도센서 값 읽기
        phsen[0] = analogRead(A0);
        phsen[1] = analogRead(A1);
        phsen[2] = analogRead(A2);
        phsen[3] = analogRead(A3);  // 태양빛 반사 조도센서 (b)

        // 가중 평균 적용
        int weightA = (phsen[0] * weightA1) + (phsen[1] * weightA2) + (phsen[2] * weightA3);
        int B = phsen[3];

        Serial.print("Weighted A: ");
        Serial.println(weightA);
        Serial.print("B: ");
        Serial.println(B);

        // LED 밝기 조절 (창가)
        int brightness1 = 0;
        for (int i = 0; i < 12; i++) {
            if (weightA <= cmp[i]) {
                brightness1 = bright[i];
            }
        }

        // LED 밝기 조절 (햇빛)
        int brightness2 = brightness1 - B;

        // LED 출력
        for (int i = 0; i < 3; i++) {
            analogWrite(p1[i], brightness1);
            analogWrite(p2[i], brightness2);
        }

        //LCD설정
        lcd.setCursor(0, 0);
        lcd.print("Time: ");
        lcd.print(hour);  // 설정된 시간 표시
        lcd.print("h   "); 

        lcd.setCursor(0, 1);
        lcd.print("W:");
        lcd.print(weightA1, 2);  // A1 가중치
        lcd.print("/");
        lcd.print(weightA2, 2);  // A2 가중치
        lcd.print("/");
        lcd.print(weightA3, 2);  // A3 가중치
        lcd.print("   ");
    } 
    else {  // 수동 모드 전환
        int d = ledstate ? 255 : 0;
        for (int i = 0; i < 3; i++) {
            analogWrite(p1[i], d);
            analogWrite(p2[i], d);
        }
    }

    delay(10000);
}
