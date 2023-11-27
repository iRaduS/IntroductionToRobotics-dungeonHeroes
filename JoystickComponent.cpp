#include <Arduino.h>
#include "JoystickComponent.h"

JoystickComponent::JoystickComponent(unsigned int xPin, unsigned int yPin, unsigned int swPin) : xPin(xPin), yPin(yPin), swPin(swPin) { }

void JoystickComponent::hook_OnSetup() {
  pinMode(swPin, INPUT_PULLUP);
}

void JoystickComponent::hook_OnLoop() { }

void JoystickComponent::debounceButton() { }

void JoystickComponent::updateMovement() { }