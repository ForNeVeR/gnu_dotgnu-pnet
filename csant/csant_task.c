/*
 * csant_task.c - Dispatch functions for csant task elements.
 *
 * Copyright (C) 2001, 2002  Southern Storm Software, Pty Ltd.
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

/*
 * Handle a property set task.
 */
static int Task_Property(CSAntTask *task)
{
	const char *name;
	const char *value;
	name = CSAntTaskParam(task, "name");
	value = CSAntTaskParam(task, "value");
	if(!name)
	{
		fprintf(stderr, "<property> element with no property name\n");
		return 0;
	}
	if(!value)
	{
		fprintf(stderr, "%s: no value specified for property\n", name);
	}
	CSAntDefineProperty(name, -1, value, 0);
	return 1;
}

/*
 * Echo a message to stdout.
 */
static int Task_Echo(CSAntTask *task)
{
	const char *msg = CSAntTaskParam(task, "message");
	if(!msg)
	{
		fprintf(stderr, "<echo> element with no message\n");
		return 0;
	}
	puts(msg);
	return 1;
}

/*
 * Echo a message to stdout and fail the build.
 */
static int Task_Fail(CSAntTask *task)
{
	const char *msg = CSAntTaskParam(task, "message");
	if(msg)
	{
		puts(msg);
	}
	return CSAntJustPrint;
}

/*
 * Invoke a sub-process containing another invocation of "csant".
 */
static int Task_CSAnt(CSAntTask *task)
{
	char *baseSrcDir;
	char *baseBuildDir;
	const char *buildFile;
	const char *target;
	const char *compiler;
	char *argv[20];
	int argc, posn;
	int result;

	/* Adjust the base directories if necessary */
	baseSrcDir = CSAntDirCombine
		(CSAntBaseSrcDir, CSAntTaskParam(task, "basedir"));
	baseBuildDir = CSAntDirCombine
		(CSAntBaseBuildDir, CSAntTaskParam(task, "basedir"));

	/* Locate the new build file and target */
	buildFile = CSAntTaskParam(task, "buildfile");
	target = CSAntTaskParam(task, "target");

	/* Locate the compiler, which may have been overridden by properties */
	compiler = CSAntGetProperty("csant.compiler", -1);

	/* Construct the command-line to be spawned */
	argv[0] = CSAntGetProgramName();
	argc = 1;
	if(baseSrcDir)
	{
		argv[argc++] = "-b";
		argv[argc++] = baseSrcDir;
	}
	if(baseBuildDir)
	{
		argv[argc++] = "-B";
		argv[argc++] = baseBuildDir;
	}
	if(buildFile)
	{
		argv[argc++] = "-f";
		argv[argc++] = (char *)buildFile;
	}
	if(CSAntKeepGoing)
	{
		argv[argc++] = "-k";
	}
	if(CSAntJustPrint)
	{
		argv[argc++] = "-n";
	}
	if(CSAntSilent)
	{
		argv[argc++] = "-s";
	}
	argv[argc++] = "-C";
	argv[argc++] = (char *)compiler;
	if(target)
	{
		argv[argc++] = (char *)target;
	}
	argv[argc] = 0;

	/* Print the spawn command-line */
	if(!CSAntSilent)
	{
		for(posn = 0; posn < argc; ++posn)
		{
			fputs(argv[posn], stdout);
			if(posn < (argc - 1))
			{
				putc(' ', stdout);
			}
		}
		putc('\n', stdout);
	}

	/* Spawn the sub-process */
	result = (ILSpawnProcess(argv) == 0);

	/* Clean up and exit */
	ILFree(baseSrcDir);
	ILFree(baseBuildDir);
	return result;
}

CSAntTaskInfo const CSAntTasks[] = {
	{"compile",			CSAntTask_Compile},
	{"cscc",			CSAntTask_Cscc},
	{"csc",				CSAntTask_Csc},
	{"mcs",				CSAntTask_Mcs},
	{"csdoc",			CSAntTask_Csdoc},
	{"property",		Task_Property},
	{"echo",			Task_Echo},
	{"fail",			Task_Fail},
	{"csant",			Task_CSAnt},
};
int const CSAntNumTasks = (sizeof(CSAntTasks) / sizeof(CSAntTaskInfo));

#ifdef	__cplusplus
};
#endif
