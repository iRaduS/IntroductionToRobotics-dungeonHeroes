#include <EEPROM.h>
#include <LiquidCrystal.h>
#include <LedControl.h>

#include "JoystickComponent.h"
const unsigned int xPin = A0, 
  yPin = A1, 
  swPin = 3;
JoystickComponent *joystickComponent = new JoystickComponent(xPin, yPin, swPin);
 
#include "LedControlComponent.h"
const unsigned int dinPin = 12,
  clockPin = 11,
  loadPin = 10;
LedControl ledControl = LedControl(dinPin, clockPin, loadPin);
LedControlComponent *ledControlComponent = new LedControlComponent(ledControl);

#define DEFINED_COMPONENTS 2
HookableInterface* componentsDefined[DEFINED_COMPONENTS] = {joystickComponent, ledControlComponent};

void setup() {
  for (unsigned int i = 0; i < DEFINED_COMPONENTS; ++i) {
    componentsDefined[i]->hook_OnSetup();
  }
}

void loop() {
  for (unsigned int i = 0; i < DEFINED_COMPONENTS; ++i) {
    componentsDefined[i]->hook_OnLoop();
  }
}
