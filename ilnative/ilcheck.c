/*
 * ilcheck.c Check runtime calls against the mscorlib.dll
 *
 * Copyright (C) 2001  Southern Storm Software, Pty Ltd.
 *
 * Hacked by Gopal.V from ilnative.c
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
#include "il_dumpasm.h"
#include "il_utils.h"
#include "il_engine.h"
#include "../engine/engine.h"

typedef struct _ILDiffDict
{
	char *name;
	void *info;
	struct _ILDiffDict *methods;
	struct _ILDiffDict *prev;
}ILDict;

ILDict * ILDict_new(ILDict *prev,char *name)
{
	ILDict *retval=(ILDict*)calloc(1,sizeof(ILDict));
	if(name!=NULL)retval->name=name;
	retval->prev=prev;
	return retval;
}

ILDict * ILDict_search(ILDict *end,char *name)
{
	ILDict *i;
	for(i=end;i!=NULL;i=i->prev)
	{
		if(!strcmp(i->name,name))return i;
	}
	return NULL;
}

ILDict *klasses;
#ifdef	__cplusplus
extern	"C" {
#endif

/*
 * Table of command-line options.
 */
static ILCmdLineOption const options[] = {
	{"-v", 'v', 0, 0, 0},
	{"-a",'a',0,"-a","output all internal calls"},
	{"--all",'a',0,0,0},
	{"-fxml",'f',1,"-fxml","output XML instead of human-readable text"},
	{"--version", 'v', 0,
		"--version    or -v",
		"Print the version of the program."},
	{"--help", 'h', 0,
		"--help",
		"Print this help message."},
	{0, 0, 0, 0, 0}
};

static void usage(const char *progname);
static void version(void);
static int printNatives(const char *filename, ILContext *context,
						int xml,int all);

int main(int argc, char *argv[])
{
	char *progname = argv[0];
	int sawStdin;
	int state, opt;
	char *param;
	int errors,xml=0,all=0;
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
			case 'f':
			{
				xml=1;
				break;
			}
			case 'a':
			{
				all=1;
				break;
			}
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
	while(argc > 1)
	{
		if(!strcmp(argv[1], "-"))
		{
			/* Dump the contents of stdin, but only once */
			if(!sawStdin)
			{
				errors |= printNatives("-", context, xml,all);
				sawStdin = 1;
			}
		}
		else
		{
			/* Dump the contents of a regular file */
			errors |= printNatives(argv[1], context, xml,all);
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
	fprintf(stdout, "ILCHECK " VERSION " - IL Internal Call Check \n");
	fprintf(stdout, "Copyright (c) 2001 Southern Storm Software, Pty Ltd.\n");
	fprintf(stdout, "          (c) 2002 FSF India.\n");
	fprintf(stdout, "\n");
	fprintf(stdout, "Usage: %s [options] input ...\n", progname);
	fprintf(stdout, "\n");
	ILCmdLineHelp(options);
}

static void version(void)
{

	printf("ILCHECK " VERSION " - IL Internal Call Check \n");
	printf("Copyright (c) 2001 Southern Storm Software, Pty Ltd.\n");
	printf("          (c) 2002 FSF India.\n");
	printf("\n");
	printf("ILCHECK comes with ABSOLUTELY NO WARRANTY.  This is free software,\n");
	printf("and you are welcome to redistribute it under the terms of the\n");
	printf("GNU General Public License.  See the file COPYING for further details.\n");
	printf("\n");
	printf("Use the `--help' option to get help on the command-line options.\n");
}

/*
 * Dump information about a native method.
 */
static void dumpMethodInfoTxt(ILImage *image, ILMethod *method,char *stat)
{
	/* Dump the method attributes */
	ILDumpFlags(stdout, ILMethod_Attrs(method),
				ILMethodDefinitionFlags, 0);

	/* Dump the method signature */
	ILDumpMethodType(stdout, image, ILMethod_Signature(method), 0,
					 ILMethod_Owner(method), ILMethod_Name(method),
					 method);
	putc(' ', stdout);

	/* Dump the implementation flags */
	ILDumpFlags(stdout, ILMethod_ImplAttrs(method),
				ILMethodImplementationFlags, 0);

	/* Terminate the line */
	putc('\n', stdout);
}

static void dumpMethodInfoXml(ILImage *image, ILMethod *method,char *stat)
{
	/* Dump the method attributes */
	printf("\t <internalcall name=\"%s\"",ILMethod_Name(method));
	printf("class = \"%s.%s\" ",ILClass_Namespace(ILMethod_Owner(method)),ILClass_Name(ILMethod_Owner(method)));
	printf(" signature=\"");
	/* Dump the method signature */
	ILDumpMethodType(stdout, image, ILMethod_Signature(method), 0,
					 ILMethod_Owner(method), ILMethod_Name(method),
					 method);
	/* Terminate the line */
	printf("\" status=\"%s\"/>\n",stat);
}
static void addmethodinfo(ILImage *image, ILMethod *method,char *stat)
{
	/* Dump the method attributes */
	char *name=(char*)ILClass_Name(ILMethod_Owner(method));
	ILDict *this_class;	
	if(!ILDict_search(klasses,name))
	{	
		klasses=ILDict_new(klasses,name);
	}
	this_class=ILDict_search(klasses,name);
	this_class->methods=ILDict_new(this_class->methods,stat);
	this_class->methods->info=method;
}
/*
 * Load an IL image an display the native methods.
 */
static int printNatives(const char *filename, ILContext *context,
						int xml,int all)
{
	ILImage *image;
	unsigned long numMethods;
	unsigned long token;
	ILMethod *method;
	ILInternalInfo info;
	
	ILDict *klass,*mthd;
	
	klasses=ILDict_new(NULL,"<start>");
	/* Attempt to load the image into memory */
	if(ILImageLoadFromFile(filename, context, &image,
					  	   IL_LOADFLAG_FORCE_32BIT |
					  	   IL_LOADFLAG_NO_RESOLVE, 1) != 0)
	{
		return 1;
	}

	/* Walk the MethodDef table and print all internalcalls that are
	   missing from the internalcall table */
	numMethods = ILImageNumTokens(image, IL_META_TOKEN_METHOD_DEF);
	for(token = 1; token <= numMethods; ++token)
	{
		method = (ILMethod *)ILImageTokenInfo
									(image, IL_META_TOKEN_METHOD_DEF | token);
		if(method)
		{
			if(ILMethod_IsInternalCall(method) ||
			        ILMethod_IsNative(method))
			{								
				if(!_ILFindInternalCall(method,0,&info))
				{
					addmethodinfo(image,method,"MISSING");
				}
				else if(all)
				{
					addmethodinfo(image,method,"OK");
				}
			}
		}
	}
	if(xml)printf("<InternalCallStatus>\n");
	for(klass=klasses;klass->prev!=NULL;klass=klass->prev)
	{
		if(xml)
		{
			printf("<class namespace=\"%s\" name=\"%s\">\n",ILClass_Namespace(ILMethod_Owner(klass->methods->info)),klass->name);
		}
		for(mthd=klass->methods;mthd!=NULL;mthd=mthd->prev)
		{
			if(xml)dumpMethodInfoXml(image,mthd->info,mthd->name);
			else dumpMethodInfoTxt(image,mthd->info,mthd->name);
		}
		if(xml)
		{
			printf("</class>\n");
		}
	}
	if(xml)printf("</InternalCallStatus>\n");
	/* Clean up and exit */
	ILImageDestroy(image);
	return 0;
}

#ifdef	__cplusplus
};
#endif
