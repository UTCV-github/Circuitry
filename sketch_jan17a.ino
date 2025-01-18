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

const int max_angle = 120;
//const int min_angle = 10;
const int safe_angle = 50;
const int servo_delay = 40;
const int servo_interval = 5;

double initialRed = 0;
double dRed = 0;
double newStartTime = 0;
/*END*/

void setup() {
  // Color Sensor Code
  // Setting the outputs
  pinMode(S0, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);
  pinMode(S4, OUTPUT);
  
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
  myservo.write(max_angle); //initializes the angle to 120
  
  Serial.begin(9600);         // initialize serial
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
        digitalWrite(S4, HIGH);
        analogWrite(motor, 255);
      }
    }
  }

  if (released){ //if button has been pressed and released 
    Serial.println("\nREACTION REACTION REACTION");
    for (int i = max_angle ; i>-5 ; i -= servo_interval){
      //i sets the degree of rotation for the servo - must change it depending on delay (speed) set
      myservo.write(i);
      //delays to give time to the servo to rotate -- reduce number inside to make it spin faster (controls the speed)
      delay(servo_delay);
    }
    
    //safe angle, after the servo goes 120, goes back to 50 degrees
    for (int i = -5 ; i<safe_angle ; i += servo_interval){
      myservo.write(i); //min_angle
      delay(servo_delay);
    }

    digitalWrite(S2,LOW);
    digitalWrite(S3,LOW);
    Serial.println("Reaction Starts Now!");
    released = false;
    newStartTime = currentTime;
    delay(1000); //Delay 1 sec before first initial red colour reading 
    initialRed = pulseIn(sensorOut, LOW); // obtain initial red frequency value to compare with current
  }

  else if (waiting){ //Print WAITING until button is pressed 
      Serial.println("\nWAITING");
      delay(500);
    }

/***** ENDS HERE *******/
  else{
    // Timer
    currentTime = millis();
    timeDiff = (currentTime - newStartTime)/1000;

    // Setting RED (R) filtered photodiodes to be read
    digitalWrite(S2,LOW);
    digitalWrite(S3,LOW);
  
    // Reading the output frequency
    redFrequency = pulseIn(sensorOut, LOW);
    dRed = abs(redFrequency - initialRed);
    
    // Setting GREEN (G) filtered photodiodes to be read
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

    delay(300); // adjust how frequently you want the colours to update; decided 0.3 s was optimal

    // if(buttonValue == LOW){
    //   Serial.print("REACTION REACTION REACTION REACTION");
    //   Serial.println(" ");
    // }

    Serial.print("R: "); Serial.print(redFrequency, DEC); Serial.print(" ");
    Serial.print("G: "); Serial.print(greenFrequency, DEC); Serial.print(" ");
    Serial.print("B: "); Serial.print(blueFrequency, DEC); Serial.print(" ");
    Serial.print("C: "); Serial.print(clearFrequency, DEC); Serial.print(" ");
    Serial.println(" ");
    Serial.print(dRed);

  if ((dRed >= 35) && (timeDiff>20)){
    counter += 1;
    if (counter == 3){
      analogWrite(motor, 0);
      Serial.print("Reaction Over!!");
      Serial.println(" ");
      Serial.print("Final Time Diff:"); Serial.print(timeDiff, DEC);
      digitalWrite(S4, LOW);
      while(true);
    }
  } else{ //Ensures change occurs 3 consecutive times 
    counter = 0;
  }

  analogWrite(trans_ctrl, 255);
  digitalWrite(LED_BUILTIN, HIGH);

  Serial.print("|| Time Diff:"); Serial.print(timeDiff, DEC);
  Serial.println(" ");
  }
}