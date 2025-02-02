// Libraries 
#include <Servo.h>
#include <Wire.h>
#include <Keyboard.h>

#define S0 4
#define S1 5
#define S2 6
#define S3 7
#define S4 3 
#define sensorOut 8
#define button 13

/*ADDED CODE*/
#define servo_pin 9
/*ENDS HERE*/

int redFrequency = 0;
int greenFrequency = 0;
int blueFrequency = 0;
int clearFrequency = 0;
int counter = 0;
int i = 0;       // Counter variable for taking initial red reading 

int startTime;
double currentTime;
double timeDiff;

const int stirrer_rot_speed = 255;

const int trans_ctrl = 10; // Connect pwm pin to pin 10
const int motor = 11;

/*Added*/

Servo myservo; //servo motor
bool released = false;
bool pressed_down = false;
bool waiting = true; 

bool logging = false; // Alternating between T and F after every press of the button

const int max_angle = 120; 
const int safe_angle = 50;
const int servo_delay = 40;
const int servo_interval = 10;

double initialRed = 0;
double dRed = 0;
double newStartTime = 0;

// moving average variables
double array_v[10] = {100}; // initializing
int counter_g = 0;        // initializing
int counter_r = 0;  //initalizing
int finalcounter = 0;

/*END*/

double PrintSensorReading(double initialRed){
  // Setting RED (R) filtered photodiodes to be read
    digitalWrite(S2,LOW);
    digitalWrite(S3,LOW);

    // Reading the output frequency
    redFrequency = pulseIn(sensorOut, LOW);
    dRed = abs(redFrequency - initialRed);

    // Setting GREEN (G) filtered photodiodes to be read DO WE NEED THESE
    digitalWrite(S2,HIGH);
    digitalWrite(S3,HIGH);

    // Reading the output frequency
    greenFrequency = pulseIn(sensorOut, LOW);

    // Setting BLUE (B) filtered photodiodes to be read
    digitalWrite(S2,LOW);
    digitalWrite(S3,HIGH);

    // Reading the output frequency
    blueFrequency = pulseIn(sensorOut, LOW);

    // Setting Clear (C) filtered photodiodes to be read
    digitalWrite(S2,HIGH);
    digitalWrite(S3,LOW);

    // Reading the output frequency
    clearFrequency = pulseIn(sensorOut, LOW);

    // Print out the sensor readings 
    Serial.print("R: "); Serial.print(redFrequency, DEC); Serial.print(" ");
    Serial.print("G: "); Serial.print(greenFrequency, DEC); Serial.print(" ");
    Serial.print("B: "); Serial.print(blueFrequency, DEC); Serial.print(" ");
    Serial.print("C: "); Serial.print(clearFrequency, DEC); Serial.print(" ");
    Serial.println(" ");

    return dRed;
}

void setup() {
  // Color Sensor Code
  // Setting the outputs
  pinMode(S0, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);
  pinMode(S4, OUTPUT); // transistor ?

  // Setting the sensorOut as an input
  pinMode(sensorOut, INPUT);

  pinMode(motor, OUTPUT);
  analogWrite(motor, 0);

  pinMode(trans_ctrl, OUTPUT);

  digitalWrite(13, INPUT_PULLUP);

  // Setting frequency scaling to 20%
  digitalWrite(S0,HIGH);
  digitalWrite(S1,LOW);

  // Servo code (ADDED)
  myservo.attach(servo_pin);
  myservo.write(max_angle); // initializes angle to 120 

  Serial.begin(9600);         // initialize serial

  Serial.println("Checking for TCS3200 color sensor..."); // Initial message

  // Check the color sensor connection until connected 
  while (true) {
    // Set S2 and S3 to detect red frequency
    digitalWrite(S2, LOW);
    digitalWrite(S3, LOW);

    // Measure the frequency on the sensorOut pin
    unsigned long pulseLength = pulseIn(sensorOut, LOW, 500); // 100ms timeout

    if (pulseLength > 0) {
      // If a valid pulse is detected, the sensor is connected
      Serial.println("Color sensor detected");
      break; // Exit the loop
    } else {
      // No pulse detected, sensor might not be connected
      Serial.println("No color sensor detected");
    }

    delay(2000); // Check every 2 second
  }
}

void loop() {

/***** ADDED CODE *******/
  //     i = min ang; i <= max ang; i ++
  int servoButton = digitalRead(button);

  if (!servoButton){ //if the button is pressed down 
    pressed_down = true;
    while (pressed_down){ //while the button is held down 
      servoButton = digitalRead(button);
      if (servoButton){ //if the button is released 
        released = true;
        pressed_down = false;
        waiting = false;
        logging = !logging; // Control if the button is to start/stop recording
         // 
        analogWrite(motor, 255); 
        i = 0; // Reset the counter for initial red frequency values
      }
    }
  }
    
  if (released && logging){ //if button has been pressed and released 
     Serial.println("\nREACTION REACTION REACTION");
    for (int i = max_angle ; i>-20 ; i -= servo_interval){
      //i sets the degree of rotation for the servo - must change it depending on delay (speed) set
      if(i < 0) { myservo.write(0); }
      myservo.write(i);
      //delays to give time to the servo to rotate -- reduce number inside to make it spin faster (controls the speed)
      delay(servo_delay);
    }
    delay(servo_delay+1000);
    digitalWrite(S4, HIGH);
    
    //safe angle, after the servo goes 120, goes back to 50 degrees
    for (int i = -5 ; i<safe_angle ; i += servo_interval/2){
      myservo.write(i); //min_angle
      delay(servo_delay);
    }

    digitalWrite(S2,LOW);
    digitalWrite(S3,LOW);

    // Have a 3 s delay between pressing the button and adding the starting chemical
    while (i < 6) {
      i++;
      initialRed = pulseIn(sensorOut, LOW); // obtain initial red frequency values
      Serial.print("R: "); Serial.print(initialRed, DEC); Serial.println(" "); // Print out the red value
      delay(500); //Delay 0.5 sec between each reading 
    }

    newStartTime = millis(); // Obtain a new starttime
    Serial.println("Reaction Starts Now!");
    released = false;
    newStartTime = currentTime;
    delay(1000); //Delay 1 sec before first initial red colour reading 
    initialRed = pulseIn(sensorOut, LOW); // obtain initial red frequency value to compare with current
  }  // END OF ADDED CODE
  else if (waiting){ //Print WAITING until button is pressed 
      // Serial.println("\nWAITING");
      delay(500);
    }

  else if (logging == false){ //Skip the printing part if logging is false 
      // Serial.println("\nWAITING");
      return;
    }

/***** ENDS HERE *******/
  else{
    // Timer
    currentTime = millis();
    timeDiff = (currentTime - newStartTime)/1000;

    // // if(buttonValue == LOW){
    // //   Serial.print("REACTION REACTION REACTION REACTION");
    // //   Serial.println(" ");
    // // }

    dRed = PrintSensorReading(initialRed);
    delay(300); // adjust how frequently you want the colours to update; decided 0.3 s was optimal
    Serial.print(dRed);

///////////////////////////////////////////////////////////////////////////////////////////////
  /*
    this is using an arbitrary average value of 50
    the assumption is that once dRed hits 60, its going to continuously give values equal to 60 or below that, to which then
    the code will begin to check the values until per every set of 10, there is more then 3 values below the arbitrary average
    value we set

    NOTES: PLEASE REAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAD
    1. dRed is a weird value, so like it gives numbers anywhere from 4 to 80 so it was kinda hard to test with that
        i had to use redFrequency afterwards, and its way easier, the bounds i set are arbitrary so change them to whatever works, or the variable too
    2. so it does work in theory, as in it takes ten numbers, discards the first one for the most recent one, and keeps checking 
        what the number is and changes the counter accordingly, as tested
    3. did some changes, mainly in the loops but it should work. I commented out the first if statement, which was meant to start checking as soon as the frequency drops below
        a certain value so that code would be faster, but thats cause I was testing, i think it would be good to have when actually testing using the chemicals

  */
  //if (redFrequency < 200) { ---------------- this one right here, i commented out
      // if there is more than 3 occurances of a value less then the arbitrary average per set of ten then end
      if (counter_r >= 3){
        analogWrite(trans_ctrl, 255);
        digitalWrite(LED_BUILTIN, HIGH);

        Serial.print("|| Time Diff:"); Serial.print(timeDiff, DEC);
        Serial.println(" ");
        analogWrite(motor, 0);

        Serial.print("Reaction Over!!");
        Serial.println(" ");
        Serial.print("Final Time Diff:"); Serial.print(timeDiff, DEC);
        digitalWrite(S4, LOW);
  
        while(true){
          finalcounter += 1;
          dRed = PrintSensorReading(initialRed);
          delay(300); // adjust how frequently you want the colours to update; decided 0.3 s was optimal
          Serial.print(dRed);
          if (finalcounter == 50){
            while(true);
          }
        }
      }

      // if the value is less then the average requirement, add 1 to the counter 
      else if (redFrequency < 5){ // 53){ // arbitrary value of 50
        //if (array_v[counter_g] > 50){
          counter_r++;
          array_v[counter_g] = redFrequency;
          for(int i = 0; i < 10; i++)
            {
              Serial.print(array_v[i]);
              Serial.print("||");
            }
        //}
      }

      // if the value is greater then the average
      else if (redFrequency >= 5){// 53){ // arbitrary value of 50
        // if an average value stored which is less then 50, then reduce the counter by 1
        // this means that the set of ten will have one less
        if (array_v[counter_g] < 5){
          if (counter_r > 0){
            counter_r--;
          }
          array_v[counter_g] = redFrequency;
        }
      }
    
      // add 1 to the counter 
      counter_g++;
      //reset
      if (counter_g > 9){
        counter_g = 0;
      }
 // }

///////////////////////////////////////////////////////////////////////////////////////////////

  analogWrite(trans_ctrl, 255);
  digitalWrite(LED_BUILTIN, HIGH);

  Serial.print("|| Time Diff:"); Serial.print(timeDiff, DEC);
  Serial.println(" ");
  }
}