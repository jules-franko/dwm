/* See LICENSE file for copyright and license details. */
#include "X11/XF86keysym.h"

/*MACROS*/
#define TERMINAL "st"
#define BROWSER "firefox"

/* appearance */
static const unsigned int borderpx  = 3;        /* border pixel of windows */
static const unsigned int snap      = 32;       /* snap pixel */
static const int swallowfloating    = 1;        /* 1 means swallow floating windows by default */
static const unsigned int gappih    = 20;       /* horiz inner gap between windows */
static const unsigned int gappiv    = 10;       /* vert inner gap between windows */
static const unsigned int gappoh    = 30;       /* horiz outer gap between windows and screen edge */
static const unsigned int gappov    = 50;       /* vert outer gap between windows and screen edge */
static const int smartgaps          = 0;        /* 1 means no outer gap when there is only one window */
static const int showbar            = 1;        /* 0 means no bar */
static const int topbar             = 1;        /* 0 means bottom bar */
static const char *fonts[]          = { "JetBrainsMono:size=11.5", "Symbols Nerd Font:size=14", "NotoColorEmoji:size=14"};
//static const char *fonts[]          = { "monospace:size=11", "Symbols Nerd Font:size=14", "NotoColorEmoji:size=14"};
//static const char *fonts[]          = { "xos4 terminus:size=13", "Symbols Nerd Font:size=14", "NotoColorEmoji:size=14", "Font Awesome 6 Free Regular:size=14", "Font Awesome 6 Brands Regular:size=14", "Font Awesome v4 Compatibility:size=14"};
static const char dmenufont[]       = "monospace:size=11";
static const char col_gray1[]       = "#222222";
static const char col_gray2[]       = "#444444";
static const char col_gray3[]       = "#bbbbbb";
static const char col_gray4[]       = "#eeeeee";
static const char col_cyan[]        = "#005577";
static const char *colors[][SchemeN][3] = {
		/*               fg         bg         border   */
	{ /* default */
		[SchemeNorm] = { col_gray3, col_gray1, col_gray2 },
		[SchemeSel]  = { col_gray4, col_cyan,  col_cyan  },
	},
	{ /* gruvbox */
		[SchemeNorm] = { "#bdae93", "#282828", "#504945" },
		[SchemeSel]  = { "#fbf1c7", "#98971a", "#83a598"  },
	},
	{ /* gruvbox */
		[SchemeNorm] = { "#bdae93", "#282828", "#504945" },
		[SchemeSel]  = { "#fbf1c7", "#d88b27", "#83a598"  },
	},
	{ /* default */
		[SchemeNorm] = { "#f3edf4", col_gray1,  col_gray2 },
		[SchemeSel]  = { "#8c5d99", "#f3edf4",  "#8c5d99"  },
	},
};

/* tagging */
//static const char *tags[] = { "1", "2", "3", "4", "5", "6", "7", "8", "9" };
//static const char *tags[] = { "󰣇", "󰈹", "󰝚", "󰭹", "󰸼", "", "", "", "" };
static const char *tags[] = { "", "󰈹", "󰝚", "󰭹", "󰸼", "", "", "", "" };

static const char ptagf[] = "[%s %s]";	/* format of a tag label */
static const char etagf[] = "[%s]";	/* format of an empty tag */
static const int lcaselbl = 1;		/* 1 means make tag label lowercase */	

static const unsigned int ulinepad	= 5;	/* horizontal padding between the underline and tag */
static const unsigned int ulinestroke	= 3;	/* thickness / height of the underline */
static const unsigned int ulinevoffset	= 0;	/* how far above the bottom of the bar the line should appear */
static const int ulineall 		= 0;	/* 1 to show underline on all tags, 0 for just the active ones */

static const Rule rules[] = {
	/* xprop(1):
	 *	WM_CLASS(STRING) = instance, class
	 *	WM_NAME(STRING) = title
	 */
	/* class     instance  title           tags mask  isfloating  isterminal  noswallow  monitor */
	{ BROWSER,   NULL,     NULL,           1 << 1,    0,          0,           0,        -1 },
	{ "goofcord",NULL,     NULL,           1 << 3,    0,          0,           0,        -1 },
	{ "Spotify", NULL,     NULL,           1 << 2,    0,          0,           0,        -1 },
	{ "steam",   NULL,     NULL,           1 << 4,    0,          0,           0,        -1 },
	//{ "st",      NULL,     NULL,           0,         0,          1,           0,        -1 },
	{ NULL,      NULL,     "ncmpcpp",      0,         1,          1,           0,        -1 },
	{ NULL,      NULL,     "Error",        1 << 3,    1,          1,           0,        -1 },
	{ NULL,      NULL,     "ranger",       0,         0,          1,           1,        -1 },
	{ NULL,      NULL,     "stt",          0,         1,          1,           0,        -1 },
	{ NULL,      NULL,     "st",           0,         0,          1,           0,        -1 },
	{ NULL,      NULL,     "Event Tester", 0,         0,          0,           1,        -1 }, /* xev */
};

/* layout(s) */
static const float mfact     = 0.55; /* factor of master area size [0.05..0.95] */
static const int nmaster     = 1;    /* number of clients in master area */
static const int resizehints = 0;    /* 1 means respect size hints in tiled resizals */
static const int lockfullscreen = 1; /* 1 will force focus on the fullscreen window */

#include "fibonacci.c"
static const Layout layouts[] = {
	/* symbol     arrange function */
	{ "[]=",      tile },    /* first entry is default */
	{ "><>",      NULL },    /* no layout function means floating behavior */
	{ "[M]",      monocle },
 	{ "[@]",      spiral },
 	{ "[\\]",      dwindle },
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
static const char *dmenucmd[] = { "dmenu_run", "-m", dmenumon, "-fn", dmenufont, "-nb", col_gray1, "-nf", col_gray3, "-sb", col_cyan, "-sf", col_gray4, NULL };
static const char *termcmd[]  = { "st", NULL };

static const Key keys[] = {
	/* modifier                     key        function        argument */
	{ MODKEY,                       XK_F1,     spawn,         SHCMD( TERMINAL " -e nvim $HOME/.local/src/dwm/config.h") },
	{ MODKEY,                       XK_a,      spawn,          {.v = dmenucmd } },
	{ MODKEY,                       XK_e,      spawn,          SHCMD( TERMINAL " -t ranger -e ranger") },
	{ MODKEY,                       XK_Return, spawn,          SHCMD( TERMINAL ) },
	{ MODKEY,                       XK_z,      spawn,          SHCMD( "thunar" ) },
        { MODKEY,                       XK_f,      spawn,          SHCMD( BROWSER ) },
	{ MODKEY|ControlMask,           XK_Return, spawn,          SHCMD( "st -t stt") },
	{ MODKEY,                       XK_r,      spawn,          SHCMD( "wallchanger.sh") },
	{ MODKEY,                       XK_s,      spawn,          SHCMD( "screenshotxclip" ) },
	{ MODKEY|ShiftMask,             XK_s,      spawn,          SHCMD( "screenshotsel" ) },
	{ MODKEY,                       XK_x,      spawn,          {.v = ( const char*[] ){"notes", NULL} } },
	{ MODKEY,                       XK_m,      spawn,          SHCMD( TERMINAL " -e ncmpcpp; kill -45 $(pidof dwmblocks)") },
	{ MODKEY,                       XK_n,      spawn,          SHCMD( TERMINAL " -e newsboat") },
	{ MODKEY,                       XK_g,      spawn,          SHCMD( TERMINAL " -e weechat") },
	{ MODKEY,                       XK_o,      spawn,          SHCMD( "findsong") },
	{ MODKEY,                       XK_u,      spawn,          SHCMD( "passmenu") },
	{ MODKEY|ControlMask,           XK_q,      spawn,          SHCMD( "killprocess") },
	{ MODKEY|ControlMask,           XK_s,      spawn,          SHCMD( "switchoutput") },
	{ MODKEY|ControlMask,           XK_o,      spawn,          SHCMD( "xsecurelock") },
	{ MODKEY|ControlMask,           XK_w,      spawn,          SHCMD( "setrandomwallpaper") },
	{ MODKEY,                       XK_p,      spawn,          SHCMD("mpc toggle; kill -45 $(pidof dwmblocks)") },
	{ MODKEY,                       XK_c,      spawn,          SHCMD( TERMINAL " -e calcurse") },
	{ MODKEY,                       XK_v,      spawn,          {.v = ( const char*[] ){ "alacritty", "-e", "nvim", NULL } } },
	{ MODKEY,                       XK_y,      spawn,          {.v = ( const char*[] ){ "alacritty", "-e", "neomutt", NULL } } },
	{ MODKEY,                       XK_BackSpace,      spawn,          {.v = ( const char*[] ){ "dwmactions", NULL } } },
	{ 0,                            XF86XK_MonBrightnessUp,    spawn,          SHCMD( "xbacklight +2") },
	{ 0,                            XF86XK_MonBrightnessDown,  spawn,          SHCMD( "xbacklight -2") },
	{ 0,                            XF86XK_AudioRaiseVolume,   spawn,          SHCMD("pactl set-sink-volume @DEFAULT_SINK@ +5%; kill -44 $(pidof dwmblocks)") },
	{ 0,                            XF86XK_AudioLowerVolume,   spawn,          SHCMD("pactl set-sink-volume @DEFAULT_SINK@ -5%; kill -44 $(pidof dwmblocks)") },

	{ MODKEY,                       XK_b,      togglebar,      {0} },
	{ MODKEY,                       XK_j,      focusstack,     {.i = +1 } },
	{ MODKEY,                       XK_k,      focusstack,     {.i = -1 } },
	{ MODKEY,                       XK_i,      incnmaster,     {.i = +1 } },
	{ MODKEY,                       XK_d,      incnmaster,     {.i = -1 } },
	{ MODKEY,                       XK_Left,      setmfact,       {.f = -0.05} },
	{ MODKEY,                       XK_Right,      setmfact,       {.f = +0.05} },
	//{ MODKEY|ControlMask,           XK_h,      incrgaps,       {.i = +1 } },
	//{ MODKEY|ControlMask,           XK_l,      incrgaps,       {.i = -1 } },
	{ MODKEY|ControlMask,  	        XK_h,      incrogaps,      {.i = +1 } },
	{ MODKEY|ControlMask,           XK_l,      incrogaps,      {.i = -1 } },
	{ MODKEY|ShiftMask|ControlMask,  XK_h,      incrigaps,      {.i = +1 } },
	{ MODKEY|ShiftMask|ControlMask,  XK_l,      incrigaps,      {.i = -1 } },
	{ MODKEY|ShiftMask,             XK_a,      togglegaps,     {0} },
	{ MODKEY|Mod4Mask|ShiftMask,    XK_0,      defaultgaps,    {0} },
	{ MODKEY,                       XK_y,      incrihgaps,     {.i = +1 } },
	{ MODKEY,                       XK_o,      incrihgaps,     {.i = -1 } },
	{ MODKEY|ControlMask,           XK_y,      incrivgaps,     {.i = +1 } },
	{ MODKEY|ControlMask,           XK_o,      incrivgaps,     {.i = -1 } },
	{ MODKEY|Mod4Mask,              XK_y,      incrohgaps,     {.i = +1 } },
	{ MODKEY|Mod4Mask,              XK_o,      incrohgaps,     {.i = -1 } },
	{ MODKEY|ShiftMask,             XK_y,      incrovgaps,     {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_o,      incrovgaps,     {.i = -1 } },
	{ MODKEY|ShiftMask,             XK_Return, zoom,           {0} },
	{ MODKEY,                       XK_Tab,    view,           {0} },
	{ MODKEY,                       XK_q,      killclient,     {0} },
	{ MODKEY,                       XK_t,      setlayout,      {.v = &layouts[0]} },
	//{ MODKEY,                       XK_w,      setlayout,      {.v = &layouts[1]} },
	{ MODKEY|ControlMask,           XK_m,      setlayout,      {.v = &layouts[2]} },
	{ MODKEY|ControlMask,           XK_f,      setlayout,      {.v = &layouts[3]} },
	{ MODKEY|ShiftMask,             XK_f,      setlayout,      {.v = &layouts[4]} },
	{ MODKEY,                       XK_space,  setlayout,      {0} },
	{ MODKEY,                       XK_w,  togglefloating, {0} },
	{ MODKEY,                       XK_0,      view,           {.ui = ~0 } },
	{ MODKEY|ShiftMask,             XK_0,      tag,            {.ui = ~0 } },
	{ MODKEY,                       XK_comma,  focusmon,       {.i = -1 } },
	{ MODKEY,                       XK_period, focusmon,       {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_comma,  tagmon,         {.i = -1 } },
	{ MODKEY|ShiftMask,             XK_period, tagmon,         {.i = +1 } },
	{ MODKEY|ShiftMask,             XK_n,      setscheme,      {.i = +1 } },
	{ MODKEY,                       XK_l,      viewnext,       {0} },
	{ MODKEY,                       XK_h,      viewprev,       {0} },
	{ MODKEY|ShiftMask,             XK_l,  tagtonext,      {0} },
	{ MODKEY|ShiftMask,             XK_h,   tagtoprev,      {0} },
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

