#include <Streaming.h>
#include "ESC.h"
#include <ESP32Servo.h>

#define LED_PIN 13                    // Pin for the LED 
#define POT_PIN 32                    // Analog pin used to connect the potentiometer
#define ESC_PIN 4                     // Digital pin used to provide PWM to ESC

ESC myESC (ESC_PIN, 1000, 2000, 1000);         // ESC_Name (PIN, Minimum Value, Maximum Value, Arm Value)

int val;                                // variable to read the value from the analog pin

void setup() {
  Serial.begin(115200);                 //was 9600, should be 115200 by default
  analogReadResolution(12);             //j used sketch?
  pinMode(LED_PIN, OUTPUT);             // LED Visual Output
  myESC.arm();
  digitalWrite(LED_PIN, HIGH);
  delay(5000);                          // Wait for a while
}

void loop() {
  val = analogRead(POT_PIN);            // reads the value of the potentiometer (value between 0 and 1023)
  val = map(val, 0, 4096, 0, 2000);  // scale it to use it with the ESC (value between 1000 and 2000)
  myESC.speed(val);                     // sets the ESC speed according to the scaled value
  Serial.print("Analog Reading: ");
  Serial.println(analogRead(POT_PIN));
  Serial.print("Pwm Percent: ");
  Serial.println(val/2000.0*100);
  
  delay(1000);                            // Wait for a while
}
