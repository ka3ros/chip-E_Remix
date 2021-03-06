/************************************************
*  KaeRoS                                       *
*  Makerbot's Chip-E Remix challenge            *
*                                               *
*                                               *
*                 ______________                *
*                /              |               *
*               |   ____ ____   |               *
*               |  [   ] [   ]  |               *
*               |  [___] [___]  |               *
*               |               |               *
*               |               |               *
*               |      o o      |               *
*               |               |               *
*               |       o       |               *
*                ---------------                *
*                   ||     ||                   *
*                   ||     ||                   *
*                   ||     ||                   *
*              -------     -------              *
*              -------     -------              *
*                                               *
*************************************************/
#include <math.h>
#include <stdlib.h>

#include <SoftwareSerial.h>
// I added a period parameter to the home(unsigned int period=500) method to have a smoother move
// with each call to home() my chip-e had an epyleptic shock
// while 4 steps don't end on full flat feet so I really needed home()
#include <ChipE.h> 

//HC-SR04 ultrasound sensor
#include <NewPing.h>
#define TRIGGER 6
#define ECHO    7
#define MIN_DISTANCE 2
#define MAX_DISTANCE 300
NewPing mySonar(TRIGGER, ECHO, MAX_DISTANCE);

// Nokia 5110 LCDs
#define L_CLK 26
#define L_DIN 25
#define L_DC  24
#define L_CE  23
#define L_RST 22

#define R_CLK 31
#define R_DIN 30
#define R_DC  29
#define R_CE  28
#define R_RST 27
#include <dual5110Lcd.h>

// bluetooth comm
#define RxD 15 //RX3
#define TxD 14 //TX3

// servo control
/*        |       O       |
 * YR ==> |               | <== YL
 *         ---------------
 *            ||     ||
 *            ||     ||
 *            ||     ||
 * RR ==>   -----   ------  <== RL */
#define PIN_RR 2  // Ankle//Roll Right
#define PIN_RL 3  // Ankle//Roll Left
#define PIN_YR 4  // HipYaw Right
#define PIN_YL 5  // Hip/Yaw Left
#define TRIM_RR -3  // Trim on the right ankle (adjust +/- as necessary)
#define TRIM_RL 0   // Trim on the left ankle (adjust +/- as necessary)
#define TRIM_YR +4  // Trim on the right hip (adjust +/- as necessary)
#define TRIM_YL 0   // Trim on the left hip (adjust +/- as necessary)

#define BTCommandTurnLeft
#define BTCommandTurnRight
#define BTCommandWalkFront
#define BTCommandWalkBack

SoftwareSerial HC06(RxD,TxD);
ChipE chip;
// calibration: distance walked with 4 forward steps
// at first, let's consider forward~=backward
#define STEPWALK 4
#define DISTANCE_PER_STEP 2 // cm


void goFront(unsigned short steps=1)
{
  unsigned int distance;
  distance=mySonar.ping_cm();
  lcd5110[L5110].clearDisplay();
  lcd5110[L5110].setTextColor(BLACK);
  lcd5110[L5110].setCursor(0,0);
  lcd5110[L5110].print(distance);
  lcd5110[L5110].println(" cm");
  lcd5110[L5110].display();
  if(distance==0)
    distance=MAX_DISTANCE;
  Serial.print(distance);
  Serial.println(" cm");
  if(distance>(STEPWALK*DISTANCE_PER_STEP)+MIN_DISTANCE)
  {
    chip.walk(STEPWALK,2000,FORWARD);
    chip.home(1000);
  }
  else
  {
    Serial.print("Obstacle detected in ");
    Serial.print(distance);
    Serial.println(" cm. Turning to the left");
    chip.turn(2,2000,LEFT);
  }
}

void setup()
{
  Serial.begin(9600);

  // init 4 servos positions (All at 90°)
  chip.init( PIN_YL, PIN_YR, PIN_RL, PIN_RR );
  chip.setTrims( TRIM_YL, TRIM_YR, TRIM_RL, TRIM_RR ); //Set the servo trims
  chip.home(); //Move feet to home position
  delay(5000);

  // init both LCDs
  setupN5110LCDs();
  // init eyes
  initEyes();
  delay(3000);
//  HC06.begin(115200);
  openEyes();
  Serial.print("Minimal distance to walk: ");
  Serial.print((STEPWALK*DISTANCE_PER_STEP)+MIN_DISTANCE);
  Serial.println(" cm");
}
unsigned short loops=0;
void loop()
{
  loops++;
  Serial.print(loops);
  Serial.print(" : ");
  goFront();
  delay(1000);
  
/*  char what2do="";
  if (HC06.available())
  {
    what2do=HC06.read();
    Serial.write(what2do);
    switch(what2do)
    {
      case BTCommandTurnLeft:
        look(LookLeft);
        chip.turn( 3, 2000, LEFT );
      break;
      case BTCommandTurnRight:
        look(LookRight);
        chip.turn( 3, 2000, RIGHT );
      break;
      case BTCommandWalkFront:
        chip.walk( 4, 2000, FORWARD );
      break;
      case BTCommandWalkBack:
        chip.walk( 4, 2000, BACKWARD );
      break;
    }
  }*/
}
