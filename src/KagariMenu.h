//======================================================================
// Kagari
//  Application Menu.

#ifndef _KAGARI_MENU_H
#define _KAGARI_MENU_H

#include <InterfaceKit.h>
#include <StorageKit.h>
#include <map>
#include "KagariInfo.h"

//======================================================================
// pop up menu.

class
KagariAppMenu : public BPopUpMenu
{
public:
  KagariAppMenu(const char *name, KagariInfo *info);
  ~KagariAppMenu(void);

protected:
  bool AddDynamicItem(add_state e);
  virtual BPoint ScreenLocation(void);

private:
  enum position_state
  {
    POSITION_UPPER,
    POSITION_LOWER,
  };

  void Update(void);
  BMenuItem *MakeAppItem(uint32 command, const struct AppRef *ref);
  void AddLastItem(BMenu* menu, BMenuItem *item);
  void AddLastItem(BMenu *menu, BMenu *item);
  void LookUpPosition(void);

  uint32 mTopNum;
  BMenu *mAllMenu;
  KagariInfo *mInfo;
  position_state mPosition;
};

#endif
