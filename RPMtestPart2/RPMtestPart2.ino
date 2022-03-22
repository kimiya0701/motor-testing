#include <Streaming.h>

#define MOTOR_POLES 4.0
#define pulse_ip 7
int ontime,offtime,duty;
float freq,period,rpm;
   
void setup()
{
  pinMode(pulse_ip,INPUT);
  Serial.begin(9600);
}
void loop()
{
   ontime = pulseIn(pulse_ip,HIGH);
   offtime = pulseIn(pulse_ip,LOW);
   period = ontime+offtime;
   freq = 1000000.0/period;
   duty = (ontime/period)*100; 
   rpm = (freq * 60)/ MOTOR_POLES;
   
    Serial << "On Time: " << ontime << "us\n";
    Serial << "Off Time: " << offtime << "us\n";
    Serial << "Period: " << period << "us\n";
    Serial << "Frequency: " << freq << "Hz\n";
    Serial << "RPM: " << rpm << "rpm\n";
    
   delay(1000);
}
