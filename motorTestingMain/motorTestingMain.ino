#include <Streaming.h>
#include <HX711.h>
#include <movingAvg.h>

/****************************************************************
Pin Definitions
*****************************************************************/

//RPM Sensor
#define MOTOR_POLES 4
#define RPM_IN 7

//Battery Voltage
#define VOLT_DIV_IN A2
#define VOLT_DIV_RATIO 5.4

//Current Sensor
#define CURRENTMETER A5
#define CURRENTERROR 0.2

//Load Cell
#define DOUT  3
#define CLK  4


/****************************************************************
Variables Initialization
*****************************************************************/
//RPM Sensor
int ontime = 0;
int offtime = 0;
int duty = 0;
float freq = 0.0;
float period = 0.0;
float rpm = 0.0;

//Battery Voltage
unsigned int voltage_reading = 0;
float battery_voltage = 0;

//Current Sensor
float current_reading = 0.0;
float current_drawn = 0.0;
float current_sensor_voltage = 0.0;
const int numReadings = 10;
float readings[numReadings];
int readIndex = 0;
float total = 0.0;
float current_average = 0.0;

//Load Cell
HX711 scale;
float calibration_factor = 51410; //calculated in march for testing

/****************************************************************
Main Program
*****************************************************************/
void setup() {
  Serial.begin(9600);

  //initializing rpm input pin
  pinMode(RPM_IN, INPUT);
  
  //initializing current input pin and moving average
  pinMode(CURRENTMETER, INPUT);
  for (int thisReading = 0; thisReading < numReadings; thisReading++) {
    readings[thisReading] = 0;
  }

  scale.begin(DOUT, CLK);
  scale.set_scale();
  scale.tare(); //Reset the scale to 0
}

void loop() {

  //voltage calculations
  voltage_reading = analogRead(VOLT_DIV_IN);
  battery_voltage = ((4.92*voltage_reading)/1023)*VOLT_DIV_RATIO; // make sure to check that AREF is 4.92 (change if needed)

  //current calculations  
  current_reading = analogRead(CURRENTMETER);
  current_sensor_voltage = current_reading*(5.0/1023.0);
  current_drawn = (current_sensor_voltage * 5000.0) / (0.01*45300.0);
  // subtract the last reading:
  total = total - readings[readIndex];
  // read from the sensor:
  readings[readIndex] = current_drawn;
  // add the reading to the total:
  total = total + readings[readIndex];
  // advance to the next position in the array:
  readIndex = readIndex + 1;

  // if we're at the end of the array...
  if (readIndex >= numReadings) {
    // ...wrap around to the beginning:
    readIndex = 0;
  }

  // calculate the average:
  current_average = total / numReadings;
  
  //thrust calculations
  scale.set_scale(calibration_factor);

  //rpm calculations
  ontime = pulseIn(RPM_IN,HIGH);
  offtime = pulseIn(RPM_IN,LOW);
  period = ontime+offtime;
  freq = 1000000.0/period;
  //duty = (ontime/period)*100; 
  rpm = (freq * 60)/ MOTOR_POLES;
  
  // print all the data
  Serial << 0.72*scale.get_units() << "lbs, ";

  Serial << battery_voltage << "V, ";

  Serial << current_average << "A, ";
  
  Serial << rpm << endl;

  delay(1000);
}
