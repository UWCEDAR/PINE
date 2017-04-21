// PinGuesser (C)2017 Mike Borowczak
// Version 0.1  
// UWYO CEDAR | Cybersecurity Education And Research
// http://www.uwyo.edu/CEDAR



#include <Adafruit_GFX.h>
#include <UTFTGLUE.h>
#include <Keypad.h>

UTFTGLUE myGLCD(0x9488, A2, A1, A3, A4, A0);

// Declare which fonts we will be using

//#if !defined(SmallFont)
//extern uint8_t SmallFont[];    //.kbv GLUE defines as GFXFont ref
//#endif

#if !defined(SevenSegmentFull)
extern uint8_t SevenSegmentFull[];    //.kbv GLUE defines as GFXFont ref
#endif

const byte rows = 4; //four rows
const byte cols = 4; //three columns
char keys[rows][cols] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

byte rowPins[rows] = {53, 51, 49, 47}; //connect to the row pinouts of the keypad
byte colPins[cols] = {45, 43, 41, 39}; //connect to the column pinouts of the keypad

Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, rows, cols );

const  int xstartOffset = 0;
const  int ystartOffset = 15;
const  int gridgap = 2;
const  int sqSize = 80;
int counter = 0;
int guessPin = 0;
int secretPin = 0;
int highInit = 2017;
int lowInit = 1950;
int high = 0;;
int low = 0;
int livesLeft = 0;
void setup()
{
  randomSeed(analogRead(5));   //.kbv Due does not like A0
  pinMode(A0, OUTPUT);       //.kbv mcufriend have RD on A0
  digitalWrite(A0, HIGH);
  high = highInit;
  low = lowInit;
  livesLeft = int(ceil(log(high - low)));
  myGLCD.InitLCD(PORTRAIT);
  myGLCD.setFont(SevenSegmentFull);

  secretPin = random(low, high);

  guessPin = 0;
  counter = 0;
  drawKeyPad();

  myGLCD.setBackColor(255, 255, 255);
  myGLCD.setColor(0, 0, 0);
  myGLCD.print("~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~", CENTER, ystartOffset + 4 * (sqSize + gridgap));
  myGLCD.print(" WELCOME TO PIN GUESSER ", CENTER, ystartOffset + 4 * (sqSize + gridgap) + 10);
  myGLCD.print(" WOULD YOU LIKE TO PLAY A GAME? ", CENTER, ystartOffset + 4 * (sqSize + gridgap) + 20);
  myGLCD.print("You have 5 guesses to find a pin between 1950 & 2017!", CENTER, ystartOffset + 4 * (sqSize + gridgap) + 30);
  myGLCD.print("~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~", CENTER, ystartOffset + 4 * (sqSize + gridgap) + 40);
  myGLCD.print(" A PRODUCT OF UWYO CEDAR ", CENTER, ystartOffset + 4 * (sqSize + gridgap) + 50);
  myGLCD.print(" CYBERSECURITY EDUCATION AND RESEARCH  ", CENTER, ystartOffset + 4 * (sqSize + gridgap) + 60);
  myGLCD.print("~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~*~", CENTER, ystartOffset + 4 * (sqSize + gridgap) + 70);
}


void loop()
{
  int maxX = myGLCD.getDisplayXSize();
  int maxY = myGLCD.getDisplayYSize();
  if (counter == 4) {
    guessPin = guessPin + 1;
    myGLCD.clrScr();
    if (secretPin == guessPin) {
      bigMsg("You Win!", 34, 139, 34);
      delay(1000);
      //Hard Reset!
      setup();
    }
    else if (secretPin < guessPin) {
      if (guessPin < high) {
        high = guessPin;
      }
      bigMsg("To High!", 255, 0, 0);
      livesLeft = livesLeft - 1;
    }
    else {
      if (guessPin > low) {
        low = guessPin;
      }
      bigMsg("To Low!", 255, 0, 0);
      livesLeft = livesLeft - 1;
    }

    if (livesLeft < 0) {
      bigMsg("You ran out of guesses. Can you guess it quicker?", 34, 139, 34);
      delay(1000);
    }

    guessPin = 0;
    counter = 0;
    drawKeyPad();

  }



  char customKey = keypad.getKey();

  if (customKey) {

    drawKey(char2idx(customKey), xstartOffset, ystartOffset, gridgap, sqSize, 34, 139, 34);
    delay(250);

    int keyValue = customKey - '0';
    if (keyValue >= 0 && keyValue <= 9) {
      guessPin = guessPin + (keyValue) * pow(10, 3 - counter);
      drawKey(char2idx(customKey), xstartOffset, ystartOffset, gridgap, sqSize, 0, 0, 255);

      drawPinKey(customKey, counter, xstartOffset, ystartOffset + 4 * (sqSize + gridgap), gridgap, sqSize, 255, 0, 255);
      counter = counter + 1;
    }
    else {
      switch (customKey) {
        case 'A':
        case 'B':
        case 'C':
        case 'D': {
            String mhelp = "Hint -  ";
            String temp = "";
            int randSelect = int(random(1, 3));
            switch (randSelect) {
              case (1):
                temp = "What about halfway between " + String(low) + " and " + String(high) + "? ";
                mhelp.concat(temp);
                break;
              case (2):
                {
                  char array[120];
                  for (int idx = 0; idx < 80; idx++) {
                    array[idx] = ' ';
                  }
                  myGLCD.setBackColor(255, 223, 0);
                  myGLCD.setColor(255, 0, 0);
                  myGLCD.print(array, CENTER, maxY - 45);
                  temp = "So, halfway? What about " + String(int(floor((high - low) / 2)) + low) + "?  ";
                  mhelp.concat(temp);
                }
                break;
            }

            char mh[240] = "";
            mhelp.toCharArray(mh, mhelp.length());
            myGLCD.setBackColor(255, 223, 0);
            myGLCD.setColor(255, 0, 0);
            myGLCD.print(mh, CENTER, maxY - 45);
          }
          break;
        default:
          guessPin = 0;
          counter = 0;
          drawKeyPad();
      }



    }

  }
}

void setPin() {


}

void drawKeyPad() {
  int maxX = myGLCD.getDisplayXSize();
  int maxY = myGLCD.getDisplayYSize();

  myGLCD.clrScr();
  myGLCD.setColor(255, 0, 0);
  myGLCD.fillRect(0, 0, maxX, 13);

  myGLCD.setColor(64, 64, 64);
  myGLCD.fillRect(0, maxY - 20, maxX, maxY);

  myGLCD.setColor(255, 223, 0);
  myGLCD.fillRect(0, maxY - 50, maxX, maxY - 20);

  myGLCD.setColor(255, 255, 255);
  myGLCD.setBackColor(255, 0, 0);
  myGLCD.print("*  UWRAMPED | CEDAR | MBOROWCZAK *", CENTER, 1);

  myGLCD.setBackColor(255, 223, 0);
  myGLCD.setColor(255, 0, 0);

  String lowhig = "Number between " + String(low) + " and " + String(high) + " !";
  char lh[240] = "";
  lowhig.toCharArray(lh, lowhig.length());

  String LivesLeft = "You have " + String(livesLeft) + " guesses reamining!";

  if (livesLeft < 1) {
    int tnum = (-1 * livesLeft) + 1;
    LivesLeft = "You have used " + String(tnum) + " extra guess(es)!";
  }
  char ll[240] = "";
  LivesLeft.toCharArray(ll, LivesLeft.length());

  myGLCD.print(lh, CENTER, maxY - 45);
  myGLCD.print(ll, CENTER, maxY - 30);
  myGLCD.setBackColor(64, 64, 64);
  myGLCD.setColor(255, 255, 0);
  myGLCD.print("uwpd.org/RAMPED/ | uwyo.edu/CEDAR", CENTER, maxY - 16);



  myGLCD.setColor(0, 0, 0);
  myGLCD.fillRect(1, 15, 478, 304);


  for (int i = 1; i < 17; i++)
  {

    int R = 0;
    int G = 0;
    int B = 255;

    if ((i % 4 == 0 || floor((i - 1) / 4) == 3) ) {
      R = 255;
      B = 0;

    }
    if (i == 14) {
      R = 0;
      B = 255;
    }

    drawKey(i, xstartOffset, ystartOffset, gridgap, sqSize, R, G, B);


  }

}


void drawPinKey(char key, int pidx, int xstartOffset, int ystartOffset, int gridgap, int sqSize, int R, int G, int B) {

  char val[2] = "";
  myGLCD.setBackColor(R, G, B);
  myGLCD.setColor(R, G, B);

  val[0] = key;

  int xStart = xstartOffset + (pidx) * sqSize + gridgap;
  int yStart = ystartOffset;
  myGLCD.fillRect(xStart, yStart, xStart + sqSize, yStart + sqSize);
  myGLCD.setFont(SevenSegNumFont);
  myGLCD.setColor(0, 0, 0);
  int printXloc = (xStart + sqSize / 2);
  int printYloc = (yStart + sqSize / 2);
  myGLCD.print(val, printXloc, printYloc);


}

void drawKey(int i, int xstartOffset, int ystartOffset, int gridgap, int sqSize, int R, int G, int B) {
  int valC = (i - 1) % 4;
  int valR = floor((i - 1) / 4);
  char val[2] = "";
  val[0] = char(keys[valR][valC]);

  //KeyVal.concat(val);
  myGLCD.setBackColor(R, G, B);
  myGLCD.setColor(R, G, B);



  int xStart = xstartOffset + ((i - 1) % 4) * sqSize + gridgap;
  int yStart = ystartOffset + floor((i - 1) / 4) * sqSize + gridgap;
  myGLCD.fillRect(xStart, yStart, xStart + sqSize, yStart + sqSize);
  myGLCD.setFont(SevenSegNumFont);
  myGLCD.setColor(0, 0, 0);
  int printXloc = (xStart + sqSize / 2);
  int printYloc = (yStart + sqSize / 2);
  myGLCD.print(val, printXloc, printYloc);


}

int char2idx(char lookup) {

  for (int ridx = 0; ridx < 4; ridx++) {
    for (int cidx = 0; cidx < 4; cidx++) {
      if (keys[ridx][cidx] == lookup) {
        return (ridx * 4) + cidx + 1;
      }
    }
  }
  return -1;
}

void bigMsg(String msg, int R, int G, int B) {




  int maxX = myGLCD.getDisplayXSize();
  int maxY = myGLCD.getDisplayYSize();
  int xoffset = 20;
  int yoffset = 50;
  myGLCD.clrScr();
  char myMsg[240];
  msg.toCharArray(myMsg, msg.length());

  myGLCD.fillScr(0, 0, 255);
  myGLCD.setColor(R, G, B);

  myGLCD.fillRoundRect(0 + xoffset, yoffset, (maxX - xoffset), (maxY - yoffset));

  myGLCD.setColor(255, 255, 255);
  myGLCD.setBackColor(R, G, B);
  myGLCD.print(myMsg, CENTER, ((maxY / 2) - 64));

  int tguess = int(ceil(log(highInit - lowInit))) - livesLeft + 1;

  String totalGuess = "You've guessed " + String(tguess) + " time(s)! ";
  char tgMsg[240];
  totalGuess.toCharArray(tgMsg, totalGuess.length());


  myGLCD.print(tgMsg, CENTER, (maxY / 2) - 45);

  myGLCD.print(myMsg, CENTER, ((maxY / 2) - 64));
  if (guessPin == secretPin) {
    myGLCD.print("Play again", CENTER, (maxY / 2) - 32);
    myGLCD.print("in 60 Seconds!", CENTER, (maxY / 2) - 16);
    delay(60000);
  } else {
    myGLCD.print("Guess Again in a", CENTER, (maxY / 2) - 32);
    myGLCD.print("few seconds...", CENTER, (maxY / 2) - 16);
    delay(2500);
  }
}

