#include <EEPROM.h>
#include <LiquidCrystal.h>
#include <LedControl.h>
#define INIT_MENU_SELECTION 0
#define MAX_NAME_SIZE 3
#define DEFAULT_AXIS_POS 7

// Declare the enum of the game status
enum GameStateEnum {
  HELLO_SCREEN,
  INTRO_SCREEN,
  SETTINGS_SCREEN,
  PAUSE_SCREEN,
  GAME_SCREEN,
  END_ROUND_SCREEN,
  END_GAME_SCREEN
};
GameStateEnum gameState = HELLO_SCREEN, oldGameState = HELLO_SCREEN;
unsigned int menuSelection = INIT_MENU_SELECTION;
const unsigned int menuMessageScrollTime = 500, menuScrollTime = 500;
unsigned long currentMessageScrollTime, currentScrollTime;

// Declare the variables for the LCD
const unsigned int rs = 9,
  en = 8,
  d4 = 7,
  d5 = 6,
  d6 = 5,
  d7 = 4;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
byte bombChar[8] = {
	0b00110,
	0b01000,
	0b00100,
	0b01110,
	0b11111,
	0b10111,
	0b11111,
	0b01110
};
bool showOnceMessage = false;

// Declare the variables for the joystick
const unsigned int xAxis = A0,
  yAxis = A2,
  swButton = 3;
unsigned int xValue, yValue, swValue, oldSwValue;

// Declare a function to show a menu according to the screen
void lcdShowMenu() {
  if (oldGameState != gameState || gameState == HELLO_SCREEN) {
    lcd.begin(16, 2);
    lcd.clear();
    lcd.setCursor(0, 0);
  }

  switch (gameState) {
    case HELLO_SCREEN: {
      char *playerName = new char[MAX_NAME_SIZE];
      sprintf(playerName, "%c%c%c", EEPROM.read(0), EEPROM.read(1), EEPROM.read(2));

      if (!strcmp(playerName, "000")) {
        const char welcomeMessage[] = "Hello! Welcome to the bomberman game.";
        size_t welcomeMessageLength = strlen(welcomeMessage);

        lcd.print(welcomeMessage);

        currentMessageScrollTime = millis();
        while (welcomeMessageLength != (1 << 4)) {
          if ((millis() - currentMessageScrollTime) > menuMessageScrollTime) {
            currentMessageScrollTime = millis();
            welcomeMessageLength --;
            
            lcd.scrollDisplayLeft();
          }
        }
      } else {
        char customPlayerMessage[32];
        sprintf(customPlayerMessage, "Good to see you back, %s!", playerName);
        size_t customPlayerMessageLength = strlen(customPlayerMessage);

        lcd.print(customPlayerMessage);

        currentMessageScrollTime = millis();
        while (customPlayerMessageLength != (1 << 4)) {
          if ((millis() - currentMessageScrollTime) > menuMessageScrollTime) {
            currentMessageScrollTime = millis();
            customPlayerMessageLength --;
            
            lcd.scrollDisplayLeft();
          }
        }
      }

      if (playerName != NULL) {
        delete[] playerName;
      }
      break;
    }
    case INTRO_SCREEN: {
      if (!showOnceMessage) {
        lcd.setCursor(2, 0);
        lcd.write(byte(0));
        lcd.print("Bomberman");
        lcd.write(byte(0));

        showOnceMessage = true;
      }

      lcd.blink();
      switch (menuSelection) {
        case 0: {
          clearLcdLine(1);
          lcd.setCursor(0, 1);
          lcd.print("> Start new game");
          lcd.setCursor(0, 1);
          break;
        }
        case 1: {
          clearLcdLine(1);
          lcd.setCursor(0, 1);
          lcd.print("> Settings");
          lcd.setCursor(0, 1);
          break;
        }
        case 2: {
          clearLcdLine(1);
          lcd.setCursor(0, 1);
          lcd.print("> About the game");
          lcd.setCursor(0, 1);
          break;
        }
      }
      break;
    }
  }
}

void clearLcdLine(int line) {
  lcd.setCursor(0, line);
  for (unsigned int i = 0; i < 20; i++) {
    lcd.print(" ");
  }
}

void setup() {
  Serial.begin(9600);
  pinMode(swButton, INPUT_PULLUP);
  lcd.createChar(0, bombChar);

  gameState = HELLO_SCREEN;
  lcdShowMenu();
  delay(250); // It doesn't matter if the whole system freezes because it's for the hello message screen not needed for complex delaying with millis

  gameState = INTRO_SCREEN;
}

void loop() {
  xValue = analogRead(xAxis);
  yValue = analogRead(yAxis);

  // Correct the interval of the joystick axis
  xValue = map(xValue, 0, (1 << 10) - 1, 0, (1 << 4) - 1);
  yValue = map(yValue, 0, (1 << 10) - 1, 0, (1 << 4) - 1);

  Serial.print(xValue);
  Serial.print(" | ");
  Serial.println(yValue);

  // Show new LCD screen based on the menu setup
  if (gameState != oldGameState) {
    lcdShowMenu();

    showOnceMessage = false;
    oldGameState = gameState;
  }

  switch (gameState) {
    case INTRO_SCREEN: {
      if ((millis() - currentScrollTime) > menuScrollTime && xValue != DEFAULT_AXIS_POS) {
        menuSelection += (xValue < 7) ? -1 : 1, currentScrollTime = millis();
        lcdShowMenu();
      }
      break;
    }
  }
}
