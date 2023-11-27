#ifndef LedControlComponent_h
#define LedControlComponent_h
#include <LedControl.h>
#include "HookableInterface.h"

class LedControlComponent : public HookableInterface {
private:
  LedControl ledControl;
public:
  LedControlComponent(LedControl&);
  void hook_OnLoop();
  void hook_OnSetup();
  LedControl& getLedControl();
};

#endif