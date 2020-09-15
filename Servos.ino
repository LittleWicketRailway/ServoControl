#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
#include <CMRI.h>
#include <Auto485.h>

#define CMRI_ADDR 1
#define DE_PIN 2
#define numServos 1 //The number of servos connected

Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver(); //setup the board address 0
Auto485 bus(DE_PIN); // Arduino pin 2 -> MAX485 DE and RE pins
CMRI cmri(CMRI_ADDR, 24, 48, bus);

int Status[numServos]; //Create a table to hold the status of each turnout, signal, etc.
int Throw[numServos]; //Create a table to hold the throw value for each servo
int Close[numServos]; //Create a table to hold the close value for each servo

void setup() {
  Serial.begin(9600);
  bus.begin(9600);
  pwm.begin();
  pwm.setPWMFreq(50);  // This is the maximum PWM frequency

  //SET THE THROW AND CLOSE VALUES FOR EACH SERVO BASED ON THE CALIBRATION PROCESS
  
  //Servo connection 0 - point motor
  Throw[0] = 1015;
  Close[0] = 1800;
}

void loop(){
   cmri.process();

    for (int i = 0; i < numServos; i++) {
        Status[i] = (cmri.get_bit(i));
        if (Status[i] == 1){
            pwm.writeMicroseconds(i, Throw[i]);
        }
        else {
            pwm.writeMicroseconds(i, Close[i]);
        }
    }
}
