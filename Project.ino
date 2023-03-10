#include <LedControl.h>
#include <LiquidCrystal.h>
#include <EEPROM.h>

const byte dinPin = 12;
const byte clockPin = 11;
const byte loadPin = 10;
const byte pinX = A0;
const byte pinY = A1;
const byte RS = 9;
const byte enable = 8;
const byte d4 = 7;
const byte d5 = 6;
const byte d6 = 5;
const byte d7 = 4;
const byte pinSw = 2;
const byte pinLED = 3;

bool gameEnd = false;

const byte driveNumbers = 1;

LiquidCrystal lcd(RS, enable, d4, d5, d6, d7);
LedControl lc = LedControl(dinPin, clockPin, loadPin, driveNumbers);
// DIN, CLK, LOAD, No. DRIVER

//memory management
//0 matrixlight
//1 lcdlight
//2 sound(on or off)
//3 level
//10-19 name of the current player
//100-149 names of top 5 players
//200-224 scores of top 5 players
//195 numberofwinners

byte numberOfWinners;
const byte numberOfWinnersAddress = 195;

const byte matrixLightIndex = 0;
const byte LCDLightIndex = 1;
const byte soundIndex = 2;
const byte levelIndex = 3;
const byte currentNameIndex = 10;
const byte nameStartIndex = 100;
const byte scoreStartIndex = 200;
const byte soundOn = 1;
const byte soundOff = 0;

byte matrixLightMax = 4;
byte LCDLightMax = 4;
byte matrixLightDefaultVal = 1;
byte LCDLightDefaultVal = 1;
byte matrixLightMulti = 2;
byte LCDLightMulti = 40;
byte matrixLight = 0;
byte LCDLight = 0;
byte sound = 0;
byte gameLevel = 0;
byte gameLevelMax = 4;
byte gameLevelDefaultVal = 1;
String name = "          ";

const byte CustomCharSize = 8;
byte arrowUp[CustomCharSize] = {
	0b00100,
	0b01110,
	0b11111,
	0b01110,
	0b01110,
	0b01110,
	0b01110,
	0b01110
};

byte arrowDown[CustomCharSize] = {
	0b01110,
	0b01110,
	0b01110,
	0b01110,
	0b01110,
	0b11111,
	0b01110,
	0b00100
};

byte arrowLeft[CustomCharSize] = {
	0b00010,
	0b00100,
	0b01000,
	0b10000,
	0b10000,
	0b01000,
	0b00100,
	0b00010
};

byte arrowRight[CustomCharSize] = {
	0b01000,
	0b00100,
	0b00010,
	0b00001,
	0b00001,
	0b00010,
	0b00100,
	0b01000
};

const byte menuState = 0;
const byte playState = 1;
const byte highscoresState = 2;
const byte aboutState = 3;
const byte settingsState = 4;
const byte endState = 10;

const int minThreshold = 256;
const int maxThreshold = 768;
const byte moveInterval = 100;
byte swState = HIGH;
byte lastSwState = HIGH;
bool joyMoved = false;
int xValue = 0;
int yValue = 0;

int score = 0;

const byte matrixSize = 8;

bool buttonPress = false;

const byte nameSize = 10;

//functions which check if the joystick was moved past the threshold and one which checks if it was moved back
void joyMoveBack() {
  if (yValue > minThreshold && yValue < maxThreshold && xValue > minThreshold && xValue < maxThreshold) {
    joyMoved = false;
  }
}

bool moveDown() {
  if (xValue < minThreshold && joyMoved == false) {
    joyMoved = true;
    return true;
  }
  return false;
}

bool moveUp() {
  if (xValue > maxThreshold && joyMoved == false) {
    joyMoved = true;
    return true;
  }
  return false;
}

bool moveLeft() {
  if (yValue < minThreshold && joyMoved == false) {
    joyMoved = true;
    return true;
  }
  return false;
}

bool moveRight() {
  if (yValue > maxThreshold && joyMoved == false) {
    joyMoved = true;
    return true;
  }
  return false;
}


const byte arrowUpIndex = 0;
const byte arrowDownIndex = 1;
const byte arrowLeftIndex = 2;
const byte arrowRightIndex = 3;

byte state = 0;

const byte LCDUpLeft = 1;
const byte LCDUpRight = 2;
const byte LCDDownLeft = 3;
const byte LCDDownRight = 4;

const int menuDelay = 1000;

//the first LCD screen
void startMenu() {
  lcd.clear();
  lcd.setCursor(1, 0);
  lcd.print("Arduino Tetris");
  delay(menuDelay);
  lcd.setCursor(1, 1);
  lcd.print("Left to start");
  while (true) {
    xValue = analogRead(pinX);
    yValue = analogRead(pinY);
    joyMoveBack();
    if (!joyMoved) {
      if (moveLeft()) {
        return;
      } 
    }
  }
}

void checkPress() {
  if (swState != lastSwState && swState == LOW) {   
    buttonPress = true;
  }
  lastSwState = swState;
}

byte matrixTransitionDelaySettings = 10;

void turnMatrixOn() {
  for (byte row = 0; row < matrixSize; row++) {
    for (byte col = 0; col < matrixSize; col++) {
      lc.setLed(0, row, col, true); // turns on LED at col, row
      delay(matrixTransitionDelaySettings);
    }
  }
}

void turnMatrixOff() {
  for (byte row = 0; row < matrixSize; row++) {
    for (byte col = 0; col < matrixSize; col++) {
      lc.setLed(0, row, col, false); // turns off LED at col, row
      delay(matrixTransitionDelaySettings);
    }
  }
}

byte mainScreenSelection = 1;
//1play  2highscores
//3about 4settings
void mainScreen() {
  lcd.clear();
  lcd.setCursor(1, 0);
  lcd.print("Play");
  lcd.setCursor(7, 0);
  lcd.print("Highscores");
  lcd.setCursor(1, 1);
  lcd.print("About");
  lcd.setCursor(7, 1);
  lcd.print("Settings");

  switch (mainScreenSelection) {
    case LCDUpLeft: lcd.setCursor(0, 0); lcd.write(byte(arrowRightIndex)); break;
    case LCDUpRight: lcd.setCursor(6, 0); lcd.write(byte(arrowRightIndex)); break;
    case LCDDownLeft: lcd.setCursor(0, 1); lcd.write(byte(arrowRightIndex)); break;
    case LCDDownRight: lcd.setCursor(6, 1); lcd.write(byte(arrowRightIndex)); break;
  }

  while (true) {
    xValue = analogRead(pinX);
    yValue = analogRead(pinY);
    swState = digitalRead(pinSw);

    checkPress();
    joyMoveBack();

    if (!joyMoved) {
      if (buttonPress) {
        state = mainScreenSelection;
        buttonPress = false;
        break;        
      }
      if (moveUp()) {
        if (mainScreenSelection == LCDDownLeft) {
          mainScreenSelection = LCDUpLeft;
          break;          
        }
        if (mainScreenSelection == LCDDownRight) {
          mainScreenSelection = LCDUpRight;
          break;          
        }
      }
      if (moveDown()) {
        if (mainScreenSelection == LCDUpLeft) {
          mainScreenSelection = LCDDownLeft;
          break;          
        }
        if (mainScreenSelection == LCDUpRight) {
          mainScreenSelection = LCDDownRight;
          break;          
        }
      }
      if (moveLeft()) {
        if (mainScreenSelection == LCDUpRight) {
          mainScreenSelection = LCDUpLeft;
          break;          
        }
        if (mainScreenSelection == LCDDownRight) {
          mainScreenSelection = LCDDownLeft;
          break;          
        }
      }
      if (moveRight()) {
        if (mainScreenSelection == LCDUpLeft) {
          mainScreenSelection = LCDUpRight;
          break;          
        }
        if (mainScreenSelection == LCDDownLeft) {
          mainScreenSelection = LCDDownRight;
          break;          
        }
      }
    }
  }
}

byte nameIndex = 0;

const byte blinkPeriod = 200;
unsigned long blinkMillis;
bool isBlinked = false;
bool pageExit = false;

const byte nameIndexMax = 9;

//blinks the selected char during name selection
void blinkChar() {
  lcd.setCursor(nameIndex, 1);
  lcd.print(" ");
}

void unblinkChar() {
  lcd.setCursor(nameIndex, 1);
  lcd.print(name[nameIndex]);  
}

void settingsName() {
  pageExit = false;
  while (!pageExit) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Name:   ");
    lcd.write(byte(arrowUpIndex));
    lcd.print("Back");
    lcd.setCursor(0 , 1);
    lcd.print(name);
    blinkMillis = millis();
    while (true) {
      xValue = analogRead(pinX);
      yValue = analogRead(pinY);
      swState = digitalRead(pinSw);

      checkPress();
      joyMoveBack();
    
      if (!joyMoved) {
        if (buttonPress) {
          char currentLetter = name[nameIndex];
          if (currentLetter == 'Z') { currentLetter = ' '; }
          else if (currentLetter == ' ') { currentLetter = 'A'; }
          else { currentLetter++; }
          name[nameIndex] = currentLetter;
          buttonPress = false;
        }
        if (millis() - blinkMillis > blinkPeriod && !isBlinked) {
          blinkMillis = millis();
          isBlinked = true;
          blinkChar();
        }
        else if (millis() - blinkMillis > blinkPeriod && isBlinked) {
          blinkMillis = millis();
          isBlinked = false;
          unblinkChar();
        }
        if (moveRight()) {
          unblinkChar();
          if (nameIndex < nameIndexMax) { nameIndex++; }
          break;          
        }
        if (moveLeft()) {
          unblinkChar();
          if (nameIndex > 0) { nameIndex--; }
          break;   
        }
        if (moveUp()) {
          pageExit = true;
          break;
        }
      }
    }
  }
  for (byte i = 0; i < nameSize; i++) {
    EEPROM.update(currentNameIndex + i, name[i]);
  }
}

void settingsLevel() {
  pageExit = false;
  while (!pageExit) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Level:  ");
    lcd.write(byte(arrowUpIndex));
    lcd.print("Back");
    lcd.setCursor(0 , 1);
    lcd.print(gameLevel);
    while (true) {
      xValue = analogRead(pinX);
      yValue = analogRead(pinY);
      swState = digitalRead(pinSw);
      
      checkPress();
      joyMoveBack();
    
      if (!joyMoved) {  
          if (moveUp()) {
            pageExit = true;
            break;
          }
      }
      if (buttonPress) {
        gameLevel++;
        gameLevel %= gameLevelMax + 1;
        if (!gameLevel) { gameLevel++; }
        EEPROM.update(levelIndex, gameLevel);
        buttonPress = false;
        break;
      }      
    }
  }
}

bool matrixIsOn = false;
byte settingsLightSelection = 1;

const byte matrixLightSelection = 1;
const byte LCDLightSelection = 2;
const byte exitSelection = 3;

void settingsLight() {
  pageExit = false;
  while (!pageExit) {
    lcd.clear();
    lcd.setCursor(1, 0);
    lcd.print("Matrix light:");
    lcd.print(matrixLight);
    lcd.setCursor(1 , 1);
    lcd.print("LCD light:");
    lcd.print(LCDLight);
    lcd.setCursor(13, 1);
    lcd.print("Ext");

    if (!matrixIsOn && settingsLightSelection == matrixLightSelection) { turnMatrixOn(); matrixIsOn = true; }
    else if (matrixIsOn && settingsLightSelection != matrixLightSelection) { turnMatrixOff(); matrixIsOn = false; } 

    switch (settingsLightSelection) {
      case matrixLightSelection: lcd.setCursor(0, 0); lcd.write(byte(arrowRightIndex)); break;
      case LCDLightSelection: lcd.setCursor(0, 1); lcd.write(byte(arrowRightIndex)); break;
      case exitSelection: lcd.setCursor(12, 1); lcd.write(byte(arrowRightIndex)); break;
    }
    while (true) {
      xValue = analogRead(pinX);
      yValue = analogRead(pinY);
      swState = digitalRead(pinSw);
      
      checkPress();
      joyMoveBack();
    
      if (!joyMoved) {  
        if (moveUp() && settingsLightSelection == LCDLightSelection) {
          settingsLightSelection = matrixLightSelection;
          break;
        }
        if (moveDown() && settingsLightSelection == matrixLightSelection) {
          settingsLightSelection = LCDLightSelection;
          break;
        }
        if (moveRight() && settingsLightSelection == LCDLightSelection) {
          settingsLightSelection = exitSelection;
          break;
        }
        if (moveLeft() && settingsLightSelection == exitSelection) {
          settingsLightSelection = LCDLightSelection;
          break;
        }
      }
      if (buttonPress) {
        if (settingsLightSelection == matrixLightSelection) {
          matrixLight++;
          matrixLight %= matrixLightMax + 1;
          if (!matrixLight) { matrixLight++; }
          lc.setIntensity(0, matrixLight*matrixLightMulti);
          EEPROM.update(matrixLightIndex, matrixLight);
        }
        else if (settingsLightSelection == LCDLightSelection) {
          LCDLight++;
          LCDLight %= LCDLightMax + 1;
          if (!LCDLight) { LCDLight++; }
          analogWrite(pinLED, LCDLight*LCDLightMulti);
          EEPROM.update(LCDLightIndex, LCDLight);
        }
        else if (settingsLightSelection == exitSelection) {
          pageExit = true;
        }
        buttonPress = false;
        break;
      }      
    }
  }
}
const byte byteMaxVal = 255;
const byte scoreIndexMulti = 10;
const byte scoreMemorySize = 5;
const byte nameMemorySize = 10;
const int endGameDelay = 3000;
const byte maxWinners = 5;
const int highscoreDelay = 3000;

byte settingsSelection = 1;

const byte settingsMenuModeOne = 1;
const byte settingsMenuModeTwo = 2;

byte settingsMenu = settingsMenuModeOne;
void settings() {
  if (settingsMenu == settingsMenuModeOne) {
    lcd.clear();
    lcd.setCursor(1, 0);
    lcd.print("Name");
    lcd.setCursor(7, 0);
    lcd.print("Next Page");
    lcd.setCursor(1, 1);
    lcd.print("Level");
    lcd.setCursor(7, 1);
    lcd.print("Exit");
  }
  else if (settingsMenu = settingsMenuModeTwo) {
    lcd.clear();
    lcd.setCursor(1, 0);
    lcd.print("Light");
    lcd.setCursor(7, 0);
    lcd.print("Back");
    lcd.setCursor(1, 1);
    lcd.print("Sound");
    lcd.setCursor(7, 1);
    lcd.print("Exit");
  }

  switch (settingsSelection) {
    case LCDUpLeft: lcd.setCursor(0, 0); lcd.write(byte(arrowRightIndex)); break;
    case LCDUpRight: lcd.setCursor(6, 0); lcd.write(byte(arrowRightIndex)); break;
    case LCDDownLeft: lcd.setCursor(0, 1); lcd.write(byte(arrowRightIndex)); break;
    case LCDDownRight: lcd.setCursor(6, 1); lcd.write(byte(arrowRightIndex)); break;
  }

  while (true) {
    xValue = analogRead(pinX);
    yValue = analogRead(pinY);
    swState = digitalRead(pinSw);

    checkPress();
    joyMoveBack();

    if (!joyMoved) {
      if (buttonPress) {
        buttonPress = false;
        if (settingsMenu == settingsMenuModeOne) {
          switch (settingsSelection) {
            case LCDUpLeft: settingsName(); buttonPress = false; break;
            case LCDUpRight: settingsMenu = settingsMenuModeTwo; break;
            case LCDDownLeft: settingsLevel(); buttonPress = false; break;
            case LCDDownRight: state = menuState; break;
          }          
        }
        else if (settingsMenu == settingsMenuModeTwo) {
          switch (settingsSelection) {
            case LCDUpLeft: settingsLight(); buttonPress = false; break;
            case LCDUpRight: settingsMenu = settingsMenuModeOne; break;
            // case LCDDownLeft: settingsSound(); buttonPress = false; break;
            case LCDDownRight: state = menuState; break;
          }
        }
        break;        
      }
      if (moveUp()) {
        if (settingsSelection == LCDDownLeft) {
          settingsSelection = LCDUpLeft;
          break;          
        }
        if (settingsSelection == LCDDownRight) {
          settingsSelection = LCDUpRight;
          break;          
        }
      }
      if (moveDown()) {
        if (settingsSelection == LCDUpLeft) {
          settingsSelection = LCDDownLeft;
          break;          
        }
        if (settingsSelection == LCDUpRight) {
          settingsSelection = LCDDownRight;
          break;          
        }
      }
      if (moveLeft()) {
        if (settingsSelection == LCDUpRight) {
          settingsSelection = LCDUpLeft;
          break;          
        }
        if (settingsSelection == LCDDownRight) {
          settingsSelection = LCDDownLeft;
          break;          
        }
      }
      if (moveRight()) {
        if (settingsSelection == LCDUpLeft) {
          settingsSelection = LCDUpRight;
          break;          
        }
        if (settingsSelection == LCDUpLeft) {
          settingsSelection = LCDDownRight;
          break;          
        }
      }
    }
  }
}


void generateHighscoresScreen() {
  lcd.clear();
  lcd.setCursor(3, 0);
  lcd.print("Highscores");
  lcd.write(byte(arrowRightIndex));     
  lcd.setCursor(0, 1);       
  lcd.print("Back");
  lcd.write(byte(arrowUpIndex));
}

//unfinished
void highscores() {
  // generateHighscoresScreen();
  // byte currentWinner = 0;
  // while (true) {
  //   xValue = analogRead(pinX);
  //   yValue = analogRead(pinY);
  //   joyMoveBack();
  //   if (!joyMoved) {
  //     if (moveUp() && currentWinner == 0) {
  //       state = menuState;
  //       return;
  //     }
  //     if (moveRight()) {
  //       if (!numberOfWinners) {        
  //         lcd.setCursor(0, 0);
  //         lcd.print("No Winners! ");  
  //         lcd.setCursor(0, 1);
  //         lcd.write(byte(arrowLeftIndex));
  //         lcd.print("Back");
  //         while (true) {
  //             xValue = analogRead(pinX);
  //             yValue = analogRead(pinY);
  //             joyMoveBack();
  //             if (moveLeft()) {
  //               generateHighscoresScreen();
  //               break;
  //             }
  //         }          
  //       }
  //       if (currentWinner != 5) {
  //         currentWinner++;
  //         lcd.setCursor(0, 0);
  //         String localWinnerName = readName(numberOfWinnersAddress+(currentWinner-1)*nameSize);
  //         lcd.print(localWinnerName);
  //         lcd.setCursor(0, 1);
  //         int localWinnerScore = EEPROM.read(scoreStartIndex + scoreMemorySize*(currentWinner-1));
  //         if (localWinnerScore = byteMaxVal) {
  //           byte iter = 0;
  //           while(true) {
  //             byte tempScore = EEPROM.read(scoreStartIndex + scoreMemorySize*(currentWinner-1) + ++iter);
  //             localWinnerScore += tempScore;
  //             if (tempScore < byteMaxVal) {
  //               break;
  //             }
  //           }
  //         }
  //         lcd.print(localWinnerScore);
  //         lcd.setCursor(0, 6);
  //         lcd.write(byte(arrowLeftIndex))
  //         lcd.wr
  //       }  
  //     }
  //     // if (moveLeft()) {
         
  //     // }
  //   }
  // }
  state = menuState;
}

const byte aboutModeGame = 0;
const byte aboutModeCreator = 1;

void about() {
  byte currentAbout = aboutModeCreator;
  lcd.clear();
  while (true) {
    xValue = analogRead(pinX);
    yValue = analogRead(pinY);
    joyMoveBack();
    if (!joyMoved) {
      if (moveUp() && currentAbout == aboutModeCreator) {
        state = menuState;
        return;
      }
      if (moveLeft()) {
        lcd.clear();
        currentAbout = aboutModeCreator;
      }
      if (moveRight()) {
        lcd.clear();
        currentAbout = aboutModeGame;
      }     
      if (currentAbout == aboutModeGame) {
        lcd.setCursor(0, 0);
        lcd.print("Classic Tetris!");
        lcd.write(byte(2));
        lcd.setCursor(0, 1);
        lcd.print("Fill rows!");
      }
      if (currentAbout == 1) {
        lcd.setCursor(0, 0);
        lcd.print("Neacsu Sasa");
        lcd.write(byte(arrowUpIndex));
        lcd.print("Back")   ;   
        lcd.setCursor(0, 1);
        lcd.print("Github: NSA-25");
        lcd.setCursor(15, 1);
        lcd.write(byte(arrowRightIndex));
      }        
    }
  }
}

//used for checking which leds to light up
byte gameMatrix[matrixSize][matrixSize] = {
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0},
  {0, 0, 0, 0, 0, 0, 0, 0}
};

void displayWinnerScreen() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("WELL DONE!");
  delay(highscoreDelay);
  lcd.setCursor(0, 1);
  lcd.print("HIGHSCORE!");
  delay(highscoreDelay);
}

void endGame(){ 
  if (numberOfWinners < maxWinners) {
    byte winnerPlace = 1; 
    for (byte i = 0; i < numberOfWinners; i++) {
      int olderScore = EEPROM.read(scoreStartIndex+i*scoreIndexMulti);
      byte iter = 1;
      if (olderScore = byteMaxVal) {
        for (byte j = 0; j < scoreMemorySize; j++) {
          byte scoreContinuation = EEPROM.read(scoreStartIndex+i*scoreIndexMulti+iter);
          iter++;
          olderScore += scoreContinuation;
          if (scoreContinuation != byteMaxVal) { break; }
        }        
      }
      if (score <= olderScore) {
        winnerPlace += 1;
      }
    }
    for (byte i = 0; i < nameMemorySize; i++) {
      EEPROM.update(nameStartIndex + i + (winnerPlace-1)*nameMemorySize, name[i]);
    }
    byte iter = 0;
    if (score > byteMaxVal){
      while (score > byteMaxVal) {
        EEPROM.update(scoreStartIndex + (winnerPlace-1)*scoreMemorySize + iter, score);
        iter++;
        score -= byteMaxVal;        
      }
    }
    EEPROM.update(scoreStartIndex + (winnerPlace-1)*scoreMemorySize + iter, score);
    displayWinnerScreen();
    numberOfWinners++;
    EEPROM.update(numberOfWinnersAddress, numberOfWinners);    
  }
  else {
    byte winnerPlace = maxWinners;
    for (byte i = maxWinners; i < -1; i++) {
      int olderScore = EEPROM.read(scoreStartIndex+i*scoreIndexMulti);
      byte iter = 1;
      if (olderScore = byteMaxVal) {
        for (byte j = 0; j < scoreMemorySize; j++) {
          byte scoreContinuation = EEPROM.read(scoreStartIndex+i*scoreIndexMulti + iter);
          iter++;
          olderScore += scoreContinuation;
          if (scoreContinuation != byteMaxVal) { break; }
        }        
      }
      if (score <= olderScore) {
        break;
      }
      else {
        winnerPlace--;
      }
    }
    if (winnerPlace < maxWinners) {
      for (byte i = 0; i < nameMemorySize; i++) {
        EEPROM.update(nameStartIndex + i + winnerPlace*nameMemorySize, name[i]);
      }
      displayWinnerScreen();
      byte iter = 0;
      if (score > byteMaxVal) { 
        while (score > byteMaxVal) {
          EEPROM.update(scoreStartIndex + winnerPlace*scoreMemorySize + iter, score);
          iter++;
          score -= byteMaxVal;        
        }
      }
        EEPROM.update(scoreStartIndex + (winnerPlace-1)*scoreMemorySize + iter, score);
      }
  }
  lcd.clear();
  lcd.setCursor(1, 0);
  lcd.print("Good game!");
  lcd.setCursor(1, 1);
  lcd.print("Score: ");
  lcd.print(score, DEC);
  turnMatrixOn();
  delay(endGameDelay);
  turnMatrixOff();
  gameEnd = true;
  for (byte row = 0; row < matrixSize; row++) {
    for (byte col = 0; col < matrixSize; col++) {
      gameMatrix[row][col] = 0;

    }
  }
  score = 0;
  state = menuState;
  return;
}

//Pieces
//Names:
//O    T    Z    L    S    J   I
//
//00   0     0   0   0     0   0
//00   00   00   0   00    0   0
//     0    0    00   0   00   0
//                             0
//Rotations:
//0    4    2    4    2    4   2
//rotation 1:        rotation 2:        rotation 3:        rotation 4:         (except block I and O)
//row 1 left-right   column 1 down-up   row 2 right-left   column 2 up-down
//row 2 left-right   column 2 down-up   row 1 right-left   column 1 up-down
//011    10
//110    11            
//       01
//
//111    11    100   01
//001    10    111   01
//       10          11 

const byte numberOfPieces = 7;
const byte pieceSize = 4;
const byte pieceCoordSize = 2;
const byte blockCols = 3;
const byte blockRepSize = 5;

const byte idT = 0;
const byte idZ = 1;
const byte idL = 2;
const byte idS = 3;
const byte idJ = 4;
const byte idO = 5;
const byte idI = 6;

const byte rotations[numberOfPieces] = {4, 2, 4, 2, 4, 0, 2};
//rotationsT = 4
//rotationsZ = 2
//rotationsL = 4
//rotationsS = 2
//rotationsJ = 4
//rotationsO = 0
//rotationsI = 2

//Blocks are represented in four matrices to account for all possible rotations
const byte blocksRep1[blockRepSize][blockCols][blockCols] = {
  {
    {0, 1, 0},
    {1, 1, 1},
    {0, 0, 0}
  },
  {
    {1, 1, 0},
    {0, 1, 1},
    {0, 0, 0}
  },
  {
    {1, 1, 1},
    {1, 0, 0},
    {0, 0, 0}
  },
  {
    {0, 1, 1},
    {1, 1, 0},
    {0, 0, 0}
  },
  {
    {1, 1, 1},
    {0, 0, 1},
    {0, 0, 0}
  }
};

const byte blocksRep2[blockRepSize][blockCols][blockCols] = {
  {
    {0, 1, 0},
    {0, 1, 1},
    {0, 1, 0}
  },
  {
    {0, 0, 1},
    {0, 1, 1},
    {0, 1, 0}
  },
  {
    {0, 1, 1},
    {0, 0, 1},
    {0, 0, 1}
  },
  {
    {0, 1, 0},
    {0, 1, 1},
    {0, 0, 1}
  },
  {
    {0, 0, 1},
    {0, 0, 1},
    {0, 1, 1}
  }
};

const byte blocksRep3[blockRepSize][blockCols][blockCols] = {
  {
    {0, 0, 0},
    {1, 1, 1},
    {0, 1, 0}
  },
  {
    {1, 1, 0},
    {0, 1, 1},
    {0, 0, 0}
  },
  {
    {0, 0, 0},
    {0, 0, 1},
    {1, 1, 1}
  },
  {
    {0, 1, 1},
    {1, 1, 0},
    {0, 0, 0}
  },
  {
    {0, 0, 0},
    {1, 0, 0},
    {1, 1, 1}
  }
};

const byte blocksRep4[blockRepSize][blockCols][blockCols] = {
  {
    {0, 1, 0},
    {1, 1, 0},
    {0, 1, 0}
  },
  {
    {0, 0, 1},
    {0, 1, 1},
    {0, 1, 0}
  },
  {
    {1, 0, 0},
    {1, 0, 0},
    {1, 1, 0}
  },
  {
    {0, 1, 0},
    {0, 1, 1},
    {0, 0, 1}
  },
  {
    {1, 1, 0},
    {1, 0, 0},
    {1, 0, 0}
  }
};

const byte currentBlockRep[3][3] = {
  {0, 0, 0},
  {0, 0, 0},
  {0, 0, 0}
};

byte blockId = 1;
byte rotation = 1;
//row, column
const byte pieceStartRow = 3;
const byte pieceStartCol = 7;
const byte defaultCenterRow = 4;
const byte defaultCenterCol = 6;
byte currentBlockCenter[pieceCoordSize] = {defaultCenterRow, defaultCenterCol};

byte piecePositions[pieceSize][pieceCoordSize] = {
  {0, 0},
  {0, 0},
  {0, 0},
  {0, 0}  
};

byte previousPiecePositions[pieceSize][pieceCoordSize] = {
  {0, 0},
  {0, 0},
  {0, 0},
  {0, 0}  
};

//helper function to add the created piece positions in the possitions and game matrixes
void populatePositions(byte r1, byte c1, byte r2, byte c2, byte r3, byte c3, byte r4, byte c4) {
    piecePositions[0][0] = r1;
    piecePositions[0][1] = c1;
    piecePositions[1][0] = r2;
    piecePositions[1][1] = c2;
    piecePositions[2][0] = r3;
    piecePositions[2][1] = c3;
    piecePositions[3][0] = r4;
    piecePositions[3][1] = c4;

    previousPiecePositions[0][0] = r1;
    previousPiecePositions[0][1] = c1;
    previousPiecePositions[1][0] = r2;
    previousPiecePositions[1][1] = c2;
    previousPiecePositions[2][0] = r3;
    previousPiecePositions[2][1] = c3;
    previousPiecePositions[3][0] = r4;
    previousPiecePositions[3][1] = c4;

    gameMatrix[r1][c1] = 1;
    gameMatrix[r2][c2] = 1;
    gameMatrix[r3][c3] = 1;
    gameMatrix[r4][c4] = 1;
}

const byte directionSize = 8;
const byte blockSize = 3;
const byte rotationsSize = 4;

//row, column
//up-left, up, up-right, left, right, down-left, down, down-right
//avaiable for this matrix specific orientation (1088AS sign on the right)
const int directions[matrixSize][pieceCoordSize] = {
  {-1, 1}, {0, 1}, {1, 1}, {-1, 0}, {1, 0}, {-1, -1}, {0, -1}, {1, -1}
};

// the function used to rotate a piece. It checks all possible directions next to the center of the piece to see if the space needed for rotation is occupied.
// if it's occupied there will be a return statement and the rotation won't take place (unless the occupied space is the old position of the piece before the rotation)
// it unwrites the old position and adds the new in the positions and game matrix
void rotate() {
  byte positionIndex = 0;
  byte directionIndex = 0;
  byte newPos[pieceSize][pieceCoordSize];
  rotation %= rotationsSize;
  Serial.println(rotation);
  switch(rotation) {
    case 0:
        for (byte i = 0; i < blockSize; i++) {
          for (byte j = 0; j < blockSize; j++) {
            if (i == 1 && j == 1) {
              directionIndex--;
              if (blocksRep2[blockId][i][j]) {
                newPos[positionIndex][0] = currentBlockCenter[0];
                newPos[positionIndex][1] = currentBlockCenter[1];
                positionIndex++;
              }
            }
            else if (blocksRep2[blockId][i][j]) {
              if (gameMatrix[currentBlockCenter[0] + directions[directionIndex][0]][currentBlockCenter[1] + directions[directionIndex][1]]) {
                if (!isPiece(currentBlockCenter[0] + directions[directionIndex][0], currentBlockCenter[1] + directions[directionIndex][1])) { return; }
              }
              newPos[positionIndex][0] = currentBlockCenter[0] + directions[directionIndex][0];
              newPos[positionIndex][1] = currentBlockCenter[1] + directions[directionIndex][1];
              positionIndex++;
            }
            directionIndex++;
          }
        }
        rotation++;
      break;
    case 1:
      for (byte i = 0; i < blockSize; i++) {
        for (byte j = 0; j < blockSize; j++) {
          if (i == 1 && j == 1) {
            directionIndex--;
            if (blocksRep3[blockId][i][j]) {
              newPos[positionIndex][0] = currentBlockCenter[0];
              newPos[positionIndex][1] = currentBlockCenter[1];
              positionIndex++;
            }
          }
          else if (blocksRep3[blockId][i][j]) {
            if (gameMatrix[currentBlockCenter[0] + directions[directionIndex][0]][currentBlockCenter[1] + directions[directionIndex][1]]) {
              if (!isPiece(currentBlockCenter[0] + directions[directionIndex][0], currentBlockCenter[1] + directions[directionIndex][1])) { return; }
            }
            newPos[positionIndex][0] = currentBlockCenter[0] + directions[directionIndex][0];
            newPos[positionIndex][1] = currentBlockCenter[1] + directions[directionIndex][1];
            positionIndex++;
          }
          directionIndex++;
        }
      }
      rotation++;
      break;
    case 2:
      for (byte i = 0; i < blockSize; i++) {
        for (byte j = 0; j < blockSize; j++) {
          if (i == 1 && j == 1) {
            directionIndex--;
            if (blocksRep4[blockId][i][j]) {
              newPos[positionIndex][0] = currentBlockCenter[0];
              newPos[positionIndex][1] = currentBlockCenter[1];
              positionIndex++;
            }
          }
          else if (blocksRep4[blockId][i][j]) {
            if (gameMatrix[currentBlockCenter[0] + directions[directionIndex][0]][currentBlockCenter[1] + directions[directionIndex][1]]) {
              if (!isPiece(currentBlockCenter[0] + directions[directionIndex][0], currentBlockCenter[1] + directions[directionIndex][1])) { return; }
            }
            newPos[positionIndex][0] = currentBlockCenter[0]+directions[directionIndex][0];
            newPos[positionIndex][1] = currentBlockCenter[1]+directions[directionIndex][1];
            positionIndex++;
          }
          directionIndex++;
        }
      }
      rotation++;
      break;
    case 3:
      for (byte i = 0; i < blockSize; i++) {
        for (byte j = 0; j < blockSize; j++) {
          if (i == 1 && j == 1) {
            directionIndex--;
            if (blocksRep1[blockId][i][j]) {
              newPos[positionIndex][0] = currentBlockCenter[0];
              newPos[positionIndex][1] = currentBlockCenter[1];
              positionIndex++;
            }
          }
          else if (blocksRep1[blockId][i][j]) {
            if (gameMatrix[currentBlockCenter[0] + directions[directionIndex][0]][currentBlockCenter[1] + directions[directionIndex][1]]) {
              if (!isPiece(currentBlockCenter[0] + directions[directionIndex][0], currentBlockCenter[1] + directions[directionIndex][1])) { return; }
            }
            newPos[positionIndex][0] = currentBlockCenter[0]+directions[directionIndex][0];
            newPos[positionIndex][1] = currentBlockCenter[1]+directions[directionIndex][1];
            positionIndex++;
          }
          directionIndex++;
        }
      }
      rotation++;
      break;
  }
  movePieceHelper();
  for (byte i = 0; i < pieceSize; i++) { piecePositions[i][0] = newPos[i][0]; piecePositions[i][1] = newPos[i][1];}
  unwritePiece();
  writePiece();
}

const byte rotationsHalf = 2;

// generates a new block when called
// if it fails to spawn a block the game ends
// the block and its rotation are picked randomly
void generateBlock() {
  //There was an issue with random(), it always spawned 0 during the first call
  //Calling the function here seemed to fix it
  currentBlockCenter[0] = defaultCenterRow;
  currentBlockCenter[1] = defaultCenterCol;
  random(numberOfPieces);
  blockId = random(numberOfPieces);
  if (blockId != idO) { rotation = random(rotationsSize); }
  if (blockId == idO) {
    lc.setLed(0, pieceStartRow, pieceStartCol, true);
    lc.setLed(0, pieceStartRow + 1, pieceStartCol, true);
    lc.setLed(0, pieceStartRow, pieceStartCol - 1, true);
    lc.setLed(0, pieceStartRow + 1, pieceStartCol - 1, true);
    if (gameMatrix[pieceStartRow][pieceStartCol] || gameMatrix[pieceStartRow + 1][pieceStartCol] || gameMatrix[pieceStartRow][pieceStartCol - 1] || gameMatrix[pieceStartRow-1][pieceStartCol - 1]) {
      endGame();
      return;
    }
    populatePositions(pieceStartRow, pieceStartCol, pieceStartRow + 1, pieceStartCol, pieceStartRow, pieceStartCol - 1, pieceStartRow + 1, pieceStartCol - 1);
  }
  else if (blockId == idI) {
    if (rotation % rotationsHalf) {
      lc.setLed(0, pieceStartRow - 1, pieceStartCol, true);
      lc.setLed(0, pieceStartRow, pieceStartCol, true);
      lc.setLed(0, pieceStartRow + 1, pieceStartCol, true);
      lc.setLed(0, pieceStartRow + 2, pieceStartCol, true);
      if (gameMatrix[pieceStartRow - 1][pieceStartCol] || gameMatrix[pieceStartRow][pieceStartCol] || gameMatrix[pieceStartRow + 1][pieceStartCol] || gameMatrix[pieceStartRow + 2][pieceStartCol]) {
        endGame();
        return;
      }   
      populatePositions(pieceStartRow - 1, pieceStartCol, pieceStartRow, pieceStartCol, pieceStartRow + 1, pieceStartCol, pieceStartRow + 2, pieceStartCol);
    }
    else {
      lc.setLed(0, pieceStartRow, pieceStartCol, true);
      lc.setLed(0, pieceStartRow, pieceStartCol - 1, true);
      lc.setLed(0, pieceStartRow, pieceStartCol - 2, true);
      lc.setLed(0, pieceStartRow, pieceStartCol - 3, true);

      if (gameMatrix[pieceStartRow][pieceStartCol] || gameMatrix[pieceStartRow][pieceStartCol - 1] || gameMatrix[pieceStartRow][pieceStartCol - 2] || gameMatrix[pieceStartRow][pieceStartCol - 3]) {
        endGame();
        return;
      } 

      populatePositions(pieceStartRow, pieceStartCol, pieceStartRow, pieceStartCol - 1, pieceStartRow, pieceStartCol - 2, pieceStartRow, pieceStartCol - 3);
    } 
  }  
  else {
    byte posCoordinates[matrixSize];  
    byte posCoordinatesIndex = 0;    
    if (rotations[blockId] == rotationsHalf) { rotation %= rotationsHalf; }
    if (rotation == 0) {
      for (byte i = 0; i < blockCols; i++) {
        for (byte j = 0; j < blockCols; j++) {    
          if (blocksRep1[blockId][i][j]) {
            lc.setLed(0, pieceStartRow + j, pieceStartCol - i, true);
            posCoordinates[posCoordinatesIndex++] = pieceStartRow + j;
            posCoordinates[posCoordinatesIndex++] = pieceStartCol - i;
          }
        }
      }
    }
    else if (rotation == 1) {
      for (byte i = 0; i < blockCols; i++) {
        for (byte j = 0; j < blockCols; j++) {    
          if (blocksRep2[blockId][i][j]) {
            lc.setLed(0, pieceStartRow + j, pieceStartCol - i, true);
            posCoordinates[posCoordinatesIndex++] = pieceStartRow + j;
            posCoordinates[posCoordinatesIndex++] = pieceStartCol - i;
          }
        }
      }
    }
    else if (rotation == 2) {
      for (byte i = 0; i < blockCols; i++) {
        for (byte j = 0; j < blockCols; j++) {    
          if (blocksRep3[blockId][i][j]) {
            lc.setLed(0, pieceStartRow + j, pieceStartCol - i, true);
            posCoordinates[posCoordinatesIndex++] = pieceStartRow + j;
            posCoordinates[posCoordinatesIndex++] = pieceStartCol - i;
          }
        }
      }
    }
    else {
      for (byte i = 0; i < blockCols; i++) {
        for (byte j = 0; j < blockCols; j++) {    
          if (blocksRep4[blockId][i][j]) {
            lc.setLed(0, pieceStartRow+j, pieceStartCol - i, true);
            posCoordinates[posCoordinatesIndex++] = pieceStartRow + j;
            posCoordinates[posCoordinatesIndex++] = pieceStartCol - i;
          }
        }
      }
    }
    if (gameMatrix[posCoordinates[0]][posCoordinates[1]] || gameMatrix[posCoordinates[2]][posCoordinates[3]] || gameMatrix[posCoordinates[4]][posCoordinates[5]] || gameMatrix[posCoordinates[6]][posCoordinates[7]]) {
      endGame();
      return;
    } 
    populatePositions(posCoordinates[0], posCoordinates[1], posCoordinates[2], posCoordinates[3], posCoordinates[4], posCoordinates[5], posCoordinates[6], posCoordinates[7]);
  }
}

// depending on the level, several initial lines will be created
// these will have several random leds on or off (some are hardcoded to prevent an initial full or empty line)
void generateInitialBlocks() {
  byte blockLines = gameLevel - 1;
  for (byte col = 0; col < blockLines; col++) {
    for (byte row = 1; row < matrixSize - 1; row++) {
      byte randomLight = random(2);
      lc.setLed(0, row, col, randomLight);
      gameMatrix[row][col] = randomLight;      
    }
    lc.setLed(0, 0, col, true);    
    gameMatrix[0][col] = 1;
    lc.setLed(0, matrixSize - 1, col, false);    
    gameMatrix[matrixSize - 1][col] = false;
  }
}

void writePiece() {
  for (byte i = 0; i < pieceSize; i++) {
    lc.setLed(0, piecePositions[i][0], piecePositions[i][1], true);
    gameMatrix[piecePositions[i][0]][piecePositions[i][1]] = 1;
  }
}

void unwritePiece() {
  for (byte i = 0; i < pieceSize; i++) {
    if (piecePositions[i][0] != previousPiecePositions[i][0] || piecePositions[i][1] != previousPiecePositions[i][1]) {
      lc.setLed(0, previousPiecePositions[i][0], previousPiecePositions[i][1], false); 
      gameMatrix[previousPiecePositions[i][0]][previousPiecePositions[i][1]] = 0;
    }
  }
}

void movePieceHelper() {
  for (byte i = 0; i < pieceSize; i++) { previousPiecePositions[i][0] = piecePositions[i][0]; previousPiecePositions[i][1] = piecePositions[i][1]; }
}

//used to check if an occupied space is the piece itself or not
bool isPiece(byte row, byte col) {
  for (byte i = 0; i < pieceSize; i++) {
    if (piecePositions[i][0] == row && piecePositions[i][1] == col) { return true; }
  }
  return false;
}

// called when a line is full
// deletes the line and moves the following ones down by one
void updateMatrix(byte line) {
  for (byte i = line; i < matrixSize - 1; i++) {
    for (byte j = 0; j < matrixSize; j++) {
      gameMatrix[j][i] = gameMatrix[j][i + 1];
    }    
  }
  for (byte i = 0; i < matrixSize; i++) {
    gameMatrix[i][matrixSize - 1] = 0;
  }
  for (byte row = 0; row < matrixSize; row++) {
    for (byte col = 0; col < matrixSize; col++) {
      lc.setLed(0, row, col, gameMatrix[row][col]);
    }
  }
}

// called when a piece can't move further down
// if a line is full the score is incremented and it calls updateMatrix which deletes the line
// after that a new piece is generated
void pieceLanded() {
  bool fullLines[matrixSize] = {false, false, false, false, false, false, false, false};
  for (byte i = 0; i < matrixSize; i++) {
    fullLines[i] = true;
    for (byte j = 0; j < matrixSize; j++) {
      if (!gameMatrix[j][i]) {
        fullLines[i] = false;
        break;
      }
    }
  }
  for (int i = matrixSize - 1; i > -1; i--) {
    if (fullLines[i]) {
      score++;
      displayGameLCD();
      for (byte j = 0; j < matrixSize; j++) {
        gameMatrix[j][i] = 0;
      }
      updateMatrix(i);
    }
  }
  generateBlock();
}

void movePieceLeft() {
  for (byte i = 0; i < pieceSize; i++) {
    if (!piecePositions[i][0] || (gameMatrix[piecePositions[i][0] - 1][piecePositions[i][1]])) {
      if (!isPiece(piecePositions[i][0] - 1, piecePositions[i][1])) {
        return;
      }
    }
  }
  movePieceHelper();
  currentBlockCenter[0]--;
  for (byte i = 0; i < pieceSize; i++) { piecePositions[i][0]--; }
  unwritePiece();
  writePiece();
}

void movePieceRight() {
  for (byte i = 0; i < pieceSize; i++) {
    if (piecePositions[i][0] == matrixSize - 1 || (gameMatrix[piecePositions[i][0] + 1][piecePositions[i][1]])) {
      if (!isPiece(piecePositions[i][0] + 1, piecePositions[i][1])) {
        return;
      }
    }
  }
  movePieceHelper();
  currentBlockCenter[0]++;
  for (byte i = 0; i < pieceSize; i++) { piecePositions[i][0]++; }
  unwritePiece();
  writePiece();
}

void movePieceDown() {
  for (byte i = 0; i < pieceSize; i++) {
    if (!piecePositions[i][1] || gameMatrix[piecePositions[i][0]][piecePositions[i][1] - 1]) {
      if (!isPiece(piecePositions[i][0], piecePositions[i][1] - 1)) {
        pieceLanded();
        return;
      }
    }
  }
  movePieceHelper();
  currentBlockCenter[1]--;
  for (byte i = 0; i < pieceSize; i++) { piecePositions[i][1]--; }
  unwritePiece();
  writePiece();
}


void displayGameLCD() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(name);
  lcd.setCursor(11, 0);
  lcd.print("Lvl:");
  lcd.print(gameLevel);
  lcd.setCursor(0, 1);
  lcd.print("Score:");
  lcd.print(score);
}

void startGame() {
  displayGameLCD();
  if (gameLevel > gameLevelDefaultVal) {
    generateInitialBlocks();
  }
  generateBlock();  
  while (!gameEnd) {
    xValue = analogRead(pinX);
    yValue = analogRead(pinY);
    swState = digitalRead(pinSw);

    checkPress();
    joyMoveBack();

    if (!joyMoved) {
      if (moveLeft()) {
        movePieceLeft();
      }
      if (moveRight()) {
        movePieceRight();
      }
      if (moveDown()) {
        movePieceDown();
      }
      if (buttonPress) {
        buttonPress = false;
        if (blockId != idO && blockId != idI) {
          rotate();          
        }
      }
    }
  }
}

String readName(byte address) {
  String localName = "          ";
  for (byte i = 0; i < nameSize; i++) {
    localName[i] = EEPROM.read(address + i);
  }
  return localName;
}

const byte pinRandomGenerator = 13;

void setup() {
  numberOfWinners = EEPROM.read(numberOfWinnersAddress);
  Serial.begin(9600);
  Serial.println("New start");
  randomSeed(analogRead(pinRandomGenerator));
  matrixLight = EEPROM.read(matrixLightIndex);
  if (!matrixLight || matrixLight > matrixLightMax) {
    matrixLight = matrixLightDefaultVal;
    EEPROM.update(matrixLightIndex, matrixLightDefaultVal);
  }

  LCDLight = EEPROM.read(LCDLightIndex);
  if (!LCDLight || LCDLight > LCDLightMax) {
    LCDLight = LCDLightDefaultVal;
    EEPROM.update(LCDLightIndex, LCDLightDefaultVal);
  }

  sound = EEPROM.read(soundIndex);
  if (sound > soundOn) {
    sound = soundOn;
    EEPROM.update(soundIndex, soundOn);
  }

  gameLevel = EEPROM.read(levelIndex);
  if (!gameLevel || gameLevel > gameLevelMax) {
    gameLevel = gameLevelDefaultVal;
    EEPROM.update(levelIndex, gameLevelDefaultVal);    
  }

  String memoryName = readName(currentNameIndex);
  for (byte i = 0; i < nameSize; i++) {
    name[i] = memoryName[i];
  }
  lc.shutdown(0, false); // turn off power saving, enables display
  lc.setIntensity(0, matrixLight*matrixLightMulti);
  analogWrite(pinLED, LCDLight*LCDLightMulti);
  lcd.begin(16, 2);

  lcd.createChar(0, arrowUp);
  lcd.createChar(1, arrowDown);
  lcd.createChar(2, arrowLeft);
  lcd.createChar(3, arrowRight);
  pinMode(pinSw, INPUT_PULLUP);
  pinMode(pinLED, OUTPUT);
  startMenu();
}
void loop() {
  switch (state) {
    case menuState:
      mainScreen();
      break;
    case playState:
      gameEnd = false;
      startGame();
      break;
    case highscoresState:
      highscores();
      break;
    case aboutState:
      about();
      break;
    case settingsState:
      settings();
      break;
    case endState:
      endGame();
      break;
  }
}