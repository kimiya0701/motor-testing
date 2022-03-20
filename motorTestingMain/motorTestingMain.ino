#define USE_TIMER_1 true
#define USE_TIMER_2 true

#include <TimerInterrupt.h>
#include <Streaming.h>
#include <HX711.h>
#include <movingAvg.h>

//RPM Sensor
#define MOTOR_POLES 4
#define RPM_IN 2

//Battery Voltage
#define VOLT_DIV_IN A2
#define VOLT_DIV_RATIO 5.4

//Current Sensor
#define CURRENTMETER A5

//Load Cell
#define DOUT  3
#define CLK  4

//RPM Sensor
void rpm_isr();
void rpm_counter();
unsigned long rpmcount = 0;
unsigned long rpm = 0;

//Battery Voltage
unsigned int voltage_reading = 0;
float battery_voltage = 0;

//Current Sensor
float current_reading = 0.0;
float current_drawn = 0.0;
float current_sensor_voltage = 0.0;
movingAvg current(100);

//Load Cell
HX711 scale;
float calibration_factor_original = -2070; //miguel and kimiya testing
float calibration_factor_1 = (112500.000000000000000); //jun testing
float calibration_factor_2 = -(4380000/2); // jun testing

void setup() {
  Serial.begin(9600);

  //initializing current input pin and moving average
  pinMode(CURRENTMETER, INPUT);
  current.begin();

  //initializes RPM timer and interrupts
  ITimer2.init();
  ITimer2.attachInterruptInterval(1000, rpm_counter);   // 1 second interrupt interval
  attachInterrupt(digitalPinToInterrupt(RPM_IN),rpm_isr,RISING);

  scale.begin(DOUT, CLK);
  scale.set_scale();
  scale.tare(); //Reset the scale to 0
}

void loop() {

  //voltage calculations
  voltage_reading = analogRead(VOLT_DIV_IN);
  battery_voltage = ((5.4*voltage_reading)/1023)*VOLT_DIV_RATIO; // make sure to check that AREF is 5.4 (change if needed)

  //current calculations  
  current_reading = analogRead(CURRENTMETER);
  current_sensor_voltage = current_reading*(5.0/1023.0);
  current_drawn = (current_sensor_voltage - 2.5)/0.0625;
  int current_avg = current.reading(current_drawn);
  
  //thrust calculations
  scale.set_scale(calibration_factor_1);
  
  // print all the data
  
  Serial << "Thrust: " << 0.72*scale.get_units();

  //Serial << ", Moment: " << (scale.get_units())*0.72 << "lbs"; //force on propeller

  Serial << ", Battery Voltage: " << battery_voltage << 'V';

  Serial << ", Current: " << current_avg/100 << 'A';
  
  Serial << ", RPM: " << rpm << endl;

  delay(1000);
}

void rpm_isr(void)
{
  rpmcount++;  
}

void rpm_counter(void)
{
  rpm = (rpmcount * 60)/MOTOR_POLES;
  rpmcount = 0;
}
