#ifndef _KAGARI_DEFS_H
#define _KAGARI_DEFS_H

#define cViewName "Kamnagi.KagariView"
#define cAppSignature "application/x-vnd.Kamnagi.Kagari"
#define cDeskbarSignature "application/x-vnd.be-tskb"

static const int cMaxDisplayItem = 20;
static const int cSecondPerMonth = 60 * 60 * 24 * 30;

enum
{
  K_PREFERENCES_REQUESTED = 256,
  K_PREFERENCES_QUITED,
  K_ABOUT_REQUESTED,
  K_ABOUT_QUITED,

  K_INFO_INIT,
};


enum
{
  K_DISPLAY_LAUNCHER_AND_DESKBAR = 1,
  K_DISPLAY_LAUNCHER,
  K_DISPLAY_DESKBAR,
};


enum
{
  LAUNCH_REQUESTED = 'Lreq',
  REFRESH_REQUESTED = 'Rreq',
  SET_EXPIRE_PERIOD = 'Sexp',
  SET_TOP_N = 'Stop',
  REMOVE_MYSELF = 'Rslf',
};

#endif
