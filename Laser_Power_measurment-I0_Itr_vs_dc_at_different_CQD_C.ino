#include "max6675.h"

#define  trans_pin  3

int soPin = 4;// SO = Serial Out
int csPin = 5;// CS = chip select CS pin
int sckPin = 6;// SCK = Serial Clock pin 
MAX6675 k_type_module(sckPin, csPin, soPin);// create instance object of MAX6675                             
double temperature = 0;
float t = 0;
float A = 1.0206;
float  B = 8.4271;

int i= 10;


unsigned long denat_start_time;
unsigned long exten_start_time;
unsigned long process_start_time;
unsigned long ss_start_time;

void setup() {
pinMode(trans_pin, OUTPUT);
Serial.begin(9600);
}

void loop() {
 process_start_time = millis();
   while(((millis() - process_start_time)/1000) < 20){ //// keep dc constant for 20 seconds
       analogWrite(trans_pin, i);
           update_temperature();
           display_results();
     }
i = i+10;
process_start_time = millis();
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
          Serial.println(i); 
          delay(1000);
}
