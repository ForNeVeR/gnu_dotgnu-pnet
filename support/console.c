/*
 * console.c - Console I/O routines.
 *
 * Copyright (C) 2003  Southern Storm Software, Pty Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#include "il_console.h"
#include "il_system.h"
#include <stdio.h>
#ifndef IL_WIN32_PLATFORM
#if TIME_WITH_SYS_TIME
	#include <sys/time.h>
    #include <time.h>
#else
    #if HAVE_SYS_TIME_H
		#include <sys/time.h>
    #else
        #include <time.h>
    #endif
#endif
#ifdef HAVE_SYS_TYPES_H
	#include <sys/types.h>
#endif
#ifdef HAVE_SYS_SELECT_H
	#include <sys/select.h>
#endif
#ifdef HAVE_UNISTD_H
	#include <unistd.h>
#endif
#include <errno.h>
#include <signal.h>
#ifdef HAVE_TERMCAP_H
	#include <termcap.h>
#elif defined(HAVE_TERM_H)
	#include <term.h>
#endif
#ifdef HAVE_SYS_IOCTL_H
	#include <sys/ioctl.h>
#endif
#ifdef HAVE_TERMIOS_H
	#include <termios.h>
	#ifdef HAVE_TCGETATTR
		#define	USE_TTY_OPS	1
		#define	USE_TERMIOS	1
	#endif
#elif defined(HAVE_SYS_TERMIOS_H)
	#include <sys/termios.h>
	#ifdef HAVE_TCGETATTR
		#define	USE_TTY_OPS	1
		#define	USE_TERMIOS	1
	#endif
#endif
#endif

#ifdef	__cplusplus
extern	"C" {
#endif

/*
 * Determine which version of the console routines to use.
 */
#ifdef IL_WIN32_PLATFORM
	#define	IL_CONSOLE_WIN32	1
#else
#if defined(USE_TTY_OPS) && \
		(defined(HAVE_TERMCAP_H) || defined(HAVE_TERM_H)) && \
		defined(HAVE_TGOTO)
	#define	IL_CONSOLE_TERMCAP	1
#endif
#endif

/*
 * Key codes - keep in sync with "System.ConsoleKey".
 */
typedef enum
{
	Key_Back				= 0x08,
	Key_Tab					= 0x09,
	Key_Clear				= 0x0C,
	Key_Enter				= 0x0D,
	Key_Pause				= 0x13,
	Key_Escape				= 0x1B,
	Key_SpaceBar			= 0x20,
	Key_PageUp				= 0x21,
	Key_PageDown			= 0x22,
	Key_End					= 0x23,
	Key_Home				= 0x24,
	Key_LeftArrow			= 0x25,
	Key_UpArrow				= 0x26,
	Key_RightArrow			= 0x27,
	Key_DownArrow			= 0x28,
	Key_Select				= 0x29,
	Key_Print				= 0x2A,
	Key_Execute				= 0x2B,
	Key_PrintScreen			= 0x2C,
	Key_Insert				= 0x2D,
	Key_Delete				= 0x2E,
	Key_Help				= 0x2F,
	Key_D0					= 0x30,
	Key_D1					= 0x31,
	Key_D2					= 0x32,
	Key_D3					= 0x33,
	Key_D4					= 0x34,
	Key_D5					= 0x35,
	Key_D6					= 0x36,
	Key_D7					= 0x37,
	Key_D8					= 0x38,
	Key_D9					= 0x39,
	Key_A					= 0x41,
	Key_B					= 0x42,
	Key_C					= 0x43,
	Key_D					= 0x44,
	Key_E					= 0x45,
	Key_F					= 0x46,
	Key_G					= 0x47,
	Key_H					= 0x48,
	Key_I					= 0x49,
	Key_J					= 0x4A,
	Key_K					= 0x4B,
	Key_L					= 0x4C,
	Key_M					= 0x4D,
	Key_N					= 0x4E,
	Key_O					= 0x4F,
	Key_P					= 0x50,
	Key_Q					= 0x51,
	Key_R					= 0x52,
	Key_S					= 0x53,
	Key_T					= 0x54,
	Key_U					= 0x55,
	Key_V					= 0x56,
	Key_W					= 0x57,
	Key_X					= 0x58,
	Key_Y					= 0x59,
	Key_Z					= 0x5A,
	Key_LeftWindows			= 0x5B,
	Key_RightWindows		= 0x5C,
	Key_Applications		= 0x5D,
	Key_Sleep				= 0x5F,
	Key_NumPad0				= 0x60,
	Key_NumPad1				= 0x61,
	Key_NumPad2				= 0x62,
	Key_NumPad3				= 0x63,
	Key_NumPad4				= 0x64,
	Key_NumPad5				= 0x65,
	Key_NumPad6				= 0x66,
	Key_NumPad7				= 0x67,
	Key_NumPad8				= 0x68,
	Key_NumPad9				= 0x69,
	Key_Multiply			= 0x6A,
	Key_Add					= 0x6B,
	Key_Separator			= 0x6C,
	Key_Subtract			= 0x6D,
	Key_Decimal				= 0x6E,
	Key_Divide				= 0x6F,
	Key_F1					= 0x70,
	Key_F2					= 0x71,
	Key_F3					= 0x72,
	Key_F4					= 0x73,
	Key_F5					= 0x74,
	Key_F6					= 0x75,
	Key_F7					= 0x76,
	Key_F8					= 0x77,
	Key_F9					= 0x78,
	Key_F10					= 0x79,
	Key_F11					= 0x7A,
	Key_F12					= 0x7B,
	Key_F13					= 0x7C,
	Key_F14					= 0x7D,
	Key_F15					= 0x7E,
	Key_F16					= 0x7F,
	Key_F17					= 0x80,
	Key_F18					= 0x81,
	Key_F19					= 0x82,
	Key_F20					= 0x83,
	Key_F21					= 0x84,
	Key_F22					= 0x85,
	Key_F23					= 0x86,
	Key_F24					= 0x87,
	Key_BrowserBack			= 0xA6,
	Key_BrowserForward		= 0xA7,
	Key_BrowserRefresh		= 0xA8,
	Key_BrowserStop			= 0xA9,
	Key_BrowserSearch		= 0xAA,
	Key_BrowserFavorites	= 0xAB,
	Key_BrowserHome			= 0xAC,
	Key_VolumeMute			= 0xAD,
	Key_VolumeDown			= 0xAE,
	Key_VolumeUp			= 0xAF,
	Key_MediaNext			= 0xB0,
	Key_MediaPrevious		= 0xB1,
	Key_MediaStop			= 0xB2,
	Key_MediaPlay			= 0xB3,
	Key_LaunchMail			= 0xB4,
	Key_LaunchMediaSelect	= 0xB5,
	Key_LaunchApp1			= 0xB6,
	Key_LaunchApp2			= 0xB7,
	Key_Oem1				= 0xBA,
	Key_OemPlus				= 0xBB,
	Key_OemComma			= 0xBC,
	Key_OemMinus			= 0xBD,
	Key_OemPeriod			= 0xBE,
	Key_Oem2				= 0xBF,
	Key_Oem3				= 0xC0,
	Key_Oem4				= 0xDB,
	Key_Oem5				= 0xDC,
	Key_Oem6				= 0xDD,
	Key_Oem7				= 0xDE,
	Key_Oem8				= 0xDF,
	Key_Oem102				= 0xE2,
	Key_Process				= 0xE5,
	Key_Packet				= 0xE7,
	Key_Attention			= 0xF6,
	Key_CrSel				= 0xF7,
	Key_ExSel				= 0xF8,
	Key_EraseEndOfFile		= 0xF9,
	Key_Play				= 0xFA,
	Key_Zoom				= 0xFB,
	Key_NoName				= 0xFC,
	Key_Pa1					= 0xFD,
	Key_OemClear			= 0xFE,

	/* Special key codes that are specific to our console implementation
	   to propagate certain signals up to the application program */
	Key_SizeChanged			= 0x1200,	/* Window size changed */
	Key_Resumed				= 0x1201,	/* Resumed after CTRL-Z suspend */
	Key_Interrupt			= 0x1202,	/* CTRL-C interrupt detected */
	Key_CtrlBreak			= 0x1203,	/* CTRL-BREAK interrupt detected */

} ConsoleKey;

/*
 * Modifier masks - keep in sync with "System.ConsoleModifiers".
 */
typedef enum
{
	Mod_Alt		= 0x0001,
	Mod_Shift	= 0x0002,
	Mod_Control	= 0x0004

} ConsoleModifiers;

#if defined(IL_CONSOLE_TERMCAP)

/*
 * Forward declaration.
 */
static void MapCharToKey(int ch, ILInt32 *key, ILInt32 *modifiers);
#define	IL_NEED_MAPCHAR		1
#define	IL_CHECK_ERASE		1

/*
 * Capability names for various special keys.
 */
typedef struct
{
	const char		   *name;
	ConsoleKey			key;
	ConsoleModifiers	modifiers;

} SpecialKeyCap;
static SpecialKeyCap const SpecialKeys[] = {
	{"#1",			Key_Help,			Mod_Shift},
	{"#2",			Key_Home,			Mod_Shift},
	{"#4",			Key_LeftArrow,		Mod_Shift},
	{"%1",			Key_Help,			0},
	{"%9",			Key_Print,			0},
	{"&9",			Key_Home,			Mod_Shift},
	{"*7",			Key_End,			Mod_Shift},
	{"@1",			Key_Home,			0},
	{"@7",			Key_End,			0},
	{"k0",			Key_F10,			0},
	{"k1",			Key_F1,				0},
	{"k2",			Key_F2,				0},
	{"k3",			Key_F3,				0},
	{"k4",			Key_F4,				0},
	{"k5",			Key_F5,				0},
	{"k6",			Key_F6,				0},
	{"k7",			Key_F7,				0},
	{"k8",			Key_F8,				0},
	{"k9",			Key_F9,				0},
	{"k;",			Key_F10,			0},
	{"F1",			Key_F11,			0},
	{"F2",			Key_F12,			0},
	{"F3",			Key_F13,			0},
	{"F4",			Key_F14,			0},
	{"F5",			Key_F15,			0},
	{"F6",			Key_F16,			0},
	{"F7",			Key_F17,			0},
	{"F8",			Key_F18,			0},
	{"F9",			Key_F19,			0},
	{"FA",			Key_F20,			0},
	{"FB",			Key_F21,			0},
	{"FC",			Key_F22,			0},
	{"FD",			Key_F23,			0},
	{"FE",			Key_F24,			0},
	{"K1",			Key_Home,			0},
	{"K3",			Key_PageUp,			0},
	{"K4",			Key_End,			0},
	{"K5",			Key_PageDown,		0},
	{"kb",			Key_Back,			0},
	{"kB",			Key_Tab,			Mod_Shift},
	{"kd",			Key_DownArrow,		0},
	{"kD",			Key_Delete,			0},
	{"kF",			Key_PageDown,		0},
	{"kh",			Key_Home,			0},
	{"kH",			Key_PageDown,		Mod_Control},
	{"kI",			Key_Insert,			0},
	{"kl",			Key_LeftArrow,		0},
	{"kN",			Key_PageDown,		0},
	{"kP",			Key_PageUp,			0},
	{"kr",			Key_RightArrow,		0},
	{"kR",			Key_PageUp,			0},
	{"ku",			Key_UpArrow,		0},

	/* Known VT* special key codes that are sometimes not listed in termcap but
	   can be generated if the terminal is in an odd ANSI/VT compat mode */
	{"\033[A",		Key_UpArrow,		0},
	{"\033[B",		Key_DownArrow,		0},
	{"\033[C",		Key_RightArrow,		0},
	{"\033[D",		Key_LeftArrow,		0},
	{"\033[H",		Key_Home,			0},
	{"\033[F",		Key_End,			0},
	{"\033[P",		Key_F1,				0},
	{"\033[Q",		Key_F2,				0},
	{"\033[R",		Key_F3,				0},
	{"\033[S",		Key_F4,				0},
	{"\033[Z",		Key_Tab,			Mod_Shift},
	{"\033OA",		Key_UpArrow,		0},
	{"\033OB",		Key_DownArrow,		0},
	{"\033OC",		Key_RightArrow,		0},
	{"\033OD",		Key_LeftArrow,		0},
	{"\033OH",		Key_Home,			0},
	{"\033OF",		Key_End,			0},
	{"\033OP",		Key_F1,				0},
	{"\033OQ",		Key_F2,				0},
	{"\033OR",		Key_F3,				0},
	{"\033OS",		Key_F4,				0},
	{"\033OZ",		Key_Tab,			Mod_Shift},
	{"\033A",		Key_UpArrow,		0},
	{"\033B",		Key_DownArrow,		0},
	{"\033C",		Key_RightArrow,		0},
	{"\033D",		Key_LeftArrow,		0},
	{"\033H",		Key_Home,			0},
	{"\033F",		Key_End,			0},
	{"\033P",		Key_F1,				0},
	{"\033Q",		Key_F2,				0},
	{"\033R",		Key_F3,				0},
	{"\033S",		Key_F4,				0},
	{"\033Z",		Key_Tab,			Mod_Shift},

	/* Other common keycodes, typical to the "linux" and "xterm" entries */
	{"\033[[A",		Key_F1,				0},
	{"\033[[B",		Key_F2,				0},
	{"\033[[C",		Key_F3,				0},
	{"\033[[D",		Key_F4,				0},
	{"\033[[E",		Key_F5,				0},
	{"\033[15~",	Key_F5,				0},
	{"\033[17~",	Key_F6,				0},
	{"\033[18~",	Key_F7,				0},
	{"\033[19~",	Key_F8,				0},
	{"\033[20~",	Key_F9,				0},
	{"\033[21~",	Key_F10,			0},
	{"\033[23~",	Key_F11,			0},
	{"\033[24~",	Key_F12,			0},
	{"\033[25~",	Key_F13,			0},
	{"\033[26~",	Key_F14,			0},
	{"\033[28~",	Key_F15,			0},
	{"\033[29~",	Key_F16,			0},
	{"\033[31~",	Key_F17,			0},
	{"\033[32~",	Key_F18,			0},
	{"\033[33~",	Key_F19,			0},
	{"\033[34~",	Key_F20,			0},
	{"\033[[2A",	Key_F1,				0},
	{"\033[[2B",	Key_F2,				0},
	{"\033[[2C",	Key_F3,				0},
	{"\033[[2D",	Key_F4,				0},
	{"\033[[2E",	Key_F5,				0},
	{"\033[2P",		Key_F1,				Mod_Shift},
	{"\033[2Q",		Key_F2,				Mod_Shift},
	{"\033[2R",		Key_F3,				Mod_Shift},
	{"\033[2S",		Key_F4,				Mod_Shift},
	{"\033[15;2~",	Key_F5,				Mod_Shift},
	{"\033[17;2~",	Key_F6,				Mod_Shift},
	{"\033[18;2~",	Key_F7,				Mod_Shift},
	{"\033[19;2~",	Key_F8,				Mod_Shift},
	{"\033[20;2~",	Key_F9,				Mod_Shift},
	{"\033[21;2~",	Key_F10,			Mod_Shift},
	{"\033[23;2~",	Key_F11,			Mod_Shift},
	{"\033[24;2~",	Key_F12,			Mod_Shift},
	{"\033[25;2~",	Key_F13,			0},
	{"\033[26;2~",	Key_F14,			0},
	{"\033[28;2~",	Key_F15,			0},
	{"\033[29;2~",	Key_F16,			0},
	{"\033[31;2~",	Key_F17,			0},
	{"\033[32;2~",	Key_F18,			0},
	{"\033[33;2~",	Key_F19,			0},
	{"\033[34;2~",	Key_F20,			0},
	{"\033O2P",		Key_F1,				Mod_Shift},
	{"\033O2Q",		Key_F2,				Mod_Shift},
	{"\033O2R",		Key_F3,				Mod_Shift},
	{"\033O2S",		Key_F4,				Mod_Shift},
	{"\033[1~",		Key_Home,			0},
	{"\033[2~",		Key_Insert,			0},
	{"\033[3~",		Key_Delete,			0},
	{"\033[4~",		Key_End,			0},
	{"\033[5~",		Key_PageUp,			0},
	{"\033[6~",		Key_PageDown,		0},
	{"\033[7~",		Key_Home,			0},
	{"\033[8~",		Key_End,			0},
	{"\033[2H",		Key_Home,			Mod_Shift},
	{"\033[2F",		Key_End,			Mod_Shift},
	{"\033O2H",		Key_Home,			Mod_Shift},
	{"\033O2F",		Key_End,			Mod_Shift},
	{"\033[1;2~",	Key_Home,			Mod_Shift},
	{"\033[2;2~",	Key_Insert,			Mod_Shift},
	{"\033[3;2~",	Key_Delete,			Mod_Shift},
	{"\033[4;2~",	Key_End,			Mod_Shift},
	{"\033[5;2~",	Key_PageUp,			Mod_Shift},
	{"\033[6;2~",	Key_PageDown,		Mod_Shift},
	{"\033[7;2~",	Key_Home,			Mod_Shift},
	{"\033[8;2~",	Key_End,			Mod_Shift},
};
#define	NumSpecialKeys	(sizeof(SpecialKeys) / sizeof(SpecialKeyCap))
static char *SpecialKeyStrings[NumSpecialKeys];

/*
 * Global state for the termcap-based console driver.
 */
static int consoleMode = IL_CONSOLE_NORMAL;
static int termcapInitialized = 0;
static char *termcapBuffer = 0;
static char *termcapBuffer2 = 0;
static int consoleIsTty = 0;
static int screenX = 0;
static int screenY = 0;
static int screenWidth = 80;
static int screenHeight = 24;
static int terminalHasColor = 0;
static int terminalHasTitle = 0;
static int eraseIsDel = 0;
static int volatile windowSizeChanged = 0;
static int volatile processResumed = 0;
static int volatile interruptSeen = 0;
#define	KEY_BUFFER_SIZE		32
static char keyBuffer[KEY_BUFFER_SIZE];
static int keyBufferLen = 0;
static char *enterAltMode = 0;
static char *leaveAltMode = 0;
#ifdef USE_TERMIOS
static struct termios savedNormal;
static struct termios currentAttrs;
#endif

/*
 * Output a character to the console.
 */
static int ConsolePutchar(int ch)
{
	putc(ch, stdout);
	return 0;
}

/*
 * Output a particular string capability.  Returns zero if no such capability.
 */
static int OutputStringCap(char *name)
{
	char *area = termcapBuffer2;
	char *str = tgetstr(name, &area);
	if(str)
	{
		tputs(str, 1, ConsolePutchar);
		fflush(stdout);
		return 1;
	}
	else
	{
		return 0;
	}
}

/*
 * Initialize the termcap-based console driver.
 * Returns zero if stdin/stdout are not tty's.
 */
static int InitTermcapDriver(void)
{
	char *term;
	char *area;
	char *str;
	int posn;
	if(termcapInitialized)
	{
		return consoleIsTty;
	}
	term = getenv("TERM");
	if(!isatty(0) || !isatty(1) || !term || *term == '\0')
	{
		/* Either stdin or stdout is not a tty, so cannot use console mode */
		consoleIsTty = 0;
	}
	else if((termcapBuffer = (char *)ILMalloc(8192)) == 0 ||
	        (termcapBuffer2 = (char *)ILMalloc(8192)) == 0)
	{
		/* Insufficient memory to do termcap processing */
		consoleIsTty = 0;
	}
	else if(tgetent(termcapBuffer, term) <= 0)
	{
		/* Could not load the termcap entry for the terminal */
		consoleIsTty = 0;
	}
	else
	{
		/* Prime the screen size information from the termcap details */
		screenWidth = tgetnum("co");
		if(screenWidth <= 0)
		{
			screenWidth = 80;
		}
		screenHeight = tgetnum("li");
		if(screenHeight <= 0)
		{
			screenHeight = 24;
		}

		/* Query the terminal itself as to what the size is */
	#ifdef TIOCGWINSZ
		{
			struct winsize size;
			size.ws_row = 0;
			size.ws_col = 0;
			if(ioctl(0, TIOCGWINSZ, &size) >= 0)
			{
				if(size.ws_row > 0)
				{
					screenHeight = size.ws_row;
				}
				if(size.ws_col > 0)
				{
					screenWidth = size.ws_col;
				}
			}
		}
	#endif

		/* There's no way to get the current cursor position, so we just
		   assume that the position is the start of the bottom-most line */
		screenX = 0;
		screenY = screenHeight - 1;

		/* Determine if the terminal supports the xterm title sequence */
		if(!strcmp(term, "xterm") ||
		   !strncmp(term, "xterm-", 6) ||
		   !strcmp(term, "color-xterm") ||
		   !strcmp(term, "color_xterm") ||
		   !strcmp(term, "rxvt"))
		{
			terminalHasTitle = 1;
		}

		/* Determine if the terminal has ANSI color sequences */
		area = termcapBuffer2;
		if(tgetstr("AB", &area))
		{
			terminalHasColor = 1;
		}
		else if(terminalHasTitle)
		{
			/* All of the xterm variants support color */
			terminalHasColor = 1;
		}
		else if(!strcmp(term, "linux"))
		{
			terminalHasColor = 1;
		}

		/* Populate the list of special keys */
		for(posn = 0; posn < NumSpecialKeys; ++posn)
		{
			if(SpecialKeys[posn].name[0] == '\033')
			{
				/* This is an explicit key code for things that are
				   usually keycodes but not the ones listed in termcap */
				SpecialKeyStrings[posn] = ILDupString(SpecialKeys[posn].name);
			}
			else
			{
				area = termcapBuffer2;
				str = tgetstr(SpecialKeys[posn].name, &area);
				if(str && *str != '\0')
				{
					SpecialKeyStrings[posn] = ILDupString(str);
				}
				else
				{
					SpecialKeyStrings[posn] = 0;
				}
			}
		}

		/* Get the strings to enter or leave "alternative" mode */
		enterAltMode = 0;
		leaveAltMode = 0;
		if(consoleMode == IL_CONSOLE_CBREAK_ALT ||
		   consoleMode == IL_CONSOLE_RAW_ALT)
		{
			area = termcapBuffer2;
			str = tgetstr("ti", &area);
			if(str && *str != '\0')
			{
				enterAltMode = ILDupString(str);
			}
			area = termcapBuffer2;
			str = tgetstr("te", &area);
			if(str && *str != '\0')
			{
				leaveAltMode = ILDupString(str);
			}
			if(enterAltMode)
			{
				/* Write out the "enter alternative mode" string */
				tputs(enterAltMode, 1, ConsolePutchar);
				fflush(stdout);
			}
		}

		/* The console is a known tty */
		consoleIsTty = 1;
	}
	termcapInitialized = 1;
	return consoleIsTty;
}

/*
 * Refresh the window size from the kernel after a size change.
 */
static void RefreshWindowSize(void)
{
#ifdef TIOCGWINSZ
	struct winsize size;
	size.ws_row = 0;
	size.ws_col = 0;
	if(ioctl(0, TIOCGWINSZ, &size) >= 0)
	{
		if(size.ws_row > 0)
		{
			screenHeight = size.ws_row;
		}
		if(size.ws_col > 0)
		{
			screenWidth = size.ws_col;
		}
	}
#endif
}

#ifdef SIGINT

/*
 * Notice the signal that indicates a CTRL-C interrupt so that it
 * can be inserted into the character-at-a-time input buffer.
 * This is only used in cbreak mode, never normal or raw.
 */
static void InterruptProcess(int sig)
{
	interruptSeen = 1;
	signal(SIGINT, InterruptProcess);
}

#endif

#ifdef SIGWINCH

/*
 * Notice the signal that indicates a change in window size.
 */
static void WindowSizeChanged(int sig)
{
	windowSizeChanged = 1;
	signal(SIGWINCH, WindowSizeChanged);
}

#endif

#if defined(SIGTSTP) && defined(SIGCONT)

/*
 * Notice the signal that indicates that the process should suspend.
 */
static void StopProcess(int sig)
{
	/* Leave "alternative" mode */
	if(leaveAltMode)
	{
		write(1, leaveAltMode, strlen(leaveAltMode));
	}

#ifdef USE_TERMIOS
	/* Restore the default tty attributes */
	tcsetattr(0, TCSANOW, &savedNormal);
#endif

	/* Re-install the signal handler for next time */
	signal(SIGTSTP, StopProcess);

	/* Suspend the process for real */
	kill(getpid(), SIGSTOP);
}

/*
 * Notice the signal that indicates that the process has resumed.
 */
static void ContinueProcess(int sig)
{
#ifdef USE_TERMIOS
	/* Restore the tty settings that we require */
	tcsetattr(0, TCSANOW, &currentAttrs);
#endif

	/* Re-enter "alternative" mode */
	if(enterAltMode)
	{
		write(1, enterAltMode, strlen(enterAltMode));
	}

	/* Record that the process has resumed */
	processResumed = 1;

	/* Re-install the signal handler for next time */
	signal(SIGCONT, ContinueProcess);
}

#endif

void ILConsoleSetMode(ILInt32 mode)
{
	char *term;

	/* Bail out if no mode change required */
	if(mode == consoleMode)
	{
		return;
	}

	/* Bail out if the console is not a tty */
	term = getenv("TERM");
	if(!isatty(0) || !isatty(1) || !term || *term == '\0')
	{
		return;
	}

	/* Save the original tty attributes if changing from "normal" */
	if(consoleMode == IL_CONSOLE_NORMAL)
	{
#ifdef USE_TERMIOS
		tcgetattr(0, &savedNormal);
		eraseIsDel = (savedNormal.c_cc[VERASE] == 0x7F);
#endif
	}

	/* Create the desired attributes */
	if(mode == IL_CONSOLE_NORMAL)
	{
#ifdef USE_TERMIOS
		currentAttrs = savedNormal;
#endif
	}
	else if(mode == IL_CONSOLE_CBREAK || mode == IL_CONSOLE_CBREAK_ALT)
	{
#ifdef USE_TERMIOS
		currentAttrs = savedNormal;
		currentAttrs.c_iflag &= ~(INLCR | IGNCR | ICRNL);
		currentAttrs.c_oflag &= ~(ONLCR | OCRNL | ONOCR | ONLRET);
		currentAttrs.c_lflag &= ~(ICANON | ECHO);
		currentAttrs.c_lflag |= ISIG;
#endif
	}
	else
	{
#ifdef USE_TERMIOS
		currentAttrs = savedNormal;
		currentAttrs.c_iflag &= ~(INLCR | IGNCR | ICRNL);
		currentAttrs.c_oflag &= ~(ONLCR | OCRNL | ONOCR | ONLRET);
		currentAttrs.c_lflag &= ~(ICANON | ECHO | ISIG);
#endif
	}

	/* Set the attributes */
#ifdef USE_TERMIOS
	tcsetattr(0, TCSANOW, &currentAttrs);
#endif

	/* Leave "alternative" mode if we were previously in it */
	if(leaveAltMode)
	{
		write(1, leaveAltMode, strlen(leaveAltMode));
	}
	enterAltMode = 0;
	leaveAltMode = 0;

	/* Trap interesting signals */
#ifdef SIGINT
	if(mode == IL_CONSOLE_CBREAK || mode == IL_CONSOLE_CBREAK_ALT)
	{
		signal(SIGINT, InterruptProcess);
	}
	else
	{
		signal(SIGINT, SIG_DFL);
	}
#endif
#ifdef SIGWINCH
	signal(SIGWINCH, WindowSizeChanged);
#endif
#if defined(SIGTSTP) && defined(SIGCONT)
	signal(SIGTSTP, StopProcess);
	signal(SIGCONT, ContinueProcess);
#endif

	/* We are now in the new console mode */
	consoleMode = mode;
}

ILInt32 ILConsoleGetMode(void)
{
	return consoleMode;
}

void ILConsoleBeep(void)
{
	if(InitTermcapDriver())
	{
		OutputStringCap("bl");
	}
}

void ILConsoleClear(void)
{
	if(InitTermcapDriver())
	{
		if(!OutputStringCap("cl"))
		{
			OutputStringCap("ho");
			OutputStringCap("cd");
		}
	}
}

int ILConsoleKeyAvailable(void)
{
	fd_set readSet;
	struct timeval tv;
	int result;
	for(;;)
	{
		if(windowSizeChanged || processResumed || interruptSeen)
		{
			/* There is a pending signal to turn into a key indication */
			return 1;
		}
		if(keyBufferLen > 0)
		{
			/* There are left-over lookahead keys in the buffer */
			return 1;
		}
		FD_ZERO(&readSet);
		FD_SET(0, &readSet);
		tv.tv_sec = 0;
		tv.tv_usec = 0;
		result = select(1, &readSet, (fd_set *)0, (fd_set *)0, &tv);
		if(result > 0)
		{
			/* There is something waiting in the stdin input buffer */
			return 1;
		}
		else if(result < 0 && errno == EINTR)
		{
			/* The system call was interrupted, so retry it */
			continue;
		}
		else
		{
			/* Timeout occurred, so no key available */
			break;
		}
	}
	return 0;
}

void ILConsoleReadKey(ILUInt16 *ch, ILInt32 *key, ILInt32 *modifiers)
{
	fd_set readSet;
	struct timeval tv;
	struct timeval *tvp;
	int result, c, posn;
	int len, prefixMatch;
	for(;;)
	{
		/* Turn pending signals into special key codes */
		if(processResumed)
		{
			processResumed = 0;
			*ch = 0;
			*key = Key_Resumed;
			*modifiers = 0;
			return;
		}
		if(windowSizeChanged)
		{
			RefreshWindowSize();
			windowSizeChanged = 0;
			*ch = 0;
			*key = Key_SizeChanged;
			*modifiers = 0;
			return;
		}
		if(interruptSeen)
		{
			interruptSeen = 0;
			*ch = 0;
			*key = Key_Interrupt;
			*modifiers = 0;
			return;
		}

		/* Wait for an incoming character */
		FD_ZERO(&readSet);
		FD_SET(0, &readSet);
		if(keyBufferLen > 0)
		{
			tv.tv_sec = 0;
			tv.tv_usec = 300000;
			tvp = &tv;
		}
		else
		{
			tvp = (struct timeval *)0;
		}
		result = select(1, &readSet, (fd_set *)0, (fd_set *)0, tvp);
		if(result > 0)
		{
			/* There is something waiting in the stdin input buffer */
			if(read(0, keyBuffer + keyBufferLen, 1) == 1)
			{
				++keyBufferLen;
				prefixMatch = 0;
				for(posn = 0; posn < NumSpecialKeys; ++posn)
				{
					if(SpecialKeyStrings[posn] &&
					   SpecialKeyStrings[posn][0] == keyBuffer[0])
					{
						/* We have a potential prefix match */
						len = strlen(SpecialKeyStrings[posn]);
						if(len <= keyBufferLen &&
						   !ILMemCmp(keyBuffer, SpecialKeyStrings[posn], len))
						{
							/* Found a special key */
							if(keyBufferLen > len)
							{
								ILMemMove(keyBuffer, keyBuffer + len,
										  keyBufferLen - len);
							}
							keyBufferLen -= len;
							*ch = 0;
							*key = SpecialKeys[posn].key;
							*modifiers = SpecialKeys[posn].modifiers;
							return;
						}
						else if(len > keyBufferLen &&
						        !ILMemCmp(keyBuffer,
										  SpecialKeyStrings[posn],
										  keyBufferLen))
						{
							/* The buffer contains an entire key prefix */
							prefixMatch = 1;
						}
					}
				}
				if(!prefixMatch || keyBufferLen >= KEY_BUFFER_SIZE)
				{
					/* No prefix match, so we have a normal character */
					goto normalChar;
				}
			}
		}
		else if(result < 0 && errno == EINTR)
		{
			/* The system call was interrupted by a signal, so retry */
			continue;
		}
		else if(result < 0)
		{
			/* Some kind of error occurred */
			break;
		}
		else if(keyBufferLen > 0)
		{
			/* We got a timeout while processing lookahead characters.
			   Extract the first character from the buffer and return it.
			   It will normally be an Escape character, indicating the
			   actual Escape key rather than the start of a special key */
		normalChar:
			c = (((int)(keyBuffer[0])) & 0xFF);
			--keyBufferLen;
			if(keyBufferLen > 0)
			{
				ILMemMove(keyBuffer, keyBuffer + 1, keyBufferLen);
			}
			*ch = (ILUInt16)c;
			MapCharToKey(c, key, modifiers);
			return;
		}
	}
	*ch = 0;
	*key = 0;
	*modifiers = 0;
}

void ILConsoleSetPosition(ILInt32 x, ILInt32 y)
{
	if(InitTermcapDriver())
	{
		char *area = termcapBuffer2;
		char *str = tgetstr("cm", &area);
		if(!str)
		{
			return;
		}
		if(x < 0)
		{
			x = 0;
		}
		else if(x >= screenWidth)
		{
			x = screenWidth - 1;
		}
		if(y < 0)
		{
			y = 0;
		}
		else if(y >= screenHeight)
		{
			y = screenHeight - 1;
		}
		str = tgoto(str, x, y);
		if(str)
		{
			tputs(str, 1, ConsolePutchar);
			fflush(stdout);
		}
		screenX = x;
		screenY = y;
	}
}

void ILConsoleGetPosition(ILInt32 *x, ILInt32 *y)
{
	*x = screenX;
	*y = screenY;
}

void ILConsoleGetBufferSize(ILInt32 *width, ILInt32 *height)
{
	if(windowSizeChanged)
	{
		RefreshWindowSize();
	}
	*width = screenWidth;
	*height = screenHeight;
}

void ILConsoleGetWindowSize(ILInt32 *left, ILInt32 *top,
							ILInt32 *width, ILInt32 *height)
{
	if(windowSizeChanged)
	{
		RefreshWindowSize();
	}
	*left = 0;
	*top = 0;
	*width = screenWidth;
	*height = screenHeight;
}

void ILConsoleSetTitle(const char *title)
{
	if(InitTermcapDriver() && terminalHasTitle)
	{
		fputs("\033]0;", stdout);
		if(title)
		{
			int ch;
			while((ch = (*title++ & 0xFF)) != '\0')
			{
				if(ch >= 0x20)
				{
					putc(ch, stdout);
				}
				else if(ch == '\t')
				{
					putc(' ', stdout);
				}
			}
		}
		putc(0x07, stdout);
		fflush(stdout);
	}
}

void ILConsoleSetAttributes(ILInt32 attrs)
{
	if(InitTermcapDriver())
	{
		if(terminalHasColor)
		{
			/* Use ANSI escapes to set the color.  If the foreground is gray,
			   then we use the default foreground color.  If the background
			   is black, then we use the default background color.  This
			   avoids problems where the user's preferred color scheme is
			   overridden by poorly written console applications */
			static char const MapAnsiColor[8] =
				{'0', '4', '2', '6', '1', '5', '3', '7'};
			fputs("\033[0", stdout);
			if((attrs & 0x0F) != 0x07)
			{
				if((attrs & 0x08) != 0)
				{
					fputs(";1", stdout);
				}
				fputs(";3", stdout);
				putc(MapAnsiColor[attrs & 0x07], stdout);
			}
			if((attrs & 0xF0) != 0x00)
			{
				fputs(";4", stdout);
				putc(MapAnsiColor[(attrs >> 4) & 0x07], stdout);
			}
			putc('m', stdout);
			fflush(stdout);
		}
		else
		{
			/* Non-color terminal: use the "bold" or "standout" capability */
			if((attrs & 0x08) != 0)
			{
				/* Enable a high intensity mode */
				if(!OutputStringCap("md"))
				{
					OutputStringCap("so");
				}
			}
			else
			{
				/* Return to the normal mode */
				if(!OutputStringCap("me"))
				{
					OutputStringCap("se");
				}
			}
		}
	}
}

void ILConsoleWriteChar(ILInt32 ch)
{
	ch &= 0xFF;
	if(InitTermcapDriver())
	{
		if(ch == 0x0A)
		{
			/* Translate newline into CRLF */
			putc(0x0D, stdout);
			putc(0x0A, stdout);
			screenX = 0;
			if(screenY < (screenHeight - 1))
			{
				++screenY;
			}
		}
		else if(ch == 0x0D)
		{
			/* Return to the start of the current line */
			putc(0x0D, stdout);
			screenX = 0;
		}
		else if(ch == 0x08)
		{
			/* Backspace and erase one character */
			if(screenX > 0)
			{
				if(!OutputStringCap("bc"))
				{
					putc(0x08, stdout);
				}
				--screenX;
			}
			else if(screenY > 0)
			{
				screenX = screenWidth - 1;
				--screenY;
				if(tgetflag("bw"))
				{
					/* The terminal will wrap to the previous line for us */
					if(!OutputStringCap("bc"))
					{
						putc(0x08, stdout);
					}
				}
				else
				{
					/* Simulate a wrap back to the previous line */
					ILConsoleSetPosition(screenX, screenY);
				}
			}
		}
		else if(ch == 0x09)
		{
			/* Move to the next TAB stop (uniformly spaced at 8 characters) */
			do
			{
				ILConsoleWriteChar(0x20);
			}
			while((screenX % 8) != 0);
		}
		else if(ch != 0x00 && ch != 0x1B)
		{
			/* Output a regular character */
			putc(ch, stdout);
			++screenX;
			if(screenX >= screenWidth)
			{
				if(!tgetflag("am"))
				{
					/* The cursor doesn't automatically wrap, so simulate */
					putc(0x0D, stdout);
					putc(0x0A, stdout);
				}
				screenX = 0;
				if(screenY < (screenHeight - 1))
				{
					++screenY;
				}
			}
		}
	}
	else
	{
		putc(ch, stdout);
	}
}

#else /* No console */

/*
 * Forward declaration.
 */
static void MapCharToKey(int ch, ILInt32 *key, ILInt32 *modifiers);
#define	IL_NEED_MAPCHAR		1

void ILConsoleSetMode(ILInt32 mode)
{
	/* Nothing to do here */
}

ILInt32 ILConsoleGetMode(void)
{
	return IL_CONSOLE_NORMAL;
}

void ILConsoleBeep(void)
{
	/* Nothing to do here */
}

void ILConsoleClear(void)
{
	/* Nothing to do here */
}

int ILConsoleKeyAvailable(void)
{
	/* Nothing to do here */
	return 0;
}

void ILConsoleReadKey(ILUInt16 *ch, ILInt32 *key, ILInt32 *modifiers)
{
	int c = (getc(stdin) & 0xFF);
	*ch = (ILUInt16)c;
	MapCharToKey(c, key, modifiers);
}

void ILConsoleSetPosition(ILInt32 x, ILInt32 y)
{
	/* Nothing to do here */
}

void ILConsoleGetPosition(ILInt32 *x, ILInt32 *y)
{
	*x = 0;
	*y = 0;
}

void ILConsoleGetBufferSize(ILInt32 *width, ILInt32 *height)
{
	*width = 80;
	*height = 24;
}

void ILConsoleGetWindowSize(ILInt32 *left, ILInt32 *top,
							ILInt32 *width, ILInt32 *height)
{
	*left = 0;
	*top = 0;
	*width = 80;
	*height = 24;
}

void ILConsoleSetTitle(const char *title)
{
	/* Nothing to do here */
}

void ILConsoleSetAttributes(ILInt32 attrs)
{
	/* Nothing to do here */
}

void ILConsoleWriteChar(ILInt32 ch)
{
	putc(ch & 0xFF, stdout);
}

#endif /* No console */

#ifdef IL_NEED_MAPCHAR

/*
 * Table that maps the 128 ASCII characters to equivalent key specifications.
 * This is based on a standard US-101 keyboard layout mapping.
 */
typedef struct
{
	ConsoleKey			key;
	ConsoleModifiers	modifiers;

} MapKeyInfo;
static MapKeyInfo const MapKeys[128] = {
	{Key_D2,		Mod_Shift | Mod_Control},	/* ^@ */
	{Key_A,			Mod_Control},
	{Key_B,			Mod_Control},
	{Key_C,			Mod_Control},
	{Key_D,			Mod_Control},
	{Key_E,			Mod_Control},
	{Key_F,			Mod_Control},
	{Key_G,			Mod_Control},
	{Key_Back,		0},
	{Key_Tab,		0},
	{Key_J,			Mod_Control},
	{Key_K,			Mod_Control},
	{Key_L,			Mod_Control},
	{Key_Enter,		0},
	{Key_N,			Mod_Control},
	{Key_O,			Mod_Control},
	{Key_P,			Mod_Control},
	{Key_Q,			Mod_Control},
	{Key_R,			Mod_Control},
	{Key_S,			Mod_Control},
	{Key_T,			Mod_Control},
	{Key_U,			Mod_Control},
	{Key_V,			Mod_Control},
	{Key_W,			Mod_Control},
	{Key_X,			Mod_Control},
	{Key_Y,			Mod_Control},
	{Key_Z,			Mod_Control},
	{Key_Escape,	0},
	{Key_Oem5,		Mod_Control},				/* ^\ */
	{Key_Oem6,		Mod_Control},				/* ^] */
	{Key_D6,		Mod_Shift | Mod_Control},	/* ^^ */
	{Key_OemMinus,	Mod_Shift | Mod_Control},	/* ^_ */
	{Key_SpaceBar,	0},
	{Key_D1,		Mod_Shift},					/* ! */
	{Key_Oem7,		Mod_Shift},					/* " */
	{Key_D3,		Mod_Shift},					/* # */
	{Key_D4,		Mod_Shift},					/* $ */
	{Key_D5,		Mod_Shift},					/* % */
	{Key_D7,		Mod_Shift},					/* & */
	{Key_Oem7,		0},							/* ' */
	{Key_D9,		Mod_Shift},					/* ( */
	{Key_D0,		Mod_Shift},					/* ) */
	{Key_D8,		Mod_Shift},					/* * */
	{Key_OemPlus,	Mod_Shift},					/* + */
	{Key_OemComma,	0},							/* , */
	{Key_OemMinus,	0},							/* - */
	{Key_OemPeriod,	0},							/* . */
	{Key_Oem2,		0},							/* / */
	{Key_D0,		0},
	{Key_D1,		0},
	{Key_D2,		0},
	{Key_D3,		0},
	{Key_D4,		0},
	{Key_D5,		0},
	{Key_D6,		0},
	{Key_D7,		0},
	{Key_D8,		0},
	{Key_D9,		0},
	{Key_Oem1,		Mod_Shift},					/* : */
	{Key_Oem1,		0},							/* ; */
	{Key_OemComma,	Mod_Shift},					/* < */
	{Key_OemPlus,	0},							/* = */
	{Key_OemPeriod,	Mod_Shift},					/* > */
	{Key_Oem2,		Mod_Shift},					/* ? */
	{Key_D2,		Mod_Shift},					/* @ */
	{Key_A,			Mod_Shift},
	{Key_B,			Mod_Shift},
	{Key_C,			Mod_Shift},
	{Key_D,			Mod_Shift},
	{Key_E,			Mod_Shift},
	{Key_F,			Mod_Shift},
	{Key_G,			Mod_Shift},
	{Key_H,			Mod_Shift},
	{Key_I,			Mod_Shift},
	{Key_J,			Mod_Shift},
	{Key_K,			Mod_Shift},
	{Key_L,			Mod_Shift},
	{Key_M,			Mod_Shift},
	{Key_N,			Mod_Shift},
	{Key_O,			Mod_Shift},
	{Key_P,			Mod_Shift},
	{Key_Q,			Mod_Shift},
	{Key_R,			Mod_Shift},
	{Key_S,			Mod_Shift},
	{Key_T,			Mod_Shift},
	{Key_U,			Mod_Shift},
	{Key_V,			Mod_Shift},
	{Key_W,			Mod_Shift},
	{Key_X,			Mod_Shift},
	{Key_Y,			Mod_Shift},
	{Key_Z,			Mod_Shift},
	{Key_Oem4,		0},							/* [ */
	{Key_Oem5,		0},							/* \ */
	{Key_Oem6,		0},							/* ] */
	{Key_D6,		Mod_Shift},					/* ^ */
	{Key_OemMinus,	Mod_Shift},					/* _ */
	{Key_Oem3,		0},							/* ` */
	{Key_A,			0},
	{Key_B,			0},
	{Key_C,			0},
	{Key_D,			0},
	{Key_E,			0},
	{Key_F,			0},
	{Key_G,			0},
	{Key_H,			0},
	{Key_I,			0},
	{Key_J,			0},
	{Key_K,			0},
	{Key_L,			0},
	{Key_M,			0},
	{Key_N,			0},
	{Key_O,			0},
	{Key_P,			0},
	{Key_Q,			0},
	{Key_R,			0},
	{Key_S,			0},
	{Key_T,			0},
	{Key_U,			0},
	{Key_V,			0},
	{Key_W,			0},
	{Key_X,			0},
	{Key_Y,			0},
	{Key_Z,			0},
	{Key_Oem4,		Mod_Shift},					/* { */
	{Key_Oem5,		Mod_Shift},					/* | */
	{Key_Oem6,		Mod_Shift},					/* } */
	{Key_Oem3,		Mod_Shift},					/* ~ */
	{Key_Delete,	0},
};

/*
 * Map an ASCII character code to a Windows-like key specification.
 */
static void MapCharToKey(int ch, ILInt32 *key, ILInt32 *modifiers)
{
#ifdef IL_CHECK_ERASE
	/* If erase is set to ^?, then map that to the backspace key
	   instead of the delete key */
	if(ch == 0x7F && eraseIsDel)
	{
		*key = Key_Back;
		*modifiers = 0;
	}
	else
#endif
	if(ch >= 0 && ch < 128)
	{
		*key = (ILInt32)(MapKeys[ch].key);
		*modifiers = (ILInt32)(MapKeys[ch].modifiers);
	}
	else
	{
		*key = 0;
		*modifiers = 0;
	}
}

#endif /* IL_NEED_MAPCHAR */

#ifdef	__cplusplus
};
#endif
