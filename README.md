# Stormwater Detector

This repository contains the script for a sensing device that detects if water comes into contact with a specific area and notifies someone using SMS. 

## How To Use
At the top of the script, there are several important fields to modify depending on the setup:
- Set the target phone number by changing the `SMS_TARGET` variable
- Set the length of time that the sensor is disabled after sending it a stop command by changing the `DISABLE_NOTIFICATIONS` variable
- Set the passive cooldown (essentially the polling rate) by changing the `period` variable
- Input your SIM card pin number and the APN settings of the mobile carrier it is registered to by changing the `GSM_PIN`, `apn`, `gprsUser`, and `gprsPass` variables

Some additional debugging features that might be helpful include:
- Disable the notification cooldown by setting `ENABLE_COOLDOWNS` to false
- Defining `DUMP_AT_COMMANDS` will show all the AT commands in the serial monitor

## Requirements
- ESP32 Board with a SIM7000 or SIM800 series modem (This project used a Lilygo T-SIM7000G)
- Photoelectric liquid level sensor that outputs a logical "HIGH" in the presence of water (This project used a FS-IR02) 
- Activated sim card
