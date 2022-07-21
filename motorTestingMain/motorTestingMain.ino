#include <Streaming.h>
#include <HX711.h>
#include <movingAvg.h>
#include <OneWire.h> //for temperature probe
#include <DallasTemperature.h> //for temperature probe

/****************************************************************
Pin Definitions
*****************************************************************/

//RPM Sensor
#define MOTOR_POLES 4
#define RPM_IN 7

//Battery Voltage
#define VOLT_DIV_IN A1
#define VOLT_DIV_RATIO 5.4

//Current Sensor
#define CURRENTMETER A5
#define CURRENTERROR 0.2

//Load Cell
#define DOUT  3
#define CLK  4

// Temp Probes: All DS18B20 Sensors here are connected to pin 2 on the Arduino, adjust value according to which pin# data cable is connected to on arduino.
#define ONE_WIRE_BUS 2

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

//Temp Probes
int numberOfDevices; //To store number of sensor connected

//Load Cell
HX711 scale;
float calibration_factor = -94200.00; //calculated in march for testing

//Temp Probes
OneWire oneWire(ONE_WIRE_BUS); // Creating a oneWire instance(object)

DallasTemperature sensors(&oneWire); // Pass our oneWire object reference to Dallas Temperature. 

DeviceAddress tempDeviceAddress; // Variable to store a single sensor address


/***************************************************************
Temp Probe Error Check:
***************************************************************/
/*----------------------------------------------------------------*/


/******************************************************
function to print a sensor address
******************************************************/
void printAddress(DeviceAddress deviceAddress) {
  
  for (uint8_t i = 0; i < 8; i++) {
    
    if (deviceAddress[i] < 16) 
      Serial.print("0");
      Serial.print(deviceAddress[i], HEX);
      
  }//End of for loop
  
}
/****************************************************************
End of Temp Error Check
*****************************************************************/


/****************************************************************
Main Program
*****************************************************************/
void setup() {
  Serial.begin(9600);

  sensors.begin();//for temp probe
  
  // Get the number of sensors connected to the the wire( digital pin 2)
  numberOfDevices = sensors.getDeviceCount();
  
  //Serial.print(numberOfDevices, DEC);
 // Serial.println(" temperature probes detected.");

  // Loop through each sensor and print out address
 /* for(int i=0; i<numberOfDevices; i++) {
    
    // Search the data wire for address and store the address in "tempDeviceAddress" variable
    if(sensors.getAddress(tempDeviceAddress, i)) {
      
      Serial.print("Found probe ");
      Serial.print(i, DEC);
      Serial.print(" with address: ");
      printAddress(tempDeviceAddress);
      Serial.println();
      
    } else {
      
      Serial.print("Found ghost device at ");
      Serial.print(i, DEC);
      Serial.print(" but could not detect address. Check power and cabling");
      
    }
    
  }//End of Temperature probe for loop
  */

  //initializing rpm input pin
  pinMode(RPM_IN, INPUT);
  
  //initializing current input pin and moving average
  pinMode(CURRENTMETER, INPUT);
  for (int thisReading = 0; thisReading < numReadings; thisReading++) {
    readings[thisReading] = 0;
  }

  // THRUST
  scale.begin(DOUT, CLK);
  scale.set_scale();
  scale.tare(); //Reset the scale to 0
}

void loop() {

  //Temp Probe
  sensors.requestTemperatures(); // Send the command to get temperatures from all sensors.

  //voltage calculations
  voltage_reading = analogRead(VOLT_DIV_IN);
  battery_voltage = ((5.04*voltage_reading)/1023)*VOLT_DIV_RATIO; // make sure to check that AREF is 4.92 (change if needed)

  /*CURRENT*/
  current_reading = analogRead(CURRENTMETER);
  current_sensor_voltage = current_reading*(5.0/1023.0);
  current_drawn = current_sensor_voltage*13 + 0.1;
  //current_drawn = (current_sensor_voltage * 5000.0) / (0.01*45300.0);
  
  /*MOVING AVERAGE*/
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
  
  /*THRUST CALCS*/
  scale.set_scale(calibration_factor);

  /* RPM CALCS */
  ontime = pulseIn(RPM_IN,HIGH);
  offtime = pulseIn(RPM_IN,LOW);
  period = ontime+offtime;
  freq = 1000000.0/period;
  //duty = (ontime/period)*100; 
  rpm = (freq * 60)/ MOTOR_POLES;
  
  /* PRINT THE DATA*/

  //Thrust
  Serial << 0.8*9.81*scale.get_units() << ",N, "; //multiply by 0.72 for the moment arm
  //Serial << scale.get_units() << " ,kgs, ";

  
  //Voltage
  Serial << battery_voltage << ",V, ";
  
  //Current
  Serial << current_average << ",A, ";
  
  //RPM
  Serial << rpm << ", ";

  //Prints Temperature Probe Measurements
  // Loop through each device, print out temperature one by one
  for(int i=0; i<numberOfDevices; i++) {
    
    // Search the wire for address and store the address in tempDeviceAddress
    if(sensors.getAddress(tempDeviceAddress, i)){
    //Serial<<" Probe ";
    //Serial<<i;

    // Print the temperature
      float tempC = sensors.getTempC(tempDeviceAddress); //Temperature in degree celsius
      Serial<<tempC<<" ,C";
      //Serial<<", "<<DallasTemperature::toFahrenheit(tempC)<<" F };"; //Converts tempC to F -removed
    }
  }
    
  Serial<<"\n\r"; //seperates data lines
  
  delay(100);
  }
  
