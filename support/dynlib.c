/*
 * dynlib.c - Dynamic library support routines.
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
#ifdef HAVE_DLFCN_H
	#include <dlfcn.h>
#endif

#ifdef	__cplusplus
extern	"C" {
#endif

#if defined(HAVE_DLFCN_H) && defined(HAVE_DLOPEN)

void *ILDynLibraryOpen(const char *name)
{
	void *handle;
	int len;
	char *fullName;
	const char *error;

	/* Convert the name into its full form */
#ifdef _WIN32
	len = strlen(fullName);
	if(len < 4 || ILStrICmp(name + len - 4, ".dll") != 0)
	{
		fullName = (char *)ILMalloc(len + 5);
		if(!fullName)
		{
			return 0;
		}
		strcpy(fullName, name);
		if(len > 0 && name[len - 1] == '.')
		{
			strcat(fullName, "dll");
		}
		else
		{
			strcat(fullName, ".dll");
		}
	}
	else
	{
		fullName = ILDupString(name);
		if(!fullName)
		{
			return 0;
		}
	}
#else	/* !_WIN32 */
	int needSO, needLib;
	len = 0;
	needSO = 1;
	while(name[len] != '\0')
	{
		if(name[len] == '.' && name[len + 1] == 's' && name[len + 2] == 'o' &&
		   (name[len + 3] == '\0' || name[len + 3] == '.'))
		{
			needSO = 0;
			break;
		}
		++len;
	}
	if(strchr(name, '/') == 0 &&
	   (name[0] != 'l' || name[1] != 'i' || name[2] != 'b'))
	{
		needLib = 1;
	}
	else
	{
		needLib = 0;
	}
	fullName = (char *)ILMalloc(strlen(name) + (needSO ? 3 : 0) +
											   (needLib ? 3 : 0) + 1);
	if(!fullName)
	{
		return 0;
	}
	if(needLib)
	{
		strcpy(fullName, "lib");
		strcat(fullName, name);
	}
	else
	{
		strcpy(fullName, name);
	}
	if(needSO)
	{
		strcat(fullName, ".so");
	}
#endif	/* !_WIN32 */

	/* Attempt to load the library */
	handle = dlopen(fullName, RTLD_LAZY | RTLD_GLOBAL);
	if(!handle)
	{
		error = dlerror();
		fprintf(stderr, "%s: %s\n", fullName,
				(error ? error : "could not load dynamic library"));
		ILFree(fullName);
		return 0;
	}

	/* Done */
	ILFree(fullName);
	return handle;
}

void  ILDynLibraryClose(void *handle)
{
	dlclose(handle);
}

void *ILDynLibraryGetSymbol(void *handle, const char *symbol)
{
	void *value = dlsym(handle, (char *)symbol);
	const char *error = dlerror();
	char *newName;
	if(error == 0)
	{
		return value;
	}
	newName = (char *)ILMalloc(strlen(symbol) + 2);
	if(newName)
	{
		/* Try again with '_' prepended to the name in case
		   we are running on a system with a busted "dlsym" */
		newName[0] = '_';
		strcpy(newName + 1, symbol);
		value = dlsym(handle, newName);
		error = dlerror();
		if(error == 0)
		{
			ILFree(newName);
			return value;
		}
		ILFree(newName);
	}
	fprintf(stderr, "%s: %s\n", symbol, error);
	return 0;
}

#else	/* No dynamic library support */

void *ILDynLibraryOpen(const char *name)
{
	fprintf(stderr, "%s: dynamic libraries are not available\n", name);
	return 0;
}

void  ILDynLibraryClose(void *handle)
{
}

void *ILDynLibraryGetSymbol(void *handle, const char *symbol)
{
	return 0;
}

#endif	/* No dynamic library support */

#ifdef	__cplusplus
};
#endif
