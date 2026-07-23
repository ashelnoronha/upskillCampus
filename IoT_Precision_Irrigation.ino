#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <DHT.h>
#include <DHT_U.h>

#define SOIL_PIN 34
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define RAIN_PIN 27
#define LDR_PIN 35
#define BUZZER_PIN 26

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

#define DHTPIN 4
#define DHTTYPE DHT22
#define PUMP_PIN 25

DHT dht(DHTPIN, DHTTYPE);

bool pumpWasOn = false;

int screen = 0;
unsigned long lastScreenChange = 0;

void setup() {
  Serial.begin(115200);

  Wire.begin(21, 22);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    while (true);
  }

  dht.begin();

  pinMode(PUMP_PIN, OUTPUT);
  pinMode(RAIN_PIN, INPUT_PULLUP);
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);

  display.clearDisplay();

  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  display.setCursor(10, 20);
  display.println("PRECISION");

  display.setCursor(10, 30);
  display.println("IRRIGATION");

  display.setCursor(10, 55);
  display.println("Initializing...");

  display.display();
  delay(2000);
}

void loop() {

float temperature = dht.readTemperature();
float humidity = dht.readHumidity();
int soilValue = analogRead(SOIL_PIN);
int soilPercent = map(soilValue, 0, 4095, 100, 0);
String soilStatus;
int lightValue = analogRead(LDR_PIN);
bool isDay = (lightValue < 2000);
bool humidityOK = (humidity < 85);
bool temperatureOK = (temperature > 30);
int rainState = digitalRead(RAIN_PIN);

if (millis() - lastScreenChange > 2000)
{
    screen++;
    if (screen > 2)
        screen = 0;

    lastScreenChange = millis();
}


if (soilPercent <= 30) {
  soilStatus = "DRY";
}
else if (soilPercent <= 70) {
  soilStatus = "MOIST";
}
else {
  soilStatus = "WET";
}

//Pump logic
bool pumpOn =
    (soilStatus == "DRY") && (rainState == HIGH) && 
    isDay && humidityOK && temperatureOK;

if (pumpOn)
{
    digitalWrite(PUMP_PIN, HIGH);

    // Beep only once when pump starts
    if (!pumpWasOn)
    {
        tone(BUZZER_PIN, 1000);
        delay(2000);      // Beep for 5 seconds
        noTone(BUZZER_PIN);
    }
}
else
{
    digitalWrite(PUMP_PIN, LOW);
}

pumpWasOn = pumpOn;

display.clearDisplay();
display.setTextSize(1);
display.setTextColor(SSD1306_WHITE);

if (screen == 0)
{
    display.setCursor(0,0);
    display.println("PRECISION IRRIGATION");

    display.setCursor(0,10);
    display.print("Temp : ");
    display.print(temperature,1);
    display.println(" C");

    display.setCursor(0,20);
    display.print("Hum  : ");
    display.print(humidity,0);
    display.println("%");

    display.setCursor(0,30);
    display.print("Light: ");
    display.println(isDay ? "DAY" : "NIGHT");

    display.setCursor(110,56);
    display.print("1/3");
}

else if (screen == 1)
{
    display.setCursor(0,0);
    display.println("SYSTEM STATUS");

    display.setCursor(0,10);
    display.print("Soil : ");
    display.print(soilPercent);
    display.print("% ");
    display.println(soilStatus);

    display.setCursor(0,20);
    display.print("Rain : ");
    display.println(rainState == HIGH ? "NO" : "YES");

    display.setCursor(0,30);
    display.print("Pump : ");
    display.println(pumpOn ? "ON" : "OFF");

    display.setCursor(110,56);
    display.print("2/3");
}

else
{
    display.setCursor(0,0);
    display.println("PUMP CONDITIONS");

    display.print("Soil : ");
    display.println(soilStatus);

    display.print("Rain : ");
    display.println(rainState == HIGH ? "NO" : "YES");

    display.print("Temp : ");
    display.println(temperatureOK ? "OK" : "LOW");

    display.print("Hum  : ");
    display.println(humidityOK ? "OK" : "HIGH");

    display.print("Light: ");
    display.println(isDay ? "DAY" : "NIGHT");

    display.setCursor(110,56);
    display.print("3/3");
}

display.display();

}