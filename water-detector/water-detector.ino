#include <MKRGSM.h>
 
const char PINNUMBER[] = "4821";

char PHONENUMBER1[] = "6507146242";

char PHONENUMBER2[] = "1234567890";
 
GSM gsmAccess;

GSM_SMS sms;
 
int liquidLevel = 0;

bool sendWaterDetectedSMS = true;

unsigned long stopCommandTime = 0;

const unsigned long stopDuration = 86400000; // 24 hours in milliseconds
 
// Function to initialize the setup of the Arduino

void setup() {

  Serial.begin(9600);

  pinMode(5, INPUT);
 
  bool connected = false;
 
  // Loop until the GSM module is successfully initialized

  while (!connected) {

    if (gsmAccess.begin(PINNUMBER) == GSM_READY) {

      connected = true;

    } else {

      Serial.println("GSM initialization failed. Retrying...");

      delay(1000);

    }

  }
 
  Serial.println("GSM module initialized successfully.");

}
 
// Function to send SMS to specified phone numbers

void sendsms(const char* message) {

  sms.beginSMS(PHONENUMBER1);

  sms.print(message);

  sms.endSMS();
 
  sms.beginSMS(PHONENUMBER2);

  sms.print(message);

  sms.endSMS();

}
 
// Function to process incoming SMS messages

void processIncomingSMS() {

  if (sms.available()) {

    char smsContent[160];

    int index = 0;
 
    // Read the incoming SMS content character by character

    while (sms.available()) {

      char c = sms.read();

      if (c != -1 && index < sizeof(smsContent) - 1) {

        smsContent[index] = c;

        index++;

      }

    }

    smsContent[index] = '\0';
 
    Serial.println("Received SMS:");

    Serial.println(smsContent);
 
    // Check if the SMS contains the stop command

    if (strstr(smsContent, "STFU") || strstr(smsContent, "stfu") || strstr(smsContent, "Stfu")) {

      stopCommandTime = millis();

      sendWaterDetectedSMS = false;

      Serial.println("Stop command received. Notifications disabled for 24 hours.");

    }

  }

}
 
// Main loop of the Arduino program

void loop() {

  unsigned long currentMillis = millis();

  liquidLevel = digitalRead(5);
 
  Serial.print("Liquid Level: ");

  Serial.println(liquidLevel);
 
  // If water is detected and notifications are enabled, send an SMS

  if (liquidLevel == 1 && sendWaterDetectedSMS) {

    sendsms("Water detected. Reply STFU to this message to disable notifications for 24 hours.");

    sendWaterDetectedSMS = false;

    Serial.println("Water detected. SMS sent.");

  }
 
  // Process incoming SMS messages

  processIncomingSMS();
 
  // Re-enable notifications after the stop duration has passed

  if (!sendWaterDetectedSMS && (currentMillis - stopCommandTime >= stopDuration)) {

    sendWaterDetectedSMS = true;

    Serial.println("Notifications enabled after stop duration.");

  }
 
  // Delay for 20 seconds

  delay(20000);

}
