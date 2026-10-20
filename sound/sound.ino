const byte SOUND_AO = A0;
const byte SOUND_DO = 2;
const byte LED_PIN = 9;
const byte ACTIVE_LEVEL = LOW;  // 실제 보드가 반대라면 HIGH로 변경
const unsigned long COOLDOWN_MS = 250;
bool ledOn = false;
bool wasActive = false;
unsigned long lastEvent = 0;
unsigned long eventCount = 0;
unsigned long windowAt = 0;
unsigned long lastReport = 0;
int minimumAdc = 1023;
int maximumAdc = 0;
int peakToPeak = 0;

void setup() {
  Serial.begin(9600);
  pinMode(SOUND_DO, INPUT_PULLUP);
  pinMode(LED_PIN, OUTPUT);
  lastEvent = millis() - COOLDOWN_MS;
  windowAt = millis();
  Serial.println(F("Sound ready: raise level above threshold, then lower it"));
}

void loop() {
  const unsigned long now = millis();
  const int raw = analogRead(SOUND_AO);
  minimumAdc = min(minimumAdc, raw);
  maximumAdc = max(maximumAdc, raw);

  const bool active = digitalRead(SOUND_DO) == ACTIVE_LEVEL;
  if (active && !wasActive && now - lastEvent >= COOLDOWN_MS) {
    lastEvent = now;
    eventCount++;
    ledOn = !ledOn;
    digitalWrite(LED_PIN, ledOn ? HIGH : LOW);
  }
  wasActive = active;

  // 마이크 AO는 파형이므로 20ms 구간의 최대-최소 차이도 함께 관찰합니다.
  if (now - windowAt >= 20) {
    peakToPeak = maximumAdc - minimumAdc;
    minimumAdc = 1023;
    maximumAdc = 0;
    windowAt = now;
  }
  if (now - lastReport >= 100) {
    lastReport = now;
    Serial.print(F("AO:")); Serial.print(raw);
    Serial.print(F("Peak:")); Serial.print(peakToPeak);
    Serial.print(F(" DO:")); Serial.print(active ? 1 : 0);
    Serial.print(F(" LED:")); Serial.print(ledOn ? 1 : 0);
    Serial.print(F(" Count:")); Serial.println(eventCount);
  }
}
