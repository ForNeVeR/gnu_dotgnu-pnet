/*
 * ilalink.c - Link IL images together to form an assembly.
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

/*

Options:

	-ename			Entry point
	-llibrary		Assembly to link against
	-Ldir			Directory to search for assemblies within
	-shared/-static	Adjust link style
	-m32bit-only	Output a 32-bit only executable
	-fresources=res	Add a resource file to the output
	-c				CUI subsystem
	-G				GUI subsystem
*/

#include <stdio.h>
#include "il_system.h"
#include "il_image.h"
#include "il_program.h"
#include "il_writer.h"
#include "il_linker.h"
#include "il_utils.h"

#ifdef	__cplusplus
extern	"C" {
#endif

/*
 * Table of command-line options.
 */
static ILCmdLineOption const options[] = {
	{"-o", 'o', 1, 0, 0},
	{"--output", 'o', 1,
		"--output file      or -o file",
		"Specify the output file to use.  The default name is\n"
		"based on the name of the first input file."},
	{"-e", 'e', 0,
		"--format exe       or -e",
		"Set the output format to executable."},
	{"-d", 'd', 0,
		"--format dll       or -d",
		"Set the output format to dynamic link library."},
	{"-j", 'j', 0,
		"--format obj       or -j",
		"Set the output format to object file."},
	{"--format", 'f', 1, 0, 0},
	{"-l", 'l', 1, 0, 0},
	{"--library", 'l', 1,
		"--library name     or -l name",
		"Link against the library `name'."},
	{"-a", 'a', 1, 0, 0},
	{"--assembly", 'a', 1,
		"--assembly name    or -a name",
		"Specify the name of the assembly to embed in the output."},
	{"-A", 'A', 1, 0, 0},
	{"--assembly-version", 'A', 1,
		"--assembly-version version  or -A version",
		"Specify the assembly version to embed in the output."},
	{"-E", 'E', 1, 0, 0},
	{"--entry-point", 'E', 1,
		"--entry-point name or -E name",
		"Specify the name of the entry point for the program."},
	{"-r", 'r', 1, 0, 0},
	{"--res", 'r', 1,
		"--res file         or -r file",
		"Link the specified resource file with the output."},
	{"-R", 'R', 0, 0, 0},
	{"--resources-only", 'R', 0,
		"--resources-only   or -R",
		"Create an output that only contains resources."},
	{"-H", 'H', 1, 0, 0},
	{"--hash-algorithm", 'H', 1,
		"--hash-algorithm   or -H",
		"Specify the algorithm to use to hash files (SHA1 or MD5)"},
	{"-3", '3', 0, 0, 0},
	{"-c", 'c', 0, 0, 0},
	{"-G", 'G', 0, 0, 0},
	{"-m", 'm', 0, 0, 0},
	{"--32bit-only", '3', 0,
		"--32bit-only       or -3",
		"The resulting output file can only be used on 32-bit systems."},
	{"--cui-subsystem", 'c', 0,
		"--cui-subsystem    or -c",
		"Compile for the command-line subsystem (default)."},
	{"--gui-subsystem", 'G', 0,
		"--gui-subsystem    or -G",
		"Compile for the GUI subsystem."},
	{"--version", 'v', 0,
		"--version          or -v",
		"Print the version of the program"},
	{"--help", 'h', 0,
		"--help",
		"Print this help message."},
	{0, 0, 0, 0, 0}
};

static void usage(const char *progname);
static void version(void);
static void outOfMemory(void);
static int parseVersion(ILUInt16 *version, const char *str);
static int addLibrary(ILLinker *linker, const char *filename);
static int addResource(ILLinker *linker, const char *filename);
static int processFile(ILLinker *linker, const char *filename,
					   FILE *stream, int isStdin);

int main(int argc, char *argv[])
{
	char *progname = argv[0];
	int sawStdin;
	int state, opt;
	char *param;
	FILE *outfile;
	FILE *infile;
	int errors = 0;
	char *outputFile = NULL;
	int format = -1;
	int flags = 0;
	int resourcesOnly = 1;
	char *assemblyName = NULL;
	ILUInt16 assemblyVersion[4] = {0, 0, 0, 0};
	char *entryPoint = NULL;
	int hashAlgorithm = IL_META_HASHALG_SHA1;
	int len;
	char **libraries;
	int numLibraries = 0;
	char **resources;
	int numResources = 0;
	int temp;
	ILLinker *linker;

	/* Allocate an array to hold the libraries to link against */
	libraries = (char **)ILCalloc(argc, sizeof(char *));
	if(!libraries)
	{
		outOfMemory();
	}

	/* Allocate an array to hold the resource files to link against */
	resources = (char **)ILCalloc(argc, sizeof(char **));
	if(!resources)
	{
		outOfMemory();
	}

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

			case 'l':
			{
				libraries[numLibraries++] = param;
			}
			break;

			case 'a':
			{
				assemblyName = param;
			}
			break;

			case 'A':
			{
				if(!parseVersion(assemblyVersion, param))
				{
					fprintf(stderr,
							"%s: `%s' is not a valid assembly version\n",
							progname, param);
					return 1;
				}
			}
			break;

			case 'E':
			{
				entryPoint = param;
			}
			break;

			case 'r':
			{
				resources[numResources++] = param;
			}
			break;

			case 'R':
			{
				resourcesOnly = 1;
			}
			break;

			case 'H':
			{
				if(!ILStrICmp(param, "SHA1") || !ILStrICmp(param, "SHA-1"))
				{
					hashAlgorithm = IL_META_HASHALG_SHA1;
				}
				else if(!ILStrICmp(param, "MD5"))
				{
					hashAlgorithm = IL_META_HASHALG_MD5;
				}
				else
				{
					fprintf(stderr,
							"%s: `%s' is not a valid hash algorithm.\n",
							progname, param);
					fprintf(stderr, "Valid values are `SHA1' and `MD5'.\n");
					return 1;
				}
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

	/* Determine the default output format from the
	   extension on the output file */
	if(format == -1)
	{
		format = IL_IMAGETYPE_EXE;
		if(outputFile)
		{
			len = strlen(outputFile);
			while(len > 0 && outputFile[len - 1] != '.' &&
				  outputFile[len - 1] != '/' &&
				  outputFile[len - 1] != '\\')
			{
				--len;
			}
			if(len > 0 && outputFile[len - 1] == '.')
			{
				if(!ILStrICmp(outputFile + len, "dll"))
				{
					format = IL_IMAGETYPE_DLL;
				}
				else if(!ILStrICmp(outputFile + len, "obj"))
				{
					format = IL_IMAGETYPE_OBJ;
				}
			}
		}
	}

	/* Set the default output filename if necessary */
	if(!outputFile)
	{
		if(format == IL_IMAGETYPE_EXE)
		{
			outputFile = "a.out.exe";
		}
		else if(format == IL_IMAGETYPE_DLL)
		{
			outputFile = "a.out.dll";
		}
		else
		{
			outputFile = "a.out.obj";
		}
	}

	/* Open the output file */
	if((outfile = fopen(outputFile, "wb")) == NULL)
	{
		/* Try again in case libc does not understand "wb" */
		if((outfile = fopen(outputFile, "w")) == NULL)
		{
			perror(outputFile);
			return 1;
		}
	}

	/* Create the linker context */
	linker = ILLinkerCreate(outfile, 1, format, flags);
	if(!linker)
	{
		fclose(outfile);
		ILDeleteFile(outputFile);
		outOfMemory();
	}

	/* Add the libraries to the linker context */
	for(temp = 0; temp < numLibraries; ++temp)
	{
		errors |= addLibrary(linker, libraries[temp]);
	}

	/* Add the resources to the linker context */
	for(temp = 0; temp < numResources; ++temp)
	{
		errors |= addResource(linker, resources[temp]);
	}

	/* Process the input files */
	sawStdin = 0;
	while(argc > 1)
	{
		if(resourcesOnly)
		{
			/* All command-line arguments are resource files */
			errors |= addResource(linker, argv[1]);
		}
		else if(!strcmp(argv[1], "-"))
		{
			/* Process the contents of stdin, but only once */
			if(!sawStdin)
			{
				errors |= processFile(linker, "stdin", stdin, 1);
				sawStdin = 1;
			}
		}
		else
		{
			/* Process the contents of a regular file */
			if((infile = fopen(argv[1], "rb")) == NULL)
			{
				/* Try again in case libc did not understand the 'b' */
				if((infile = fopen(argv[1], "r")) == NULL)
				{
					perror(argv[1]);
					errors = 1;
					++argv;
					--argc;
					continue;
				}
			}
			errors |= processFile(linker, argv[1], infile, 0);
		}
		++argv;
		--argc;
	}

	/* Destroy the linker context */
	len = ILLinkerDestroy(linker);
	fclose(outfile);
	if(!len)
	{
		ILDeleteFile(outputFile);
		errors = 1;
	}
	else if(len < 0)
	{
		ILDeleteFile(outputFile);
		outOfMemory();
	}

	/* Done */
	return errors;
}

static void usage(const char *progname)
{
	fprintf(stderr, "ILALINK " VERSION " - IL Assembly Link Utility\n");
	fprintf(stderr, "Copyright (c) 2001 Southern Storm Software, Pty Ltd.\n");
	fprintf(stderr, "\n");
	fprintf(stderr, "Usage: %s [options] input ...\n", progname);
	fprintf(stderr, "\n");
	ILCmdLineHelp(options);
}

static void version(void)
{

	printf("ILALINK " VERSION " - IL Assembly Link Utility\n");
	printf("Copyright (c) 2001 Southern Storm Software, Pty Ltd.\n");
	printf("\n");
	printf("ILALINK comes with ABSOLUTELY NO WARRANTY.  This is free software,\n");
	printf("and you are welcome to redistribute it under the terms of the\n");
	printf("GNU General Public License.  See the file COPYING for further details.\n");
	printf("\n");
	printf("Use the `--help' option to get help on the command-line options.\n");
}

static void outOfMemory(void)
{
	fputs("virtual memory exhausted\n", stderr);
	exit(1);
}

/*
 * Parse an assembly version string.
 */
static int parseVersion(ILUInt16 *version, const char *str)
{
	int posn;
	ILUInt32 value;
	for(posn = 0; posn < 4; ++posn)
	{
		if(*str < '0' || *str > '9')
		{
			return 0;
		}
		value = (ILUInt32)(*str++ - '0');
		while(*str >= '0' && *str <= '9')
		{
			value = value * ((ILUInt32)10) + (ILUInt32)(*str++ - '0');
			if(value >= (ILUInt32)0x10000)
			{
				return 0;
			}
		}
		version[posn] = (ILUInt16)value;
		if(posn == 3)
		{
			break;
		}
		if(*str != ':' && *str != '.')
		{
			return 0;
		}
		++str;
	}
	return (*str == '\0');
}

/*
 * Add a library to a linker context.  Returns non-zero on error.
 */
static int addLibrary(ILLinker *linker, const char *filename)
{
	FILE *file;
	ILContext *context;
	int loadError;
	ILImage *image;
	int errors = 0;

	/* Open the library file */
	if((file = fopen(filename, "rb")) == NULL)
	{
		/* Try again in case libc does not understand "rb" */
		if((file = fopen(filename, "r")) == NULL)
		{
			perror(filename);
			return 1;
		}
	}

	/* Load the library as an image */
	context = ILContextCreate();
	if(!context)
	{
		outOfMemory();
	}
	loadError = ILImageLoad(file, filename, context, &image,
							IL_LOADFLAG_FORCE_32BIT | IL_LOADFLAG_NO_RESOLVE);
	if(loadError)
	{
		fprintf(stderr, "%s: %s\n", filename, ILImageLoadError(loadError));
		ILContextDestroy(context);
		fclose(file);
		return 1;
	}
	fclose(file);

	/* Add the library image to the linker context */
	if(!ILLinkerAddLibrary(linker, image, filename))
	{
		errors = 1;
	}

	/* Clean up and exit */
	ILContextDestroy(context);
	return errors;
}

/*
 * Add a resource file to the final image.
 */
static int addResource(ILLinker *linker, const char *filename)
{
	/* TODO */
	return 0;
}

/*
 * Process an image file to be linked into the final image.
 * Returns non-zero on error.
 */
static int processFile(ILLinker *linker, const char *filename,
					   FILE *stream, int isStdin)
{
	int errors = 0;
	ILContext *context;
	ILImage *image;
	int loadError;

	/* Attempt to load the image into memory */
	context = ILContextCreate();
	if(!context)
	{
		outOfMemory();
	}
	loadError = ILImageLoad(stream, filename, context, &image,
							IL_LOADFLAG_FORCE_32BIT | IL_LOADFLAG_NO_RESOLVE);
	if(!isStdin)
	{
		fclose(stream);
	}
	if(loadError != 0)
	{
		fprintf(stderr, "%s: %s\n", filename, ILImageLoadError(loadError));
		errors = 1;
	}
	else
	{
		/* Add the image to the linker context */
		if(!ILLinkerAddImage(linker, image, filename))
		{
			errors = 1;
		}
	}

	/* Clean up and exit */
	ILContextDestroy(context);
	return errors;
}

#ifdef	__cplusplus
};
#endif
