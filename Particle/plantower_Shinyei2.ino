//Shinyei variables
int Shinyei = D3;
unsigned long duration1;
unsigned long starttime;
unsigned long sampletime_ms = 5000;
unsigned long lowpulseoccupancy1 = 0;
float ratio1 = 0;
float concentration1 = 0;

const char * apikey1 = "9X4RQPU5EZQ3O8AD";
const char * apikey2 = "4UOSBI1ALJHHK6OS";

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

struct serverdata{
  char shinyei[128];
  char framelen[128];
  char pm10_standard[128], pm25_standard[128], pm100_standard[128];
  char pm10_env[128], pm25_env[128], pm100_env[128];
  char particles_03um[128], particles_05um[128], particles_10um[128], particles_25um[128], particles_50um[128], particles_100um[128];
  char checksum[128];
};
struct serverdata send;

//Plantower vairables
int shiftedtxdata;
int prevdata1, prevdata2, prevdata3;
int count = 1; 

void setup(){
  
  //Serial for plantower
  Serial1.begin(9600, SERIAL_8N1);

  //Setup for Shinyei
  pinMode(Shinyei,INPUT);
}

void loop(){
  //Plantower data collection and processing
  int txdata = Serial1.read();
  if(txdata != -260){
    if(count % 2 == 1){
      shiftedtxdata = txdata << 8;
      if(count == 1){
        if(txdata != 66){
          count = 0;
        }
      }
      count++;
    }
    else{
      int dataval = shiftedtxdata + txdata;
      if (dataval == 16973){
        count = 2;
      }
      switch(count){
        case 2 :
        case 4 :
          if(dataval != 28){
            
          }
          data.framelen = dataval;
        break;
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
          char Xenonplant1[622];
          char Xenonplant2[622];
          
          //Shinyei Code
          duration1 = pulseIn(Shinyei, LOW);
          lowpulseoccupancy1 = lowpulseoccupancy1+duration1;
          if ((millis()-starttime) > sampletime_ms){
            ratio1 = 100*(lowpulseoccupancy1/((millis()-starttime)*10.0));
            int ratioint = (int)ratio1;
            snprintf(send.shinyei, sizeof(send.shinyei), "%d", ratioint);
            lowpulseoccupancy1 = 0;
            snprintf(Xenonplant2, sizeof(Xenonplant2), "{\"f1\":\"%d\",\"f2\":\"%d\",\"f3\":\"%d\",\"f4\":\"%d\",\"f5\":\"%d\",\"f6\":\"%d\",\"f7\":\"%d\",\"k\":\"%s\"}", data.particles_03um, data.particles_05um, data.particles_10um, data.particles_25um, data.particles_50um, data.particles_100um, ratioint, apikey2);
            snprintf(Xenonplant1, sizeof(Xenonplant1), "{\"f1\":\"%d\",\"f2\":\"%d\",\"f3\":\"%d\",\"f4\":\"%d\",\"f5\":\"%d\",\"f6\":\"%d\",\"f7\":\"%d\",\"f8\":\"%d\",\"k\":\"%s\"}", data.pm10_standard, data.pm25_standard, data.pm100_standard, data.pm10_env, data.pm25_env, data.pm100_env, data.framelen, data.checksum, apikey1);
            Particle.publish("Xenonplant2", Xenonplant2, PRIVATE);
            Particle.publish("Xenonplant1", Xenonplant1, PRIVATE);
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
