#include <SPI.h>
#include <MFRC522.h>
#include "Arduino.h"
#include "accesscard.h"
#define RST_PIN 9
#define SS_PIN 10
#define MAXIMUM_AMOUNT_OF_CARDS 30
#define DELIMITER ","
#define TERMINATOR '\n' // for testing commands with serial monitor
#define ENDOFTAG ">"
MFRC522 mfrc522(SS_PIN, RST_PIN);  // Create MFRC522 instance

// Protocol constants
const char CLEARALL[] = "<CLEARALL>";
const char ADDCARD[] = "<ADDCARD>"; 
const char REMOTEOPEN[] = "<REMOTEOPEN>";
const char LISTCARDS[] = "<LISTALL>";

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

void ReadRFIDSensor(CARD_INVENTORY* card_inventory_state){
  if(mfrc522.PICC_IsNewCardPresent()) {
    unsigned long uid = GetCardIDAsHexaDecimal();
  
    if(uid != -1){
      sprintf(card_id_buffer, "%u", uid);
      Serial.println(card_id_buffer);
      digitalWrite(8, HIGH);
      delay(20);
      digitalWrite(8, LOW);
    }
    if (AccessCardExists(card_inventory_state, card_id_buffer)) {
      RemoteOpenDoor();
    }
  }
}

void ReadSerialBus(CARD_INVENTORY* card_inventory_state, char* buf, int max_size) {

  char parsed_character;
  int bytes_available;

  bytes_available = Serial.available();
  
  for (int i = 0; i < bytes_available; i++) {
      parsed_character = Serial.read();
      if (parsed_character == TERMINATOR) {
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
  Serial.println("--- [SERIAL]: SENDING UNLOCK COMMAND...");
  Serial.println("--- [SERIAL]: DOOR IS UNLOCKED.");    
}

void RemoteLockDoor() {
  Serial.println("--- [SERIAL]: SENDING LOCK COMMAND...");
  Serial.println("--- [SERIAL]: DOOR IS LOCKED.");
  digitalWrite(8, LOW);
}

void AddAccessCard(CARD_INVENTORY* card_inventory_state, char* serial_feed) {

  static int COUNTER = 1;
  short new_card_index;
  bool card_exists;
  char* token_ptr;
  char new_card_number[100];
  
  new_card_index = GetIndexForNewAccessCard(card_inventory_state);

  token_ptr = strtok(serial_feed, ENDOFTAG);
  token_ptr = strtok(NULL, ENDOFTAG);

  strcpy(new_card_number, token_ptr);
  
  strcpy(card_inventory_state->array_of_access_cards[new_card_index].number, new_card_number);

  if (AccessCardExists(card_inventory_state, new_card_number)) {
    for (byte i = 0; i < 5; i++) {
      digitalWrite(8, HIGH);
      delay(40);
      digitalWrite(8, LOW);
      delay(40);
    }
    Serial.println("OK");
  } else {
    Serial.println("ERROR");
  }
}

void PurgeCardInventory(CARD_INVENTORY* card_inventory_state) {
  free(card_inventory_state->array_of_access_cards);
  card_inventory_state->number_of_access_cards = 0;
}

void ListAllAccessCards(CARD_INVENTORY* card_inventory_state) {  

  for (int i = 0; i < card_inventory_state->number_of_access_cards; i++) {
    Serial.println(card_inventory_state->array_of_access_cards[i].number);
  }
  Serial.print('>');
}

void main_loop(CARD_INVENTORY* card_inventory_state) {

  while (true) {
    
    if (millis() % 250 == 0) {
      memset(serial_feed, 0, sizeof(serial_feed));
      
      ReadRFIDSensor(card_inventory_state);
      ReadSerialBus(card_inventory_state, serial_feed, sizeof(serial_feed));    
        
      if (suspend && millis() >= suspend_endtime) {
        RemoteLockDoor();
        suspend = false;
      }
      
      if (serial_feed[0] != 0) {
        if (strcmp(serial_feed, REMOTEOPEN) == 0) {
          RemoteOpenDoor();
        } else if (strncmp(serial_feed, ADDCARD, 9) == 0) {
          AddAccessCard(card_inventory_state, serial_feed);
        } else if (strcmp(serial_feed, LISTCARDS) == 0) {
          ListAllAccessCards(card_inventory_state);
        } else if (strcmp(serial_feed, CLEARALL) == 0) {
          PurgeCardInventory(card_inventory_state);
        }
      }
    }
  }
}
       
void loop() {
  CARD_INVENTORY card_inventory_state;
  card_inventory_state.array_of_access_cards = NULL;
  card_inventory_state.number_of_access_cards = 0;
    
  main_loop(&card_inventory_state);
}
