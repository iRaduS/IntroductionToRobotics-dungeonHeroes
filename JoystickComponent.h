#ifndef JoystickComponent_h
#define JoystickComponent_h
#include "HookableInterface.h"

class JoystickComponent : public HookableInterface {
private:
  const unsigned int xPin, yPin, swPin;
public:
  JoystickComponent(unsigned int, unsigned int, unsigned int);
  void hook_OnLoop();
  void hook_OnSetup();
  void debounceButton();
  void updateMovement();
};

#endif