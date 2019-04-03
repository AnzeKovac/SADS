#include <SPI.h>
//Library MFRC522 should be installed first
#include <MFRC522.h>
MFRC522 rfid(5, 4); // Instance of the class
 
void setup() { 
  Serial.begin(115200);
  SPI.begin(); // Init SPI bus
  rfid.PCD_Init(); // Init MFRC522 
}
 
void loop() {
 
  // Look for tags
  if (!rfid.PICC_IsNewCardPresent())
    return;
 
  // Check if serial read can bo done from tag
  if (!rfid.PICC_ReadCardSerial())
    return;
 
  Serial.println("Tag detected do something in code");
 
  // Halt PICC
  rfid.PICC_HaltA();
 
  // Stop encryption on PCD
  rfid.PCD_StopCrypto1();
}
 
