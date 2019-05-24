#include "Serial2/Serial2.h"

//Honeywell vairables
int hPM100;
int hPM25;
int framelength2;
int shiftedtxdata2;
int dataval2;
int count2 = 1;
int checksum2;

SYSTEM_THREAD(ENABLED);



void setup(){
  
  //Serial for plantower
  Serial2.begin(9600, SERIAL_8N1);

  //Serial for terminal
  Serial.begin(9600);
}

void loop(){
  //Honeywell data collection and processing
  int txdata2 = Serial2.read();
  if(txdata2 != -260){
    if(count2 % 2 == 1){
      shiftedtxdata2 = txdata2 << 8;
      count2++;
    }
    else{
      int dataval2 = shiftedtxdata2 + txdata2;
      if (dataval2 == 16973){
        count2 = 2;
      }
      switch(count2){
        case 4 :
          framelength2 = dataval2;
        case 8 :
          hPM25 = dataval2;
        break;
        case 10 :
          hPM100 = dataval2;
        break;
        case 32 :
          checksum2 = dataval2;
          Serial.println("Honeywell:");
          Serial.print("PM 2.5: "); Serial.print(hPM25);
          Serial.print("\tPM 10: "); Serial.println(hPM100);
        break;
        default :
        break;
      }
      count2++;
    }
    if(count2 == 33){
      count2 = 1;
    }
  }
}
