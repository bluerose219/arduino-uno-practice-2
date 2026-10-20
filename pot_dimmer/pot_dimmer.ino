const byte POT_PIN = A0;
const byte LED_PIN = 9;  // Uno에서 ~ 표시가 있는 PWM 핀
unsigned long lastReport = 0;

void setup() {
  Serial.begin(9600);
  pinMode(LED_PIN, OUTPUT);
  Serial.println(F("Potentiometer ready: ADC 0..1023 -> PWM 0..255"));
}

void loop() {
  const int raw = analogRead(POT_PIN);
  const int pwm = map(raw, 0, 1023, 0, 255);
  analogWrite(LED_PIN, pwm);

  const unsigned long now = millis();
  if (now - lastReport >= 100) {
    lastReport = now;
    Serial.print(F("ADC:")); Serial.print(raw);
    Serial.print(F(" PWM:")); Serial.print(pwm);
    Serial.print(F(" Voltage:")); Serial.println(raw * (5.0 / 1023.0), 2);
  }
}