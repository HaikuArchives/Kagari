// about
//

#include "KagariDefs.h"
#include "KagariAbout.h"

static const char 
sDialogDefinition[] =
" \
top = \
  box -style no_border -mergin 5 -align fill { \
    box -style fancy -label 'About Kagari' -mergin 5 -align fill { \
      list -orientation horizontal -padding 5 -align fill { \
        list -padding 5 -align fill { \
          slider -name gl_set_r -label 'R' -value 50 -halign fill \
            -min 0 -max 100 -width 10 -height 20 -async 1, \
          slider -name gl_set_v -label 'V' -value 50 -halign fill \
            -min 0 -max 100 -width 10 -height 20 -async 1, \
          button -name gl_expand  -label 'Expand' , \
          button -name gl_restart -label 'Restart', \
        }, \
        adaptor -name gl_adaptor \
      } \
    } \
  }; \
";


KagariAbout::KagariAbout(void)
  : YDialog(sDialogDefinition,
	    BPoint(100, 100),
	    "About",
	    B_TITLED_WINDOW_LOOK,
	    B_NORMAL_WINDOW_FEEL,
	    B_NOT_ZOOMABLE | B_NOT_RESIZABLE,
	    B_ALL_WORKSPACES)
  , mSp(NULL)
{
  mSp = 
    new SpGLView(BRect(0, 0, 150, 150), 
		 BGL_RGB | BGL_DOUBLE | BGL_DEPTH);
  mSp->SetResizingMode(B_FOLLOW_RIGHT | B_FOLLOW_V_CENTER);
  this->AdaptView("gl_adaptor", mSp);
  mSp->SetR(0.5);
  mSp->SetV(0.5);

  this->ResizeToPreferred();
}


KagariAbout::~KagariAbout(void)
{
  be_app->PostMessage(K_ABOUT_QUITED);
}


void
KagariAbout::ValueModified(const char *name, int32 value)
{
  string cmd(name);

  if (cmd == "gl_set_r")
    mSp->SetR((float)this->Value(name) / 100.0);
  else if (cmd == "gl_set_v")
    mSp->SetV((float)this->Value(name) / 100.0);
  else if (cmd == "gl_expand")
    mSp->Expand(1.2);
  else if (cmd == "gl_restart")
    {
      mSp->Restart();
      mSp->SetR((float)this->Value("gl_set_r") / 100.0);
      mSp->SetV((float)this->Value("gl_set_v") / 100.0);
    }
}
