#include "tasks.h"
#include <Arduino.h>

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
    //Sending the data to the firebase, needed a second queue
    if(xQueueReceive(humidityHandleDB, &humidityDB, portMAX_DELAY)){
      Firebase.RTDB.setFloat(&fbdo, "/bedroom/humidity", humidityDB);   
    }
    if(xQueueReceive(temperatureHandleDB, &tempDB, portMAX_DELAY)){
      Firebase.RTDB.setFloat(&fbdo, "/bedroom/temperature", tempDB);    
    }
    vTaskDelay(100 / portTICK_PERIOD_MS);
  }
}