/*
 * csant_cscc.c - Task dispatch for launching C# compilers.
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
#include "csant_fileset.h"

#ifdef	__cplusplus
extern	"C" {
#endif

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
 * Find the pathname for "mcs".
 */
static char *FindMonoPath(void)
{
	const char *value;

	/* Check for the "mcs" property */
	value = CSAntGetProperty("mcs", -1);
	if(value)
	{
		return (char *)value;
	}

	/* Check for the "MCS" environment variable */
	value = CSAntGetProperty("csant.env.MCS", -1);
	if(value)
	{
		return (char *)value;
	}

	/* Assume that it is "mcs" somewhere on the path */
	return "mcs";
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
 * Add an argument to an argv-style list if it isn't already present.
 */
static void AddUnique(char ***argv, int *argc, const char *value, int len)
{
	int posn;
	char *copyValue;
	for(posn = 0; posn < *argc; ++posn)
	{
		if(!strncmp((*argv)[posn], value, len) &&
		   (*argv)[posn][len] == '\0')
		{
			return;
		}
	}
	copyValue = (char *)ILMalloc(len + 1);
	if(!copyValue)
	{
		CSAntOutOfMemory();
	}
	ILMemCpy(copyValue, value, len);
	copyValue[len] = '\0';
	AddArg(argv, argc, copyValue);
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
 * Add an argument and length-specified value to an argv-style list.
 */
static void AddValueLenArg(char ***argv, int *argc, char *name,
						   char *value, int len)
{
	char *str = (char *)ILMalloc(strlen(name) + len + 1);
	if(!str)
	{
		CSAntOutOfMemory();
	}
	strcpy(str, name);
	ILMemCpy(str + strlen(name), value, len);
	str[strlen(name) + len] = '\0';
	AddArg(argv, argc, str);
}

/*
 * Free an argv-style list.
 */
static void FreeArgs(char **argv, int argc)
{
	int posn;
	for(posn = 0; posn < argc; ++posn)
	{
		if(argv[posn] != 0)
		{
			ILFree(argv[posn]);
		}
	}
	if(argv)
	{
		ILFree(argv);
	}
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
 * Flag values.
 */
#define	COMP_FLAG_NOT_SET		-1
#define	COMP_FLAG_TRUE			1
#define	COMP_FLAG_FALSE			0

/*
 * Arguments for executing a C# compiler.
 */
typedef struct
{
	char		   *output;
	const char	   *target;
	int				debug;
	int				checked;
	int				unsafe;
	int				noStdLib;
	int				optimize;
	int				warnAsError;
	CSAntFileSet   *sources;
	CSAntFileSet   *references;
	CSAntFileSet   *resources;
	CSAntFileSet   *modules;
	char		  **defines;
	int				numDefines;
	char		  **args;
	int				numArgs;

} CSAntCompileArgs;

/*
 * Free a set of compile arguments.
 */
static void FreeCompileArgs(CSAntCompileArgs *args)
{
	if(args->output)
	{
		ILFree(args->output);
	}
	CSAntFileSetDestroy(args->sources);
	CSAntFileSetDestroy(args->references);
	CSAntFileSetDestroy(args->resources);
	CSAntFileSetDestroy(args->modules);
	FreeArgs(args->defines, args->numDefines);
	FreeArgs(args->args, args->numArgs);
}

/*
 * Parse compile arguments from a task block.  Returns zero on error.
 */
static int ParseCompileArgs(CSAntTask *task, CSAntCompileArgs *args,
							int isCsc, const char *compilerName)
{
	const char *value;
	const char *value2;
	char *copyValue;
	CSAntTask *node;

	/* Initialize the arguments */
	args->output = (char *)CSAntTaskParam(task, "output");
	args->target = CSAntTaskParam(task, "target");
	args->debug = COMP_FLAG_NOT_SET;
	args->checked = COMP_FLAG_NOT_SET;
	args->unsafe = COMP_FLAG_NOT_SET;
	args->noStdLib = COMP_FLAG_NOT_SET;
	args->optimize = COMP_FLAG_NOT_SET;
	args->warnAsError = COMP_FLAG_NOT_SET;
	args->sources = CSAntFileSetLoad(task, "sources");
	args->references = CSAntFileSetLoad(task, "references");
	args->resources = CSAntFileSetLoad(task, "resources");
	args->modules = CSAntFileSetLoad(task, "modules");
	args->defines = 0;
	args->numDefines = 0;
	args->args = 0;
	args->numArgs = 0;

	/* Validate "output" and "target" */
	if(!(args->output))
	{
		fprintf(stderr, "%s: no output specified\n", task->name);
		FreeCompileArgs(args);
		return 0;
	}
	args->output = CSAntDirCombine(CSAntBaseDir, args->output);
	if(!(args->target))
	{
		if(isCsc)
		{
			fprintf(stderr, "%s: no target type specified\n", task->name);
			FreeCompileArgs(args);
			return 0;
		}
		if(EndsIn(args->output, ".dll"))
		{
			args->target = "library";
		}
		else
		{
			args->target = "exe";
		}
	}

	/* We need at least one source file */
	if(CSAntFileSetSize(args->sources) == 0)
	{
		fprintf(stderr, "%s: no source files specified\n", task->name);
		FreeCompileArgs(args);
		return 0;
	}

	/* Set the flag values */
	value = CSAntTaskParam(task, "debug");
	if(value)
	{
		args->debug = !ILStrICmp(value, "true");
	}
	value = CSAntTaskParam(task, "checked");
	if(value)
	{
		args->checked = !ILStrICmp(value, "true");
	}
	value = CSAntTaskParam(task, "unsafe");
	if(value)
	{
		args->unsafe = !ILStrICmp(value, "true");
	}
	value = CSAntTaskParam(task, "nostdlib");
	if(value)
	{
		args->noStdLib = !ILStrICmp(value, "true");
	}
	value = CSAntTaskParam(task, "optimize");
	if(value)
	{
		args->optimize = !ILStrICmp(value, "true");
	}
	value = CSAntTaskParam(task, "warnaserror");
	if(value)
	{
		args->warnAsError = !ILStrICmp(value, "true");
	}

	/* Get the list of symbol definitions */
	value = CSAntTaskParam(task, "define");
	if(value)
	{
		while(*value != '\0')
		{
			if(*value == ' ' || *value == ';' || *value == ',')
			{
				++value;
				continue;
			}
			value2 = value;
			while(*value != '\0' && *value != ' ' &&
			      *value != ';' && *value != ',')
			{
				++value;
			}
			AddUnique(&(args->defines), &(args->numDefines), value2,
					  (int)(value - value2));
		}
	}
	node = task->taskChildren;
	while(node != 0)
	{
		if(!strcmp(node->name, "define"))
		{
			value = CSAntTaskParam(node, "name");
			value2 = CSAntTaskParam(node, "value");
			if(value && value2 && !ILStrICmp(value2, "true"))
			{
				AddUnique(&(args->defines), &(args->numDefines), value,
						  strlen(value));
			}
		}
		node = node->next;
	}

	/* Define "DEBUG" and "TRACE" if debugging is enabled */
	if(args->debug == COMP_FLAG_TRUE)
	{
		AddUnique(&(args->defines), &(args->numDefines), "DEBUG", 5);
		AddUnique(&(args->defines), &(args->numDefines), "TRACE", 5);
	}

	/* Collect up additional arguments */
	node = task->taskChildren;
	while(node != 0)
	{
		if(!strcmp(node->name, "arg"))
		{
			/* Does this argument only apply to a specific compiler? */
			value = CSAntTaskParam(node, "compiler");
			if(!value || !ILStrICmp(value, compilerName))
			{
				value = CSAntTaskParam(node, "value");
				if(value)
				{
					copyValue = ILDupString(value);
					if(!copyValue)
					{
						CSAntOutOfMemory();
					}
					AddArg(&(args->args), &(args->numArgs), copyValue);
				}
			}
		}
		node = node->next;
	}

	/* Done */
	return 1;
}

/*
 * Build a command-line for "cscc".
 */
static char **BuildCsccCommandLine(CSAntCompileArgs *args)
{
	char **argv = 0;
	int argc = 0;
	int posn;
	unsigned long numFiles;
	unsigned long file;
	char *temp;
	int len;

	/* Add the program name */
	AddArg(&argv, &argc, FindPNetPath());

	/* Set the output file */
	AddArg(&argv, &argc, "-o");
	AddArg(&argv, &argc, (char *)(args->output));

	/* Enable debugging if necessary */
	if(args->debug == COMP_FLAG_TRUE)
	{
		AddArg(&argv, &argc, "-g");
	}

	/* Set the checked compilation state */
	if(args->checked == COMP_FLAG_TRUE)
	{
		AddArg(&argv, &argc, "-fchecked");
	}
	else if(args->checked == COMP_FLAG_FALSE)
	{
		AddArg(&argv, &argc, "-funchecked");
	}

	/* Set the unsafe compilation state */
	if(args->unsafe == COMP_FLAG_TRUE)
	{
		AddArg(&argv, &argc, "-funsafe");
	}

	/* Disable the standard library if necessary */
	if(args->noStdLib == COMP_FLAG_TRUE)
	{
		AddArg(&argv, &argc, "-nostdlib");
	}

	/* Set the optimization level */
	if(args->optimize == COMP_FLAG_TRUE)
	{
		AddArg(&argv, &argc, "-O2");
	}
	else if(args->optimize == COMP_FLAG_FALSE)
	{
		AddArg(&argv, &argc, "-O0");
	}

	/* Convert warnings into errors if requested */
	if(args->warnAsError == COMP_FLAG_TRUE)
	{
		AddArg(&argv, &argc, "-Werror");
	}

	/* Define pre-processor symbols */
	for(posn = 0; posn < args->numDefines; ++posn)
	{
		AddValueArg(&argv, &argc, "-D", args->defines[posn]);
	}

	/* Add any extra arguments that were supplied */
	for(posn = 0; posn < args->numArgs; ++posn)
	{
		AddArg(&argv, &argc, args->args[posn]);
	}

	/* Add the resources to the command-line */
	numFiles = CSAntFileSetSize(args->resources);
	for(file = 0; file < numFiles; ++file)
	{
		AddValueArg(&argv, &argc, "-fresources=",
					CSAntFileSetFile(args->resources, file));
	}

	/* Add the modules to the command-line */
	numFiles = CSAntFileSetSize(args->modules);
	for(file = 0; file < numFiles; ++file)
	{
		AddValueArg(&argv, &argc, "-fmodule=",
					CSAntFileSetFile(args->modules, file));
	}

	/* Add the source files to the command-line */
	numFiles = CSAntFileSetSize(args->sources);
	for(file = 0; file < numFiles; ++file)
	{
		AddArg(&argv, &argc, CSAntFileSetFile(args->sources, file));
	}

	/* Add the library references to the command-line */
	numFiles = CSAntFileSetSize(args->references);
	for(file = 0; file < numFiles; ++file)
	{
		temp = CSAntFileSetFile(args->references, file);
		len = strlen(temp);
		while(len > 0 && temp[len - 1] != '/' && temp[len - 1] != '\\')
		{
			--len;
		}
		if(len > 0)
		{
			if(len == 1)
			{
				AddValueLenArg(&argv, &argc, "-L", temp, 1);
			}
			else
			{
				AddValueLenArg(&argv, &argc, "-L", temp, len - 1);
			}
		}
		if(EndsIn(temp + len, ".dll"))
		{
			AddValueLenArg(&argv, &argc, "-l", temp + len,
						   strlen(temp + len) - 4);
		}
		else
		{
			AddValueArg(&argv, &argc, "-l", temp + len);
		}
	}

	/* Terminate the command-line */
	AddArg(&argv, &argc, (char *)0);

	/* Done */
	return argv;
}

/*
 * Build a command-line for "csc".
 */
static char **BuildCscCommandLine(CSAntCompileArgs *args)
{
	char **argv = 0;
	int argc = 0;
	int posn, len, len2;
	unsigned long numFiles;
	unsigned long file;
	char *temp;
	char *temp2;

	/* Add the program name and fixed options */
	AddArg(&argv, &argc, FindMSPath());
	AddArg(&argv, &argc, "/nologo");

	/* Enable debugging if necessary */
	if(args->debug == COMP_FLAG_TRUE)
	{
		AddArg(&argv, &argc, "/debug");
	}

	/* Set the checked compilation state */
	if(args->checked == COMP_FLAG_TRUE)
	{
		AddArg(&argv, &argc, "/checked+");
	}
	else if(args->checked == COMP_FLAG_FALSE)
	{
		AddArg(&argv, &argc, "/checked-");
	}

	/* Set the unsafe compilation state */
	if(args->unsafe == COMP_FLAG_TRUE)
	{
		AddArg(&argv, &argc, "/unsafe");
	}

	/* Disable the standard library if necessary */
	if(args->noStdLib == COMP_FLAG_TRUE)
	{
		AddArg(&argv, &argc, "/nostdlib");
		AddArg(&argv, &argc, "/noconfig");
	}

	/* Set the optimization level */
	if(args->optimize == COMP_FLAG_TRUE)
	{
		AddArg(&argv, &argc, "/optimize+");
	}
	else if(args->optimize == COMP_FLAG_FALSE)
	{
		AddArg(&argv, &argc, "/optimize-");
	}

	/* Convert warnings into errors if requested */
	if(args->warnAsError == COMP_FLAG_TRUE)
	{
		AddArg(&argv, &argc, "/warnaserror+");
	}

	/* Set the target and output file */
	AddValueArg(&argv, &argc, "/target:", (char *)(args->target));
	temp = CSAntDirCombineWin32(args->output, 0, 0);
	AddValueArg(&argv, &argc, "/out:", temp);
	ILFree(temp);

	/* Define pre-processor symbols */
	temp = 0;
	for(posn = 0; posn < args->numDefines; ++posn)
	{
		temp = SemiColonList(temp, args->defines[posn]);
	}
	if(temp != 0)
	{
		AddValueArg(&argv, &argc, "/define:", temp);
	}

	/* Add the resources to the command-line */
	numFiles = CSAntFileSetSize(args->resources);
	for(file = 0; file < numFiles; ++file)
	{
		temp = CSAntFileSetFile(args->resources, file);
		len = len2 = strlen(temp);
		while(len > 0 && temp[len - 1] != '/' && temp[len - 1] != '\\')
		{
			--len;
		}
		if(len > 0)
		{
			temp2 = (char *)ILMalloc(len2 + (len2 - len) + 2);
			if(!temp2)
			{
				CSAntOutOfMemory();
			}
			strcpy(temp2, temp);
			strcat(temp2, ",");
			strcat(temp2, temp + len);
			AddValueArg(&argv, &argc, "/resource:", temp2);
			ILFree(temp2);
		}
		else
		{
			AddValueArg(&argv, &argc, "/resource:", temp);
		}
	}

	/* Add the references to the command-line */
	numFiles = CSAntFileSetSize(args->references);
	temp = 0;
	for(file = 0; file < numFiles; ++file)
	{
		temp = SemiColonList(temp,
					CSAntDirCombineWin32
						(CSAntFileSetFile(args->references, file), 0, 0));
	}
	if(temp != 0)
	{
		AddValueArg(&argv, &argc, "/reference:", temp);
	}

	/* Add the modules to the command-line */
	numFiles = CSAntFileSetSize(args->modules);
	temp = 0;
	for(file = 0; file < numFiles; ++file)
	{
		temp = SemiColonList(temp,
					CSAntDirCombineWin32
						(CSAntFileSetFile(args->modules, file), 0, 0));
	}
	if(temp != 0)
	{
		AddValueArg(&argv, &argc, "/addmodule:", temp);
	}

	/* Add any extra arguments that were supplied */
	for(posn = 0; posn < args->numArgs; ++posn)
	{
		AddArg(&argv, &argc, args->args[posn]);
	}

	/* Add the source files to the command-line */
	numFiles = CSAntFileSetSize(args->sources);
	for(file = 0; file < numFiles; ++file)
	{
		AddArg(&argv, &argc,
			   CSAntDirCombineWin32
			   		(CSAntFileSetFile(args->sources, file), 0, 0));
	}

	/* Terminate the command-line */
	AddArg(&argv, &argc, (char *)0);

	/* Done */
	return argv;
}

/*
 * Build a command-line for "mcs".
 */
static char **BuildMcsCommandLine(CSAntCompileArgs *args)
{
	char **argv = 0;
	int argc = 0;
	int posn;
	unsigned long numFiles;
	unsigned long file;
	char *temp;
	int len;

	/* Add the program name */
	AddArg(&argv, &argc, FindMonoPath());

	/* Set the target and output file */
	AddArg(&argv, &argc, "--target");
	AddArg(&argv, &argc, (char *)(args->target));
	AddArg(&argv, &argc, "-o");
	AddArg(&argv, &argc, (char *)(args->output));

	/* Set the checked compilation state */
	if(args->checked == COMP_FLAG_TRUE)
	{
		AddArg(&argv, &argc, "--checked");
	}

	/* Disable the standard library if necessary */
	if(args->noStdLib == COMP_FLAG_TRUE)
	{
		AddArg(&argv, &argc, "--nostdlib");
	}

	/* Add any extra arguments that were supplied */
	for(posn = 0; posn < args->numArgs; ++posn)
	{
		AddArg(&argv, &argc, args->args[posn]);
	}

	/* Add the source files to the command-line */
	numFiles = CSAntFileSetSize(args->sources);
	for(file = 0; file < numFiles; ++file)
	{
		AddArg(&argv, &argc, CSAntFileSetFile(args->sources, file));
	}

	/* Add the library references to the command-line */
	numFiles = CSAntFileSetSize(args->references);
	for(file = 0; file < numFiles; ++file)
	{
		temp = CSAntFileSetFile(args->references, file);
		len = strlen(temp);
		while(len > 0 && temp[len - 1] != '/' && temp[len - 1] != '\\')
		{
			--len;
		}
		if(len > 0)
		{
			if(len == 1)
			{
				AddArg(&argv, &argc, "-L");
				AddValueLenArg(&argv, &argc, "", temp, 1);
			}
			else
			{
				AddArg(&argv, &argc, "-L");
				AddValueLenArg(&argv, &argc, "", temp, len - 1);
			}
		}
		AddArg(&argv, &argc, "-r");
		AddArg(&argv, &argc, temp + len);
	}

	/* Terminate the command-line */
	AddArg(&argv, &argc, (char *)0);

	/* Done */
	return argv;
}

/*
 * Build a command-line and execute it.
 */
static int BuildAndExecute(CSAntCompileArgs *args,
						   char **(*func)(CSAntCompileArgs *))
{
	char **argv;
	int argc;
	int result;

	/* Check the timestamps on the input and output files */
	if(!CSAntFileSetNewer(args->sources, args->output) &&
	   !CSAntFileSetNewer(args->references, args->output) &&
	   !CSAntFileSetNewer(args->resources, args->output) &&
	   !CSAntFileSetNewer(args->modules, args->output))
	{
		return 1;
	}

	/* Build the command-line using the supplied function */
	argv = (*func)(args);

	/* Print the command to be executed */
	if(!CSAntSilent)
	{
		argc = 0;
		while(argv[argc] != 0)
		{
			fputs(argv[argc], stdout);
			++argc;
			if(argv[argc] != 0)
			{
				putc(' ', stdout);
			}
		}
		putc('\n', stdout);
	}

	/* Execute the command */
	if(!CSAntJustPrint)
	{
		result = (ILSpawnProcess(argv) == 0);
	}
	else
	{
		result = 1;
	}

	/* Clean up and exit */
	ILFree(argv);
	FreeCompileArgs(args);
	return result;
}

/*
 * Handle a "cscc" task, which invokes the Portable.NET C# compiler.
 */
int CSAntTask_Cscc(CSAntTask *task)
{
	CSAntCompileArgs args;

	/* Parse the arguments */
	if(!ParseCompileArgs(task, &args, 0, "cscc"))
	{
		return 0;
	}

	/* Execute the command */
	return BuildAndExecute(&args, BuildCsccCommandLine);
}

/*
 * Handle a "csc" task, which invokes the Microsoft C# compiler.
 */
int CSAntTask_Csc(CSAntTask *task)
{
	CSAntCompileArgs args;

	/* Parse the arguments */
	if(!ParseCompileArgs(task, &args, 1, "csc"))
	{
		return 0;
	}

	/* Execute the command */
	return BuildAndExecute(&args, BuildCscCommandLine);
}

/*
 * Handle a "mcs" task, which invokes the Mono C# compiler.
 */
int CSAntTask_Mcs(CSAntTask *task)
{
	CSAntCompileArgs args;

	/* Parse the arguments */
	if(!ParseCompileArgs(task, &args, 0, "msc"))
	{
		return 0;
	}

	/* Execute the command */
	return BuildAndExecute(&args, BuildMcsCommandLine);
}

/*
 * Handle a "compile" task, which invokes the configured C# compiler.
 */
int CSAntTask_Compile(CSAntTask *task)
{
	CSAntCompileArgs args;
	const char *compiler = CSAntGetProperty("csant.compiler", -1);

	/* Parse the arguments */
	if(!ParseCompileArgs(task, &args, 0, compiler))
	{
		return 0;
	}

	/* Execute the command */
	if(!ILStrICmp(compiler, "cscc"))
	{
		return BuildAndExecute(&args, BuildCsccCommandLine);
	}
	else if(!ILStrICmp(compiler, "csc"))
	{
		return BuildAndExecute(&args, BuildCscCommandLine);
	}
	else if(!ILStrICmp(compiler, "mcs"))
	{
		return BuildAndExecute(&args, BuildMcsCommandLine);
	}
	else
	{
		fprintf(stderr, "%s: unknown compiler name\n", compiler);
		return 0;
	}
}

#ifdef	__cplusplus
};
#endif
