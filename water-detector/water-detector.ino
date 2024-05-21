#include <MKRGSM.h>

//PIN number
const char PINNUMBER[] = "4821";
//Phone number
char PHONENUMBER[] = "7077429889";

GSM gsmAccess;
GSM_SMS sms;

int liquidLevel = 0;

void setup()
{
  Serial.begin(9600);
  pinMode(5, INPUT);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  
  bool connected = false;
  
  while (!connected) {
    if (gsmAccess.begin(PINNUMBER) == GSM_READY) {
      connected = true;
    } else {
      Serial.println("Not connected");
      delay(1000);
    }
  }

  Serial.println("GSM initialized");
}

void sendsms()
{
  sms.beginSMS(PHONENUMBER);
  sms.print("Water detected. Reply to this message to disable notifications for 24 hours.");
  sms.endSMS();
  Serial.println("\nCOMPLETE!\n");
  delay(20000); //Minimum delay between texts
}

void stopsms()
{
  Serial.println("\nStopped");
  sms.beginSMS(PHONENUMBER);
  sms.print("Notifications disabled for 24 hours.");
  sms.endSMS();
  delay(20000); //Delay if you text the number back
}

void loop()
{
  liquidLevel = digitalRead(5);
  Serial.print("liquidLevel= "); Serial.println(liquidLevel, DEC);
  delay(1000);

  if(liquidLevel == 1) {
    sendsms();
  }

  if (sms.available()) {
    stopsms();
  }
}

/*
  Removing the below code breaks it though I'm not sure why.
*/

int readSerial(char result[]) {
  int i = 0;
  while (1) {
    while (Serial.available() > 0) {
      char inChar = Serial.read();
      if (inChar == '\n') {
        result[i] = '\0';
        Serial.flush();
        return 0;
      }
      if (inChar != '\r') {
        result[i] = inChar;
        i++;
      }
    }
  }
}
