/*
 * ilverify.c - Bulk verification tool for IL programs.
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
#include "engine.h"
#include "il_utils.h"

#ifdef	__cplusplus
extern	"C" {
#endif

/*
 * Table of command-line options.
 */
static ILCmdLineOption const options[] = {
	{"-u", 'u', 0, 0, 0},
	{"-v", 'v', 0, 0, 0},
	{"--unsafe", 'u', 0,
		"--unsafe  or -u",
		"Allow unsafe code."},
	{"--version", 'v', 0,
		"--version or -v",
		"Print the version of the program"},
	{"--help", 'h', 0,
		"--help",
		"Print this help message."},
	{0, 0, 0, 0, 0}
};

static void usage(const char *progname);
static void version(void);
static int verify(const char *filename, FILE *stream, int closeStream,
				  ILContext *context, int allowUnsafe);

int main(int argc, char *argv[])
{
	char *progname = argv[0];
	int allowUnsafe = 0;
	int sawStdin;
	int state, opt;
	char *param;
	FILE *infile;
	int errors;
	ILContext *context;

	/* Parse the command-line arguments */
	state = 0;
	while((opt = ILCmdLineNextOption(&argc, &argv, &state,
									 options, &param)) != 0)
	{
		switch(opt)
		{
			case 'u':
			{
				allowUnsafe = 1;
			}
			break;

			case 'v':
			{
				version();
				return 0;
			}
			/* Not reached */

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

	/* Initialize the engine, to ensure that the garbage collector is OK */
	ILExecInit(0);

	/* Create a context to use for image loading */
	context = ILContextCreate();
	if(!context)
	{
		fprintf(stderr, "%s: out of memory\n", progname);
		return 1;
	}

	/* Load and verify the input files */
	sawStdin = 0;
	errors = 0;
	while(argc > 1)
	{
		if(!strcmp(argv[1], "-"))
		{
			/* Verify the contents of stdin, but only once */
			if(!sawStdin)
			{
				errors |= verify("stdin", stdin, 0, context, allowUnsafe);
				sawStdin = 1;
			}
		}
		else
		{
			/* Verify the contents of a regular file */
			if((infile = fopen(argv[1], "rb")) == NULL)
			{
				/* Try again in case libc did not understand the 'b' */
				if((infile = fopen(argv[1], "r")) == NULL)
				{
					perror(argv[1]);
					errors = 1;
					++argv;
					--argc;
					continue;
				}
			}
			errors |= verify(argv[1], infile, 1, context, allowUnsafe);
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
	fprintf(stderr, "ILVERIFY " VERSION " - IL Image Verification Utility\n");
	fprintf(stderr, "Copyright (c) 2001 Southern Storm Software, Pty Ltd.\n");
	fprintf(stderr, "\n");
	fprintf(stderr, "Usage: %s [options] input ...\n", progname);
	fprintf(stderr, "\n");
	ILCmdLineHelp(options);
}

static void version(void)
{
	printf("ILVERIFY " VERSION " - IL Image Verification Utility\n");
	printf("Copyright (c) 2001 Southern Storm Software, Pty Ltd.\n");
	printf("\n");
	printf("ILVERIFY comes with ABSOLUTELY NO WARRANTY.  This is free software,\n");
	printf("and you are welcome to redistribute it under the terms of the\n");
	printf("GNU General Public License.  See the file COPYING for further details.\n");
	printf("\n");
	printf("Use the `--help' option to get help on the command-line options.\n");
}

/*
 * Stubs for coder functions.
 */
static ILCoder *Coder_Create(ILUInt32 size)
{
	return 0;
}
static ILUInt32 Coder_Generation(ILCoder *coder)
{
	return 1;
}
static void *Coder_Alloc(ILCoder *coder, ILUInt32 size)
{
	return 0;
}
static int Coder_Setup(ILCoder *coder, unsigned char **start,
					   ILMethod *method, ILMethodCode *code)
{
	return 1;
}
static int Coder_SetupExtern(ILCoder *coder, unsigned char **start,
							 ILMethod *method, void *fn, void *cif,
							 int isInternal)
{
	return 1;
}
static int Coder_SetupExternCtor(ILCoder *coder, unsigned char **start,
							     ILMethod *method, void *fn, void *cif,
							     void *ctorfn, void *ctorcif, int isInternal)
{
	return 1;
}
static int Coder_CtorOffset(ILCoder *coder)
{
	return 0;
}
static void Coder_Destroy(ILCoder *coder)
{
}
static void Coder_Flush(ILCoder *coder)
{
}
static int Coder_Finish(ILCoder *coder)
{
	return 1;
}
static int Coder_Restart(ILCoder *coder)
{
	return 0;
}
static void Coder_Label(ILCoder *coder, ILUInt32 offset)
{
}
static void Coder_StackRefresh(ILCoder *coder, ILEngineStackItem *stack,
							   ILUInt32 stackSize)
{
}
static void Coder_Constant(ILCoder *coder, int opcode, unsigned char *arg)
{
}
static void Coder_StringConstant(ILCoder *coder, ILToken token)
{
}
static void Coder_Binary(ILCoder *coder, int opcode, ILEngineType type1,
				   	     ILEngineType type2)
{
}
static void Coder_BinaryPtr(ILCoder *coder, int opcode, ILEngineType type1,
				      		ILEngineType type2)
{
}
static void Coder_Shift(ILCoder *coder, int opcode, ILEngineType type1,
				  		ILEngineType type2)
{
}
static void Coder_Unary(ILCoder *coder, int opcode, ILEngineType type)
{
}
static void Coder_LoadArg(ILCoder *coder, ILUInt32 num, ILType *type)
{
}
static void Coder_StoreArg(ILCoder *coder, ILUInt32 num,
					 	   ILEngineType engineType, ILType *type)
{
}
static void Coder_AddrOfArg(ILCoder *coder, ILUInt32 num)
{
}
static void Coder_LoadLocal(ILCoder *coder, ILUInt32 num, ILType *type)
{
}
static void Coder_StoreLocal(ILCoder *coder, ILUInt32 num,
					   		 ILEngineType engineType, ILType *type)
{
}
static void Coder_AddrOfLocal(ILCoder *coder, ILUInt32 num)
{
}
static void Coder_Dup(ILCoder *coder, ILEngineType valueType, ILType *type)
{
}
static void Coder_Pop(ILCoder *coder, ILEngineType valueType, ILType *type)
{
}
static void Coder_ArrayAccess(ILCoder *coder, int opcode,
							  ILEngineType indexType, ILType *elemType)
{
}
static void Coder_PtrAccess(ILCoder *coder, int opcode)
{
}
static void Coder_PtrAccessManaged(ILCoder *coder, int opcode,
								   ILClass *classInfo)
{
}
static void Coder_Branch(ILCoder *coder, int opcode, ILUInt32 dest,
				   		 ILEngineType type1, ILEngineType type2)
{
}
static void Coder_SwitchStart(ILCoder *coder, ILUInt32 numEntries)
{
}
static void Coder_SwitchEntry(ILCoder *coder, ILUInt32 dest)
{
}
static void Coder_Compare(ILCoder *coder, int opcode,
				   		  ILEngineType type1, ILEngineType type2,
						  int invertTest)
{
}
static void Coder_Conv(ILCoder *coder, int opcode, ILEngineType type)
{
}
static void Coder_ToPointer(ILCoder *coder, ILEngineType type1,
							ILEngineStackItem *type2)
{
}
static void Coder_PtrPrefix(ILCoder *coder, int alignment)
{
}
static void Coder_ArrayLength(ILCoder *coder)
{
}
static void Coder_NewArray(ILCoder *coder, ILType *arrayType,
						   ILClass *arrayClass, ILEngineType lengthType)
{
}
static void Coder_LocalAlloc(ILCoder *coder, ILEngineType sizeType)
{
}
static void Coder_CastClass(ILCoder *coder, ILClass *classInfo,
							int throwException)
{
}
static void Coder_LoadField(ILCoder *coder, ILEngineType ptrType,
							ILType *objectType, ILField *field,
							ILType *fieldType)
{
}
static void Coder_LoadStaticField(ILCoder *coder, ILField *field,
							      ILType *fieldType)
{
}
static void Coder_LoadFieldAddr(ILCoder *coder, ILEngineType ptrType,
							    ILType *objectType, ILField *field,
							    ILType *fieldType)
{
}
static void Coder_LoadStaticFieldAddr(ILCoder *coder, ILField *field,
							          ILType *fieldType)
{
}
static void Coder_StoreField(ILCoder *coder, ILEngineType ptrType,
							 ILType *objectType, ILField *field,
							 ILType *fieldType, ILEngineType valueType)
{
}
static void Coder_StoreStaticField(ILCoder *coder, ILField *field,
							       ILType *fieldType, ILEngineType valueType)
{
}
static void Coder_CopyObject(ILCoder *coder, ILEngineType destPtrType,
							 ILEngineType srcPtrType, ILClass *classInfo)
{
}
static void Coder_CopyBlock(ILCoder *coder, ILEngineType destPtrType,
							ILEngineType srcPtrType)
{
}
static void Coder_InitObject(ILCoder *coder, ILEngineType ptrType,
							 ILClass *classInfo)
{
}
static void Coder_InitBlock(ILCoder *coder, ILEngineType ptrType)
{
}
static void Coder_Box(ILCoder *coder, ILClass *boxClass,
					  ILEngineType valueType, ILUInt32 size)
{
}
static void Coder_BoxSmaller(ILCoder *coder, ILClass *boxClass,
					   		 ILEngineType valueType, ILType *smallerType)
{
}
static void Coder_Unbox(ILCoder *coder, ILClass *boxClass)
{
}
static void Coder_MakeTypedRef(ILCoder *coder, ILClass *classInfo)
{
}
static void Coder_RefAnyVal(ILCoder *coder, ILClass *classInfo)
{
}
static void Coder_RefAnyType(ILCoder *coder)
{
}
static void Coder_PushToken(ILCoder *coder, ILProgramItem *item)
{
}
static void Coder_SizeOf(ILCoder *coder, ILType *type)
{
}
static void Coder_ArgList(ILCoder *coder)
{
}
static void Coder_UpConvertArg(ILCoder *coder, ILEngineStackItem *args,
						       ILUInt32 numArgs, ILUInt32 param,
						       ILType *paramType)
{
}
static void Coder_DownConvertArg(ILCoder *coder, ILEngineStackItem *args,
						         ILUInt32 numArgs, ILUInt32 param,
						         ILType *paramType)
{
}
static void Coder_ValueCtorArgs(ILCoder *coder, ILClass *classInfo,
								ILEngineStackItem *args, ILUInt32 numArgs)
{
}
static void Coder_CallMethod(ILCoder *coder, ILEngineStackItem *args,
					   		 ILUInt32 numArgs, ILEngineStackItem *returnItem,
							 ILMethod *methodInfo)
{
}
static void Coder_CallCtor(ILCoder *coder, ILEngineStackItem *args,
					   	   ILUInt32 numArgs, ILMethod *methodInfo)
{
}
static void Coder_CallVirtual(ILCoder *coder, ILEngineStackItem *args,
					    	  ILUInt32 numArgs, ILEngineStackItem *returnItem,
							  ILMethod *methodInfo)
{
}
static void Coder_CallInterface(ILCoder *coder, ILEngineStackItem *args,
					      		ILUInt32 numArgs, ILEngineStackItem *returnItem,
								ILMethod *methodInfo)
{
}
static void Coder_JumpMethod(ILCoder *coder, ILMethod *methodInfo)
{
}
static void Coder_ReturnInsn(ILCoder *coder, ILEngineType engineType,
							 ILType *returnType)
{
}
static void Coder_LoadFuncAddr(ILCoder *coder, ILMethod *methodInfo)
{
}
static void Coder_LoadVirtualAddr(ILCoder *coder, ILMethod *methodInfo)
{
}
static void Coder_LoadInterfaceAddr(ILCoder *coder, ILMethod *methodInfo)
{
}
static void Coder_Throw(ILCoder *coder)
{
}
static void Coder_Jsr(ILCoder *coder, ILUInt32 dest)
{
}
static void Coder_RetFromJsr(ILCoder *coder)
{
}

/*
 * Default coder class and instance.
 */
static ILCoderClass const DefaultCoderClass = {
	Coder_Create,
	Coder_Generation,
	Coder_Alloc,
	Coder_Setup,
	Coder_SetupExtern,
	Coder_SetupExternCtor,
	Coder_CtorOffset,
	Coder_Destroy,
	Coder_Flush,
	Coder_Finish,
	Coder_Restart,
	Coder_Label,
	Coder_StackRefresh,
	Coder_Constant,
	Coder_StringConstant,
	Coder_Binary,
	Coder_BinaryPtr,
	Coder_Shift,
	Coder_Unary,
	Coder_LoadArg,
	Coder_StoreArg,
	Coder_AddrOfArg,
	Coder_LoadLocal,
	Coder_StoreLocal,
	Coder_AddrOfLocal,
	Coder_Dup,
	Coder_Pop,
	Coder_ArrayAccess,
	Coder_PtrAccess,
	Coder_PtrAccessManaged,
	Coder_Branch,
	Coder_SwitchStart,
	Coder_SwitchEntry,
	Coder_Compare,
	Coder_Conv,
	Coder_ToPointer,
	Coder_PtrPrefix,
	Coder_ArrayLength,
	Coder_NewArray,
	Coder_LocalAlloc,
	Coder_CastClass,
	Coder_LoadField,
	Coder_LoadStaticField,
	Coder_LoadFieldAddr,
	Coder_LoadStaticFieldAddr,
	Coder_StoreField,
	Coder_StoreStaticField,
	Coder_CopyObject,
	Coder_CopyBlock,
	Coder_InitObject,
	Coder_InitBlock,
	Coder_Box,
	Coder_BoxSmaller,
	Coder_Unbox,
	Coder_MakeTypedRef,
	Coder_RefAnyVal,
	Coder_RefAnyType,
	Coder_PushToken,
	Coder_SizeOf,
	Coder_ArgList,
	Coder_UpConvertArg,
	Coder_DownConvertArg,
	Coder_ValueCtorArgs,
	Coder_CallMethod,
	Coder_CallCtor,
	Coder_CallVirtual,
	Coder_CallInterface,
	Coder_JumpMethod,
	Coder_ReturnInsn,
	Coder_LoadFuncAddr,
	Coder_LoadVirtualAddr,
	Coder_LoadInterfaceAddr,
	Coder_Throw,
	Coder_Jsr,
	Coder_RetFromJsr,
};
static ILCoder DefaultCoder = {&DefaultCoderClass};

/*
 * Print a verification error.
 */
static void printError(ILMethod *method, const char *msg)
{
	ILClass *classInfo = ILMethod_Owner(method);
	const char *namespace = ILClass_Namespace(classInfo);
	if(namespace)
	{
		fputs(namespace, stdout);
		putc('.', stdout);
	}
	fputs(ILClass_Name(classInfo), stdout);
	fputs("::", stdout);
	fputs(ILMethod_Name(method), stdout);
	fputs(" - ", stdout);
	fputs(msg, stdout);
	putc('\n', stdout);
}

/*
 * Load an IL image from an input stream and verify all of its methods.
 */
static int verify(const char *filename, FILE *stream, int closeStream,
				  ILContext *context, int allowUnsafe)
{
	int loadError;
	ILImage *image;
	ILMethod *method;
	ILMethodCode code;
	int result;
	unsigned char *start;

	/* Attempt to load the image into memory */
	loadError = ILImageLoad(stream, filename, context, &image,
							IL_LOADFLAG_FORCE_32BIT | IL_LOADFLAG_NO_RESOLVE);
	if(closeStream)
	{
		fclose(stream);
	}
	if(loadError != 0)
	{
		fprintf(stderr, "%s: %s\n", filename, ILImageLoadError(loadError));
		return 0;
	}

	/* Scan the entire MethodDef table and verify everything we find */
	method = 0;
	while((method = (ILMethod *)ILImageNextToken
				(image, IL_META_TOKEN_METHOD_DEF, (void *)method)) != 0)
	{
		/* Skip this method if it does not have IL bytecode */
		if(!ILMethod_RVA(method))
		{
			continue;
		}

		/* Get the IL bytecode for the method */
		if(!ILMethodGetCode(method, &code))
		{
			printError(method, "malformed code");
			continue;
		}

		/* Verify the method */
		result = _ILVerify(&DefaultCoder, &start, method, &code, allowUnsafe);
		if(!result)
		{
			printError(method, "could not verify code");
		}
	}

	/* Clean up and exit */
	ILImageDestroy(image);
	return 0;
}

#ifdef	__cplusplus
};
#endif
