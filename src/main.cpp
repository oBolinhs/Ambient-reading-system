#include <Arduino.h>
#include <Wire.h>
#include <U8g2lib.h>
#include "DHT.h"

#define DHTPIN 17     // Digital pin connected to the DHT sensor
#define DHTTYPE DHT11   // DHT 22  (AM2302), AM2321

DHT dht(DHTPIN, DHTTYPE);


QueueHandle_t humidityHandleSend = NULL;
QueueHandle_t temperatureHandleSend = NULL;

TaskHandle_t readDHTTask = NULL;
TaskHandle_t displayDataTask = NULL;

U8G2_SSD1306_64X48_ER_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);

//função da primeira task
void readAmbient(void *parameter) {
  float humiditySend = 0.0f;
  float tempCelsSend = 0.0f;
  float lastHumidity = 0.0f;
  float lastTemp = 0.0f;
  while(true)
  {
    //Read the values for humidity and temperature
    humiditySend = dht.readHumidity();
    tempCelsSend = dht.readTemperature(); 
    if(humiditySend != lastHumidity ){
      xQueueSend(humidityHandleSend, &humiditySend, portMAX_DELAY);
      lastHumidity = humiditySend;
    }
    if(tempCelsSend != lastTemp){
      xQueueSend(temperatureHandleSend, &tempCelsSend, portMAX_DELAY);
      lastTemp = tempCelsSend;
    }
    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}

//função da segunda task
void displayData(void *parameter) {
  float humidityReceive = 0.0f;
  float tempReceive = 0.0f;

  while(true){
    if(xQueueReceive(humidityHandleSend, &humidityReceive, portMAX_DELAY)){
      u8g2.drawStr(0,10,"Humidity: ");
      String hum = String(humidityReceive,1);
      u8g2.drawStr(0,17, hum.c_str());
      u8g2.sendBuffer();
    }
    if(xQueueReceive(temperatureHandleSend, &tempReceive, portMAX_DELAY)){
      u8g2.drawStr(0,30,"Temperature: ");
      String temp = String(tempReceive,1);
      u8g2.drawStr(0,37, temp.c_str());
      u8g2.sendBuffer();
    }
    vTaskDelay(100 / portTICK_PERIOD_MS);
  }

}

void setup() {
  Serial.begin(115200);
  Serial.println(F("DHT11 test!"));

  dht.begin();
  humidityHandleSend = xQueueCreate(5, sizeof(float));
  temperatureHandleSend = xQueueCreate(5,sizeof(float));

  // Initialize the screen
  if (!u8g2.begin()) {
    Serial.println("OLED initialization failed!");
    while (1);
  }
  u8g2.clearBuffer();

  u8g2.setFont(u8g2_font_5x7_tr);


  xTaskCreatePinnedToCore(
    readAmbient,
    "readAmbient",
    10000,
    NULL,
    1,
    &readDHTTask,
    0
  );
  xTaskCreatePinnedToCore(
    displayData,
    "displayData",
    10000,
    NULL,
    2,
    &displayDataTask,
    0
  );
}

void loop() {
    
}