// 10초 카운트다운 + LED 3개 + 버튼 점수 게임
// 남은 시간 10~7초: D13, 7~4초: D12, 4~0초: D11 점등
// D11 점등 중 버튼 -> +1, 그 외 -> -1 (0 미만 없음), 9점이면 3초 깜빡인 뒤 초기화
// 버튼을 1.5초 이상 꾹 누르면 LED와 세그먼트 전부 꺼짐, 꺼진 상태에서 누르면 처음부터 다시 시작
// 공통 캐소드(COM = GND), A~G 각각에 330Ω 저항을 사용합니다.
const byte SEGMENT_PINS[] = {2, 3, 4, 5, 6, 7, 8};  // A, B, C, D, E, F, G
const byte BUTTON_PIN = 10;
const byte LED_PINS[] = {13, 12, 11};  // 카운트다운 순서
const byte DIGITS[] = {
  0x3F, 0x06, 0x5B, 0x4F, 0x66,
  0x6D, 0x7D, 0x07, 0x7F, 0x6F
};
const unsigned long COUNTDOWN_MS = 10000;
const unsigned long GREEN_FROM_MS = 4000;   // 남은 시간이 4초 이하면 D11
const unsigned long YELLOW_FROM_MS = 7000;  // 남은 시간이 7초 이하면 D12
const unsigned long WIN_BLINK_MS = 3000;
const unsigned long BLINK_INTERVAL_MS = 250;
const byte WIN_SCORE = 9;
const unsigned long LONG_PRESS_MS = 1500;  // 이 시간 이상 누르면 LED 전부 끄기
const byte NO_LED = 255;  // setLeds에 넘기면 모든 LED 꺼짐

byte score = 0;
byte activeLed = 0;  // LED_PINS 인덱스
bool winning = false;
bool sleeping = false;  // 길게 눌러서 꺼진 상태
bool longHandled = false;
unsigned long pressedAt = 0;
unsigned long countdownAt = 0;
unsigned long winAt = 0;
int lastReading = HIGH;
int stableButton = HIGH;
unsigned long changedAt = 0;
unsigned long lastReport = 0;

void showScore() {
  for (byte segment = 0; segment < 7; segment++) {
    digitalWrite(SEGMENT_PINS[segment], bitRead(DIGITS[score], segment));
  }
  Serial.print(F("Score:")); Serial.println(score);
}

void setLeds(byte index) {
  for (byte i = 0; i < 3; i++) {
    digitalWrite(LED_PINS[i], i == index ? HIGH : LOW);
  }
}

void resetGame() {
  score = 0;
  winning = false;
  countdownAt = millis();
  showScore();
  Serial.println(F("Game reset: countdown 10s"));
}

void onPress() {
  if (activeLed == 2) {
    score++;
  } else if (score > 0) {
    score--;
  }
  showScore();
  if (score >= WIN_SCORE) {
    winning = true;
    winAt = millis();
    Serial.println(F("WIN! blinking 3s"));
  }
}

void setup() {
  Serial.begin(9600);
  for (byte pin : SEGMENT_PINS) pinMode(pin, OUTPUT);
  for (byte pin : LED_PINS) pinMode(pin, OUTPUT);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  resetGame();
}

void loop() {
  const unsigned long now = millis();

  // 짧게 누르면 뗄 때 점수 처리, 길게 누르면 누르고 있는 중에 LED 끄기
  const int reading = digitalRead(BUTTON_PIN);
  if (reading != lastReading) changedAt = now;
  lastReading = reading;
  bool shortPress = false;
  if (now - changedAt >= 35 && reading != stableButton) {
    stableButton = reading;
    if (stableButton == LOW) {
      pressedAt = now;
      longHandled = false;
    } else if (!longHandled) {
      shortPress = true;
    }
  }
  if (stableButton == LOW && !longHandled && now - pressedAt >= LONG_PRESS_MS) {
    longHandled = true;
    if (!sleeping && !winning) {
      sleeping = true;
      setLeds(NO_LED);
      for (byte pin : SEGMENT_PINS) digitalWrite(pin, LOW);
      Serial.println(F("Long press: LEDs and FND off, press again to restart"));
    }
  }

  if (sleeping) {
    if (shortPress) {
      sleeping = false;
      resetGame();
    }
    return;
  }

  if (winning) {
    // 3초 동안 모든 LED 깜빡임, 이후 초기화
    const bool on = ((now - winAt) / BLINK_INTERVAL_MS) % 2 == 0;
    for (byte pin : LED_PINS) digitalWrite(pin, on ? HIGH : LOW);
    if (now - winAt >= WIN_BLINK_MS) resetGame();
    return;
  }

  // 카운트다운이 끝나면 다시 10초부터 시작
  if (now - countdownAt >= COUNTDOWN_MS) countdownAt = now;
  const unsigned long remaining = COUNTDOWN_MS - (now - countdownAt);
  if (remaining <= GREEN_FROM_MS) {
    activeLed = 2;
  } else if (remaining <= YELLOW_FROM_MS) {
    activeLed = 1;
  } else {
    activeLed = 0;
  }
  setLeds(activeLed);

  if (shortPress) onPress();

  if (now - lastReport >= 1000) {
    lastReport = now;
    Serial.print(F("Remaining:")); Serial.print((remaining + 999) / 1000);
    Serial.print(F("s LED:")); Serial.println(LED_PINS[activeLed]);
  }
}
