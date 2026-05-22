# Ambient reading system

The ambient reading is a personal project developed to expand my skills utilizing PlatformIO, programming ESP32 and using FreeRTOS. Developed to read the temperature and humidity of a given room, it uploads these values into a Firebase Database and displays them on an OLED screen.

## Implemented Hardware

The project makes use of **DHT11 module** to read ambient temperature and humidity, a **0.66in OLED** 64x48 to display data and an **ESP32 Dev Module** as the processing unit.

## Implemented Software

It is divided into 2 files, the tasks.cpp/h where it is declared the task functions and the main.cpp where the setup is, where we initialize the tasks
