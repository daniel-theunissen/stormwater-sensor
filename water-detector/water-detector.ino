#define TINY_GSM_MODEM_SIM7000
#define TINY_GSM_RX_BUFFER 1024 // Set RX buffer to 1Kb
#define SerialAT Serial1

#include <TinyGsmClient.h>
#include <SPI.h>
#include <Ticker.h>
#include "config.h"

#ifdef DUMP_AT_COMMANDS  // if enabled it requires the streamDebugger lib
  #include <StreamDebugger.h>
  StreamDebugger debugger(SerialAT, Serial);
  TinyGsm modem(debugger);
#else
  TinyGsm modem(SerialAT);
#endif

#define uS_TO_S_FACTOR 1000000ULL  // Conversion factor for micro seconds to seconds

// Define pins for serial communication with the modem
#define UART_BAUD   115200
#define PIN_DTR     25
#define PIN_TX      27
#define PIN_RX      26
#define PWR_PIN     4
#define LED_PIN     12

// Initialize/Declare variables
unsigned int tStart;
bool onCooldown = false;
int liquidLevel = 0;
String message;
int counter, lastIndex, numberOfPieces = 24;
String pieces[24], input;

// Everything in setup is loosely adapted from https://github.com/Xinyuan-LilyGO/LilyGO-T-SIM7000G/blob/master/examples/Arduino_NetworkTest/Arduino_NetworkTest.ino
// and https://github.com/damianjwilliams/tsim7070g/blob/main/stationary_display/stationary_display.ino
void setup() {
  // Set console baud rate
  Serial.begin(UART_BAUD);
  delay(10);

  // Set sensor pin to input
  pinMode(DATA_PIN, INPUT);

  // Set LED OFF
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  // Turn on the modem
  pinMode(PWR_PIN, OUTPUT);
  digitalWrite(PWR_PIN, HIGH);
  delay(300);
  digitalWrite(PWR_PIN, LOW);

  Serial.println("\nWait...");

  delay(1000);

  SerialAT.begin(UART_BAUD, SERIAL_8N1, PIN_RX, PIN_TX);

  // Restart modem (can take a while)
  Serial.println("Initializing modem...");
  if (!modem.restart()) {
    Serial.println("Failed to restart modem, attempting to continue without restarting");
  }

  // Initialize modem
  Serial.println("Initializing modem...");
  if (!modem.init()) {
    Serial.println("Failed to initialize modem");
  }

  String name = modem.getModemName();
  delay(500);
  Serial.println("Modem Name: " + name);

  String modemInfo = modem.getModemInfo();
  delay(500);
  Serial.println("Modem Info: " + modemInfo);
  
  // Unlock SIM card with a PIN if needed
  if ( GSM_PIN && modem.getSimStatus() != 3 ) {
      modem.simUnlock(GSM_PIN);
      Serial.println("Sim unlocked");
  }
  modem.sendAT("+CFUN=0 ");
  if (modem.waitResponse(10000L) != 1) {
    DBG(" +CFUN=0  false ");
  }
  delay(200);

  /*
    2 Automatic
    13 GSM only
    38 LTE only
    51 GSM and LTE only
  * * * */
  String res;
  // CHANGE NETWORK MODE, IF NEEDED
  res = modem.setNetworkMode(2);
  if (res != "1") {
    DBG("setNetworkMode  false ");
    return ;
  }
  delay(200);

  /*
    1 CAT-M
    2 NB-Iot
    3 CAT-M and NB-IoT
  * * */
  // CHANGE PREFERRED MODE, IF NEEDED
  res = modem.setPreferredMode(3);
  if (res != "1") {
    DBG("setPreferredMode  false ");
    return ;
  }
  delay(200);

  // Sets modem to full functionality (receive, send, LTE)
  modem.sendAT("+CFUN=1 ");
  if (modem.waitResponse(10000L) != 1) {
    DBG(" +CFUN=1  false ");
  }
  delay(200);

  SerialAT.println("AT+CGDCONT?");
  
  delay(500);
  
  // Defines Packet Data Protocol (PDP) context
  if (SerialAT.available()) {
    input = SerialAT.readString();
    for (int i = 0; i < input.length(); i++) {
      if (input.substring(i, i + 1) == "\n") {
        pieces[counter] = input.substring(lastIndex, i);
        lastIndex = i + 1;
        counter++;
       }
        if (i == input.length() - 1) {
          pieces[counter] = input.substring(lastIndex, i);
        }
      }
      // Reset for reuse
      input = "";
      counter = 0;
      lastIndex = 0;

      for ( int y = 0; y < numberOfPieces; y++) {
        for ( int x = 0; x < pieces[y].length(); x++) {
          char c = pieces[y][x];  //gets one byte from buffer
          if (c == ',') {
            if (input.indexOf(": ") >= 0) {
              String data = input.substring((input.indexOf(": ") + 1));
              if ( data.toInt() > 0 && data.toInt() < 25) {
                modem.sendAT("+CGDCONT=" + String(data.toInt()) + ",\"IP\",\"" + String(apn) + "\",\"0.0.0.0\",0,0,0,0");
              }
              input = "";
              break;
            }
          // Reset for reuse
          input = "";
         } else {
          input += c;
         }
      }
    }
  } else {
    Serial.println("Failed to get PDP!");
  }

  Serial.println("\n\n\nWaiting for network...");
  if (!modem.waitForNetwork()) {
    delay(10000);
    return;
  }

  if (modem.isNetworkConnected()) {
    Serial.println("Network connected");
  }

  // --------TESTING GPRS--------
  
  Serial.println("\n---Starting GPRS TEST---\n");
  Serial.println("Connecting to: " + String(apn));
  modem.gprsConnect(apn, gprsUser, gprsPass);
  if (!modem.gprsConnect(apn, gprsUser, gprsPass)) {
    Serial.println("fail");
    delay(10000);
    return;
  }

  Serial.print("GPRS status: ");
  if (modem.isGprsConnected()) {
    Serial.println("connected");
  } else {
    Serial.println("not connected");
  }

  String ccid = modem.getSimCCID();
  Serial.println("CCID: " + ccid);

  String imei = modem.getIMEI();
  Serial.println("IMEI: " + imei);

  String cop = modem.getOperator();
  Serial.println("Operator: " + cop);

  IPAddress local = modem.localIP();
  Serial.println("Local IP: " + String(local));

  int csq = modem.getSignalQuality();
  Serial.println("Signal quality: " + String(csq));

  SerialAT.println("AT+CPSI?");     //Get connection type and band
  delay(500);
  if (SerialAT.available()) {
    String r = SerialAT.readString();
    Serial.println(r);
  }

  modem.gprsDisconnect();
  if (!modem.isGprsConnected()) {
    Serial.println("GPRS disconnected");
  } else {
    Serial.println("GPRS disconnect: Failed.");
  }
  
  modem.disableGPS();

  // Set SIM7000G GPIO4 LOW ,turn off GPS power
  // CMD:AT+SGPIO=0,4,1,0
  // Only in version 20200415 is there a function to control GPS power
  modem.sendAT("+SGPIO=0,4,1,0");
  if (modem.waitResponse(10000L) != 1) {
    DBG(" SGPIO=0,4,1,0 false ");
  }
  Serial.println("\n---End of GPRS TEST---\n");

  modem.sendAT("+CMGF=1");                                                       // Set the message format to text mode
  modem.waitResponse(1000L);
  modem.sendAT("+CNMI=2,2,0,0,0\r");
  delay(100);
  //modem.sendAT("+CMGF=0");                                                       // Set the message format to PDU mode
  
}
 
// Function to send SMS to specified phone number

void sendSMS(const char* message) {
  String res;
  res = modem.sendSMS(SMS_TARGET, String(message));
  DBG("SMS:", res ? "OK" : "fail");
}
 
// Function to process incoming SMS messages
// Adapted from https://github.com/damianjwilliams/tsim7070g/blob/main/stationary_display/stationary_display.ino

String readSMS() { 
  
  String message;
  modem.sendAT("+CMGL=\"ALL\"");  // List all stored messages
  modem.waitResponse(5000L);
  String data = SerialAT.readString();
  
  // Check for received messages
  if(data.indexOf("+CMT:") > 0){
    message = parseSMS(data);
    Serial.print("The message in the readSMS function is: ");
    Serial.println(message);
    return message; 
  }     
}


String parseSMS(String data) {

  data.replace(",,", ",");
  data.replace("\r", ",");
  data.replace("\"", "");
  //Serial.println(data);

  String for_mess = data;

  char delimiter = ',';
  String date_str =  parse_SMS_by_delim(for_mess, delimiter,2);
  String time_str =  parse_SMS_by_delim(for_mess, delimiter,3);
  String message_str =  parse_SMS_by_delim(for_mess, delimiter,4);

  /*
  Serial.println("************************");
  Serial.println(date_str);
  Serial.println(time_str);
  Serial.println(message_str);
  Serial.println("************************");
  */
  
  return message_str;

}

String parse_SMS_by_delim(String sms, char delimiter, int targetIndex) {
  // Tokenize the SMS content using the specified delimiter
  int delimiterIndex = sms.indexOf(delimiter);
  int currentIndex = 0;

  while (delimiterIndex != -1) {
    if (currentIndex == targetIndex) {
    
    String targetToken = sms.substring(0, delimiterIndex);
    targetToken.replace("\"", "");
    targetToken.replace("\r", "");
    targetToken.replace("\n", "");
    return targetToken;
    }

    // Move to the next token
    sms = sms.substring(delimiterIndex + 1);
    delimiterIndex = sms.indexOf(delimiter);
    currentIndex++;
  }

  // If the target token is not found, return an empty string
  return "";
}
 

void loop() {
 
 // If the device is not on cooldown, check for liquid
  if (!onCooldown) {
    liquidLevel = digitalRead(DATA_PIN);
    Serial.print("Liquid Level: ");
    Serial.println(liquidLevel);

    // If water is detected, send a notification
    if (liquidLevel == 1) {
      Serial.println("Water detected. SMS sent.");
      sendSMS("Water detected. Reply STFU to this message to disable notifications for 24 hours. A confirmation text will be sent");

      #if ENABLE_COOLDOWNS
        // Start cooldown timer
        tStart = millis();
        onCooldown = true;
        Serial.print("A 30s cooldown has just started. Start time: ");
        Serial.println(tStart);
      #endif

      delay(200);
    }
  }
  
  message = readSMS();

  if (message == "STFU") {
    Serial.println("************************");
    Serial.println("STOPPED");
    Serial.println("************************");

    sendSMS("Notifications disabled.");

    #if ENABLE_COOLDOWNS
      Serial.print("Disabled for specified time.");  

      // Puts ESP32 into deep sleep mode for specified time
      esp_sleep_enable_timer_wakeup(DISABLE_TIME * uS_TO_S_FACTOR);
      delay(200);
      esp_deep_sleep_start();
    #endif
  }
  
  // Checks the elapsed time since cooldown started
  if(((millis()-tStart) > SLEEP_TIME) && onCooldown){
    Serial.print("The current time is: ");
    Serial.println(millis());
    Serial.print("The difference is: ");
    Serial.println(millis()-tStart);
    Serial.println("resetting cooldown");
    onCooldown = false;
  }
}
