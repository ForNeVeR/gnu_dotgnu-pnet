/*
 * cs_errors.c - Error reporting functions for the C# compiler.
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
#ifdef HAVE_STDARG_H
#include <stdarg.h>
#define	VA_LIST				va_list
#define	VA_START			va_list va; va_start(va, format)
#define	VA_END				va_end(va)
#define	VA_GET_LIST			va
#else
#ifdef HAVE_VARARGS_H
#include <varargs.h>
#define	VA_LIST				va_list
#define	VA_START			va_list va; va_start(va)
#define	VA_END				va_end(va)
#define	VA_GET_LIST			va
#else
#define	VA_LIST				int
#define	VA_START
#define	VA_END
#define	VA_GET_LIST			0
#endif
#endif

#ifdef	__cplusplus
extern	"C" {
#endif

int CSHaveErrors = 0;
int CSHaveWarnings = 0;
int CSUnsafeLevel = 0;

/*
 * Print an error or warning message to stderr.
 */
static void PrintMessage(char *filename, unsigned long linenum, int warning,
						 const char *format, VA_LIST va)
{
	/* Print the filename and line number information */
	if(filename)
	{
		fputs(filename, stderr);
		putc(':', stderr);
	}
	fprintf(stderr, "%lu: ", linenum);

	/* Print the "warning: " prefix if this is a warning */
	if(warning)
	{
		fputs("warning: ", stderr);
	}

	/* Print the message */
#ifdef HAVE_VFPRINTF
	vfprintf(stderr, format, va);
#else
	/* Shouldn't happen, but at least print the format */
	fputs(format, stderr);
#endif

	/* Terminate the line */
	putc('\n', stderr);
}

void CSError(const char *format, ...)
{
	VA_START;
	PrintMessage(CSCurrentFilename(), CSCurrentLine(), 0, format, VA_GET_LIST);
	VA_END;
	CSHaveErrors = 1;
}

void CSErrorOnLine(char *filename, unsigned long linenum,
				   const char *format, ...)
{
	VA_START;
	PrintMessage(filename, linenum, 0, format, VA_GET_LIST);
	VA_END;
	CSHaveErrors = 1;
}

void CSWarning(const char *format, ...)
{
	if(!inhibit_warnings)
	{
		VA_START;
		PrintMessage(CSCurrentFilename(), CSCurrentLine(), 1,
					 format, VA_GET_LIST);
		VA_END;
		CSHaveWarnings = 1;
		if(warnings_as_errors)
		{
			CSHaveErrors = 1;
		}
	}
}

/*
 * Determine if a warning has been enabled.
 */
static int WarningEnabled(const char *type)
{
	if(inhibit_warnings)
	{
		return 0;
	}
	if(*type != '-')
	{
		/* Warning is off by default unless explicitly enabled */
		if(all_warnings)
		{
			if(CSStringListContainsInv(warning_flags, num_warning_flags, type))
			{
				return 0;
			}
			return 1;
		}
		else
		{
			return CSStringListContains(warning_flags, num_warning_flags, type);
		}
	}
	else
	{
		/* Warning is on by default unless explicitly disabled */
		if(all_warnings)
		{
			if(CSStringListContainsInv(warning_flags, num_warning_flags,
									   type + 1))
			{
				return 0;
			}
			return 1;
		}
		else
		{
			return !CSStringListContainsInv(warning_flags, num_warning_flags,
									        type + 1);
		}
	}
}

void CSTypedWarning(const char *type, const char *format, ...)
{
	if(WarningEnabled(type))
	{
		VA_START;
		PrintMessage(CSCurrentFilename(), CSCurrentLine(), 1,
					 format, VA_GET_LIST);
		VA_END;
		CSHaveWarnings = 1;
		if(warnings_as_errors)
		{
			CSHaveErrors = 1;
		}
	}
}

void CSWarningOnLine(char *filename, unsigned long linenum,
				     const char *format, ...)
{
	if(!inhibit_warnings)
	{
		VA_START;
		PrintMessage(filename, linenum, 1, format, VA_GET_LIST);
		VA_END;
		CSHaveWarnings = 1;
		if(warnings_as_errors)
		{
			CSHaveErrors = 1;
		}
	}
}

void CSTypedWarningOnLine(char *filename, unsigned long linenum,
				     	  const char *type, const char *format, ...)
{
	if(WarningEnabled(type))
	{
		VA_START;
		PrintMessage(filename, linenum, 1, format, VA_GET_LIST);
		VA_END;
		CSHaveWarnings = 1;
		if(warnings_as_errors)
		{
			CSHaveErrors = 1;
		}
	}
}

void CSUnsafeMessage(const char *format, ...)
{
	char newformat[BUFSIZ];
	if(CSStringListContains(extension_flags, num_extension_flags, "unsafe"))
	{
		if(CSUnsafeLevel == 0)
		{
			/* Unsafe construct used outside an unsafe context */
			VA_START;
			strcpy(newformat, format);
			strcat(newformat, " used outside an unsafe context");
			PrintMessage(CSCurrentFilename(), CSCurrentLine(), 0,
						 newformat, VA_GET_LIST);
			VA_END;
			CSHaveErrors = 1;
		}
		else
		{
			/* Unsafe construct used inside an unsafe construct */
			if(WarningEnabled("unsafe"))
			{
				VA_START;
				PrintMessage(CSCurrentFilename(), CSCurrentLine(), 1,
							 format, VA_GET_LIST);
				VA_END;
				CSHaveWarnings = 1;
				if(warnings_as_errors)
				{
					CSHaveErrors = 1;
				}
			}
		}
	}
	else
	{
		/* Unsafe constructs are not permitted */
		VA_START;
		strcpy(newformat, format);
		strcat(newformat, " not permitted unless -funsafe specified");
		PrintMessage(CSCurrentFilename(), CSCurrentLine(), 0,
					 newformat, VA_GET_LIST);
		VA_END;
		CSHaveErrors = 1;
	}
}

void CSUnsafeTypeMessage()
{
	CSUnsafeMessage("unsafe pointer type");
}

void CSUnsafeEnter(const char *format, ...)
{
	char newformat[BUFSIZ];
	if(CSStringListContains(extension_flags, num_extension_flags, "unsafe"))
	{
		/* Unsafe constructs are permitted, so just print a warning */
		if(WarningEnabled("unsafe"))
		{
			VA_START;
			PrintMessage(CSCurrentFilename(), CSCurrentLine(), 1,
						 format, VA_GET_LIST);
			VA_END;
			CSHaveWarnings = 1;
			if(warnings_as_errors)
			{
				CSHaveErrors = 1;
			}
		}
	}
	else
	{
		/* Unsafe constructs are not permitted */
		VA_START;
		strcpy(newformat, format);
		strcat(newformat, " not permitted unless -funsafe specified");
		PrintMessage(CSCurrentFilename(), CSCurrentLine(), 0,
					 newformat, VA_GET_LIST);
		VA_END;
		CSHaveErrors = 1;
	}
	++CSUnsafeLevel;
}

void CSUnsafeLeave(void)
{
	--CSUnsafeLevel;
}

#ifdef	__cplusplus
};
#endif
