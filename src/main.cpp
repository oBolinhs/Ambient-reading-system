#include <Arduino.h>
#include <Wire.h>
#include <U8g2lib.h>
#include "DHT.h"
#include <Firebase_ESP_Client.h>

#define DHTPIN 17     // Digital pin connected to the DHT sensor
#define DHTTYPE DHT11   // DHT 22  (AM2302), AM2321

#define API_KEY "AIzaSyDR8ACcwzVig4FV_UKU-ZqUbaBMMpgfvFU"
#define DATABASE_URL "esp-projects-44cdb-default-rtdb.europe-west1.firebasedatabase.app"

#define USER_EMAIL "esp32.ambient@gmail.com"
#define USER_PASSWORD "dht_123456"

const char* ssid = "Burger King - Guest";
const char* password = "VinhoVerde";

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
void readAmbient(void *parameter) {

  //Declaring the used variables
  float humiditySend = 0.0f;
  float tempCelsSend = 0.0f;
  float lastHumidity = 0.0f;
  float lastTemp = 0.0f;
  while(true)
  {
    //Read the values for humidity and temperature
    humiditySend = dht.readHumidity();
    tempCelsSend = dht.readTemperature(); 

    //If the value remains unchanged, no need to update the OLED screen
    if(humiditySend != lastHumidity ){
      xQueueSend(humidityHandleSend, &humiditySend, portMAX_DELAY); //send the variable address via queue
      xQueueSend(humidityHandleDB, &humiditySend, portMAX_DELAY);
      lastHumidity = humiditySend;    //update the last value
    }
    if(tempCelsSend != lastTemp){
      xQueueSend(temperatureHandleSend, &tempCelsSend, portMAX_DELAY);  //send the variable address via queue
      xQueueSend(temperatureHandleDB, &tempCelsSend, portMAX_DELAY);
      lastTemp = tempCelsSend;      //update the last value
    }
    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}

//função da segunda task
void displayData(void *parameter) {

  //Declaring the internal variables
  float humidityReceive = 0.0f;
  float tempReceive = 0.0f;

  while(true){

    //if its received from the humidity
    if(xQueueReceive(humidityHandleSend, &humidityReceive, portMAX_DELAY)){
      //Section for printing on the OLED
      u8g2.drawStr(0,10,"Humidity: ");
      String hum = String(humidityReceive,1);
      u8g2.drawStr(0,17, hum.c_str());
      u8g2.sendBuffer();
    }
    //if its recevied from the temperature
    if(xQueueReceive(temperatureHandleSend, &tempReceive, portMAX_DELAY)){
      //Section for printing on the OLED
      u8g2.drawStr(0,30,"Temperature: ");
      String temp = String(tempReceive,1);
      u8g2.drawStr(0,37, temp.c_str());
      u8g2.sendBuffer();
    }
    vTaskDelay(100 / portTICK_PERIOD_MS);
  }

}

void sendFirebase(void *parameter) {
  float humidityDB = 0.0f;
  float tempDB = 0.0f;
  while(true) {
    //Sending the data to the firebase
    if(xQueueReceive(humidityHandleDB, &humidityDB, portMAX_DELAY)){
      Firebase.RTDB.setFloat(&fbdo, "/bedroom/humidity", humidityDB);
    }
    if(xQueueReceive(temperatureHandleDB, &tempDB, portMAX_DELAY)){
      Firebase.RTDB.setFloat(&fbdo, "/bedroom/temperature", tempDB);    
    }
    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}

void setup() {
  Serial.begin(115200);
  Serial.println(F("DHT11 test!"));

  //Initialize the DHT
  dht.begin();

  //Configure wifi
  WiFi.begin(ssid, password);
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