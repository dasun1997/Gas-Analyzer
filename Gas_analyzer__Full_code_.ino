#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "DHT.h"

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define DHTPIN 13
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

const int mq4 = A0;  // CH4 gas sensor (MQ-4)
const int mq135 = A1;  // CO2 gas sensor (MQ-135)
const int mq7 = A2;  // CO gas sensor (MQ-7)
const int mq8 = A3;  // H2 gas sensor (MQ-8)

// Calibration values for MQ sensors
const double m_mq4 = -0.6527;
const double b_mq4 = 1.30;
const double m_mq135 = 0.602;
const double b_mq135 = 0.99;
const double m_mq7 = -0.75;
const double b_mq7 = 1.45;
const double m_mq8 = -0.86;
const double b_mq8 = 1.67;

double analogToPPM(int sensorPin, double m, double b)
{
  int sensorValue = analogRead(sensorPin);
  float sensor_volt = sensorValue * (5.0 / 1023.0);
  float RS_gas = ((5.0 * 10.0) / sensor_volt) - 10.0;
  float ratio = RS_gas / 21.91;  // R0 value
  double ppm_log = (log10(ratio) - b) / m;
  return ppm_log;
}

void setup()
{
  Serial.begin(9600);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.setTextColor(SSD1306_WHITE);

  // Initialize DHT sensor
  dht.begin();

  // Display initialization messages
  display.clearDisplay();
  display.setTextSize(2);
  display.setCursor(50, 0);
  display.println("Gas");
  display.setCursor(20, 20);
  display.println("Analyzer");

  display.setTextSize(1);
  display.setCursor(10, 50);
  display.print("By ");
  display.print("MBDK ");
  display.println("Siriwardena");
  display.display();
  delay(5000);

  // Loading animation
  for (int i = 0; i < 3; i++)
  {
    display.clearDisplay();
    display.setTextSize(2);
    display.setCursor(5, 25);
    display.print("Loading");
    for (int j = 0; j <= i; j++)
    {
      display.print(".");
    }
    display.display();
    delay(1000);
  }

  // Set pin modes
  pinMode(mq4, INPUT);
  pinMode(mq135, INPUT);
  pinMode(mq7, INPUT);
  pinMode(mq8, INPUT);
}

void displayData(double mq4, double mq135, double mq7, double mq8, float temperature, float humidity)
{
  display.clearDisplay();
  display.setTextSize(1);
  display.setCursor(0, 0);
  display.println("CH4 : " + String(mq4, 4) + " PPM"); // CH4 gas (MQ-4)
  display.setCursor(0, 10);
  display.println("CO2 : " + String(mq135, 4) + " PPM"); // CO2 gas (MQ-135)
  display.setCursor(0, 20);
  display.println("CO : " + String(mq7, 4) + " PPM"); // CO gas (MQ-7)
  display.setCursor(0, 30);
  display.println("H2 : " + String(mq8, 4) + " PPM"); // H2 gas (MQ-8)
  display.setCursor(0, 40);
  display.println("Temp: " + String(temperature, 1) + " C");
  display.setCursor(0, 50);
  display.println("Humidity: " + String(humidity, 1) + " %");
  display.display();
}

void loop()
{
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  float f = dht.readTemperature(true);
  
  if (isnan(h) || isnan(t) || isnan(f))
  {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

  double mq4_value = analogToPPM(mq4, m_mq4, b_mq4);  // CH4 gas (MQ-4)
  double mq135_value = analogToPPM(mq135, m_mq135, b_mq135);  // CO2 gas (MQ-135)
  double mq7_value = analogToPPM(mq7, m_mq7, b_mq7);  // CO gas (MQ-7)
  double mq8_value = analogToPPM(mq8, m_mq8, b_mq8);  // H2 gas (MQ-8)

  displayData(mq4_value, mq135_value, mq7_value, mq8_value, t, h);

  Serial.println("************************************");
  Serial.print("CH4 Value: ");
  Serial.print(mq4_value, DEC);
  Serial.println(" PPM");
  Serial.print("CO2 Value: ");
  Serial.print(mq135_value, DEC);
  Serial.println(" PPM");
  Serial.print("CO Value: ");
  Serial.print(mq7_value, DEC);
  Serial.println(" PPM");
  Serial.print("H2 Value: ");
  Serial.print(mq8_value, DEC);
  Serial.println(" PPM");
  Serial.print("DHT-Temp: ");
  Serial.print(t);
  Serial.print("°C");
  Serial.print(" Humidity: ");
  Serial.print(h);
  Serial.println("%");

  delay(800);
}
