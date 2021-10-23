#include "max6675.h"

#define  trans_pin  3
#define current_sensor_pin A0
double Vout = 0;
double current = 0;
int current_num_samples = 100; // Number of temperature samples for averaging
const double scale_factor = 0.066; // 30A
const double vRef = 5.00;
const double resConvert = 1024;
double resADC = vRef / resConvert;
double zeroPoint = 2.5;

int soPin = 4;// SO = Serial Out
int csPin = 5;// CS = chip select CS pin
int sckPin = 6;// SCK = Serial Clock pin 
MAX6675 k_type_module(sckPin, csPin, soPin);// create instance object of MAX6675                             
double temperature = 0;
float t = 0;
float A = 1.0206;
float  B = 8.4271;

int i= 0;

unsigned long denat_start_time;
unsigned long exten_start_time;
unsigned long process_start_time;
unsigned long ss_start_time;

void setup() {
pinMode(trans_pin, OUTPUT);
Serial.begin(9600);
}

void loop() {

while(temperature < 65){
  analogWrite(trans_pin, 150); // Full power laser to reach 65 as soon as possible
  update_temperature();
  display_results();
}
update_temperature();
display_results();
process_start_time = millis();
while(((millis() - process_start_time)/1000) < 1800){ //// leave the Laser on for 30 minutes at duty cylce 70 to reach equ_temp 65 based on the steadystate analysis test
    if (temperature >=65){
      analogWrite(trans_pin, 20);
          update_temperature();
          display_results();
    }
    if (temperature <65){
      analogWrite(trans_pin, 60);
          update_temperature();
          display_results();
    }
}
while(1){
  analogWrite(trans_pin, 0);
}
}

//i  = i+20;
//ss_start_time = millis();
//  while(((millis() - ss_start_time)/1000) < 180){ // maintain dutycycle constant fot 3 minutes//
//      analogWrite(trans_pin, i);
//          update_temperature();
//          update_current();
//          display_results();
//    }
//    delay(350);
//}
 
  
//  // Now turn off the laser to reach the temperature 60 first. and make sure temperature has reached 60. 
// while(temperature >= 60){
//    analogWrite(trans_pin, 0); // This will bring the temperature to 60 real quick(0 analogwrite output -> laser off)
//    update_temperature();
//    display_results();
// }
// 
// // Now begin the second step:
//  exten_start_time = millis();
//  while(((millis() - exten_start_time)/1000) < 40){ // maintain temperature at 60 for 40 seconds//
//    if (temperature <= 60){
//      analogWrite(trans_pin, 55);
//          update_temperature();
//          display_results();
//    }
//    if (temperature >= 62){
//      analogWrite(trans_pin, 0);
//          update_temperature();
//          display_results();
//  }
//            update_temperature();
//            display_results(); 
//  }
//
//}

void update_temperature(){
  t = k_type_module.readCelsius();
  temperature = A*t - B;  
}

void update_current()
{
  // Vout is read current_num_samples Times for precision
  for (int i = 0; i < current_num_samples; i++) {
    Vout = (Vout + (resADC * analogRead(current_sensor_pin)));
    delay(3);
}
  Vout = Vout / current_num_samples;
  current = (Vout - zeroPoint) / scale_factor;
}

void display_results(){
          Serial.print(millis()/1000); 
          Serial.print(" , ");
          Serial.println(temperature); 
          delay(1000);
}
