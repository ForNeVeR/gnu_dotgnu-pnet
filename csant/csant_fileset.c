/*
 * csant_fileset.c - File set management routines.
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

#include "csant_defs.h"
#include "csant_fileset.h"
#include "il_regex.h"
#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif
#ifdef HAVE_SYS_STAT_H
#include <sys/stat.h>
#endif

#ifdef	__cplusplus
extern	"C" {
#endif

/*
 * Internal definition of the file set structure.
 */
struct _tagCSAntFileSet
{
	unsigned long	numFiles;
	unsigned long	maxFiles;
	char		  **files;

};

/*
 * Stack item for the stack of directories yet to be visited.
 */
typedef struct _tagCSAntDirStack CSAntDirStack;
struct _tagCSAntDirStack
{
	char          *dirName;
	CSAntDirStack *next;

};

/*
 * Add a character to the "regex" string.
 */
static void RegexChar(char **regex, int *regexLen, int *regexMax, char ch)
{
	char *newRegex = (char *)ILRealloc(*regex, *regexMax + 32);
	if(!newRegex)
	{
		CSAntOutOfMemory();
	}
	*regexMax += 32;
	*regex = newRegex;
	newRegex[(*regexLen)++] = ch;
}
#define	REGEX_CHAR(ch)	\
			do { \
				if(regexLen < regexMax) \
				{ \
					regex[regexLen++] = (char)(ch); \
				} \
				else \
				{ \
					RegexChar(&regex, &regexLen, &regexMax, (char)(ch)); \
				} \
			} while (0)

static int CheckIf(CSAntTask *node)
{
	const char *val=CSAntTaskParam(node,"if");
	if(!val)
	{
		return 1;
	}
	if(val && !ILStrICmp(val,"true"))
	{
		return 1;
	}
	return 0;
}

/*
 * Build a regular expression from a set of include or exclude names.
 * Returns non-zero if recursion into sub-directories is specified.
 */
typedef regex_t RegexState;
static int BuildIncludeRegex(CSAntTask *node, const char *name,
							 RegexState *state)
{
	CSAntTask *subNode;
	const char *arg;
	char *regex = 0;
	int regexLen = 0;
	int regexMax = 0;
	int haveCase = 0;
	int recursive = 0;
	char ch;

	/* Build the regular expression */
	subNode = node->taskChildren;
	while(subNode != 0)
	{
		if(!CheckIf(subNode))
		{
			subNode=subNode->next;
			continue;
		}
		if(!strcmp(subNode->name, name) &&
		   (arg = CSAntTaskParam(subNode, "name")) != 0)
		{
			if(haveCase)
			{
				REGEX_CHAR('|');
			}
			else
			{
				haveCase = 1;
			}
			REGEX_CHAR('^');
			while(*arg != 0)
			{
				ch = *arg++;
				if(ch == '*' && *arg == '*')
				{
					/* Recursive regular expression definition */
					recursive = 1;
					if(arg[1] == '/' || arg[1] == '\\')
					{
						REGEX_CHAR('(');
						REGEX_CHAR('.');
						REGEX_CHAR('*');
					#ifdef IL_WIN32_NATIVE
						REGEX_CHAR('\\');
						REGEX_CHAR('\\');
					#else
						REGEX_CHAR('/');
					#endif
						REGEX_CHAR('|');
						REGEX_CHAR('(');
						REGEX_CHAR(')');
						REGEX_CHAR(')');
						arg += 2;
					}
					else
					{
						REGEX_CHAR('.');
						REGEX_CHAR('*');
						++arg;
					}
				}
				else if(ch == '*')
				{
					/* Match anything that doesn't involve a separator */
					REGEX_CHAR('[');
					REGEX_CHAR('^');
				#ifdef IL_WIN32_NATIVE
					REGEX_CHAR('\\');
					REGEX_CHAR('\\');
				#else
					REGEX_CHAR('/');
				#endif
					REGEX_CHAR(']');
					REGEX_CHAR('*');
				}
				else if(ch == '?')
				{
					/* Match a single character */
					REGEX_CHAR('.');
				}
				else if(ch == '/' || ch == '\\')
				{
					/* Match a directory separator */
					recursive = 1;
				#ifdef IL_WIN32_NATIVE
					REGEX_CHAR('\\');
					REGEX_CHAR('\\');
				#else
					REGEX_CHAR('/');
				#endif
				}
				else if(ch == '[')
				{
					/* Match a set of characters */
					REGEX_CHAR('[');
					while(*arg != 0 && *arg != ']')
					{
						REGEX_CHAR(*arg);
						++arg;
					}
					REGEX_CHAR(']');
					if(*arg != '\0')
					{
						++arg;
					}
				}
				else if(ch == '.' || ch == '^' || ch == '$' ||
						ch == ']' || ch == '\\' || ch == '(' || ch == ')')
				{
					/* Match a special character */
					REGEX_CHAR('\\');
					REGEX_CHAR(ch);
				}
				else
				{
					/* Match an ordinary character */
				#ifdef _WIN32
					if(ch >= 'A' && ch <= 'Z')
					{
						REGEX_CHAR('[');
						REGEX_CHAR(ch);
						REGEX_CHAR(ch - 'A' + 'a');
						REGEX_CHAR(']');
					}
					else if(ch >= 'a' && ch <= 'z')
					{
						REGEX_CHAR('[');
						REGEX_CHAR(ch - 'a' + 'A');
						REGEX_CHAR(ch);
						REGEX_CHAR(']');
					}
					else
					{
						REGEX_CHAR(ch);
					}
				#else
					REGEX_CHAR(ch);
				#endif
				}
			}
			REGEX_CHAR('$');
		}
		subNode = subNode->next;
	}

	/* If there are no cases, then construct a regex that is
	   guaranteed never to match any filenames */
	if(!haveCase)
	{
		REGEX_CHAR('^');
		REGEX_CHAR('(');
		REGEX_CHAR(')');
		REGEX_CHAR('$');
	}

	/* Terminate the regular expression */
	REGEX_CHAR('\0');

	/* Compile the register expression and exit */
	if(IL_regcomp(state, regex, REG_EXTENDED | REG_NOSUB) != 0)
	{
		fprintf(stderr, "Invalid regular expression: %s\n", regex);
		exit(1);
	}
	ILFree(regex);
	return recursive;
}

/*
 * Match a pathname against an include regular expression.
 */
static int MatchInclude(char *pathname, RegexState *state)
{
	return (IL_regexec(state, pathname, 0, 0, 0) == 0);
}

/*
 * Add a pathname to a file set.
 */
static void AddToFileSet(CSAntFileSet *fileset, char *pathname)
{
	if(fileset->numFiles >= fileset->maxFiles)
	{
		fileset->files = (char **)ILRealloc(fileset->files,
											sizeof(char *) *
												(fileset->numFiles + 32));
		if(!(fileset->files))
		{
			CSAntOutOfMemory();
		}
		fileset->maxFiles = fileset->numFiles + 32;
	}
	fileset->files[(fileset->numFiles)++] = pathname;
}

/*
 * Determine if a pathname corresponds to a directory.
 */
static int IsDirectory(const char *pathname)
{
#ifdef HAVE_STAT
	struct stat st;
	if(stat(pathname, &st) >= 0 && S_ISDIR(st.st_mode))
	{
		return 1;
	}
	else
	{
		return 0;
	}
#else
	return 0;
#endif
}

CSAntFileSet *CSAntFileSetLoad(CSAntTask *task, const char *name,
							   const char *configBaseDir)
{
	CSAntFileSet *fileset;
	CSAntTask *node;
	const char *arg;
	const char *arg2;
	char *baseDir;
	char *currentDir;
	char *pathname;
	char *tempName;
	CSAntDirStack *stack;
	CSAntDirStack *nextStack;
	CSAntDir *dir;
	const char *filename;
	int recursive;
	RegexState state;
	unsigned long posn;
	unsigned long outposn;

	/* Find the sub-node */
	node = task->taskChildren;
	while(node != 0 && strcmp(node->name, name) != 0)
	{
		node = node->next;
	}
	if(!node)
	{
		return 0;
	}

	/* Construct the starting base directory and add it to the stack */
	arg = CSAntTaskParam(node, "basedir");
	if(arg)
	{
		baseDir = CSAntDirCombine(configBaseDir, arg);
	}
	else
	{
		baseDir = CSAntDirCombine(configBaseDir, 0);
	}
	stack = (CSAntDirStack *)ILMalloc(sizeof(CSAntDirStack));
	if(!stack)
	{
		CSAntOutOfMemory();
	}
	stack->dirName = baseDir;
	stack->next = 0;

	/* Construct the file set */
	fileset = (CSAntFileSet *)ILMalloc(sizeof(CSAntFileSet));
	if(!fileset)
	{
		CSAntOutOfMemory();
	}
	fileset->numFiles = 0;
	fileset->maxFiles = 0;
	fileset->files = 0;

	/* Compile the "includes" regular expression */
	recursive = BuildIncludeRegex(node, "includes", &state);

	/* Pop items from the directory stack and scan them until
	   all matches have been processed */
	while(stack != 0)
	{
		/* Pop the top-most stack item */
		currentDir = stack->dirName;
		nextStack = stack->next;
		ILFree(stack);
		stack = nextStack;

		/* Scan the directory for inclusions and exclusions */
		dir = CSAntDirOpen(currentDir, (const char *)0);
		if(dir)
		{
			while((filename = CSAntDirNext(dir)) != 0)
			{
				/* Always exclude CVS-related files and directories */
				if(!strcmp(filename, "CVS") || !strcmp(filename, ".cvsignore"))
				{
					continue;
				}

				/* Build the full pathname */
				pathname = CSAntDirCombine(currentDir, filename);

				/* Different processing is required for directories and files */
				if(recursive && IsDirectory(pathname))
				{
					/* Add to the fileset if we have a match */
					if(MatchInclude(pathname + strlen(baseDir) + 1, &state))
					{
						tempName = ILDupString(pathname);
						if(!tempName)
						{
							CSAntOutOfMemory();
						}
						AddToFileSet(fileset, tempName);
					}

					/* Add the directory to the stack for later processing */
					nextStack = (CSAntDirStack *)ILMalloc
									(sizeof(CSAntDirStack));
					if(!nextStack)
					{
						CSAntOutOfMemory();
					}
					nextStack->dirName = pathname;
					nextStack->next = stack;
					stack = nextStack;
				}
				else
				{
					/* Add the pathname to the file set if it matches */
					if(MatchInclude(pathname + strlen(baseDir) + 1, &state))
					{
						AddToFileSet(fileset, pathname);
					}
					else
					{
						ILFree(pathname);
					}
				}
			}
			CSAntDirClose(dir);
		}

		/* Free the current directory name, which we no longer require */
		if(currentDir != baseDir)
		{
			ILFree(currentDir);
		}
	}

	/* Free the regular expression state */
	IL_regfree(&state);

	/* Construct a new regular expression for the "excludes" list */
	BuildIncludeRegex(node, "excludes", &state);

	/* Remove pathnames from the fileset that match the regex */
	outposn = 0;
	for(posn = 0; posn < fileset->numFiles; ++posn)
	{
		if(!MatchInclude(fileset->files[posn] + strlen(baseDir) + 1, &state))
		{
			fileset->files[outposn++] = fileset->files[posn];
		}
		else
		{
			ILFree(fileset->files[posn]);
		}
	}
	fileset->numFiles = outposn;

	/* Add simple files that are named using the <file> tag */
	node = node->taskChildren;
	while(node != 0)
	{
		if(!strcmp(node->name, "file"))
		{
			arg = CSAntTaskParam(node, "name");
			if(arg)
			{
				arg2 = CSAntTaskParam(node, "basedir");
				if(arg2)
				{
					pathname = CSAntDirCombine(arg2, arg);
				}
				else
				{
					pathname = CSAntDirCombine(baseDir, arg);
				}
				AddToFileSet(fileset, pathname);
			}
		}
		node = node->next;
	}

	/* We don't need the base directory any more */
	ILFree(baseDir);

	/* Free the regular expression state */
	IL_regfree(&state);

	/* Done */
	return fileset;
}

void CSAntFileSetDestroy(CSAntFileSet *fileset)
{
	unsigned long posn;
	if(!fileset)
	{
		return;
	}
	for(posn = 0; posn < fileset->numFiles; ++posn)
	{
		ILFree(fileset->files[posn]);
	}
	if(fileset->files)
	{
		ILFree(fileset->files);
	}
	ILFree(fileset);
}

unsigned long CSAntFileSetSize(CSAntFileSet *fileset)
{
	if(fileset)
	{
		return fileset->numFiles;
	}
	else
	{
		return 0;
	}
}

char *CSAntFileSetFile(CSAntFileSet *fileset, unsigned long num)
{
	if(fileset && num < fileset->numFiles)
	{
		return fileset->files[num];
	}
	else
	{
		return 0;
	}
}

int CSAntFileSetNewer(CSAntFileSet *fileset, const char *filename)
{
#ifdef HAVE_STAT
	struct stat st1;
	struct stat st2;
	unsigned long posn;

	/* Assume that the file is not newer if the fileset is non-existent */
	if(!fileset)
	{
		return 0;
	}

	/* If the file doesn't exist at all, then assume
	   that the fileset is newer */
	if(stat(filename, &st1) < 0)
	{
		return 1;
	}

	/* Process each of the files in the fileset in turn */
	for(posn = 0; posn < fileset->numFiles; ++posn)
	{
		if(stat(fileset->files[posn], &st2) >= 0 &&
		   st2.st_mtime > st1.st_mtime)
		{
			return 1;
		}
	}

	/* Not newer */
	return 0;
#else
	/* We don't know how to determine newness, so it is always new */
	return 1;
#endif
}

#ifdef	__cplusplus
};
#endif
