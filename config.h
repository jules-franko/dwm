/* See LICENSE file for copyright and license details. */
#include "X11/XF86keysym.h"

/* appearance */
static const int swallowfloating = 1;
static const unsigned int borderpx  = 3; /*2*/       /* border pixel of windows */
static const unsigned int snap      = 32;       /* snap pixel */
static const unsigned int gappih    = 20;       /* horiz inner gap between windows */
static const unsigned int gappiv    = 10;       /* vert inner gap between windows */
static const unsigned int gappoh    = 10;       /* horiz outer gap between windows and screen edge */
static const unsigned int gappov    = 30;       /* vert outer gap between windows and screen edge */
static const int smartgaps          = 0;        /* 1 means no outer gap when there is only one window */
static const int showbar            = 1;        /* 0 means no bar */
static const int topbar             = 1;        /* 0 means bottom bar */
//static const char *fonts[]          = { "xos4 terminus:size=15","fontawesome" };
//static const char *fonts[]          = { "monospace:size=11", "NotoColorEmoji", "fontawesome" };
static const char *fonts[]          = { "monospace:size=11", "NotoColorEmoji"};
static const char dmenufont[]       = "terminus:size=15";
static const char col_gray1[]       = "#222222";
static const char col_gray2[]       = "#444444";
static const char col_gray3[]       = "#bbbbbb";
static const char col_gray4[]       = "#eeeeee";
static const char col_cyan[]        = "#303030"; /*RED: af0000*/ /*F7F751*/ /*217f42 Green*/ /*005577*/ /*FFFF55*/
static const char *colors[][3]      = {
	/*               fg         bg         border   */
	[SchemeNorm] = { col_gray3, col_gray1, col_gray2 }, /*col_gray2*/
        [SchemeSel]  = { col_gray3, "#005577",  "#cc241d"  },
};

/* tagging */
//static const char *tags[] = { "", "", "", "", "", "", "", "", "" };
static const char *tags[] = { "1", "2", "3", "4", "5", "6", "7", "8", "9", "0" };
//static const char *tags[] = { "[]", "[]", "[]", "[]", "[]", "[]", "[]", "[]", "[9]" };

static const Rule rules[] = {
	/* xprop(1):
	 *	WM_CLASS(STRING) = instance, class
	 *	WM_NAME(STRING) = title
	 */
	/* class     instance  title           tags mask  iscentered  isfloating  isterminal  noswallow  monitor */
	{ "St",      NULL,     NULL,    	 0,        0,          0,           1,         1, 	 -1},
	{ NULL,      NULL,     "stt",    	 0,        1,          1,           1,         1, 	 -1},
	//{ "Thunar",      NULL,  NULL,    	 0,        1,          1,           0,         0, 	 -1},
	{ NULL, NULL, "ranger", 	 	0, 	   1, 	       1,  	    1,	       0,	 -1},
	{ NULL, NULL, "ncmpcpp", 		0, 	  1, 	      1, 	   0,	      0,	 -1},
	{ NULL, NULL, "rmpc", 			0, 	  1, 	      1, 	   1,	      1,	 -1},
	{ NULL, NULL, "newsboat", 		0, 	  0, 	      0, 	   1,	      0,	 -1},
};

/* layout(s) */
static const float mfact     = 0.55; /* factor of master area size [0.05..0.95] */
static const int nmaster     = 1;    /* number of clients in master area */
static const int resizehints = 0;    /* 1 means respect size hints in tiled resizals */
static const int lockfullscreen = 1; /* 1 will force focus on the fullscreen window */

static const Layout layouts[] = {
	/* symbol     arrange function */
	{ "[]=",      tile },    /* first entry is default */
	{ "><>",      NULL },    /* no layout function means floating behavior */
	{ "[M]",      monocle },
};

/* key definitions */
#define MODKEY Mod4Mask
#define TAGKEYS(KEY,TAG) \
	{ MODKEY,                       KEY,      view,           {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask,           KEY,      toggleview,     {.ui = 1 << TAG} }, \
	{ MODKEY|ShiftMask,             KEY,      tag,            {.ui = 1 << TAG} }, \
	{ MODKEY|ControlMask|ShiftMask, KEY,      toggletag,      {.ui = 1 << TAG} },

/* helper for spawning shell commands in the pre dwm-5.0 fashion */
#define SHCMD(cmd) { .v = (const char*[]){ "/bin/sh", "-c", cmd, NULL } }

/* commands */
static char dmenumon[2] = "0"; /* component of dmenucmd, manipulated in spawn() */
static const char *dmenucmd[] = { "dmenu_run", "-m", dmenumon, "-fn", dmenufont, "-nb", col_gray1, "-nf", col_gray3, "-sb", col_cyan, "-sf", col_gray1, NULL };
static const char *termcmd[]  = { "alacritty", NULL };
static const char *termcmdsmall[]  = { "alacritty", "-t", "stt", NULL };
static const char *thunarcmd[]  = { "alacritty", "-t", "ranger", "-e", "ranger", NULL };
static const char *thunarcmd2[]  = { "thunar", NULL };
static const char *librewolfcmd[]  = { "librewolf", NULL };
static const char *changewall[]  = { "wallchanger.sh", NULL };
static const char *bup[]  = { "brightnessctl", "set", "+2%", NULL };
static const char *bdown[]  = { "brightnessctl", "set", "2%-", NULL };
static const char *ss[] = {"screenshotsel", NULL};
static const char *misato[] = {"feh", "/home/kana/Pictures/misato", NULL};
static const char *roficmd[] = { "rofi", "-show", "drun", "-show-icons", NULL };
static const char *music[] = {"alacritty", "-e", "ncmpcpp;", "kill", "-45", "$(pidof", "dwmblocks)", NULL};
static const char *rss[] = {"alacritty", "-e", "newsboat", NULL};
static const char *mail[] = {"alacritty", "-e", "weechat", NULL};
static const char *findsong[] = {"findsong", NULL};
static const char *imageview[] = {"imageview", NULL};
static const char *killprocess[] = {"killprocess", NULL};
static const char *playvideo[] = {"playvideo", NULL};
static const char *passmenu[] = {"passmenu", NULL};
static const char *switchoutput[] = {"switchoutput", NULL};

static const Key keys[] = {
	/* modifier                     key        function        argument */
	{ MODKEY,                       XK_a,      spawn,          {.v = roficmd } },
	{ MODKEY,                       XK_e,      spawn,          {.v = thunarcmd } },
	{ MODKEY,                       XK_z,      spawn,          {.v = thunarcmd2 } },
        { MODKEY,                       XK_f,      spawn,          {.v = librewolfcmd } },
	{ MODKEY,                       XK_Return, spawn,          {.v = termcmd } },
	{ MODKEY|ControlMask,           XK_Return, spawn,          {.v = termcmdsmall } },
	{ MODKEY,                       XK_r,      spawn,          {.v = changewall } },
	{ MODKEY,                       XK_s,      spawn,          {.v = ss } },
	{ MODKEY,                       XK_x,      spawn,          {.v = ( const char*[] ){"notes", NULL} } },
	{ MODKEY,                       XK_y,      spawn,          {.v = misato } },
	{ MODKEY,                       XK_m,      spawn,          SHCMD("alacritty -e ncmpcpp; kill -45 $(pidof dwmblocks)") },
	{ MODKEY,                       XK_n,      spawn,          {.v = rss } },
	{ MODKEY,                       XK_g,      spawn,          {.v = mail } },
	{ MODKEY,                       XK_o,      spawn,          {.v = findsong } },
	//{ MODKEY,                       XK_u,      spawn,          {.v = imageview } },
	{ MODKEY,                       XK_u,      spawn,          {.v = passmenu } },
	{ MODKEY|ControlMask,           XK_q,      spawn,          {.v = killprocess } },
	{ MODKEY|ControlMask,           XK_s,      spawn,          {.v = switchoutput } },
	//{ MODKEY,                       XK_p,      spawn,          {.v = ( const char*[] ){ "mpc", "toggle", NULL } } },
	{ MODKEY,                       XK_p,      spawn,          SHCMD("mpc toggle; kill -45 $(pidof dwmblocks)") },
	{ MODKEY,                       XK_c,      spawn,          {.v = ( const char*[] ){ "alacritty", "-e", "calcurse", NULL } } },
	{ MODKEY,                       XK_v,      spawn,          {.v = ( const char*[] ){ "alacritty", "-e", "nvim", NULL } } },
	{ MODKEY,                       XK_y,      spawn,          {.v = ( const char*[] ){ "alacritty", "-e", "neomutt", NULL } } },
	{ MODKEY,                       XK_BackSpace,      spawn,          {.v = ( const char*[] ){ "dwmactions", NULL } } },
	{ 0,                            XF86XK_MonBrightnessUp,    spawn,          {.v = bup } },
	{ 0,                            XF86XK_MonBrightnessDown,  spawn,          {.v = bdown } },
	//{ 0,                            XF86XK_AudioRaiseVolume,   spawn,          {.v = vup } },
	{ 0,                            XF86XK_AudioRaiseVolume,   spawn,          SHCMD("pactl set-sink-volume @DEFAULT_SINK@ +5%; kill -44 $(pidof dwmblocks)") },
	{ 0,                            XF86XK_AudioLowerVolume,   spawn,          SHCMD("pactl set-sink-volume @DEFAULT_SINK@ -5%; kill -44 $(pidof dwmblocks)") },
	//{ 0,                            XF86XK_AudioLowerVolume,   spawn,          {.v = vdown } },
	{ MODKEY,                       XK_b,      togglebar,      {0} },
	{ MODKEY,                       XK_j,      focusstack,     {.i = +1 } },
	{ MODKEY,                       XK_k,      focusstack,     {.i = -1 } },
	{ MODKEY,                       XK_i,      incnmaster,     {.i = +1 } },
	{ MODKEY,                       XK_d,      incnmaster,     {.i = -1 } },
	{ MODKEY,                       XK_Left,      setmfact,       {.f = -0.05} },
	{ MODKEY,                       XK_Right,      setmfact,       {.f = +0.05} },
	{ MODKEY|ShiftMask,             XK_Return, zoom,           {0} },
	{ MODKEY,                       XK_Tab,    view,           {0} },
	{ MODKEY,                       XK_q,      killclient,     {0} },
	{ MODKEY,                       XK_t,      setlayout,      {.v = &layouts[0]} },
	{ MODKEY|ShiftMask,             XK_f,      setlayout,      {.v = &layouts[1]} },
	{ MODKEY|ShiftMask,             XK_m,      setlayout,      {.v = &layouts[2]} },
	{ MODKEY,                       XK_space,  setlayout,      {0} },
	{ MODKEY,                       XK_w,  togglefloating, {0} },
	{ MODKEY,                       XK_0,      view,           {.ui = ~0 } },
	{ MODKEY|ShiftMask,             XK_0,      tag,            {.ui = ~0 } },
	{ MODKEY,                       XK_comma,  focusmon,       {.i = -1 } },
	{ MODKEY,                       XK_period, focusmon,       {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_comma,  tagmon,         {.i = -1 } },
	{ MODKEY|ShiftMask,             XK_period, tagmon,         {.i = +1 } },
	//{ MODKEY,			XK_Right,  viewnext,       {0} },
	//{ MODKEY,			XK_Left,   viewprev,       {0} },
	//{ MODKEY|ShiftMask,		XK_Right,  tagtonext,      {0} },
	//{ MODKEY|ShiftMask,		XK_Left,   tagtoprev,      {0} },
	{ MODKEY,			XK_l,  viewnext,       {0} },
	{ MODKEY,			XK_h,   viewprev,       {0} },
	{ MODKEY|ShiftMask,		XK_l,  tagtonext,      {0} },
	{ MODKEY|ShiftMask,		XK_h,   tagtoprev,      {0} },

	/*Vanity Gaps*/
	{ MODKEY|ControlMask,                       XK_h,      incrgaps,       {.i = +1 } },
	{ MODKEY|ControlMask,                       XK_l,      incrgaps,       {.i = -1 } },
/*{ MODKEY|Mod4Mask|ShiftMask,    XK_h,      incrogaps,      {.i = +1 } },
	{ MODKEY|Mod4Mask|ShiftMask,    XK_l,      incrogaps,      {.i = -1 } },
	{ MODKEY|Mod4Mask|ControlMask,  XK_h,      incrigaps,      {.i = +1 } },
	{ MODKEY|Mod4Mask|ControlMask,  XK_l,      incrigaps,      {.i = -1 } },
	{ MODKEY|Mod4Mask,              XK_0,      togglegaps,     {0} },
	{ MODKEY|Mod4Mask|ShiftMask,    XK_0,      defaultgaps,    {0} },
	{ MODKEY,                       XK_y,      incrihgaps,     {.i = +1 } },
	{ MODKEY,                       XK_o,      incrihgaps,     {.i = -1 } },
	{ MODKEY|ControlMask,           XK_y,      incrivgaps,     {.i = +1 } },
	{ MODKEY|ControlMask,           XK_o,      incrivgaps,     {.i = -1 } },
	{ MODKEY|Mod4Mask,              XK_y,      incrohgaps,     {.i = +1 } },
	{ MODKEY|Mod4Mask,              XK_o,      incrohgaps,     {.i = -1 } },
	{ MODKEY|ShiftMask,             XK_y,      incrovgaps,     {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_o,      incrovgaps,     {.i = -1 } },*/
	{ MODKEY|ControlMask,			XK_Down,	moveresize,		{.v = (int []){ 0, 25, 0, 0 }}},
	{ MODKEY|ControlMask,			XK_Up,		moveresize,		{.v = (int []){ 0, -25, 0, 0 }}},
	{ MODKEY|ControlMask,			XK_Right,	moveresize,		{.v = (int []){ 25, 0, 0, 0 }}},
	{ MODKEY|ControlMask,			XK_Left,	moveresize,		{.v = (int []){ -25, 0, 0, 0 }}},
	{ MODKEY|ShiftMask|ControlMask,		XK_Down,	moveresize,		{.v = (int []){ 0, 0, 0, 25 }}},
	{ MODKEY|ShiftMask|ControlMask,		XK_Up,		moveresize,		{.v = (int []){ 0, 0, 0, -25 }}},
	{ MODKEY|ShiftMask|ControlMask,		XK_Right,	moveresize,		{.v = (int []){ 0, 0, 25, 0 }}},
	{ MODKEY|ShiftMask|ControlMask,		XK_Left,	moveresize,		{.v = (int []){ 0, 0, -25, 0 }}},
	TAGKEYS(                        XK_1,                      0)
	TAGKEYS(                        XK_2,                      1)
	TAGKEYS(                        XK_3,                      2)
	TAGKEYS(                        XK_4,                      3)
	TAGKEYS(                        XK_5,                      4)
	TAGKEYS(                        XK_6,                      5)
	TAGKEYS(                        XK_7,                      6)
	TAGKEYS(                        XK_8,                      7)
	TAGKEYS(                        XK_9,                      8)
	{ MODKEY|ShiftMask,             XK_e,      quit,           {0} },
	{ 0, XF86XK_AudioPrev,                         spawn,                  {.v = (const char*[]){ "mpc", "prev", NULL } } },
	{ 0, XF86XK_AudioNext,                         spawn,                  {.v = (const char*[]){ "mpc", "next", NULL } } },
	{ MODKEY|ShiftMask,             XK_bracketright,spawn,                 SHCMD("mpc next; kill -45 $(pidof dwmblocks)") },
	{ MODKEY|ShiftMask,             XK_bracketleft,spawn,                  SHCMD("mpc prev; kill -45 $(pidof dwmblocks)") },
	{ MODKEY,		        XK_bracketleft,spawn,                  SHCMD("mpc seek -10; kill -45 $(pidof dwmblocks)") },
	{ MODKEY,		        XK_bracketright,spawn,                 SHCMD("mpc seek +10; kill -45 $(pidof dwmblocks)") },
	{ 0, XF86XK_AudioPause,                        spawn,                  {.v = (const char*[]){ "mpc", "pause", NULL } } },
	{ 0, XF86XK_AudioPlay,                         spawn,                  {.v = (const char*[]){ "mpc", "play", NULL } } },
	{ 0, XF86XK_AudioStop,                         spawn,                  {.v = (const char*[]){ "mpc", "stop", NULL } } },
	{ 0, XF86XK_AudioRewind,                       spawn,                  {.v = (const char*[]){ "mpc", "seek", "-10", NULL } } },
	{ 0, XF86XK_AudioForward,                      spawn,                  {.v = (const char*[]){ "mpc", "seek", "+10", NULL } } },
	{ 0, XF86XK_AudioMedia,                        spawn,                  {.v = (const char*[]){ "st",  "-e", "ncmpcpp", NULL } } },
	{ 0, XF86XK_AudioMicMute,                      spawn,                  SHCMD("pactl set-source-mute @DEFAULT_SOURCE@ toggle") },
};

/* button definitions */
/* click can be ClkTagBar, ClkLtSymbol, ClkStatusText, ClkWinTitle, ClkClientWin, or ClkRootWin */
static const Button buttons[] = {
	/* click                event mask      button          function        argument */
	{ ClkLtSymbol,          0,              Button1,        setlayout,      {0} },
	{ ClkLtSymbol,          0,              Button3,        setlayout,      {.v = &layouts[2]} },
	{ ClkWinTitle,          0,              Button2,        zoom,           {0} },
	{ ClkStatusText,        0,              Button2,        spawn,          {.v = termcmd } },
	{ ClkClientWin,         MODKEY,         Button1,        movemouse,      {0} },
	{ ClkClientWin,         MODKEY,         Button2,        togglefloating, {0} },
	{ ClkClientWin,         MODKEY,         Button3,        resizemouse,    {0} },
	{ ClkTagBar,            0,              Button1,        view,           {0} },
	{ ClkTagBar,            0,              Button3,        toggleview,     {0} },
	{ ClkTagBar,            MODKEY,         Button1,        tag,            {0} },
	{ ClkTagBar,            MODKEY,         Button3,        toggletag,      {0} },
};

