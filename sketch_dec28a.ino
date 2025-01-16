#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// Definovanie rozmerov displeja
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64

// Vytvorenie objektu pre displej
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// Definovanie pinov pre senzory
#define MQ135_PIN 35  // Pin pre MQ135 (CO2, NH3, NOx, SnO2)
#define MQ7_PIN 34    // Pin pre MQ7 (CO)
#define DUST_LED_PIN 5 // Pin pre LED indikátor GP2Y1014AU0F
#define DUST_ANALOG_PIN 26 // Pin pre analógový výstup GP2Y1014AU0F

// Funkcia na prevod analógového čítania na koncentráciu CO2 v ppm (MQ135)
float convertToPPM_MQ135(int analogValue) {
  float voltage = (analogValue / 4095.0) * 3.3;  // Prevod na napätie
  float ppm = voltage * 100;  // Tento výpočet je orientačný, kalibrácia je potrebná
  return ppm;
}

// Funkcia na prevod analógového čítania na koncentráciu NH3 v ppm (MQ135)
float convertToPPM_NH3(int analogValue) {
  float voltage = (analogValue / 4095.0) * 3.3;  // Prevod na napätie
  float ppm = voltage * 50;  // Kalibrujte podľa datasheetu
  return ppm;
}

// Funkcia na prevod analógového čítania na koncentráciu NOx v ppm (MQ135)
float convertToPPM_NOx(int analogValue) {
  float voltage = (analogValue / 4095.0) * 3.3;  // Prevod na napätie
  float ppm = voltage * 80;  // Kalibrujte podľa datasheetu
  return ppm;
}

// Funkcia na prevod analógového čítania na koncentráciu SnO2 v ppm (MQ135)
float convertToPPM_SnO2(int analogValue) {
  float voltage = (analogValue / 4095.0) * 3.3;  // Prevod na napätie
  float ppm = voltage * 40;  // Kalibrujte podľa datasheetu
  return ppm;
}

// Funkcia na prevod analógového čítania na koncentráciu CO v ppm (MQ7)
float convertToPPM_MQ7(int analogValue) {
  float voltage = (analogValue / 4095.0) * 3.3;  // Prevod na napätie
  float ppm = voltage * 50;  // Tento výpočet je orientačný, kalibrácia je potrebná
  return ppm;
}

// Funkcia na prevod analógového čítania na hodnotu prachu v µg/m3 (GP2Y1014AU0F)
float convertToDust(int analogValue) {
  float voltage = (analogValue / 4095.0) * 3.3;  // Prevod na napätie
  float dust = voltage * 500;  // Tento výpočet je orientačný, kalibrácia je potrebná
  return dust;
}

unsigned long previousMillis = 0; // Časovač pre blikanie displeja
const unsigned long interval = 20000; // 20 sekúnd

void setup() {
  // Inicializácia sériovej komunikácie
  Serial.begin(115200);

  // Inicializácia displeja
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("OLED initialization failed!");
    while (true);
  }

  // Inicializácia pinov
  pinMode(DUST_LED_PIN, OUTPUT);
  digitalWrite(DUST_LED_PIN, HIGH);  // Zapnutie LED pre GP2Y1014AU0F

  // Vymaž displej
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
}

void loop() {
  // Čítanie hodnoty z MQ135
  int mq135_value = analogRead(MQ135_PIN);
  float co2_ppm = convertToPPM_MQ135(mq135_value);
  float nh3_ppm = convertToPPM_NH3(mq135_value);
  float nox_ppm = convertToPPM_NOx(mq135_value);
  float sno2_ppm = convertToPPM_SnO2(mq135_value);

  // Čítanie hodnoty z MQ7 (CO)
  int mq7_value = analogRead(MQ7_PIN);
  float co_ppm = convertToPPM_MQ7(mq7_value);

  // Čítanie hodnoty prachu z GP2Y1014AU0F
  int dust_value = analogRead(DUST_ANALOG_PIN);
  float dust_ug_m3 = convertToDust(dust_value);

  // Časovač pre blikanie displeja
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;

    // Displej sa vypne na 1 sekundu
    display.clearDisplay();
    display.display();
    delay(1000);

    // Displej zobrazí bielu obrazovku na 3 sekundy
    display.clearDisplay();
    display.fillScreen(SSD1306_WHITE);
    display.display();
    delay(3000);
  }

  // Zobrazenie hodnôt na displeji
  display.clearDisplay();
  display.setCursor(0, 0);
  display.print("CO2: ");
  display.print(co2_ppm);
  display.println(" ppm");

  display.setCursor(0, 10);
  display.print("NH3: ");
  display.print(nh3_ppm);
  display.println(" ppm");

  display.setCursor(0, 20);
  display.print("NOx: ");
  display.print(nox_ppm);
  display.println(" ppm");

  display.setCursor(0, 30);
  display.print("SnO2: ");
  display.print(sno2_ppm);
  display.println(" ppm");

  display.setCursor(0, 40);
  display.print("CO: ");
  display.print(co_ppm);
  display.println(" ppm");

  display.setCursor(0, 50);
  display.print("Dust: ");
  display.print(dust_ug_m3);
  display.println(" ug/m3");

  // Aktualizácia displeja
  display.display();

  // Pauza medzi čítaniami
  delay(1000);  // Čítať každú sekundu
}
