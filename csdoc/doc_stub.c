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

#include <dirent.h>

#include "il_system.h"
#include "il_utils.h"
#include "doc_tree.h"
#include "doc_backend.h"
#ifdef HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif
#ifdef HAVE_SYS_STAT_H
#include <sys/stat.h>
#endif
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#include <errno.h>

#ifdef	__cplusplus
extern	"C" {
#endif

char *license="\
/*\n\
 * %s.cs - Implementation of \"%s\" \n\
 *\n\
 * Copyright (C) 2002  Southern Storm Software, Pty Ltd.\n\
 * Copyright (C) 2002  FSF India.\n\
 * \n\
 * Authors : Gopal.V (using csdoc2stub) \n\
 *\n\
 * This program is free software; you can redistribute it and/or modify\n\
 * it under the terms of the GNU General Public License as published by\n\
 * the Free Software Foundation; either version 2 of the License, or\n\
 * (at your option) any later version.\n\
 *\n\
 * This program is distributed in the hope that it will be useful,\n\
 * but WITHOUT ANY WARRANTY; without even the implied warranty of\n\
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n\
 * GNU General Public License for more details.\n\
 *\n\
 * You should have received a copy of the GNU General Public License\n\
 * along with this program; if not, write to the Free Software\n\
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA\n\
 */\n\n";

/*********END GOPAL's H4cks**************/
		
char const ILDocProgramHeader[] =
	"CSDOC2STUB " VERSION " - Convert C# documentation into Stub C#";

char const ILDocProgramName[] = "CSDOC2STUB";

ILCmdLineOption const ILDocProgramOptions[] = {
	{0, 0, 0, 0, 0}
};

char *ILDocDefaultOutput(int numInputs, char **inputs, const char *progname)
{
	if(inputs[0])return inputs[0];
	return "";
}

int ILDocValidateOutput(char *outputPath, const char *progname)
{
	/* Nothing to do here: any pathname is considered valid */
	return 1;
}

void ILDocPrintMember(FILE *fp,ILDocType *type,ILDocMember *foo)
{
	char * sig=strdup(foo->csSignature);
	if(foo->memberType == ILDocMemberType_Constructor || foo->memberType ==
					ILDocMemberType_Method)
	{
		sig[strlen(sig)-1]='\0';
		fprintf(fp,"\t\t[TODO]\n");
		fprintf(fp,"\t\t%s\n",sig);
		fprintf(fp,"\t\t{\n\t\t/*TODO*/\n\t\t}\n\n");
	}
	else fprintf(fp,"\t\t%s\n\n",sig);
}
void ILDocPrintType(ILDocNamespace *ns, ILDocType *type,char *name)
{
	ILDocMember *i;
	FILE *fp;
	char *fname=(char*)calloc(strlen(name)+strlen(type->name)+10,sizeof(char));
	sprintf(fname,"%s/%s.cs",name,type->name);
	fp=fopen(fname,"w");
	if(!fp)
	{
		fprintf(stderr,"Could not open %s\n",fname);
	}
	printf("\twriting %s\n",fname);
	fprintf(fp,license,type->name,type->fullName);
	fprintf(fp,"namespace %s\n",ns->name);
	fprintf(fp,"{\n");
	fprintf(fp,"\t%s\n",type->csSignature);
	fprintf(fp,"\t{\n");
	
	for(i=type->members;i!=NULL;i=i->next)
		ILDocPrintMember(fp,type,i);
	
	fprintf(fp,"\t}\n");
	fprintf(fp,"}//namespace");
	fclose(fp);
	free(fname);
}

void ILDocPrintNS(ILDocNamespace *ns)
{
	ILDocType *i;
	int j;
	char *name=strdup(ns->name);
	for(j=0;j<strlen(name);j++)
	{
			if(name[j]=='.')
			{
				name[j]='\0';
				mkdir(name,0777);
				//create a new dir every time we correct a NS
				name[j]='/';
			}
	}
	printf("Processing %s namespace\n",name);
	mkdir(name,0777);//make dir if still not made
	for(i=ns->types;i!=NULL;i=i->nextNamespace)
	{
		ILDocPrintType(ns,i,name);
	}
	free(name);
}

int ILDocConvert(ILDocTree *tree, int numInputs, char **inputs,
				 char *outputPath, const char *progname)
{
	ILDocNamespace *i=tree->namespaces;
	while(i!=NULL)
	{
		ILDocPrintNS(i);
		i=i->next;
	}
	return 1;
}

#ifdef	__cplusplus
};
#endif
