/*
 * csant_cscc.c - Task dispatch for launching "cscc" and "csc".
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

#ifdef	__cplusplus
extern	"C" {
#endif

#if 0

/*
 * Find the pathname for "cscc".
 */
static char *FindPNetPath(void)
{
	const char *value;

	/* Check for the "cscc" property */
	value = CSAntGetProperty("cscc", -1);
	if(value)
	{
		return (char *)value;
	}

	/* Check for the "CSCC" environment variable */
	value = CSAntGetProperty("csant.env.CSCC", -1);
	if(value)
	{
		return (char *)value;
	}

	/* Assume that it is "cscc" somewhere on the path */
	return "cscc";
}

/*
 * Find the pathname for "csc".
 */
static char *FindMSPath(void)
{
	const char *value;

	/* Check for the "csc" property */
	value = CSAntGetProperty("csc", -1);
	if(value)
	{
		return (char *)value;
	}

	/* Check for the "CSC" environment variable */
	value = CSAntGetProperty("csant.env.CSC", -1);
	if(value)
	{
		return (char *)value;
	}

	/* Assume that it is "csc" somewhere on the path */
	return "csc";
}

/*
 * Add an argument to an argv-style list.
 */
static void AddArg(char ***argv, int *argc, char *value)
{
	if((*argc & 7) == 0)
	{
		*argv = (char **)ILRealloc(*argv, (*argc + 8) * sizeof(char *));
		if(!(*argv))
		{
			CSAntOutOfMemory();
		}
	}
	(*argv)[(*argc)++] = value;
}

/*
 * Add an argument and value to an argv-style list.
 */
static void AddValueArg(char ***argv, int *argc, char *name, char *value)
{
	char *str = (char *)ILMalloc(strlen(name) + strlen(value) + 1);
	if(!str)
	{
		CSAntOutOfMemory();
	}
	strcpy(str, name);
	strcat(str, value);
	AddArg(argv, argc, str);
}

/*
 * Construct a semi-colon separated list of values.
 */
static char *SemiColonList(char *list, char *value)
{
	if(!list)
	{
		list = ILDupString(value);
		if(!list)
		{
			CSAntOutOfMemory();
		}
		return list;
	}
	else
	{
		list = (char *)ILRealloc(list, strlen(list) + strlen(value) + 2);
		if(!list)
		{
			CSAntOutOfMemory();
		}
		strcat(list, ";");
		strcat(list, value);
		return list;
	}
}

/*
 * Determine if a filanem ends in a particular extension.
 */
static int EndsIn(const char *filename, const char *extension)
{
	int flen = strlen(filename);
	int elen = strlen(extension);
	if(elen < flen && !ILStrICmp(filename + flen - elen, extension))
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

/*
 * Convert a "cscc" command-line into a call on "csc" instead.
 */
static char **ConvertPNetToMS(char *argv[])
{
	char **newArgv = 0;
	int newArgc = 0;
	char *defines = 0;
	char **libDirs = 0;
	int numLibDirs = 0;
	char **libraries = 0;
	int numLibraries = 0;
	char *resources = 0;
	char **files = 0;
	int numFiles = 0;
	char *libRefs = 0;
	int argc, posn, posn2;
	char *arg;

	/* Replace the program name with "csc" */
	AddArg(&newArgv, &newArgc, FindMSPath());

	/* Add options to stop the Microsoft compiler printing silly messages */
	AddArg(&newArgv, &newArgc, "/nologo");

	/* Scan the input arguments and convert them */
	argc = 1;
	while((arg = argv[argc]) != 0)
	{
		if(!strcmp(arg, "-nostdlib"))
		{
			AddArg(&newArgv, &newArgc, "/nostdlib");
		}
		else if(!strcmp(arg, "-g"))
		{
			AddArg(&newArgv, &newArgc, "/debug+");
		}
		else if(!strcmp(arg, "-O") || !strcmp(arg, "-O2") ||
				!strcmp(arg, "-O3"))
		{
			AddArg(&newArgv, &newArgc, "/optimize+");
		}
		else if(!strcmp(arg, "-O0"))
		{
			AddArg(&newArgv, &newArgc, "/optimize-");
		}
		else if(!strcmp(arg, "-o"))
		{
			if(argv[argc + 1] != 0)
			{
				arg = CSAntDirCombineWin32(CSAntBaseDir, argv[argc + 1], 0);
				if(EndsIn(arg, ".dll"))
				{
					AddArg(&newArgv, &newArgc, "/target:library");
				}
				else if(EndsIn(arg, ".exe"))
				{
					AddArg(&newArgv, &newArgc, "/target:exe");
				}
				AddValueArg(&newArgv, &newArgc, "/out:", arg);
				++argc;
			}
		}
		else if(!strcmp(arg, "-Werror"))
		{
			AddArg(&newArgv, &newArgc, "/warnaserror+");
		}
		else if(!strcmp(arg, "-Wall"))
		{
			AddArg(&newArgv, &newArgc, "/warn:4");
		}
		else if(!strcmp(arg, "-Wno-dll-import"))
		{
			AddArg(&newArgv, &newArgc, "/nowarn:626");
		}
		else if(!strcmp(arg, "-Wno-field-assign"))
		{
			AddArg(&newArgv, &newArgc, "/nowarn:649");
		}
		else if(!strcmp(arg, "-fsemantic-check"))
		{
			AddArg(&newArgv, &newArgc, "/nooutput");
		}
		else if(!strcmp(arg, "-fchecked"))
		{
			AddArg(&newArgv, &newArgc, "/checked+");
		}
		else if(!strcmp(arg, "-funchecked"))
		{
			AddArg(&newArgv, &newArgc, "/checked-");
		}
		else if(!strcmp(arg, "-fincremental"))
		{
			AddArg(&newArgv, &newArgc, "/incremental");
		}
		else if(!strcmp(arg, "-funsafe"))
		{
			AddArg(&newArgv, &newArgc, "/unsafe");
		}
		else if(!strncmp(arg, "-fresources=", 12))
		{
			resources = SemiColonList(resources,
							CSAntDirCombineWin32(CSAntBaseDir, arg + 12, 0));
		}
		else if(!strcmp(arg, "-e"))
		{
			if(argv[argc + 1] != 0)
			{
				AddValueArg(&newArgv, &newArgc, "/main:", argv[argc + 1]);
				++argc;
			}
		}
		else if(!strncmp(arg, "-D", 2))
		{
			if(arg[2] != '\0')
			{
				defines = SemiColonList(defines, arg + 2);
			}
			else if(argv[argc + 1] != 0)
			{
				defines = SemiColonList(defines, argv[argc + 1]);
				++argc;
			}
		}
		else if(!strncmp(arg, "-L", 2))
		{
			if(arg[2] != '\0')
			{
				AddArg(&libDirs, &numLibDirs,
					   CSAntDirCombineWin32(CSAntBaseDir, arg + 2, 0));
			}
			else if(argv[argc + 1] != 0)
			{
				AddArg(&libDirs, &numLibDirs,
					   CSAntDirCombineWin32(CSAntBaseDir, argv[argc + 1], 0));
				++argc;
			}
		}
		else if(!strncmp(arg, "-l", 2))
		{
			if(arg[2] != '\0')
			{
				AddArg(&libraries, &numLibraries, arg + 2);
			}
			else if(argv[argc + 1] != 0)
			{
				AddArg(&libraries, &numLibraries, argv[argc + 1]);
				++argc;
			}
		}
		else if(arg[0] != '-')
		{
			AddArg(&files, &numFiles,
				   CSAntDirCombineWin32(CSAntBaseDir, arg, 0));
		}
		++argc;
	}

	/* Add the defines to the command-line */
	if(defines)
	{
		AddValueArg(&newArgv, &newArgc, "/define:", defines);
	}

	/* Add library references to the command-line */
	if(numLibDirs && numLibraries)
	{
		for(posn = 0; posn < numLibraries; ++posn)
		{
			for(posn2 = 0; posn2 < numLibDirs; ++posn2)
			{
				arg = CSAntDirCombineWin32(libDirs[posn2], libraries[posn],
										   ".dll");
				if(ILFileExists(arg, (char **)0))
				{
					libRefs = SemiColonList(libRefs, arg);
				}
				ILFree(arg);
			}
		}
	}
	if(libRefs)
	{
		AddValueArg(&newArgv, &newArgc, "/reference:", libRefs);
	}

	/* Add the resources to the command-line */
	if(resources)
	{
		AddValueArg(&newArgv, &newArgc, "/resource:", resources);
	}

	/* Add the source files to the command-line */
	for(posn = 0; posn < numFiles; ++posn)
	{
		AddArg(&newArgv, &newArgc, files[posn]);
	}

	/* Terminate the command-line */
	AddArg(&newArgv, &newArgc, 0);

	/* Done */
	return newArgv;
}

/*
 * Convert a "csc" command-line into a call on "cscc" instead.
 */
static int ConvertMSToPNet(char *argv[])
{
	return 1;
}

#endif

/*
 * Handle a "cscc" task, which invokes the Portable.NET C# compiler.
 */
int CSAntTask_Cscc(CSAntTask *task)
{
	return 1;
}

/*
 * Handle a "csc" task, which invokes the Microsoft C# compiler.
 */
int CSAntTask_Csc(CSAntTask *task)
{
	return 1;
}

#ifdef	__cplusplus
};
#endif
