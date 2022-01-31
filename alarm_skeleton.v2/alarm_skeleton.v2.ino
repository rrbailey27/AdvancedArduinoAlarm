#include <Adafruit_PN532.h>

//Defines the pins for the RFID reader. Note that pins 2 & 3 are used by the shield and cannot be used for anything else.
Adafruit_PN532 nfc(2, 3);

//This will be the ID of the tag you will use to deactivate your system
//YOU'LL HAVE TO EDIT THESE FOR YOUR SPECIFIC TAG!!
byte targetID[] = {0x41, 0x01, 0x24, 0xFC};

//define the states in easy-to-read terms
enum {ALARM_OFF, ALARM_ARMED, ALARM_INTRUDER};
int currentState = ALARM_OFF; //start off with the alarm deactivated

//Some pin definitions. The RFID is taken care of for us.
const int buttonPin = 7;
const int photoPin  = A0;
const int alarmPin  = 11;
const int ledPin    = 13;

void setup(void) 
{
  Serial.begin(9600);
  Serial.println("Hello!");

  if(!SetupRFIDShield()) //sets up the RFID
  {
    Serial.print("Didn't find PN53x board");
    while(1) {}
  }

  pinMode(buttonPin, INPUT_PULLUP); //if we use INPUT_PULLUP, we don't have to have an external pullup resistor
  pinMode(photoPin, INPUT);
  pinMode(alarmPin, OUTPUT);
  pinMode(ledPin, OUTPUT);

  Serial.println("setup() complete");
}

void loop(void)
/*
 * Our loop() is just a set of checker-handler pairs.
 * A "checker" returns TRUE if something has happneed
 * A handler" does someting when the checker is TRUE
 */
{
  if(CheckArmingButton()) HandleArmingButton();
  if(CheckIfLaserBroken()) HandleLaserBroken();
  if(CheckForRFID()) HandleRFID();
}

/*
// ARMING BUTTON HANDLER
*/

void HandleArmingButton(void)
{
  if(currentState == ALARM_OFF)
  {
    Serial.println("Arming!");

    //take the action: light the LED
    digitalWrite(ledPin, HIGH);    
    
    currentState = ALARM_ARMED; //Don't forget to change the state!
  }
}

/*
// LASER BROKEN HANDLER
*/

void HandleLaserBroken(void)
{
  //YOUR CODE HERE!
}

/*
// RFID DETECTED HANDLER
*/
void HandleRFID(void)
{
  //YOUR CODE HERE!
}


/*
 * BUTTON CHECKER -- you don't need to edit this one
 */
int prevButtonState = HIGH; //button up => pin reads HIGH

bool CheckArmingButton(void)
{
  bool retVal = false;
  
  int currButtonState = digitalRead(buttonPin);
  if(prevButtonState != currButtonState)
  {
    delay(10); //this is a cheat for debouncing
    if(currButtonState == LOW) retVal = true;  //button is down => pin reads LOW
  }
  prevButtonState = currButtonState;

  return retVal;
}

/*
 * LASER CHECKER. YOU WILL NEED TO EDIT THIS ONE
 */
enum {DARK, LIGHT};
int prevLaserSensorState = DARK; //the program starts with the laser off

bool CheckIfLaserBroken(void)
{
  bool retVal = false;

  //add code here to detect the EVENT of the laser being broken. See the button checker above for hints
  
  return retVal;
}

/*
 * RFID CHECKER - DON'T TOUCH THIS FUNCTION OR ANYTHING BELOW IT -- it's all ready to go
 */
bool CheckForRFID(void) 
/*
 * The RFID is complicated enough that we've taken care of it for you. Note, however, that this checker
 * isn't true event-driven -- it will send success repeatedly if you hold a tag near it. Luckily, because
 * we've implememted a state machine, all of the extra successes will be ignored.
 */
{
  byte uid[] = { 0, 0, 0, 0, 0, 0, 0 };  // Buffer to store the returned UID
  byte uidLength;                        // Length of the UID (4 or 7 bytes depending on ISO14443A card type)
    
  // Wait for an ISO14443A type cards (Mifare, etc.).  When one is found
  // 'uid' will be populated with the UID, and uidLength will indicate
  // if the uid is 4 bytes (Mifare Classic) or 7 bytes (Mifare Ultralight)
  bool success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength, 40);
    
  if (success) 
  {
    Serial.println("Tag found!");
    if(uidLength == 4)
    {
      Serial.print(uid[0], HEX);
      Serial.print('\t');
      Serial.print(uid[1], HEX);
      Serial.print('\t');
      Serial.print(uid[2], HEX);
      Serial.print('\t');
      Serial.print(uid[3], HEX);
      Serial.print('\n');
      if(targetID[0] == uid[0] && targetID[1] == uid[1] && targetID[2] == uid[2] && targetID[3] == uid[3])
      {
        Serial.println("Match!");
        return true;
      }
    }
  }

  return false;
}

bool SetupRFIDShield(void)
{
  //start the RFID reader and check that it's alive
  nfc.begin(); 
  if (!nfc.getFirmwareVersion()) 
  {
    return false;
  }
  
  // configure board to read RFID tags
  nfc.SAMConfig();

  return true;
}
