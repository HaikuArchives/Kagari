// about dialog
//

#ifndef KAGARI_ABOUT_H
#define KAGARI_ABOUT_H

#include "YDialog.h"
#include "SpGLView.h"

class KagariAbout : public YDialog
{
public:
  KagariAbout(void);
  ~KagariAbout(void);

private:
  void ValueModified(const char *name, int32 value);

  SpGLView *mSp;
};

#endif
