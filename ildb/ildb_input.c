/*
 * ildb_input.c - Input handling for the debugger.
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

#include "il_system.h"
#include "ildb_input.h"
#include <stdio.h>

/*
 * Turn off "HAVE_CONFIG_H", because it confuses the
 * readline headers on some systems.
 */
#undef HAVE_CONFIG_H

/*
 * Determine if we have the readline and history libraries.
 */
#if defined(HAVE_LIBREADLINE) && defined(HAVE_READLINE_READLINE_H)
#include <readline/readline.h>
#define	ILDB_USE_READLINE
#if defined(HAVE_LIBHISTORY) && defined(HAVE_READLINE_HISTORY_H)
#include <readline/history.h>
#define	ILDB_USE_HISTORY
#endif
#endif

#ifdef	__cplusplus
extern	"C" {
#endif

static int  canRepeat = 0;

#ifdef ILDB_USE_READLINE

static char *lastLine = NULL;

#else	/* !ILDB_USE_READLINE */

static char buffer[BUFSIZ];
static char lastBuffer[BUFSIZ];

#endif	/* !ILDB_USE_READLINE */

void ILDbInputInitialize(void)
{
#ifdef ILDB_USE_READLINE

	/* Set the name to use for conditional parsing of .inputrc */
	rl_readline_name = "ildb";

	/* Initialize the readline library */
	rl_initialize();

	/* Use the default TAB behaviour: we don't want filename completion */
	rl_bind_key('\t', rl_insert);

#ifdef ILDB_USE_HISTORY
	/* Initialize the history handling */
	using_history();
#endif	/* ILDB_USE_HISTORY */

#endif	/* ILDB_USE_READLINE */

	/* Nothing in the repeat buffer yet */
	canRepeat = 0;
}

void ILDbInputTerminate(void)
{
#ifdef ILDB_USE_READLINE
	if(lastLine)
	{
		free(lastLine);
		lastLine = NULL;
	}
#endif	/* ILDB_USE_READLINE */
	canRepeat = 0;
}

char *ILDbInputGetLine(char *prompt)
{
#ifdef ILDB_USE_READLINE

	/* Save the last line until we know if we should repeat it */
	char *saveLine = lastLine;

	/* Read a new line */
	lastLine = readline(prompt);

#ifdef ILDB_USE_HISTORY
	/* Add the line to the history if it is not empty */
	if(lastLine && *lastLine != '\0')
	{
		add_history(lastLine);
	}
#endif	/* ILDB_USE_HISTORY */

	/* Perform repeat buffer processing */
	if(lastLine && *lastLine == '\0' && saveLine && canRepeat)
	{
		free(lastLine);
		lastLine = saveLine;
	}
	else if(saveLine)
	{
		free(saveLine);
	}
	saveLine = lastLine;
	if(lastLine)
	{
		canRepeat = 1;
	}

	/* Return the line to the caller */
	return lastLine;

#else	/* !ILDB_USE_READLINE */

	/* We don't have readline, so fall back to ye olde fgets */
	int len;

	/* Print the prompt */
	fputs(prompt, stdout);
	fflush(stdout);

	/* Get the next line of input, and strip newlines from it */
	if(!fgets(buffer, BUFSIZ, stdin))
	{
		return 0;
	}
	len = strlen(buffer);
	while(len > 0 && (buffer[len - 1] == '\n' || buffer[len - 1] == '\r'))
	{
		--len;
	}
	buffer[len] = '\0';

	/* Perform repeat buffer processing */
	if(canRepeat && len == 0)
	{
		strcpy(buffer, lastBuffer);
	}
	else
	{
		strcpy(lastBuffer, buffer);
		canRepeat = 1;
	}

	/* Return the line to the caller */
	return buffer;

#endif	/* !ILDB_USE_READLINE */
}

void ILDbInputClearRepeat(void)
{
	canRepeat = 0;
}

void ILDbInputDisable(void)
{
}

void ILDbInputEnable(void)
{
}

int ILDbInputConfirm(char *prompt, int defAnswer)
{
	char *line;
	ILDbInputClearRepeat();
	do
	{
		line = ILDbInputGetLine(prompt);
		ILDbInputClearRepeat();
		if(!line)
		{
			return defAnswer;
		}
		while(*line != '\0')
		{
			if(*line != ' ' && *line != '\t')
			{
				break;
			}
			++line;
		}
		if(*line == 'y' || *line == 'Y')
		{
			return 1;
		}
		else if(*line == 'n' || *line == 'N')
		{
			return 0;
		}
		else if(*line != '\0')
		{
			fputs("Incorrect response. Please enter y or n.\n", stdout);
			fflush(stdout);
		}
	}
	while(*line != '\0');
	return defAnswer;
}

#ifdef	__cplusplus
};
#endif
