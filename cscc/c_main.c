/*
 * c_main.c - Main entry point for the C compiler plug-in.
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

#include <cscc/c/c_internal.h>

#ifdef	__cplusplus
extern	"C" {
#endif

/*
 * Entry points for the parser and lexer.
 */
extern int c_debug;
extern int c_parse(void);
extern void c_restart(FILE *infile);

/*
 * Configuration variables that are used by "cc_main.c".
 */
char const CCPluginName[] = "cscc-c-s";
int const CCPluginOptionParseMode = CMDLINE_PARSE_PLUGIN;
int const CCPluginUsesPreproc = 0;
int const CCPluginJVMSupported = 0;
int const CCPluginSkipCodeGen = 0;

int CCPluginInit(void)
{
	/* Read the state of the "-m32bit-only" flag, which affects the
	   layout and definition of various types */
	if(CCStringListContains(machine_flags, num_machine_flags,
							"32bit-only"))
	{
		gen_32bit_only = 1;
	}
	return 1;
}

void CCPluginShutdown(int status)
{
	/* Nothing to do here */
}

int CCPluginParse(void)
{
	/*c_debug = 1;*/
	return c_parse();
}

void CCPluginRestart(FILE *infile)
{
	c_restart(infile);
}

void CCPluginSemAnalysis(void)
{
	/* Nothing to do here: we do analysis on the fly during parsing */
}

void CCPluginPostCodeGen(void)
{
	/* Output the string constant pool */
	CGenStringPool(&CCCodeGen);
}

int main(int argc, char *argv[])
{
	return CCMain(argc, argv);
}

#ifdef	__cplusplus
};
#endif
