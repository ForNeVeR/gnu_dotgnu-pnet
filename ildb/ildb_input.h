/*
 * ildb_input.h - Input handling for the debugger.
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

#ifndef	_ILDB_INPUT_H
#define	_ILDB_INPUT_H

#ifdef	__cplusplus
extern	"C" {
#endif

/*
 * Initialize the input routines.
 */
void ILDbInputInitialize(void);

/*
 * Terminate the input routines.
 */
void ILDbInputTerminate(void);

/*
 * Get the next line of input.
 */
char *ILDbInputGetLine(char *prompt);

/*
 * Clear the "repeat buffer" so that newline does
 * not cause the last command to be repeated.
 */
void ILDbInputClearRepeat(void);

/*
 * Disable input temporarily.
 */
void ILDbInputDisable(void);

/*
 * Re-enable input.
 */
void ILDbInputEnable(void);

/*
 * Confirm an action by action for a y/n answer.
 */
int ILDbInputConfirm(char *prompt, int defAnswer);

#ifdef	__cplusplus
};
#endif

#endif	/* _ILDB_INPUT_H */
