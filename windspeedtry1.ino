
// include EEPROM write - required to memorize antenna / band config.
#include <SPI.h>
#include <SD.h>
#include <math.h>
int VaneValue;// raw analog value from wind vane 
int Direction;// translated 0 - 360 direction 
int CalDirection;// converted value with offset applied 
int LastValue; 
int windspeed;

#define Offset 0; 
const int chipSelect = 4;

volatile unsigned long RPMTops;  // RPM tops counter in interrupt routine                             
volatile unsigned long ContactTime;  // Timer to avoid conatct bounce in interrupt routine                              

float RPM;       // RPM count
float TEMP;      // Temp
                                    
#define RPMsensor (2)      //The pin location of the anemometer sensor

#define  NTCpin    (A4)    // Pin for NTC 10k





////////////////////////////////////////////////////////////////////
void setup() { 
  LastValue = 1; 
    if (!SD.begin(chipSelect)) {
    //Serial.println("Card failed, or not present");
    // don't do anything more
    // print to the serial port too:
  }
  File dataFile = SD.open("datalog.txt", FILE_WRITE);
        if (dataFile) {
          dataFile.println("Speed,Vane Value,Direction,Heading");
    dataFile.close();
        }
    else {
    //Serial.println("error opening datalog.txt");
  }

//  Clean EEPROM
//  EEPROM.write (1, 0);
//  EEPROM.write (2, 0);
   
   // set up the LCD's number of columns and rows: 
  //Serial.begin(9600);
  
  
  delay (2000);
  
// CALIBRATE if button depressed at startup !



  

pinMode(RPMsensor, INPUT); 
attachInterrupt(0, rpm, FALLING); 


    
 } 

/////////////////////////////////////////////////////////////////////////////////

void loop() { 






         
  // measure RPM
  
   RPMTops = 0;   //Set NbTops to 0 ready for calculations
   sei();         //Enables interrupts
   delay (10000);  //Wait 3 seconds to average
   cli();         //Disable interrupts

 
 // convert to km/h
 

 if ((RPMTops >= 0) and (RPMTops <= 21)) RPM = RPMTops * .746;
 if ((RPMTops > 21) and (RPMTops <= 45)) RPM = RPMTops * .715;
 if ((RPMTops > 45) and (RPMTops <= 90)) RPM = RPMTops * .684;
 if ((RPMTops > 90) and (RPMTops <= 156)) RPM = RPMTops * .652;
 if ((RPMTops > 156) and (RPMTops <= 999)) RPM = RPMTops * .621;

     
 // print the speed 


  float rounded_speed = round(RPM*100)/100;
  float newrpm = RPM/3.333;
    String dataString = "";

  File dataFile = SD.open("datalog.txt", FILE_WRITE);

  // if the file is available, write to it:

   //if the file isn't open, pop up an error:
VaneValue = analogRead(A4); 
Direction = map(VaneValue, 0, 1023, 0, 360); 
CalDirection = Direction + Offset; 

if(CalDirection > 360) 
CalDirection = CalDirection - 360; 

if(CalDirection < 0) 
CalDirection = CalDirection + 360; 

// Only update the display if change greater than 2 degrees. 
  if (dataFile) {
    dataFile.print(newrpm); dataFile.print(",");
    dataFile.print(VaneValue); dataFile.print(",");  
    dataFile.print(CalDirection); dataFile.print(",");
    dataFile.print(windspeed); dataFile.print(","); 
    getHeading(CalDirection); 
    dataFile.close();
    // print to the serial port too:
  }
    else {
    //Serial.println("error opening datalog.txt");
  }
LastValue = CalDirection; 
}


//// This is the function that interrupt calls to measure  RPM  

 void rpm ()   { 

    if ((millis() - ContactTime) > 15 ) {  // debounce of REED contact. With 15ms speed more than 150 km/h can be measured
      RPMTops++; 
      ContactTime = millis();
    }
    
 }

 void getHeading(int direction) { 
  File dataFile = SD.open("datalog.txt", FILE_WRITE);
    if (dataFile) {
if(direction < 22) 
dataFile.println("N"); 
else if (direction < 67) 
dataFile.println("NE"); 
else if (direction < 112) 
dataFile.println("E"); 
else if (direction < 157) 
dataFile.println("SE"); 
else if (direction < 212) 
dataFile.println("S"); 
else if (direction < 247) 
dataFile.println("SW"); 
else if (direction < 292) 
dataFile.println("W"); 
else if (direction < 337) 
dataFile.println("NW"); 
else 
dataFile.println("N"); 
    dataFile.close();
    // print to the serial port too:
  }
    else {
    //Serial.println("error opening datalog.txt");
  }
}


 
