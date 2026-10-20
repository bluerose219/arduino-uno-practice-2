const byte SOUND_AO = A0;
const byte SOUND_DO = 2;
const byte SOUND_LED = 9;    // 소리가 들리는 중
const byte QUIET_LED = 11;   // 소리가 안 들림
const byte ACTIVE_LEVEL = LOW;  // 실제 보드가 반대라면 HIGH로 변경
const unsigned long HOLD_MS = 200;  // 마지막 소리 이후 이 시간 동안은 "들리는 중"으로 유지
unsigned long lastSoundAt = 0;
unsigned long lastReport = 0;

void setup() {
  Serial.begin(9600);
  pinMode(SOUND_DO, INPUT_PULLUP);
  pinMode(SOUND_LED, OUTPUT);
  pinMode(QUIET_LED, OUTPUT);
  lastSoundAt = millis() - HOLD_MS;
  Serial.println(F("Sound two LED ready: quiet -> D11, sound -> D9"));
}

void loop() {
  const unsigned long now = millis();
  const bool active = digitalRead(SOUND_DO) == ACTIVE_LEVEL;
  if (active) {
    lastSoundAt = now;
  }

  // DO는 파형에 따라 짧게 깜빡이므로 HOLD_MS 동안 유지해서 "들리는 중"으로 판단합니다.
  const bool hearing = now - lastSoundAt < HOLD_MS;
  digitalWrite(SOUND_LED, hearing ? HIGH : LOW);
  digitalWrite(QUIET_LED, hearing ? LOW : HIGH);

  if (now - lastReport >= 100) {
    lastReport = now;
    Serial.print(F("AO:")); Serial.print(analogRead(SOUND_AO));
    Serial.print(F(" DO:")); Serial.print(active ? 1 : 0);
    Serial.print(F(" State:")); Serial.println(hearing ? F("SOUND") : F("QUIET"));
  }
}
