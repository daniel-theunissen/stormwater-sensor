// You should really only be changing the following values
// -------------------------------------------------------

// Set a recipient phone number (it must be in international format including the "+" sign)
#define SMS_TARGET ""

// Set the active cooldown time (how long it sleeps after you text "STFU")
#define DISABLE_TIME 86400 // 24 hours in seconds
//#define DISABLE_TIME 60 // 60 seconds

// Set the passive cooldown time (how long it takes to send another text after it detects water)
#define SLEEP_TIME 30000   // 30 seconds in milliseconds

// Enable or disable cooldowns
#define ENABLE_COOLDOWNS true

// Uncomment to see all AT commands
//#define DUMP_AT_COMMANDS

// Set DATA PIN for detector
#define DATA_PIN 5
// -------------------------------------------------------



//If you are having issues with the network or swap the SIM card, edit the following
// -------------------------------------------------------

// set GSM PIN, if any
#define GSM_PIN ""

// GPRS credentials, if any
const char apn[]  = "";     //SET TO APN
const char gprsUser[] = "";
const char gprsPass[] = "";
// -------------------------------------------------------