// Tracker light source.
// Version 2.0

//#define DEBUG
#include <Servo.h> 


int DEBUG_DELAY = 500;

Servo myservo;  // create servo object to control a servo 
                // a maximum of eight servo objects can be created 

// Servo 1
enum movingDirection
{
  STOP,
  EAST,
  WEST
};
 
 //-----------------------------------------------------------------
 // // yellow/orange leads.  - This turns the motor 'east' away from 'futaba'
//-----------------------------------------------------------------
int light_sensistive_resPin = 0;
int light_digital_val = 0;
int last_light_pin0_reading = 0;
//-----------------------------------------------------------------
// Red/Black leads.  - This turns the motor 'west' towards 'futaba'
//-----------------------------------------------------------------
int light_sensistive_resPin_two = 1;
int light_digital_val_two = 0;
int last_light_pin1_reading = 0;


// Servo settings
int pos = 0;    // variable to store the servo position 
boolean atMax = 0;
int startValue = 83;
int *ptrToPosition;
int moveDirection=0;

// Optimum settings
int maxVal=0;
int maxPos=0;

//----------------------------
// CONFIGURATIONS
//----------------------------
int lightSensitivity = 10; // Light threshold.  Increase the number for sensitivity.
int motorSkips = 2;     // Motor will skip these many positions to allow for faster positioning.

//========================================================
// ----------------------------- END OF GLOBAL SETTINGS ----------------------------
//========================================================
void setup() 
{ 
  Serial.begin(9600);
  myservo.attach(9);  // attaches the servo on pin 9 to the servo object 
} 
 
 
// ======
// START
// ======
void loop() 
{
  // Higher (resistance)  means there's light.  
  getPhotoReadings();

  ptrToPosition = &startValue;
  
  /* Move and search for light source*/
  #ifdef DEBUG
      Serial.print("Pos (f):");
      Serial.println((int)*ptrToPosition);
  #endif
  determineDirToSearch();
  
  go_and_get_values(ptrToPosition);
  
  #ifdef DEBUG
      Serial.print("Pos (t)");
      Serial.println((int)*ptrToPosition);
  #endif
} 

void go_and_get_values(int *incomingVal)
{
  #ifdef DEBUG
      Serial.print("Moving:  ");
      Serial.println(moveDirection);
  #endif 
  
  if( moveDirection == WEST)
  {
    *incomingVal -= motorSkips;
    myservo.write((*incomingVal));
  }
  else if (moveDirection  == EAST)
  {
    *incomingVal += motorSkips;
    myservo.write((*incomingVal));
  }
  else
  {
    // We're at the optimum spot
  }
  delay(5);
}

void getPhotoReadings()
{
 
  // Store last readings
  last_light_pin0_reading = light_digital_val;
  last_light_pin1_reading = light_digital_val_two;
  
  // Update to latest readings.
  light_digital_val = analogRead(light_sensistive_resPin);
  light_digital_val_two = analogRead(light_sensistive_resPin_two);
  
  light_digital_val = map(light_digital_val, 0, 1023, 0, 255);
  light_digital_val_two = map(light_digital_val_two, 0, 1023, 0, 255);
  
  printValues(light_digital_val, light_digital_val_two);
  delay(DEBUG_DELAY);  
  
}

void determineDirToSearch()
{   
  boolean sensorOneGood = (last_light_pin0_reading > light_digital_val);
  boolean sensorTwoGood = (last_light_pin1_reading > light_digital_val_two);
  
  // Add the two photoresistors together.
  //int newValue = max(maxVal, (light_digital_val + light_digital_val_two));
 
 int newValue = max(light_digital_val,light_digital_val_two);
 
 int calc_threshold = abs(light_digital_val - light_digital_val_two);
 
 boolean fallswithinlimit = (lightSensitivity >= calc_threshold);
 
 motorSkips = map(calc_threshold,0,100,1,5);
 
  #ifdef DEBUG
    Serial.print("\tmotorSkips:"); 
    Serial.println(motorSkips);
   
    Serial.print("\calc_threshold:"); 
    Serial.println(calc_threshold);
 #endif
 
  /* Which sensor found the new readings?  */
  
  // Move motor in that direction, until both see the light.
  /* Move the sensor until both readings see it */


    if ((light_digital_val > light_digital_val_two) && !fallswithinlimit)
    {
        moveDirection = EAST;
    }
    else if ((light_digital_val < light_digital_val_two) && !fallswithinlimit)
    {
        moveDirection = WEST;
    }
    else
    {
      // Falls within threshold ignore.
      moveDirection = STOP;
    }
    
}


 
void printValues(int a, int b)
{
  
  #ifdef DEBUG
  Serial.print("Mapped value:");
  Serial.print(a);
  Serial.print(",");
  Serial.println(b);
  #endif
  
  delay(DEBUG_DELAY);
  
}
