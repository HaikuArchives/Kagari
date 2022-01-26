// Preferences

#include <cstring>
#include <cstdlib>
#include <string>
#include "KagariApp.h"
#include "YDialog.h"
#include "SpGLView.h"

static const char
sDialogDefinition[] =
" \
general = \
  box -style fancy -label 'General Settings' -mergin 5 -align fill { \
    list -align fill { \
      slider -name expire_date -label 'Expire Date' -halign fill \
          -min 1 -max 30 -width 150 -height 20 -async 1, \
      box -style fancy -label 'Show' -mergin 5 -align fill { \
        list -align fill { \
          radio -name display_flag -value 1 -label 'both Deskbar and Launcher', \
          radio -name display_flag -value 2 -label 'Launcher only', \
          radio -name display_flag -value 3 -label 'Deskbar only', \
        } \
      } \
    } \
  }; \
 \
launcher = \
  box -style fancy -label 'Floating Launcher' -mergin 5 -align fill { \
    list -align fill { \
      radio -name pane_icon_size -value 16 -label 'Mini Icon', \
      radio -name pane_icon_size -value 32 -label 'Large Icon',  \
      checkbox -name pane_on_top -label 'Always on Top', \
      checkbox -name pane_lock -label 'Lock' , \
    }  \
  }; \
 \
deskbar = \
  box -style fancy -label 'Deskbar Menu' -mergin 5 -align fill{ \
    list -align fill { \
      radio -name menu_icon_size -value 16 -label 'Mini Icon', \
      radio -name menu_icon_size -value 32 -label 'Large Icon', \
      checkbox -name menu_avoid_deskbar -value 0 -label 'Avoid Deskbar', \
      slider -name menu_display_top -label 'Display top' -halign fill \
              -min 0 -max 30 -width 150 -height 20 -async 1 \
    }, \
  }; \
 \
 \
 \
top = \
  list -mergin 5 -padding 5 -align fill { \
    tab -mergin 5 -align fill {  \
      general -tablabel 'General', \
      launcher -tablabel 'Launcher', \
      deskbar -tablabel  'Deskbar', \
    }, \
  }; \
";


KagariPreferences::KagariPreferences(void)
  : YDialog(sDialogDefinition,
	    BPoint(100, 100),
	    "Preferences",
	    B_TITLED_WINDOW_LOOK,
	    B_NORMAL_WINDOW_FEEL,
	    B_NOT_ZOOMABLE | B_NOT_RESIZABLE,
	    B_ALL_WORKSPACES)
{
  this->SetValue("pane_icon_size", k_pref->GetInt32("pane_icon_size"));
  this->SetValue("menu_icon_size", k_pref->GetInt32("menu_icon_size"));
  this->SetValue("display_flag", k_pref->GetInt32("display_flag"));
  this->SetValue("menu_display_top", k_pref->GetInt32("menu_display_top"));
  this->SetValue("menu_avoid_deskbar", k_pref->GetInt32("menu_avoid_deskbar"));
  this->SetValue("expire_date", k_pref->GetInt32("expire_date"));
  this->SetValue("pane_lock", k_pref->GetInt32("pane_lock"));
  this->SetValue("pane_on_top", k_pref->GetInt32("pane_on_top"));
  this->UpdateLabel("menu_display_top");
  this->UpdateLabel("expire_date");
  this->MoveTo(k_pref->GetPoint("pref_position"));
}


KagariPreferences::~KagariPreferences(void)
{
  k_pref->SetPoint("pref_position",
		   BPoint(this->Frame().left, this->Frame().top));

  k_app->PostMessage(K_PREFERENCES_QUITED);
}

void
KagariPreferences::UpdateLabel(const char *name)
{
  int32 value;
  char buf[256];

  value = this->Value(name);
  if (strcmp(name, "menu_display_top") == 0)
    {
      if (value == 0)
	sprintf(buf, "Dispaly no icon.");
      else if (value == 1)
	sprintf(buf, "Display an icon.");
      else 
	sprintf(buf, "Display top %d icons.", (int)value);
    }
  else if (strcmp(name, "expire_date") == 0)
    {
      if (value == 1)
	sprintf(buf, "Expire after a day.");
      else
	sprintf(buf, "Expire after %d days.", (int)value);
    }

  this->SetLabel(name, buf);
}


void
KagariPreferences::ValueModified(const char *name, int32 value)
{
  string cmd(name);

  if (cmd == "pane_icon_size" ||
      cmd == "menu_icon_size" ||
      cmd == "menu_avoid_deskbar" ||
      cmd == "display_flag" ||
      cmd == "menu_display_top" ||
      cmd == "expire_date" ||
      cmd == "pane_on_top" ||
      cmd == "pane_lock")
    k_pref->SetInt32(name, value);

  else 
    cout << name << " " << value << "\n";

  if (cmd == "menu_display_top" ||
      cmd == "expire_date")
    this->UpdateLabel(name);
}


