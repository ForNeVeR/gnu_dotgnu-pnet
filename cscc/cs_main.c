/*
 * cs_main.c - Main entry point for the C# compiler plug-in.
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

#include "cs_internal.h"

#ifdef	__cplusplus
extern	"C" {
#endif

/*
 * Entry points for the parser and lexer.
 */
extern int cs_debug;
extern int cs_parse(void);
extern void cs_restart(FILE *infile);

/*
 * Forward declarations.
 */
static void PreprocessClose(void);
static int InitCodeGen(void);
static void CloseCodeGen(void);
static void ParseFile(FILE *file, const char *filename, int is_stdin);

int main(int argc, char *argv[])
{
	FILE *infile;
	int saw_stdin;
	int posn;
	int status;

	/* Parse the command-line options */
	CSParseCommandLine(argc, argv, CMDLINE_PARSE_PLUGIN, "cscc-cs");

	/* Initialize the code generator if we aren't pre-processing */
	if(!preprocess_flag)
	{
		status = InitCodeGen();
		if(status != 0)
		{
			return status;
		}
	}

	/* Parse all of the input files */
	saw_stdin = 0;
	for(posn = 0; posn < num_input_files; ++posn)
	{
		if(!strcmp(input_files[posn], "-"))
		{
			if(!saw_stdin)
			{
				ParseFile(stdin, "stdin", 1);
				saw_stdin = 1;
			}
		}
		else
		{
			if((infile = fopen(input_files[posn], "r")) == NULL)
			{
				perror(input_files[posn]);
				CSHaveErrors |= 1;
			}
			else
			{
				ParseFile(infile, input_files[posn], 0);
			}
		}
	}

	/* If we are only pre-processing, then we are finished */
	if(preprocess_flag)
	{
		PreprocessClose();
		return (CSHaveErrors != 0);
	}

	/* If we are only performing a syntax check, then bail out now */
	if(CSStringListContains(extension_flags, num_extension_flags,
							"syntax-check"))
	{
		CloseCodeGen();
		return (CSHaveErrors != 0);
	}

	/* Bail out if nothing was parsed, and we have errors */
	if(!CSParseTree && CSHaveErrors != 0)
	{
		CloseCodeGen();
		return 1;
	}

	/* Inform the code generator that we are doing semantic analysis */
	CSCodeGen.semAnalysis = 1;
	CSCodeGen.typeGather = 1;

	/* Perform type gathering */
	CSParseTree = CSTypeGather(&CSCodeGen, CSGlobalScope, CSParseTree);
	CSCodeGen.typeGather = 0;

	/* Perform semantic analysis */
	ILNode_SemAnalysis(CSParseTree, &CSCodeGen, &CSParseTree);

	/* We are no longer doing semantic analysis */
	CSCodeGen.semAnalysis = 0;

	/* If we are only performing a semantic check, then bail out now */
	if(CSStringListContains(extension_flags, num_extension_flags,
							"semantic-check"))
	{
		CloseCodeGen();
		return (CSHaveErrors != 0);
	}

	/* Generate the code if no errors occurred in the previous phases */
	if(CSHaveErrors == 0)
	{
		ILGenModulesAndAssemblies(&CSCodeGen);
		if(CSCodeGen.outputIsJava)
		{
			JavaGenDiscard(CSParseTree, &CSCodeGen);
		}
		else
		{
			ILNode_GenDiscard(CSParseTree, &CSCodeGen);
		}
	}

	/* Close the code generator */
	CloseCodeGen();

	/* Done */
	return (CSHaveErrors != 0);
}

/*
 * State for the pre-processor.
 */
static FILE *preproc_outfile = 0;
static int preproc_is_stdout = 0;
static const char *preproc_filename = 0;

/*
 * Pre-process the input stream and write it directly to the output stream.
 */
static int Preprocess(void)
{
	FILE *outfile;
	int is_stdout;
	char buffer[BUFSIZ];
	int size;
	const char *filename;
	CSPreProcSymbol *symbol;
	char *expectedFile;
	unsigned long expectedLine;

	/* Open the output stream */
	if(!preproc_outfile)
	{
		if(!output_filename || !strcmp(output_filename, "-"))
		{
			outfile = stdout;
			is_stdout = 1;
			filename = "stdout";
		}
		else
		{
			if((outfile = fopen(output_filename, "w")) == NULL)
			{
				perror(output_filename);
				return 1;
			}
			is_stdout = 0;
			filename = output_filename;
		}
		preproc_outfile = outfile;
		preproc_filename = filename;
		preproc_is_stdout = is_stdout;
	}
	else
	{
		outfile = preproc_outfile;
		filename = preproc_filename;
	}

	/* Warn about the -C option, which we don't support yet */
	if(preproc_comments_flag)
	{
		fprintf(stderr, "%s: warning: -C is not yet supported by "
						"the C# pre-processor\n", progname);
	}

	/* Copy the pre-processed data to the output stream */
	expectedFile = 0;
	expectedLine = 0;
	while((size = CSPreProcGetBuffer(&CSPreProcessorStream, buffer, BUFSIZ))
						> 0)
	{
		if(dump_output_format != DUMP_MACROS_ONLY)
		{
			if(!no_preproc_lines_flag)
			{
				/* Output line number information if it has changed */
				if(expectedFile != CSPreProcessorStream.lexerFileName ||
				   expectedLine != CSPreProcessorStream.lexerLineNumber)
				{
					expectedLine = CSPreProcessorStream.lexerLineNumber;
					if(expectedFile != CSPreProcessorStream.lexerFileName)
					{
						expectedFile = CSPreProcessorStream.lexerFileName;
						if(fprintf(outfile, "#line %lu \"%s\"\n",
								   expectedLine, expectedFile) < 0)
						{
							goto truncated;
						}
					}
					else
					{
						if(fprintf(outfile, "#line %lu\n", expectedLine) < 0)
						{
							goto truncated;
						}
					}
				}
				++expectedLine;
			}
			if(fwrite(buffer, 1, size, outfile) != (unsigned)size)
			{
			truncated:
				fprintf(stderr, "%s: output truncated\n", filename);
				return 1;
			}
		}
	}

	/* Dump the defined macros if necessary */
	if(dump_output_format != DUMP_OUTPUT_ONLY)
	{
		symbol = CSPreProcessorStream.symbols;
		while(symbol != 0)
		{
			fputs("#define ", outfile);
			fputs(symbol->name, outfile);
			putc('\n', outfile);
			symbol = symbol->next;
		}
	}

	/* Finished with this file */
	return CSPreProcessorStream.error;
}

/*
 * Close the pre-processor output stream.
 */
static void PreprocessClose(void)
{
	if(preproc_outfile != 0 && !preproc_is_stdout)
	{
		fclose(preproc_outfile);
	}
}

/*
 * Check for the existence of a library within a particular directory.
 * Returns the full pathname, allocated with "ILMalloc", if found.
 * Otherwise returns NULL.
 */
static char *CheckPath(const char *directory, const char *name, int len)
{
	int dirlen = strlen(directory);
	char *path = ILMalloc(dirlen + len + 6);
	if(!path)
	{
		CSOutOfMemory();
	}
	strcpy(path, directory);
	path[dirlen++] = '/';
	ILMemCpy(path + dirlen, name, len);
	strcpy(path + dirlen + len, ".dll");
	if(ILFileExists(path, (char **)0))
	{
		return path;
	}
	ILFree(path);
	return 0;
}

/*
 * Load a library from a specific path.  Returns zero on failure.
 * If "freePath" is non-zero, then free "path" afterwards.
 */
static int LoadLibraryFromPath(const char *path, int freePath)
{
	FILE *file;
	ILImage *image;
	int loadError;

	/* Attempt to open the library file */
	if((file = fopen(path, "rb")) == NULL)
	{
		/* Try again with "r", in case libc does not understand "rb" */
		if((file = fopen(path, "r")) == NULL)
		{
			perror(path);
			if(freePath)
			{
				ILFree((char *)path);
			}
			return 0;
		}
	}

	/* Attempt to load the image */
	loadError = ILImageLoad(file, path, CSCodeGen.context, &image,
							IL_LOADFLAG_FORCE_32BIT |
							IL_LOADFLAG_PRE_VALIDATE);
	if(loadError != 0)
	{
		fclose(file);
		fprintf(stderr, "%s: %s\n", path, ILImageLoadError(loadError));
		if(freePath)
		{
			ILFree((char *)path);
		}
		return 0;
	}
	fclose(file);

	/* Clean up and exit */
	if(freePath)
	{
		ILFree((char *)path);
	}
	return 1;
}

/*
 * Load the contents of a library into the code generator's context.
 * Returns zero if the library load failed.
 */
static int LoadLibrary(const char *name)
{
	int len;
	int index;
	char *path;
	ILImage *image;
	ILAssembly *assem;
	const char *assemName;
	int ch1, ch2;

	/* If the name includes a path, then don't bother searching */
	if(strchr(name, '/') != 0 || strchr(name, '\\') != 0)
	{
		return LoadLibraryFromPath(name, 0);
	}

	/* Strip the ".dll" from the name, if present */
	len = strlen(name);
	if(len >= 4 && !ILStrICmp(name + len - 4, ".dll"))
	{
		len -= 4;
	}

	/* If we already have an assembly with this name,
	   then we assume that we already have the library.
	   It could have been loaded during dynamic linking,
	   or because the programmer specified the same
	   library twice on the compiler command-line */
	image = 0;
	while((image = ILContextNextImage(CSCodeGen.context, image)) != 0)
	{
		assem = ILAssembly_FromToken(image, (IL_META_TOKEN_ASSEMBLY | 1));
		if(assem)
		{
			assemName = ILAssembly_Name(assem);
			index = 0;
			while(assemName[index] != '\0' && index < len)
			{
				/* Use case-insensitive comparisons on assembly names */
				ch1 = assemName[index];
				if(ch1 >= 'a' && ch1 <= 'z')
				{
					ch1 = (ch1 - 'a' + 'A');
				}
				ch2 = name[index];
				if(ch2 >= 'a' && ch2 <= 'z')
				{
					ch2 = (ch2 - 'a' + 'A');
				}
				if(ch1 != ch2)
				{
					break;
				}
				++index;
			}
			if(assemName[index] == '\0' && index == len)
			{
				/* The assembly is already loaded */
				return 1;
			}
		}
	}

	/* Search the library link path for the name */
	for(index = 0; index < num_link_dirs; ++index)
	{
		path = CheckPath(link_dirs[index], name, len);
		if(path)
		{
			return LoadLibraryFromPath(path, 1);
		}
	}

	/* Last try: look in the current directory */
	path = CheckPath(".", name, len);
	if(path)
	{
		return LoadLibraryFromPath(path, 1);
	}

	/* Could not locate the library */
	fprintf(stderr, "%s: No such library\n", name);
	return 0;
}

/*
 * Initialize the code generator for tree building and assembly output.
 */
static int InitCodeGen(void)
{
	FILE *outfile;
	int library;

	/* Attempt to open the assembly output stream */
	if(!CSStringListContains(extension_flags, num_extension_flags,
							 "syntax-check"))
	{
		if(!output_filename || !strcmp(output_filename, "-"))
		{
			outfile = stdout;
		}
		else
		{
			if((outfile = fopen(output_filename, "w")) == NULL)
			{
				perror(output_filename);
				return 1;
			}
		}
	}
	else
	{
		/* We are performing a syntax check only, so don't
		   bother generating the assembly code output */
		outfile = NULL;
	}

	/* Initialize the code generator */
	ILGenInfoInit(&CSCodeGen, progname, outfile, nostdlib_flag);

	/* Set the default "checked" state */
	if(CSStringListContains(extension_flags, num_extension_flags, "checked"))
	{
		CSCodeGen.overflowInsns = 1;
	}
	else if(CSStringListContains(extension_flags, num_extension_flags,
								 "unchecked"))
	{
		CSCodeGen.overflowInsns = 0;
	}
	else
	{
		CSCodeGen.overflowInsns = 0;
	}

	/* Switch to Java mode if necessary */
	if(CSStringListContains(machine_flags, num_machine_flags, "jvm"))
	{
		ILGenInfoToJava(&CSCodeGen);
	}

	/* Load the "mscorlib" library, to get the standard library */
	if(!nostdlib_flag)
	{
		char *name = CSStringListGetValue(extension_flags, num_extension_flags,
										  "stdlib-name");
		if(!name)
		{
			name = "mscorlib";
		}
		if(!LoadLibrary(name))
		{
			return 1;
		}
	}

	/* Load all of the other libraries, in reverse order */
	for(library = num_libraries - 1; library >= 0; --library)
	{
		if(!LoadLibrary(libraries[library]))
		{
			return 1;
		}
	}

	/* Ready to go now */
	return 0;
}

/*
 * Close and destroy the code generator.
 */
static void CloseCodeGen(void)
{
	if(CSCodeGen.asmOutput != stdout && CSCodeGen.asmOutput != NULL)
	{
		fclose(CSCodeGen.asmOutput);
	}
	ILGenInfoDestroy(&CSCodeGen);
}

/*
 * Parse a single input file.
 */
static void ParseFile(FILE *file, const char *filename, int is_stdin)
{
	int posn;

	/* Set up the pre-processor */
	CSPreProcInit(&CSPreProcessorStream, file, filename, !is_stdin);
	if(CSStringListContains(extension_flags, num_extension_flags,
							"latin1-charset"))
	{
		/* Disable UTF-8 pre-processing if the input charset is Latin-1 */
		CSPreProcessorStream.utf8 = 0;
	}
	for(posn = 0; posn < num_pre_defined_symbols; ++posn)
	{
		CSPreProcDefine(&CSPreProcessorStream, pre_defined_symbols[posn]);
	}
	for(posn = 0; posn < num_user_defined_symbols; ++posn)
	{
		CSPreProcDefine(&CSPreProcessorStream, user_defined_symbols[posn]);
	}

	/* If we are only pre-processing, then do that now */
	if(preprocess_flag)
	{
		CSHaveErrors |= Preprocess();
		CSPreProcDestroy(&CSPreProcessorStream);
		return;
	}

	/* Print the name of the file if verbose mode is enabled */
	if(verbose_mode == VERBOSE_FILENAMES)
	{
		fputs("Parsing ", stderr);
		fputs(filename, stderr);
		putc('\n', stderr);
	}

	/* Initialize the lexical analyser */
	CSGetSetKeywords = 0;
	cs_restart(NULL);

	/* Parse the input file */
	/*cs_debug = 1;*/
	CSHaveErrors |= cs_parse();

	/* Destroy the pre-processor stream */
	CSHaveErrors |= CSPreProcessorStream.error;
	CSPreProcDestroy(&CSPreProcessorStream);
}

#ifdef	__cplusplus
};
#endif
