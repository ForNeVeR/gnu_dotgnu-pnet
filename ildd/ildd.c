/*
 * ildd.c - Print information about library dependencies.
 *
 * Copyright (C) 2002  Southern Storm Software, Pty Ltd.
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
#include "il_utils.h"

#ifdef	__cplusplus
extern	"C" {
#endif

/*
 * Table of command-line options.
 */
static ILCmdLineOption const options[] = {
	{"-v", 'v', 0, 0, 0},
	{"--version", 'v', 0,
		"--version or -v",
		"Print the version of the program."},
	{"--help", 'h', 0,
		"--help",
		"Print this help message."},
	{0, 0, 0, 0, 0}
};

static void usage(const char *progname);
static void version(void);
static int printDependencies(const char *filename, ILContext *context,
							 int multiple);

int main(int argc, char *argv[])
{
	char *progname = argv[0];
	int sawStdin;
	int state, opt;
	char *param;
	int errors;
	int multiple;
	ILContext *context;

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

	/* Create a context to use for image loading */
	context = ILContextCreate();
	if(!context)
	{
		fprintf(stderr, "%s: out of memory\n", progname);
		return 1;
	}

	/* Load and print information about the input files */
	sawStdin = 0;
	errors = 0;
	multiple = (argc > 2);
	while(argc > 1)
	{
		if(!strcmp(argv[1], "-"))
		{
			/* Dump the contents of stdin, but only once */
			if(!sawStdin)
			{
				errors |= printDependencies("-", context, multiple);
				sawStdin = 1;
			}
		}
		else
		{
			/* Dump the contents of a regular file */
			errors |= printDependencies(argv[1], context, multiple);
		}
		++argv;
		--argc;
	}

	/* Destroy the context */
	ILContextDestroy(context);
	
	/* Done */
	return errors;
}

static void usage(const char *progname)
{
	fprintf(stdout, "ILDD " VERSION " - IL Library Dependencies Utility\n");
	fprintf(stdout, "Copyright (c) 2002 Southern Storm Software, Pty Ltd.\n");
	fprintf(stdout, "\n");
	fprintf(stdout, "Usage: %s [options] input ...\n", progname);
	fprintf(stdout, "\n");
	ILCmdLineHelp(options);
}

static void version(void)
{

	fprintf(stdout, "ILDD " VERSION " - IL Library Dependencies Utility\n");
	fprintf(stdout, "Copyright (c) 2002 Southern Storm Software, Pty Ltd.\n");
	printf("\n");
	printf("ILDD comes with ABSOLUTELY NO WARRANTY.  This is free software,\n");
	printf("and you are welcome to redistribute it under the terms of the\n");
	printf("GNU General Public License.  See the file COPYING for further details.\n");
	printf("\n");
	printf("Use the `--help' option to get help on the command-line options.\n");
}

/*
 * Load an IL image from an input stream and print its dependency information.
 */
static int printDependencies(const char *filename, ILContext *context,
							 int multiple)
{
	ILImage *image;
	ILAssembly *assem;
	const char *name;
	const ILUInt16 *version;
	char *path;
	ILPInvoke *pinv;
	ILMethod *method;
	void *dynlib;
	void *symbol;

	/* Attempt to load the image into memory */
	if(ILImageLoadFromFile(filename, context, &image,
					       IL_LOADFLAG_FORCE_32BIT |
						   IL_LOADFLAG_NO_RESOLVE, 1) != 0)
	{
		return 1;
	}

	/* Print the file header if we have multiple files */
	if(multiple)
	{
		if(!strcmp(filename, "-"))
		{
			fputs("stdin:\n", stdout);
		}
		else
		{
			fputs(filename, stdout);
			fputs(":\n", stdout);
		}
	}

	/* Print the assemblies that this file depends upon */
	assem = 0;
	while((assem = (ILAssembly *)ILImageNextToken
				(image, IL_META_TOKEN_ASSEMBLY_REF, assem)) != 0)
	{
		/* Print the assembly name and version */
		putc('\t', stdout);
		name = ILAssembly_Name(assem);
		version = ILAssemblyGetVersion(assem);
		fputs(name, stdout);
		printf("/%d:%d:%d:%d => ",
			   (int)(version[0]), (int)(version[1]),
			   (int)(version[2]), (int)(version[3]));

		/* Search for the full pathname of the referenced assembly */
		path = ILImageSearchPath(name, version, filename, 0, 0, 0, 0, 0, 0);
		if(path)
		{
			fputs(path, stdout);
			ILFree(path);
		}
		else
		{
			fputs("??", stdout);
		}
		putc('\n', stdout);
	}

	/* Print the external PInvoke'd functions that this file depends upon */
	pinv = 0;
	while((pinv = (ILPInvoke *)ILImageNextToken
				(image, IL_META_TOKEN_IMPL_MAP, pinv)) != 0)
	{
		/* Print the name of the PInvoke method */
		putc('\t', stdout);
		name = ILPInvoke_Alias(pinv);
		if(!name)
		{
			method = ILPInvokeGetMethod(pinv);
			if(method)
			{
				name = ILMethod_Name(method);
			}
			else
			{
				name = "??";
			}
		}
		fputs(name, stdout);
		fputs(" => ", stdout);

		/* Print the module that contains the PInvoke declaration */
		path = ILPInvokeResolveModule(pinv);
		if(path)
		{
			fputs(path, stdout);
			dynlib = ILDynLibraryOpen(path);
			if(!dynlib)
			{
				fputs(" (module not found)", stdout);
			}
			else
			{
				symbol = ILDynLibraryGetSymbol(dynlib, name);
				if(!symbol)
				{
					fputs(" (symbol not found)", stdout);
				}
			}
			ILFree(path);
		}
		else
		{
			fputs("??", stdout);
		}
		putc('\n', stdout);
	}

	/* Clean up and exit */
	ILImageDestroy(image);
	return 0;
}

#ifdef	__cplusplus
};
#endif
