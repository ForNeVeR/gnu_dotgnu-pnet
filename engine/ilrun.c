/*
 * ilrun.c - Command-line version of the runtime engine.
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

#include "il_engine.h"
#include "il_image.h"
#include "il_utils.h"

#ifdef	__cplusplus
extern	"C" {
#endif

/*
 * Table of command-line options.
 */
static ILCmdLineOption const options[] = {
	{"--version", 'v', 0,
		"--version or -v",
		"Print the version of the program"},
	{"--help", 'h', 0,
		"--help",
		"Print this help message."},
	{0, 0, 0, 0, 0}
};

static void usage(const char *progname);
static void version(void);

int main(int argc, char *argv[])
{
	char *progname = argv[0];
	int state, opt;
	char *param;
	ILExecProcess *process;
	ILMethod *method;
	int error;
	ILInt32 retval;

	/* Parse the command-line arguments */
	state = 0;
	while((opt = ILCmdLineNextOption(&argc, &argv, &state,
									 options, &param)) != 0)
	{
		switch(opt)
		{
			case 'v':
			{
				version();
				return 0;
			}
			/* Not reached */

			default:
			{
				usage(progname);
				return 1;
			}
			/* Not reached */
		}
	}

	/* We need at least one input file argument */
	if(argc <= 1)
	{
		usage(progname);
		return 1;
	}

	/* Create a process to load the program into */
	process = ILExecProcessCreate();
	if(!process)
	{
		fprintf(stderr, "%s: could not create process\n", progname);
		return 1;
	}

	/* Attempt to load the program into the process */
	error = ILExecProcessLoadFile(process, argv[1]);
	if(error < 0)
	{
		perror(argv[1]);
		return 1;
	}
	else if(error > 0)
	{
		fprintf(stderr, "%s: %s\n", argv[1], ILImageLoadError(error));
		return 1;
	}

	/* Find the entry point method */
	method = ILExecProcessGetEntry(process);
	if(!method)
	{
		fprintf(stderr, "%s: no program entry point\n", argv[1]);
		ILExecProcessDestroy(process);
		return 1;
	}

	/* Call the entry point */
	retval = 0;
	ILExecThreadCall(ILExecProcessGetMain(process), method,
					 &retval, (void *)0);

	/* Clean up the process and exit */
	error = ILExecProcessGetStatus(process);
	ILExecProcessDestroy(process);
	return (int)retval;
}

static void usage(const char *progname)
{
	fprintf(stderr, "ILRUN " VERSION " - IL Program Runtime\n");
	fprintf(stderr, "Copyright (c) 2001 Southern Storm Software, Pty Ltd.\n");
	fprintf(stderr, "\n");
	fprintf(stderr, "Usage: %s [options] program [args]\n", progname);
	fprintf(stderr, "\n");
	ILCmdLineHelp(options);
}

static void version(void)
{
	printf("ILRUN " VERSION " - IL Program Runtime\n");
	printf("Copyright (c) 2001 Southern Storm Software, Pty Ltd.\n");
	printf("\n");
	printf("ILRUN comes with ABSOLUTELY NO WARRANTY.  This is free software,\n");
	printf("and you are welcome to redistribute it under the terms of the\n");
	printf("GNU General Public License.  See the file COPYING for further details.\n");
	printf("\n");
	printf("Use the `--help' option to get help on the command-line options.\n");
}

#ifdef	__cplusplus
};
#endif
