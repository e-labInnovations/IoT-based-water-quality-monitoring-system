#include <Wire.h>
#include <DallasTemperature.h>
#include <LiquidCrystal.h>
#define DS18B20 2

#define phPin A1                                                  // the pH meter Analog output is connected with the Arduino’s Analog
#define turbidityPin A0                                       // the turbidity meter Analog output is connected with the Arduino’s Analog
float tempValue, ntu;
int turbMax = 740;                                               // Turbidity max.

String sendData;                                              //For sending measured values to ESP8266 through serial communication

unsigned long int avgValue;                                   //Store the average value of the sensor feedback
float b;
int buf[10], temp;

const int rs = 13, en = 12, d4 = 8, d5 = 9, d6 = 10, d7 = 11;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
OneWire ourWire(DS18B20);
DallasTemperature sensor(&ourWire);
byte degree_symbol[8] = { 0x07, 0x05, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00 };

void setup() {
  Serial.begin(9600);
  sensor.begin();
  lcd.begin(16, 2);              // initialize the lcd
  lcd.createChar(0, degree_symbol);

  lcd.begin(16, 2);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("IoT BASED WATER");
  lcd.setCursor(0, 1);
  lcd.print("    QUALITY");
  delay(2000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("   MONITORING");
  lcd.setCursor(0, 1);
  lcd.print("     SYSTEM");
  delay(2000);
}

void loop() {
  // ######################################################## pH MEASUREMENT ########################################################
  for (int i = 0; i < 10; i++) {                              //Get 10 sample value from the sensor for smooth the value
    buf[i] = analogRead(phPin);
    delay(10);
  }
  for (int i = 0; i < 9; i++) {                             //sort the analog from small to large
    for (int j = i + 1; j < 10; j++) {
      if (buf[i] > buf[j]) {
        temp = buf[i];
        buf[i] = buf[j];
        buf[j] = temp;
      }
    }
  }
  avgValue = 0;
  for (int i = 2; i < 8; i++)                             //take the average value of 6 center sample
    avgValue += buf[i];
  float phValue = (float)avgValue * 5.0 / 1024 / 6;       //convert the analog into millivolt
  phValue = 3.5 * phValue;                              //convert the millivolt into pH value

  // ######################################################## TURBIDITY MEASUREMENT ########################################################
ntu = toNTU( );


  // ######################################################## TEMPERATURE MEASUREMENT ########################################################
  sensor.requestTemperatures();
  tempValue = sensor.getTempCByIndex(0);

  //############ TEMPERATURE MONITORING ##############
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Temperature:");
  lcd.setCursor(0, 1);
  lcd.print(tempValue);
  lcd.write(char(0));
  lcd.print("C");
  delay(2000);

  //############ pH MONITORING ##############
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("pH:");
  lcd.setCursor(4, 0);
  lcd.print(phValue, 2);
  if (phValue >= 8) {
    lcd.setCursor(0, 1);
    lcd.print("Alkali");
  }
  else if (phValue <= 6) {
    lcd.setCursor(0, 1);
    lcd.print("Acidic");
  }
  else {
    lcd.setCursor(0, 1);
    lcd.print("Normal");
  }
  delay(2000);


  //############ TURBIDITY MONITORING ##############
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Turbidity:");
  lcd.setCursor(0, 1);
  lcd.print(ntu);
  lcd.print(" NTU ");
  delay(10);

  //############ SEND DATA TO ESP8266 ##############
  sendData = "field1=" + String(tempValue) + "&field2=" + ntu + "&field3=" + phValue;
  Serial.println(sendData);
  delay(2000);
}


// ##### FOR TURBIDITY MEASUREMENT #####
float toNTU( ) {
  int sensorValue = (turbMax - analogRead(turbidityPin));
//  float voltage = analogRead(A1) * (5.0 / 1024.0);

  if (sensorValue > turbMax ) {
    sensorValue = turbMax;
  }
  if (sensorValue < 0 ) {
    sensorValue = 0;
  }
  float turbValue = map(sensorValue, 0, turbMax, 0, 500);
  return turbValue;
}
