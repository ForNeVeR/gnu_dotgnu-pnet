/*
 * cg_gen.h - Definition of the "ILGenInfo" structure and helper routines.
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

#ifndef	_CODEGEN_CG_GEN_H
#define	_CODEGEN_CG_GEN_H

#ifdef	__cplusplus
extern	"C" {
#endif

/*
 * Loop stack entry.  If "continueLabel" is NULL, and "breakLabel"
 * is not NULL, then the entry is a "switch".  If both are NULL,
 * then it indicates the position of a "try" block on the stack.
 */
typedef struct
{
	ILLabel		   *continueLabel;
	ILLabel		   *breakLabel;
	ILLabel		   *finallyLabel;

} ILLoopStack;

/*
 * Goto list entry.
 */
typedef struct _tagILGotoEntry
{
	char		   *name;
	int				defined : 1;
	int				crossedTry : 1;
	long			loopStackSize;
	long			scopeLevel;
	ILLabel			label;
	struct _tagILGotoEntry *next;

} ILGotoEntry;

/*
 * Information about allocated local variable slots.
 */
typedef struct _tagILLocalVar
{
	char		   *name;
	long			scopeLevel;
	ILType		   *type;
	short			allocated;

} ILLocalVar;

/*
 * Opaque definition of the Java generator routines.
 */
typedef struct _tagILJavaGenInfo ILJavaGenInfo;

/*
 * Structure of the code generation context.
 */
struct _tagILGenInfo
{
	char		   *progname;			/* Name of the program */
	FILE		   *asmOutput;			/* Assembly output stream */
	ILContext	   *context;			/* Context that is being built */
	ILImage		   *image;				/* Image that is being built */
	ILImage		   *libImage;			/* Image that holds library defns */
	ILMemPool		nodePool;			/* Memory pool for the nodes */
	ILMemPool		scopePool;			/* Memory pool for scopes */
	ILMemPool		scopeDataPool;		/* Memory pool for scope data items */
	ILLabel			nextLabel;			/* Next temporary label to allocate */
	int				overflowInsns : 1;	/* Use overflow instructions */
	int				pedanticArith : 1;	/* Make arithmetic always accurate */
	int				clsCompliant : 1;	/* Use strict CLS library only */
	int				semAnalysis : 1;	/* Non-zero during semantic analysis */
	int				typeGather : 1;		/* Non-zero during type gathering */
	int				useJavaLib : 1;		/* Use Java and not C# library */
	int				outputIsJava : 1;	/* Output Java bytecode */
	int             decimalRoundMode;	/* Rounding mode for ILDecimal */
	long			stackHeight;		/* Current stack height */
	long			maxStackHeight;		/* Maximum stack height */
	ILLoopStack    *loopStack;			/* Contents of the loop stack */
	long			loopStackSize;		/* Size of the loop stack */
	long			loopStackMax;		/* Maximum size of the loop stack */
	ILType		   *returnType;			/* Return type for current method */
	long			returnVar;			/* Temp local for return value */
	ILLabel			returnLabel;		/* End of method return code label */
	long			throwVariable;		/* Variable containing thrown value */
	ILGotoEntry    *gotoList;			/* List of goto entries */
	long			scopeLevel;			/* Current scope level */
	ILLocalVar	   *tempVars;			/* Temporary variables in the method */
	unsigned		numTempVars;		/* Number of active variables */
	unsigned		maxTempVars;		/* Maximum variables in "localVars" */
	unsigned        tempLocalBase;		/* Base for temporary local variables */
	ILScope		   *currentScope;		/* Current scope for declarations */
	ILJavaGenInfo  *javaInfo;			/* Java-specific information */
	long			unsafeLevel;		/* Number of unsafe contexts */
	int			   *contextStack;		/* Statement context stack */
	long			contextStackSize;	/* Size of statement context stack */
	long			contextStackMax;	/* Max size of context stack */
	ILNode         *currentClass;		/* Current class being processed */
	ILNode		   *currentMethod;		/* Current method being processed */
	ILNode         *currentNamespace;	/* Current namespace being processed */

};

/*
 * Adjust the height of the operand stack.
 */
#define	ILGenAdjust(info,amount)	\
			do { \
				(info)->stackHeight += (amount); \
				if((info)->stackHeight > (info)->maxStackHeight) \
				{ \
					(info)->maxStackHeight = (info)->stackHeight; \
				} \
			} while (0)

/*
 * Extend the height of the operand stack to account for temporary values.
 */
#define	ILGenExtend(info,amount)	\
			do { \
				if(((info)->stackHeight + (amount)) > (info)->maxStackHeight) \
				{ \
					(info)->maxStackHeight = (info)->stackHeight + (amount); \
				} \
			} while (0)

/*
 * Initialize an ILGenInfo structure.
 */
void ILGenInfoInit(ILGenInfo *info, char *progname, FILE *asmOutput);

/*
 * Switch an ILGenInfo structure to Java bytecode generation.
 */
void ILGenInfoToJava(ILGenInfo *info);

/*
 * Destroy an ILGenInfo structure.
 */
void ILGenInfoDestroy(ILGenInfo *info);

/*
 * Report an out of memory error and abort the compiler.
 */
void ILGenOutOfMemory(ILGenInfo *info);

/*
 * Find a type descriptor for something in the "System" namespace.
 */
ILType *ILFindSystemType(ILGenInfo *info, const char *name);

/*
 * Find a type descriptor for something in a non-"System" namespace.
 */
ILType *ILFindNonSystemType(ILGenInfo *info, const char *name,
							const char *namespace);

/*
 * Convert a type into its class form.  Returns NULL if
 * there is no class form for the type.
 */
ILClass *ILTypeToClass(ILGenInfo *info, ILType *type);

/*
 * Convert a type into a machine type.
 */
ILMachineType ILTypeToMachineType(ILType *type);

/*
 * Convert a runtime value type into a language type.
 */
ILType *ILValueTypeToType(ILGenInfo *info, ILMachineType valueType);

/*
 * Allocate a temporary local variable of a particular primitive type.
 */
unsigned ILGenTempVar(ILGenInfo *info, ILMachineType type);

/*
 * Allocate a temporary local variable of a particular real type.
 */
unsigned ILGenTempTypedVar(ILGenInfo *info, ILType *type);

/*
 * Release a temporary local variable that is no longer required.
 */
void ILGenReleaseTempVar(ILGenInfo *info, unsigned localNum);

/*
 * Determine the common type to use for a binary operator.
 */
ILMachineType ILCommonType(ILGenInfo *info, ILMachineType type1,
						   ILMachineType type2, int intonly);

/*
 * Make the system library in "info->libImage".
 */
void ILGenMakeLibrary(ILGenInfo *info);

/*
 * Determine if a program item has a particular "System"
 * attribute attached to it.
 */
int ILGenItemHasAttribute(ILProgramItem *item, const char *name);

/*
 * Add a particular "System" attribute to program item.
 */
void ILGenItemAddAttribute(ILGenInfo *info, ILProgramItem *item,
						   const char *name);

/*
 * Get the parameter information associated with a specific
 * method parameter.  "signature" can be NULL, or a cached
 * copy of the method's signature.
 */
ILParameterModifier ILGenGetParamInfo(ILMethod *method, ILType *signature,
									  ILUInt32 num, ILType **type);

#ifdef	__cplusplus
};
#endif

#endif	/* _CODEGEN_CG_GEN_H */
