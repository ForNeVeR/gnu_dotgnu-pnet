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
		"--output file               or -o file",
		"Specify the output file to use.  The default name is\n"
		"based on the name of the first input file."},
	{"-e", 'e', 0,
		"--format exe                or -e",
		"Set the output format to executable."},
	{"-d", 'd', 0,
		"--format dll                or -d",
		"Set the output format to dynamic link library."},
	{"-j", 'j', 0,
		"--format obj                or -j",
		"Set the output format to object file."},
	{"--format", 'F', 1, 0, 0},
	{"-l", 'l', 1, 0, 0},
	{"--library", 'l', 1,
		"--library name              or -l name",
		"Link against the library `name'."},
	{"-L", 'L', 1, 0, 0},
	{"--library-dir", 'L', 1,
		"--library-dir name          or -L name",
		"Look for libraries in the directory `name'."},
	{"-n", 'n', 0,
		"--no-stdlib                 or -n",
		"Do not link against the standard library."},
	{"-S", 'S', 1,
		"-fstdlib-name=name          or -S name",
		"Specify the name of the standard library (default is `mscorlib')."},
	{"--shared", '1', 0,
		"--shared",
		"Link the output as a shared image (default)."},
	{"--static", '2', 0,
		"--static",
		"Link the output as a static image."},
	{"-a", 'a', 1,
		"-fassembly-name=name        or -a name",
		"Specify the name of the assembly to embed in the output."},
	{"-A", 'A', 1,
		"-fassembly-version=version  or -A version",
		"Specify the assembly version to embed in the output."},
	{"-M", 'M', 1,
		"-fmodule-name=name          or -M name",
		"Specify the name of the module to embed in the output."},
	{"-E", 'E', 1, 0, 0},
	{"--entry-point", 'E', 1,
		"--entry-point name          or -E name",
		"Specify the name of the entry point for the program."},
	{"-r", 'r', 1,
		"-fresources=file            or -r file",
		"Link the specified resource file with the output."},
	{"-R", 'R', 0, 0, 0},
	{"--resources-only", 'R', 0,
		"--resources-only            or -R",
		"Create an output that only contains resources."},
	{"-p", 'p', 0,
		"-fprivate-resources         or -p",
		"Mark the resources as private to the output assembly."},
	{"-H", 'H', 1,
		"-fhash-algorithm=name       or -H name",
		"Specify the algorithm to use to hash files (SHA1 or MD5)"},
	{"-3", '3', 0,
		"-m32bit-only                or -3",
		"The resulting output file can only be used on 32-bit systems."},
	{"-c", 'c', 0,
		"-mcui-subsystem             or -c",
		"Compile for the command-line subsystem (default)."},
	{"-G", 'G', 0,
		"-mgui-subsystem             or -G",
		"Compile for the GUI subsystem."},
	{"-f", 'f', 1, 0, 0},
	{"-m", 'm', 1, 0, 0},
	{"--version", 'v', 0,
		"--version                   or -v",
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
static int addResource(ILLinker *linker, const char *filename,
					   FILE *stream, int privateResources, int isStdin);
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
	char *moduleName = NULL;
	char *entryPoint = NULL;
	int hashAlgorithm = IL_META_HASHALG_SHA1;
	int len;
	char **libraries;
	int numLibraries = 0;
	char **libraryDirs;
	int numLibraryDirs = 0;
	char *stdLibrary = "mscorlib";
	char **resources;
	int numResources = 0;
	int jvmMode = 0;
	int useStdlib = 1;
	int isStatic = 0;
	int privateResources = 0;
	int temp, temp2;
	ILLinker *linker;

	/* Allocate an array to hold the libraries to link against */
	libraries = (char **)ILCalloc(argc, sizeof(char *));
	if(!libraries)
	{
		outOfMemory();
	}

	/* Allocate an array to hold the directories to look for libraries in */
	libraryDirs = (char **)ILCalloc(argc, sizeof(char *));
	if(!libraryDirs)
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

			case 'F':
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

			case 'L':
			{
				libraryDirs[numLibraryDirs++] = param;
			}
			break;

			case 'n':
			{
				useStdlib = 0;
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

			case 'M':
			{
				moduleName = param;
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

			case 'p':
			{
				privateResources = 1;
			}
			break;

			case 'S':
			{
				stdLibrary = param;
			}
			break;

			case '1':
			{
				isStatic = 0;
			}
			break;

			case '2':
			{
				isStatic = 1;
			}
			break;

			case 'H':
			{
			parseHashAlg:
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

			case 'f':
			{
				/* Parse a flag passed down from the compiler */
				if(!strncmp(param, "stdlib-name=", 12))
				{
					stdLibrary = param + 12;
				}
				else if(!strncmp(param, "assembly-name=", 14))
				{
					assemblyName = param + 14;
				}
				else if(!strncmp(param, "assembly-version=", 17))
				{
					if(!parseVersion(assemblyVersion, param + 17))
					{
						fprintf(stderr,
								"%s: `%s' is not a valid assembly version\n",
								progname, param);
						return 1;
					}
				}
				else if(!strncmp(param, "module-name=", 12))
				{
					moduleName = param + 12;
				}
				else if(!strncmp(param, "resources=", 10))
				{
					resources[numResources++] = param + 10;
				}
				else if(!strncmp(param, "hash-algorithm=", 15))
				{
					param += 15;
					goto parseHashAlg;
				}
				else if(!strcmp(param, "private-resources"))
				{
					privateResources = 1;
				}
				else
				{
					/* All other flags are ignored, because they may
					   be for other parts of the compiler chain */
				}
			}
			break;

			case 'm':
			{
				/* Parse a machine flag passed down from the compiler */
				if(!strcmp(param, "32bit-only"))
				{
					flags |= IL_WRITEFLAG_32BIT_ONLY;
				}
				else if(!strcmp(param, "cui-subsystem"))
				{
					flags &= ~IL_WRITEFLAG_SUBSYS_GUI;
				}
				else if(!strcmp(param, "gui-subsystem"))
				{
					flags |= IL_WRITEFLAG_SUBSYS_GUI;
				}
				else if(!strcmp(param, "jvm"))
				{
					jvmMode = 1;
				}
				else
				{
					/* All other flags are ignored, because they may
					   be for other parts of the compiler chain */
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

	/* Determine the default module and assembly names */
	temp = strlen(outputFile);
	temp2 = -1;
	while(temp > 0 && outputFile[temp - 1] != '/' &&
		  outputFile[temp - 1] != '\\')
	{
		--temp;
		if(outputFile[temp] == '.' && temp2 == -1)
		{
			temp2 = temp;
		}
	}
	if(!moduleName)
	{
		moduleName = ILDupString(outputFile + temp);
	}
	if(!assemblyName)
	{
		assemblyName = ILDupString(outputFile + temp);
		if(temp2 != -1)
		{
			assemblyName[temp2 - temp] = '\0';
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

	/* Create the global module and assembly */
	if(!ILLinkerCreateModuleAndAssembly(linker, moduleName,
										assemblyName, assemblyVersion,
										hashAlgorithm))
	{
		ILLinkerDestroy(linker);
		fclose(outfile);
		ILDeleteFile(outputFile);
		outOfMemory();
	}

	/* Add the library directories to the linker context */
	for(temp = 0; temp < numLibraryDirs; ++temp)
	{
		errors |= !ILLinkerAddLibraryDir(linker, libraryDirs[temp]);
	}
	if(useStdlib)
	{
		if(!ILLinkerAddSystemDirs(linker))
		{
			errors = 1;
		}
	}

	/* Add the libraries to the linker context */
	for(temp = 0; temp < numLibraries; ++temp)
	{
		errors |= addLibrary(linker, libraries[temp]);
	}
	if(useStdlib)
	{
		errors |= addLibrary(linker, stdLibrary);
	}

	/* Process the input files that aren't libraries */
	sawStdin = 0;
	while(argc > 1)
	{
		if(!strcmp(argv[1], "-"))
		{
			/* Process the contents of stdin, but only once */
			if(!sawStdin)
			{
				if(resourcesOnly)
				{
					errors |= addResource(linker, "stdin", stdin,
										  privateResources, 1);
				}
				else
				{
					errors |= processFile(linker, "stdin", stdin, 1);
				}
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
			if(resourcesOnly)
			{
				errors |= addResource(linker, argv[1], infile,
									  privateResources, 0);
			}
			else
			{
				errors |= processFile(linker, argv[1], infile, 0);
			}
		}
		++argv;
		--argc;
	}

	/* Add the explicit resource files to the linker context */
	for(temp = 0; temp < numResources; ++temp)
	{
		if(!strcmp(resources[temp], "-"))
		{
			/* Process the contents of stdin, but only once */
			if(!sawStdin)
			{
				errors |= addResource(linker, "stdin", stdin,
									  privateResources, 1);
				sawStdin = 1;
			}
		}
		else
		{
			if((infile = fopen(resources[temp], "rb")) == NULL)
			{
				/* Try again with "r" in case libc does not understand "rb" */
				if((infile = fopen(resources[temp], "r")) == NULL)
				{
					perror(resources[temp]);
					errors = 1;
				}
			}
			if(infile != NULL)
			{
				errors |= addResource(linker, resources[temp], infile,
									  privateResources, 0);
			}
		}
	}

	/* Set the default entry point, or report an error about
	   incorrect entry point specifications */
	if(entryPoint)
	{
		if(!ILLinkerSetEntryPoint(linker, entryPoint))
		{
			fprintf(stderr, "%s: could not locate the entry point `%s'",
					outputFile, entryPoint);
			errors = 1;
		}
	}
	if(format == IL_IMAGETYPE_EXE)
	{
		if(!ILLinkerHasEntryPoint(linker))
		{
			fprintf(stderr, "%s: no entry point specified\n", outputFile);
			errors = 1;
		}
	}
	else if(format == IL_IMAGETYPE_DLL)
	{
		if(ILLinkerHasEntryPoint(linker))
		{
			fprintf(stderr, "%s: DLL's cannot have entry points\n",
					outputFile);
			errors = 1;
		}
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
	char *newFilename;

	/* Resolve the library name */
	newFilename = ILLinkerResolveLibrary(linker, filename);
	if(!newFilename)
	{
		fprintf(stderr, "%s: library not found\n", filename);
		return 1;
	}

	/* Open the library file */
	if((file = fopen(newFilename, "rb")) == NULL)
	{
		/* Try again in case libc does not understand "rb" */
		if((file = fopen(newFilename, "r")) == NULL)
		{
			perror(newFilename);
			ILFree(newFilename);
			return 1;
		}
	}

	/* Load the library as an image */
	context = ILContextCreate();
	if(!context)
	{
		outOfMemory();
	}
	loadError = ILImageLoad(file, newFilename, context, &image,
							IL_LOADFLAG_FORCE_32BIT | IL_LOADFLAG_NO_RESOLVE);
	if(loadError)
	{
		fprintf(stderr, "%s: %s\n", newFilename, ILImageLoadError(loadError));
		ILContextDestroy(context);
		fclose(file);
		ILFree(newFilename);
		return 1;
	}
	fclose(file);

	/* Add the library image to the linker context */
	if(!ILLinkerAddLibrary(linker, image, newFilename))
	{
		errors = 1;
	}

	/* Clean up and exit */
	ILContextDestroy(context);
	ILFree(newFilename);
	return errors;
}

/*
 * Add a resource file to the final image.
 */
static int addResource(ILLinker *linker, const char *filename,
					   FILE *stream, int privateResources, int isStdin)
{
	int len;

	/* Determine the name of the resource from the filename */
	len = strlen(filename);
	while(len > 0 && filename[len - 1] != '/' &&
	      filename[len - 1] != '\\')
	{
		--len;
	}

	/* Add the resource to the linker image */
	if(!ILLinkerAddResource(linker, filename + len, privateResources, stream))
	{
		if(!isStdin)
		{
			fclose(stream);
		}
		return 1;
	}

	/* Done */
	if(!isStdin)
	{
		fclose(stream);
	}
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
