/*
 * ildb_cmd.c - Command parsing and dispatching.
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

#include "ildb_context.h"
#include "ildb_utils.h"
#include "ildb_cmd.h"
#include "il_system.h"

#ifdef	__cplusplus
extern	"C" {
#endif

/*
 * List header block.
 */
struct _tagILDbCmdList
{
	ILDbCmdInfo	   *cmds;
	int				numCmds;
	ILDbCmdList    *next;

};

void ILDbRegisterCommands(ILDb *db, ILDbCmdInfo *cmds, int numCmds)
{
	ILDbCmdList *elem = (ILDbCmdList *)ILMalloc(sizeof(ILDbCmdList));
	if(!elem)
	{
		ILDbOutOfMemory(db);
	}
	elem->cmds = cmds;
	elem->numCmds = numCmds;
	elem->next = 0;
	if(db->lastCommands)
	{
		db->lastCommands->next = elem;
	}
	else
	{
		db->commands = elem;
	}
	db->lastCommands = elem;
}

void ILDbFreeCommands(ILDb *db)
{
	ILDbCmdList *elem, *next;
	elem = db->commands;
	while(elem != 0)
	{
		next = elem->next;
		ILFree(elem);
		elem = next;
	}
	db->commands = 0;
	db->lastCommands = 0;
}

/*
 * Determine if a command match has occurred against an ILDbCmdInfo object.
 */
static int CommandMatch(ILDbCmdInfo *info, char *cmdName, int cmdNameLen,
						char *subCmdName, int subCmdNameLen)
{
	int matchAtLeast;
	int matchAtMost;
	int posn, ch;

	/* See if we have a match against the command */
	matchAtLeast = info->matchAtLeast;
	matchAtMost = strlen(info->name);
	if(!matchAtLeast)
	{
		matchAtLeast = matchAtMost;
	}
	if(cmdNameLen < matchAtLeast || cmdNameLen > matchAtMost)
	{
		return 0;
	}
	for(posn = 0; posn < cmdNameLen; ++posn)
	{
		ch = cmdName[posn];
		if(ch >= 'A' && ch <= 'Z')
		{
			ch = (ch - 'A' + 'a');
		}
		if(ch != info->name[posn])
		{
			return 0;
		}
	}

	/* If there is no sub-command in "info", then we have a match */
	if(!(info->subName))
	{
		return 1;
	}

	/* If no sub-command was supplied, then there is no match */
	if(!subCmdName)
	{
		return 0;
	}

	/* See if we have a match against the sub-command */
	matchAtLeast = info->subMatchAtLeast;
	matchAtMost = strlen(info->subName);
	if(!matchAtLeast)
	{
		matchAtLeast = matchAtMost;
	}
	if(subCmdNameLen < matchAtLeast || subCmdNameLen > matchAtMost)
	{
		return 0;
	}
	for(posn = 0; posn < subCmdNameLen; ++posn)
	{
		ch = subCmdName[posn];
		if(ch >= 'A' && ch <= 'Z')
		{
			ch = (ch - 'A' + 'a');
		}
		if(ch != info->subName[posn])
		{
			return 0;
		}
	}

	/* We have a match against both the command and sub-command */
	return 1;
}

/*
 * Add an argument to an argv array.
 */
static void AddArgument(ILDb *db, int *argc, char ***argv,
						char *value, int len)
{
	char **newArgv = (char **)ILRealloc(*argv, (*argc + 1) * sizeof(char *));
	char *newValue;
	if(!newArgv)
	{
		ILDbOutOfMemory(db);
	}
	*argv = newArgv;
	if(value)
	{
		newValue = (char *)ILMalloc(len + 1);
		if(!newValue)
		{
			ILDbOutOfMemory(db);
		}
		if(len > 0)
		{
			ILMemCpy(newValue, value, len);
		}
		newValue[len] = '\0';
		newArgv[*argc] = newValue;
	}
	else
	{
		newArgv[*argc] = 0;
	}
	++(*argc);
}

/*
 * Dispatch a command after parsings its arguments into argv form.
 */
static void DispatchArgV(ILDb *db, ILDbCmdInfo *info, char *args)
{
	int argc = 0;
	char **argv = 0;
	char *value;
	int len, quote;

	/* Break the rest of "args" into individual arguments */
	while(*args != '\0')
	{
		if(ILDbIsSpace(*args))
		{
			/* White space between arguments */
			++args;
		}
		else if(*args == '"' || *args == '\'')
		{
			/* Quoted string */
			quote = *args++;
			value = args;
			len = 0;
			while(*args != '\0' && *args != quote)
			{
				++args;
				++len;
			}
			if(*args == quote)
			{
				++args;
			}
			AddArgument(db, &argc, &argv, value, len);
		}
		else
		{
			/* Regular argument */
			value = args;
			len = 0;
			while(*args != '\0' && !ILDbIsSpace(*args) &&
			      *args != '"' && *args != '\'')
			{
				++len;
				++args;
			}
			AddArgument(db, &argc, &argv, value, len);
		}
	}
	AddArgument(db, &argc, &argv, 0, 0);

	/* Dispatch the command */
	(*(info->cmdFunc))(db, argv);

	/* Free the argument array */
	argc = 0;
	while(argv[argc] != 0)
	{
		ILFree(argv[argc]);
		++argc;
	}
	ILFree(argv);
}

void ILDbDispatchCommand(ILDb *db, char *cmd)
{
	char *cmdName;
	int cmdNameLen;
	char *subCmdName;
	int subCmdNameLen;
	ILDbCmdList *list;
	int num;
	ILDbCmdInfo *info;
	char savech, savech2;
	int isSubCmd;

	/* Skip leading white space on the command-line */
	while(ILDbIsSpace(*cmd))
	{
		++cmd;
	}

	/* If the command was the empty string, then ignore it */
	if(*cmd == '\0')
	{
		return;
	}

	/* Extract the name of the command */
	cmdName = cmd;
	while(*cmd != '\0' && !ILDbIsSpace(*cmd))
	{
		++cmd;
	}
	cmdNameLen = (int)(cmd - cmdName);

	/* Skip white space after the command name */
	while(ILDbIsSpace(*cmd))
	{
		++cmd;
	}

	/* Extract the next argument, in case this is a two-part command */
	if(*cmd != '\0')
	{
		subCmdName = cmd;
		subCmdNameLen = 0;
		while(cmd[subCmdNameLen] != '\0' && !ILDbIsSpace(cmd[subCmdNameLen]))
		{
			++subCmdNameLen;
		}
	}
	else
	{
		subCmdName = 0;
		subCmdNameLen = 0;
	}

	/* Scan through the command list looking for a match */
	isSubCmd = 0;
	list = db->commands;
	while(list != 0)
	{
		for(num = 0; num < list->numCmds; ++num)
		{
			info = &(list->cmds[num]);
			if(CommandMatch(info, cmdName, cmdNameLen,
						    subCmdName, subCmdNameLen))
			{
				/* Is this the default case for a command with sub-commands? */
				if(!(info->cmdFunc) && !(info->cmdParseFunc))
				{
					isSubCmd = 1;
					break;
				}

				/* Skip the sub-command name, which we have now matched */
				if(info->subName)
				{
					cmd += subCmdNameLen;
					while(ILDbIsSpace(*cmd))
					{
						++cmd;
					}
				}

				/* Dispatch the command */
				if(info->cmdFunc)
				{
					/* Parse the arguments using the argv[] logic */
					DispatchArgV(db, info, cmd);
				}
				else
				{
					/* Let the command function parse the arguments */
					(*(info->cmdParseFunc))(db, cmd);
				}
				return;
			}
		}
		list = list->next;
	}

	/* We don't know what this is, so print a message to that effect */
	savech = cmdName[cmdNameLen];
	cmdName[cmdNameLen] = '\0';
	if(isSubCmd)
	{
		/* A sub-command was expected */
		if(subCmdName)
		{
			/* A sub-command name was supplied */
			savech2 = subCmdName[subCmdNameLen];
			subCmdName[subCmdNameLen] = '\0';
			ILDbError(db, "Undefined %s command: \"%s\".  Try \"help %s\".\n",
					  cmdName, subCmdName, cmdName);
			subCmdName[subCmdNameLen] = savech2;
			ILDbTrace(db, "undefined command `%s'", cmdName);
		}
		else
		{
			/* No sub-command, so print a generic message */
			ILDbError(db, "Unspecified sub-command for \"%s\".  "
					  "Try \"help %s\".\n", cmdName, cmdName);
			ILDbTrace(db, "sub-command required for `%s'", cmdName);
		}
	}
	else
	{
		/* This is a regular command with no sub-commands */
		ILDbError(db, "Undefined command: \"%s\".  Try \"help\".\n", cmdName);
		ILDbTrace(db, "undefined command `%s'", cmdName);
	}
	cmdName[cmdNameLen] = savech;
}

/*
 * Print help on specific commands.
 */
static void Show_Help(ILDb *db, char *argv[])
{
	ILDbCmdList *list;
	int cmd;

	if(argv[0] && argv[1])
	{
		/* Print help on a command that has sub-commands */
		int helpShown = 0;
		list = db->commands;
		while(list != 0)
		{
			for(cmd = 0; cmd < list->numCmds; ++cmd)
			{
				if(CommandMatch(&(list->cmds[cmd]),
								argv[0], strlen(argv[0]),
							    argv[1], strlen(argv[1])) &&
				   list->cmds[cmd].subName)
				{
					if(list->cmds[cmd].longHelp)
					{
						/* Print the long help for the command */
						ILDbInfo(db, "%s", list->cmds[cmd].longHelp);
						helpShown = 1;
					}
					else if(list->cmds[cmd].help)
					{
						/* Print the short help for the command */
						ILDbInfo(db, "%s -- %s",
								 list->cmds[cmd].name,
								 list->cmds[cmd].help);
						helpShown = 1;
					}
				}
			}
			list = list->next;
		}
		if(!helpShown)
		{
			ILDbError(db, "No help found for `%s %s'", argv[0], argv[1]);
		}
	}
	else if(argv[0])
	{
		/* Print help on a main command */
		int subCommand = 0;
		int helpShown = 0;
		list = db->commands;
		while(list != 0)
		{
			for(cmd = 0; cmd < list->numCmds; ++cmd)
			{
				if(!ILStrICmp(list->cmds[cmd].name, argv[0]))
				{
					if(list->cmds[cmd].subName)
					{
						/* List short help for the matching sub-commands */
						if(list->cmds[cmd].help)
						{
							/* Print the short help for the command */
							ILDbInfo(db, "%s %s -- %s",
									 list->cmds[cmd].name,
									 list->cmds[cmd].subName,
									 list->cmds[cmd].help);
							helpShown = 1;
						}
						subCommand = 1;
					}
					else if(!subCommand)
					{
						if(list->cmds[cmd].longHelp)
						{
							/* Print the long help for the command */
							ILDbInfo(db, "%s", list->cmds[cmd].longHelp);
							helpShown = 1;
						}
						else if(list->cmds[cmd].help)
						{
							/* Print the short help for the command */
							ILDbInfo(db, "%s -- %s",
									 list->cmds[cmd].name,
									 list->cmds[cmd].help);
							helpShown = 1;
						}
					}
				}
			}
			list = list->next;
		}
		if(!helpShown)
		{
			ILDbError(db, "No help found for `%s'", argv[0]);
		}
	}
	else
	{
		/* Print help on all main commands and categories */
		list = db->commands;
		while(list != 0)
		{
			for(cmd = 0; cmd < list->numCmds; ++cmd)
			{
				if(!(list->cmds[cmd].subName) && list->cmds[cmd].help)
				{
					ILDbInfo(db, "%s -- %s",
							 list->cmds[cmd].name,
							 list->cmds[cmd].help);
				}
			}
			list = list->next;
		}
	}
}

/*
 * Table of "help" commands.
 */
ILDbCmdInfo ILDbHelpCommands[] = {
	{"help", 1, 0, 0, Show_Help, 0, "print list of commands", 0},
};
int ILDbNumHelpCommands = (sizeof(ILDbHelpCommands) / sizeof(ILDbCmdInfo));

#ifdef	__cplusplus
};
#endif
