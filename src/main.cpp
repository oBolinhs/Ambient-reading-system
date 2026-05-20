#include <Arduino.h>
#include <Wire.h>
#include <U8g2lib.h>
#include "DHT.h"
#include <Firebase_ESP_Client.h>

//include local files
#include "tasks.h"
#include "sensitive_info.h"

#define DHTPIN 17     // Digital pin connected to the DHT sensor
#define DHTTYPE DHT11   // DHT 22  (AM2302), AM2321


FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;


DHT dht(DHTPIN, DHTTYPE);

//Creating the handles for the queues
QueueHandle_t humidityHandleSend = NULL;
QueueHandle_t temperatureHandleSend = NULL;
QueueHandle_t humidityHandleDB = NULL;
QueueHandle_t temperatureHandleDB = NULL;

//Creating the handles for the tasks
TaskHandle_t readDHTTask = NULL;
TaskHandle_t displayDataTask = NULL;
TaskHandle_t sendFirebaseTask = NULL;

//Creating the u8g2 object (OLED screen)
U8G2_SSD1306_64X48_ER_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);

//First task function


void setup() {
  Serial.begin(115200);
  Serial.println(F("DHT11 test!"));

  //Initialize the DHT
  dht.begin();

  //Configure wifi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
  }
  Serial.println("Wifi connected");
  delay(1000);

  //Creating the queues
  humidityHandleSend = xQueueCreate(5, sizeof(float));
  temperatureHandleSend = xQueueCreate(5,sizeof(float));
  humidityHandleDB = xQueueCreate(5,sizeof(float));
  temperatureHandleDB = xQueueCreate(5,sizeof(float));

  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;

  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;

  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);

  Serial.println("configured firebase");

  // Initialize the screen
  if (!u8g2.begin()) {
    Serial.println("OLED initialization failed!");
    while (1);
  }
  u8g2.clearBuffer();

  //Setting up the font, since its the same all across
  u8g2.setFont(u8g2_font_5x7_tr);

  //Initialize the reading ambient task
  xTaskCreatePinnedToCore(
    readAmbient,    //name of the function
    "readAmbient",  //name
    10000,          //size
    NULL,     
    1,              //priority
    &readDHTTask,   //Handle
    0               //core
  );
  xTaskCreatePinnedToCore(
    displayData,  //name of the function
    "displayData",//name
    10000,        //size
    NULL,
    2,            //priority
    &displayDataTask,//Handle
    0             //core
  );
  xTaskCreatePinnedToCore(
    sendFirebase,  //name of the function
    "sendFirebase",//name
    10000,        //size
    NULL,
    3,            //priority
    &sendFirebaseTask,//Handle
    0             //core
  );
  
}

void loop() {
    
}