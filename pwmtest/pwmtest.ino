#define USE_TIMER_1 true
#define USE_TIMER_2 true

#include <TimerInterrupt.h>
#include <Streaming.h>
#include "HX711.h"

#define MOTOR_POLES 4
#define PWM_OUT 6
#define RPM_OUT 2
#define VOLT_DIV_OUT A2
#define VOLT_DIV_RATIO 5.4
#define SWITCH A1
#define VRX A0 // throttle
#define CLAMP_PIN A3
#define CURRENT_SENSE_RATIO 0.625
#define DOUT  3
#define CLK  4

int pwm;
float switch_reading;

void rpm_isr();
void rpm_counter();
unsigned int rpmedge = 0;
unsigned long rpmcount = 0;
unsigned long rpm = 0;
unsigned int voltage_reading = 0;
float battery_voltage = 0;
float pwmPercent = 0.0;
float current_reading = 0.0;
float current_drawn = 0.0;
float current_drawn_new = 0.0;


HX711 scale;

float calibration_factor = -2070;

void setup() {
  Serial.begin(9600);

  ITimer2.init();
  ITimer2.attachInterruptInterval(1000, rpm_counter);
 
  TCCR0B = TCCR0B & B11111000 | B00000101;

  attachInterrupt(digitalPinToInterrupt(RPM_OUT),rpm_isr,RISING);

  scale.begin(DOUT, CLK);
  scale.set_scale();
  scale.tare(); //Reset the scale to 0
}

void loop() {


  // pwm calculations
  pwm = map((float)analogRead(VRX), 495, 1023, 15, 29);
  //analogWrite(PWM_OUT, 15);
  
  switch_reading = analogRead(SWITCH);

  if(switch_reading == 0){
    analogWrite(PWM_OUT, 17);
  } else{
    analogWrite(PWM_OUT, 15);
  }
  pwmPercent = ((pwm - 15.0)/14.0) * 100.0;

  // voltage calculations
  voltage_reading = analogRead(VOLT_DIV_OUT);
  battery_voltage = ((5.4*voltage_reading)/1023)*VOLT_DIV_RATIO; // make sure to check that AREF is 5.4 (change if needed)

  //current calculations  
  current_drawn = 0.0;
  current_reading = analogRead(CLAMP_PIN);
  current_drawn_new = ((((.625*current_reading)/(10))));
  current_drawn = (((5.4*current_reading)/1023)-2.5)*CURRENT_SENSE_RATIO;

  //thrust calculations
  scale.set_scale(calibration_factor);
  

  
  // print all the data
  Serial << "PWM Percent: " << pwmPercent << "%";
  
  Serial << ", Thrust: " << 0.72*scale.get_units() << "lbs";

  //Serial << ", Moment: " << (scale.get_units())*0.72 << "lbs"; //force on propeller

  Serial << ", Battery Voltage: " << battery_voltage << 'V';

  Serial << ", Current NEW: " << current_drawn_new << 'A';

  Serial << ", Current: " << current_drawn << 'A';
  
  Serial << ", RPM: " << rpm << endl;

  delay(100);
}

void rpm_isr(void)
{
  rpmcount++;  
}

void rpm_counter(void)
{
  rpm = (rpmcount * 60)/MOTOR_POLES;
  rpmcount = 0;

  //Serial.println(rpm);
}
