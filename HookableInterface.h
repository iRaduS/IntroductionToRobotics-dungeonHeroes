#ifndef HookableInterface_h
#define HookableInterface_h

class HookableInterface {
public:
  virtual void hook_OnLoop() = 0;
  virtual void hook_OnSetup() = 0;
};

#endif