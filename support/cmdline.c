/*
 * cmdline.c - Command-line option parsing routines.
 *
 * Copyright (C) 2001  Southern Storm Software, Pty Ltd.
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

#include <stdio.h>
#include "il_system.h"
#include "il_utils.h"

#ifdef	__cplusplus
extern	"C" {
#endif

int ILCmdLineNextOption(int *argc, char ***argv, int *state,
						const ILCmdLineOption *options, char **param)
{
	char opt;

	if(*state == 0)
	{
		/* We are at the start of a new option */
		if(*argc <= 1)
		{
			return 0;
		}
		if((*argv)[1][0] != '-' || (*argv)[1][1] == '\0')
		{
			return 0;
		}
		if((*argv)[1][1] == '-')
		{
			/* The option begins with "--" */
			if((*argv)[1][2] == '\0')
			{
				--(*argc);
				++(*argv);
				return 0;
			}
			while(options->name != 0)
			{
				if(!strcmp(options->name, (*argv)[1]))
				{
					if(options->hasParam)
					{
						/* Look for a parameter for this option */
						--(*argc);
						++(*argv);
						if(*argc <= 1)
						{
							return -1;
						}
						*param = (*argv)[1];
					}
					--(*argc);
					++(*argv);
					return options->value;
				}
				++options;
			}
			return -1;
		}
		else
		{
			/* The option begins with "-" */
			*state = 1;
		}
	}

	/* If we get here, then we are parsing single-character options */
	opt = (*argv)[1][*state];
	if(opt == '-')
	{
		return -1;
	}
	while(options->name != 0)
	{
		if(options->name[0] == '-' &&
		   opt == options->name[1])
		{
			++(*state);
			if(options->hasParam)
			{
				/* Look for a parameter for this option */
				if((*argv)[1][*state] != '\0')
				{
					*param = (*argv)[1] + *state;
				}
				else
				{
					--(*argc);
					++(*argv);
					if(*argc <= 1)
					{
						return -1;
					}
					*param = (*argv)[1];
				}
				*state = 0;
				--(*argc);
				++(*argv);
				return options->value;
			}
			if((*argv)[1][*state] == '\0')
			{
				*state = 0;
				--(*argc);
				++(*argv);
			}
			return options->value;
		}
		++options;
	}
	return -1;
}

void ILCmdLineHelp(const ILCmdLineOption *options)
{
	const char *str;
	while(options->name != 0)
	{
		if(options->helpString1)
		{
			fprintf(stderr, "    %s\n        ", options->helpString1);
			str = options->helpString2;
			while(*str != '\0')
			{
				if(*str == '\n')
				{
					fputs("\n        ", stderr);
				}
				else
				{
					putc(*str, stderr);
				}
				++str;
			}
			putc('\n', stderr);
		}
		++options;
	}
}

#ifdef IL_NATIVE_WIN32

/*
 * This function stub forces Visual Studio to use a
 * command-line argument parser that supports wildcards.
 */

void __cdecl _setargv(void)
{
	extern void __cdecl __setargv(void);
	__setargv();
}

#endif

#ifdef	__cplusplus
};
#endif
