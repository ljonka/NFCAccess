/*
    NFC/Rfid Permission checker with MFRC522, SDCard and Cloud Functions
    
    Copyright (C) 2015  Leonid Verhovskij<info@leonv.de>
   
    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 3 of the License, or
    (at your option) any later version.
    
    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.
    
    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software Foundation,
    Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
   
*/

#include "SparkJson.h" //json lib
#include "SdFat.h" //sd-card lib
#include "MFRC522.h" //rfid lib

//include random number generator from stm32 lib
#include "stm32f2xx_rng.h"
#include "stm32f2xx_rcc.h"

#define SS_PIN A1
#define RST_PIN D2
#define BUFFER_SIZE 18
#define SESSIONKEY_SIZE 4

// SOFTWARE SPI pin configuration - modify as required
// The default pins are the same as HARDWARE SPI
//VCC should be 5V, 3.3V was not suficient!
#define SD_CS A2    // Also used for HARDWARE SPI setup


MFRC522 mfrc522(SS_PIN, RST_PIN);        // Create MFRC522 instance.

SdFat SD(1);
File myFile;
MFRC522::MIFARE_Key key;

//make functions available
bool nfcScanCard();
bool nfcAuthKeys();
bool nfcCheckPiccType();
int cloudUpdateKey(String);
bool fileWriteLog(String);
int fileAuthUid(String);
void openDoor();
void checkTaster();

// In this sample we use the second sector (ie block 4-7). the first sector is = 0
byte sector         = 1;
// block sector 0-3(sector0) 4-7(sector1) 8-11(sector2)
byte valueBlockA    = 4;
byte trailerBlock   = 7;
byte status;
bool sdStatus = false;

String fileLog = "log.txt";
String weekDays[7] = {"sunday", "monday", "tuesday", "wednesday", "thursday", "friday", "saturday"};
int userAuthStatus = 0;

//timout var counter for door access
unsigned long previousMillis = 0;
const int doorPin =  D0;      // number of relais pin
const int tasterPin =  D1;      //number of taster pin - used to open door from inside
const long interval = 2000;     // interval how long door will be opened
bool openDoorCommand = false;

void setup() {
	Serial.begin(9600);        // Initialize serial communications with the PC
	mfrc522.setSPIConfig();

	mfrc522.PCD_Init();        // Init MFRC522 card
	
	// Initialize SOFTWARE SPI, save status of sd init for later
    if (!SD.begin(SD_CS)) {
        Serial.println("initialization failed!");
        sdStatus = false;	
    }else{
        sdStatus = true;
    }
    
    // Prepare key - all keys are set to FFFFFFFFFFFFh at chip delivery from the factory.
    for (byte i = 0; i < 6; i++) {
        key.keyByte[i] = 0xFF;
    }
    
    //enable random number generator
    RCC_AHB2PeriphClockCmd(RCC_AHB2Periph_RNG, ENABLE);
    //start random number generator
	RNG_Cmd(ENABLE);
    
    Particle.function("updateKey", cloudUpdateKey);
    
    Time.zone(+2);
    
    pinMode(doorPin, OUTPUT);
    pinMode(tasterPin, INPUT);
}
void loop() {
    openDoor();
    checkTaster();
    if(!nfcScanCard()){
        mfrc522.PICC_HaltA();
        //Serial.println("Scanning...");
        delay(200);
        return;
    }
    // Now a card is selected. The UID and SAK is in mfrc522.uid.
        
    // Dump UID
    Serial.print("Card UID:");
    String uid = "";
    for (byte i = 0; i < mfrc522.uid.size; i++) {
        //uid += mfrc522.uid.uidByte[i] < 0x10 ? " 0" : "_";
	uid += "_";
        uid += mfrc522.uid.uidByte[i], HEX;
    } 
    
    Serial.println(uid);

    if(!nfcCheckPiccType()) return;
    
    //check if sd card is present, otherwise cancel
    if(!sdStatus){
        Serial.println("sd card initialization failed!");
        mfrc522.PICC_HaltA();
        return;
    }
    
    fileWriteLog(uid);
               
    if(!nfcAuthKeys()) {
        mfrc522.PICC_HaltA();
        mfrc522.PCD_StopCrypto1();
        Serial.println("sector decryption failed!");
        //expose uid to cloud
        uid.concat("_0");
        Particle.publish("chip-uid", uid, 60, PRIVATE);
        return;
    }
    userAuthStatus = fileAuthUid(uid);
    if(userAuthStatus == 0){
        Serial.println("User not found.");
        uid.concat("_1");
        Particle.publish("chip-uid", uid, 60, PRIVATE);
    }
    else if(userAuthStatus == 1){
        Serial.println("User not authorized.");
        uid.concat("_2");
        Particle.publish("chip-uid", uid, 60, PRIVATE);
    }else if(userAuthStatus == 2){
        Serial.println("Welcome!");
        uid.concat("_3");
        Particle.publish("chip-uid", uid, 60, PRIVATE);
        previousMillis = millis();
        openDoorCommand = true;
        openDoor();
    }
    
    // Halt PICC
    mfrc522.PICC_HaltA();

    // Stop encryption on PCD
    mfrc522.PCD_StopCrypto1();
}
void openDoor(){
    if(openDoorCommand == false){
        digitalWrite(doorPin, LOW);
    }else{
        digitalWrite(doorPin, HIGH);
        
        unsigned long currentMillis = millis();
    
        if (currentMillis - previousMillis >= interval){
            openDoorCommand = false;
        }
    }
}
void checkTaster(){
    if(digitalRead(tasterPin) == LOW){
        previousMillis = millis();
        openDoorCommand = true;
        openDoor();
    }
}
bool nfcScanCard(){
    // Look for new cards
    if ( ! mfrc522.PICC_IsNewCardPresent()) {
        return false;
    }
    // Select one of the cards
    if ( ! mfrc522.PICC_ReadCardSerial()) {
        return false;
    }
    
    return true;
}
bool nfcCheckPiccType(){
    // Dump PICC type
    byte piccType = mfrc522.PICC_GetType(mfrc522.uid.sak);
    Serial.print("PICC type: ");
    Serial.println(mfrc522.PICC_GetTypeName(piccType));
    if (    piccType != MFRC522::PICC_TYPE_MIFARE_MINI 
            && piccType != MFRC522::PICC_TYPE_MIFARE_1K
            && piccType != MFRC522::PICC_TYPE_MIFARE_4K) {
            //Serial.println("This sample only works with MIFARE Classic cards.");
            return false;
    }
    
    return true;
}
bool nfcAuthKeys(){
    // Authenticate using key A.
    status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, trailerBlock, &key, &(mfrc522.uid));
    if (status != MFRC522::STATUS_OK) {
            Serial.print("PCD_Authenticate() failed: ");
            Serial.println(mfrc522.GetStatusCodeName(status));
            return false;
    }
    // Authenticate using key B.
    status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_B, trailerBlock, &key, &(mfrc522.uid));
    if (status != MFRC522::STATUS_OK) {
            Serial.print("PCD_Authenticate() failed: ");
            Serial.println(mfrc522.GetStatusCodeName(status));
            return false;
    }
    
    return true;
}
/*
 * @return: 0 = User not exist, 1 = auth error!; 2=auth successful
*/
int fileAuthUid(String uid){
    int id;
    DynamicJsonBuffer jsonBuffer;
         //read existing data from file
        myFile = SD.open(uid);
        String command = "";
        char character;
        if (myFile) {
            // read from the file until there's nothing else in it:
            while (myFile.available()) {
                character = myFile.read();
                command += character;
            }
            // close the file:
            myFile.close();
        } else {
            return 0; //no file found, end of user search..
        }
        
        //debug only!
        Serial.println(command);
        
        //parse config
        char json[command.length() + 1];
        command.toCharArray(json, command.length() + 1);
        JsonObject& root = jsonBuffer.parseObject(json);
        
        if (!root.success()) {
            //Serial.println("parseObject() failed");
            return 0;//goto next user
        }
        //compare uid
        if(!uid.equals(root["u"])) return 0; //goto next user
        
        //compare time
        int currentWeekDay = Time.weekday();
        int currentHour = Time.hour();
        
        //look for daily access permission
        if(root["t"][currentWeekDay-1] == 0){
            return 1; //access denied on current day
        }
        
        //compare and override otp
        unsigned char sessionKey[BUFFER_SIZE];
	    byte size = BUFFER_SIZE;
	    status = mfrc522.MIFARE_Read(valueBlockA, sessionKey, &size);
	    if (status != MFRC522::STATUS_OK) {
		    Serial.print(F("MIFARE_Read() failed: "));
		    Serial.println(mfrc522.GetStatusCodeName(status));
		    return 1; //READ Failed
	    }
	    
	    //convert char array to uint32_t
	    uint32_t intSessionKey;
	    memcpy(&intSessionKey, sessionKey, 4);
	    if(root["otp"] != 0 && root["otp"] != intSessionKey){
	        return 1;
	    }
	   
	    //wait for random number generator and save values
	    while(!RNG_GetFlagStatus(RNG_FLAG_DRDY));
	    uint32_t otp = RNG_GetRandomNumber(); //read and clear random number
	    
	    Serial.print("OTP: ");
	    Serial.println(otp);
	    
	    unsigned char otpChar[16];
	    for(int i=0;i<16;i++) otpChar[i] = 0xFF;
	    memcpy(otpChar, &otp, SESSIONKEY_SIZE);
	    status = mfrc522.MIFARE_Write(valueBlockA, otpChar, 16); //must be 16 here, otherwise write fail
	    if (status != MFRC522::STATUS_OK) {
		    Serial.print(F("updateSessionId() failed: "));
		    Serial.println(mfrc522.GetStatusCodeName(status));
		    return 1;
	    }
	    
	    
	    root["otp"] = otp;
	    
	    String fileName = String(id);
	    char charFile[fileName.length() + 1];
	    fileName.toCharArray(charFile, fileName.length() + 1);
	    
	    SD.remove(charFile); //rewrite file
	    
	    myFile = SD.open(String(id), FILE_WRITE);
	if (myFile) {
            root.printTo(myFile);
            // close the file:
            myFile.close();
            return 2;
        } else {
            // if the file didn't open, print an error:
            Serial.println("error opening uid file");
            return 1;
        }
}
bool fileWriteLog(String command){
    // open the file. note that only one file can be open at a time,
    // so you have to close this one before opening another.
    myFile = SD.open(fileLog, FILE_WRITE);
     
    // if the file opened okay, write to it:
    if (myFile) {
        myFile.print(Time.now());
        myFile.print(" : ");
        myFile.println(command);
        // close the file:
        myFile.close();
    } else {
        // if the file didn't open, print an error:
        Serial.println("error opening fileLog");
    }
}
int cloudUpdateKey(String command){    
    //Convert cloud param to json
    DynamicJsonBuffer jsonBuffer;
    char json[command.length() + 1];
    command.toCharArray(json, command.length() + 1);
    JsonObject& root = jsonBuffer.parseObject(json);
    
    if (!root.success()) {
        Serial.println("parseObject() failed");
        return -1;
    }
    
    int id;
    bool userFound = false;
    bool firstUser = true;
    const char* charUid = root["u"];
     
    String fileName = String(charUid);
    SD.remove(fileName); //rewrite file if user exists
	    
    myFile = SD.open(fileName, FILE_WRITE);
    // if the file opened okay, write to it:
    if (myFile && sdStatus) {
        Serial.print("Writing uid file");
        root.printTo(myFile);
        // close the file:
        myFile.close();
        Serial.println(" done.");
        return 1;
    } else {
        // if the file didn't open, print an error:
        Serial.println("error opening uid file");
        return -1;
    }
}
