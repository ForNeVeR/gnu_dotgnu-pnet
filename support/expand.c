/*
 * expand.c - Expand filenames to full pathnames.
 *
 * Copyright (C) 2002  Southern Storm Software, Pty Ltd.
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
#ifdef HAVE_UNISTD_H
	#include <unistd.h>
#endif

#ifdef	__cplusplus
extern	"C" {
#endif

char *ILExpandFilename(const char *filename, char *searchPath)
{
	char *env;
	char *path;
	char *newPath;
	int len, len2;

	/* TODO: Win32 pathname expansion */

	/* Should we do a path search? */
	if(searchPath && strchr(filename, '/') == 0 && strchr(filename, '\\') == 0)
	{
		env = searchPath;
		if(env)
		{
			while(*env != '\0')
			{
				if(*env == ':')
				{
					++env;
				}
				else
				{
					len = 1;
					while(env[len] != '\0' && env[len] != ':')
					{
						++len;
					}
					path = (char *)ILMalloc(len + strlen(filename) + 2);
					if(path)
					{
						strncpy(path, env, len);
						path[len++] = '/';
						strcpy(path + len, filename);
						if(ILFileExists(path, &newPath))
						{
							if(newPath)
							{
								ILFree(path);
								return newPath;
							}
							return path;
						}
						ILFree(path);
					}
					env += len;
				}
			}
		}
	}

	/* Get the starting directory for name resolution */
	if(*filename == '/')
	{
		path = ILDupString("/");
		++filename;
	}
	else
	{
		path = ILGetCwd();
	}
	if(!path)
	{
		return 0;
	}

	/* Process all of the filename components */
	while(*filename != '\0')
	{
		if(*filename == '.' && (filename[1] == '/' || filename[1] == '\\'))
		{
			/* Same directory: skip this component */
			filename += 2;
		}
		if(*filename == '.' && filename[1] == '\0')
		{
			/* Same directory: skip this component */
			++filename;
		}
		else if(*filename == '.' && filename[1] == '.' &&
		        (filename[2] == '/' || filename[2] == '\\' ||
				 filename[2] == '\0'))
		{
			/* Parent directory: strip the last component from "path" */
			len = strlen(path);
			while(len > 0 && path[len - 1] != '/' && path[len - 1] != '\\')
			{
				--len;
			}
			if(len > 0)
			{
				--len;
			}
			path[len] = '\0';
			if(filename[2] == '\0')
			{
				filename += 2;
			}
			else
			{
				filename += 3;
			}
		}
		else if(*filename == '/' || *filename == '\\')
		{
			/* Skip spurious directory separators */
			++filename;
		}
		else
		{
			/* Append this component to "path" */
			len = 1;
			while(filename[len] != '\0' && filename[len] != '/' &&
			      filename[len] != '\\')
			{
				++len;
			}
			len2 = strlen(path);
			newPath = (char *)ILMalloc(len2 + len + 2);
			if(!newPath)
			{
				ILFree(path);
				return 0;
			}
			strcpy(newPath, path);
			if(len2 <= 0 || (newPath[len2 - 1] != '/' &&
							 newPath[len2 - 1] != '\\'))
			{
				newPath[len2++] = '/';
			}
			ILMemCpy(newPath + len2, filename, len);
			newPath[len2 + len] = '\0';
			filename += len;
			ILFree(path);
			path = newPath;
		}
	}

	/* Done */
	return path;
}

#ifdef	__cplusplus
};
#endif
