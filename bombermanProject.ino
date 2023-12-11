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
  ABOUT_SCREEN,
  GAME_SCREEN,
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
                   d7 = 4,
                   pwmPin = 3;
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
byte lcdBrightness = EEPROM.read(4);

// Declare the variables for the led matrix
const unsigned int dinPin = 12,
                   clockPin = 11,
                   loadPin = 10;
LedControl lc = LedControl(dinPin, clockPin, loadPin, 1);
byte matrixBrightness = EEPROM.read(3);

// Declare the variables for the joystick
const unsigned int xAxis = A0,
                   yAxis = A2,
                   swPin = 1;
unsigned int xValue, yValue;
byte lastSwButtonState = HIGH, swButtonState = HIGH;
unsigned long lastSwDebounceTime = 0, debounceDelay = 50;

// Declare a function to show a menu according to the screen
void lcdShowMenu() {
  if (oldGameState != gameState || gameState == HELLO_SCREEN) {
    lcd.begin(16, 2);
    lcd.clear();
    lcd.setCursor(0, 0);
  }

  switch (gameState) {
    case HELLO_SCREEN:
      {
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
              welcomeMessageLength--;

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
              customPlayerMessageLength--;

              lcd.scrollDisplayLeft();
            }
          }
        }

        if (playerName != NULL) {
          delete[] playerName;
        }
        break;
      }
    case INTRO_SCREEN:
      {
        if (!showOnceMessage) {
          lcd.setCursor(2, 0);
          lcd.write(byte(0));
          lcd.print("Bomberman");
          lcd.write(byte(0));

          showOnceMessage = true;
        }

        lcd.blink();
        switch (menuSelection) {
          case 0:
            {
              clearLcdLine(1);
              lcd.setCursor(0, 1);
              lcd.print("> Start new game");
              lcd.setCursor(0, 1);
              break;
            }
          case 1:
            {
              clearLcdLine(1);
              lcd.setCursor(0, 1);
              lcd.print("> Settings");
              lcd.setCursor(0, 1);
              break;
            }
          case 2:
            {
              clearLcdLine(1);
              lcd.setCursor(0, 1);
              lcd.print("> About the game");
              lcd.setCursor(0, 1);
              break;
            }
        }
        break;
      }
    case ABOUT_SCREEN:
      {
        lcd.clear();
        lcd.noBlink();
        lcd.setCursor(0, 0);
        lcd.print("Get more info:");
        lcd.setCursor(0, 1);
        lcd.print("https://git.io/...");
        break;
      }
    case SETTINGS_SCREEN:
      {
        switch (menuSelection) {
          case 0:
            {
              if (!showOnceMessage) {
                lcd.clear();
                lcd.noBlink();
                lcd.setCursor(0, 0);
                lcd.print("LCD Brightness");

                showOnceMessage = true;
              }
              clearLcdLine(1);
              lcd.setCursor(0, 1);
              lcd.print("> ");
              lcd.print(lcdBrightness);
              break;
            }
          case 1:
            {
              if (!showOnceMessage) {
                lcd.clear();
                lcd.noBlink();
                lcd.setCursor(0, 0);
                lcd.print("LED Brightness");

                showOnceMessage = true;
              }
              clearLcdLine(1);
              lcd.setCursor(0, 1);
              lcd.print("> ");
              lcd.print(matrixBrightness);
              break;
            }
          case 2:
            {
              lcd.clear();
              lcd.noBlink();
              lcd.setCursor(0, 0);
              lcd.print("Back");
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

void debounceSwitchButton() {
  int reading = digitalRead(swPin);
  Serial.print("Reading switch Button: ");
  Serial.println(reading);

  if (reading != lastSwButtonState) {
    lastSwDebounceTime = millis();
  }

  if ((millis() - lastSwDebounceTime) > debounceDelay) {
    if (reading != swButtonState) {
      swButtonState = reading;

      if (swButtonState == LOW) {
        switch (gameState) {
          case INTRO_SCREEN:
            {
              switch (menuSelection) {
                case 0:
                  {
                    // start the game
                    break;
                  }
                case 1:
                  {
                    gameState = SETTINGS_SCREEN;
                    menuSelection = INIT_MENU_SELECTION;
                    break;
                  }
                case 2:
                  {
                    gameState = ABOUT_SCREEN;
                    break;
                  }
              }
              break;
            }
          case ABOUT_SCREEN:
            {
              gameState = INTRO_SCREEN;
              break;
            }
          case SETTINGS_SCREEN:
            {
              switch (menuSelection) {
                case 0:
                  {
                    EEPROM.update(4, lcdBrightness);
                    break;
                  }
                case 1:
                  {
                    EEPROM.update(3, matrixBrightness);
                    break;
                  }
                case 2:
                  {
                    showOnceMessage = false;
                    gameState = INTRO_SCREEN;
                    menuSelection = 1;
                    break;
                  }
              }
              break;
            }
        }
      }
    }
  }
  lastSwButtonState = reading;
}

void setup() {
  Serial.begin(9600);

  lc.shutdown(0, false);
  lc.setIntensity(0, matrixBrightness);
  lc.clearDisplay(0);
  lc.setLed(0, 0, 0, HIGH);

  pinMode(swPin, INPUT_PULLUP);
  pinMode(pwmPin, OUTPUT);
  analogWrite(pwmPin, lcdBrightness);
  lcd.createChar(0, bombChar);

  gameState = HELLO_SCREEN;
  lcdShowMenu();
  delay(250);  // It doesn't matter if the whole system freezes because it's for the hello message screen not needed for complex delaying with millis

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
    showOnceMessage = false;
    lcdShowMenu();

    oldGameState = gameState;
  }

  switch (gameState) {
    case INTRO_SCREEN:
      {
        if ((millis() - currentScrollTime) > menuScrollTime && xValue != DEFAULT_AXIS_POS) {
          menuSelection += (xValue < 7) ? -1 : 1, currentScrollTime = millis();

          if (menuSelection < 0) {
            menuSelection = 0;
          } else if (menuSelection > 2) {
            menuSelection = 2;
          }
          lcdShowMenu();
        }
        break;
      }
    case SETTINGS_SCREEN:
      {
        if ((millis() - currentScrollTime) > menuScrollTime && yValue != DEFAULT_AXIS_POS) {
          if (menuSelection != 2) {
            if (menuSelection) {
              matrixBrightness += (yValue < 7) ? 1 : -1, currentScrollTime = millis();
              matrixBrightness = matrixBrightness % 256;
              lc.setIntensity(0, matrixBrightness);
            } else {
              lcdBrightness += (yValue < 7) ? 1 : -1, currentScrollTime = millis();
              lcdBrightness = lcdBrightness % 256;
              // @TODO ask in lab how to evict noise from PWM
            }
            lcdShowMenu();
          }
        } else if ((millis() - currentScrollTime) > menuScrollTime && xValue != DEFAULT_AXIS_POS) {
          showOnceMessage = false;
          menuSelection += (xValue < 7) ? -1 : 1, currentScrollTime = millis();

          if (menuSelection < 0) {
            menuSelection = 0;
          } else if (menuSelection > 2) {
            menuSelection = 2;
          }
          lcdShowMenu();
        }
        break;
      }
  }

  // Debounce joystick button
  debounceSwitchButton();
}
