

//Particle Code for collecting data from Shinyei, Temp/RH
#include "cmath"
//Include Libraries here:
#include "Adafruit_Si7021.h"

//Create the object
Adafruit_Si7021 sensor = Adafruit_Si7021();

//Shinyei variables
int Shinyei = D3;
unsigned long duration1;
unsigned long starttime;
unsigned long sampletime_ms = 400;
unsigned long lowpulseoccupancy1 = 0;
float ratio1 = 0;
float concentration1 = 0;

//Struct to store plantower data
struct pms5003data {
  uint16_t framelen;
  uint16_t pm10_standard, pm25_standard, pm100_standard;
  uint16_t pm10_env, pm25_env, pm100_env;
  uint16_t particles_03um, particles_05um, particles_10um, particles_25um, particles_50um, particles_100um;
  uint16_t unused;
  uint16_t checksum;
};
struct pms5003data data;

//Plantower vairables
int shiftedtxdata;
int prevdata1, prevdata2, prevdata3;
int count = 1; 

void setup(){
  
  //Serial for plantower
  Serial1.begin(9600, SERIAL_8N1);

  //Serial for terminal
  Serial.begin(9600);

  //Setup for Shinyei
  pinMode(Shinyei,INPUT);
  if (!sensor.begin()) {
    Serial.println("Did not find Si7021 sensor!");
  }
}

void loop(){
  //Plantower data collection and processing
    digitalWrite(D2,HIGH);

  int txdata = Serial1.read();
  if(txdata != -260){
    if(count % 2 == 1){
      shiftedtxdata = txdata << 8;
      count++;
    }
    else{
      int dataval = shiftedtxdata + txdata;
      if (dataval == 16973){
        count = 2;
      }
      switch(count){
        case 4 :
          data.framelen = dataval;
        case 6 :
          data.pm10_standard = dataval;
        break;
        case 8 :
          data.pm25_standard = dataval;
        break;
        case 10 :
          data.pm100_standard = dataval;
        break;
        case 12 :
          data.pm10_env = dataval;
        break;
        case 14 :
          data.pm25_env = dataval;
        break;
        case 16 :
          data.pm100_env = dataval;
        break;
        case 18 :
          data.particles_03um = dataval;
        break;
        case 20 :
          data.particles_05um = dataval;
        break;
        case 22 :
          data.particles_10um = dataval;
        break;
        case 24 :
          data.particles_25um = dataval;
        break;
        case 26 :
          data.particles_50um = dataval;
        break;
        case 28 :
          data.particles_100um = dataval;
        break;
        case 32 :
          data.checksum = dataval;
          Serial.println("---------------------------------------");
          Serial.println("Concentration Units (standard) uq / m^3");
          Serial.print("PM 1.0: "); Serial.print(data.pm10_standard);
          Serial.print("\t\tPM 2.5: "); Serial.print(data.pm25_standard);
          Serial.print("\t\tPM 10: "); Serial.println(data.pm100_standard);
          Serial.println("---------------------------------------");
          Serial.println("Concentration Units (environmental) ug / m^3");
          Serial.print("PM 1.0: "); Serial.print(data.pm10_env);
          Serial.print("\t\tPM 2.5: "); Serial.print(data.pm25_env);
          Serial.print("\t\tPM 10: "); Serial.println(data.pm100_env);
          Serial.println("---------------------------------------");
          Serial.print("Particles > 0.3um / 0.1L air: "); Serial.println(data.particles_03um);
          Serial.print("Particles > 0.5um / 0.1L air: "); Serial.println(data.particles_05um);
          Serial.print("Particles > 1.0um / 0.1L air: "); Serial.println(data.particles_10um);
          Serial.print("Particles > 2.5um / 0.1L air: "); Serial.println(data.particles_25um);
          Serial.print("Particles > 5.0um / 0.1L air: "); Serial.println(data.particles_50um);
          Serial.print("Particles > 10.0 um / 0.1L air: "); Serial.println(data.particles_100um);
          Serial.println("---------------------------------------");
          //Shinyei Code
              duration1 = pulseIn(Shinyei, LOW);
    lowpulseoccupancy1 = lowpulseoccupancy1+duration1;
          if ((millis()-starttime) > sampletime_ms){
      ratio1 = lowpulseoccupancy1/((millis()-starttime)*1000.0);
      concentration1 = 1.1*pow(ratio1,3)-3.8*pow(ratio1,2)+520*ratio1+0.62; // using spec sheet curve
      Serial.print("Shinyei:");
      Serial.println(lowpulseoccupancy1);
      Serial.println((int)concentration1);
      lowpulseoccupancy1 = 0;
      starttime = millis();
    }
        break;
        default :
        break;
      }
      count++;
    }
    if(count == 33){
      count = 1;
    }
  }
}

//exponential function
float pow(float a, int b){
  for(int i=0; i < b; i++){
    a = a * a;
  }
  return a;
}
