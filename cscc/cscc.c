/*
 * cscc.c - Front-end that wraps around compiler language plug-ins.
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

Environment variables:

	CSCC_INCLUDE_PATH		Where to look for included system files.
	CSCC_INCLUDE_CPP_PATH	Where to look for included C++ system files.
	CSCC_LIB_PATH			Where to look for link libraries.
	CSCC_PLUGINS_PATH		Where to look for language plug-ins.
	CSCC_ILASM_PATH			Pathname of the "ilasm" program.
	CSCC_ILALINK_PATH		Pathname of the "ilalink" program.

*/

#include "cs_internal.h"

#ifdef	__cplusplus
extern	"C" {
#endif

/*
 * The default system include path.  We look in "lib" first,
 * just in case there is a pre-compiled "dll" for a package.
 */
#ifdef CSCC_LIB_PREFIX
#define	INCLUDE_PATH	\
			CSCC_LIB_PREFIX "/lib:" \
			CSCC_LIB_PREFIX "/include:" \
			"/usr/local/lib/cscc/lib:" \
			"/usr/local/lib/cscc/include:" \
			"/usr/lib/cscc/lib:" \
			"/usr/lib/cscc/include"
#else
#define	INCLUDE_PATH	\
			"/usr/local/lib/cscc/lib:" \
			"/usr/local/lib/cscc/include:" \
			"/usr/lib/cscc/lib:"
			"/usr/lib/cscc/include"
#endif

/*
 * The default system C++ include path.
 */
#ifdef CSCC_LIB_PREFIX
#define	INCLUDE_CPP_PATH	\
			"/usr/local/lib/cscc/include/c++:" \
			"/usr/lib/cscc/include/c++"
#else
#define	INCLUDE_CPP_PATH	\
			CSCC_LIB_PREFIX "/include/c++:" \
			"/usr/local/lib/cscc/include/c++:" \
			"/usr/lib/cscc/include/c++"
#endif

/*
 * The default system library link path.
 */
#ifdef CSCC_LIB_PREFIX
#define	LIB_PATH	\
			CSCC_LIB_PREFIX "/lib:" \
			"/usr/local/lib/cscc/lib:" \
			"/usr/lib/cscc/lib"
#else
#define	LIB_PATH	\
			"/usr/local/lib/cscc/lib:" \
			"/usr/lib/cscc/lib"
#endif

/*
 * The default plugins path.
 */
#ifdef CSCC_LIB_PREFIX
#define	PLUGINS_PATH	\
			CSCC_LIB_PREFIX "/plugins:" \
			"/usr/local/lib/cscc/plugins:" \
			"/usr/lib/cscc/plugins"
#else
#define	PLUGINS_PATH	\
			"/usr/local/lib/cscc/plugins:" \
			"/usr/lib/cscc/plugins"
#endif

/*
 * The default search path for "ilasm" and "ilalink"
 * if not found on the real PATH.
 */
#ifdef CSCC_BIN_PREFIX
#define	ILASM_PATH	\
			CSCC_BIN_PREFIX ":" \
			"/usr/local/bin:" \
			"/usr/bin:" \
			"/bin"
#else
#define	ILASM_PATH	\
			"/usr/local/bin:" \
			"/usr/bin:" \
			"/bin"
#endif

/*
 * File processing types.
 */
#define	FILEPROC_TYPE_BINARY		0	/* Binary for linking */
#define	FILEPROC_TYPE_IL			1	/* Process with the assembler */
#define	FILEPROC_TYPE_JL			2	/* Process with the assembler (JVM) */
#define	FILEPROC_TYPE_SINGLE		3	/* Process with single-file plugin */
#define	FILEPROC_TYPE_MULTIPLE		4	/* Process with multiple-file plugin */
#define	FILEPROC_TYPE_DONE			5	/* Already processed */
#define	FILEPROC_TYPE_UNKNOWN		6	/* Unknown format: can't process */

/*
 * Global variables.
 */
static char *ilasm_program = 0;
static char *ilalink_program = 0;
static char *csharp_compiler = 0;
static char **plugin_list;
static int *file_proc_types;

/*
 * Forward declarations.
 */
static void ParseCommandLine(int argc, char *argv[]);
static char *FindLanguagePlugin(char *name, char *ext);
static int CompareExtensions(const char *ext1, const char *ext2);
static int IsSinglePlugin(const char *filename);
static int ProcessWithAssembler(const char *filename, int jvmMode);
static int ProcessWithPlugin(const char *filename, char *plugin,
							 int filenum, int isMultiple);
static int LinkExecutable(void);

int main(int argc, char *argv[])
{
	int filenum;
	char *filename;
	int len;
	char *language;
	char *extension;
	char *plugin;
	int status = 0;
	int newstatus;

	/* Parse the command-line options */
	ParseCommandLine(argc, argv);

	/* Find the language plugins to be used for each of the input files */
	plugin_list = (char **)ILCalloc(num_input_files, sizeof(char *));
	file_proc_types = (int *)ILCalloc(num_input_files, sizeof(int));
	if(!plugin_list || !file_proc_types)
	{
		CSOutOfMemory();
	}
	for(filenum = 0; filenum < num_input_files; ++filenum)
	{
		/* Get the filename and determine which language it is expressed in */
		filename = input_files[filenum];
		if(!strcmp(filename, "-"))
		{
			/* Process stdin using the default language */
			language = prog_language_name;
			extension = 0;
		}
		else
		{
			/* Determine how to process the file based on its extension */
			len = strlen(filename);
			while(len > 0 && filename[len - 1] != '/' &&
			      filename[len - 1] != '\\' && filename[len - 1] != ':' &&
				  filename[len - 1] != '.')
			{
				--len;
			}
			if(len > 0 && filename[len - 1] == '.')
			{
				/* Check for special extensions */
				if(CompareExtensions(filename + len, "o") ||
				   CompareExtensions(filename + len, "obj") ||
				   CompareExtensions(filename + len, "dll") ||
				   CompareExtensions(filename + len, "exe") ||
				   CompareExtensions(filename + len, "a"))
				{
					/* This is a binary file to be passed to the linker */
					language = 0;
					extension = 0;
				}
				else if(prog_language != PROG_LANG_DEFAULT)
				{
					/* Use the command-line specified language always */
					language = prog_language_name;
					extension = 0;
				}
				else if(CompareExtensions(filename + len, "il") ||
						CompareExtensions(filename + len, "s") ||
						CompareExtensions(filename + len, "S"))
				{
					/* This is an assembly file */
					language = "il";
					extension = 0;
				}
				else if(CompareExtensions(filename + len, "jl"))
				{
					/* This is an assembly file with JVM bytecode */
					language = "jl";
					extension = 0;
				}
				else if(CompareExtensions(filename + len, "cs"))
				{
					/* This is a C# source file */
					language = "cs";
					extension = "cs";
				}
				else
				{
					/* Use the extension as the language name */
					language = filename + len;
					extension = filename + len;
				}
			}
			else
			{
				/* No extension, so use the default language */
				language = prog_language_name;
				extension = 0;
			}
		}

		/* If no language, then it is a binary that we should skip */
		if(!language)
		{
			continue;
		}

		/* Determine the appropriate way to process the file */
		if(!strcmp(language, "il"))
		{
			/* Assemble this input file using "ilasm" */
			file_proc_types[filenum] = FILEPROC_TYPE_IL;
		}
		else if(!strcmp(language, "jl"))
		{
			/* Assemble this input file using "ilasm" in JVM mode */
			file_proc_types[filenum] = FILEPROC_TYPE_JL;
		}
		else if((plugin = FindLanguagePlugin(language, extension)) != 0)
		{
			/* Compile this input file using a language-specific plug-in */
			plugin_list[filenum] = plugin;
			if(IsSinglePlugin(plugin))
			{
				file_proc_types[filenum] = FILEPROC_TYPE_SINGLE;
			}
			else
			{
				file_proc_types[filenum] = FILEPROC_TYPE_MULTIPLE;
			}
		}
		else
		{
			/* This language is not understood or there is no plug-in for it */
			file_proc_types[filenum] = FILEPROC_TYPE_UNKNOWN;
			fprintf(stderr, "%s: language %s not recognized or "
							"plug-in not found\n", progname, language);
			if(!status)
			{
				status = 1;
			}
		}
	}

	/* Process each of the input files in turn */
	for(filenum = 0; filenum < num_input_files; ++filenum)
	{
		filename = input_files[filenum];
		switch(file_proc_types[filenum])
		{
			case FILEPROC_TYPE_BINARY:
			{
				/* Add the binary to the list of files to be linked */
				CSAddLinkFile(filename, 0);
			}
			break;

			case FILEPROC_TYPE_IL:
			{
				/* Assemble this input file using "ilasm" */
				newstatus = ProcessWithAssembler(filename, 0);
				if(newstatus && !status)
				{
					status = newstatus;
				}
			}
			break;

			case FILEPROC_TYPE_JL:
			{
				/* Assemble this input file using "ilasm" in JVM mode */
				newstatus = ProcessWithAssembler(filename, 1);
				if(newstatus && !status)
				{
					status = newstatus;
				}
			}
			break;

			case FILEPROC_TYPE_SINGLE:
			{
				/* Compile this input file using a language-specific plug-in */
				newstatus = ProcessWithPlugin
								(filename, plugin_list[filenum], filenum, 0);
				if(newstatus && !status)
				{
					status = newstatus;
				}
			}
			break;

			case FILEPROC_TYPE_MULTIPLE:
			{
				/* Compile this input file and all other files for the
				   same language using a language-specific plug-in */
				newstatus = ProcessWithPlugin
								(filename, plugin_list[filenum], filenum, 1);
				if(newstatus && !status)
				{
					status = newstatus;
				}
			}
			break;

			default:	break;
		}
	}

	/* Link the final executable */
	if(!CSStringListContains(extension_flags, num_extension_flags,
							 "syntax-check") &&
	   !CSStringListContains(extension_flags, num_extension_flags,
							 "semantic-check"))
	{
		if(status == 0 && executable_flag)
		{
			status = LinkExecutable();
		}
	}

	/* Delete temporary files that were created prior to the link */
	for(len = 0; len < num_files_to_link; ++len)
	{
		if(files_to_link_temp[len])
		{
			/* Delete this temporary object file */
			ILDeleteFile(files_to_link[len]);
		}
	}

	/* Done */
	return status;
}

/*
 * Change the extension on a filename and return a new filename.
 */
static char *ChangeExtension(char *filename, char *ext)
{
	int len = strlen(filename);
	char *newpath;
	while(len > 0 && filename[len - 1] != '.' &&
	      filename[len - 1] != '/' && filename[len - 1] != '\\' &&
		  filename[len - 1] != ':')
	{
		--len;
	}
	if(len < 1 || filename[len - 1] != '.')
	{
		len = strlen(filename);
	}
	else
	{
		--len;
	}
	newpath = (char *)ILMalloc(len + strlen(ext) + 2);
	if(!newpath)
	{
		CSOutOfMemory();
	}
	ILMemCpy(newpath, filename, len);
	newpath[len] = '.';
	strcpy(newpath + len + 1, ext);
	return newpath;
}

/*
 * Parse the command-line options.
 */
static void ParseCommandLine(int argc, char *argv[])
{
	char *env;

	/* Call the centralised option parser */
	CSParseCommandLine(argc, argv, CMDLINE_PARSE_CSCC, "cscc");

	/* Add the system include directories */
	if(!nostdinc_flag)
	{
		env = getenv("CSCC_INCLUDE_PATH");
		if(env && *env != '\0')
		{
			CSAddPathStrings(&sys_include_dirs, &num_sys_include_dirs, env);
		}
		else
		{
			CSAddPathStrings(&sys_include_dirs, &num_sys_include_dirs,
							 INCLUDE_PATH);
		}
	}
	if(!nostdinc_cpp_flag)
	{
		env = getenv("CSCC_INCLUDE_CPP_PATH");
		if(env && *env != '\0')
		{
			CSAddPathStrings(&sys_cpp_include_dirs, &num_sys_cpp_include_dirs,
							 env);
		}
		else
		{
			CSAddPathStrings(&sys_cpp_include_dirs, &num_sys_cpp_include_dirs,
							 INCLUDE_CPP_PATH);
		}
	}

	/* Add the system link directories */
	if(!nostdlib_flag)
	{
		env = getenv("CSCC_LIB_PATH");
		if(env && *env != '\0')
		{
			CSAddPathStrings(&sys_link_dirs, &num_sys_link_dirs, env);
		}
		else
		{
			CSAddPathStrings(&sys_link_dirs, &num_sys_link_dirs, LIB_PATH);
		}
	}

	/* If we are not building an executable, then suppress the entry point */
	if(!executable_flag || shared_flag)
	{
		entry_point = 0;
	}

	/* Determine the default output filename */
	if(!output_filename)
	{
		if(executable_flag)
		{
			/* The default output filename is "a.out.dll" or "a.out.exe" */
			if(shared_flag)
			{
				output_filename = "a.out.dll";
			}
			else
			{
				output_filename = "a.out.exe";
			}
		}
		else if(compile_flag)
		{
			/* Use the name of the source file with a ".obj" extension */
			output_filename = ChangeExtension(input_files[0], "obj");
		}
		else if(assemble_flag)
		{
			/* Use the source file name with a ".il" or ".jl" extension */
			if(!CSStringListContains(machine_flags, num_machine_flags, "jvm"))
			{
				output_filename = ChangeExtension(input_files[0], "il");
			}
			else
			{
				output_filename = ChangeExtension(input_files[0], "jl");
			}
		}
		else if(!preprocess_flag)
		{
			/* Instead of the typical "a.out" used by C/C++ compilers,
			   we use "a.dll" and "a.exe" instead, to reflect the
			   Windows-ish nature of IL binaries */
			if(shared_flag)
			{
				output_filename = "a.dll";
			}
			else
			{
				output_filename = "a.exe";
			}
		}
	}
	else if(executable_flag && !shared_flag)
	{
		/* Set the "shared" flag if the executable name ends in ".dll" */
		int len = strlen(output_filename);
		if(len >= 4 && !ILStrICmp(output_filename + len - 4, ".dll"))
		{
			shared_flag = 1;
		}
	}

	/* Set "prog_language_name" to a reasonable default */
	if(!prog_language_name)
	{
		if(prog_language == PROG_LANG_IL)
		{
			prog_language_name = "il";
		}
		else if(prog_language == PROG_LANG_JL)
		{
			prog_language_name = "jl";
		}
		else
		{
			prog_language_name = "cs";
		}
	}
}

/*
 * Determine if a file is present.  Returns the filename,
 * which may be altered to include an extension.
 */
static char *FilePresent(char *filename)
{
	char *newExePath = 0;
	if(!ILFileExists(filename, &newExePath))
	{
		return 0;
	}
	if(newExePath)
	{
		return newExePath;
	}
	else
	{
		return filename;
	}
}

/*
 * Search a path list for a particular executable.
 */
static char *SearchPath(char *path, char *name)
{
	int len;
	int namelen = strlen(name);
	char *temppath;
	char *newpath;
	if(!path)
	{
		return 0;
	}
	while(*path != '\0')
	{
		if(*path == ' ' || *path == ':')
		{
			++path;
			continue;
		}
		len = 1;
		while(path[len] != ':' && path[len] != '\0')
		{
			++len;
		}
		while(len > 0 && path[len - 1] == ' ')
		{
			--len;
		}
		temppath = (char *)ILMalloc(len + namelen + 2);
		if(!temppath)
		{
			CSOutOfMemory();
		}
		strncpy(temppath, path, len);
		temppath[len] = '/';
		strcpy(temppath + len + 1, name);
		if((newpath = FilePresent(temppath)) != 0)
		{
			return newpath;
		}
		ILFree(temppath);
		path += len;
	}
	return 0;
}

/*
 * Find the plugin for a specific key, testing for
 * either the single or multiple file case.
 */
static char *FindKeyPluginEither(char *key, int lowerCase, int singleFile)
{
	char *name;
	char *path;
	char *newpath;
	int len;

	/* Convert the key into lower case if necessary */
	if(lowerCase)
	{
		char *newkey = (char *)ILMalloc(strlen(key) + 1);
		int sawUpper;
		if(!newkey)
		{
			CSOutOfMemory();
		}
		strcpy(newkey, key);
		key = newkey;
		sawUpper = 0;
		while(*newkey != '\0')
		{
			if(*newkey >= 'A' && *newkey <= 'Z')
			{
				*newkey = (*newkey - 'A' + 'a');
				sawUpper = 1;
			}
			++newkey;
		}
		if(!sawUpper)
		{
			/* No upper case characters, so we've already checked this */
			ILFree(key);
			return 0;
		}
	}

	/* Look for an option named "-fplugin-key-path" */
	name = (char *)ILMalloc(strlen(key) + 13);
	if(!name)
	{
		CSOutOfMemory();
	}
	strcpy(name, "plugin-");
	strcat(name, key);
	strcat(name, "-path");
	path = CSStringListGetValue(extension_flags, num_extension_flags, name);
	ILFree(name);
	if(path)
	{
		return path;
	}

	/* Search the CSCC_PLUGINS_PATH */
	name = (char *)ILMalloc(strlen(key) + 9);
	if(!name)
	{
		CSOutOfMemory();
	}
	strcpy(name, "cscc-");
	strcat(name, key);
	if(singleFile)
	{
		strcat(name, "-s");
	}
	path = SearchPath(getenv("CSCC_PLUGINS_PATH"), name);
	if(path)
	{
		ILFree(name);
		return path;
	}

	/* Search the default plugins path */
	path = SearchPath(PLUGINS_PATH, name);
	if(path)
	{
		ILFree(name);
		return path;
	}

	/* If argv[0] contains a directory, then look there */
	if(strchr(progname, '/') != 0)
	{
		len = strlen(progname);
		while(len > 0 && progname[len - 1] != '/')
		{
			--len;
		}
		path = (char *)ILMalloc(len + strlen(name) + 1);
		if(!path)
		{
			CSOutOfMemory();
		}
		strncpy(path, progname, len);
		strcpy(path + len, name);
		if((newpath = FilePresent(path)) != 0)
		{
			ILFree(name);
			return newpath;
		}
		ILFree(path);
	}

	/* Search the normal execution PATH */
	path = SearchPath(getenv("PATH"), name);
	if(path)
	{
		ILFree(name);
		return path;
	}

	/* Could not find the plugin */
	ILFree(name);
	return 0;
}

/*
 * Find the plugin for a specific key.
 */
static char *FindKeyPlugin(char *key, int lowerCase)
{
	char *path;

	/* Look for a multiple file plugin */
	path = FindKeyPluginEither(key, lowerCase, 0);
	if(path)
	{
		return path;
	}

	/* Look for a single file plugin */
	return FindKeyPluginEither(key, lowerCase, 1);
}

/*
 * Find the plugin for a specific language.
 */
static char *FindLanguagePlugin(char *name, char *ext)
{
	char *plugin;

	/* Special check for C# so we only fetch the compiler once.
	   This increases efficiency slightly if there are multiple
	   .cs files specified on the command-line */
	if(!strcmp(name, "csharp") || !strcmp(name, "cs"))
	{
		if(csharp_compiler)
		{
			return csharp_compiler;
		}
		csharp_compiler = FindKeyPlugin("cs", 0);
		if(!csharp_compiler)
		{
			csharp_compiler = FindKeyPlugin("csharp", 0);
		}
		return csharp_compiler;
	}

	/* Look for the plugin based on the language name first */
	if(name)
	{
		plugin = FindKeyPlugin(name, 0);
		if(plugin)
		{
			return plugin;
		}
	}

	/* Look for the plugin based on the extension */
	if(ext)
	{
		plugin = FindKeyPlugin(ext, 0);
		if(plugin)
		{
			return plugin;
		}
		plugin = FindKeyPlugin(ext, 1);
		if(plugin)
		{
			return plugin;
		}
	}

	/* Could not find a suitable plugin */
	return 0;
}

/*
 * Find the "ilasm" program.
 */
static void FindILAsmProgram(void)
{
	char *path;

	/* Check for a "-filasm-path" option on the command-line */
	path = CSStringListGetValue(extension_flags, num_extension_flags,
								"ilasm-path");
	if(path)
	{
		ilasm_program = path;
		return;
	}

	/* Use the CSCC_ILASM_PATH environment variable if present */
	path = getenv("CSCC_ILASM_PATH");
	if(path && *path != '\0' && (path = FilePresent(path)) != 0)
	{
		ilasm_program = path;
		return;
	}

	/* Search the contents of PATH */
	path = SearchPath(getenv("PATH"), "ilasm");
	if(path)
	{
		ilasm_program = path;
		return;
	}

	/* Search the contents of the default path */
	path = SearchPath(ILASM_PATH, "ilasm");
	if(path)
	{
		ilasm_program = path;
		return;
	}

	/* Could not find "ilasm", so bail out of the compiler */
	fprintf(stderr, "%s: could not locate the `ilasm' program\n", progname);
	exit(1);
}

/*
 * Find the "ilalink" program.
 */
static void FindILALinkProgram(void)
{
	char *path;

	/* Check for a "-filalink-path" option on the command-line */
	path = CSStringListGetValue(extension_flags, num_extension_flags,
								"ilalink-path");
	if(path)
	{
		ilalink_program = path;
		return;
	}

	/* Use the CSCC_ILALINK_PATH environment variable if present */
	path = getenv("CSCC_ILALINK_PATH");
	if(path && *path != '\0' && (path = FilePresent(path)) != 0)
	{
		ilalink_program = path;
		return;
	}

	/* Search the contents of PATH */
	path = SearchPath(getenv("PATH"), "ilalink");
	if(path)
	{
		ilalink_program = path;
		return;
	}

	/* Search the contents of the default path */
	path = SearchPath(ILASM_PATH, "ilalink");
	if(path)
	{
		ilalink_program = path;
		return;
	}

	/* Could not find "ilalink", so bail out of the compiler */
	fprintf(stderr, "%s: could not locate the `ilalink' program\n", progname);
	exit(1);
}

/*
 * Compare two file extensions for equality, while ignoring case.
 */
static int CompareExtensions(const char *ext1, const char *ext2)
{
	while(*ext1 != '\0' && *ext2 != '\0')
	{
		if(*ext1 >= 'A' && *ext1 <= 'Z')
		{
			if((*ext1 - 'A' + 'a') != *ext2)
			{
				return 0;
			}
		}
		else
		{
			if(*ext1 != *ext2)
			{
				return 0;
			}
		}
		++ext1;
		++ext2;
	}
	return (*ext1 == '\0' && *ext2 == '\0');
}

/*
 * Determine if a filename corresponds to a single-file plugin.
 */
static int IsSinglePlugin(const char *filename)
{
	int len = strlen(filename);

	/* Strip off the ".exe" extension if we are on a Windows system */
	if(len > 4 &&
	   (filename[len - 1] == 'e' || filename[len - 1] == 'E') &&
	   (filename[len - 2] == 'x' || filename[len - 2] == 'X') &&
	   (filename[len - 3] == 'e' || filename[len - 3] == 'E') &&
	   filename[len - 4] == '.')
	{
		len -= 4;
	}

	/* Determine if the executable name ends in "-s" */
	if(len > 2 &&
	   (filename[len - 1] == 's' || filename[len - 1] == 'S') &&
	   filename[len - 2] == '.')
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

/*
 * Process an input file using the assembler.
 */
static int ProcessWithAssembler(const char *filename, int jvmMode)
{
	FindILAsmProgram();
	return 0;
}

/*
 * Add an argument to a list of command-line arguments.
 */
static void AddArgument(char ***list, int *num, char *str)
{
	if(((*num) & 15) == 0)
	{
		char **newlist = (char **)ILRealloc(*list, sizeof(char *) *
												   (*num + 16));
		if(!newlist)
		{
			CSOutOfMemory();
		}
		*list = newlist;
	}
	(*list)[*num] = str;
	++(*num);
}

/*
 * Execute a child process and wait for it to exit.
 * Returns the status code.
 */
static int ExecChild(char **argv, const char *filename)
{
	int status;

	/* Dump the filename or command-line if we are in verbose mode */
	if(verbose_mode == VERBOSE_CMDLINES)
	{
		int posn;
		for(posn = 0; argv[posn] != 0; ++posn)
		{
			if(posn != 0)
			{
				putc(' ', stderr);
			}
			fputs(argv[posn], stderr);
		}
		putc('\n', stderr);
	}

	/* Use the system-specifc process spawn routine */
	status = ILSpawnProcess(argv);
	if(status < 0)
	{
		return 1;
	}
	else
	{
		return status;
	}
}

/*
 * Process an input file using a plug-in.
 */
static int ProcessWithPlugin(const char *filename, char *plugin,
						     int filenum, int isMultiple)
{
	char **cmdline;
	int cmdline_size;
	int posn, status;
	char *asm_output;
	char *obj_output;

	/* If we are compiling to ".obj" or an executable, then
	   get the location of "ilasm" now.  There's no point
	   invoking the plug-in if we cannot assemble the output
	   when it terminates */
	if(compile_flag || executable_flag)
	{
		FindILAsmProgram();
	}

	/* Build the command-line for the plug-in */
	cmdline = 0;
	cmdline_size = 0;
	AddArgument(&cmdline, &cmdline_size, plugin);
	if(preprocess_flag)
	{
		AddArgument(&cmdline, &cmdline_size, "-E");
		if(no_preproc_lines_flag)
		{
			AddArgument(&cmdline, &cmdline_size, "-P");
		}
		if(preproc_comments_flag)
		{
			AddArgument(&cmdline, &cmdline_size, "-C");
		}
		if(dump_output_format == DUMP_MACROS_ONLY)
		{
			AddArgument(&cmdline, &cmdline_size, "-dM");
		}
		else if(dump_output_format == DUMP_MACROS_AND_OUTPUT)
		{
			AddArgument(&cmdline, &cmdline_size, "-dD");
		}
	}
	if(debug_flag)
	{
		AddArgument(&cmdline, &cmdline_size, "-g");
	}
	if(disable_optimizations)
	{
		AddArgument(&cmdline, &cmdline_size, "-O0");
	}
	else if(optimize_flag == 1)
	{
		AddArgument(&cmdline, &cmdline_size, "-O");
	}
	else if(optimize_flag == 2)
	{
		AddArgument(&cmdline, &cmdline_size, "-O2");
	}
	else if(optimize_flag == 3)
	{
		AddArgument(&cmdline, &cmdline_size, "-O3");
	}
	if(undef_flag)
	{
		AddArgument(&cmdline, &cmdline_size, "-undef");
	}
	for(posn = 0; posn < num_user_defined_symbols; ++posn)
	{
		AddArgument(&cmdline, &cmdline_size, "-D");
		AddArgument(&cmdline, &cmdline_size, user_defined_symbols[posn]);
	}
	for(posn = 0; posn < num_pre_defined_symbols; ++posn)
	{
		AddArgument(&cmdline, &cmdline_size, "-D");
		AddArgument(&cmdline, &cmdline_size, pre_defined_symbols[posn]);
	}
	for(posn = 0; posn < num_undefined_symbols; ++posn)
	{
		AddArgument(&cmdline, &cmdline_size, "-U");
		AddArgument(&cmdline, &cmdline_size, undefined_symbols[posn]);
	}
	if(nostdinc_flag)
	{
		AddArgument(&cmdline, &cmdline_size, "-nostdinc");
	}
	if(nostdinc_cpp_flag)
	{
		AddArgument(&cmdline, &cmdline_size, "-nostdinc++");
	}
	for(posn = 0; posn < num_include_dirs; ++posn)
	{
		AddArgument(&cmdline, &cmdline_size, "-I");
		AddArgument(&cmdline, &cmdline_size, include_dirs[posn]);
	}
	for(posn = 0; posn < num_sys_include_dirs; ++posn)
	{
		AddArgument(&cmdline, &cmdline_size, "-J");
		AddArgument(&cmdline, &cmdline_size, sys_include_dirs[posn]);
	}
	for(posn = 0; posn < num_sys_cpp_include_dirs; ++posn)
	{
		AddArgument(&cmdline, &cmdline_size, "-K");
		AddArgument(&cmdline, &cmdline_size, sys_cpp_include_dirs[posn]);
	}
	if(nostdlib_flag)
	{
		AddArgument(&cmdline, &cmdline_size, "-nostdlib");
	}
	for(posn = 0; posn < num_link_dirs; ++posn)
	{
		AddArgument(&cmdline, &cmdline_size, "-L");
		AddArgument(&cmdline, &cmdline_size, link_dirs[posn]);
	}
	for(posn = 0; posn < num_sys_link_dirs; ++posn)
	{
		AddArgument(&cmdline, &cmdline_size, "-M");
		AddArgument(&cmdline, &cmdline_size, sys_link_dirs[posn]);
	}
	for(posn = 0; posn < num_libraries; ++posn)
	{
		AddArgument(&cmdline, &cmdline_size, "-l");
		AddArgument(&cmdline, &cmdline_size, libraries[posn]);
	}
	if(all_warnings)
	{
		AddArgument(&cmdline, &cmdline_size, "-Wall");
	}
	if(warnings_as_errors)
	{
		AddArgument(&cmdline, &cmdline_size, "-Werror");
	}
	for(posn = 0; posn < num_warning_flags; ++posn)
	{
		AddArgument(&cmdline, &cmdline_size, "-W");
		AddArgument(&cmdline, &cmdline_size, warning_flags[posn]);
	}
	if(inhibit_warnings)
	{
		AddArgument(&cmdline, &cmdline_size, "-w");
	}
	for(posn = 0; posn < num_extension_flags; ++posn)
	{
		AddArgument(&cmdline, &cmdline_size, "-f");
		AddArgument(&cmdline, &cmdline_size, extension_flags[posn]);
	}
	for(posn = 0; posn < num_machine_flags; ++posn)
	{
		AddArgument(&cmdline, &cmdline_size, "-m");
		AddArgument(&cmdline, &cmdline_size, machine_flags[posn]);
	}
	if(verbose_mode == VERBOSE_FILENAMES)
	{
		AddArgument(&cmdline, &cmdline_size, "-v");
	}

	/* Add the output filename to the command-line */
	AddArgument(&cmdline, &cmdline_size, "-o");
	if(assemble_flag)
	{
		if(output_filename)
		{
			/* Use the supplied assembly code output filename */
			AddArgument(&cmdline, &cmdline_size, output_filename);
			asm_output = output_filename;
		}
		else if(CSStringListContains(machine_flags, num_machine_flags, "jvm"))
		{
			/* Create a JL assembly output name based on the input */
			asm_output = ChangeExtension((char *)filename, "jl");
			AddArgument(&cmdline, &cmdline_size, asm_output);
		}
		else
		{
			/* Create an IL assembly output name based on the input */
			asm_output = ChangeExtension((char *)filename, "il");
			AddArgument(&cmdline, &cmdline_size, asm_output);
		}
	}
	else if(CSStringListContains(machine_flags, num_machine_flags, "jvm"))
	{
		asm_output = ChangeExtension((char *)filename, "jltmp");
		AddArgument(&cmdline, &cmdline_size, asm_output);
	}
	else
	{
		asm_output = ChangeExtension((char *)filename, "iltmp");
		AddArgument(&cmdline, &cmdline_size, asm_output);
	}

	/* Output the "--" separator, in case some filenames start with "-" */
	AddArgument(&cmdline, &cmdline_size, "--");

	/* Add the name of the input file to the command-line */
	AddArgument(&cmdline, &cmdline_size, (char *)filename);

	/* If this is a multiple-file plugin, then add the names of all
	   other source files for the same plugin to the command-line */
	if(isMultiple)
	{
		for(posn = filenum + 1; posn < num_input_files; ++posn)
		{
			if(file_proc_types[posn] == FILEPROC_TYPE_MULTIPLE &&
			   !strcmp(plugin, plugin_list[posn]))
			{
				/* Add the name to the command-line */
				AddArgument(&cmdline, &cmdline_size, input_files[posn]);

				/* Mark the file as done so we don't process it again */
				file_proc_types[posn] = FILEPROC_TYPE_DONE;
			}
		}
	}

	/* Terminate the command-line */
	AddArgument(&cmdline, &cmdline_size, 0);

	/* Execute the plugin */
	status = ExecChild(cmdline, filename);
	ILFree(cmdline);
	if(status != 0)
	{
		ILDeleteFile(asm_output);
		return status;
	}

	/* If we were assembling or syntax-checking the file, then stop now */
	if(assemble_flag)
	{
		return 0;
	}
	if(CSStringListContains(extension_flags, num_extension_flags,
							"syntax-check"))
	{
		ILDeleteFile(asm_output);
		return 0;
	}

	/* Build the assembler command-line */
	cmdline = 0;
	cmdline_size = 0;
	AddArgument(&cmdline, &cmdline_size, ilasm_program);
	if(executable_flag)
	{
		obj_output = ChangeExtension((char *)filename, "objtmp");
		CSAddLinkFile(obj_output, 1);
	}
	else if(compile_flag)
	{
		obj_output = output_filename;
	}
	else
	{
		obj_output = ChangeExtension((char *)filename, "obj");
	}
	AddArgument(&cmdline, &cmdline_size, "-o");
	AddArgument(&cmdline, &cmdline_size, obj_output);
	if(debug_flag)
	{
		AddArgument(&cmdline, &cmdline_size, "-g");
	}
	for(posn = 0; posn < num_extension_flags; ++posn)
	{
		AddArgument(&cmdline, &cmdline_size, "-f");
		AddArgument(&cmdline, &cmdline_size, extension_flags[posn]);
	}
	for(posn = 0; posn < num_machine_flags; ++posn)
	{
		AddArgument(&cmdline, &cmdline_size, "-m");
		AddArgument(&cmdline, &cmdline_size, machine_flags[posn]);
	}
	AddArgument(&cmdline, &cmdline_size, "--");
	AddArgument(&cmdline, &cmdline_size, asm_output);
	AddArgument(&cmdline, &cmdline_size, 0);

	/* Execute the assembler */
	status = ExecChild(cmdline, 0);
	ILFree(cmdline);
	if(status != 0)
	{
		ILDeleteFile(asm_output);
		ILDeleteFile(obj_output);
		return status;
	}
	ILDeleteFile(asm_output);

	/* Done */
	return 0;
}

/*
 * Link the final executable.
 */
static int LinkExecutable(void)
{
	char **cmdline;
	int cmdline_size;
	int posn, status;

	/* Find the linker executable */
	FindILALinkProgram();

	/* Build the linker command-line */
	cmdline = 0;
	cmdline_size = 0;
	AddArgument(&cmdline, &cmdline_size, ilalink_program);
	AddArgument(&cmdline, &cmdline_size, "-o");
	AddArgument(&cmdline, &cmdline_size, output_filename);
	if(shared_flag)
	{
		AddArgument(&cmdline, &cmdline_size, "--format");
		AddArgument(&cmdline, &cmdline_size, "dll");
	}
	else
	{
		AddArgument(&cmdline, &cmdline_size, "--format");
		AddArgument(&cmdline, &cmdline_size, "exe");
	}
	if(entry_point)
	{
		AddArgument(&cmdline, &cmdline_size, "--entry-point");
		AddArgument(&cmdline, &cmdline_size, entry_point);
	}
	if(nostdlib_flag)
	{
		AddArgument(&cmdline, &cmdline_size, "--no-stdlib");
	}
	for(posn = 0; posn < num_extension_flags; ++posn)
	{
		AddArgument(&cmdline, &cmdline_size, "-f");
		AddArgument(&cmdline, &cmdline_size, extension_flags[posn]);
	}
	for(posn = 0; posn < num_machine_flags; ++posn)
	{
		AddArgument(&cmdline, &cmdline_size, "-m");
		AddArgument(&cmdline, &cmdline_size, machine_flags[posn]);
	}
	for(posn = 0; posn < num_link_dirs; ++posn)
	{
		AddArgument(&cmdline, &cmdline_size, "-L");
		AddArgument(&cmdline, &cmdline_size, link_dirs[posn]);
	}
	for(posn = 0; posn < num_sys_link_dirs; ++posn)
	{
		AddArgument(&cmdline, &cmdline_size, "-L");
		AddArgument(&cmdline, &cmdline_size, sys_link_dirs[posn]);
	}
	AddArgument(&cmdline, &cmdline_size, "-L");
	AddArgument(&cmdline, &cmdline_size, ".");
	for(posn = 0; posn < num_libraries; ++posn)
	{
		AddArgument(&cmdline, &cmdline_size, "-l");
		AddArgument(&cmdline, &cmdline_size, libraries[posn]);
	}
	AddArgument(&cmdline, &cmdline_size, "--");
	for(posn = 0; posn < num_files_to_link; ++posn)
	{
		AddArgument(&cmdline, &cmdline_size, files_to_link[posn]);
	}
	AddArgument(&cmdline, &cmdline_size, 0);

	/* Execute the linker */
	status = ExecChild(cmdline, 0);
	ILFree(cmdline);
	if(status != 0)
	{
		ILDeleteFile(output_filename);
		return status;
	}

	/* Done */
	return 0;
}

#ifdef	__cplusplus
};
#endif
