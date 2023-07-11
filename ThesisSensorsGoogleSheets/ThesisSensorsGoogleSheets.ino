#include "ClosedCube_HDC1080.h"
#include <OneWire.h>
#include <DallasTemperature.h>
#include <WiFi.h>
#include <Adafruit_BMP280.h> // Address at 0x76
#include <Adafruit_ADS1X15.h>
#include <HTTPClient.h>


float WINDSPEED_VAL = 0.0;
float AMBIENTTEMP_VAL = 25.0;
float HUMIDITY_VAL = 50.0;
float WATERTEMP_VAL = 25.0;
float ATMOSPHERICPRESSURE_VAL = 1000.0;
float EC_VAL = 1200.0;
float PH_VAL = 6.5;


// WiFi credentials
const char* ssid = "";
const char* password = "";
const char* host = "script.google.com";
String GOOGLE_SCRIPT_ID = "";
                                                            /*Here No_of_Parameters decides how many parameters you want to send it to Google Sheets at once, change it according to your needs*/

#define WindSpeedPin                      16
#define WaterTempPin                      18



int COUNT = 0;
unsigned long LASTDEBOUNCETIME = 0;
unsigned long DEBOUNCEDELAY = 1000;


Adafruit_ADS1115 ads;
OneWire oneWire(WaterTempPin);
DallasTemperature sensors(&oneWire);
ClosedCube_HDC1080 hdc1080;
Adafruit_BMP280 bmp;
// HTTPClient https;
WiFiClientSecure client;




void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  connectWifi();
  // Google_Sheets_Init(column_name_in_sheets, Sheets_GAS_ID, No_of_Parameters );  


  ads.setGain(GAIN_ONE);
  ads.begin();
  Wire.begin();
  bmp.begin();
  sensors.begin();
  hdc1080.begin(0x40);

  pinMode(WindSpeedPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(WindSpeedPin), onChange, FALLING);
  
    Serial.print("EC: ");
    Serial.print('\t');
    Serial.print("PH: ");
    Serial.print('\t');
    Serial.print("AMBIENT TEMP: ");
    Serial.print('\t');
    Serial.print("HUMIDITY: ");
    Serial.print('\t');
    Serial.print("WATER TEMP: ");
    Serial.print('\t');
    Serial.print("WIND SPEED: ");
    Serial.print('\t');
    Serial.print("ATMOSPHERIC PRESSURE: ");
    Serial.println('\t');
}



void loop() {
  // put your main code here, to run repeatedly:
    unsigned long clock5Cycle = millis() / 1000 % 3600 % 60 % 5;
    EC_VAL = getEC();
    PH_VAL = getPH();x
    WINDSPEED_VAL = getWindSpeed();
    AMBIENTTEMP_VAL = getAmbientTemperature();
    HUMIDITY_VAL = getHumidity();
    WATERTEMP_VAL = getWaterTemperature();
    ATMOSPHERICPRESSURE_VAL = getAtmosphericPressure();


//    Serial.print("EC: ");
//    Serial.println(EC_VAL);
//    Serial.print('\t');
//    Serial.print("PH: ");
//    Serial.println(PH_VAL);
//    Serial.print("AMBIENT TEMP: ");
//    Serial.println(AMBIENTTEMP_VAL);
//    Serial.print("HUMIDITY: ");
//    Serial.println(HUMIDITY_VAL);
//    Serial.print("WATER TEMP: ");
//    Serial.println(WATERTEMP_VAL);
//    Serial.print("WIND SPEED: ");
//    Serial.println(WINDSPEED_VAL);
//    Serial.print("ATMOSPHERIC PRESSURE: ");
//    Serial.println(ATMOSPHERICPRESSURE_VAL);
//    Serial.println("-------------------");
//    delay(2000);

//    Serial.print("EC: ");
    Serial.print(EC_VAL);
    Serial.print('\t');
//    Serial.print("PH: ");
    Serial.print(PH_VAL);
    Serial.print('\t');
//    Serial.print("AMBIENT TEMP: ");
    Serial.print(AMBIENTTEMP_VAL);
    Serial.print('\t');
//    Serial.print("HUMIDITY: ");
    Serial.print(HUMIDITY_VAL);
    Serial.print('\t');
//    Serial.print("WATER TEMP: ");
    Serial.print(WATERTEMP_VAL);
    Serial.print('\t');
//    Serial.print("WIND SPEED: ");
    Serial.print(WINDSPEED_VAL);
    Serial.print('\t');
//    Serial.print("ATMOSPHERIC PRESSURE: ");
    Serial.print(ATMOSPHERICPRESSURE_VAL);
    Serial.println('\t');
//    Serial.print("-------------------");
    delay(2000);


  static unsigned long lastTick = 0;
  if (millis() - lastTick >= 15000) {
    lastTick = millis();
    sendGoogleSheets();
  }

}



void sendGoogleSheets() {
    String urlFinal = "https://script.google.com/macros/s/"+GOOGLE_SCRIPT_ID+"/exec?"+"&ec=" + EC_VAL + "&ph=" + PH_VAL + "&windSpeed=" + WINDSPEED_VAL + "&ambientTemp=" + AMBIENTTEMP_VAL + "&waterTemp=" + WATERTEMP_VAL + "&hum=" + HUMIDITY_VAL + "&atmosPressure=" + ATMOSPHERICPRESSURE_VAL;
    
    Serial.print("POST data to spreadsheet:");
    Serial.println(urlFinal);

    HTTPClient http;

    http.begin(urlFinal.c_str());
    http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);

    int httpCode = http.GET(); 

    Serial.print("HTTP Status Code: ");
    Serial.println(httpCode);

    //---------------------------------------------------------------------
    //getting response from google sheet
      String payload;
      if (httpCode > 0 && httpCode < 300) {
          payload = http.getString();
          Serial.println("Payload: "+payload);    
      }
      else {
        ESP.restart();
      }
    //---------------------------------------------------------------------
    http.end();
}


// Get EC value from sensor
float getEC() {
  float ecVoltage = ads.readADC_SingleEnded(0); // read the voltage
  float ec = (0.09552969993876301*ecVoltage) + 0.7599510104103047;
  return ec;
}


float getPH() {
  float phVoltage = ads.readADC_SingleEnded(1);
  float ph;
  if (phVoltage <= 9450) {
    ph = (0.0015745856353591163*phVoltage) - 8.01983425414365;
  }
  else if (phVoltage > 9450) {
    ph = (0.0016571428571428568*phVoltage) - 8.799999999999997;
  }
  return ph;
}

float getAmbientTemperature() {
  float ambientTemp = hdc1080.readTemperature();
  return ambientTemp;
}


float getHumidity() {
  float hum = hdc1080.readHumidity();
  return hum;
}


float getWaterTemperature() {
  sensors.requestTemperatures();
  float celsius = sensors.getTempCByIndex(0);
  return celsius;
}


float getWindSpeed() {
  float windSpeed;
  if ((millis() - LASTDEBOUNCETIME) > DEBOUNCEDELAY) {
    LASTDEBOUNCETIME = millis();
    windSpeed = ((COUNT * 8.75) / 100);
    COUNT = 0;
  }

  return windSpeed;
}


void onChange() {
  if (digitalRead(WindSpeedPin) == LOW) {
    COUNT++;
  }
}


float getAtmosphericPressure() {
  float atmosphericPressure = bmp.readPressure();
  return atmosphericPressure / 100;
}


// Connect to WiFi
void connectWifi(){
  WiFi.begin(ssid, password);
  int attempts = 0;
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(300);
    attempts++;
    if(attempts > 7) {
      ESP.restart();
    }
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();
}
