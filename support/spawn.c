/*
 * spawn.c - Spawn child processes.
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

#include "il_system.h"
#include "il_utils.h"
#include <stdio.h>
#ifdef _WIN32
	#include <windows.h>
	#include <process.h>
#else
	#ifdef HAVE_SYS_TYPES_H
		#include <sys/types.h>
	#endif
	#ifdef HAVE_UNISTD_H
		#include <unistd.h>
	#endif
	#ifdef HAVE_SYS_WAIT_H
		#include <sys/wait.h>
	#endif
	#ifndef WEXITSTATUS
		#define	WEXITSTATUS(status)		((unsigned)(status) >> 8)
	#endif
	#ifndef WIFEXITED
		#define	WIFEXITED(status)		(((status) & 255) == 0)
	#endif
#endif

#ifdef	__cplusplus
extern	"C" {
#endif

#ifdef	_WIN32

/*
 * Use Windows-specific functions to spawn child processes.
 */
int ILSpawnProcess(char *argv[])
{
	int arg;
	int len;
	char *newargv[3];
	char *tempfile = 0;
	int status;
	char temppath[MAX_PATH];
	char tempname[MAX_PATH + 3];
	FILE *file;

	/* Scan the command-line to see if it may be too big to
	   fit in the system's command-line buffer.  If so, we
	   must write the options to a temporary file instead */
	arg = 1;
	len = 0;
	while(argv[arg] != 0)
	{
		if(strchr(argv[arg], ' ') != 0)
		{
			/* We must quote this argument: always put it in the option file,
			   so that the options don't get messed up by Microsoft's version
			   of "spawn*", which doesn't quote arguments with spaces */
			len = 200;
			break;
		}
		else if(argv[arg][0] == '\0' || argv[arg][0] == '@')
		{
			/* Quote arguments that are empty or start with '@' */
			len = 200;
			break;
		}
		else
		{
			/* This argument doesn't need quoting */
			len += strlen(argv[arg]);
		}
		++len;
		++arg;
	}

	/* Is the command-line too big?  Theoretically, modern versions
	   of Windows can handle command-lines of 1024 characters.
	   However, older versions could only handle 128.  So, we will
	   be paranoid and use the lower limit instead, and also leave
	   a few extra characters spare just in case */
	if(len >= 120)
	{
		/* Create the temporary option file */
		if(!GetTempPath(sizeof(temppath), temppath))
		{
			return -1;
		}
		if(!GetTempFileName(temppath, "CMD", 0, tempname + 2))
		{
			return -1;
		}
		strcpy(temppath, tempname + 2);
		tempname[0] = '@';
		tempname[1] = '"';
		strcat(tempname, "\"");
		tempfile = temppath;
		if((file = fopen(tempfile, "w")) == NULL)
		{
			return -1;
		}
		arg = 1;
		while(argv[arg] != 0)
		{
			if(argv[arg][0] == '\0' || argv[arg][0] == '@' ||
			   strchr(argv[arg], ' ') != 0)
			{
				putc('"', file);
				fputs(argv[arg], file);
				putc('"', file);
			}
			else
			{
				fputs(argv[arg], file);
			}
			putc('\n', file);
			++arg;
		}
		fclose(file);

		/* Set up a new argument array */
		newargv[0] = argv[0];
		newargv[1] = tempname;
		newargv[2] = 0;
		argv = newargv;
	}

	/* Spawn the child process and wait for it to exit */
	status = spawnvp(_P_WAIT, argv[0], (const char * const *)argv);

	/* Delete the temporary option file */
	if(tempfile)
	{
		DeleteFile(tempfile);
	}

	/* Done */
	return status;
}

#else
#if defined(HAVE_FORK) && defined(HAVE_EXECV) && (defined(HAVE_WAITPID) || defined(HAVE_WAIT))

/*
 * Use Unix-specific functions to spawn child processes.
 */
int ILSpawnProcess(char *argv[])
{
	int pid = fork();
	if(pid < 0)
	{
		/* Could not fork the child process */
		perror("fork");
		return -1;
	}
	else if(pid == 0)
	{
		/* We are in the child process */
		execv(argv[0], argv);
		perror(argv[0]);
		exit(1);
		return -1;		/* Keep the compiler happy */
	}
	else
	{
		/* We are in the parent process */
		int status = 1;
	#ifdef HAVE_WAITPID
		waitpid(pid, &status, 0);
	#else
		int result;
		while((result = wait(&status)) != pid && result != -1)
		{
			/* Some other child fell: not the one we are interested in */
		}
	#endif
		if(WIFEXITED(status))
		{
			/* Return the child's exit status as the final status */
			return WEXITSTATUS(status);
		}
		else
		{
			/* Some kind of signal occurred */
			return -1;
		}
	}
}

#else

/*
 * We don't know how to spawn child processes on this system.
 */

int ILSpawnProcess(char *argv[])
{
	fputs("Don't know how to spawn child processes on this system\n", stderr);
	return -1;
}

#endif
#endif

#ifdef	__cplusplus
};
#endif
