/*Code to control a stepper motor with TMC22** stepper motor driver, AccelStepper library and Arduino: continuous rotation.*/
// Include the AccelStepper library:
#include <AccelStepper.h>
/// Define stepper motor connections and motor interface type. Motor interface type must be set to 1 when using a driver:
// Stepper 1 [upper left board]
#define enPin_1 12
#define ms1Pin_1 11 
#define ms2Pin_1 10   
#define stepPin_1 9   
#define dirPin_1 8    
#define motorInterfaceType_1 1

// Stepper 2 [upper right board]
/*
#define enPin_2 7
#define ms1Pin_2 6 
#define ms2Pin_2 5    
#define stepPin_2 3   
#define dirPin_2 2     
#define motorInterfaceType_2 1
*/

//spreadPin controls modi between 0=stealthChop and 1=spreadCycle of all connected stepper drivers (TMC220x)
#define spreadPin 4  

/// Create instances of the AccelStepper class:
AccelStepper stepper_1 = AccelStepper(motorInterfaceType_1, stepPin_1, dirPin_1);

/// Define global variables
float steps_per_second_p1 = 0;
float total_steps_p1 = 0;

void setup() {
  //Setup Serial
  Serial.begin(230400);
  //Deactivate driver (LOW active)
  pinMode(enPin_1, OUTPUT);
  digitalWrite(enPin_1, HIGH);
  // Set dirPin to LOW
  pinMode(dirPin_1, OUTPUT);
  digitalWrite(dirPin_1, LOW); 
  // Set stepPin to LOW
  pinMode(stepPin_1, OUTPUT);
  digitalWrite(stepPin_1, LOW);
  // Set modi between 0=stealthChop and 1=spreadCycle of all connected stepper drivers (TMC220x)
  pinMode(spreadPin, OUTPUT);
  digitalWrite(spreadPin, LOW);
  
}

void loop() {
  
  if (Serial.available() > 0) {
    //Read the incoming byte:
    String command_data = Serial.readStringUntil('\n');

    String steps_per_second = getValue(command_data, '_',0);
    String total_steps = getValue(command_data, '_',1);
    String microstepping = getValue(command_data, '_',2);
  
    steps_per_second_p1 = steps_per_second.toFloat();
    total_steps_p1 = total_steps.toFloat();

    if (microstepping == "8"){
      //Deactivate driver (LOW active)
      digitalWrite(enPin_1, HIGH);
      // Set stepper 1 to 1/8 microstepping
      pinMode(ms1Pin_1, OUTPUT);
      digitalWrite(ms1Pin_1, LOW);
      pinMode(ms2Pin_1, OUTPUT);
      digitalWrite(ms2Pin_1, LOW);      
    }

    if (microstepping == "16"){
      //Deactivate driver (LOW active)
      digitalWrite(enPin_1, HIGH);
      // Set stepper 1 to 1/16 microstepping
      pinMode(ms1Pin_1, OUTPUT);
      digitalWrite(ms1Pin_1, HIGH);
      pinMode(ms2Pin_1, OUTPUT);
      digitalWrite(ms2Pin_1, HIGH);
    }

    //Activate driver (LOW active)
    digitalWrite(enPin_1, LOW);
    // Set current position to zero:
    stepper_1.setCurrentPosition(0);
    // Set the maximum speed in steps per second:
    stepper_1.setMaxSpeed(4000);
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
