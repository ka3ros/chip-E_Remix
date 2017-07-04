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
#ifndef math_h
#define math_h
#include <math.h>
#endif
#ifndef stdlib_h
#define stdlib_h
#include <stdlib.h>
#endif

#include <SoftwareSerial.h>
#include <ChipE.h>

//HC-SR04 ultrason
#ifndef NewPing_h
#include <NewPing.h>
#endif
#define TRIGGER 6
#define ECHO    7
#define MIN_DISTANCE 2
#define MAX_DISTANCE 300
NewPing mySonar(TRIGGER, ECHO, MAX_DISTANCE);
//call mySonar.ping_cm() (0 => too far)

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
#define PIN_RR 2 //Roll Right
#define PIN_RL 3 //Roll Left
#define PIN_YR 4 //Yaw Right
#define PIN_YL 5 //Yaw Left
#define TRIM_RR -2 //Trim on the right ankle (adjust +/- as necessary)
#define TRIM_RL 0 //Trim on the left ankle (adjust +/- as necessary)
#define TRIM_YR +4 //Trim on the right hip (adjust +/- as necessary)
#define TRIM_YL 0 //Trim on the left hip (adjust +/- as necessary)

#define BTCommandTurnLeft
#define BTCommandTurnRight
#define BTCommandWalkFront
#define BTCommandWalkBack


SoftwareSerial HC06(RxD,TxD);
ChipE chip;
// calibration: distance walked with 2 forward steps
// first, consider forward~=backward
#define STEPWALK 2
void goFront(unsigned short steps=1)
{
  if(mySonar.ping_cm()>STEPWALK+MIN_DISTANCE)
  {
    chip.walk(2,1000,FORWARD);
  }
  else
  {
    while(mySonar.ping_cm()>STEPWALK+MIN_DISTANCE)
    {
      chip.turn(2,1000,LEFT);
    }
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
}

void loop()
{
  Serial.print("Distance: ");
  Serial.print(sonarEcho());
  Serial.println(" cm");
  for(unsigned short i=10;i>0;i--)
  {
//    set_text(L5110,0,0,string(i));
    delay(1000);
  }
  set_text(L5110,0,0,"walking");
  chip.walk( 4, 2000, FORWARD );

//  blink();
  
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
