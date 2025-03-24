const int cmp[12] = {100, 200, 250, 300, 350, 370, 400, 430, 500, 600, 800};
const int bright[12] = {0, 25, 51, 76, 100, 120, 135, 160, 200, 255, 255, 255};  // 각 단계에 맞는 LED 밝기

int phsen[6]={0,};

const int abtn = 2;   // 자동 조정 버튼 핀
bool Auto = true;  // 자동 조정 모드 상태
bool ledstate = false;  // 수동 모드 LED 상태

void setup() {
  Serial.begin(9600);
  pinMode(abtn, INPUT_PULLUP);

  // LED 핀 출력 설정
  pinMode(9, OUTPUT);
  pinMode(10, OUTPUT);
  pinMode(11, OUTPUT);
  pinMode(3, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
}

void loop() {
  // 버튼 상태 읽기
  bool autostate = digitalRead(abtn);

  // 자동 조정 버튼 눌림 상태 확인
  static bool lastauto = HIGH;
  if (autostate==LOW&&lastauto==HIGH)
  {
    Auto=!Auto;  // 자동 조정 모드 전환
    delay(5);
  }
  lastauto=autostate;

  // 각 센서 값 읽기

  if(Auto==1)
  {
    phsen[0] = analogRead(A0);
    phsen[1] = analogRead(A1);
    phsen[2] = analogRead(A2);

    phsen[3] = analogRead(A3);
    phsen[4] = analogRead(A4);
    phsen[5] = analogRead(A5);

    //(a1, a2, a3) 평균 계산
    int avg1=0;
    for(int i=0;i<3;i++)
      avg1+=phsen[i];

    avg1/=3;
    Serial.print("avg1: ");
    Serial.println(avg1);

    //(b1, b2, b3) 평균 계산
    int avg2=0;
    for(int i=3;i<6;i++)
      avg2+=phsen[i];

    avg2/=3;
    Serial.print("avg2: ");
    Serial.println(avg2);

    // LED 밝기 조절 avg1
    int brightness1=0;
    for(int i=0;i<12;i++)
    {
      if (avg1<=cmp[i])
      {
        brightness1=bright[i];
        break;
      }
    }

    analogWrite(9, brightness1);
    analogWrite(10, brightness1);
    analogWrite(11, brightness1);

    // LED 밝기 조절 avg2
    int brightness2=0;
    for(int i=0;i<12;i++)
    {
      if (avg2<=cmp[i])
      {
        brightness2=bright[i];
        break;
      }
    }

    analogWrite(3, brightness2);
    analogWrite(5, brightness2);
    analogWrite(6, brightness2);
  }
  else
  {
    int dl=ledstate?255:0;
    analogWrite(9, dl);
    analogWrite(10, dl);
    analogWrite(11, dl);
    analogWrite(3, dl);
    analogWrite(5, dl);
    analogWrite(6, dl);
  }

  delay(100);  // 0.1초 대기

}
