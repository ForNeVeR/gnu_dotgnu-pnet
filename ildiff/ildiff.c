/*
 * ildiff.c - Print differences between IL binaries.
 *
 * Copyright (C) 2001  FreeDevelopers.net
 * 
 * Author : Gopal.V
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
#include "il_types.h"
#include "il_program.h"
#include "il_utils.h"
#include "il_dumpasm.h"
#include "pretty.h"

#ifdef IF_PRETTY_PRINT
#undef IF_PRETTY_PRINT
#define IF_PRETTY_PRINT(x) if(pretty_print)x
#endif

//the above macro should be defined to use the 
//pretty print function
//this overrides the macro in the header file
//if(pretty_print) where pretty_print is *MY*
//variable name (see below )
//you could as well use if(myvar==12) instead
//if you like (if there is a myvar)

/*
 * All defines have been grouped to avoid all sorts of conflicts
 * they are used ony to make my code readable.
 */
#define IL_DIFF_OK 0
#define IL_DIFF_ERROR 1
#define IL_DIFF_SAME 0
#define IL_DIFF_DIFFERENT 1
#define IL_DIFF_FOUND 1
#define IL_DIFF_NOT_FOUND 2
/* so that I can look for both 
 * FOUND and NOT_FOUND in the search result
 */
#define IL_DIFF_IN '>'
#define IL_DIFF_OUT '<'
#define IL_DIFF_NONE ' '

int pretty_print = 0;		//global param
int same = 0;
typedef struct _ILDiffDict
{
	void *info;
	ILToken type;
	int found;	
	struct _ILDiffDict *prev;
}ILDiffDict;

/************************************
 *  code from my old compare.c file *
 ************************************/

/*
 * Typed compare operations
 */
static int ILDiff_compare_il_module(ILDiffDict *d1,ILDiffDict *d2)
{
		ILModule *m1,*m2;
		m1=(ILModule*)(d1->info);
		m2=(ILModule*)(d2->info);
		
		//since d1->info is generated using ILTokenInfo method
		//I want to know whether the name is sufficent for module ident
		//is MVID usable for this
		if(!strcmp(ILModule_Name(m1),ILModule_Name(m2)))return IL_DIFF_SAME;
		return IL_DIFF_DIFFERENT;
}
/*
 * in this methods we compare classes , I think Rhys should add a 
 * comparison method in the API. This checks only for names,namespace
 * and attrs ( No Parent check)
 */
static int ILDiff_compare_il_classes(ILDiffDict *d1,ILDiffDict *d2)
{
	ILClass *c1=(ILClass*)(d1->info);
	ILClass *c2=(ILClass*)(d2->info);
	if(!strcmp(ILClass_Name(c1),ILClass_Name(c2)))
		if((ILClass_Namespace(c1) == ILClass_Namespace(c2)) ||\
						!strcmp(ILClass_Namespace(c1),ILClass_Namespace(c2)))
			if(ILClass_Attrs(c1) == ILClass_Attrs(c2))
			return IL_DIFF_SAME;
	return IL_DIFF_DIFFERENT;
}

int ILDiff_compare_il_fields(ILDiffDict *d1,ILDiffDict *d2)
{
		ILField *f1=(ILField*)(d1->info);
		ILField *f2=(ILField*)(d2->info);
		//assuming name to be same;will correct later
		//changelog: added attrs ,owner and type check
		//but still namespace of owner is ignored
		if(!strcmp(ILField_Name(f1),ILField_Name(f2)))
			if(ILField_Attrs(f1)==ILField_Attrs(f2))
				if(!strcmp(ILClass_Name(ILField_Owner(f1)),\
									ILClass_Name(ILField_Owner(f2))))			
					if(!strcmp(ILTypeToName(ILField_Type(f1)),ILTypeToName(ILField_Type(f2))))
						return IL_DIFF_SAME;
		return IL_DIFF_DIFFERENT;
}

int ILDiff_compare_il_methods(ILDiffDict *d1,ILDiffDict *d2)
{
		ILMethod *f1=(ILMethod*)(d1->info);
		ILMethod *f2=(ILMethod*)(d2->info);
		//assuming name to be same	--need a better comparator
		//changelog : got a better comparator but is it too long ?
		//params are still not checked , add later 
		if(!strcmp(ILMethod_Name(f1),ILMethod_Name(f2)))
			if(ILMethod_Attrs(f1)==ILMethod_Attrs(f2))
				if(ILMethod_ImplAttrs(f1) == ILMethod_ImplAttrs(f2))
					if(ILMethod_CallConv(f1) == ILMethod_CallConv(f2))
						if(!strcmp(ILClass_Name(ILMethod_Owner(f1)),\
										ILClass_Name(ILMethod_Owner(f1))))
								return IL_DIFF_SAME;
		return IL_DIFF_DIFFERENT;
}
int ILDiff_compare(ILDiffDict *d1,ILDiffDict *d2)
{
		if(d1->type==d2->type)
		{
			if(IL_META_TOKEN_MODULE == d1->type)//correct me if I am wrong
			{
				return ILDiff_compare_il_module(d1,d2);
			}
			if(IL_META_TOKEN_FIELD_DEF == d1->type)
			{
				return ILDiff_compare_il_fields(d1,d2);
			}
			if(IL_META_TOKEN_METHOD_DEF == d1->type)
			{
				return ILDiff_compare_il_methods(d1,d2);
			}
			if(IL_META_TOKEN_TYPE_DEF == d1->type)
			{
				return ILDiff_compare_il_classes(d1,d2);		
			}
		}
		return IL_DIFF_DIFFERENT;
}
/****************************
 * code from my dict.c file *
 ****************************/

/*
 * wrapping dict so that can change implementation without changing
 * behaviour
 */
static ILDiffDict *ILDiffDict_new(void* val,ILToken type)
{
	ILDiffDict *retval;
	retval=(ILDiffDict*)calloc(1,sizeof(ILDiffDict));
	retval->prev=NULL;//make sure
	retval->info=val;
	retval->type=type;//just to typecast
	return retval;
}
static  ILDiffDict *ILDiff_dict_add(ILDiffDict *prev,void *val,ILToken type)
{
	ILDiffDict *retval=ILDiffDict_new(val,type);
	retval->prev=prev;
	return retval;
}

static void ILDiffDict_destroy(ILDiffDict *end)
{
	ILDiffDict *tmp;
	//move to first pos
	while(end!=NULL)
	{
		tmp=end->prev;
		free(end);
		end=tmp;
	}	
}

/*
 * end of data housekeeping functions
 */

/*
 * quick and dirty search
 */
static int ILDiff_search(ILDiffDict *list1,ILDiffDict *val)
		//one is a list and other is a val
{
	ILDiffDict *i;
	for(i=list1;i!=NULL;i=i->prev)
	{
			if(ILDiff_compare(i,val)==IL_DIFF_SAME)
			{
					//since I cannot delete
					i->found=1;
					val->found=1;
					return IL_DIFF_FOUND;
			}
	}
	return IL_DIFF_NOT_FOUND;
}
/*
 * wrapping dict traversal
 */
static void ILDiff_iterate(ILDiffDict *start,void (*func)(FILE *,ILDiffDict *,int,void *),FILE *stream,int n,void *data)
{
	ILDiffDict *i;
	for(i=start;i!=NULL;i=i->prev)
	{
		func(stream,i,n,data);
	}
}
/************************
 * End code from dict.c *
 ************************/

/*************************
 * code from old print.c *
 *************************/
static void ILDiff_dumpModuleInfo(FILE *fp,ILImage *img,ILModule *module,int isin)
{
	ILColorBold();
	fprintf(fp,"%c ",isin);
	ILColorClear();
	ILColorGreen();
	fprintf(fp,"Module ");	
	ILColorBold();
	fprintf(fp,"%s\n",(ILModule_Name(module)==NULL)?"'<Module>'":\
					ILModule_Name(module));
	ILColorClear();
}

static void ILDiff_dumpMethodInfo(FILE *fp,ILImage *img,ILMethod *method,int isin)
{		
		ILColorBold();
		fprintf(fp,"%c ",isin);
		ILColorClear();ILColorGreen();
		ILDumpFlags(fp,ILMethod_Attrs(method),ILMethodDefinitionFlags,0);
		ILColorBold();
		ILDumpMethodType(fp,img,ILMethod_Signature(method),0,\
						ILMethod_Owner(method),ILMethod_Name(method),method);
		
		fprintf(fp," ");
		ILColorRed();
		ILDumpFlags(fp,ILMethod_ImplAttrs(method),\
						ILMethodImplementationFlags,0);
		ILColorClear();
		fprintf(fp,"\n");
}
static void ILDiff_dumpClassInfo(FILE *fp,ILImage *img,ILClass *klass,int isin)
{
	ILColorBold();
	fprintf(fp,"%c ",isin);
	ILColorYellow();
	fprintf(fp,"%s",(ILClass_Namespace(klass)==NULL)?\
					"<No_Namespace>":ILClass_Namespace(klass));
	ILColorRed();
	fprintf(fp," :: ");
	ILColorClear();ILColorGreen();
	ILDumpFlags(fp,ILClass_Attrs(klass),ILTypeDefinitionFlags,0);
	ILColorBold();
	ILDumpClassName(fp,img,klass,ILClass_Attrs(klass));
	ILColorClear();
	fprintf(fp,"\n");
}
static void ILDiff_dumpFieldInfo(FILE *fp,ILImage *img,ILField *field,int isin)
{
	ILColorBold();
	fprintf(fp,"%c ",isin);
	ILColorYellow();
	fprintf(fp,"%s :: ",ILClass_Name(ILField_Owner(field)));
	ILColorClear();
	ILColorGreen();
	ILDumpFlags(fp,ILField_Attrs(field),ILFieldDefinitionFlags,0);
	ILColorBold();
	ILColorRed();
	ILDumpType(fp,img,ILField_Type(field),0);
	ILColorGreen();
	fprintf(fp,"%s",ILField_Name(field));
	ILColorClear();
	fprintf(fp,"\n");
}
static void ILDiff_dump(FILE *fp,ILImage *img,ILDiffDict *d,int type)
{
		if(d->type == IL_META_TOKEN_MODULE)
				ILDiff_dumpModuleInfo(fp,img,(ILModule*)d->info,type);
		else if(d->type == IL_META_TOKEN_METHOD_DEF)
				ILDiff_dumpMethodInfo(fp,img,(ILMethod*)d->info,type);
		else if(d->type == IL_META_TOKEN_TYPE_DEF)
				ILDiff_dumpClassInfo(fp,img,(ILClass*)d->info,type);
		else if(d->type == IL_META_TOKEN_FIELD_DEF)
				ILDiff_dumpFieldInfo(fp,img,(ILField*)d->info,type);
}
static void ILDiff_header(FILE *fp,char *txt)
{
	int len=strlen(txt);
	int i;
	ILColorMagenta();
	for(i=0;i<40-len/2;i++)fprintf(fp,"*");
	ILColorBold();
	fprintf(fp,"%s",txt);
	ILColorClear();ILColorMagenta();
	for(i=0;i<40-len/2;i++)fprintf(fp,"*");
	fprintf(fp,"\n");
	ILColorClear();
}
/******************
 * End print code *
 ******************/
/*
 * main code section
 */
//refer il_utils.h for a detailed look into Command Line options
static ILCmdLineOption const options[] = \
{
  {"-p", 'p', 0, 0, 0,},  
  {"--pretty", 'p', 0, "-p or --pretty", "Format print using colors"},
  {"-h", 'h', 0, 0, 0},
  {"--help", 'h', 0, "-h or --help", "Print this help message"},
  {"--version", 'v', 0, "-v or --version",
   "Print the version of this program"},
  {"-v", 'v', 0, 0, 0},
  {"-s",'s',0,0,0,},
  {"--same",'s',0,"-s or --same","print same instead of diff"},
  {"-o",'o',1,0,0},
  {"--output",'o',1,"-o or --output","redirect output to filename"},
  {0, 0, 0, 0, 0}
};

//I think I'd better use what Rhys uses (is popt better for options?)

/*
 * Prototypes
 */
static ILImage *ILDiff_loadImage (const char *filename, ILContext * context,\
				int flags);

static ILDiffDict *ILDiffDict_make (ILImage * img);

static void 
ILDiff_diff (FILE *,ILImage *, ILDiffDict * d1, ILImage *, ILDiffDict * d2);
/*
 * usage & version 
 */
static void usage ()
{
	fprintf (stderr, "ILDIFF " VERSION " - IL compare utility\n");
	fprintf (stderr, "Copyright (c) 2001 FreeDevelopers.net\n");
	fprintf (stderr, "\n");
	fprintf (stderr, "Usage : ildiff [options] file1 file2\n\n");
	ILCmdLineHelp (options);
}

static void version ()
{
  fprintf (stderr, "ILDIFF " VERSION " - IL Compare Utility\n");
  fprintf (stderr, "Copyright (c) 2001 FreeDevelopers.net.\n\n");
  fprintf (stderr,\
		"ILDIFF comes with ABSOLUTELY NO WARRANTY.  This is free software,\n");
  fprintf (stderr,\
		"and you are welcome to redistribute it under the terms of the\n");
  fprintf (stderr, "GNU General Public License.\n\n");
  fprintf (stderr,
	   "Use the `--help' option to get help on the command-line options.\n");
}

int main (int argc, char *argv[])
{
	FILE *out=stdout;
	char *prog = argv[0];
	char *fname1, *fname2;
	ILImage *img1, *img2;
	ILContext *context;
	ILDiffDict *d1, *d2;
	char *param;	
	int opt, state = 0;
	char *to=NULL;
  //no options for now --fill in here with opts
	while((opt=ILCmdLineNextOption (&argc, &argv, &state, options, &param))!=0)
 	{
    	switch (opt)
		{
			case 'p':
	  			pretty_print = 1;
	  			break;
			case 's':
				same=1;
				break;
			case 'v':
			  	version ();
			  	return IL_DIFF_OK;//exit()
			case 'h':
		  		usage ();
	  			return IL_DIFF_OK;//exit()
			case 'o':
				to=strdup(param);
				//fp=fopen(param,"w");
				//do not open file here itself
				//in case there is a mistake in the 
				//other params eg
				//If I forgot to give the outfile param
				//the first ILImage will be erased
				pretty_print=0;//disable pretty print
				break;
			default:
	  			usage ();
	  			return IL_DIFF_ERROR;
		}
    }
  //we need at least 2 file options
	if (argc <= 2)
    {
    	usage ();
      	return IL_DIFF_ERROR;
    }
	fname1 = argv[1];
	fname2 = argv[2];
	//now on to IL code portion	
	context = ILContextCreate();
	
 	if (!context)
  	{
    	fprintf (stderr, "%s:out of available memory\n", prog);
      	return IL_DIFF_ERROR;
    }
	//load COFF image but do not resolve for extern refs
  	img1=ILDiff_loadImage (fname1, context,\
					IL_LOADFLAG_FORCE_32BIT | IL_LOADFLAG_NO_RESOLVE);	
  	if (!img1)
    {
    	fprintf (stderr, "%s:could not load image file %s \n", prog, fname1);
      	return IL_DIFF_ERROR;
    }
  img2=ILDiff_loadImage (fname2, context,\
				  IL_LOADFLAG_FORCE_32BIT | IL_LOADFLAG_NO_RESOLVE);
  	if (!img2)
    {
      	fprintf (stderr, "%s:could not open image file %s\n", prog, fname2);
      	return IL_DIFF_ERROR;
    }
  	d1 = ILDiffDict_make (img1);
  	d2 = ILDiffDict_make (img2);
	if(to!=NULL)out=fopen(to,"w");
  	ILDiff_diff (out,img1, d1, img2, d2);
  //cleanup code
    if(out!=stdout || out!=stderr)fclose(out);
	if(to!=NULL)free(to);
  	ILImageDestroy (img1);
  	ILImageDestroy (img2);
  	ILContextDestroy (context);
	ILDiffDict_destroy(d1);
	ILDiffDict_destroy(d2);
  	return IL_DIFF_OK;
}

/**
 *  load Image from il file
 */
static ILImage* 
ILDiff_loadImage (const char *filename, ILContext * context, int flags)
{
	int loadError;
  	ILImage *image;
  	FILE *stream;
  	/*attempt to open file */
  	stream = fopen (filename, "rb");
  	if (stream == NULL)
    	stream = fopen (filename, "r");	//just in case you use an old libc
  	if (stream == NULL)
    {
    	fprintf (stderr, "file missing:%s\n", filename);
    	return NULL;		//just make sure the pointer is set to NULL
   	}
  	/* Attempt to load the image into memory */
  	loadError = ILImageLoad (stream, filename, context, &image, flags);
  	if (loadError != 0)
    {
      	fprintf (stderr, "%s: %s\n", filename, ILImageLoadError (loadError));
      	return NULL;
    }

  /* Loaded and ready to go */
  	return image;
}

/*
 * Make a dict list from
 * the pe/coff Image 
 */

/*
 * Typed dict maker
 */

static ILDiffDict * ILDiffDict_make_fields (ILImage * img, ILDiffDict * start)
{
  	unsigned long fields;
  	ILToken token;		//int 32
  	ILField *data;
  	fields = ILImageNumTokens (img, IL_META_TOKEN_FIELD_DEF);

  	//Rhys has used this , someone could write a function to iterate through
  	//tokens (I mean get the first token , I couldn't bring myself to use 
  	//ILImageNextToken(,,NULL) to get the first ) is it right to use it ?
  	//Please reply ASAP --Gopal
  	for (token = 1; token <= fields; token++)
    {
      data =(ILField *)(ILImageTokenInfo(img,IL_META_TOKEN_FIELD_DEF | token));
    	if (data)
		{
	  		start = ILDiff_dict_add (start, data, IL_META_TOKEN_FIELD_DEF);
		}
    }
  	return start;
}
static ILDiffDict *ILDiffDict_make_methods (ILImage * img, ILDiffDict * start)
{
  	unsigned long methods;
  	ILToken token;		//int 32
  	ILMethod *data;
  	methods = ILImageNumTokens (img, IL_META_TOKEN_METHOD_DEF);
  	for (token = 1; token <= methods; token++)
    {
      	data=(ILMethod*)(ILImageTokenInfo(img,IL_META_TOKEN_METHOD_DEF|token));
      	if (data)
		{
	  		start = ILDiff_dict_add (start, data, IL_META_TOKEN_METHOD_DEF);
		}
    }
  	return start;
}

static ILDiffDict * ILDiffDict_make_modules (ILImage * img, ILDiffDict * start)
{
  	unsigned long modules;
  	ILToken token;
  	ILModule *data;
  	modules = ILImageNumTokens (img, IL_META_TOKEN_MODULE);
  	for (token = 1; token <= modules; token++)
    {
      	data=(ILModule*)(ILImageTokenInfo (img, IL_META_TOKEN_MODULE | token));
      	if (data)
		{
	  		start = ILDiff_dict_add (start, data, IL_META_TOKEN_MODULE);
		}
    }
  	return start;
}
static ILDiffDict * ILDiffDict_make_classes (ILImage * img, ILDiffDict * start)
{
  	ILClass *data = 0;
  	while ((data = (ILClass *) ILImageNextToken (img, IL_META_TOKEN_TYPE_DEF,
					       data)) != 0)
    {
      	if (data)
		{
	  		start = ILDiff_dict_add (start, data, IL_META_TOKEN_TYPE_DEF);
		}
    }
  	return start;
}
static ILDiffDict * ILDiffDict_make (ILImage * img)
{
  	ILDiffDict *start = NULL;
  	start = ILDiffDict_make_modules (img, start);
  	start = ILDiffDict_make_classes (img, start);
  	start = ILDiffDict_make_fields (img, start);
  	start = ILDiffDict_make_methods (img, start);
  	return start;
}
static void  ILDiff_output (FILE *stream,ILDiffDict * d, int out, void *data)
{
  	if (d->found == same)
    	ILDiff_dump (stream,(ILImage *) (data), d, out);
}
/*
 * Yeah ! The real McCoy : the diff function
 */
static void ILDiff_diff (FILE *stream,ILImage * img1, ILDiffDict * d1, ILImage * img2, ILDiffDict * d2)
{
  	ILDiffDict *tmp;
	int result=0;
  	for (tmp = d2; tmp != NULL; tmp = tmp->prev)
    {
      	result|=ILDiff_search (d1, tmp);
    }
	if(same && (result & IL_DIFF_FOUND))
	{
		if(pretty_print)ILDiff_header(stream,"Same");
		ILDiff_iterate(d1,ILDiff_output,stream,IL_DIFF_NONE,img1);
		//we know that same members are in d1 and d2
		//so print only one list
	}
	if(!same && (result & IL_DIFF_NOT_FOUND))
	{
		if(pretty_print)ILDiff_header(stream,"Outgoing");
	 	ILDiff_iterate (d1, ILDiff_output,stream, IL_DIFF_OUT, img1);
		if(pretty_print)ILDiff_header (stream,"Incoming");
  		ILDiff_iterate (d2, ILDiff_output,stream,IL_DIFF_IN, img2);
	}
}
