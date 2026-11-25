#include <Wire.h>
#include <Zumo32U4.h>

// Create objects for access to the Zumos hardware:
Zumo32U4Motors motors;
Zumo32U4LineSensors lineSensors;
Zumo32U4OLED oled;
Zumo32U4ButtonA buttonA;
 
#define NUM_SENSORS 3                       // Number of line sensors is defined before compile
uint16_t lineSensorValues[NUM_SENSORS];     // Positive int array of 16 bits, split in number of line sensors for storing their values

int speed = 100;
int tape = 700;
int whiteTape = 100;
int stage = 0;
int previousStage = -1;
int whiteCount = 0;

void setup()  
{
  // Initializes hardware                              
  lineSensors.initThreeSensors();
  Serial.begin(9600); // 9600 bits/second transfer rate to computer when connected via cable
}

// Prints the readings from the three line sensors to the Serial Monitor
void printReadingsToSerial()
{
  // Array used to store sensor data
  char buffer[80];

  // Format the sensor readings into a single string
  sprintf(buffer, "%4d %4d %4d\n",
    lineSensorValues[0], // left sensor
    lineSensorValues[1], // center sensor
    lineSensorValues[2]  // right sensor
  );

  // Send the formatted string to the Serial Monitor
  Serial.print(buffer);
}

void readLineSensors()
{
  lineSensors.read(lineSensorValues, QTR_EMITTERS_ON);
  printReadingsToSerial(); // only relevant when manually measureing thresholds
}
 
void stop()
{
  motors.setSpeeds(0,0);
}
 
void forward()
{
  motors.setSpeeds(speed, speed);
}

void turnLeft()
{
  int turnspeed = 200;
  motors.setSpeeds(-turnspeed, turnspeed);
}

void printOLED(String s0, String s1)
{
  oled.clear();
  oled.print(s0);
  oled.gotoXY(0,1);
  oled.print(s1);
}

bool atTape()
{
  if (lineSensorValues[0] >= tape || lineSensorValues[1] >= tape || lineSensorValues[2] >= tape)
  {
    return true;
  }
  else
    return false;
}

bool atWhite()
{
  if (lineSensorValues[1] < whiteTape)
  {
    return true;
  }
  else 
    return false;
}

void start()
{
  if(buttonA.isPressed())
  {
    countDown();
    stage = 1;
  }
}

void countDown()
{
  int timer = 5;
  while(timer > 0)
  {
    printOLED("T-", String(timer));
    timer --;
    delay(600);
  }
}

void locate()
{
  readLineSensors();

  if(atTape()){
    stop();
    stage = 2;
  }

  else
    forward();
}

void hugWall()
{
  readLineSensors();
    
  if (atTape())
  {
    turnLeft();
  }
  else
  {
    motors.setSpeeds(4*speed, 0.5*speed);
  }

  if (atWhite())
  {
    whiteCount ++;
  }

  if (whiteCount > 50)
  {
    stage = 3;
    whiteCount = 0;
  }
}

void win()
{
  stop();
  delay(3000);
  stage = 0;
}


void loop()
{
  while(stage != previousStage)
  {

    switch(stage) // Prints current stage to screen
    {
      case 0:
        printOLED("Press A", "to start");
        break;

      case 1:
        printOLED("Locating", "maze");
        break;

      case 2:
        printOLED("Hugging", "wall");
        break;

      case 3:
        printOLED("You", "Win");
        break;
    }

    previousStage = stage;
  }

  switch(stage) // Controls the Zumo
  {
    case 0:
      start();
      break;

    case 1:
      locate();
      break;

    case 2:
      hugWall();
      break;

    case 3:
      win();
  }
}