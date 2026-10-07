#include <Arduino.h>
#include <SPI.h>
#include <MFRC522.h>
#include <SoftwareSerial.h>
#include <DFRobotDFPlayerMini.h>

// Pin Definitions for ESP32
#define RST_PIN   22   // Configurable, see typical pin layout above
#define SS_PIN    21   // Configurable, see typical pin layout above

#define BUTTON_BISMILLAH 32  // Example pin for Bismillah button
#define BUTTON_ANSWERS   33  // Example pin for Answers button

// Setup RFID and Audio
MFRC522 mfrc522(SS_PIN, RST_PIN);
SoftwareSerial mySoftwareSerial(16, 17); // RX, TX for DFPlayer Mini
DFRobotDFPlayerMini myDFPlayer;

void setup() {
  Serial.begin(115200);
  while (!Serial);

  // Initialize SPI bus and RFID reader
  SPI.begin();
  mfrc522.PCD_Init();
  Serial.println(F("RFID Reader Initialized."));

  // Initialize SoftwareSerial and DFPlayer Mini
  mySoftwareSerial.begin(9600);
  if (!myDFPlayer.begin(mySoftwareSerial)) {
    Serial.println(F("Unable to begin DFPlayer: Please check connection and SD card!"));
  } else {
    Serial.println(F("DFPlayer Mini online."));
    myDFPlayer.volume(20);  // Set volume value (0~30)
  }

  // Initialize Buttons with internal pull-ups
  pinMode(BUTTON_BISMILLAH, INPUT_PULLUP);
  pinMode(BUTTON_ANSWERS, INPUT_PULLUP);
}

void loop() {
  // Check if a physical button is pressed
  if (digitalRead(BUTTON_BISMILLAH) == LOW) {
    Serial.println(F("Bismillah Button Pressed!"));
    myDFPlayer.play(1); // Play track 001.mp3
    delay(500); // Debounce delay
  }

  if (digitalRead(BUTTON_ANSWERS) == LOW) {
    Serial.println(F("Answers Button Pressed!"));
    myDFPlayer.play(2); // Play track 002.mp3
    delay(500); // Debounce delay
  }

  // Look for new RFID cards/tokens
  if (!mfrc522.PICC_IsNewCardPresent()) {
    return;
  }

  // Select one of the cards
  if (!mfrc522.PICC_ReadCardSerial()) {
    return;
  }

  // Read UID and print to Serial Monitor
  Serial.print(F("Token UID: "));
  String tagID = "";
  for (byte i = 0; i < mfrc522.uid.size; i++) {
    tagID.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
    tagID.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  tagID.toUpperCase();
  Serial.println(tagID);

  // Example Logic: Map specific token UIDs to audio/visual themes
  if (tagID.substring(1) == "A1 B2 C3 D4") { // Replace with your actual token UID later
    Serial.println(F("Mosque Token Detected: Playing Du'as"));
    myDFPlayer.play(3); // Play track 003.mp3
  } 
  else {
    Serial.println(F("Unknown Token"));
    myDFPlayer.play(99); // Play error/default track if needed
  }

  // Halt PICC
  mfrc522.PICC_HaltA();
  mfrc522.PCD_StopCrypto1();
}