/*
 * pretty.h - Color print formatting
 *
 * Copyright (C) 2001  FreeDevelopers.net
 * 
 * Author : Gopal.V
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


#ifndef _IL_PRETTY_H
#define _IL_PRETTY_H
#include <stdio.h>
/*
 * Trying some fancy formatting 
 * to support this on various platforms
 * Should I give an #ifdef LINUX
 * or #ifdef GNU ?
 */
#define IL_COLOR_RED "\033[31m"
#define IL_COLOR_GREEN "\033[32m"
#define IL_COLOR_BLUE "\033[34m"
#define IL_COLOR_YELLOW "\033[33m"
#define IL_COLOR_MAGENTA "\033[35m"
#define IL_COLOR_CYAN "\033[36m"
#define IL_COLOR_BOLD "\033[1m"
#define IL_COLOR_CLEAR "\033[37m\033[22m"
//macros to use these 
#ifndef IF_PRETTY_PRINT
#define IF_PRETTY_PRINT(x) if(0)x;
//provide default macro which should be
//overriden 
//see ildiff.c for more details
#endif
#define ILColorRed() 		IF_PRETTY_PRINT(printf(IL_COLOR_RED));
#define ILColorGreen() 		IF_PRETTY_PRINT(printf(IL_COLOR_GREEN));
#define ILColorBlue() 		IF_PRETTY_PRINT(printf(IL_COLOR_BLUE));
#define ILColorMagenta() 	IF_PRETTY_PRINT(printf(IL_COLOR_MAGENTA));
#define ILColorYellow() 	IF_PRETTY_PRINT(printf(IL_COLOR_YELLOW));
#define ILColorClear()		IF_PRETTY_PRINT(printf(IL_COLOR_CLEAR));
#define ILColorBold()		IF_PRETTY_PRINT(printf(IL_COLOR_BOLD));

#endif/*_IL_PRETTY_H*/
//well ain't it pretty or what ?
