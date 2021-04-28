/*Example sketch to control a stepper motor with A4988 stepper motor driver, AccelStepper library and Arduino: continuous rotation. More info: https://www.makerguides.com */
// Include the AccelStepper library:
#include <AccelStepper.h>
/// Define stepper motor connections and motor interface type. Motor interface type must be set to 1 when using a driver:
// Stepper 1
#define enPin_1 8     //Shield v3 x-axis
#define dirPin_1 5    //Shield v3 x-axis
#define stepPin_1 2   //Shield v3 x-axis
#define motorInterfaceType_1 1
/// Create instances of the AccelStepper class:
AccelStepper stepper_1 = AccelStepper(motorInterfaceType_1, stepPin_1, dirPin_1);

/// Define global variables
float steps_per_second_p1 = 0;
float total_steps_p1 = 0;

void setup() {
  //Setup Serial
  Serial.begin(230400);
  //Set pin modes
  pinMode(enPin_1, OUTPUT);
  digitalWrite(enPin_1, HIGH); //deactivate driver (LOW active)
  pinMode(dirPin_1, OUTPUT);
  digitalWrite(dirPin_1, LOW); //LOW or HIGH
  pinMode(stepPin_1, OUTPUT);
  digitalWrite(stepPin_1, LOW);

  digitalWrite(enPin_1, LOW); //activate driver
  
  // Set the maximum speed in steps per second:
  stepper_1.setMaxSpeed(2000);

  //Set Current Position
  stepper_1.setCurrentPosition(0);
}

void loop() {
  
  if (Serial.available() > 0) {
    // read the incoming byte:
    String command_data = Serial.readStringUntil('\n');

    String steps_per_second = getValue(command_data, '_',0);
    String total_steps = getValue(command_data, '_',1);
  
    steps_per_second_p1 = steps_per_second.toFloat();
    total_steps_p1 = total_steps.toFloat();
    stepper_1.setCurrentPosition(0);
  }

  if (stepper_1.currentPosition() != total_steps_p1) {
     stepper_1.setSpeed(steps_per_second_p1);
     stepper_1.runSpeed();
  }
  
}

String getValue(String data, char separator, int index)
{
    int found = 0;
    int strIndex[] = { 0, -1 };
    int maxIndex = data.length() - 1;

    for (int i = 0; i <= maxIndex && found <= index; i++) {
        if (data.charAt(i) == separator || i == maxIndex) {
            found++;
            strIndex[0] = strIndex[1] + 1;
            strIndex[1] = (i == maxIndex) ? i+1 : i;
        }
    }
    return found > index ? data.substring(strIndex[0], strIndex[1]) : "";
}
