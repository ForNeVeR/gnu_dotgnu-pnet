/*
 * csant_task.c - Dispatch functions for csant task elements.
 *
 * Copyright (C) 2001, 2002, 2003  Southern Storm Software, Pty Ltd.
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
#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif
#ifdef HAVE_SYS_STAT_H
#include <sys/stat.h>
#endif
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#include <errno.h>

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
 * Delete a file from the build (for "clean" targets mainly)
 */
static int Task_Delete(CSAntTask *task)
{
	const char *file = CSAntTaskParam(task, "file");
	const char *fail = CSAntTaskParam(task, "failonerror");
	int retval;
	if(!file)
	{
		fprintf(stderr, "no file to delete in <delete>\n");
		return 0;
	}
	file = CSAntDirCombine(CSAntBaseBuildDir, file);
	if(!CSAntSilent)
	{
		if(!fail || ILStrICmp(fail, "true") != 0)
		{
			printf("rm -f %s\n", file);
		}
		else
		{
			printf("rm %s\n", file);
		}
	}
	retval = ILDeleteFile(file);
	if(retval && fail && !ILStrICmp(fail, "true"))
	{
		perror(file);
		return 0;
	}
	return 1;
}

/*
 * Make a directory.
 */
static int Task_Mkdir(CSAntTask *task)
{
	const char *dir = CSAntTaskParam(task, "dir");
	int retval;
	if(!dir)
	{
		fprintf(stderr, "no directory to make in <mkdir>\n");
		return 0;
	}
	dir = CSAntDirCombine(CSAntBaseBuildDir, dir);
	if(!CSAntSilent)
	{
		printf("mkdir %s\n", dir);
	}
#ifdef IL_WIN32_NATIVE
	retval = mkdir(dir);
#else
	retval = mkdir(dir, 0777);
#endif
	if(retval && errno != EEXIST)
	{
		perror(dir);
		return 0;
	}
	return 1;
}

/*
 * Copy a file.
 */
static int Task_Copy(CSAntTask *task)
{
	const char *fromfile = CSAntTaskParam(task, "file");
	const char *tofile = CSAntTaskParam(task, "tofile");
	FILE *instream;
	FILE *outstream;
	char buffer[BUFSIZ];
	int len;

	/* Validate the parameters */
	if(!fromfile)
	{
		fprintf(stderr, "no source file in <copy>\n");
		return 0;
	}
	if(!tofile)
	{
		fprintf(stderr, "no destination file in <copy>\n");
		return 0;
	}
	fromfile = CSAntDirCombine(CSAntBaseBuildDir, fromfile);
	tofile = CSAntDirCombine(CSAntBaseBuildDir, tofile);

	/* Report the command that we will be executing */
	if(!CSAntSilent)
	{
		printf("cp %s %s\n", fromfile, tofile);
	}

	/* Attempt to open the source file */
	if((instream = fopen(fromfile, "rb")) == NULL)
	{
		if((instream = fopen(fromfile, "r")) == NULL)
		{
			perror(fromfile);
			return 0;
		}
	}

	/* Attempt to open the destination file */
	if((outstream = fopen(tofile, "wb")) == NULL)
	{
		if((outstream = fopen(tofile, "w")) == NULL)
		{
			perror(tofile);
			fclose(instream);
			return 0;
		}
	}

	/* Copy the file's contents */
	while((len = (int)fread(buffer, 1, BUFSIZ, instream)) >= BUFSIZ)
	{
		fwrite(buffer, 1, len, outstream);
	}
	if(len > 0)
	{
		fwrite(buffer, 1, len, outstream);
	}
	if(ferror(outstream))
	{
		perror(tofile);
		fclose(instream);
		fclose(outstream);
		ILDeleteFile(tofile);
		return 0;
	}

	/* Done */
	fclose(instream);
	fclose(outstream);
	return 1;
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
	char *argv[40];
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
	if(CSAntRedirectCsc)
	{
		argv[argc++] = "-c";
	}
	if(CSAntForceCorLib)
	{
		argv[argc++] = "-m";
	}
	if(CSAntInstallMode)
	{
		argv[argc++] = "-i";
	}
	if(CSAntUninstallMode)
	{
		argv[argc++] = "-u";
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
	{"compile",			1, CSAntTask_Compile},
	{"cscc",			1, CSAntTask_Cscc},
	{"csc",				1, CSAntTask_Csc},
	{"mcs",				1, CSAntTask_Mcs},
	{"csdoc",			0, CSAntTask_Csdoc},
	{"resgen",			0, CSAntTask_ResGen},
	{"reslink",			1, CSAntTask_ResLink},
	{"property",		1, Task_Property},
	{"echo",			0, Task_Echo},
	{"fail",			0, Task_Fail},
	{"delete",			0, Task_Delete},
	{"mkdir",			0, Task_Mkdir},
	{"copy",			0, Task_Copy},
	{"csant",			1, Task_CSAnt},
	{"nant",			1, Task_CSAnt},
};
int const CSAntNumTasks = (sizeof(CSAntTasks) / sizeof(CSAntTaskInfo));

#ifdef	__cplusplus
};
#endif
