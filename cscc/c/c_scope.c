/*
 * c_scope.c - Scope handling for the C programming language.
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

ILScope *CCurrentScope;
ILScope *CGlobalScope;

/*
 * Make a scope name for a struct or union.
 */
static char *StructScopeName(const char *name, int structKind)
{
	ILIntString str1;
	ILIntString str2;
	if(structKind == C_STKIND_STRUCT || structKind == C_STKIND_STRUCT_NATIVE)
	{
		str1.string = "struct ";
		str1.len = 7;
	}
	else
	{
		str1.string = "union ";
		str1.len = 6;
	}
	str2.string = (char *)name;
	str2.len = strlen(name);
	return (ILInternAppendedString(str1, str2)).string;
}

/*
 * Make a scope name for an enum.
 */
static char *EnumScopeName(const char *name)
{
	ILIntString str1;
	ILIntString str2;
	str1.string = "enum ";
	str1.len = 5;
	str2.string = (char *)name;
	str2.len = strlen(name);
	return (ILInternAppendedString(str1, str2)).string;
}

/*
 * Convert a node into a persistent value that will survive
 * the treecc scope being flushed with "yynodepop()".
 */
static ILNode *PersistNode(ILNode *node)
{
	ILNode *newNode = (ILNode *)ILMalloc(sizeof(ILNode));
	if(!newNode)
	{
		CCOutOfMemory();
	}
	*newNode = *node;
	return newNode;
}

void CScopeGlobalInit(ILGenInfo *info)
{
	CGlobalScope = CCurrentScope = ILScopeCreate(info, 0);
}

void *CScopeLookup(const char *name)
{
	return (void *)ILScopeLookup(CCurrentScope, name, 1);
}

void *CScopeLookupCurrent(const char *name)
{
	return (void *)ILScopeLookup(CCurrentScope, name, 0);
}

int CScopeIsTypedef(const char *name)
{
	ILScopeData *data = ILScopeLookup(CCurrentScope, name, 1);
	if(data != 0)
	{
		return (ILScopeDataGetKind(data) == C_SCDATA_TYPEDEF);
	}
	else
	{
		return 0;
	}
}

void *CScopeLookupStructOrUnion(const char *name, int structKind)
{
	return (void *)ILScopeLookup
		(CCurrentScope, StructScopeName(name, structKind), 1);
}

int CScopeHasStructOrUnion(const char *name, int structKind)
{
	return (ILScopeLookup
				(CCurrentScope, StructScopeName(name, structKind), 0) != 0);
}

void CScopeAddStructOrUnion(const char *name, int structKind, ILType *type)
{
	ILScopeDeclareItem(CCurrentScope, StructScopeName(name, structKind),
					   C_SCDATA_STRUCT_OR_UNION, 0, 0, type);
}

void *CScopeLookupEnum(const char *name)
{
	return (void *)ILScopeLookup(CCurrentScope, EnumScopeName(name), 1);
}

int CScopeHasEnum(const char *name)
{
	return (ILScopeLookup(CCurrentScope, EnumScopeName(name), 0) != 0);
}

void CScopeAddEnum(const char *name, ILType *type)
{
	ILScopeDeclareItem(CCurrentScope, name, C_SCDATA_ENUM, 0, 0, type);
}

void CScopeAddTypedef(const char *name, ILType *type, ILNode *node)
{
	ILScopeDeclareItem(CCurrentScope, name,
					   C_SCDATA_TYPEDEF, PersistNode(node), 0, type);
}

void CScopeAddFunction(const char *name, ILNode *node, ILType *signature)
{
	ILScopeDeclareItem(CGlobalScope, name,
					   C_SCDATA_FUNCTION, PersistNode(node), 0, signature);
}

void CScopeAddFunctionForward(const char *name, int kind,
							  ILNode *node, ILType *signature)
{
	ILScopeDeclareItem(CGlobalScope, name, kind, PersistNode(node),
					   0, signature);
}

void CScopeAddInferredFunction(const char *name, ILType *signature)
{
	ILScopeDeclareItem(CGlobalScope, name,
					   C_SCDATA_FUNCTION_INFERRED, 0, 0, signature);
}

void CScopeUpdateFunction(void *data, int kind, ILNode *node, ILType *signature)
{
	ILScopeDataModify((ILScopeData *)data, kind, PersistNode(node),
					  0, signature);
}

void CScopeAddParam(const char *name, unsigned index, ILType *type)
{
	ILScopeDeclareItem(CGlobalScope, name,
					   C_SCDATA_PARAMETER_VAR, 0,
					   (void *)(ILNativeUInt)index, type);
}

void CScopeAddLocal(const char *name, ILNode *node,
					unsigned index, ILType *type)
{
	ILScopeDeclareItem(CCurrentScope, name,
					   C_SCDATA_LOCAL_VAR, PersistNode(node),
					   (void *)(ILNativeUInt)index, type);
}

void CScopeAddGlobal(const char *name, ILNode *node, ILType *type)
{
	ILScopeDeclareItem(CGlobalScope, name,
					   C_SCDATA_GLOBAL_VAR, PersistNode(node), 0, type);
}

void CScopeAddGlobalForward(const char *name, ILNode *node, ILType *type)
{
	ILScopeDeclareItem(CGlobalScope, name,
					   C_SCDATA_GLOBAL_VAR_FORWARD,
					   PersistNode(node), 0, type);
}

void CScopeUpdateGlobal(void *data, int kind, ILNode *node, ILType *type)
{
	ILScopeDataModify((ILScopeData *)data, kind,
					  PersistNode(node), 0, type);
}

void CScopeAddUndeclared(const char *name)
{
	ILScopeDeclareItem(CGlobalScope, name, C_SCDATA_UNDECLARED, 0, 0, 0);
}

int CScopeGetKind(void *data)
{
	return ILScopeDataGetKind((ILScopeData *)data);
}

ILType *CScopeGetType(void *data)
{
	return (ILType *)(ILScopeDataGetData2((ILScopeData *)data));
}

ILNode *CScopeGetNode(void *data)
{
	return ILScopeDataGetNode((ILScopeData *)data);
}

unsigned CScopeGetIndex(void *data)
{
	return (unsigned)(ILNativeUInt)(ILScopeDataGetData1((ILScopeData *)data));
}

#ifdef	__cplusplus
};
#endif
