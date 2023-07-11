#define requestButton 16
#define directionPin 18  // Change direction
#define stepPin 19
#define stepsPerRev 800
#define inductiveSensor 34
#define relayCam1 25  // Cameras
#define relayCam2 26  // Cameras
#define relayTubeLight 32
//----------------------------------------Load libraries
#include <esp_now.h>
#include <WiFi.h>
#include "time.h"
#include "esp_wifi.h"
#include <HTTPClient.h>
#include "Wire.h"
#include "BH1750.h"
//----------------------------------------

// WiFi credentials
const char *ssid = "";
const char *password = "";
const char *host = "script.google.com";
String GOOGLE_SCRIPT_ID = "";


// Time
const char *ntpServer = "hk.pool.ntp.org";
const long gmtOffset_sec = 3600 * 7;
const int daylightOffset_sec = 3600;
int YEAR, MONTH, DAY, HOUR, MINUTE, SECOND;



bool cam1_done = false;
bool cam2_done = false;
bool cam3_done = false;
bool cam4_done = false;
bool cam5_done = false;
bool cam6_done = false;
bool cam7_done = false;
bool cam8_done = false;
bool cam9_done = false;
bool cam10_done = false;
bool cam11_done = false;
bool cam12_done = false;
bool isComplete = false;
bool home = false;
bool end = false;

int step = 1;
float light_level_a;
float light_level_b;



uint8_t broadcastAddress1[] = {  };   //--> REPLACE WITH THE MAC Address of your receiver.
uint8_t broadcastAddress2[] = {  };   //--> REPLACE WITH THE MAC Address of your receiver.
uint8_t broadcastAddress3[] = {  };   //--> REPLACE WITH THE MAC Address of your receiver.
uint8_t broadcastAddress4[] = {  };   //--> REPLACE WITH THE MAC Address of your receiver.
uint8_t broadcastAddress5[] = {  };   //--> REPLACE WITH THE MAC Address of your receiver.
uint8_t broadcastAddress6[] = {  };   //--> REPLACE WITH THE MAC Address of your receiver.
uint8_t broadcastAddress7[] = {  };   //--> REPLACE WITH THE MAC Address of your receiver.
uint8_t broadcastAddress8[] = {  };   //--> REPLACE WITH THE MAC Address of your receiver.
uint8_t broadcastAddress9[] = {  };   //--> REPLACE WITH THE MAC Address of your receiver.
uint8_t broadcastAddress10[] = {  };  //--> REPLACE WITH THE MAC Address of your receiver.
uint8_t broadcastAddress11[] = {  };  //--> REPLACE WITH THE MAC Address of your receiver.
uint8_t broadcastAddress12[] = {  };  //--> REPLACE WITH THE MAC Address of your receiver.

//----------------------------------------Variables to accommodate the data to be sent.
bool send_rnd_val_1;
//----------------------------------------

//----------------------------------------Define variables to store incoming readings
int receive_rnd_val_1;
//----------------------------------------
esp_now_peer_info_t peerInfo1;
esp_now_peer_info_t peerInfo2;
esp_now_peer_info_t peerInfo3;
esp_now_peer_info_t peerInfo4;
esp_now_peer_info_t peerInfo5;
esp_now_peer_info_t peerInfo6;
esp_now_peer_info_t peerInfo7;
esp_now_peer_info_t peerInfo8;
esp_now_peer_info_t peerInfo9;
esp_now_peer_info_t peerInfo10;
esp_now_peer_info_t peerInfo11;
esp_now_peer_info_t peerInfo12;
String success;  //--> Variable to store if sending data was successful
BH1750 bh1750_a;
BH1750 bh1750_b;
WiFiClientSecure client;



//----------------------------------------Structure example to send data
// Must match with ESP32 paired.
typedef struct struct_message {
  bool rnd_1;
  int rnd_2;
} struct_message;


struct_message send_Data;  // Create a struct_message to send data.

struct_message receive_Data;  // Create a struct_message to receive data.
//----------------------------------------

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ Callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
  if (status == 0) {
    success = "Delivery Success :)";
  } else {
    success = "Delivery Fail :(";
  }
  Serial.println(">>>>>");
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ Callback when data is received


void OnDataRecv(const uint8_t *mac, const uint8_t *incomingData, int len) {
  memcpy(&receive_Data, incomingData, sizeof(receive_Data));
  Serial.println();
  Serial.println("<<<<< Receive Data:");
  Serial.print("Bytes received: ");
  Serial.println(len);
  receive_rnd_val_1 = receive_Data.rnd_2;
  if (receive_rnd_val_1 == 1) {
    cam1_done = true;
  }
  if (receive_rnd_val_1 == 2) {
    cam2_done = true;
  }
  if (receive_rnd_val_1 == 3) {
    cam3_done = true;
  }
  if (receive_rnd_val_1 == 4) {
    cam4_done = true;
  }
  if (receive_rnd_val_1 == 5) {
    cam5_done = true;
  }
  if (receive_rnd_val_1 == 6) {
    cam6_done = true;
  }
  if (receive_rnd_val_1 == 7) {
    cam7_done = true;
  }
  if (receive_rnd_val_1 == 8) {
    cam8_done = true;
  }
  if (receive_rnd_val_1 == 9) {
    cam9_done = true;
  }
  if (receive_rnd_val_1 == 10) {
    cam10_done = true;
  }
  if (receive_rnd_val_1 == 11) {
    cam11_done = true;
  }
  if (receive_rnd_val_1 == 12) {
    cam12_done = true;
  }


  Serial.println("Receive Data: ");
  Serial.println(receive_rnd_val_1);
  Serial.println("<<<<<");
}

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void setup() {
  Serial.begin(115200);
  connectWifi();
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  getLocalTime();
  pinMode(relayCam1, OUTPUT);
  pinMode(relayCam2, OUTPUT);
  pinMode(relayTubeLight, OUTPUT);

  digitalWrite(relayCam1, HIGH);
  digitalWrite(relayCam2, HIGH);
  digitalWrite(relayTubeLight, HIGH);

  Wire.begin();
  bh1750_a.begin(BH1750::CONTINUOUS_HIGH_RES_MODE, 0x5C);
  bh1750_b.begin(BH1750::CONTINUOUS_HIGH_RES_MODE, 0x23);

  pinMode(directionPin, OUTPUT);
  pinMode(stepPin, OUTPUT);
  pinMode(inductiveSensor, INPUT_PULLUP);  // define pin as Input  sensor
  goHomeBoot();

  pinMode(requestButton, INPUT_PULLUP);

  startESPNOW();
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
void loop() {
  getLocalTime();
  Serial.print(HOUR);
  Serial.print(":");
  Serial.print(MINUTE);
  Serial.print(":");
  Serial.println(SECOND);
  delay(1000);

  if ((HOUR > 5) && (HOUR <= 17) && (MINUTE % 10 == 0)) {
    home = false;
    end = false;

    while (end == false) {
      goLeft(500);
      goLeft(500);
      delay(2000);
      if ((digitalRead(inductiveSensor) == 0)) {
        end = true;
      }
      // Get sensor data
      if (bh1750_a.measurementReady()) {
        light_level_a = bh1750_a.readLightLevel();
      }

      if (bh1750_b.measurementReady()) {
        light_level_b = bh1750_b.readLightLevel();
      }

      sendLuxData();

      if (digitalRead(inductiveSensor) == 0) {
        end = true;
      }

      step++;
    }
    goHome();
    step = 1;
  }


  if ((HOUR == 4 && MINUTE == 1) || (HOUR == 18 && MINUTE == 1)) {
    esp_wifi_stop();
    esp_wifi_start();
    startESPNOW();
    digitalWrite(relayTubeLight, LOW);
    digitalWrite(relayCam1, LOW);
    digitalWrite(relayCam2, LOW);
    Serial.println("BEGIN");
    home = false;
    end = false;
    while (end == false) {
      goLeft(500);
      goLeft(500);
      delay(3000);
      if (digitalRead(inductiveSensor) == 0) {
        end = true;
      }
      unsigned long captureTime = millis() / 1000;
      unsigned long currentBatchCaptureTime = 0;
      int timeout = 0;

      bool cam1to6Complete = false;
      bool cam7to12Complete = false;
      while (!isComplete) {
        cam1to6Complete = (cam1_done) && (cam2_done) && (cam3_done) && (cam4_done) && (cam5_done) && (cam6_done);
        cam7to12Complete = (cam7_done) && (cam8_done) && (cam9_done) && (cam10_done) && (cam11_done) && (cam12_done);
        captureTime = millis() / 1000;
        // int timerOffset = 0;
        currentBatchCaptureTime = millis() / 1000 % 3600 % 1;
        if (currentBatchCaptureTime % 1 == 0) {
          Serial.println(timeout++);
        }
        // currentBatchCaptureTime = millis() / 1000 % 3600 % 120;

        if (isComplete) {
          currentBatchCaptureTime = 0;
        }

        if ((timeout == 120) && isComplete == false) {  // Every 2 minutes
          //  timerOffset = currentBatchCaptureTime;
          // currentBatchCaptureTime = 0;
          timeout = 0;

          if (!cam1to6Complete) {
            digitalWrite(relayCam1, HIGH);
            delay(2000);
            digitalWrite(relayCam1, LOW);
          }

          if (!cam7to12Complete) {
            digitalWrite(relayCam2, HIGH);
            delay(2000);
            digitalWrite(relayCam2, LOW);
          }
        }

        Serial.print("Cam1: ");
        Serial.println(cam1_done);

        Serial.print("Cam2: ");
        Serial.println(cam2_done);

        Serial.print("Cam3: ");
        Serial.println(cam3_done);

        Serial.print("Cam4: ");
        Serial.println(cam4_done);

        Serial.print("Cam5: ");
        Serial.println(cam5_done);

        Serial.print("Cam6: ");
        Serial.println(cam6_done);

        Serial.print("Cam7: ");
        Serial.println(cam7_done);

        Serial.print("Cam8: ");
        Serial.println(cam8_done);

        Serial.print("Cam9: ");
        Serial.println(cam9_done);

        Serial.print("Cam10: ");
        Serial.println(cam10_done);

        Serial.print("Cam11: ");
        Serial.println(cam11_done);

        Serial.print("Cam12: ");
        Serial.println(cam12_done);


        delay(1000);
        Serial.println("CAPTURE REQUEST");
        isComplete = (cam1_done) && (cam2_done) && (cam3_done) && (cam4_done) && (cam5_done) && (cam6_done) && (cam7_done) && (cam8_done) && (cam9_done) && (cam10_done) && (cam11_done) && (cam12_done);
        Serial.print("IS COMPLETE: ");
        Serial.println(isComplete);
        //----------------------------------------Set values to send
        send_rnd_val_1 = true;
        send_Data.rnd_1 = send_rnd_val_1;
        //----------------------------------------

        Serial.println();
        Serial.print(">>>>> ");
        Serial.println("Send data");

        //----------------------------------------Send message via ESP-NOW
        if (cam1_done == false) {
          esp_err_t result = esp_now_send(broadcastAddress1, (uint8_t *)&send_Data, sizeof(send_Data));
        }

        if (cam2_done == false) {
          esp_err_t result2 = esp_now_send(broadcastAddress2, (uint8_t *)&send_Data, sizeof(send_Data));
        }

        if (cam3_done == false) {
          esp_err_t result3 = esp_now_send(broadcastAddress3, (uint8_t *)&send_Data, sizeof(send_Data));
        }

        if (cam4_done == false) {
          esp_err_t result4 = esp_now_send(broadcastAddress4, (uint8_t *)&send_Data, sizeof(send_Data));
        }

        if (cam5_done == false) {
          esp_err_t result5 = esp_now_send(broadcastAddress5, (uint8_t *)&send_Data, sizeof(send_Data));
        }

        if (cam6_done == false) {
          esp_err_t result6 = esp_now_send(broadcastAddress6, (uint8_t *)&send_Data, sizeof(send_Data));
        }

        if (cam7_done == false) {
          esp_err_t result7 = esp_now_send(broadcastAddress7, (uint8_t *)&send_Data, sizeof(send_Data));
        }

        if (cam8_done == false) {
          esp_err_t result8 = esp_now_send(broadcastAddress8, (uint8_t *)&send_Data, sizeof(send_Data));
        }

        if (cam9_done == false) {
          esp_err_t result9 = esp_now_send(broadcastAddress9, (uint8_t *)&send_Data, sizeof(send_Data));
        }

        if (cam10_done == false) {
          esp_err_t result10 = esp_now_send(broadcastAddress10, (uint8_t *)&send_Data, sizeof(send_Data));
        }

        if (cam11_done == false) {
          esp_err_t result11 = esp_now_send(broadcastAddress11, (uint8_t *)&send_Data, sizeof(send_Data));
        }

        if (cam12_done == false) {
          esp_err_t result12 = esp_now_send(broadcastAddress12, (uint8_t *)&send_Data, sizeof(send_Data));
        }



        // if (result && result2 == ESP_OK) {
        //   Serial.println("Sent with success");
        // }
        // else {
        //   Serial.println("Error sending the data");
        // }
        //----------------------------------------
        if (digitalRead(inductiveSensor) == 0) {
          end = true;
        }
        Serial.println("AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAa");
        Serial.println(captureTime);

        Serial.println("BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBb");
        Serial.println(currentBatchCaptureTime);


        // Serial.println("CCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCCc");
        // Serial.println(currentBatchCaptureTime + timerOffset);
      }
      cam1_done = false;
      cam2_done = false;
      cam3_done = false;
      cam4_done = false;
      cam5_done = false;
      cam6_done = false;
      cam7_done = false;
      cam8_done = false;
      cam9_done = false;
      cam10_done = false;
      cam11_done = false;
      cam12_done = false;
      isComplete = false;
      captureTime = 0;
      currentBatchCaptureTime = 0;
    }
    goHome();
    digitalWrite(relayCam1, HIGH);
    digitalWrite(relayCam2, HIGH);
    digitalWrite(relayTubeLight, HIGH);
    connectWifi();
  }
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

void goLeft(int duration) {
  Serial.println("Going LEFT");
  for (int i = 0; i < stepsPerRev; i++) {
    digitalWrite(directionPin, HIGH);
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(duration);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(50);
  }
}

void goRight(int duration) {
  Serial.println("Going RIGHT");
  for (int i = 0; i < stepsPerRev; i++) {
    digitalWrite(directionPin, LOW);
    digitalWrite(stepPin, HIGH);
    delayMicroseconds(duration);
    digitalWrite(stepPin, LOW);
    delayMicroseconds(50);
  }
}

void goHomeBoot() {
  goLeft(500);
  goLeft(500);
  goLeft(500);
  goRight(500);
  goRight(500);

  while (!home) {
    goRight(500);
    if (digitalRead(inductiveSensor) == 0) {
      home = true;
    }
  }
}

void goHome() {
  goRight(500);
  goRight(500);
  while (!home) {
    goRight(500);
    if (digitalRead(inductiveSensor) == 0) {
      home = true;
    }
  }
}

void connectWifi() {
  WiFi.begin(ssid, password);
  int attempts = 0;
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(300);
    attempts++;
    if (attempts > 7) {
      ESP.restart();
    }
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();
}


void sendLuxData() {
  String urlFinal = "https://script.google.com/macros/s/" + GOOGLE_SCRIPT_ID + "/exec?" + "&step=" + step + "&sensor1=" + light_level_a + "&sensor2=" + light_level_b;

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
    Serial.println("Payload: " + payload);
  } else {
    sendLuxData();
  }
  //---------------------------------------------------------------------
  http.end();
  delay(2000);
}

// Will act as RTC or Real Time Clock
void getLocalTime() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("Failed to obtain time");
    return;
  }

  YEAR = timeinfo.tm_year + 1900;
  MONTH = timeinfo.tm_mon + 1;
  DAY = timeinfo.tm_mday;
  //  day_of_week = timeinfo.tm_wday;
  HOUR = timeinfo.tm_hour;
  MINUTE = timeinfo.tm_min;
  SECOND = timeinfo.tm_sec;

  //  Serial.println(&timeinfo, "%u %m %d %Y %H:%M:%S");
}


void startESPNOW() {
  // ESP NOW
  WiFi.mode(WIFI_STA);  //--> Set device as a Wi-Fi Station
  Serial.print("MAC ADDRESS: ");
  Serial.println(WiFi.macAddress());
  //----------------------------------------Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  //----------------------------------------

  //----------------------------------------Once ESPNow is successfully Init, we will register for Send CB to
  // get the status of Trasnmitted packet
  esp_now_register_send_cb(OnDataSent);
  //----------------------------------------

  //----------------------------------------Register peer
  memcpy(peerInfo1.peer_addr, broadcastAddress1, 6);
  memcpy(peerInfo2.peer_addr, broadcastAddress2, 6);
  memcpy(peerInfo3.peer_addr, broadcastAddress3, 6);
  memcpy(peerInfo4.peer_addr, broadcastAddress4, 6);
  memcpy(peerInfo5.peer_addr, broadcastAddress5, 6);
  memcpy(peerInfo6.peer_addr, broadcastAddress6, 6);
  memcpy(peerInfo7.peer_addr, broadcastAddress7, 6);
  memcpy(peerInfo8.peer_addr, broadcastAddress8, 6);
  memcpy(peerInfo9.peer_addr, broadcastAddress9, 6);
  memcpy(peerInfo10.peer_addr, broadcastAddress10, 6);
  memcpy(peerInfo11.peer_addr, broadcastAddress11, 6);
  memcpy(peerInfo12.peer_addr, broadcastAddress12, 6);


  peerInfo1.channel = 0;
  peerInfo2.channel = 0;
  peerInfo3.channel = 0;
  peerInfo4.channel = 0;
  peerInfo5.channel = 0;
  peerInfo6.channel = 0;
  peerInfo7.channel = 0;
  peerInfo8.channel = 0;
  peerInfo9.channel = 0;
  peerInfo10.channel = 0;
  peerInfo11.channel = 0;
  peerInfo12.channel = 0;


  peerInfo1.encrypt = false;
  peerInfo2.encrypt = false;
  peerInfo3.encrypt = false;
  peerInfo4.encrypt = false;
  peerInfo5.encrypt = false;
  peerInfo6.encrypt = false;
  peerInfo7.encrypt = false;
  peerInfo8.encrypt = false;
  peerInfo9.encrypt = false;
  peerInfo10.encrypt = false;
  peerInfo11.encrypt = false;
  peerInfo12.encrypt = false;
  //----------------------------------------

  //----------------------------------------Add peer
  if (esp_now_add_peer(&peerInfo1) != ESP_OK) {
    Serial.println("Failed to add peer 1");
    return;
  }
  if (esp_now_add_peer(&peerInfo2) != ESP_OK) {
    Serial.println("Failed to add peer 2");
    return;
  }
  if (esp_now_add_peer(&peerInfo3) != ESP_OK) {
    Serial.println("Failed to add peer 3");
    return;
  }
  if (esp_now_add_peer(&peerInfo4) != ESP_OK) {
    Serial.println("Failed to add peer 4");
    return;
  }
  if (esp_now_add_peer(&peerInfo5) != ESP_OK) {
    Serial.println("Failed to add peer 5");
    return;
  }
  if (esp_now_add_peer(&peerInfo6) != ESP_OK) {
    Serial.println("Failed to add peer 6");
    return;
  }
  if (esp_now_add_peer(&peerInfo7) != ESP_OK) {
    Serial.println("Failed to add peer 7");
    return;
  }
  if (esp_now_add_peer(&peerInfo8) != ESP_OK) {
    Serial.println("Failed to add peer 8");
    return;
  }
  if (esp_now_add_peer(&peerInfo9) != ESP_OK) {
    Serial.println("Failed to add peer 9");
    return;
  }
  if (esp_now_add_peer(&peerInfo10) != ESP_OK) {
    Serial.println("Failed to add peer 10");
    return;
  }
  if (esp_now_add_peer(&peerInfo11) != ESP_OK) {
    Serial.println("Failed to add peer 11");
    return;
  }
  if (esp_now_add_peer(&peerInfo12) != ESP_OK) {
    Serial.println("Failed to add peer 12");
    return;
  }

  //----------------------------------------

  esp_now_register_recv_cb(OnDataRecv);  //--> Register for a callback function that will be called when data is received
}