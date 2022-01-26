// Preferences Panel.
//

#ifndef KAGARI_PREFERENCES_H
#define KAGARI_PREFERENCES_H

#include "YDialog.h"

class KagariPreferences : public YDialog
{
public:
  KagariPreferences(void);
  ~KagariPreferences(void);

private:
  void ValueModified(const char *name, int32 value);
  void UpdateLabel(const char *name);
};


#endif
