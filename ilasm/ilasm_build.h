/*
 * ilasm_build.h - Data structure building helper routines.
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

#ifndef	_ILASM_BUILD_H
#define	_ILASM_BUILD_H

#include "il_program.h"
#include "il_system.h"
#include "il_utils.h"

#ifdef	__cplusplus
extern	"C" {
#endif

/*
 * Separator to use for nested class names.
 */
#define	ILASM_NESTED_CLASS_SEP		'\377'
#define	ILASM_NESTED_CLASS_SEP_STR	"\377"

/*
 * Parsed parameter information.
 */
typedef struct _tagILAsmParamInfo ILAsmParamInfo;
struct _tagILAsmParamInfo
{
	ILType		   *type;
	ILIntString		nativeType;
	ILUInt32		parameterAttrs;
	char		   *name;
	ILAsmParamInfo *next;
};

/*
 * Context within which the image is being built.
 */
extern ILContext *ILAsmContext;

/*
 * Image that is being built.
 */
extern ILImage *ILAsmImage;

/*
 * Module record for the image that is being built.
 */
extern ILModule *ILAsmModule;

/*
 * Assembly record for the image that is being built.
 */
extern ILAssembly *ILAsmAssembly;

/*
 * Last token that we saw, for attaching custom attributes.
 */
extern ILUInt32 ILAsmLastToken;

/*
 * Current item scope.
 */
extern ILProgramItem *ILAsmCurrScope;

/*
 * Current class and assembly that is being assembled.
 */
extern ILClass *ILAsmClass;
extern ILAssembly *ILAsmCurrAssemblyRef;

/*
 * The "<Module>" class in the current assembly.
 */
extern ILClass *ILAsmModuleClass;

/*
 * Debug information.
 */
extern int ILAsmDebugMode;
extern char *ILAsmDebugLastFile;

/*
 * Name of the standard library.
 */
extern char *ILAsmLibraryName;

/*
 * Reset global variables to their default state.
 */
void ILAsmBuildReset(void);

/*
 * Initialize the building routines.
 */
void ILAsmBuildInit(const char *outputFilename);

/*
 * Push a new namespace.
 */
void ILAsmBuildPushNamespace(ILIntString name);

/*
 * Pop the namespace.
 */
void ILAsmBuildPopNamespace(int nameLen);

/*
 * Push a program item as the current scope.
 */
void _ILAsmBuildPushScope(ILProgramItem *item);
#define	ILAsmBuildPushScope(item)	\
			(_ILAsmBuildPushScope(ILToProgramItem((item))))

/*
 * Pop the current scope and return to the previous item.
 */
void ILAsmBuildPopScope(void);

/*
 * Split a string into name and namespace.
 */
void ILAsmSplitName(const char *str, int len, const char **name,
					const char **namespace);

/*
 * Create a new class and push it onto the class stack.
 */
void ILAsmBuildNewClass(const char *name, ILAsmParamInfo *genericParams,
						ILClass *parent, ILUInt32 attrs);

/*
 * Pop the class stack.
 */
void ILAsmBuildPopClass(void);

/*
 * End processing of the global module class.
 */
void ILAsmBuildEndModule(void);

/*
 * Look up a class with a specific name, and create a
 * TypeRef within the given scope if not found.
 */
ILClass *ILAsmClassLookup(const char *name, ILProgramItem *scope);

/*
 * Find an AssemblyRef record given its name.
 * NULL if the name refers to the current assembly.
 */
ILAssembly *ILAsmFindAssemblyRef(const char *name);

/*
 * Find a ModuleRef record given its name.
 * NULL if the name refers to the current module.
 */
ILModule *ILAsmFindModuleRef(const char *name);

/*
 * Find a file record given its name.
 */
ILFileDecl *ILAsmFindFile(const char *name, ILUInt32 attrs, int ref);

/*
 * Resolve the token for a member reference.
 */
ILToken ILAsmResolveMember(ILProgramItem *scope, const char *name,
						   ILType *signature, int kind);

/*
 * Create a method within a class.  If there is already
 * a forward reference as a MemberRef, then reuse it.
 */
ILMethod *ILAsmMethodCreate(ILClass *classInfo, const char *name,
							ILUInt32 attributes, ILType *sig);

/*
 * Create a field within a class.  If there is already
 * a forward reference as a MemberRef, then resuse it.
 */
ILField *ILAsmFieldCreate(ILClass *classInfo, const char *name,
						  ILUInt32 attributes, ILType *sig);

/*
 * Create a custom attribute and attach it to a particular item.
 */
void ILAsmAttributeCreateFor(ILToken token, ILProgramItem *type,
							 ILIntString *value);

/*
 * Create a custom attribute and attach it to the current item.
 */
void ILAsmAttributeCreate(ILProgramItem *type, ILIntString *value);

/*
 * Create a declarative security blob for the current item.
 */
void ILAsmSecurityCreate(ILInt64 action, const void *str, int len);

/*
 * Find a numbered parameter for a particular method.
 * Returns NULL if not found.
 */
ILParameter *ILAsmFindParameter(ILMethod *method, ILUInt32 paramNum);

/*
 * Add semantics to an event or property.
 */
void ILAsmAddSemantics(int type, ILToken token);

/*
 * Process a debug line within the input stream.
 */
void ILAsmDebugLine(ILUInt32 line, ILUInt32 column, char *filename);

/*
 * Get a reference to a standard class within the "System" namespace.
 */
ILClass *ILAsmSystemClass(const char *name);

#ifdef	__cplusplus
};
#endif

#endif	/* _ILASM_BUILD_H */
