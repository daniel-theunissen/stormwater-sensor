# Stormwater Detector

This repository contains the script for a sensing device that detects if water comes into contact with a specific area and notifies someone using SMS. 

## How To Use
Clone the project repository
```
git clone https://github.com/daniel-theunissen/stormwater-sensor.git
```
Edit the following in `config.h`:
- Set the target phone number by changing the `SMS_TARGET` variable
- Set the length of time that the sensor is disabled after sending it a stop command by changing the `DISABLE_TIME` variable
- Set the passive cooldown (essentially the polling rate) by changing the `SLEEP_TIME` variable
- Set the input pin for the liquid level sensor
- Input SIM card pin number and the APN settings of the mobile carrier it is registered to by changing the `GSM_PIN`, `apn`, `gprsUser`, and `gprsPass` variables

Some additional debugging features that might be helpful:
- Disable the notification cooldown by setting `ENABLE_COOLDOWNS` to false
- Defining `DUMP_AT_COMMANDS` will show all the AT commands in the serial monitor

Flash the program to microcontroller using Arduino IDE or similar.

## Requirements
### Libraries:
- TinyGSM
- Ticker
- StreamDebugger
### Hardware:
- ESP32 Board with a SIM7000 or SIM800 series modem (This project used a Lilygo T-SIM7000G)
- Photoelectric liquid level sensor that outputs a logical "HIGH" in the presence of water (This project used a FS-IR02) 
- Activated sim card

## Sources
A lot of this code is partially adapted from the following sources:
- https://github.com/Xinyuan-LilyGO/LilyGO-T-SIM7000G/blob/master/examples/Arduino_TinyGSM/AllFunctions/AllFunctions.ino
- https://github.com/damianjwilliams/tsim7070g/tree/main
- https://RandomNerdTutorials.com/lilygo-t-sim7000g-esp32-lte-gprs-gps/

