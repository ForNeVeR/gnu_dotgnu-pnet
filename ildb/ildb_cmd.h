/*
 * ildb_cmd.h - Command parsing and dispatching.
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

#ifndef	_ILDB_CMD_H
#define	_ILDB_CMD_H

#ifdef	__cplusplus
extern	"C" {
#endif

/*
 * Command information block.
 */
typedef struct
{
	/* Name of the primary command, plus the number of characters
	   that must be present for a match */
	const char *name;
	int			matchAtLeast;

	/* Name of the secondary command, plus the number of characters
	   that must be present for a match.  NULL if no sub-command */
	const char *subName;
	int			subMatchAtLeast;

	/* Function that is called for commands with pre-parsed parameters */
	void      (*cmdFunc)(ILDb *db, char *argv[]);

	/* Function that is called for commands that parse their own parameters */
	void      (*cmdParseFunc)(ILDb *db, char *args);

	/* Help message string for the command */
	char	   *help;

} ILDbCmdInfo;

/*
 * Register a list of commands.
 */
void ILDbRegisterCommands(ILDb *db, ILDbCmdInfo *cmds, int numCmds);

/*
 * Free all command lists.
 */
void ILDbFreeCommands(ILDb *db);

/*
 * Parse and dispatch a command.
 */
void ILDbDispatchCommand(ILDb *db, char *cmd);

/*
 * Standard command lists.
 */
extern ILDbCmdInfo ILDbListCommands[];
extern int         ILDbNumListCommands;
extern ILDbCmdInfo ILDbDisplayCommands[];
extern int         ILDbNumDisplayCommands;
extern ILDbCmdInfo ILDbShowCommands[];
extern int         ILDbNumShowCommands;
extern ILDbCmdInfo ILDbSystemCommands[];
extern int         ILDbNumSystemCommands;
extern ILDbCmdInfo ILDbSetCommands[];
extern int         ILDbNumSetCommands;

#ifdef	__cplusplus
};
#endif

#endif	/* _ILDB_CMD_H */
