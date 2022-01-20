
#include "HX711.h" //This library can be obtained here http://librarymanager/All#Avia_HX711

#define LOADCELL_1_DOUT_PIN  3
#define LOADCELL_1_SCK_PIN  2
#define LOADCELL_2_DOUT_PIN 5
#define LOADCELL_2_SCK_PIN  4
#define CURRENTMETER A0
HX711 scale_1;
HX711 scale_2;

float calibration_factor_1 = -(112500.000000000000000);
float calibration_factor_2 = -(4380000/2);

float current = 0.0;
int sensorvalue = 0;

void setup() {
  
  Serial.begin(9600);
  
  
//  scale_1.begin(LOADCELL_1_DOUT_PIN, LOADCELL_1_SCK_PIN);
//  scale_1.set_scale();
//  scale_1.tare();
//
//  scale_2.begin(LOADCELL_2_DOUT_PIN, LOADCELL_2_SCK_PIN);
//  scale_2.set_scale();
//  scale_2.tare();

  pinMode(CURRENTMETER, INPUT);

  
}

void loop() {
//  scale_1.set_scale(calibration_factor_1); //Adjust to this calibration factor
//  scale_2.set_scale(calibration_factor_2); //Adjust to this calibration factor
//
//
//
//  Serial.print(scale_1.get_units(), 4);
//  
//  Serial.print(",");
//  
//  Serial.print(scale_2.get_units(), 4);
//  
//  Serial.print(",");
  
  sensorvalue = analogRead(CURRENTMETER);
  float voltage = sensorvalue*(5.0/1023.0);
  current = (voltage - 2.5)/0.0625;
  Serial.print(current);
 
  Serial.println();
}
