/*
 * doc_texi.c - Convert csdoc into texinfo.
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
#include "doc_tree.h"
#include "doc_backend.h"

#ifdef	__cplusplus
extern	"C" {
#endif

char const ILDocProgramHeader[] =
	"CSDOC2TEXI " VERSION " - Convert C# documentation into texinfo";

char const ILDocProgramName[] = "CSDOC2TEXI";

ILCmdLineOption const ILDocProgramOptions[] = {
	{0, 0, 0, 0, 0}
};

char *ILDocDefaultOutput(int numInputs, char **inputs, const char *progname)
{
	if(!strcmp(inputs[0], "-"))
	{
		/* The first input is stdin, so the output is too */
		return "-";
	}
	else
	{
		/* Adjust the first input filename to end in ".texi" */
		int len = strlen(inputs[0]);
		char *result;
		while(len > 0 && inputs[0][len - 1] != '/' &&
			  inputs[0][len - 1] != '\\' &&
			  inputs[0][len - 1] != '.')
		{
			--len;
		}
		if(len > 0 && inputs[0][len - 1] == '.')
		{
			result = (char *)ILMalloc(len + 5);
			if(!result)
			{
				ILDocOutOfMemory(progname);
			}
			ILMemCpy(result, inputs[0], len);
			strcpy(result + len, "texi");
		}
		else
		{
			result = (char *)ILMalloc(len + 6);
			if(!result)
			{
				ILDocOutOfMemory(progname);
			}
			strcpy(result, inputs[0]);
			strcat(result, ".texi");
		}
		return result;
	}
}

int ILDocValidateOutput(char *outputPath, const char *progname)
{
	/* Nothing to do here: any pathname is considered valid */
	return 1;
}

int ILDocConvert(ILDocTree *tree, char *outputPath, const char *progname)
{
	return 1;
}

#ifdef	__cplusplus
};
#endif
