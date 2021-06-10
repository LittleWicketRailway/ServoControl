/*********************************************************************************************** 
  This is a sketch to calibrate servos and obtain the minimum and maximum range settings for
  use with points. These settings can then be added to the Servos.ino sketch to set the
  thrown and closed values for yoru points.

  The video tutorials showing this sketch in action are avaiable at the Little Wicket Railway
  YouTube Channel here: ------> https://www.youtube.com/watch?v=fHAPXAcW9ZQ
  
  The following libraries must be installed in your Arduino IDE for it to work.

    -  Adafruit PWM Library by Adafruit Industries
         Menu > Tools > Manage Library
         search for and install "Adafruit PWM"
        
    -  Auto485 Serial Communication library by Michael Adams (Madleech)
         https://github.com/madleech/Auto485/zipball/master
         Menu > Sketch > Include Library > Add .ZIP library

    -  ArduinoCMRI library by Michael Adams (Madleech)
         https://github.com/madleech/ArduinoCMRI/zipball/master
         Menu > Sketch > Include Library > Add .ZIP library

    This script has been updated since the original video was uploaded to include the option
    to change to a different servo without the need to change and upload the code for each one.

    Servos can now all be wired up and while in situ by using the <num>s command within the
    serial monitor window to change the port you are calibrating.

    Help screen has also been updated with more context help and examples.

    Original code by Rob @ Little Wicket Railway
    Updated by Anthony Lloyd @ Branksdown Junction Model Railway / Sig-naTrak by GFB Designs
 **********************************************************************************************/
 

#include <Wire.h>                                                                   // This library allows you to communicate with I2C / TWI devices
#include <Adafruit_PWMServoDriver.h>                                                // This library allows you to use the Adafruit PWM Servo Driver

// called this way, it uses the default address 0x40
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();

#define CENTERPOINT 1500             // DEFAULT = 1500                              // SET THE CENTERPOINT POSITION
#define SERVO_FREQ 50                                                               // Analog servos run at ~50 Hz


const int numServos  = 16;           // DEFAULT = 16 (One full PCA9685 board)       // SET THE NUMBER OF SERVOS CONNECTED TO YOUR ARDUINO
int DefaultThrow     = 1700;         // DEFAULT = 1700                              // SET THE DEFAULT THROWN POSITION FOR THE ARRAY TABLE
int DefaultClose     = 1300;         // DEFAULT = 1300                              // SET THE DEFAULT CLOSED POSITION FOR THE ARRAY TABLE
String RailwayName   = "Little Wicket Railway";
                                     // DEFAULT = "Your Model Railway"              // SET THIS TO DISPLAY YOUR MODEL RAILWAY NAME IN THE INTRODUCTION SCREEN
                                                                                    //  This is purely for fun and does not alter any other part of the process.





/*********************************************************************************************** 
  ONLY EDIT BELOW THIS LINE IF YOU ARE COMFORTABLE WITH EDITING CODE AND KNOW WHAT YOU ARE DOING

  CHANGING THE CONTENT BELOW THIS LINE CAN BREAK THE SCRIPT, AND MAY PREVENT IT FROM UPLOADING
 **********************************************************************************************/











const String vers    = "2.0.0";                                                     // Software Version Number
int Throw[numServos];                                                               // Create a table array to store and display the throw value for each servo
int Close[numServos];                                                               // Create a table array to store and display the close value for each servo
String readString;                                                                  // Variable to read what is entered into the Serial Monitor
int servonum = 0;                                                                   // Variable to hold the current servo being calibrated

int pos;                                                                            // Variable to hold the current servo arm position.
bool cmdok = false;                                                                 // Internal Error Handling variable - Set to TRUE when a command is detected
                                                                                    //  and understood, otherwise stays FALSE and shows the error message.
char cmdrun;                                                                        // Internal Error Handling variable - Set to the command run during testing.



void setup() {                                                                      // Function runs once only at bootup
  Serial.begin(9600);                                                               // Opens the Serial Port at 9600 Baud Rate
  pos=CENTERPOINT;                                                                  // Sets initial servo arm position to CentrePOINT
  pwm.begin();                                                                      // Opens PWM Library
  pwm.setOscillatorFrequency(25000000);                                             // Sets PWM Clock Rate
  pwm.setPWMFreq(SERVO_FREQ);                                                       // Sets PWM Frequency Rate using constant above (Analog servos usually run at ~50 Hz)
  clear();                                                                          // Clears the Serial Monitor Screen
  for (int s=0; s<numServos; s++) {                                                 // LOOP TO SET DEFAULT ARRAY VALUES FOR THROW/CLOSE
    Close[s] = DefaultClose;
    Throw[s] = DefaultThrow;
  }
  intro();                                                                          // Displays Initial Intro
  help();                                                                           // Displays the Help screen
  pwm.writeMicroseconds(servonum, pos);
  Serial.print("Servo : "); Serial.print(servonum); Serial.print("  Pos : "); Serial.println(pos);
}

void loop() {                                                                       // Function loops forever once Setup Boot has completed
  while (Serial.available()) {
    char c = Serial.read();                                                         // gets one byte from serial buffer
    readString += c;                                                                // makes the string readString
    delay(2);                                                                       // slow looping to allow buffer to fill with next character
  }

  if (readString.length() >0) {                                                     // Makes sure that something has been typed in serial monitor

                                                                                    // = x X COMMAND - Set servo arm to this position
    if(readString.indexOf('=') !=-1 || readString.indexOf('x') !=-1 || readString.indexOf('X') !=-1) {
      pos = readString.toInt();
      cmdok = true; cmdrun='=';
    }
                                                                                    // + COMMAND - Add n to servo arm position (turns right)
    else if(readString.indexOf('+') >0) {
      pos = pos + readString.toInt();
      cmdok = true; cmdrun='+';
    }
                                                                                    // - COMMAND - Remove n to servo arm position (turns left)
    else if(readString.indexOf('-') >0) {
      pos = pos - readString.toInt();
      cmdok = true; cmdrun='-';
    }
                                                                                    // S COMMAND - Change Servo Port and set position to 1500 (centre)
    else if(readString.indexOf('s') >0 || readString.indexOf('S') >0) {
      servonum = readString.toInt();                                                // Change Servo Address
      pos = CENTERPOINT;                                                            // Set centrepoint of servo
      Serial.print("Now calibrating servo on port: "); Serial.print(servonum); Serial.println("  POSITION RESET TO CENTRE POINT.");
      cmdok = true; cmdrun='s';
    }
                                                                                    // H COMMAND - Display Help Screen
    else if(readString.indexOf('h') !=-1 || readString.indexOf('H') !=-1){          //
      help();                                                                       // Help screen is stored in the help(); function.
      cmdok = true; cmdrun='h';
    }
                                                                                    // C COMMAND - Save the current servo/position to the CLOSE Table Array 
    else if(readString.indexOf('c') !=-1 || readString.indexOf('C') !=-1) {
      Close[servonum] = pos;
      Serial.print("Servo : "); Serial.print(servonum); Serial.print("  Pos : "); Serial.print(pos); Serial.println("  - CLOSE SAVED COMMAND TABLE");
      cmdok = true; cmdrun='c';
    }
                                                                                    // T COMMAND - Save the current servo/position to the THROW Table Array 
    else if(readString.indexOf('t') !=-1 || readString.indexOf('T') !=-1) {
      Throw[servonum] = pos;
      Serial.print("Servo : "); Serial.print(servonum); Serial.print("  Pos : "); Serial.print(pos); Serial.println("  - THROW SAVED TO COMMAND TABLE");
      cmdok = true; cmdrun='t';
    }
                                                                                    // D COMMAND - Display the THROW & CLOSE Table Arrays in code formated
    else if(readString.indexOf('d') !=-1 || readString.indexOf('D') !=-1) {         // for pasting into the Servos.ini script.
      display();                                                                    // Display Table is stored in the display(); function.
      cmdok = true; cmdrun='d';
    }
                                                                                    // ERROR HANDLING - IS THE COMMAND UNDERSTOOD (Set TRUE by one of the IF statements above)
    if (cmdok == true ){                                                            //   - YES - VALID COMMAND AND SYNTAX 
      pwm.writeMicroseconds(servonum, pos);                                         //     Set Servo Arm position and display the feedback onto the Serial Monitor
      Serial.print("Servo : "); Serial.print(servonum); Serial.print("  Pos : "); Serial.println(pos);
    }
                                                                                    // ERROR HANDLING - IS THE COMMAND UNDERSTOOD (Set TRUE by one of the IF statements above)
    else {                                                                          //   - NO  - UNKNOWN COMMAND OR SYNTAX - Show Error message.
      Serial.print("Err01 : UNKNOWN COMMAND - for help, send the /H command - "); Serial.print(readString);
    }
    
    cmdok = false;                                                                  // ERROR HANDLING - Reset ready for next command
    readString="";                                                                  // Clear readString ready for next Serial Monitor input
  }
}

void clear() {                                                                      // Loops to display 100 lines of nothing to simulate CLS command
  for (int i = 0; i <= 100; i++) {
    blankLN();
  }
}

void blankLN() {                                                                    // inserts one blank line into serial monitor - used for formatting.
    Serial.println("");
}
void intro() {                                                                      // Displays introduction at start of program.
  Serial.print(RailwayName); Serial.println(" - Servo Calibration");                // "Railway Name" - Servo Calibration
  Serial.print("Version: "); Serial.println(vers);                                  // Displays Version Number
  blankLN();
}

void help() {                                                                       // Displays Help Screen
  Serial.println(" nnn +  Rotates the servo clockwise.");
  Serial.println(" nnn -  Rotates the servo anti-clockwise.");
  Serial.println(" nnn =  Rotate to specific position.");
  Serial.println(" nnn S  Switch to servo on port number. Will reset current position to centre");
  Serial.println("  /T    Save THROW for selected servo.");
  Serial.println("  /C    Save CLOSE for selected servo.");
  Serial.println("  /D    Displays Servo Configuration Table to copy into the Servos.ino file.");
  Serial.println("  /H    Shows this help screen.");
  blankLN();
  Serial.println("Adjust the servo arm to the required position.");
  Serial.println("Servo min and max can vary, try the 1000 - 2000 range to start with.");
  blankLN();
  Serial.println("WARNING: Exceeding the max range could damage the servo.");
  blankLN();
}

void display() {
  int s = 0;
  blankLN();
  Serial.println("--- COPY THIS INTO SERVO SCRIPT ---");
  blankLN();
  Serial.println("  // SET THE THROW AND CLOSE VALUES FOR EACH SERVO BASED ON CALIBRATION TESTS");
  blankLN();
  Serial.println("  // Edit these values to change the throw and close motion");
  Serial.println("  //        THROW ||||           CLOSE ||||        Add Servo descriptions here to help identify your points");
  Serial.println("  // ------------ vvvv --------------- vvvv ------ ----------------- vvvv ---------------------------------");
  // for (i = 0; i == numServos; i++) {
  for (int s=0; s<numServos; s++) {
    Serial.print("      Throw["); Serial.print(s); Serial.print("] "); if(s<10) Serial.print(" ");    Serial.print("= "); Serial.print(Throw[s]); Serial.print(";   ");
    Serial.print("Close["); Serial.print(s); Serial.print("] "); if(s<10) Serial.print(" "); Serial.print(" = "); Serial.print(Close[s]); Serial.print(";");
    Serial.print("    // Servo on Pin "); if(s<10) Serial.print(" "); Serial.print(s); Serial.println(" - ");
  }
  blankLN();
  Serial.print("  // Number of Servos: "); Serial.println(numServos);
  Serial.println("--- COPY THIS INTO SERVO SCRIPT ---");
  blankLN();
}
