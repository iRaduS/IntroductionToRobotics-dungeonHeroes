#include <LedControl.h>
#include "LedControlComponent.h"

LedControlComponent::LedControlComponent(LedControl &ledControl) : ledControl(ledControl) { }

void LedControlComponent::hook_OnSetup() { }

void LedControlComponent::hook_OnLoop() { }

LedControl& LedControlComponent::getLedControl() {
  return ledControl;
}