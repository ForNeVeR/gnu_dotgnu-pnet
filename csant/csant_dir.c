/*
 * csant_dir.c - Directory walking and name manipulation tools.
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

#include "csant_defs.h"
#include <dirent.h>
#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif
#ifdef HAVE_REGEX_H
#include <regex.h>
#endif
#ifdef HAVE_SYS_CYGWIN_H
#include <sys/cygwin.h>
#endif

#ifdef	__cplusplus
extern	"C" {
#endif

/*
 * Directory walking structure.
 */
struct _tagCSAntDir
{
	DIR    *dirInfo;
	int     useRegex;
#ifdef HAVE_REGCOMP
	regex_t regexState;
#endif
};

CSAntDir *CSAntDirOpen(const char *pathname, const char *fileRegex)
{
	DIR *dirInfo;
	CSAntDir *dir;
	char *regex;
	char *out;
	char lastch;
	char *newPathname;

	/* Attempt to open the directory, after normalizing the path */
	newPathname = CSAntDirCombine(pathname, 0);
	dirInfo = opendir(newPathname);
	ILFree(newPathname);
	if(!dirInfo)
	{
		return 0;
	}

	/* Allocate space for the directory walker */
	dir = (CSAntDir *)ILMalloc(sizeof(CSAntDir));
	if(!dir)
	{
		CSAntOutOfMemory();
	}
	dir->dirInfo = dirInfo;

	/* Convert the regular expression from file form into grep form */
	if(fileRegex)
	{
		regex = (char *)ILMalloc(strlen(fileRegex) * 4 + 20);
		if(!regex)
		{
			CSAntOutOfMemory();
		}
		out = regex;
		*out++ = '^';
		while(*fileRegex != '\0')
		{
			lastch = *fileRegex++;
			if(lastch == '?')
			{
				*out++ = '.';
			}
			else if(lastch == '*')
			{
				*out++ = '.';
				*out++ = '*';
			}
			else if(lastch == '[')
			{
				*out++ = '[';
				while(*fileRegex != '\0')
				{
					lastch = *fileRegex++;
					if(lastch == ']')
					{
						break;
					}
					*out++ = lastch;
				}
				*out++ = ']';
			}
			else if(lastch == '.' || lastch == '^' || lastch == '$' ||
					lastch == ']' || lastch == '\\')
			{
				*out++ = '\\';
				*out++ = lastch;
			}
			else
			{
				*out++ = lastch;
			}
		}
		*out++ = '$';
		*out = '\0';
	}
	else
	{
		regex = 0;
	}

	/* Compile the regular expression */
	if(regex)
	{
#ifdef HAVE_REGCOMP
		/* POSIX-style regular expression library */
		if(regcomp(&(dir->regexState), regex, REG_EXTENDED | REG_NOSUB) != 0)
		{
			fprintf(stderr, "Invalid regular expression\n");
			exit(1);
		}
#else
#ifdef HAVE_RE_COMP
		/* BSD-style regular expression library */
		if(re_comp(regex) != 0)
		{
			fprintf(stderr, "Invalid regular expression\n");
			exit(1);
		}
#else
		fprintf(stderr, "Regular expression library not present - aborting\n");
		exit(1);
#endif
#endif
		ILFree(regex);
		dir->useRegex = 1;
	}
	else
	{
		dir->useRegex = 0;
	}

	/* Ready to go */
	return dir;
}

void CSAntDirClose(CSAntDir *dir)
{
	closedir(dir->dirInfo);
#ifdef HAVE_REGCOMP
	if(dir->useRegex)
	{
		regfree(&(dir->regexState));
	}
#endif
	ILFree(dir);
}

const char *CSAntDirNext(CSAntDir *dir)
{
	struct dirent *entry;
	for(;;)
	{
		/* Read the next raw entry from the operating system */
		entry = readdir(dir->dirInfo);
		if(!entry)
		{
			break;
		}

		/* Always ignore "." and ".." */
		if(!strcmp(entry->d_name, ".") || !strcmp(entry->d_name, ".."))
		{
			continue;
		}

		/* Ignore the name if the regular expression does not match */
		if(dir->useRegex)
		{
		#ifdef HAVE_REGCOMP
			if(regexec(&(dir->regexState), entry->d_name, 0, 0, 0) != 0)
			{
				continue;
			}
		#else
		#ifdef HAVE_RE_COMP
			if(!re_exec(entry->d_name))
			{
				continue;
			}
		#endif
		#endif
		}

		/* We have a match */
		return entry->d_name;
	}
	return 0;
}

char *CSAntDirCombine(const char *pathname, const char *filename)
{
	char *name;
	char *temp;
	if(filename && filename[0] == '/')
	{
		name = ILDupString(filename);
		if(!name)
		{
			CSAntOutOfMemory();
		}
	}
	else
	{
		name = (char *)ILMalloc(strlen(pathname) +
							    (filename ? strlen(filename) + 1 : 0) + 1);
		if(!name)
		{
			CSAntOutOfMemory();
		}
		strcpy(name, pathname);
		if(filename)
		{
			strcat(name, "/");
			strcat(name, filename);
		}
	}
	temp = name;
	while(*temp != '\0')
	{
		if(*temp == '/' || *temp == '\\')
		{
		#ifdef IL_NATIVE_WIN32
			*temp = '\\';
		#else
			*temp = '/';
		#endif
		}
		++temp;
	}
	return name;
}

char *CSAntDirCombineWin32(const char *pathname, const char *filename,
						   const char *extension)
{
	/* Combine the path components */
	char *name;
	if(filename &&
	   (filename[0] == '/' || filename[0] == '\\' ||
	    (filename[0] != '\0' && filename[1] == ':')))
	{
		name = (char *)ILMalloc(strlen(filename) +
							    (extension ? strlen(extension) : 0) + 1);
		if(!name)
		{
			CSAntOutOfMemory();
		}
		strcpy(name, filename);
		if(extension)
		{
			strcat(name, extension);
		}
	}
	else
	{
		name = (char *)ILMalloc(strlen(pathname) +
							    (filename ? strlen(filename) + 1 : 0) +
							    (extension ? strlen(extension) : 0) + 1);
		if(!name)
		{
			CSAntOutOfMemory();
		}
		strcpy(name, pathname);
		if(filename)
		{
			strcat(name, "/");
			strcat(name, filename);
			if(extension)
			{
				strcat(name, extension);
			}
		}
	}

#if defined(HAVE_SYS_CYGWIN_H) && \
    defined(HAVE_CYGWIN_CONV_TO_WIN32_PATH)

	/* Use the Cygwin-supplied function to convert the path */
	{
		char buf[4096];
		char *temp;
		if(cygwin_conv_to_win32_path(name, buf) == 0)
		{
			temp = ILDupString(buf);
			if(!temp)
			{
				CSAntOutOfMemory();
			}
			ILFree(name);
			return temp;
		}
	}

#endif

	/* Simple method - just convert slashes into backslashes */
	{
		char *temp;
		temp = name;
		while(*temp != '\0')
		{
			if(*temp == '/' || *temp == '\\')
			{
				*temp = '\\';
			}
			++temp;
		}
	}
	return name;
}

#ifdef	__cplusplus
};
#endif
