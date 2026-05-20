#include <Arduino.h>
#include <Wire.h>
#include <U8g2lib.h>
#include "DHT.h"
#include <Firebase_ESP_Client.h>

//Declaring the functions used 
void readAmbient(void *parameter);
void displayData(void *parameter);
void displayData(void *parameter);
void sendFirebase(void *parameter);


//Declare as extern since they are declared in the main.cpp
extern QueueHandle_t humidityHandleSend;
extern QueueHandle_t temperatureHandleSend;
extern QueueHandle_t humidityHandleDB;
extern QueueHandle_t temperatureHandleDB;

extern U8G2_SSD1306_64X48_ER_F_HW_I2C u8g2;
extern DHT dht;
extern FirebaseData fbdo;