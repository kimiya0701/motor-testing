#include <Streaming.h>

//RPM Sensor
#define MOTOR_POLES 4
#define RPM_IN A0


int long frequency = 0;
int long RPM = 0;
float voltage = 0.0;
int long time = 0; 
 
void setup() {
  // put your setup code here, to run once:
  pinMode(RPM_IN, INPUT);
  Serial.begin(9600);
}
// rpm = (60*frequency)/motor poles
void loop() {
  // put your main code here, to run repeatedly:
  
  while(time < 100 ){
   time = millis(); 
   voltage = (analogRead(A0) * (4.93 / 1023.0));
   Serial.print("Voltage: ");
   Serial.println(voltage);
   // Adds one to frequency if the voltage is above 0.5V
    if(voltage > 0.2){
    frequency++;
    }
  
  }
  //Calculate RPM
  RPM = (600*frequency)/MOTOR_POLES;
  // Print RPM
  Serial.print("RPM: ");
  Serial.println(RPM);
  //Reset frequency
  frequency = 0;
  //Reset Time
  time = 0;
  
  
  }
  
  
  
