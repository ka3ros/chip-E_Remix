#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>

/* Library to drive 2 Nokia5110 LCDs
 * Max 10 chars per line
 * KaeRoS
 * FixMe: have problem with adafruit gfx lib that seem to mix
 * both screens
 */

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

#define L5110 0
#define R5110 1
#define BOTH  2

#define WIDTH  84
#define HEIGHT 48
#define CENTER HEIGHT/2
#define BORDER 2

#define eyeRad 8
#define eyeOpen HEIGHT/2-BORDER
#define eyeOpened 1
#define eyeClosed 0

#define LookCenter    0
#define LookLeft      1
#define LookRight     2
#define LookUp        3
#define LookDown      4
#define LookUpLeft    5
#define LookUpRight   6
#define LookDownLeft  7
#define LookDownRight 8

#define pupSpeed        8
#define normalLidSpeed  15
#define fastLidSpeed    8

const unsigned short LcdPins[2][5] = {
  {L_CLK,L_DIN,L_DC,L_CE,L_RST},
  {R_CLK,R_DIN,R_DC,R_CE,R_RST}
};

Adafruit_PCD8544 lcd5110[2]={
  Adafruit_PCD8544(L_CLK, L_DIN, L_DC, L_CE, L_RST),
  Adafruit_PCD8544(R_CLK, R_DIN, R_DC, R_CE, R_RST)
};
  
unsigned short eyeState[2];

unsigned short eyePos[2][2]={
  // left eye start pos
  {WIDTH/2,HEIGHT/2},
  //right eye start pos
  {WIDTH/2,HEIGHT/2}
};

void set_text(unsigned short lcd, unsigned short x, unsigned short int y, String text, int color=BLACK)
{  
  lcd5110[lcd].setTextColor(color);
  lcd5110[lcd].setCursor(x,y);
  lcd5110[lcd].println(text);
  lcd5110[lcd].display();
}
 
void clearAll()
{
  lcd5110[L5110].clearDisplay();
  lcd5110[R5110].clearDisplay();
}

void drawPup(const unsigned short eye=BOTH, const short COLOR=BLACK)
{
  lcd5110[eye].fillCircle(eyePos[eye][0],eyePos[eye][1],eyeRad,COLOR);
  lcd5110[eye].display();
}

void erasePup(const unsigned short eye=BOTH, const short COLOR=WHITE)
{
  drawPup(eye,COLOR);
}

void openEyes(const unsigned short eyes=BOTH, const unsigned short speed=normalLidSpeed)
{
  unsigned short startEye,endEye;
  unsigned short eye,i;
  if(eyes==BOTH) { startEye=0; endEye=1; }
  else { startEye=eyes;endEye=eyes;}
  for(i=0;i<eyeOpen;i++)
  {
    for(eye=startEye;eye<=endEye;eye++)
    {
      //draw upper then lower lid
      lcd5110[eye].drawLine(1,HEIGHT/2-i,WIDTH-2,HEIGHT/2-i,WHITE);
      lcd5110[eye].drawLine(1,HEIGHT/2+i,WIDTH-2,HEIGHT/2+i,WHITE);
      if(i<eyeRad)
        drawPup(eye);
      else
        lcd5110[eye].display();
      delay(speed);
    }
  }
  for(eye=startEye;eye<=endEye;eye++)
    eyeState[eye]=eyeOpened;
}

void closeEyes(const unsigned short eyes=BOTH, const unsigned short speed=normalLidSpeed)
{
  unsigned short lidPos=0; // 0 = "eye closed", HEIGHT/2-eyeRad/2 = eye open 
  unsigned short startEye,endEye;
  unsigned short eye,i;
//  return;
  if(eyes==BOTH) { startEye=0; endEye=1; }
  else { startEye=eyes;endEye=eyes;}

  //eyeOpen=Height/2-eyeRad
  for(i=eyeOpen;i>0;i-=2)
  {
    for(eye=startEye;eye<=endEye;eye++)
    {
      //draw upper then lower lid
      lcd5110[eye].drawLine(1,HEIGHT/2-i,WIDTH-2,HEIGHT/2-i,BLACK);
      lcd5110[eye].drawLine(1,HEIGHT/2+i,WIDTH-2,HEIGHT/2+i,BLACK);
      lcd5110[eye].display();
      delay(speed);
    }
    eyeState[eye]=eyeClosed;
  }
}

void blink(const unsigned short eyes=BOTH)
{
  closeEyes(eyes,fastLidSpeed);
  openEyes(eyes,fastLidSpeed);
}

void moveEyeToXY(unsigned short x, unsigned short y, unsigned short eyes=BOTH)
{
  // eyePos[0] => left eye, eyePos[0] => right eye
  // eyePos[0][0] => eyePosX,eyePos[0][1] => eyePosY
  // deplacement soit à l'horizontale soit en diagonale
  unsigned short eye,eyeX,eyeY,startEye,endEye;
  float third=PI/3; // pre-computing third of PI 
  float fourth=PI/4; // pre-computing fourth of PI
  float sixth=third/2; // pre-computing sixth of PI
  float angle;

  if(eyes==BOTH) { startEye=0; endEye=1; }
  else { startEye=eyes;endEye=eyes;}
  
  while(eyePos[startEye][0]!=x && eyePos[startEye][1]!=y)
  {
	  angle=atan2(y-eyePos[startEye][1],x-eyePos[startEye][0]);
	  /* Deplacements: 
    * de 11Pi/4 à 0 et de 0 à Pi/6 => droite
	  * de Pi/6 à Pi/3 haut/droite
	  * de Pi/3 à 2Pi/3 haut
	  * de 2Pi/3 à 5Pi/6 haut/gauche
    * de 5Pi/6 à 7Pi/6 gauche
    * de 7Pi/6 à 4Pi/3 bas/gauche
    * de 4Pi/3 à 5Pi/3 bas
    * de 5Pi/3 à 11Pi/6 bas/droite
    * priorité aux verticales et horizontales sur les diagonales
    */
    for(eye=startEye;eye<=endEye;eye++)
    {
      erasePup(eye);
      // droite
      if( (angle>=0 && angle<=sixth) || (angle>=11*fourth && angle<2*PI))
      { eyePos[eye][0]++;}
      //haut-droite
  	  else if(angle>sixth && angle<third)
      { eyePos[eye][0]++; eyePos[eye][1]--;}
      //haut
  	  else if(angle>=third && angle<=2*third)
      { eyePos[eye][1]--;}
      //haut-gauche
  	  else if(angle>2*third && angle<5*sixth)
      { eyePos[eye][0]--; eyePos[eye][1]--;}
      //gauche
  	  else if(angle>=5*sixth && angle<=7*sixth)
      { eyePos[eye][0]--;}
      //bas-gauche
  	  else if(angle>7*sixth && angle<4*third)
      { eyePos[eye][0]--; eyePos[eye][1]++;}
      //bas
  	  else if(angle>=4*third && angle<=5*third)
      { eyePos[eye][1]++;}
      //bas-droite
  	  else if(angle>5*third && angle<11*sixth)
      { eyePos[eye][0]++; eyePos[eye][1]++;}
      drawPup(eye);
      delay(pupSpeed);
	  }
  }
}

void look(unsigned short goTo=CENTER,unsigned short eyes=BOTH)
{
  switch(goTo)
  {
    case LookCenter :
      moveEyeToXY(WIDTH/2,HEIGHT/2,eyes);
      break;
    case LookUp :
      moveEyeToXY(WIDTH/2,10,eyes);      
      break;
    case LookDown :
      moveEyeToXY(WIDTH/2,HEIGHT-10,eyes);      
      break;
    case LookLeft :
      moveEyeToXY(8,HEIGHT/2,eyes);      
      break;
    case LookRight :
      moveEyeToXY(WIDTH-8,HEIGHT/2,eyes);      
      break;
    case LookUpLeft :
      moveEyeToXY(8,10,eyes);      
      break;
    case LookUpRight :
      moveEyeToXY(WIDTH-8,10,eyes);      
      break;
    case LookDownLeft :
      moveEyeToXY(8,HEIGHT-10,eyes);      
      break;
    case LookDownRight :
      moveEyeToXY(WIDTH-8,HEIGHT-10,eyes);      
      break;
  }
}

void setupN5110LCDs()
{
  for(unsigned short lcd=0;lcd<BOTH;lcd++)
  {
    lcd5110[lcd].begin();
    lcd5110[lcd].setContrast(60);
    lcd5110[lcd].clearDisplay();
    lcd5110[lcd].setTextSize(1);
  }
  
  set_text(L5110,0,0,"chip-E Remix!",BLACK);
  lcd5110[L5110].drawLine(7,11,77,11,BLACK);
  set_text(L5110,0,11,"Starting up",BLACK);
  
  set_text(R5110,0,0,"chip-E Remix!",BLACK);
  lcd5110[R5110].drawLine(7,11,77,11,BLACK);
  set_text(R5110,0,12,"I Love you");
  set_text(R5110,0,21,"mighty",BLACK);
  set_text(R5110,0,29,"creator!",BLACK);

  delay(4000);
}

void initEyes()
{
  clearAll();
  // eyeLids closed
  for(unsigned short eye=0;eye<BOTH;eye++)
  {
    eyeState[eye]=eyeClosed;
    lcd5110[eye].fillRoundRect(0,0,WIDTH-1,HEIGHT-1,4,BLACK);
    lcd5110[eye].display();
  }
}


/*
void setup()
{
  Serial.begin(9600);
  setupN5110LCDs();
  initEyes();
}
*/
/*
void loop() {

//  lcd5110[L5110].drawRoundRect(2,2,WIDTH-2,HEIGHT-2,4,BLACK);
  lcd5110[L5110].fillRoundRect(2,2,WIDTH-2,HEIGHT-2,4,BLACK);
  lcd5110[R5110].drawRect(25,18,10,10,BLACK);
  lcd5110[R5110].fillRect(28,21,10,10,BLACK);
  lcd5110[L5110].drawRoundRect(47,18,10,10,2,BLACK);
  lcd5110[L5110].fillRoundRect(50,21,10,10,2,BLACK);
  lcd5110[R5110].drawTriangle(68,18,68,28,78,23,BLACK);
  lcd5110[R5110].fillTriangle(71,21,71,31,81,26,BLACK);

  // Ein kleines bisschen Scroll-Text-Magie
  int x=0;
  int i;
  for(i=0;i<(5.6*8);i++){
    set_text_L(x,40,"Left screen.",BLACK);
    delay(i==0?1000:100);
    if(i<(5.6*8)-1)set_text_L(x,40,"Left screen.",WHITE);
    if((i)<(2.74*8))x-=1;else x+=1;
  }
  // Ein kleines bisschen Scroll-Text-Magie
  x=0;
  for(i=0;i<(5.6*8);i++){
    set_text_R(x,40,"Right screen.",BLACK);
    delay(i==0?1000:100);
    if(i<(5.6*8)-1)set_text_R(x,40,"Right screen.",WHITE);
    if((i)<(2.74*8))x-=1;else x+=1;
  }
  clearAll();
}
*/
