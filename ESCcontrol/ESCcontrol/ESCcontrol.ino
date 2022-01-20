#include <Streaming.h>
#include <Servo.h>


#define PWM_OUT 6
#define THROTTLE A0


int pwm;
float pwmPercent = 0.0;

Servo ESC;


void setup() {
  Serial.begin(9600);
  ESC.attach(6,1000,2000);
}

void loop() {
  
  pwm = map((float)analogRead(THROTTLE), 0, 895, 0, 180);
  ESC.write(pwm);
  Serial.println(analogRead(THROTTLE));
  Serial.println(pwm); 
  pwmPercent = (pwm/180.0) * 100.0;
  Serial.println(pwmPercent);

  delay(100);
}
