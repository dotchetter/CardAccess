#include <MFRC522.h>
#include <SPI.h>
#include "Arduino.h"
#define RST_PIN 9
#define SS_PIN 10
#define DELIMITER ","
#define TERMINATOR '\n' // for testing commands with serial monitor
#define ENDOFTAG ">"
#define NOT_READY "!"
#define HEARTBEAT "?"

MFRC522 mfrc522(SS_PIN, RST_PIN);  // Create MFRC522 instance

// Protocol constants
const char CLEARALL[] = "<CLEARALL>";
const char REMOTEOPEN[] = "<REMOTEOPEN>";

bool suspend = false;
unsigned long start_ms;
unsigned long current_ms;
static unsigned long suspend_starttime;
static unsigned long suspend_endtime;
char serial_feed[200];
char card_id_buffer[200];

void setup() { 
  Serial.begin(9600);
  SPI.begin();
  mfrc522.PCD_Init(); 
  pinMode(8, OUTPUT);
  delay(4);
}

unsigned long GetCardIDAsHexaDecimal(){
  unsigned long card_in_hexadecimal;
  if (!mfrc522.PICC_ReadCardSerial()) {
    return;
  }
  card_in_hexadecimal =  mfrc522.uid.uidByte[0] << 24;
  card_in_hexadecimal += mfrc522.uid.uidByte[1] << 16;
  card_in_hexadecimal += mfrc522.uid.uidByte[2] <<  8;
  card_in_hexadecimal += mfrc522.uid.uidByte[3];
  mfrc522.PICC_HaltA();
  
  return card_in_hexadecimal;
}

void ReadRFIDSensor(){
  if(mfrc522.PICC_IsNewCardPresent()) {
    unsigned long uid = GetCardIDAsHexaDecimal();
  
    if(uid != -1){
      sprintf(card_id_buffer, "%u", uid);
      Serial.println(card_id_buffer);
      digitalWrite(8, HIGH);
      delay(20);
      digitalWrite(8, LOW);
    }
  }
}

void ReadSerialBus(char* buf, int max_size) {

  char parsed_character;
  int bytes_available;

  bytes_available = Serial.available();
  
  for (int i = 0; i < bytes_available; i++) {
      parsed_character = Serial.read();
      if (parsed_character == ENDOFTAG) {
        break;
      }
      buf[i] = parsed_character;
  }
}

void RemoteOpenDoor() {
  suspend = true;
  suspend_starttime = millis();
  suspend_endtime = (suspend_starttime + 3000);
  digitalWrite(8, HIGH);  
}

void RemoteLockDoor() {
  suspend = false;
  digitalWrite(8, LOW);
}

void loop() {

  if (millis() % 10 == 0) {
    if (suspend) {
      Serial.println(NOT_READY);
      digitalWrite(8, HIGH);
    } else {
      Serial.println(HEARTBEAT);
    }

    memset(serial_feed, 0, sizeof(serial_feed));
    ReadRFIDSensor();

  if (suspend && millis() >= suspend_endtime) {
    RemoteLockDoor();
  }

  ReadSerialBus(serial_feed, sizeof(serial_feed));    
      
  if (serial_feed[0] != 0 && strcmp(serial_feed, REMOTEOPEN) == 0) {
    RemoteOpenDoor();
    }
  }
}