#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <WiFiManager.h>
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"
#include "Base64.h"
#include "CameraPins.h"
#include "esp_camera.h"
#include <esp_now.h>
#include "esp_wifi.h"


// Enter your WiFi ssid and password
const char* networkName = ""; // gets pointer for cameraName
const char* password = "";   //your network password

uint8_t broadcastAddress[] = {}; //--> REPLACE WITH THE MAC Address of your receiver.



//----------------------------------------Define variables to store incoming readings
bool receive_rnd_val_1 = false;
bool sentSuccess = false;
//----------------------------------------

esp_now_peer_info_t peerInfo;
String success; //--> Variable to store if sending data was successful

//----------------------------------------Structure example to send data
// Must match with ESP32 paired.
typedef struct struct_message {
    bool rnd_1;
    int rnd_2;
} struct_message;

struct_message send_Data; // Create a struct_message to send data.
struct_message receive_Data; // Create a struct_message to receive data.
//----------------------------------------

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ Callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
  
  if (status == 0){
    success = "Delivery Success :)";
    receive_rnd_val_1 = false;
  }
  else{
    success = "Delivery Fail :(";
  }
  Serial.println(">>>>>");
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++ Callback when data is received
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&receive_Data, incomingData, sizeof(receive_Data));
  Serial.println();
  Serial.println("<<<<< Receive Data:");
  Serial.print("Bytes received: ");
  Serial.println(len);
  receive_rnd_val_1 = receive_Data.rnd_1;
  Serial.println("Receive Data: ");
  Serial.println(receive_rnd_val_1);
  Serial.println("<<<<<");
}
//++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++



//----------------------------------------Variables to accommodate the data to be sent.
// int send_rnd_val_1 = 1;   // Change to camera number
// int send_rnd_val_1 = 2;   // Change to camera number
// int send_rnd_val_1 = 3;   // Change to camera number
// int send_rnd_val_1 = 4;   // Change to camera number
// int send_rnd_val_1 = 5;   // Change to camera number

// int send_rnd_val_1 = 6;   // Change to camera number
int send_rnd_val_1 = 7;   // Change to camera number

// int send_rnd_val_1 = 8;   // Change to camera number
// int send_rnd_val_1 = 9;   // Change to camera number
// int send_rnd_val_1 = 10;   // Change to camera number
// int send_rnd_val_1 = 11;   // Change to camera number
// int send_rnd_val_1 = 12;   // Change to camera number

//----------------------------------------

// Set static addresses
// IPAddress staticIP(192,168,3,5);   //Cam1
// IPAddress staticIP(192,168,3,6);   //Cam2
// IPAddress staticIP(192,168,3,7);   //Cam3
// IPAddress staticIP(192,168,3,8);   //Cam4
// IPAddress staticIP(192,168,3,9);   //Cam5
// IPAddress staticIP(192,168,3,10);  //Cam6
IPAddress staticIP(192,168,3,11);  //Cam7
// IPAddress staticIP(192,168,3,12);  //Cam8
// IPAddress staticIP(192,168,3,13);  //Cam9
// IPAddress staticIP(192,168,3,14);  //Cam10
// IPAddress staticIP(192,168,3,15);  //Cam11
// IPAddress staticIP(192,168,3,16);  //Cam12

IPAddress gateway(192,168,3,1);
IPAddress subnet(255,255,255,0);
IPAddress dns(192,168,3,1);

// String cameraName = "Cam1";
// String cameraName = "Cam2";
// String cameraName = "Cam3";
// String cameraName = "Cam4";
// String cameraName = "Cam5";
// String cameraName = "Cam6";
String cameraName = "Cam7";
// String cameraName = "Cam8";
// String cameraName = "Cam9";
// String cameraName = "Cam10";
// String cameraName = "Cam11";
// String cameraName = "Cam12";
String script = "";
// e.g.: String script = "/macros/s/ASLKJDFxkJOIASFvDLSKJxLv.../exec"



framesize_t frameSize = FRAMESIZE_UXGA; // lower res is more stable
long captureDelay = 1000; // ms
#define DEBUG // comment to disable serial print
///////////////////////////////////////////////////////



////////////////////// FRAMESIZES /////////////////////
//FRAMESIZE_96X96    // 96x96
//FRAMESIZE_QQVGA    // 160x120
//FRAMESIZE_QCIF     // 176x144
//FRAMESIZE_HQVGA    // 240x176
//FRAMESIZE_240X240  // 240x240
//FRAMESIZE_QVGA     // 320x240
//FRAMESIZE_CIF      // 400x296
//FRAMESIZE_HVGA     // 480x320
//FRAMESIZE_VGA      // 640x480
//FRAMESIZE_SVGA     // 800x600
//FRAMESIZE_XGA      // 1024x768
//FRAMESIZE_HD       // 1280x720
//FRAMESIZE_SXGA     // 1280x1024
//FRAMESIZE_UXGA     // 1600x1200
///////////////////////////////////////////////////////

///////////////// CUSTOM GLOBAL VALUES ////////////////
long initDelay = 8000; // ms
int numDummyImages = 3; // works after init delay to remove poorly exposed images
// #define RESERVED_IMAGEFILE_BYTES 100000 // works well for 3 batches
// #define IMAGEFILE_BATCHES 3
#define RESERVED_IMAGEFILE_BYTES 87500 // works well for 3 batches
#define IMAGEFILE_BATCHES 4
///////////////////////////////////////////////////////

int captureCount = 0; // number of successful image uploads

// parameters to be sent to the GApps Script
String folderName = "&folderName="; // defaults to cameraName
// String folderName = "&folderName=";
String fileName = "&fileName=";     // defaults to cameraName (GApps Script adds timestamp)
String file = "&file=";             // will hold base64 encoded jpeg image  

// macros allow you to remove print statements easily when debugging is no longer needed
#ifdef DEBUG    
  #define DEBUG_BEGIN(...)    Serial.begin(__VA_ARGS__)
  #define PRINT(...)          Serial.print(__VA_ARGS__)     
  #define PRINTLN(...)        Serial.println(__VA_ARGS__) 
  #define PRINTF(...)         Serial.printf(__VA_ARGS__) 
#else
  #define DEBUG_BEGIN(...)
  #define PRINT(...)   
  #define PRINTLN(...)  
  #define PRINTF(...)
#endif

///////////////////////////////////////////////////////
/////////////////// HELPER FUNCTIONS //////////////////
///////////////////////////////////////////////////////
void turnOnLight() { // unused
  ledcAttachPin(4, 3);
  ledcSetup(3, 5000, 8);
  ledcWrite(3,10);
}

void turnOffLight() { // unused
  ledcWrite(3,0);
  ledcDetachPin(3);
}

String urlEncode(String str) {
    String encodedString="";
    char c;
    char code0;
    char code1;
    char code2;
    for (int i =0; i < str.length(); i++){
      c=str.charAt(i);
      if (c == ' '){
        encodedString+= '+';
      } else if (isalnum(c)){
        encodedString+=c;
      } else{
        code1=(c & 0xf)+'0';
        if ((c & 0xf) >9){
            code1=(c & 0xf) - 10 + 'A';
        }
        c=(c>>4)&0xf;
        code0=c+'0';
        if (c > 9){
            code0=c - 10 + 'A';
        }
        code2='\0';
        encodedString+='%';
        encodedString+=code0;
        encodedString+=code1;
        //encodedString+=code2;
      }
      yield(); // not sure what this does, so removed
    }
    return encodedString;
}

// prints message through wifi client
void clientPrint(WiFiClientSecure& clientTCP, const String& msg) {
  // note use of &; these are references
  // they allow you to use the variable you input rather than create a new instance (therefore save memory)
  // but be careful: modifications to them will change the original variable
  // use const to ensure they won't be modified
  
  int msgLength = msg.length();
  for (int i = 0; i < msgLength; i = i+1000) {
    if (i+1000 > msgLength) {
      clientTCP.print(msg.substring(i, msgLength));
    } else {
      clientTCP.print(msg.substring(i, i+1000));
    }
  }
}

#ifdef DEBUG
String getSummary(String msg) {
  return (msg.substring(0,50) + "..." + msg.substring(msg.length()-50,msg.length()));
}
#endif
///////////////////////////////////////////////////////
///////////////// END HELPER FUNCTIONS ////////////////
///////////////////////////////////////////////////////

///////////////////////////////////////////////////////
//////////////////// INIT FUNCTIONS ///////////////////
///////////////////////////////////////////////////////
void startWiFi() {
  PRINT("Starting WiFi...");
  
  WiFi.mode(WIFI_STA);
  // WiFiManager wm;
  // wm.setDebugOutput(false);
  // // wm.resetSettings();
  // char* networkName = &cameraName[0]; // gets pointer for cameraName
  // PRINT("connect to \"");
  // PRINT(networkName);
  // PRINT("\" on your phone to enter WiFi credentials...");
  // wm.setSTAStaticIPConfig(staticIP, gateway, subnet, dns); // optional DNS 4th argument
  // bool result = wm.autoConnect(networkName, password); // second param is password (blank for none)
  if (!WiFi.config(staticIP, gateway, subnet, dns)) {
    Serial.println("STA failed to configure");
    ESP.restart();
  }
    WiFi.begin(networkName, password);
    while (WiFi.status() != WL_CONNECTED) {
      delay(50);
      Serial.print(".");
    }
    PRINTLN("success");
    PRINT("STA IP address: ");
    PRINTLN(WiFi.localIP()); 
          Serial.print("Subnet Mask: ");
          Serial.println(WiFi.subnetMask());
          Serial.print("Gateway IP: ");
          Serial.println(WiFi.gatewayIP());
          Serial.print("DNS 1: ");
          Serial.println(WiFi.dnsIP(0));
          Serial.print("DNS 2: ");
          Serial.println(WiFi.dnsIP(1));
}

void wait(long duration) {
  long start = millis();
  while (millis() - start < duration) {}//do nothing
}

void startCamera() {
  camera_config_t cameraConfig;
  cameraConfig.ledc_channel = LEDC_CHANNEL_0;
  cameraConfig.ledc_timer = LEDC_TIMER_0;
  cameraConfig.pin_d0 = Y2_GPIO_NUM;
  cameraConfig.pin_d1 = Y3_GPIO_NUM;
  cameraConfig.pin_d2 = Y4_GPIO_NUM;
  cameraConfig.pin_d3 = Y5_GPIO_NUM;
  cameraConfig.pin_d4 = Y6_GPIO_NUM;
  cameraConfig.pin_d5 = Y7_GPIO_NUM;
  cameraConfig.pin_d6 = Y8_GPIO_NUM;
  cameraConfig.pin_d7 = Y9_GPIO_NUM;
  cameraConfig.pin_xclk = XCLK_GPIO_NUM;
  cameraConfig.pin_pclk = PCLK_GPIO_NUM;
  cameraConfig.pin_vsync = VSYNC_GPIO_NUM;
  cameraConfig.pin_href = HREF_GPIO_NUM;
  cameraConfig.pin_sscb_sda = SIOD_GPIO_NUM;
  cameraConfig.pin_sscb_scl = SIOC_GPIO_NUM;
  cameraConfig.pin_pwdn = PWDN_GPIO_NUM;
  cameraConfig.pin_reset = RESET_GPIO_NUM;
  cameraConfig.xclk_freq_hz = 20000000;
  cameraConfig.pixel_format = PIXFORMAT_JPEG;
  cameraConfig.grab_mode = CAMERA_GRAB_LATEST; // important for ensuring correct image is pulled from buffer
  
  //init with high specs to pre-allocate larger buffers
  if(psramFound()){
    PRINTLN("PSRAM found");
    cameraConfig.frame_size = FRAMESIZE_UXGA;
    cameraConfig.jpeg_quality = 10;  //0-63 lower number means higher quality
    cameraConfig.fb_count = 2;
  } else {
    PRINTLN("PSRAM not found");
    cameraConfig.frame_size = FRAMESIZE_SVGA;
    cameraConfig.jpeg_quality = 12;  //0-63 lower number means higher quality
    cameraConfig.fb_count = 1;
  }

  // initialize camera
  esp_err_t err = esp_camera_init(&cameraConfig);
  if (err != ESP_OK) {
    PRINTF("Camera init failed with error 0x%x", err);
    ESP.restart();
  }

  // set desired frame size
  sensor_t * s = esp_camera_sensor_get();
  s->set_framesize(s, frameSize);  // UXGA|SXGA|XGA|SVGA|VGA|CIF|QVGA|HQVGA|QQVGA
  s->set_brightness(s, 2);     // -2 to 2
  s->set_contrast(s, 2);       // -2 to 2
  s->set_saturation(s, 2);     // -2 to 2
  s->set_special_effect(s, 0); // 0 to 6 (0 - No Effect, 1 - Negative, 2 - Grayscale, 3 - Red Tint, 4 - Green Tint, 5 - Blue Tint, 6 - Sepia)
  s->set_whitebal(s, 0);       // 0 = disable , 1 = enable
  s->set_awb_gain(s, 1);       // 0 = disable , 1 = enable
  s->set_wb_mode(s, 1);        // 0 to 4 - if awb_gain enabled (0 - Auto, 1 - Sunny, 2 - Cloudy, 3 - Office, 4 - Home)
  s->set_exposure_ctrl(s, 1);  // 0 = disable , 1 = enable
  s->set_aec2(s, 1);           // 0 = disable , 1 = enable
  s->set_ae_level(s, 2);       // -2 to 2
  s->set_aec_value(s, 1200);    // 0 to 1200
  s->set_gain_ctrl(s, 1);      // 0 = disable , 1 = enable
  s->set_agc_gain(s, 0);       // 0 to 30
  s->set_gainceiling(s, (gainceiling_t)0);  // 0 to 6
  s->set_bpc(s, 0);            // 0 = disable , 1 = enable
  s->set_wpc(s, 1);            // 0 = disable , 1 = enable
  s->set_raw_gma(s, 1);        // 0 = disable , 1 = enable
  s->set_lenc(s, 1);           // 0 = disable , 1 = enable
  s->set_hmirror(s, 1);        // 0 = disable , 1 = enable
  s->set_vflip(s, 1);          // 0 = disable , 1 = enable
  s->set_dcw(s, 1);            // 0 = disable , 1 = enable
  s->set_colorbar(s, 0);       // 0 = disable , 1 = enable

  // wait before capturing to allow auto-exposure to adjust (discussed in https://github.com/espressif/esp32-camera/issues/314)
  // higher resolutions will take longer
  PRINT("Waiting " + String(initDelay) + "ms after init..."); // 8sec delay + 3 dummy captures seems to work on UXGA framesize
  wait(initDelay);
  PRINTLN("done");
  
  PRINT("Capturing " + String(numDummyImages) + " dummy images...");
  for (int i = 0; i < numDummyImages; i++) {
    camera_fb_t* fb = esp_camera_fb_get(); 
    esp_camera_fb_return(fb);
  }
  PRINTLN("done");
}
///////////////////////////////////////////////////////
////////////////// END INIT FUNCTIONS /////////////////
///////////////////////////////////////////////////////

///////////////////////////////////////////////////////
///////////// CAPTURE AND UPLOAD FUNCTIONS ////////////
///////////////////////////////////////////////////////
camera_fb_t* captureImage() {
  
  PRINT("Capturing image...");
  camera_fb_t * fb = NULL;
  fb = esp_camera_fb_get();  
  if(!fb) {
    PRINTLN("failed. Restarting");
    ESP.restart();
  } else {
    PRINTLN("success");
    return fb;
  }
}

int getImageFileLength(camera_fb_t* fb, WiFiClientSecure& clientTCP, const bool printToClient) {
  
  int totalLength = 0;

  const int base64EncodeBatchSize = 3; // not to be confused with an imageFile batchSize
  char *input = (char *)fb->buf; // pointer to image (frame) buffer
  char output[base64_enc_len(base64EncodeBatchSize)]; // char array initialized with a known size (when batchSize of data is encoded to base64)
  int len = fb->len; // length of frame buffer

  int batchSize = len/IMAGEFILE_BATCHES;
  while (batchSize % base64EncodeBatchSize != 0) {batchSize++;} // keep batchSize divisible by base64EncodeBatchSize

  // create array of indicies where one batch ends and another begins
  // for example, if the frame buffer is 100,000 long, and we have 3 batches, batchSize = 33333, indicies = [33333, 66666]
  int indicies[IMAGEFILE_BATCHES - 1];
  for (int i = 0; i < IMAGEFILE_BATCHES - 1; i++) {
    indicies[i] = batchSize * (i + 1);
  }

  // initialize imageFile string (it will contain image data, so it will be large!)
  String imageFile;
  imageFile.reserve(RESERVED_IMAGEFILE_BYTES); // set aside space for imageFile
  imageFile += "data:image/jpeg;base64,"; // prefix for jpeg

  int currentBatch = 0;
  
  // print frame buffer to wifi client
  for (int i=0; i <= len; i++) {
    base64_encode(output, (input++), base64EncodeBatchSize);

    // do url encoding on every base64 batch
    if (i % base64EncodeBatchSize == 0) {imageFile += urlEncode(String(output));}

    // if we've reached the end of a imageFile batch, print batch and reset imageFile to save memory
    if (i == indicies[currentBatch]) {
      if (printToClient) {clientPrint(clientTCP, imageFile);} // print batch
      totalLength += imageFile.length();
      imageFile = ""; // reset
      currentBatch++;
    }
  }

  if (printToClient) {clientPrint(clientTCP, imageFile);} // print last batch
  totalLength += imageFile.length();
  
  return totalLength;
}


bool uploadImage(camera_fb_t* fb) {  
  const char* scriptDomain = "script.google.com";
  
  PRINT("Connecting to " + String(scriptDomain) + "...");
  WiFiClientSecure clientTCP;
  clientTCP.setInsecure(); // need insecure; won't connect to script.google.com otherwise

  // 443 is standard port for transmission control protocol (TCP)
  if (clientTCP.connect(scriptDomain, 443)) {
    PRINTLN("success");
    
    bool printToClient = false;
    int imageFileLength = getImageFileLength(fb, clientTCP, printToClient);
    PRINT(F("File length: "));
    PRINTLN(imageFileLength);
    
    // String params = fileName + file;
    String params = folderName + fileName + file;


    PRINT("Uploading");
    clientTCP.println("POST " + script + " HTTP/1.1");
    clientTCP.println("Host: " + String(scriptDomain));
    clientTCP.println("Content-Length: " + String(params.length()+imageFileLength));
    clientTCP.println("Content-Type: application/x-www-form-urlencoded");
    clientTCP.println("Connection: keep-alive");
    clientTCP.println();
    
    clientTCP.print(params); // sends completed folderName and fileName; file will be added in loop:

    printToClient = true;
    getImageFileLength(fb, clientTCP, printToClient);
        
    esp_camera_fb_return(fb); // clear image from camera buffer
    
    int waitTime = 10000; // timeout 10 seconds
    long startTime = millis();
    boolean firstNewLineFound = false;
    String body = "";
    
    // wait for response
    while ((startTime + waitTime) > millis()) {
      PRINT(".");
      wait(100);     
      while (clientTCP.available()) {
          char c = clientTCP.read();
          // if document has started, save stream
          if (firstNewLineFound==true) {body += String(c);}
          // if we find the first newline string, begin saving future characters
          if (c == '\n' && body.length() == 0) {firstNewLineFound=true;}
       }
       if (body.length()>0) break; // break if we have grabbed info
    }
    clientTCP.stop();

    if (body.length()>0) {
      // check if exception exists in response
      int exceptionStart = body.indexOf("Exception:");
      int exceptionEnd = body.indexOf("</div>", exceptionStart);
      if (exceptionStart > 0) {
        PRINTLN("error");
        PRINTLN(body.substring(exceptionStart, exceptionEnd)); // print exception
        ESP.restart();
        return false;
      } else {
        captureCount++;
        PRINT("success (");
        PRINT(captureCount);
        PRINTLN(")");
        return true;
      }
    } else {
      PRINT("failed (timeout)");
      ESP.restart();
      // return false; // timeout
    }
  }
  else {
    PRINTLN("failed");
    ESP.restart();
    // return false;
  }  
}
///////////////////////////////////////////////////////
/////////// END CAPTURE AND UPLOAD FUNCTIONS //////////
///////////////////////////////////////////////////////

///////////////////////////////////////////////////////
//////////////////// SETUP AND LOOP ///////////////////
///////////////////////////////////////////////////////
void setup() {
  // disable brown-out detection (WiFi and camera can cause voltage drops)
    WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);
    DEBUG_BEGIN(115200);
    wait(10);


      startESPNOW();
        //----------------------------------------
        esp_now_register_recv_cb(OnDataRecv); //--> Register for a callback function that will be called when data is received
        //----------------------------------------

  folderName += cameraName;
  fileName += cameraName + ".jpg";


  startCamera();
  startWiFi();

  delay(2000);
  if (receive_rnd_val_1 == true) {
    uploadImage(captureImage());
    PRINT("Waiting " + String(captureDelay) + "ms...");
    sentSuccess = true;
    // wait(captureDelay);
    PRINTLN("UPLOADED TO GDRIVE");
  }


}

void loop() {
        // SEND DONE TO MAIN
            while (sentSuccess == true) {
                //----------------------------------------Init ESP-NOW
                //----------------------------------------
                esp_wifi_stop();
                // esp_wifi_deinit();
                // wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();

                // esp_wifi_init(&cfg);
                esp_wifi_start();
                startESPNOW();

                send_Data.rnd_2 = send_rnd_val_1;
                esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &send_Data, sizeof(send_Data));
                if (result == ESP_OK) {
                  Serial.println("Sent with success");
                  sentSuccess = false;
                }
                else {
                  Serial.print("Error sending the data: ");
                  Serial.println(send_Data.rnd_2);
                }

                Serial.println("CHECKPOOOOOOOOOOOOOOOIIIIIIIIIINNNNNNNNNTTTTTT");

            }
        // SEND DONE TO MAIN
  Serial.println("MAINLOOP");
  delay(5000);
  ESP.restart();
}
///////////////////////////////////////////////////////
////////////////// END SETUP AND LOOP /////////////////
///////////////////////////////////////////////////////


void startESPNOW() {
        // ESP NOW
      Serial.println("STARTING ESPNOW");
      WiFi.mode(WIFI_STA); //--> Set device as a Wi-Fi Station for ESPNOW
      Serial.print("MAC ADDRESS: ");
      Serial.println(WiFi.macAddress());
      if (esp_now_init() != ESP_OK) {
        Serial.println("Error initializing ESP-NOW");
        return;
      }
        //----------------------------------------Once ESPNow is successfully Init, we will register for Send CB to
        // get the status of Trasnmitted packet
        esp_now_register_send_cb(OnDataSent);
        //----------------------------------------
        //----------------------------------------Register peer
        memcpy(peerInfo.peer_addr, broadcastAddress, 6);
        peerInfo.channel = 0;  
        peerInfo.encrypt = false;
        //----------------------------------------
        
        //----------------------------------------Add peer        
        if (esp_now_add_peer(&peerInfo) != ESP_OK){
          Serial.println("ESPNOW: Failed to add peer");
          return;
        }
        Serial.println("ESPNOW started");
}

