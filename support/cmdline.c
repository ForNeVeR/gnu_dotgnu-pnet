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
		/* This is the first time we have been called,
		   so expand response files before we get going */
		ILCmdLineExpand(argc, argv);
		*state = 1;
	}

	if(*state == 1)
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
			*state = 2;
		}
	}

	/* If we get here, then we are parsing single-character options */
	opt = (*argv)[1][*state - 1];
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
				if((*argv)[1][*state - 1] != '\0')
				{
					*param = (*argv)[1] + *state - 1;
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
				*state = 1;
				--(*argc);
				++(*argv);
				return options->value;
			}
			if((*argv)[1][*state - 1] == '\0')
			{
				*state = 1;
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

#ifndef IL_NATIVE_WIN32

/*
 * Abort due to insufficient memory.
 */
static void OutOfMemory(void)
{
	fputs("virtual memory exhausted\n", stderr);
	exit(1);
}

/*
 * Add a new argument to the command-line that is being built.
 */
static void AddNewArg(int *argc, char ***argv, int *maxArgc, char *value)
{
	if(*argc >= *maxArgc)
	{
		char **newArgs = (char **)ILRealloc
				(*argv, (*maxArgc + 32) * sizeof(char *));
		if(!newArgs)
		{
			OutOfMemory();
		}
		*maxArgc += 32;
		*argv = newArgs;
	}
	(*argv)[*argc] = value;
	++(*argc);
}

void ILCmdLineExpand(int *argc, char ***argv)
{
	int arg;
	int newArgc;
	int maxArgc;
	char **newArgv;
	int len;
	char *filename;
	FILE *file;
	char buffer[BUFSIZ];
	char *temp;

	/* See if we have any response file references first */
	for(arg = 1; arg < *argc; ++arg)
	{
		if((*argv)[arg][0] == '@' && (*argv)[arg][1] == '"')
		{
			break;
		}
	}
	if(arg >= *argc)
	{
		/* No response files, so nothing further to do */
		return;
	}

	/* Build a new command-line */
	newArgc = 0;
	maxArgc = 0;
	newArgv = 0;
	AddNewArg(&newArgc, &newArgv, &maxArgc, (*argv)[0]);
	for(arg = 1; arg < *argc; ++arg)
	{
		if((*argv)[arg][0] == '@' && (*argv)[arg][1] == '"')
		{
			/* Response file reference */
			len = strlen((*argv)[arg]);
			if(len >= 3 && (*argv)[arg][len - 1] == '"')
			{
				filename = ILDupString((*argv)[arg] + 2);
				if(!filename)
				{
					OutOfMemory();
				}
				filename[len - 3] = '\0';
				if((file = fopen(filename, "r")) == NULL)
				{
					perror(filename);
					exit(1);
				}
				while(fgets(buffer, BUFSIZ, file))
				{
					len = strlen(buffer);
					while(len > 0 &&
					      (buffer[len - 1] == '\r' || buffer[len - 1] == '\n'))
					{
						--len;
					}
					buffer[len] = '\0';
					if(*buffer == '"' && len >= 2 && buffer[len - 1] == '"')
					{
						/* Quoted argument in response file */
						buffer[len - 1] = '\0';
						temp = ILDupString(buffer + 1);
					}
					else
					{
						/* Ordinary argument in response file */
						temp = ILDupString(buffer);
					}
					if(!temp)
					{
						OutOfMemory();
					}
					AddNewArg(&newArgc, &newArgv, &maxArgc, temp);
				}
				fclose(file);
			}
			else
			{
				/* Badly formed response filename */
				fputs((*argv)[arg], stderr);
				fputs(": bad response filename specification\n", stderr);
				exit(1);
			}
		}
		else
		{
			/* Ordinary argument */
			AddNewArg(&newArgc, &newArgv, &maxArgc, (*argv)[arg]);
		}
	}
	AddNewArg(&newArgc, &newArgv, &maxArgc, 0);

	/* Return the new command-line to the caller */
	*argc = newArgc - 1;
	*argv = newArgv;
}

#else /* IL_NATIVE_WIN32 */

void ILCmdLineExpand(int *argc, char ***argv)
{
	/* Visual Studio's command-line parser already expands response files */
}

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
