// This code is prepared to perfrom PCR using the photon-to-heat conversion property of Carbon Qunatum Dots //
// Designed by S.Balou 10/11/2021 //

#include "max6675.h" // Include max6675 library to read MX6675 k-type thermocouple module

// Relay PIN //
int trans_pin = 3;

//Fan Pin//
int fan = 9;


// Laser Operating temperature:(55-95) for PCR and (65) for LAMP + Number of PCR cycles (35)
int pcrCycle = 0;
unsigned long current_millis;
unsigned long init_denat_time;
unsigned long denat_start_time;
unsigned long exte_start_time;
unsigned long anneal_start_time;

//PCR variables
int denat_time = 20;//Duration of Denaturing
int exte_time = 30;
int Anneal_time = 20;//Duration of Extention
int int_denat_duration = 60;//Duration of initial Denaturing
int maxcycle = 30; //Number of PCR Cycles
int denat_T = 94; // Min Denaturing temperature
int exte_T = 71; // Min Extention temperature
int Anneal_T = 54;// Min Annealing temperature

// K type Thermistor Variables and Pins //
int soPin = 4;// SO = Serial Out
int csPin = 5;// CS = chip select CS pin
int sckPin = 6;// SCK = Serial Clock pin 
MAX6675 k_type_module(sckPin, csPin, soPin);// create instance object of MAX6675 
double temperature_read = 0;      // Intantaneous temperature value
double final_temperature_read = 0;   // Variable used to calculate the final temperature
double temperature_num_samples = 5; // Number of temperatures values sampled for averaging  
double temperature_offset = 5; // temperature offset to calibrate the temperature sensor                              
double temperature = 0;
float t = 0;
float A = 1.0206;
float  B = 8.4271;


//Duty Cycle values obtained by doing steadtstate analysis
int a  = 120;
int b = 90;
int c = 50;



void setup()
{
  Serial.begin(9600);
  pinMode(trans_pin, OUTPUT);
  pinMode(fan, OUTPUT);
   
  // We have to wait for the temperature to reach 95 first:
  Serial.println("Please wait for the temperature to reach 95");

  while(temperature < denat_T){
    analogWrite(trans_pin, a);
    digitalWrite(fan, LOW);//fan off
    update_temperature();
    display_results();
  }

  Serial.println("Temperature has reached 95, please wait 60s for the initial denaturing to complete");
  init_denat_time = millis()/1000;
  while((millis()/1000)-init_denat_time <= int_denat_duration){
    if (temperature <= denat_T){
   analogWrite(trans_pin, a);
   digitalWrite(fan, LOW);//fan off
     update_temperature();
     display_results();
  }
    if (temperature >= denat_T+2){
     analogWrite(trans_pin, a-10);
     digitalWrite(fan, LOW);//fan off
     update_temperature();
     display_results();
  }
  update_temperature();
  display_results();
  }
  Serial.println("Initial Denaturing has completed, Amplification Loop will start now");
  pcrCycle++;
}

void loop()
{
while (pcrCycle <= maxcycle){  // While we are within the 30 cycles (TRUE) arduino will do as follows:
Serial.println("Going to Denaturing Temperature");
      while(temperature < denat_T){ // Make sure temperature is 95 to begin the loop
        analogWrite(trans_pin, a);
        digitalWrite(fan, LOW);//fan off
        update_temperature();
        display_results();
      }
      Serial.println("Denaturing DNA Now");
      denat_start_time = millis()/1000;
      while((millis()/1000)- denat_start_time < denat_time){ // As long as we are within the 20s time frame
        
        if (temperature <= denat_T){
        analogWrite(trans_pin, a);
        digitalWrite(fan, LOW);//fan off
        update_temperature();
        display_results();
      }
      if (temperature > denat_T+2){
        analogWrite(trans_pin, a-10);//Laser Off
        digitalWrite(fan, LOW);//fan off
        update_temperature();
        display_results();
      }
        update_temperature();
        display_results();
      }
Serial.println("Going to Annealing Temperature");
      while(temperature > Anneal_T){ // Make sure the temperature has reached 55 to begin the second step (Annealing)
        analogWrite(trans_pin, 0);//Laser Off
        digitalWrite(fan, HIGH);//fan on
        update_temperature();
        display_results();
      }
Serial.println("Annealing DNA Now");
      anneal_start_time = millis()/1000;
      while ((millis()/1000)-anneal_start_time < Anneal_time){ // As long as we are in 20s time frame
        
      if (temperature <= Anneal_T){
        analogWrite(trans_pin, c);//Laser On
        digitalWrite(fan, LOW);//fan off
        update_temperature();
        display_results();
      }
      if (temperature > exte_T+2){
        analogWrite(trans_pin, c-10);//Laser Off
        digitalWrite(fan, LOW);//fan off
        update_temperature();
        display_results();
      }
        update_temperature();
        display_results();
    }

Serial.println("Going to Extension Temperature");
      while(temperature < exte_T){ // Make sure the temperature has reached 72 to begin the second step (Annealing)
        analogWrite(trans_pin, b);//Laser On
        digitalWrite(fan, LOW);//fan off
        update_temperature();
        display_results();
      }

Serial.println("Extending DNA Now");  
      exte_start_time = millis()/1000;
      while ((millis()/1000)-exte_start_time < exte_time){ // As long as we are in 30s time frame
        
      if (temperature <= exte_T){
        analogWrite(trans_pin, b);//Laser On
        digitalWrite(fan, LOW);//fan off
        update_temperature();
        display_results();
      }
      if (temperature > exte_T+2){
        analogWrite(trans_pin, b-10);//Laser Off
        digitalWrite(fan, LOW);//fan off
        update_temperature();
        display_results();
      }
        update_temperature();
        display_results();
    }
        update_temperature();
        display_results();
        pcrCycle++;
    }
while (pcrCycle > maxcycle){
analogWrite(trans_pin, 0);
Serial.println("Process Ended, GoodBye");
  }
}
  
void update_temperature(){
  t = k_type_module.readCelsius();
  temperature = A*t - B;  
}

void display_results(){
          Serial.print(millis()/1000); 
          Serial.print(" , ");
          Serial.print(temperature); 
          Serial.print(" , ");
          Serial.println(pcrCycle); 
          delay(1000);
}
