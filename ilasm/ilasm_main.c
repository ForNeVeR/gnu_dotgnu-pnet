/*
 * ilasm_main.c - Main entry point for "ilasm".
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
#include "il_utils.h"
#include "ilasm_build.h"
#include "ilasm_output.h"

#ifdef	__cplusplus
extern	"C" {
#endif

/*
 * Table of command-line options.
 */
static ILCmdLineOption const options[] = {
	{"-o", 'o', 1,
		"--output file     or    -o file",
		"Specify the output file to use.  The default is\n"
		"`input.EXT' where EXT depends upon `--format'."},
	{"-e", 'e', 0,
		"--format exe      or    -e",
		"Set the output format to executable."},
	{"-d", 'd', 0,
		"--format dll      or    -d",
		"Set the output format to dynamic link library."},
	{"-j", 'j', 0,
		"--format obj      or    -j",
		"Set the output format to object file."},
	{"-g", 'g', 0, 0, 0},
	{"-n", 'n', 0, 0, 0},
	{"-s", 's', 0, 0, 0},
	{"-l", 'l', 0, 0, 0},
	{"-3", '3', 0, 0, 0},
	{"-c", 'c', 0, 0, 0},
	{"-G", 'G', 0, 0, 0},
	{"-m", 'm', 0, 0, 0},
	{"-r", 'r', 1, 0, 0},
	{"-v", 'v', 0, 0, 0},
	{"--output", 'o', 1, 0, 0},
	{"--format", 'f', 1, 0, 0},
	{"--debug", 'g', 0,
		"--debug           or    -g",
		"Write debug information to the output file."},
	{"--no-debug", 'n', 0,
		"--no-debug        or    -n",
		"Suppress debug information in the output file."},
	{"--short-insns", 's', 0,
		"--short-insns     or    -s",
		"Use shorter instructions if possible (default)."},
	{"--no-short-insns", 'l', 0,
		"--no-short-insns  or    -l",
		"Use the stated version of an instruction."},
	{"--32bit-only", '3', 0,
		"--32bit-only      or    -3",
		"The resulting output file can only be used on 32-bit systems."},
	{"--cui-subsystem", 'c', 0,
		"--cui-subsystem   or    -c",
		"Compile for the command-line subsystem (default)."},
	{"--gui-subsystem", 'G', 0,
		"--gui-subsystem   or    -G",
		"Compile for the GUI subsystem."},
	{"--res", 'r', 0,
		"--res file        or    -r file",
		"Link the specified resource file with the output."},
	{"-D", 'D', 0, 0, 0},		/* Enable debugging of yacc parser */
	{"--version", 'v', 0,
		"--version         or    -v",
		"Print the version of the program"},
	{"--help", 'h', 0,
		"--help",
		"Print this help message."},
	{0, 0, 0, 0, 0}
};

/*
 * Yacc entry points for the ILASM parser.
 */
extern int ilasm_parse(void);
extern void ilasm_restart(FILE *infile);
extern int ilasm_debug;

/*
 * Global state used by the Lex scanner and the Yacc parser.
 */
extern char *ILAsmFilename;
extern long  ILAsmLineNum;
extern int   ILAsmErrors;
extern int   ILAsmParseHexBytes;

/*
 * Code generation flags.
 */
int ILAsmShortInsns = 0;

static void usage(const char *progname);
static void version(void);

int main(int argc, char *argv[])
{
	char *progname = argv[0];
	char *outputFile = 0;
	char *resourceFile = 0;
	int format = -1;
	int debug = 0;
	int sawStdin;
	int state, opt, len;
	char *param;
	FILE *infile;
	FILE *outfile;
	int flags = IL_WRITEFLAG_SUBSYS_CUI;

	/* Parse the command-line arguments */
	state = 0;
	while((opt = ILCmdLineNextOption(&argc, &argv, &state,
									 options, &param)) != 0)
	{
		switch(opt)
		{
			case 'o':
			{
				outputFile = param;
			}
			break;

			case 'f':
			{
				if(!strcmp(param, "exe"))
				{
					format = IL_IMAGETYPE_EXE;
				}
				else if(!strcmp(param, "dll"))
				{
					format = IL_IMAGETYPE_DLL;
				}
				else if(!strcmp(param, "obj"))
				{
					format = IL_IMAGETYPE_OBJ;
				}
				else
				{
					usage(progname);
					return 1;
				}
			}
			break;

			case 'e':
			{
				format = IL_IMAGETYPE_EXE;
			}
			break;

			case 'd':
			{
				format = IL_IMAGETYPE_DLL;
			}
			break;

			case 'j':
			{
				format = IL_IMAGETYPE_OBJ;
			}
			break;

			case 'g':
			{
				debug = 1;
			}
			break;

			case 'n':
			{
				debug = 0;
			}
			break;

			case 'r':
			{
				resourceFile = param;
			}
			break;

			case 's':
			{
				ILAsmShortInsns = 1;
			}
			break;

			case 'l':
			{
				ILAsmShortInsns = 0;
			}
			break;

			case '3':
			{
				flags |= IL_WRITEFLAG_32BIT_ONLY;
			}
			break;

			case 'c':
			{
				flags &= ~IL_WRITEFLAG_SUBSYS_GUI;
			}
			break;

			case 'G':
			{
				flags |= IL_WRITEFLAG_SUBSYS_GUI;
			}
			break;

			case 'D':
			{
				ilasm_debug = 1;
			}
			break;

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

	/* We need at least one source file argument */
	if(argc <= 1)
	{
		usage(progname);
		return 1;
	}

	/* Determine the default output format from the output file extension */
	if(format == -1)
	{
		format = IL_IMAGETYPE_OBJ;
		if(outputFile)
		{
			len = strlen(outputFile);
			while(len > 0 && outputFile[len - 1] != '/' &&
			      outputFile[len - 1] != '\\' &&
				  outputFile[len - 1] != '.')
			{
				--len;
			}
			if(len > 0 && outputFile[len - 1] == '.')
			{
				if(!ILStrICmp(outputFile + len, "exe"))
				{
					format = IL_IMAGETYPE_EXE;
				}
				else if(!ILStrICmp(outputFile + len, "dll"))
				{
					format = IL_IMAGETYPE_DLL;
				}
			}
		}
	}

	/* If we don't have an output filename, then synthesize one */
	if(!outputFile)
	{
		ILIntString ext;
		ILIntString base;
		int len;
		if(format == IL_IMAGETYPE_OBJ)
		{
			ext.string = ".obj";
		}
		else if(format == IL_IMAGETYPE_DLL)
		{
			ext.string = ".dll";
		}
		else
		{
			ext.string = ".exe";
		}
		ext.len = 4;
		len = strlen(argv[1]);
		while(len > 0 && argv[1][len - 1] != '.' &&
		      argv[1][len - 1] != '/' && argv[1][len - 1] != '\\')
		{
			--len;
		}
		if(len <= 0 || argv[1][len - 1] != '.')
		{
			len = strlen(argv[1]);
		}
		else
		{
			--len;
		}
		base.string = argv[1];
		base.len = len;
		outputFile = (ILInternAppendedString(base, ext)).string;
	}

	/* Initialize the image builders and open the output stream */
	if(!strcmp(outputFile, "-"))
	{
		ILAsmBuildInit("stdout");
		ILAsmOutCreate(stdout, 0, format, flags);
		outfile = 0;
	}
	else
	{
		ILAsmBuildInit(outputFile);
		if((outfile = fopen(outputFile, "wb")) == NULL)
		{
			/* Try again in case the 'b' flag is not understood by libc */
			if((outfile = fopen(outputFile, "w")) == NULL)
			{
				perror(outputFile);
				return 1;
			}
		}
		ILAsmOutCreate(outfile, 1, format, flags);
	}
	ILAsmDebugMode = debug;

	/* Parse the source files */
	sawStdin = 0;
	ILAsmErrors = 0;
	while(argc > 1)
	{
		ILAsmLineNum = 1;
		ILAsmParseHexBytes = 0;
		if(!strcmp(argv[1], "-"))
		{
			/* Parse the contents of stdin, but only once */
			if(!sawStdin)
			{
				ILAsmFilename = "stdin";
				ilasm_restart(stdin);
				ILAsmErrors |= ilasm_parse();
				sawStdin = 1;
			}
		}
		else
		{
			/* Parse the contents of a regular file */
			if((infile = fopen(argv[1], "r")) == NULL)
			{
				perror(argv[1]);
				ILAsmErrors = 1;
			}
			else
			{
				ILAsmFilename = argv[1];
				ilasm_restart(infile);
				ILAsmErrors |= ilasm_parse();
				fclose(infile);
			}
		}
		++argv;
		--argc;
	}

	/* Exit if errors occurred during the parse */
	if(ILAsmErrors)
	{
		if(outfile != NULL)
		{
			fclose(outfile);
			ILDeleteFile(outputFile);
		}
		return 1;
	}

	/* Finalize the output */
	if(!ILAsmOutDestroy())
	{
		if(outfile != NULL)
		{
			fclose(outfile);
		}
		return 1;
	}
	if(outfile != NULL)
	{
		fclose(outfile);
	}

	/* Done */
	return 0;
}

static void usage(const char *progname)
{
	fprintf(stderr, "ILASM " VERSION " - Intermediate Language Assembler\n");
	fprintf(stderr, "Copyright (c) 2001 Southern Storm Software, Pty Ltd.\n");
	fprintf(stderr, "\n");
	fprintf(stderr, "Usage: %s [options] input.il ...\n", progname);
	fprintf(stderr, "\n");
	ILCmdLineHelp(options);
}

static void version(void)
{

	printf("ILASM " VERSION " - Intermediate Language Assembler\n");
	printf("Copyright (c) 2001 Southern Storm Software, Pty Ltd.\n");
	printf("\n");
	printf("ILASM comes with ABSOLUTELY NO WARRANTY.  This is free software,\n");
	printf("and you are welcome to redistribute it under the terms of the\n");
	printf("GNU General Public License.  See the file COPYING for further details.\n");
	printf("\n");
	printf("Use the `--help' option to get help on the command-line options.\n");
}

#ifdef	__cplusplus
};
#endif
