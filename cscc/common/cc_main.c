/*
 * cc_main.c - Main entry point for a "cscc" compiler plug-in.
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

#include "cc_main.h"
#include "cc_preproc.h"

#ifdef	__cplusplus
extern	"C" {
#endif

/*
 * Global variables that are exported to the plugin.
 */
ILGenInfo CCCodeGen;
ILNode *CCParseTree;
ILNode *CCParseTreeEnd;
ILScope *CCGlobalScope;

/*
 * Forward declarations.
 */
static void PreprocessClose(void);
static int InitCodeGen(void);
static void CloseCodeGen(void);
static void ParseFile(FILE *file, const char *filename, int is_stdin);

/*
 * State for the pre-processor.
 */
CCPreProc CCPreProcessorStream;
static FILE *preproc_outfile = 0;
static int preproc_is_stdout = 0;
static const char *preproc_filename = 0;

int CCMain(int argc, char *argv[])
{
	FILE *infile;
	int saw_stdin;
	int posn;
	int status;

	/* Parse the command-line options */
	CCParseCommandLine(argc, argv, CCPluginOptionParseMode,
					   (char *)CCPluginName);

	/* Initialize the plugin */
	if(!CCPluginInit())
	{
		return 1;
	}

	/* Initialize the code generator if we aren't pre-processing */
	if(!preprocess_flag)
	{
		status = InitCodeGen();
		if(status != 0)
		{
			CCPluginShutdown(status);
			return status;
		}
	}
	else if(!CCPluginUsesPreproc)
	{
		/* We can't use a pre-processor for this language */
		fprintf(stderr, _("%s: pre-processing is not supported\n"), progname);
		CCPluginShutdown(1);
		return 1;
	}

	/* Parse all of the input files */
	saw_stdin = 0;
	CCParseTree = 0;
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
				CCHaveErrors |= 1;
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
		CCPluginShutdown(CCHaveErrors != 0);
		return (CCHaveErrors != 0);
	}

	/* If we are only performing a syntax check, then bail out now */
	if(CCStringListContains(extension_flags, num_extension_flags,
							"syntax-check"))
	{
		CloseCodeGen();
		CCPluginShutdown(CCHaveErrors != 0);
		return (CCHaveErrors != 0);
	}

	/* Bail out if nothing was parsed, and we have errors */
	if(!CCParseTree && CCHaveErrors != 0)
	{
		CloseCodeGen();
		CCPluginShutdown(1);
		return 1;
	}

	/* Perform semantic analysis */
	CCCodeGen.semAnalysis = 1;
	CCPluginSemAnalysis();
	CCCodeGen.semAnalysis = 0;

	/* If we are only performing a semantic check, then bail out now */
	if(CCStringListContains(extension_flags, num_extension_flags,
							"semantic-check"))
	{
		CloseCodeGen();
		CCPluginShutdown(CCHaveErrors != 0);
		return (CCHaveErrors != 0);
	}

	/* Generate the code if no errors occurred in the previous phases */
	if(CCHaveErrors == 0 && !CCPluginSkipCodeGen)
	{
		ILGenModulesAndAssemblies(&CCCodeGen);
		if(CCCodeGen.outputIsJava)
		{
			JavaGenDiscard(CCParseTree, &CCCodeGen);
		}
		else
		{
			ILNode_GenDiscard(CCParseTree, &CCCodeGen);
		}
	}

	/* Perform post code generation tasks */
	CCPluginPostCodeGen();

	/* Close the code generator */
	CloseCodeGen();

	/* Done */
	CCPluginShutdown(CCHaveErrors != 0);
	return (CCHaveErrors != 0);
}

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
	CCPreProcSymbol *symbol;
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
		fprintf(stderr, _("%s: warning: -C is not yet supported by "
						  "the C# pre-processor\n"), progname);
	}

	/* Copy the pre-processed data to the output stream */
	expectedFile = 0;
	expectedLine = 0;
	while((size = CCPreProcGetBuffer(&CCPreProcessorStream, buffer, BUFSIZ))
						> 0)
	{
		if(dump_output_format != DUMP_MACROS_ONLY)
		{
			if(!no_preproc_lines_flag)
			{
				/* Output line number information if it has changed */
				if(expectedFile != CCPreProcessorStream.lexerFileName ||
				   expectedLine != CCPreProcessorStream.lexerLineNumber)
				{
					expectedLine = CCPreProcessorStream.lexerLineNumber;
					if(expectedFile != CCPreProcessorStream.lexerFileName)
					{
						expectedFile = CCPreProcessorStream.lexerFileName;
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
				fprintf(stderr, _("%s: output truncated\n"), filename);
				return 1;
			}
		}
	}

	/* Dump the defined macros if necessary */
	if(dump_output_format != DUMP_OUTPUT_ONLY)
	{
		symbol = CCPreProcessorStream.symbols;
		while(symbol != 0)
		{
			fputs("#define ", outfile);
			fputs(symbol->name, outfile);
			putc('\n', outfile);
			symbol = symbol->next;
		}
	}

	/* Finished with this file */
	return CCPreProcessorStream.error;
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
		CCOutOfMemory();
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
	ILImage *image;
	int loadError;

	/* Attempt to load the image */
	loadError = ILImageLoadFromFile(path, CCCodeGen.context, &image,
									IL_LOADFLAG_FORCE_32BIT |
									IL_LOADFLAG_PRE_VALIDATE, 1);
	if(loadError == 0)
	{
		if(!ILAssemblyCreateImport(CCCodeGen.image, image))
		{
			CCOutOfMemory();
		}
	}

	/* Clean up and exit */
	if(freePath)
	{
		ILFree((char *)path);
	}
	return (loadError == 0);
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
	while((image = ILContextNextImage(CCCodeGen.context, image)) != 0)
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
	fprintf(stderr, _("%s: No such library\n"), name);
	return 0;
}

/*
 * Initialize the code generator for tree building and assembly output.
 */
static int InitCodeGen(void)
{
	FILE *outfile;
	int library;
	int useBuiltinLibrary;

	/* Attempt to open the assembly output stream */
	if(!CCStringListContains(extension_flags, num_extension_flags,
							 "syntax-check") &&
	   !CCStringListContains(extension_flags, num_extension_flags,
							 "semantic-check") &&
	   !CCPluginSkipCodeGen)
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
		/* We are performing a syntax or semantic check only, so don't
		   bother generating the assembly code output */
		outfile = NULL;
	}

	/* Determine if we need the builtin mscorlib-replacement library.
	   This is mainly of use to the "cscctest" test suite */
	useBuiltinLibrary = CCStringListContains(extension_flags,
											 num_extension_flags,
											 "builtin-library");

	/* Initialize the code generator */
	ILGenInfoInit(&CCCodeGen, progname,
				  CCStringListGetValue(extension_flags, num_extension_flags,
				  					   "target-assembly-name"),
				  outfile, useBuiltinLibrary);
	CCCodeGen.debugFlag = debug_flag;

	/* Set the default "checked" state */
	if(CCStringListContains(extension_flags, num_extension_flags, "checked"))
	{
		CCCodeGen.overflowInsns = 1;
	}
	else if(CCStringListContains(extension_flags, num_extension_flags,
								 "unchecked"))
	{
		CCCodeGen.overflowInsns = 0;
	}
	else
	{
		CCCodeGen.overflowInsns = 0;
	}

	/* Switch to JVM mode if necessary */
	if(CCStringListContains(machine_flags, num_machine_flags, "jvm"))
	{
		if(!CCPluginJVMSupported)
		{
			fprintf(stderr, _("%s: compiling to the JVM is not supported\n"),
				    progname);
			return 1;
		}
		ILGenInfoToJava(&CCCodeGen);
	}

	/* Load the "mscorlib" library, to get the standard library */
	if(!nostdlib_flag)
	{
		char *name = CCStringListGetValue(extension_flags, num_extension_flags,
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
	if(CCCodeGen.asmOutput != stdout && CCCodeGen.asmOutput != NULL)
	{
		fclose(CCCodeGen.asmOutput);
	}
	ILGenInfoDestroy(&CCCodeGen);
}

/*
 * Parse a single input file.
 */
static void ParseFile(FILE *file, const char *filename, int is_stdin)
{
	int posn;

	/* Set up the pre-processor or the simple file reader */
	if(CCPluginUsesPreproc)
	{
		CCPreProcInit(&CCPreProcessorStream, file, filename, !is_stdin);
		if(CCStringListContains(extension_flags, num_extension_flags,
								"latin1-charset"))
		{
			/* Disable UTF-8 pre-processing if the input charset is Latin-1 */
			CCPreProcessorStream.utf8 = 0;
		}
		for(posn = 0; posn < num_pre_defined_symbols; ++posn)
		{
			CCPreProcDefine(&CCPreProcessorStream, pre_defined_symbols[posn]);
		}
		for(posn = 0; posn < num_user_defined_symbols; ++posn)
		{
			CCPreProcDefine(&CCPreProcessorStream, user_defined_symbols[posn]);
		}

		/* If we are only pre-processing, then do that now */
		if(preprocess_flag)
		{
			CCHaveErrors |= Preprocess();
			CCPreProcDestroy(&CCPreProcessorStream);
			return;
		}
	}
	else
	{
		CCPreProcessorStream.stream = file;
		CCPreProcessorStream.lineNumber = 1;
		CCPreProcessorStream.filename =
			ILInternString((char *)filename, -1).string;
		CCPreProcessorStream.lexerLineNumber = 1;
		CCPreProcessorStream.lexerFileName = CCPreProcessorStream.filename;
	}

	/* Print the name of the file if verbose mode is enabled */
	if(verbose_mode == VERBOSE_FILENAMES)
	{
		fprintf(stderr, _("Parsing %s\n"), filename);
	}

	/* Initialize the lexical analyser */
	CCPluginRestart(NULL);

	/* Parse the input file */
	CCHaveErrors |= CCPluginParse();

	/* Destroy the pre-processor stream */
	if(CCPluginUsesPreproc)
	{
		CCHaveErrors |= CCPreProcessorStream.error;
		CCPreProcDestroy(&CCPreProcessorStream);
	}
	else if(!is_stdin)
	{
		fclose(file);
	}
}

int CCPluginInput(char *buf, int maxSize)
{
	if(CCPluginUsesPreproc)
	{
		return CCPreProcGetBuffer(&CCPreProcessorStream, buf, maxSize);
	}
	else
	{
		FILE *file = CCPreProcessorStream.stream;
		int ch, len;
		len = 0;
		CCPreProcessorStream.lexerLineNumber = CCPreProcessorStream.lineNumber;
		CCPreProcessorStream.lexerFileName = CCPreProcessorStream.filename;
		while(maxSize > 0)
		{
			if((ch = getc(file)) == EOF)
			{
				break;
			}
			else if(ch == '\n')
			{
				/* Unix end of line sequence */
				*buf = '\n';
				++len;
				++(CCPreProcessorStream.lineNumber);
				break;
			}
			else if(ch == '\r')
			{
				/* MS-DOS or Mac end of line sequence */
				ch = getc(file);
				if(ch != '\n' && ch != EOF)
				{
					ungetc(ch, file);
				}
				*buf = '\n';
				++len;
				++(CCPreProcessorStream.lineNumber);
				break;
			}
			else
			{
				*buf++ = (char)ch;
				++len;
				--maxSize;
			}
		}
		return len;
	}
}

void CCPluginParseError(char *msg, char *text)
{
	char *newmsg;
	int posn, outposn;

	if(!strcmp(msg, "parse error") || !strcmp(msg, "syntax error"))
	{
		/* This error is pretty much useless at telling the user
		   what is happening.  Try to print a better message
		   based on the current input token */
	simpleError:
		if(text && *text != '\0')
		{
			CCError(_("parse error at or near `%s'"), text);
		}
		else
		{
			CCError(_("parse error"));
		}
	}
	else if(!strncmp(msg, "parse error, expecting `", 24))
	{
		/* We have to quote the token names in the "%token" declarations
		   within yacc grammars so that byacc doesn't mess up the output.
		   But the quoting causes Bison to output quote characters in
		   error messages which look awful.  This code attempts to fix
		   things up */
		newmsg = ILDupString(msg);
	expectingError:
		if(newmsg)
		{
			posn = 0;
			outposn = 0;
			while(newmsg[posn] != '\0')
			{
				if(newmsg[posn] == '`')
				{
					if(newmsg[posn + 1] == '"' && newmsg[posn + 2] == '`')
					{
						/* Convert <`"`> into <`> */
						posn += 2;
						newmsg[outposn++] = '`';
					}
					else if(newmsg[posn + 1] == '"')
					{
						/* Convert <`"> into <> */
						++posn;
					}
					else if(newmsg[posn + 1] == '`' ||
					        newmsg[posn + 1] == '\'')
					{
						/* Convert <``> or <`'> into <`> */
						++posn;
						newmsg[outposn++] = '`';
					}
					else
					{
						/* Ordinary <`> on its own */
						newmsg[outposn++] = '`';
					}
				}
				else if(newmsg[posn] == '\\')
				{
					/* Ignore backslashes in the input */
				}
				else if(newmsg[posn] == '"' && newmsg[posn + 1] == '\'')
				{
					/* Convert <"'> into <> */
					++posn;
				}
				else if(newmsg[posn] == '\'' && newmsg[posn + 1] == '"' &&
				        newmsg[posn + 2] == '\'')
				{
					/* Convert <'"'> into <'> */
					posn += 2;
					newmsg[outposn++] = '\'';
				}
				else if(newmsg[posn] == '\'' && newmsg[posn + 1] == '\'')
				{
					/* Convert <''> into <'> */
					++posn;
					newmsg[outposn++] = '\'';
				}
				else
				{
					/* Ordinary character */
					newmsg[outposn++] = newmsg[posn];
				}
				++posn;
			}
			newmsg[outposn] = '\0';
			if(text && *text != '\0')
			{
				CCError(_("%s, at or near `%s'"), newmsg, text);
			}
			else
			{
				CCError("%s", newmsg);
			}
			ILFree(newmsg);
		}
		else
		{
			if(text && *text != '\0')
			{
				CCError(_("%s at or near `%s'"), msg, text);
			}
			else
			{
				CCError("%s", msg);
			}
		}
	}
	else if(!strncmp(msg, "parse error, unexpected ", 24))
	{
		/* The error probably has the form "parse error, unexpected ...,
		   expecting ..." - strip out the "unexpected" part */
		posn = 24;
		while(msg[posn] != '\0' &&
			  strncmp(msg + posn, ", expecting ", 12) != 0)
		{
			++posn;
		}
		if(msg[posn] == '\0')
		{
			goto simpleError;
		}
		newmsg = (char *)ILMalloc(strlen(msg) + 1);
		if(!newmsg)
		{
			goto defaultError;
		}
		strcpy(newmsg, "parse error, expecting ");
		strcat(newmsg, msg + posn + 12);
		goto expectingError;
	}
	else
	{
		/* The parser has probably included information as to what
		   is expected in this context, so report that */
	defaultError:
		if(text && *text != '\0')
		{
			CCError(_("%s at or near `%s'"), msg, text);
		}
		else
		{
			CCError("%s", msg);
		}
	}
}

void CCPluginAddTopLevel(ILNode *node)
{
	if(!node)
	{
		return;
	}
	if(!CCParseTree)
	{
		CCParseTree = ILNode_List_create();
		CCParseTreeEnd = 0;
	}
	if(!CCParseTreeEnd)
	{
		CCParseTreeEnd = CCParseTree;
	}
	ILNode_List_Add(CCParseTreeEnd, node);
	if(((ILNode_List *)CCParseTreeEnd)->rest)
	{
		CCParseTreeEnd = (ILNode *)(((ILNode_List *)CCParseTreeEnd)->rest);
	}
}

/*
 * The following functions are required by the treecc node routines.
 */

char *yycurrfilename(void)
{
	return CCPreProcessorStream.lexerFileName;
}

long yycurrlinenum(void)
{
	return CCPreProcessorStream.lexerLineNumber;
}

void yynodefailed(void)
{
	CCOutOfMemory();
}

#ifdef	__cplusplus
};
#endif
