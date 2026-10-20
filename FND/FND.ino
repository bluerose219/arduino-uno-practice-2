// 실습 3: 버튼을 누를 때마다 한 자리 7세그먼트의 숫자 증가
// 공통 캐소드(COM = GND), A~G 각각에 330Ω 저항을 사용합니다.
const byte SEGMENT_PINS[] = {2, 3, 4, 5, 6, 7, 8};  // A, B, C, D, E, F, G
const byte BUTTON_PIN = 10;
const byte LED_PIN = 11;
const byte DIGITS[] = {
  0x3F, 0x06, 0x5B, 0x4F, 0x66,
  0x6D, 0x7D, 0x07, 0x7F, 0x6F
};
byte number = 0;
int lastReading = HIGH;
int stableButton = HIGH;
unsigned long changedAt = 0;

void showNumber() {
  for (byte segment = 0; segment < 7; segment++) {
    digitalWrite(SEGMENT_PINS[segment], bitRead(DIGITS[number], segment));
  }
  digitalWrite(LED_PIN, number == 0 ? HIGH : LOW);
  Serial.print(F("Digit:")); Serial.println(number);
}

void setup() {
  Serial.begin(9600);
  for (byte pin : SEGMENT_PINS) pinMode(pin, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(LED_PIN, OUTPUT);
  Serial.println(F("Counter ready: press button once per number"));
  showNumber();
}

void loop() {
  const int reading = digitalRead(BUTTON_PIN);
  const unsigned long now = millis();
  if (reading != lastReading) changedAt = now;
  lastReading = reading;
  if (now - changedAt >= 35 && reading != stableButton) {
    stableButton = reading;
    if (stableButton == LOW) {
      number = (number + 1) % 10;
      showNumber();
    }
  }
}
