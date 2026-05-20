#include <Arduino.h>
#include <Wire.h>
#include <U8g2lib.h>
#include "DHT.h"
#include <Firebase_ESP_Client.h>

void readAmbient(void *parameter);
void displayData(void *parameter);
void displayData(void *parameter);
void sendFirebase(void *parameter);

extern QueueHandle_t humidityHandleSend = NULL;
extern QueueHandle_t temperatureHandleSend = NULL;
extern QueueHandle_t humidityHandleDB = NULL;
extern QueueHandle_t temperatureHandleDB = NULL;

extern U8G2_SSD1306_64X48_ER_F_HW_I2C u8g2;
extern DHT dht;
extern FirebaseData fbdo;