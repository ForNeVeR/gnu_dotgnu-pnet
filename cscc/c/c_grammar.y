%{
/*
 * c_grammar.y - Input file for yacc that defines the syntax of the C language.
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

/* Rename the lex/yacc symbols to support multiple parsers */
#include "c_rename.h"

#include <stdio.h>
#include <cscc/c/c_internal.h>
#include "il_dumpasm.h"

#define	YYERROR_VERBOSE

/*
 * Current context.
 */
static char *functionName = "";
static ILType *currentStruct = 0;
static ILType *currentEnum = 0;
static ILInt32 currentEnumValue = 0;
static ILNode *initializers = 0;

/*
 * Imports from the lexical analyser.
 */
extern int yylex(void);
#ifdef YYTEXT_POINTER
extern char *c_text;
#else
extern char c_text[];
#endif

static void yyerror(char *msg)
{
	CCPluginParseError(msg, c_text);
}

/*
 * Fix up an identifier node that was previously detected as
 * undeclared, but we weren't sure if it was going to be used
 * as a normal identifier or a forward-referenced function name.
 */
static ILNode *FixIdentifierNode(ILNode *node, int functionRef)
{
	char *name;

	/* Bail out if the node is not an identifier */
	if(!yyisa(node, ILNode_Identifier))
	{
		return node;
	}

	/* Extract the name from the identifier node */
	name = ILQualIdentName(node, 0);

	/* Report an error if not a function reference */
	if(!functionRef)
	{
		if(functionName && functionName[0] != '\0')
		{
			CCErrorOnLine(yygetfilename(node), yygetlinenum(node),
				_("`%s' undeclared (first use in this function)"), name);
			CCErrorOnLine(yygetfilename(node), yygetlinenum(node),
				_("(Each undeclared identifier is reported only once"));
			CCErrorOnLine(yygetfilename(node), yygetlinenum(node),
				_("for each function it appears in.)"));
			CScopeAddUndeclared(name);
		}
		else
		{
			CCErrorOnLine(yygetfilename(node), yygetlinenum(node),
				_("`%s' undeclared"), name);
		}
		return ILNode_Error_create();
	}

	/* Create a forward-reference to the function, which will be
	   fixed up during semantic analysis to contain the signature */
	return ILNode_FunctionRef_create(name, ILType_Invalid);
}

/*
 * Copy the contents of one parameter declaration list to another.
 */
static void CopyParamDecls(ILNode *dest, ILNode *src)
{
	if(yyisa(src, ILNode_List))
	{
		ILNode_ListIter iter;
		ILNode *node;
		ILNode_ListIter_Init(&iter, src);
		while((node = ILNode_ListIter_Next(&iter)) != 0)
		{
			ILNode_List_Add(dest, node);
		}
	}
	else
	{
		ILNode_List_Add(dest, src);
	}
}

/*
 * Process a "struct" or "union" field.
 */
static void ProcessField(CDeclSpec spec, CDeclarator decl)
{
	ILType *type;

	/* Get the final type for the declarator */
	type = CDeclFinalize(&CCCodeGen, spec, decl, 0, 0);

	/* Define the field within the current "struct" or "union" */
	if(!CTypeDefineField(&CCCodeGen, currentStruct, decl.name, type))
	{
		CCErrorOnLine(yygetfilename(decl.node), yygetlinenum(decl.node),
					  _("storage size of `%s' is not known"), decl.name);
	}
}

/*
 * Report an invalid bit field type error.
 */
static void BitFieldInvalidType(char *name, ILNode *node)
{
	if(name)
	{
		CCErrorOnLine(yygetfilename(node), yygetlinenum(node),
					  _("bit-field `%s' has invalid type"), name);
	}
	else
	{
		CCError(_("bit-field has invalid type"));
	}
}

/*
 * Report a zero-width bit field error.
 */
static void BitFieldZeroWidth(char *name, ILNode *node)
{
	if(name)
	{
		CCErrorOnLine(yygetfilename(node), yygetlinenum(node),
					  _("zero width for bit-field `%s'"), name);
	}
	else
	{
		CCError(_("zero width for bit-field"));
	}
}

/*
 * Report bit field size warning.
 */
static void BitFieldMaxSize(char *name, ILNode *node)
{
	if(name)
	{
		CCWarningOnLine(yygetfilename(node), yygetlinenum(node),
					    _("width of `%s' exceeds its type"), name);
	}
	else
	{
		CCWarning(_("width of bit-field exceeds its type"));
	}
}

/*
 * Process a "struct" or "union" bit field.
 */
static void ProcessBitField(CDeclSpec spec, CDeclarator decl, ILUInt32 size)
{
	ILType *type;
	ILType *baseType;
	ILType *nonEnumType;
	ILUInt32 maxSize;
	int typeInvalid;

	/* Get the final type for the declarator */
	type = CDeclFinalize(&CCCodeGen, spec, decl, 0, 0);

	/* Get the base type, removing prefixes and enum wrappers */
	baseType = ILTypeStripPrefixes(type);
	nonEnumType = ILTypeGetEnumType(baseType);
	if(nonEnumType != baseType)
	{
		baseType = nonEnumType;
		if(CTypeIsConst(type))
		{
			baseType = CTypeAddConst(&CCCodeGen, baseType);
		}
		if(CTypeIsVolatile(type))
		{
			baseType = CTypeAddVolatile(&CCCodeGen, baseType);
		}
		type = baseType;
		baseType = ILTypeStripPrefixes(type);
	}

	/* Make sure that the type is integer and that the size is in range */
	typeInvalid = 0;
	if(ILType_IsPrimitive(baseType))
	{
		switch(ILType_ToElement(baseType))
		{
			case IL_META_ELEMTYPE_I1:
			case IL_META_ELEMTYPE_U1:
			{
				maxSize = 8;
			}
			break;

			case IL_META_ELEMTYPE_I2:
			case IL_META_ELEMTYPE_U2:
			case IL_META_ELEMTYPE_CHAR:
			{
				maxSize = 16;
			}
			break;

			case IL_META_ELEMTYPE_I4:
			case IL_META_ELEMTYPE_U4:
			{
				maxSize = 32;
			}
			break;

			case IL_META_ELEMTYPE_I8:
			case IL_META_ELEMTYPE_U8:
			{
				maxSize = 64;
			}
			break;

			default:
			{
				BitFieldInvalidType(decl.name, decl.node);
				type = ILType_UInt64;
				maxSize = 64;
				typeInvalid = 1;
			}
			break;
		}
	}
	else
	{
		BitFieldInvalidType(decl.name, decl.node);
		type = ILType_UInt64;
		maxSize = 64;
		typeInvalid = 1;
	}
	if(!size)
	{
		BitFieldZeroWidth(decl.name, decl.node);
		size = 1;
	}
	else if(size > maxSize)
	{
		if(!typeInvalid)
		{
			BitFieldMaxSize(decl.name, decl.node);
		}
		size = maxSize;
	}

	/* Define the bit field within the current "struct" or "union" */
	if(!CTypeDefineBitField(&CCCodeGen, currentStruct, decl.name,
							type, size, maxSize))
	{
		CCErrorOnLine(yygetfilename(decl.node), yygetlinenum(decl.node),
					  _("storage size of `%s' is not known"), decl.name);
	}
}

/*
 * Add a global initializer statement to the pending list.
 */
static void AddInitializer(char *name, ILNode *node, ILType *type, ILNode *init)
{
	ILNode *stmt;

	/* TODO: this is a hack for missing array assignment - fix later */
	if(yyisa(init, ILNode_ArrayInit))
	{
		return;
	}

	/* Build the initialization statement */
	stmt = ILNode_CGlobalVar_create(name, type, CTypeDecay(&CCCodeGen, type));
	CGenCloneLine(stmt, node);
	if(CTypeIsArray(type))
	{
		stmt = ILNode_CAssignArray_create(stmt, init);
	}
	else
	{
		stmt = ILNode_Assign_create(stmt, init);
	}
	CGenCloneLine(stmt, init);

	/* Add the statement to the pending list */
	if(!initializers)
	{
		initializers = ILNode_List_create();
	}
	ILNode_List_Add(initializers, stmt);
}

/*
 * Report a redeclaration error.
 */
static void ReportRedeclared(const char *name, ILNode *node, void *data)
{
	CCErrorOnLine(yygetfilename(node), yygetlinenum(node),
				  _("redeclaration of `%s'"), name);
	node = CScopeGetNode(data);
	if(node)
	{
		CCErrorOnLine(yygetfilename(node), yygetlinenum(node),
					  _("`%s' previously declared here"), name);
	}
}

/*
 * Process attributes that were attached to a function definition.
 */
static void ProcessFunctionAttributes(CDeclSpec spec, CDeclarator decl,
									  ILType *signature)
{
	const char *arg;
	const char *name;
	ILUInt32 flags;
	int isPrivate = ((spec.specifiers & C_SPEC_STATIC) != 0);

	/* Flush any initializers that are pending */
	if(initializers)
	{
		CFunctionFlushInits(&CCCodeGen, initializers);
		initializers = 0;
	}

	/* Check for strong and weak alias definitions */
	if((arg = CAttrGetString(decl.attrs, "alias", "__alias__")) != 0)
	{
		if(CAttrPresent(decl.attrs, "weak", "__weak__"))
		{
			CFunctionWeakAlias(&CCCodeGen, decl.name, decl.node,
							   signature, arg, isPrivate);
		}
		else
		{
			CFunctionStrongAlias(&CCCodeGen, decl.name, decl.node,
								 signature, arg, isPrivate);
		}
		return;
	}

	/* Check for PInvoke definitions */
	if((arg = CAttrGetString(decl.attrs, "pinvoke", "__pinvoke__")) != 0)
	{
		name = CAttrGetString(decl.attrs, "name", "__name__");
		if(CAttrPresent(decl.attrs, "ansi", "__ansi__"))
		{
			flags = IL_META_PINVOKE_CHAR_SET_ANSI;
		}
		else if(CAttrPresent(decl.attrs, "unicode", "__unicode__"))
		{
			flags = IL_META_PINVOKE_CHAR_SET_UNICODE;
		}
		else if(CAttrPresent(decl.attrs, "auto", "__auto__"))
		{
			flags = IL_META_PINVOKE_CHAR_SET_AUTO;
		}
		else
		{
			flags = IL_META_PINVOKE_CHAR_SET_NOT_SPEC;
		}
		if(CAttrPresent(decl.attrs, "nomangle", "__nomangle__"))
		{
			flags |= IL_META_PINVOKE_NO_MANGLE;
		}
		if(CAttrPresent(decl.attrs, "preservesig", "__preservesig__"))
		{
			flags |= IL_META_PINVOKE_OLE;
		}
		if(CAttrPresent(decl.attrs, "lasterr", "__lasterr__"))
		{
			flags |= IL_META_PINVOKE_SUPPORTS_LAST_ERROR;
		}
		if(CAttrPresent(decl.attrs, "winapi", "__winapi__"))
		{
			flags |= IL_META_PINVOKE_CALL_CONV_WINAPI;
		}
		else if(CAttrPresent(decl.attrs, "cdecl", "__cdecl__"))
		{
			flags |= IL_META_PINVOKE_CALL_CONV_CDECL;
		}
		else if(CAttrPresent(decl.attrs, "stdcall", "__stdcall__"))
		{
			flags |= IL_META_PINVOKE_CALL_CONV_STDCALL;
		}
		else if(CAttrPresent(decl.attrs, "thiscall", "__thiscall__"))
		{
			flags |= IL_META_PINVOKE_CALL_CONV_THISCALL;
		}
		else if(CAttrPresent(decl.attrs, "fastcall", "__fastcall__"))
		{
			flags |= IL_META_PINVOKE_CALL_CONV_FASTCALL;
		}
		CFunctionPInvoke(&CCCodeGen, decl.name, decl.node,
						 signature, arg, name, flags, isPrivate);
		return;
	}
}

/*
 * Process a local or global function declaration.
 */
static void ProcessFunctionDeclaration(CDeclSpec spec, CDeclarator decl,
							   		   ILNode *init, ILNode **list)
{
	ILType *signature;
	void *data;

	/* We cannot use initializers with function declarations */
	if(init != 0)
	{
		CCErrorOnLine(yygetfilename(decl.node), yygetlinenum(decl.node),
					  _("function `%s' is initialized like a variable"),
					  decl.name);
	}

	/* Build the final function signature */
	signature = CDeclFinalize(&CCCodeGen, spec, decl, 0, 0);

	/* See if there is already something registered with this name */
	data = CScopeLookup(decl.name);
	if(data)
	{
		if(CScopeGetKind(data) == C_SCDATA_FUNCTION ||
		   CScopeGetKind(data) == C_SCDATA_FUNCTION_FORWARD ||
		   CScopeGetKind(data) == C_SCDATA_FUNCTION_INFERRED)
		{
			/* The previous definition was an ANSI prototype */
			if(decl.isKR)
			{
				if(!CTypeIsIdentical
						(ILTypeGetReturnWithPrefixes(signature),
						 ILTypeGetReturnWithPrefixes
						 	(CScopeGetType(data))))
				{
					ReportRedeclared(decl.name, decl.node, data);
				}
				else if(decl.attrs)
				{
					ProcessFunctionAttributes(spec, decl, signature);
				}
			}
			else
			{
				if(!CTypeIsIdentical(signature, CScopeGetType(data)))
				{
					ReportRedeclared(decl.name, decl.node, data);
				}
				else if(decl.attrs)
				{
					ProcessFunctionAttributes(spec, decl, signature);
				}
			}
		}
		else if(CScopeGetKind(data) == C_SCDATA_FUNCTION_FORWARD_KR)
		{
			/* The previous definition was a K&R prototype */
			if(!CTypeIsIdentical
					(ILTypeGetReturnWithPrefixes(signature),
					 ILTypeGetReturnWithPrefixes
					 	(CScopeGetType(data))))
			{
				ReportRedeclared(decl.name, decl.node, data);
			}
			else if(!(decl.isKR))
			{
				/* Convert the K&R prototype into an ANSI prototype */
				CScopeUpdateFunction(decl.name, C_SCDATA_FUNCTION_FORWARD,
									 decl.node, signature);
				if(decl.attrs)
				{
					ProcessFunctionAttributes(spec, decl, signature);
				}
			}
			else if(decl.attrs)
			{
				ProcessFunctionAttributes(spec, decl, signature);
			}
		}
		else
		{
			ReportRedeclared(decl.name, decl.node, data);
		}
		return;
	}

	/* Process any function attributes that are present */
	if(decl.attrs)
	{
		ProcessFunctionAttributes(spec, decl, signature);
	}

	/* Add the function's forward definition to the global scope */
	if(decl.isKR)
	{
		CScopeAddFunctionForward(decl.name, C_SCDATA_FUNCTION_FORWARD_KR,
								 decl.node, signature);
	}
	else
	{
		CScopeAddFunctionForward(decl.name, C_SCDATA_FUNCTION_FORWARD,
								 decl.node, signature);
	}
}

/*
 * Get the size of an array initializer for a particular array type.
 */
static ILUInt32 ArrayInitializerSize(ILType *type, ILNode *init)
{
	/* Handle the string case first */
	if(yyisa(init, ILNode_CString))
	{
		return (ILUInt32)(((ILNode_CString *)init)->len) + 1;
	}

	/* If this isn't an array initializer, then bail out */
	if(!yyisa(init, ILNode_ArrayInit))
	{
		CCErrorOnLine(yygetfilename(init), yygetlinenum(init),
					  _("invalid initializer"));
		return 1;
	}

	/* TODO: handle arrays of structures that don't have nested bracketing */
	return ILNode_List_Length(((ILNode_ArrayInit *)init)->expr);
}

/*
 * Process a local or global declaration.
 */
static void ProcessDeclaration(CDeclSpec spec, CDeclarator decl,
							   ILNode *init, ILNode **list)
{
	ILType *type;
	void *data;
	unsigned index;
	ILNode *assign;
	ILType *prevType;
	ILUInt32 size;

	/* If there is a parameter list associated with the declarator, then
	   we are declaring a forward function reference, not a variable */
	if(decl.params != 0 ||
	   (spec.baseType != ILType_Invalid && CTypeIsFunction(spec.baseType)))
	{
		if((spec.specifiers & C_SPEC_TYPEDEF) == 0)
		{
			ProcessFunctionDeclaration(spec, decl, init, list);
			return;
		}
	}

	/* Finalize the type that is associated with the declaration */
	type = CDeclFinalize(&CCCodeGen, spec, decl, 0, 0);

	/* See if there is already something in this scope for the name */
	data = CScopeLookupCurrent(decl.name);

	/* Determine what kind of item to declare */
	if((spec.specifiers & C_SPEC_TYPEDEF) != 0)
	{
		/* Define a new type in the local scope */
		if(data)
		{
			ReportRedeclared(decl.name, decl.node, data);
		}
		else
		{
			CScopeAddTypedef(decl.name, type, decl.node);
		}
	}
	else if((spec.specifiers & C_SPEC_EXTERN) != 0)
	{
		/* Declare a forward reference for a global variable */
		if(data)
		{
			if(CScopeGetKind(data) == C_SCDATA_GLOBAL_VAR ||
			   CScopeGetKind(data) == C_SCDATA_GLOBAL_VAR_FORWARD)
			{
				/* We've already seen this variable - this may be a
				   redundant redeclaration */
				prevType = CScopeGetType(data);
				if(!CTypeIsIdentical(type, prevType))
				{
					ReportRedeclared(decl.name, decl.node, data);
				}
				else if(init != 0)
				{
					CCWarningOnLine(yygetfilename(decl.node),
									yygetlinenum(decl.node),
						_("`%s' initialized and declared `extern'"),
						decl.name);
				}
			}
			else
			{
				/* Already declared as a different kind of symbol */
				ReportRedeclared(decl.name, decl.node, data);
			}
			return;
		}

		/* Cannot use initializers with "extern" variables */
		if(init != 0)
		{
			CCWarningOnLine(yygetfilename(decl.node),
							yygetlinenum(decl.node),
							_("`%s' initialized and declared `extern'"),
							decl.name);
		}

		/* Declare the global variable for later */
		if(data)
		{
			CScopeUpdateGlobal(data, C_SCDATA_GLOBAL_VAR_FORWARD,
							   decl.node, type);
		}
		else
		{
			CScopeAddGlobalForward(decl.name, decl.node, type);
		}
	}
	else if(CCurrentScope == CGlobalScope ||
	        (spec.specifiers & C_SPEC_STATIC) != 0)
	{
		/* Declare a global variable */
		if(data)
		{
			if(CScopeGetKind(data) != C_SCDATA_GLOBAL_VAR_FORWARD)
			{
				ReportRedeclared(decl.name, decl.node, data);
				return;
			}
		}

		/* Extend open arrays to the same size as initializers */
		if(CTypeIsOpenArray(type))
		{
			if(init != 0)
			{
				/* Infer the initializer size from the expression */
				size = ArrayInitializerSize(type, init);
			}
			else
			{
				/* If there is no initializer, then change to one element */
				CCWarningOnLine(yygetfilename(decl.node),
								yygetlinenum(decl.node),
								_("array `%s' assumed to have one element"),
								decl.name);
				size = 1;
			}
			type = CTypeCreateArray(&CCCodeGen, CTypeGetElemType(type), size);
		}

		/* Make sure that the type size is fixed or dynamic */
		if(CTypeSizeAndAlign(type, 0) == CTYPE_UNKNOWN)
		{
			CCErrorOnLine(yygetfilename(decl.node), yygetlinenum(decl.node),
						  _("storage size of `%s' is not known"), decl.name);
		}

		/* Mark the type for output */
		CTypeMarkForOutput(&CCCodeGen, type);

		/* Declare the global variable in the assembly output stream */
		if(CCCodeGen.asmOutput)
		{
			if((spec.specifiers & C_SPEC_STATIC) != 0)
			{
				fputs(".field private static ", CCCodeGen.asmOutput);
			}
			else
			{
				fputs(".field public static ", CCCodeGen.asmOutput);
			}
			ILDumpType(CCCodeGen.asmOutput, CCCodeGen.image, type,
					   IL_DUMP_QUOTE_NAMES);
			putc(' ', CCCodeGen.asmOutput);
			ILDumpIdentifier(CCCodeGen.asmOutput, decl.name, 0,
							 IL_DUMP_QUOTE_NAMES);
			putc('\n', CCCodeGen.asmOutput);
		}

		/* Add the global variable to the current scope */
		if(data)
		{
			CScopeUpdateGlobal(data, C_SCDATA_GLOBAL_VAR, decl.node, type);
		}
		else
		{
			CScopeAddGlobal(decl.name, decl.node, type);
		}

		/* Process the initializer */
		if(init)
		{
			AddInitializer(decl.name, decl.node, type, init);
		}
	}
	else
	{
		/* Declare a local variable in the current scope */
		if(data)
		{
			ReportRedeclared(decl.name, decl.node, data);
		}
		else
		{
			/* Extend open arrays to the same size as initializers */
			if(CTypeIsOpenArray(type))
			{
				if(init != 0)
				{
					/* Infer the initializer size from the expression */
					size = ArrayInitializerSize(type, init);
				}
				else
				{
					/* If there is no initializer, then change to one element */
					CCWarningOnLine(yygetfilename(decl.node),
									yygetlinenum(decl.node),
									_("array `%s' assumed to have one element"),
									decl.name);
					size = 1;
				}
				type = CTypeCreateArray
					(&CCCodeGen, CTypeGetElemType(type), size);
			}

			/* Allocate the global variable */
			index = CGenAllocLocal(&CCCodeGen, type);
			CScopeAddLocal(decl.name, decl.node, index, type);
			if(init)
			{
				if(!(*list))
				{
					*list = ILNode_List_create();
				}
				assign = ILNode_CLocalVar_create
					(index, ILTypeToMachineType(type), type,
					 CTypeDecay(&CCCodeGen, type));
				CGenCloneLine(assign, decl.node);
				if(CTypeIsArray(type))
				{
					assign = ILNode_CAssignArray_create(assign, init);
				}
				else
				{
					assign = ILNode_Assign_create(assign, init);
				}
				CGenCloneLine(assign, init);
				ILNode_List_Add(*list, assign);
			}
		}
	}
}

/*
 * Evaluate a constant expression to an "unsigned int" value.
 * Used for array and bit field sizes.
 */
static ILUInt32 EvaluateSize(ILNode *expr)
{
	CSemValue value;
	ILEvalValue *evalValue;
	ILUInt32 size;

	/* Perform inline semantic analysis on the expression */
	value = CSemInlineAnalysis(&CCCodeGen, expr, CCurrentScope);
	if(!CSemIsConstant(value))
	{
		if(!CSemIsError(value))
		{
			CCErrorOnLine(yygetfilename(expr), yygetlinenum(expr),
						  _("constant value required"));
		}
		return 1;
	}

	/* Convert the constant value into an "unsigned int" value */
	evalValue = CSemGetConstant(value);
	if(!evalValue)
	{
		CCErrorOnLine(yygetfilename(expr), yygetlinenum(expr),
					  _("compile-time constant value required"));
		return 1;
	}
	size = 1;
	switch(evalValue->valueType)
	{
		case ILMachineType_Int8:
		case ILMachineType_UInt8:
		case ILMachineType_Int16:
		case ILMachineType_UInt16:
		case ILMachineType_Char:
		case ILMachineType_Int32:
		case ILMachineType_NativeInt:
		{
			if(evalValue->un.i4Value >= 0)
			{
				size = (ILUInt32)(evalValue->un.i4Value);
			}
			else
			{
				CCErrorOnLine(yygetfilename(expr), yygetlinenum(expr),
							  _("size value is negative"));
			}
		}
		break;

		case ILMachineType_UInt32:
		case ILMachineType_NativeUInt:
		{
			size = (ILUInt32)(evalValue->un.i4Value);
		}
		break;

		case ILMachineType_Int64:
		{
			if(evalValue->un.i8Value > (ILInt64)IL_MAX_UINT32)
			{
				CCErrorOnLine(yygetfilename(expr), yygetlinenum(expr),
							  _("size value is too large"));
			}
			else if(evalValue->un.i8Value >= 0)
			{
				size = (ILUInt32)(evalValue->un.i8Value);
			}
			else
			{
				CCErrorOnLine(yygetfilename(expr), yygetlinenum(expr),
							  _("size value is negative"));
			}
		}
		break;

		case ILMachineType_UInt64:
		{
			if(((ILUInt64)(evalValue->un.i8Value)) > (ILUInt64)IL_MAX_UINT32)
			{
				CCErrorOnLine(yygetfilename(expr), yygetlinenum(expr),
							  _("size value is too large"));
			}
			else
			{
				size = (ILUInt32)(evalValue->un.i8Value);
			}
		}
		break;

		default:
		{
			CCErrorOnLine(yygetfilename(expr), yygetlinenum(expr),
						  _("size value has non-integer type"));
		}
		break;
	}

	/* Return the computed size to the caller */
	return size;
}

/*
 * Evaluate a constant expression to an "int" value.
 * Used for enumerated constant values.
 */
static ILInt32 EvaluateIntConstant(ILNode *expr)
{
	CSemValue value;
	ILEvalValue *evalValue;

	/* Perform inline semantic analysis on the expression */
	value = CSemInlineAnalysis(&CCCodeGen, expr, CCurrentScope);
	if(!CSemIsConstant(value))
	{
		if(!CSemIsError(value))
		{
			CCErrorOnLine(yygetfilename(expr), yygetlinenum(expr),
						  _("constant value required"));
		}
		return 0;
	}

	/* Convert the constant value into an "int" value */
	evalValue = CSemGetConstant(value);
	if(!evalValue || !ILGenCastConst(&CCCodeGen, evalValue,
									 evalValue->valueType,
									 ILMachineType_Int32))
	{
		CCErrorOnLine(yygetfilename(expr), yygetlinenum(expr),
					  _("compile-time constant value required"));
		return 0;
	}
	return evalValue->un.i4Value;
}

%}

/*
 * Define the structure of yylval.
 */
%union {

	CLexIntConst		integer;
	CLexFloatConst		real;
	ILIntString			string;
	char	           *name;
	ILNode             *node;
	ILType			   *type;
	CDeclSpec			declSpec;
	CDeclarator			decl;
	int					kind;
	struct {
		ILType		   *type;
		ILType		   *parent;
	}					structInfo;
	ILMethod           *methodInfo;

}

/*
 * Basic lexical values.
 */
%token IDENTIFIER		"an identifier"
%token INTEGER_CONSTANT	"an integer value"
%token FLOAT_CONSTANT	"a floating point value"
%token IMAG_CONSTANT	"an imaginary value"
%token STRING_LITERAL	"a string literal"
%token TYPE_NAME		"a type identifier"

/*
 * Operators.
 */
%token PTR_OP			"`->'"
%token INC_OP			"`++'"
%token DEC_OP			"`--'"
%token LEFT_OP			"`<<'"
%token RIGHT_OP			"`>>'"
%token LE_OP			"`<='"
%token GE_OP			"`>='"
%token EQ_OP			"`=='"
%token NE_OP			"`!='"
%token AND_OP			"`&&'"
%token OR_OP			"`||'"
%token MUL_ASSIGN_OP	"`*='"
%token DIV_ASSIGN_OP	"`/='"
%token MOD_ASSIGN_OP	"`%='"
%token ADD_ASSIGN_OP	"`+='"
%token SUB_ASSIGN_OP	"`-='"
%token LEFT_ASSIGN_OP	"`<<='"
%token RIGHT_ASSIGN_OP	"`>>='"
%token AND_ASSIGN_OP	"`&='"
%token XOR_ASSIGN_OP	"`^='"
%token OR_ASSIGN_OP		"`|='"

/*
 * Reserved words.
 */
%token K_ASM			"`asm'"
%token K_AUTO			"`auto'"
%token K_BREAK			"`break'"
%token K_CASE			"`case'"
%token K_CHAR			"`char'"
%token K_CONST			"`const'"
%token K_CONTINUE		"`continue'"
%token K_DEFAULT		"`default'"
%token K_DO				"`do'"
%token K_DOUBLE			"`double'"
%token K_ELSE			"`else'"
%token K_ENUM			"`enum'"
%token K_EXTERN			"`extern'"
%token K_FLOAT			"`float'"
%token K_FOR			"`for'"
%token K_GOTO			"`goto'"
%token K_IF				"`if'"
%token K_INLINE			"`inline'"
%token K_INT			"`int'"
%token K_LONG			"`long'"
%token K_REGISTER		"`register'"
%token K_RETURN			"`return'"
%token K_SHORT			"`short'"
%token K_SIGNED			"`signed'"
%token K_SIZEOF			"`sizeof'"
%token K_STATIC			"`static'"
%token K_STRUCT			"`struct'"
%token K_SWITCH			"`switch'"
%token K_TYPEDEF		"`typedef'"
%token K_TYPEOF			"`typeof'"
%token K_UNION			"`union'"
%token K_UNSIGNED		"`unsigned'"
%token K_VOID			"`void'"
%token K_VOLATILE		"`volatile'"
%token K_WHILE			"`while'"
%token K_ELIPSIS		"`...'"
%token K_VA_LIST		"`__builtin_va_list'"
%token K_VA_START		"`__builtin_va_start'"
%token K_VA_ARG			"`__builtin_va_arg'"
%token K_VA_END			"`__builtin_va_end'"
%token K_SETJMP			"`__builtin_setjmp'"
%token K_ALLOCA			"`__builtin_alloca'"
%token K_ATTRIBUTE		"`__attribute__'"
%token K_BOOL			"`_Bool'"
%token K_WCHAR			"`__wchar__'"
%token K_NATIVE			"`__native__'"
%token K_CSHARP			"`__csharp__'"
%token K_INVOKE			"`__invoke__'"
%token K_FUNCTION		"`__FUNCTION__'"
%token K_FUNC			"`__func__'"

/*
 * Define the yylval types of the various non-terminals.
 */
%type <name>		IDENTIFIER TYPE_NAME
%type <integer>		INTEGER_CONSTANT
%type <real>		FLOAT_CONSTANT IMAG_CONSTANT
%type <string>		STRING_LITERAL StringLiteral

%type <name>		AnyIdentifier Identifier

%type <node>		PrimaryExpression PostfixExpression
%type <node>		ArgumentExpressionList UnaryExpression CastExpression
%type <node>		MultiplicativeExpression AdditiveExpression ShiftExpression
%type <node>		RelationalExpression EqualityExpression AndExpression
%type <node>		XorExpression OrExpression LogicalAndExpression
%type <node>		LogicalOrExpression ConditionalExpression
%type <node>		AssignmentExpression Expression ConstantExpression
%type <node>		BoolExpression ConstantAttributeExpression
%type <node>		AttributeArgs

%type <node>		Statement Statement2 LabeledStatement CompoundStatement
%type <node>		OptDeclarationList DeclarationList Declaration
%type <node>		OptStatementList StatementList ExpressionStatement
%type <node>		SelectionStatement IterationStatement JumpStatement
%type <node>		AsmStatement

%type <node>		ParameterIdentifierList IdentifierList ParameterList
%type <node>		ParameterTypeList ParameterDeclaration ParamDeclaration
%type <node>		OptParamDeclarationList ParamDeclarationList
%type <node>		ParamDeclaratorList /*ParamDeclaration*/

%type <node>		StructDeclaratorList StructDeclarator InitDeclarator
%type <node>		InitDeclaratorList Initializer InitializerList

%type <type>		CSharpSpecifier CSharpType CSharpBuiltinType
%type <node>		QualifiedIdentifier
%type <kind>		DimensionSeparators DimensionSeparatorList

%type <node>		FunctionBody Attributes AttributeList Attribute

%type <type>		TypeName StructOrUnionSpecifier EnumSpecifier

%type <declSpec>	StorageClassSpecifier TypeSpecifier DeclarationSpecifiers
%type <declSpec>	TypeSpecifierList

%type <decl>		Declarator Declarator2 Pointer AbstractDeclarator
%type <decl>		AbstractDeclarator2 ParamDeclarator /*ParamDeclarator*/

%type <kind>		StructOrUnion TypeQualifierList TypeQualifier

%expect 1

%start File
%%

AnyIdentifier
	: IDENTIFIER
	| TYPE_NAME
	;

Identifier
	: IDENTIFIER
	;

PrimaryExpression
	: Identifier			{
				/* Resolve the identifier to a node */
				void *data = CScopeLookup($1);
				ILType *type, *decayedType;
				if(!data)
				{
					/* We don't know what this identifier is, so return
					   a generic node until the next higher level can
					   make a determination as to whether the name is truly
					   undeclared or a forward reference to a function */
					$$ = ILNode_Identifier_create($1);
				}
				else
				{
					/* Determine the kind of node to create */
					switch(CScopeGetKind(data))
					{
						case C_SCDATA_LOCAL_VAR:
						{
							/* Create a local variable reference */
							type = CScopeGetType(data);
							decayedType = CTypeDecay(&CCCodeGen, type);
							$$ = ILNode_CLocalVar_create
								(CScopeGetIndex(data),
								 ILTypeToMachineType(decayedType), type,
								 decayedType);
						}
						break;

						case C_SCDATA_PARAMETER_VAR:
						{
							/* Create a parameter variable reference */
							type = CScopeGetType(data);
							$$ = ILNode_CArgumentVar_create
								(CScopeGetIndex(data),
								 ILTypeToMachineType(type), type);
						}
						break;

						case C_SCDATA_GLOBAL_VAR:
						case C_SCDATA_GLOBAL_VAR_FORWARD:
						{
							/* Create a global variable reference */
							type = CScopeGetType(data);
							decayedType = CTypeDecay(&CCCodeGen, type);
							$$ = ILNode_CGlobalVar_create
								($1, type, decayedType);
						}
						break;

						case C_SCDATA_FUNCTION:
						case C_SCDATA_FUNCTION_FORWARD:
						case C_SCDATA_FUNCTION_FORWARD_KR:
						case C_SCDATA_FUNCTION_INFERRED:
						{
							$$ = ILNode_FunctionRef_create
									($1, CScopeGetType(data));
						}
						break;

						case C_SCDATA_ENUM_CONSTANT:
						{
							ILInt32 value = CScopeGetEnumConst(data);
							if(value >= 0)
							{
								$$ = ILNode_Int32_create
									((ILUInt64)value, 0, 1);
							}
							else
							{
								$$ = ILNode_Int32_create
									((ILUInt64)(ILUInt32)(-value), 1, 1);
							}
						}
						break;

						default:
						{
							/* Previously detected an undeclared identifier */
							$$ = ILNode_Error_create();
						}
						break;
					}
				}
			}
	| INTEGER_CONSTANT		{
				/* Convert the integer value into a node */
				switch($1.type)
				{
					case ILMachineType_Int8:
					{
						$$ = ILNode_Int8_create($1.value, $1.isneg, 1);
					}
					break;

					case ILMachineType_UInt8:
					{
						$$ = ILNode_UInt8_create($1.value, $1.isneg, 1);
					}
					break;

					case ILMachineType_Int16:
					{
						$$ = ILNode_Int16_create($1.value, $1.isneg, 1);
					}
					break;

					case ILMachineType_UInt16:
					{
						$$ = ILNode_UInt16_create($1.value, $1.isneg, 1);
					}
					break;

					case ILMachineType_Int32:
					{
						$$ = ILNode_Int32_create($1.value, $1.isneg, 1);
					}
					break;

					case ILMachineType_UInt32:
					{
						$$ = ILNode_UInt32_create($1.value, $1.isneg, 1);
					}
					break;

					case ILMachineType_Int64:
					{
						$$ = ILNode_Int64_create($1.value, $1.isneg, 1);
					}
					break;

					case ILMachineType_UInt64:
					{
						$$ = ILNode_UInt64_create($1.value, $1.isneg, 1);
					}
					break;

					default:
					{
						/* Shouldn't happen */
						$$ = ILNode_Error_create();
					}
					break;
				}
			}
	| FLOAT_CONSTANT		{
				/* Convert the floating-point value into a node */
				if($1.type == ILMachineType_Float32)
				{
					$$ = ILNode_Float32_create($1.value);
				}
				else
				{
					$$ = ILNode_Float64_create($1.value);
				}
			}
	| IMAG_CONSTANT		{
				/* Convert the imaginary floating-point value into a node */
				/* TODO: true imaginary constants */
				if($1.type == ILMachineType_Float32)
				{
					$$ = ILNode_Float32_create($1.value);
				}
				else
				{
					$$ = ILNode_Float64_create($1.value);
				}
			}
	| StringLiteral			{
				$$ = ILNode_CString_create($1.string, $1.len);
			}
	| K_FUNC				{
				/* Create a reference to the local "__func__" array.
				   In this implementation, we create a reference to
				   the string pool.  The pool implementation will take
				   care of pointing all copies of the function name
				   at the same memory location */
				ILIntString str = ILInternString(functionName, -1);
				$$ = ILNode_CString_create(str.string, str.len);
			}
	| '(' Expression ')'		{ $$ = $2; }
	| '(' CompoundStatement ')'	{ $$ = $2; }
	;

StringLiteral
	: STRING_LITERAL	{ $$ = $1; }
	| K_FUNCTION		{
				$$ = ILInternString(functionName, strlen(functionName));
			}
	| StringLiteral STRING_LITERAL	{
				$$ = ILInternAppendedString($1, $2);
			}
	| StringLiteral K_FUNCTION		{
				$$ = ILInternAppendedString
					($1, ILInternString(functionName, strlen(functionName)));
			}
	;

PostfixExpression
	: PrimaryExpression
	| PostfixExpression '[' Expression ']'	{
				$$ = ILNode_CArrayAccess_create(FixIdentifierNode($1, 0), $3);
			}
	| PostfixExpression '(' ')'	{
				$$ = ILNode_CInvocationExpression_create
						(FixIdentifierNode($1, 1), 0);
			}
	| PostfixExpression '(' ArgumentExpressionList ')'	{
				$$ = ILNode_CInvocationExpression_create
						(FixIdentifierNode($1, 1), $3);
			}
	| PostfixExpression '.' AnyIdentifier	{
				$$ = ILNode_CMemberField_create
						(FixIdentifierNode($1, 0), $3);
			}
	| K_INVOKE TYPE_NAME '.' AnyIdentifier '(' ')'	{
				$$ = ILNode_CSharpInvocation_create
						(CScopeGetType(CScopeLookup($2)), $4, 0);
			}
	| K_INVOKE TYPE_NAME '.' AnyIdentifier '(' ArgumentExpressionList ')'	{
				$$ = ILNode_CSharpInvocation_create
						(CScopeGetType(CScopeLookup($2)), $4, $6);
			}
	| PostfixExpression PTR_OP AnyIdentifier	{
				$$ = ILNode_CDerefField_create
						(FixIdentifierNode($1, 0), $3);
			}
	| PostfixExpression INC_OP		{
				$$ = ILNode_PostInc_create(FixIdentifierNode($1, 0));
			}
	| PostfixExpression DEC_OP		{
				$$ = ILNode_PostDec_create(FixIdentifierNode($1, 0));
			}
	| K_VA_ARG '(' UnaryExpression ',' TypeName ')'	{
				$$ = ILNode_VaArg_create($3, $5);
			}
	| K_VA_START '(' UnaryExpression ',' Identifier ')'	{
				$$ = ILNode_VaStart_create($3, $5);
			}
	| K_VA_START '(' UnaryExpression ')'	{
				$$ = ILNode_VaStart_create($3, 0);
			}
	| K_VA_END '(' UnaryExpression ')'	{
				$$ = ILNode_VaEnd_create($3);
			}
	| K_SETJMP '(' UnaryExpression ')'	{
				$$ = ILNode_SetJmp_create($3);
			}
	| K_ALLOCA '(' AssignmentExpression ')'	{
				$$ = ILNode_AllocA_create($3);
			}
	;

ArgumentExpressionList
	: AssignmentExpression
	| ArgumentExpressionList ',' AssignmentExpression	{
				$$ = ILNode_ArgList_create($1, $3);
			}
	;

UnaryExpression
	: PostfixExpression			{ $$ = FixIdentifierNode($1, 0); }
	| INC_OP UnaryExpression	{ $$ = ILNode_PreInc_create($2); }
	| DEC_OP UnaryExpression	{ $$ = ILNode_PreDec_create($2); }
	| '-' CastExpression		{
				/* Negate simple integer and floating point values in-place */
				$$ = ILNode_Neg_create($2);
			}
	| '+' CastExpression		{ $$ = ILNode_UnaryPlus_create($2); }
	| '~' CastExpression		{ $$ = ILNode_Not_create($2); }
	| '!' CastExpression		{
			$$ = ILNode_LogicalNot_create(ILNode_ToBool_create($2));
		}
	| '&' CastExpression		{ $$ = ILNode_AddressOf_create($2); }
	| '*' CastExpression		{ $$ = ILNode_CDeref_create($2); }
	| K_SIZEOF UnaryExpression	{ $$ = ILNode_SizeOfExpr_create($2); }
	| K_SIZEOF '(' TypeName ')'	{ $$ = ILNode_SizeOfType_create($3); }
	;

CastExpression
	: UnaryExpression
	| '(' TypeName ')' CastExpression	{
				$$ = ILNode_CastType_create($4, $2);
			}
	;

MultiplicativeExpression
	: CastExpression
	| MultiplicativeExpression '*' CastExpression	{
				$$ = ILNode_Mul_create($1, $3);
			}
	| MultiplicativeExpression '/' CastExpression	{
				$$ = ILNode_Div_create($1, $3);
			}
	| MultiplicativeExpression '%' CastExpression	{
				$$ = ILNode_Rem_create($1, $3);
			}
	;

AdditiveExpression
	: MultiplicativeExpression
	| AdditiveExpression '+' MultiplicativeExpression	{
				$$ = ILNode_Add_create($1, $3);
			}
	| AdditiveExpression '-' MultiplicativeExpression	{
				$$ = ILNode_Sub_create($1, $3);
			}
	;

ShiftExpression
	: AdditiveExpression
	| ShiftExpression LEFT_OP AdditiveExpression	{
				$$ = ILNode_Shl_create($1, $3);
			}
	| ShiftExpression RIGHT_OP AdditiveExpression	{
				$$ = ILNode_Shr_create($1, $3);
			}
	;

RelationalExpression
	: ShiftExpression
	| RelationalExpression '<' ShiftExpression	{
				$$ = ILNode_Lt_create($1, $3);
			}
	| RelationalExpression '>' ShiftExpression	{
				$$ = ILNode_Gt_create($1, $3);
			}
	| RelationalExpression LE_OP ShiftExpression	{
				$$ = ILNode_Le_create($1, $3);
			}
	| RelationalExpression GE_OP ShiftExpression	{
				$$ = ILNode_Ge_create($1, $3);
			}
	;

EqualityExpression
	: RelationalExpression
	| EqualityExpression EQ_OP RelationalExpression	{
				$$ = ILNode_Eq_create($1, $3);
			}
	| EqualityExpression NE_OP RelationalExpression	{
				$$ = ILNode_Ne_create($1, $3);
			}
	;

AndExpression
	: EqualityExpression
	| AndExpression '&' EqualityExpression	{
				$$ = ILNode_And_create($1, $3);
			}
	;

XorExpression
	: AndExpression
	| XorExpression '^' AndExpression	{
				$$ = ILNode_Xor_create($1, $3);
			}
	;

OrExpression
	: XorExpression
	| OrExpression '|' XorExpression	{
				$$ = ILNode_Or_create($1, $3);
			}
	;

LogicalAndExpression
	: OrExpression
	| LogicalAndExpression AND_OP OrExpression	{
				$$ = ILNode_LogicalAnd_create(ILNode_ToBool_create($1),
											  ILNode_ToBool_create($3));
			}
	;

LogicalOrExpression
	: LogicalAndExpression
	| LogicalOrExpression OR_OP LogicalAndExpression	{
				$$ = ILNode_LogicalOr_create(ILNode_ToBool_create($1),
											 ILNode_ToBool_create($3));
			}
	;

ConditionalExpression
	: LogicalOrExpression
	| LogicalOrExpression '?' LogicalOrExpression ':' ConditionalExpression	{
				$$ = ILNode_Conditional_create
						(ILNode_ToBool_create($1), $3, $5);
			}
	;

AssignmentExpression
	: ConditionalExpression
	| UnaryExpression '=' AssignmentExpression	{
				$$ = ILNode_Assign_create($1, $3);
			}
	| UnaryExpression MUL_ASSIGN_OP AssignmentExpression	{
				$$ = ILNode_AssignMul_create(ILNode_Mul_create($1, $3));
			}
	| UnaryExpression DIV_ASSIGN_OP AssignmentExpression	{
				$$ = ILNode_AssignDiv_create(ILNode_Div_create($1, $3));
			}
	| UnaryExpression MOD_ASSIGN_OP AssignmentExpression	{
				$$ = ILNode_AssignRem_create(ILNode_Rem_create($1, $3));
			}
	| UnaryExpression ADD_ASSIGN_OP AssignmentExpression	{
				$$ = ILNode_AssignAdd_create(ILNode_Add_create($1, $3));
			}
	| UnaryExpression SUB_ASSIGN_OP AssignmentExpression	{
				$$ = ILNode_AssignSub_create(ILNode_Sub_create($1, $3));
			}
	| UnaryExpression LEFT_ASSIGN_OP AssignmentExpression	{
				$$ = ILNode_AssignShl_create(ILNode_Shl_create($1, $3));
			}
	| UnaryExpression RIGHT_ASSIGN_OP AssignmentExpression	{
				$$ = ILNode_AssignShr_create(ILNode_Shr_create($1, $3));
			}
	| UnaryExpression AND_ASSIGN_OP AssignmentExpression	{
				$$ = ILNode_AssignAnd_create(ILNode_And_create($1, $3));
			}
	| UnaryExpression XOR_ASSIGN_OP AssignmentExpression	{
				$$ = ILNode_AssignXor_create(ILNode_Xor_create($1, $3));
			}
	| UnaryExpression OR_ASSIGN_OP AssignmentExpression	{
				$$ = ILNode_AssignOr_create(ILNode_Or_create($1, $3));
			}
	;

Expression
	: AssignmentExpression
	| Expression ',' AssignmentExpression	{
				$$ = ILNode_Comma_create($1, $3);
			}
	;

ConstantExpression
	: ConditionalExpression	{
				$$ = ILNode_ToConst_create($1);
			}
	;

Declaration
	: DeclarationSpecifiers ';'		{
				CDeclSpec spec;

				/* Finalize the declaration specifier */
				if(CCurrentScope == CGlobalScope)
				{
					spec = CDeclSpecFinalize($1, 0, 0, C_KIND_GLOBAL_NAME);
				}
				else
				{
					spec = CDeclSpecFinalize($1, 0, 0, C_KIND_LOCAL_NAME);
				}

				/* Check for useless declarations */
				if((!CTypeIsStruct(spec.baseType) &&
				    !CTypeIsUnion(spec.baseType) &&
					!CTypeIsEnum(spec.baseType)) ||
				    (spec.specifiers & C_SPEC_STORAGE) != 0)
				{
					CCWarning(_("useless keyword or type name "
								"in empty declaration"));
				}

				/* This declaration's code is the empty statement */
				$$ = ILNode_Empty_create();
			}
	| DeclarationSpecifiers InitDeclaratorList ';'	{
				CDeclSpec spec;
				ILNode_CDeclarator *decl;
				ILNode_ListIter iter;
				ILNode *list;

				/* Find the first declarator, for error reporting */
				if(!yyisa($2, ILNode_List))
				{
					decl = (ILNode_CDeclarator *)($2);
				}
				else
				{
					ILNode_ListIter_Init(&iter, $2);
					decl = (ILNode_CDeclarator *)(ILNode_ListIter_Next(&iter));
				}

				/* Finalize the declaration specifier */
				if(CCurrentScope == CGlobalScope)
				{
					spec = CDeclSpecFinalize
						($1, decl->decl.node, decl->decl.name,
						 C_KIND_GLOBAL_NAME);
				}
				else
				{
					spec = CDeclSpecFinalize
						($1, decl->decl.node, decl->decl.name,
						 C_KIND_LOCAL_NAME);
				}

				/* Process the declarations in the list */
				list = 0;
				if(yyisa($2, ILNode_CInitDeclarator))
				{
					ProcessDeclaration(spec, decl->decl,
						   ((ILNode_CInitDeclarator *)decl)->init, &list);
				}
				else if(yyisa($2, ILNode_CDeclarator))
				{
					ProcessDeclaration(spec, decl->decl, 0, &list);
				}
				else
				{
					ILNode_ListIter_Init(&iter, $2);
					while((decl = (ILNode_CDeclarator *)
							ILNode_ListIter_Next(&iter)) != 0)
					{
						if(yyisa(decl, ILNode_CInitDeclarator))
						{
							ProcessDeclaration(spec, decl->decl,
							   ((ILNode_CInitDeclarator *)decl)->init, &list);
						}
						else
						{
							ProcessDeclaration(spec, decl->decl, 0, &list);
						}
					}
				}
				if(!list)
				{
					list = ILNode_Empty_create();
				}
				$$ = list;
			}
	;

DeclarationSpecifiers
	: StorageClassSpecifier
	| StorageClassSpecifier DeclarationSpecifiers	{
				$$ = CDeclSpecCombine($1, $2);
			}
	| TypeSpecifier
	| TypeSpecifier DeclarationSpecifiers	{
				$$ = CDeclSpecCombine($1, $2);
			}
	;

InitDeclaratorList
	: InitDeclarator
	| InitDeclaratorList ',' InitDeclarator	{
				if(yyisa($1, ILNode_List))
				{
					ILNode_List_Add($1, $3);
					$$ = $1;
				}
				else
				{
					$$ = ILNode_List_create();
					ILNode_List_Add($$, $1);
					ILNode_List_Add($$, $3);
				}
			}
	;

InitDeclarator
	: Declarator	{
				$$ = ILNode_CDeclarator_create($1);
			}
	| Declarator '=' Initializer	{
				$$ = ILNode_CInitDeclarator_create($1, $3);
			}
	;

StorageClassSpecifier
	: K_TYPEDEF			{ CDeclSpecSet($$, C_SPEC_TYPEDEF); }
	| K_EXTERN			{ CDeclSpecSet($$, C_SPEC_EXTERN); }
	| K_STATIC			{ CDeclSpecSet($$, C_SPEC_STATIC); }
	| K_AUTO			{ CDeclSpecSet($$, C_SPEC_AUTO); }
	| K_REGISTER		{ CDeclSpecSet($$, C_SPEC_REGISTER); }
	| K_INLINE			{ CDeclSpecSet($$, C_SPEC_INLINE); }
	;

TypeSpecifier
	: K_CHAR			{ CDeclSpecSetType($$, ILType_Int8); }
	| K_SHORT			{ CDeclSpecSet($$, C_SPEC_SHORT); }
	| K_INT				{ CDeclSpecSetType($$, ILType_Int32); }
	| K_LONG			{ CDeclSpecSet($$, C_SPEC_LONG); }
	| K_SIGNED			{ CDeclSpecSet($$, C_SPEC_SIGNED); }
	| K_UNSIGNED		{ CDeclSpecSet($$, C_SPEC_UNSIGNED); }
	| K_NATIVE			{ CDeclSpecSet($$, C_SPEC_NATIVE); }
	| K_FLOAT			{ CDeclSpecSetType($$, ILType_Float32); }
	| K_DOUBLE			{ CDeclSpecSetType($$, ILType_Float64); }
	| K_CONST			{ CDeclSpecSet($$, C_SPEC_CONST); }
	| K_VOLATILE		{ CDeclSpecSet($$, C_SPEC_VOLATILE); }
	| K_VOID			{ CDeclSpecSetType($$, ILType_Void); }
	| K_BOOL			{ CDeclSpecSetType($$, ILType_Boolean); }
	| K_WCHAR			{ CDeclSpecSetType($$, ILType_Char); }
	| K_VA_LIST			{ CDeclSpecSetType($$, CTypeCreateVaList(&CCCodeGen)); }
	| StructOrUnionSpecifier		{ CDeclSpecSetType($$, $1); }
	| EnumSpecifier					{ CDeclSpecSetType($$, $1); }
	| K_TYPEOF '(' Expression ')'	{
				/* Perform inline semantic analysis on the expression */
				CSemValue value = CSemInlineAnalysis
						(&CCCodeGen, $3, CCurrentScope);

				/* Use the type of the expression as our return value */
				CDeclSpecSetType($$, CSemGetType(value));
			}
	| K_TYPEOF '(' TypeName ')'		{ CDeclSpecSetType($$, $3); }
	| TYPE_NAME						{
				/* Look up the type in the current scope.  We know that
				   the typedef is present, because the lexer found it */
				ILType *type = CScopeGetType(CScopeLookup($1));
				CDeclSpecSetType($$, type);
			}
	| CSharpSpecifier	{ CDeclSpecSetType($$, $1); }
	;

StructOrUnionSpecifier
	: StructOrUnion AnyIdentifier '{'	{
				/* Look for a definition in the local scope */
				if(!CScopeHasStructOrUnion($2, $1))
				{
					/* Define the struct or union type */
					$<structInfo>$.type = CTypeDefineStructOrUnion
							(&CCCodeGen, $2, $1, functionName);
				}
				else
				{
					/* We've already seen a definition for this type before */
					if($1 == C_STKIND_STRUCT)
					{
						CCError(_("redefinition of `struct %s'"), $2);
					}
					else
					{
						CCError(_("redefinition of `union %s'"), $2);
					}

					/* Create an anonymous type as a place-holder */
					$<structInfo>$.type = CTypeDefineAnonStructOrUnion
						(&CCCodeGen, currentStruct, functionName, $1);
				}

				/* Push in a scope level for the structure */
				$<structInfo>$.parent = currentStruct;
				currentStruct = $<structInfo>$.type;

				/* Add the type to the current scope */
				CScopeAddStructOrUnion($2, $1, $<structInfo>$.type);
			}
	  StructDeclarationList '}'	{
	  			/* Pop the structure scope */
				currentStruct = $<structInfo>4.parent;

				/* Terminate the structure definition */
				$$ = CTypeEndStruct(&CCCodeGen, $<structInfo>4.type, 0);
	  		}
	| StructOrUnion '{' 	{
				/* Define an anonymous struct or union type */
				$<structInfo>$.type = CTypeDefineAnonStructOrUnion
					(&CCCodeGen, currentStruct, functionName, $1);

				/* Push in a scope level for the structure */
				$<structInfo>$.parent = currentStruct;
				currentStruct = $<structInfo>$.type;
			}
	  StructDeclarationList '}'		{
	  			/* Pop the structure scope */
				currentStruct = $<structInfo>3.parent;

				/* Terminate the structure definition */
				$$ = CTypeEndStruct(&CCCodeGen, $<structInfo>3.type, 1);
	  		}
	| StructOrUnion AnyIdentifier	{
				/* Look for an existing definition for this type */
				void *data = CScopeLookupStructOrUnion($2, $1);
				if(!data)
				{
					/* Create a reference to the named struct or union type,
					   assuming that it is at the global level */
					$$ = CTypeCreateStructOrUnion
							(&CCCodeGen, $2, $1, 0);
				}
				else
				{
					/* Use the type that was registered in the scope */
					$$ = CScopeGetType(data);
				}
			}
	;

StructOrUnion
	: K_STRUCT					{ $$ = C_STKIND_STRUCT; }
	| K_STRUCT K_NATIVE			{ $$ = C_STKIND_STRUCT_NATIVE; }
	| K_UNION					{ $$ = C_STKIND_UNION; }
	| K_UNION K_NATIVE			{ $$ = C_STKIND_UNION_NATIVE; }
	;

StructDeclarationList
	: /* empty */
	| StructDeclarationList2
	;

StructDeclarationList2
	: StructDeclaration
	| StructDeclarationList2 StructDeclaration
	;

StructDeclaration
	: TypeSpecifierList StructDeclaratorList ';'	{
				/* Process each of the declarators in the list */
				ILNode_ListIter iter;
				ILNode_CDeclarator *decl;
				CDeclSpec spec;
				if(yyisa($2, ILNode_List))
				{
					/* There are two or more declarators in the list */
					spec = CDeclSpecFinalize($1, 0, 0, 0);
					ILNode_ListIter_Init(&iter, $2);
					while((decl = (ILNode_CDeclarator *)
							ILNode_ListIter_Next(&iter)) != 0)
					{
						if(yyisa(decl, ILNode_CBitFieldDeclarator))
						{
							ProcessBitField
								(spec, ((ILNode_CBitFieldDeclarator *)decl)
											->decl,
								 ((ILNode_CBitFieldDeclarator *)decl)
								 			->bitFieldSize);
						}
						else
						{
							ProcessField(spec, decl->decl);
						}
					}
				}
				else if(yyisa($2, ILNode_CBitFieldDeclarator))
				{
					/* There is a single bit field in the list */
					spec = CDeclSpecFinalize
						($1, ((ILNode_CBitFieldDeclarator *)($2))->decl.node,
						 ((ILNode_CBitFieldDeclarator *)($2))->decl.name, 0);
					ProcessBitField
						(spec, ((ILNode_CBitFieldDeclarator *)($2))->decl,
						 ((ILNode_CBitFieldDeclarator *)($2))->bitFieldSize);
				}
				else
				{
					/* There is a single ordinary field in the list */
					spec = CDeclSpecFinalize
						($1, ((ILNode_CDeclarator *)($2))->decl.node,
						 ((ILNode_CDeclarator *)($2))->decl.name, 0);
					ProcessField(spec, ((ILNode_CDeclarator *)($2))->decl);
				}
			}
	;

StructDeclaratorList
	: StructDeclarator
	| StructDeclaratorList ',' StructDeclarator		{
				if(yyisa($1, ILNode_List))
				{
					ILNode_List_Add($1, $3);
					$$ = $1;
				}
				else
				{
					$$ = ILNode_List_create();
					ILNode_List_Add($$, $1);
					ILNode_List_Add($$, $3);
				}
			}
	;

StructDeclarator
	: Declarator				{ $$ = ILNode_CDeclarator_create($1); }
	| ':' ConstantExpression	{
				ILUInt32 size = EvaluateSize($2);
				CDeclarator decl;
				CDeclSetName(decl, 0, 0);
				$$ = ILNode_CBitFieldDeclarator_create(decl, size);
			}
	| Declarator ':' ConstantExpression		{
				ILUInt32 size = EvaluateSize($3);
				$$ = ILNode_CBitFieldDeclarator_create($1, size);
			}
	;

EnumSpecifier
	: K_ENUM '{'	{
				/* Define an anonymous enum type */
				$<structInfo>$.type = CTypeDefineAnonEnum
						(&CCCodeGen, functionName);

				/* Push in a scope level for the enum */
				$<structInfo>$.parent = currentEnum;
				currentEnum = $<structInfo>$.type;
				currentEnumValue = 0;
			}
	  EnumeratorList '}'	{
	  			/* Pop the enum scope */
				currentEnum = $<structInfo>3.parent;

				/* Return the completed type to the next higher level */
				$$ = $<structInfo>3.type;
	  		}
	| K_ENUM AnyIdentifier '{'	{
				/* Look for a definition in the local scope */
				if(!CScopeHasEnum($2))
				{
					/* Define the enum type */
					$<structInfo>$.type = CTypeDefineEnum
							(&CCCodeGen, $2, functionName);
				}
				else
				{
					/* We've already seen a definition for this type before */
					CCError(_("redefinition of `enum %s'"), $2);

					/* Create an anonymous type as a place-holder */
					$<structInfo>$.type = CTypeDefineAnonEnum
							(&CCCodeGen, functionName);
				}

				/* Push in a scope level for the enum */
				$<structInfo>$.parent = currentEnum;
				currentEnum = $<structInfo>$.type;
				currentEnumValue = 0;

				/* Add the type to the current scope */
				CScopeAddEnum($2, $<structInfo>$.type);
			}
	  EnumeratorList '}'	{
	  			/* Pop the enum scope */
				currentEnum = $<structInfo>4.parent;

				/* Return the completed type to the next higher level */
				$$ = $<structInfo>4.type;
			}
	| K_ENUM AnyIdentifier		{
				/* Look for an existing definition for this type */
				void *data = CScopeLookupEnum($2);
				if(!data)
				{
					/* Create a reference to the named enum type
					   assuming that it is at the global level */
					$$ = CTypeCreateEnum(&CCCodeGen, $2, 0);
				}
				else
				{
					/* Use the type that was registered in the scope */
					$$ = CScopeGetType(data);
				}
			}
	;

EnumeratorList
	: EnumeratorListNoComma
	| EnumeratorListNoComma ','
	;

EnumeratorListNoComma
	: Enumerator
	| EnumeratorListNoComma ',' Enumerator
	;

Enumerator
	: IDENTIFIER		{
				/* See if we already have a definition with this name */
				void *data = CScopeLookupCurrent($1);
				ILNode *node;
				if(data)
				{
					CCError(_("redeclaration of `%s'"), $1);
					node = CScopeGetNode(data);
					if(node)
					{
						CCErrorOnLine(yygetfilename(node), yygetlinenum(node),
					  				  _("`%s' previously declared here"), $1);
					}
				}
				else
				{
					CTypeDefineEnumConst(&CCCodeGen, currentEnum,
										 $1, currentEnumValue);
					CScopeAddEnumConst($1, ILQualIdentSimple($1),
									   currentEnumValue, currentEnum);
				}
				++currentEnumValue;
			}
	| IDENTIFIER '=' ConstantExpression		{
				void *data;
				ILNode *node;

				/* Update the current enumeration value */
				currentEnumValue = EvaluateIntConstant($3);

				/* See if we already have a definition with this name */
				data = CScopeLookupCurrent($1);
				if(data)
				{
					CCError(_("redeclaration of `%s'"), $1);
					node = CScopeGetNode(data);
					if(node)
					{
						CCErrorOnLine(yygetfilename(node), yygetlinenum(node),
					  				  _("`%s' previously declared here"), $1);
					}
				}
				else
				{
					CTypeDefineEnumConst(&CCCodeGen, currentEnum,
										 $1, currentEnumValue);
					CScopeAddEnumConst($1, ILQualIdentSimple($1),
									   currentEnumValue, currentEnum);
				}
				++currentEnumValue;
			}
	;

CSharpSpecifier
	: K_CSHARP '(' CSharpType ')'	{ $$ = $3; }
	;

CSharpType
	: QualifiedIdentifier	{
				/* Resolve the qualified identifier to a type name */
				$$ = CTypeFromCSharp(&CCCodeGen, $1);
				if(!($$))
				{
					CCError(_("could not resolve `%s' as a C# type"),
							ILQualIdentName($1, 0));
					$$ = ILType_Int32;
				}
			}
	| CSharpBuiltinType
	| CSharpType '[' DimensionSeparators ']'	{
				/* Create a C# array type */
				if(!ILType_IsArray($1))
				{
					/* Create a simple array from a non-array element type */
					$$ = ILTypeCreateArray(CCCodeGen.context,
										   (unsigned long)($3), $1);
					if(!($$))
					{
						CCOutOfMemory();
					}
				}
				else
				{
					/* Find the position of the innermost element type */
					ILType **elem = &(ILType_ElemType($1));
					while(ILType_IsArray(*elem))
					{
						elem = &(ILType_ElemType(*elem));
					}

					/* Replace the element type with a new array */
					*elem = ILTypeCreateArray(CCCodeGen.context,
										      (unsigned long)($3), *elem);
					if(!(*elem))
					{
						CCOutOfMemory();
					}
					$$ = $1;
				}
			}
	| CSharpType '*'	{
				/* Create a C# pointer type */
				$$ = ILTypeCreateRef(CCCodeGen.context,
									 IL_TYPE_COMPLEX_PTR, $1);
				if(!($$))
				{
					CCOutOfMemory();
				}
			}
	;

QualifiedIdentifier
	: AnyIdentifier		{ $$ = ILQualIdentSimple($1); }
	| QualifiedIdentifier '.' AnyIdentifier	{
				$$ = ILNode_QualIdent_create($1, ILQualIdentSimple($3));
			}
	;

/* This is a list of builtin C# types that are also C keywords.
   Other C# builtin types are captured by "QualifiedIdentifier"
   and then resolved by "CTypeFromCSharp" */
CSharpBuiltinType
	: K_VOID			{ $$ = ILType_Void; }
	| K_SHORT			{ $$ = ILType_Int16; }
	| K_INT				{ $$ = ILType_Int32; }
	| K_LONG			{ $$ = ILType_Int64; }
	| K_CHAR			{ $$ = ILType_Char; }
	| K_FLOAT			{ $$ = ILType_Float32; }
	| K_DOUBLE			{ $$ = ILType_Float64; }
	;

DimensionSeparators
	: /* empty */					{ $$ = 1; }
	| DimensionSeparatorList		{ $$ = $1; }
	;

DimensionSeparatorList
	: ','							{ $$ = 2; }
	| DimensionSeparatorList ','	{ $$ = $1 + 1; }
	;

Declarator
	: Declarator2
	| Pointer Declarator2		{ $$ = CDeclCombine($1, $2); }
	;

Declarator2
	: IDENTIFIER				{
				CDeclSetName($$, $1, ILQualIdentSimple($1));
			}
	| IDENTIFIER Attributes		{
				CDeclSetName($$, $1, ILQualIdentSimple($1));
				$$.attrs = $2;
			}
	| '(' Declarator ')'		{ $$ = $2; }
	| Declarator2 '[' ']'		{
				$$ = CDeclCreateOpenArray(&CCCodeGen, $1);
			}
	| Declarator2 '[' ']' Attributes		{
				$$ = CDeclCreateOpenArray(&CCCodeGen, $1);
				$$.attrs = $4;
			}
	| Declarator2 '[' ConstantExpression ']'	{
				/* Evaluate the constant value */
				ILUInt32 size = EvaluateSize($3);

				/* Create the array */
				$$ = CDeclCreateArray(&CCCodeGen, $1, size);
			}
	| Declarator2 '[' ConstantExpression ']' Attributes	{
				/* Evaluate the constant value */
				ILUInt32 size = EvaluateSize($3);

				/* Create the array */
				$$ = CDeclCreateArray(&CCCodeGen, $1, size);
				$$.attrs = $5;
			}
	| Declarator2 '(' ')'		{
				$$ = CDeclCreatePrototype(&CCCodeGen, $1, 0, 0);
			}
	| Declarator2 '(' ')' Attributes	{
				$$ = CDeclCreatePrototype(&CCCodeGen, $1, 0, $4);
			}
	| Declarator2 '(' ParameterTypeList ')'	{
				$$ = CDeclCreatePrototype(&CCCodeGen, $1, $3, 0);
			}
	| Declarator2 '(' ParameterTypeList ')' Attributes	{
				$$ = CDeclCreatePrototype(&CCCodeGen, $1, $3, $5);
			}
	| Declarator2 '(' ParameterIdentifierList ')'	{
				$$ = CDeclCreatePrototype(&CCCodeGen, $1, $3, 0);
			}
	| Declarator2 '(' ParameterIdentifierList ')' Attributes {
				$$ = CDeclCreatePrototype(&CCCodeGen, $1, $3, $5);
			}
	;

Attributes
	: K_ATTRIBUTE '(' '(' AttributeList ')' ')'	{ $$ = $4; }
	;

AttributeList
	: Attribute			{
				$$ = ILNode_List_create();
				ILNode_List_Add($$, $1);
			}
	| AttributeList ',' Attribute	{
				ILNode_List_Add($1, $3);
				$$ = $1;
			}
	;

Attribute
	: AnyIdentifier		{
				$$ = ILNode_CAttribute_create($1, 0);
			}
	| AnyIdentifier '(' AttributeArgs ')'		{
				$$ = ILNode_CAttribute_create($1, $3);
			}
	;

ConstantAttributeExpression
	: ConditionalExpression		{
				ILNode *node = ILNode_ToConst_create($1);
				CSemValue value;
				ILEvalValue evalValue;
				value = CSemInlineAnalysis(&CCCodeGen, node, CCurrentScope);
				if(CSemIsRValue(value))
				{
					if(yyisa($1, ILNode_CString))
					{
						evalValue.valueType = ILMachineType_String;
						evalValue.un.strValue.str =
								((ILNode_CString *)($1))->str;
						evalValue.un.strValue.len =
								((ILNode_CString *)($1))->len;
					}
					else if(!ILNode_EvalConst(node, &CCCodeGen, &evalValue))
					{
						CCError(_("compile-time constant value required"));
						evalValue.valueType = ILMachineType_Void;
					}
				}
				else
				{
					evalValue.valueType = ILMachineType_Void;
				}
				$$ = ILNode_CAttributeValue_create(evalValue);
			}
	;

AttributeArgs
	: ConstantAttributeExpression
	| AttributeArgs ',' ConstantAttributeExpression	{
				$$ = ILNode_ArgList_create($1, $3);
			}
	;

Pointer
	: '*'						{
				$$ = CDeclCreatePointer(&CCCodeGen, 0, 0);
			}
	| '*' TypeQualifierList		{
				$$ = CDeclCreatePointer(&CCCodeGen, $2, 0);
			}
	| '*' Pointer				{
				$$ = CDeclCreatePointer(&CCCodeGen, 0, &($2));
			}
	| '*' TypeQualifierList Pointer		{
				$$ = CDeclCreatePointer(&CCCodeGen, $2, &($3));
			}
	;

TypeQualifier
	: K_CONST			{ $$ = C_SPEC_CONST; }
	| K_VOLATILE		{ $$ = C_SPEC_VOLATILE; }
	;

TypeQualifierList
	: TypeQualifier
	| TypeQualifierList TypeQualifier	{
				if(($1 & $2 & C_SPEC_CONST) != 0)
				{
					CCWarning(_("duplicate `const'"));
				}
				if(($1 & $2 & C_SPEC_VOLATILE) != 0)
				{
					CCWarning(_("duplicate `volatile'"));
				}
				$$ = ($1 | $2);
			}
	;

TypeSpecifierList
	: TypeSpecifier
	| TypeSpecifierList TypeSpecifier	{ $$ = CDeclSpecCombine($1, $2); }
	;

ParameterIdentifierList
	: IdentifierList
	| IdentifierList ',' K_ELIPSIS	{
				ILNode_List_Add($1, ILNode_FormalParameter_create
					(0, ILParamMod_arglist, 0, 0));
				$$ = $1;
			}
	;

IdentifierList
	: IDENTIFIER	{
				$$ = ILNode_List_create();
				ILNode_List_Add($$, ILNode_FormalParameter_create
					(0, ILParamMod_empty, 0, ILQualIdentSimple($1)));
			}
	| IdentifierList ',' IDENTIFIER		{
				ILNode_List_Add($1, ILNode_FormalParameter_create
					(0, ILParamMod_empty, 0, ILQualIdentSimple($3)));
				$$ = $1;
			}
	;

ParameterTypeList
	: ParameterList
	| ParameterList ',' K_ELIPSIS	{
				ILNode_List_Add($1, ILNode_FormalParameter_create
					(0, ILParamMod_arglist, 0, 0));
				$$ = $1;
			}
	;

ParameterList
	: ParameterDeclaration	{
				$$ = ILNode_List_create();
				ILNode_List_Add($$, $1);
			}
	| ParameterList ',' ParameterDeclaration	{
				ILNode_List_Add($1, $3);
				$$ = $1;
			}
	;

ParameterDeclaration
	: TypeSpecifierList Declarator		{
				ILType *type;
				ILNode *nameNode;
				CDeclSpec spec;
				spec = CDeclSpecFinalize
					($1, $2.node, $2.name, C_KIND_PARAMETER_NAME);
				type = CDeclFinalize(&CCCodeGen, spec, $2, 0, 0);
				nameNode = $2.node;
				if(!nameNode && $2.name != 0)
				{
					nameNode = ILQualIdentSimple($2.name);
				}
				$$ = ILNode_FormalParameter_create
					(0, ILParamMod_empty,
					 ILNode_MarkType_create(0, type), nameNode);
			}
	| K_REGISTER TypeSpecifierList Declarator		{
				ILType *type;
				ILNode *nameNode;
				CDeclSpec spec;
				spec = CDeclSpecFinalize
					($2, $3.node, $3.name, C_KIND_PARAMETER_NAME);
				type = CDeclFinalize(&CCCodeGen, spec, $3, 0, 0);
				nameNode = $3.node;
				if(!nameNode && $3.name != 0)
				{
					nameNode = ILQualIdentSimple($3.name);
				}
				$$ = ILNode_FormalParameter_create
					(0, ILParamMod_empty,
					 ILNode_MarkType_create(0, type), nameNode);
			}
	| TypeName							{
				$$ = ILNode_FormalParameter_create
					(0, ILParamMod_empty, ILNode_MarkType_create(0, $1), 0);
			}
	;

TypeName
	: TypeSpecifierList			{
				CDeclarator decl;
				CDeclSpec spec;
				CDeclSetName(decl, 0, 0);
				spec = CDeclSpecFinalize($1, 0, 0, 0);
				$$ = CDeclFinalize(&CCCodeGen, spec, decl, 0, 0);
			}
	| TypeSpecifierList AbstractDeclarator	{
				CDeclSpec spec;
				spec = CDeclSpecFinalize($1, 0, 0, 0);
				$$ = CDeclFinalize(&CCCodeGen, spec, $2, 0, 0);
			}
	;

AbstractDeclarator
	: Pointer
	| AbstractDeclarator2
	| Pointer AbstractDeclarator2		{ $$ = CDeclCombine($1, $2); }
	;

AbstractDeclarator2
	: '(' AbstractDeclarator ')'		{ $$ = $2; }
	| '[' ']'			{
				CDeclSetName($$, 0, 0);
				$$ = CDeclCreateOpenArray(&CCCodeGen, $$);
			}
	| '[' ']' Attributes			{
				CDeclSetName($$, 0, 0);
				$$ = CDeclCreateOpenArray(&CCCodeGen, $$);
				$$.attrs = $3;
			}
	| '[' ConstantExpression ']'	{
				/* Evaluate the constant value */
				ILUInt32 size = EvaluateSize($2);

				/* Create the array */
				CDeclSetName($$, 0, 0);
				$$ = CDeclCreateArray(&CCCodeGen, $$, size);
			}
	| '[' ConstantExpression ']' Attributes	{
				/* Evaluate the constant value */
				ILUInt32 size = EvaluateSize($2);

				/* Create the array */
				CDeclSetName($$, 0, 0);
				$$ = CDeclCreateArray(&CCCodeGen, $$, size);
				$$.attrs = $4;
			}
	| AbstractDeclarator2 '[' ']'	{
				$$ = CDeclCreateOpenArray(&CCCodeGen, $1);
			}
	| AbstractDeclarator2 '[' ']' Attributes	{
				$$ = CDeclCreateOpenArray(&CCCodeGen, $1);
				$$.attrs = $4;
			}
	| AbstractDeclarator2 '[' ConstantExpression ']'	{
				/* Evaluate the constant value */
				ILUInt32 size = EvaluateSize($3);

				/* Create the array */
				$$ = CDeclCreateArray(&CCCodeGen, $1, size);
			}
	| AbstractDeclarator2 '[' ConstantExpression ']' Attributes	{
				/* Evaluate the constant value */
				ILUInt32 size = EvaluateSize($3);

				/* Create the array */
				$$ = CDeclCreateArray(&CCCodeGen, $1, size);
				$$.attrs = $5;
			}
	| '(' ')'		{
				CDeclSetName($$, 0, 0);
				$$ = CDeclCreatePrototype(&CCCodeGen, $$, 0, 0);
			}
	| '(' ')' Attributes	{
				CDeclSetName($$, 0, 0);
				$$ = CDeclCreatePrototype(&CCCodeGen, $$, 0, $3);
			}
	| '(' ParameterTypeList ')'	{
				CDeclSetName($$, 0, 0);
				$$ = CDeclCreatePrototype(&CCCodeGen, $$, $2, 0);
			}
	| '(' ParameterTypeList ')' Attributes	{
				CDeclSetName($$, 0, 0);
				$$ = CDeclCreatePrototype(&CCCodeGen, $$, $2, $4);
			}
	| AbstractDeclarator2 '(' ')'	{
				$$ = CDeclCreatePrototype(&CCCodeGen, $1, 0, 0);
			}
	| AbstractDeclarator2 '(' ')' Attributes	{
				$$ = CDeclCreatePrototype(&CCCodeGen, $1, 0, $4);
			}
	| AbstractDeclarator2 '(' ParameterTypeList ')'	{
				$$ = CDeclCreatePrototype(&CCCodeGen, $1, $3, 0);
			}
	| AbstractDeclarator2 '(' ParameterTypeList ')' Attributes	{
				$$ = CDeclCreatePrototype(&CCCodeGen, $1, $3, $5);
			}
	;

Initializer
	: AssignmentExpression				{ $$ = $1; }
	| '{' InitializerList '}'			{ $$ = ILNode_ArrayInit_create($2); }
	| '{' InitializerList ',' '}'		{ $$ = ILNode_ArrayInit_create($2); }
	;

InitializerList
	: Initializer						{
				$$ = ILNode_List_create();
				ILNode_List_Add($$, $1);
			}
	| InitializerList ',' Initializer	{
				$$ = $1;
				ILNode_List_Add($$, $3);
			}
	;

Statement
	: Statement2		{
			#ifdef YYBISON
				if(debug_flag)
				{
					$$ = ILNode_LineInfo_create($1);
					yysetlinenum($$, @1.first_line);
				}
				else
			#endif
				{
					$$ = $1;
				}
			}
	;

Statement2
	: LabeledStatement
	| CompoundStatement
	| ExpressionStatement
	| SelectionStatement
	| IterationStatement
	| JumpStatement
	| AsmStatement
	;

LabeledStatement
	: IDENTIFIER ':' {
				$<node>$ = ILNode_GotoLabel_create($1);
			}
	  Statement		{
				$$ = ILNode_Compound_CreateFrom($<node>3, $4);
			}
	| K_CASE ConstantExpression ':' {
				$<node>$ = ILNode_CaseLabel_create($2);
			}
	  Statement	{
				$$ = ILNode_Compound_CreateFrom($<node>4, $5);
			}
	| K_DEFAULT ':' {
				$<node>$ = ILNode_DefaultLabel_create();
			}
	  Statement {
				$$ = ILNode_Compound_CreateFrom($<node>3, $4);
			}
	;

CompoundStatement
	: '{'	{
				/* Create a new scope */
				CCurrentScope = ILScopeCreate(&CCCodeGen, CCurrentScope);
			}
	  OptDeclarationList OptStatementList '}'	{
	  			/* Build the compound statement block if it isn't empty */
				if($3 != 0 || $4 != 0)
				{
					$$ = ILNode_NewScope_create
						(ILNode_Compound_CreateFrom($3, $4));
					((ILNode_NewScope *)($$))->scope = CCurrentScope;
				}
				else
				{
					$$ = ILNode_Empty_create();
				}

				/* Fix up the line number on the compound statement node */
			#ifdef YYBISON
				yysetlinenum($$, @1.first_line);
			#endif

	  			/* Pop the scope */
				CCurrentScope = ILScopeGetParent(CCurrentScope);
			}
	;

OptDeclarationList
	: DeclarationList		{ $$ = $1; }
	| /* empty */			{ $$ = 0; }
	;

DeclarationList
	: Declaration
	| DeclarationList Declaration	{
				$$ = ILNode_Compound_CreateFrom($1, $2);
			}
	;

OptStatementList
	: StatementList			{ $$ = $1; }
	| /* empty */			{ $$ = 0; }
	;

StatementList
	: Statement
	| StatementList Statement	{
				$$ = ILNode_Compound_CreateFrom($1, $2);
			}
	;

ExpressionStatement
	: ';'					{ $$ = ILNode_Empty_create(); }
	| Expression ';'		{ $$ = $1; }
	;

BoolExpression
	: Expression			{ $$ = ILNode_ToBool_create($1); }
	;

SelectionStatement
	: K_IF '(' BoolExpression ')' Statement	{
				$$ = ILNode_If_create($3, $5, ILNode_Empty_create());
			}
	| K_IF '(' BoolExpression ')' Statement K_ELSE Statement	{
				$$ = ILNode_If_create($3, $5, $7);
			}
	| K_SWITCH '(' Expression ')' Statement	{
				$$ = ILNode_Switch_create($3, 0, $5);
				CGenCloneLine($$, $3);
			}
	;

IterationStatement
	: K_WHILE '(' BoolExpression ')' Statement	{
				$$ = ILNode_While_create($3, $5);
			}
	| K_DO Statement K_WHILE '(' BoolExpression ')' ';'	{
				$$ = ILNode_Do_create($2, $5);
			}
	| K_FOR '(' ';' ';' ')' Statement	{
				$$ = ILNode_For_create(ILNode_Empty_create(),
									   ILNode_True_create(),
									   ILNode_Empty_create(), $6);
			}
	| K_FOR '(' ';' ';' Expression ')' Statement	{
				$$ = ILNode_For_create(ILNode_Empty_create(),
									   ILNode_True_create(),
									   $5, $7);
			}
	| K_FOR '(' ';' BoolExpression ';' ')' Statement	{
				$$ = ILNode_For_create(ILNode_Empty_create(), $4,
									   ILNode_Empty_create(), $7);
			}
	| K_FOR '(' ';' BoolExpression ';' Expression ')' Statement	{
				$$ = ILNode_For_create(ILNode_Empty_create(), $4, $6, $8);
			}
	| K_FOR '(' Expression ';' ';' ')' Statement	{
				$$ = ILNode_For_create($3,
									   ILNode_True_create(),
									   ILNode_Empty_create(), $7);
			}
	| K_FOR '(' Expression ';' ';' Expression ')' Statement	{
				$$ = ILNode_For_create($3, ILNode_True_create(), $6, $8);
			}
	| K_FOR '(' Expression ';' BoolExpression ';' ')' Statement	{
				$$ = ILNode_For_create($3, $5, ILNode_Empty_create(), $8);
			}
	| K_FOR '(' Expression ';' BoolExpression ';' Expression ')' Statement	{
				$$ = ILNode_For_create($3, $5, $7, $9);
			}
	;

JumpStatement
	: K_GOTO IDENTIFIER ';'		{ $$ = ILNode_Goto_create($2); }
	| K_CONTINUE ';'			{ $$ = ILNode_Continue_create(); }
	| K_BREAK ';'				{ $$ = ILNode_Break_create(); }
	| K_RETURN ';'				{ $$ = ILNode_Return_create(); }
	| K_RETURN Expression ';'	{ $$ = ILNode_ReturnExpr_create($2); }
	;

/* We currently only support simple __asm__ code forms as statements */
AsmStatement
	: K_ASM '(' StringLiteral ':' ':' ')'	{
				$$ = ILNode_AsmStmt_create($3.string);
			}
	| K_ASM K_VOLATILE '(' StringLiteral ':' ':' ')'	{
				$$ = ILNode_AsmStmt_create($4.string);
			}
	;

File
	: File2				{
				/* Flush the code for any remaining initializers */
				if(initializers != 0)
				{
					CFunctionFlushInits(&CCCodeGen, initializers);
					initializers = 0;
				}

				/* Roll the treecc node heap back to the last check point */
				yynodepop();
				yynodepush();
			}
	| /* empty */
	;

File2
	: ExternalDefinition
	| File2 ExternalDefinition
	;

ExternalDefinition
	: FunctionDefinition	{
				/* Roll the treecc node heap back to the last check point */
				yynodepop();
				yynodepush();
			}
	| Declaration			{ /* Nothing to do here */ }
	;

FunctionDefinition
	: Declarator OptParamDeclarationList '{'	{
				CDeclSpec spec;
				ILMethod *method;

				/* Flush the code for any pending initializers */
				if(initializers != 0)
				{
					CFunctionFlushInits(&CCCodeGen, initializers);
					initializers = 0;
				}

				/* The default return type in this case is "int" */
				CDeclSpecSetType(spec, ILType_Int32);

				/* Create the method block from the function header */
				method = CFunctionCreate
					(&CCCodeGen, $1.name, $1.node, spec, $1, $2);

				/* Create a new scope to hold the function body */
				CCurrentScope = ILScopeCreate(&CCCodeGen, CCurrentScope);

				/* Declare the parameters into the new scope */
				CFunctionDeclareParams(&CCCodeGen, method);
				$<methodInfo>$ = method;

				/* Set the new function name */
				functionName = $1.name;
			}
	  FunctionBody '}'		{
	  			/* Wrap the function body in a new scope record */
				ILNode *body = ILNode_NewScope_create($5);
				((ILNode_NewScope *)body)->scope = CCurrentScope;

				/* Fix up the line number on the function body */
			#ifdef YYBISON
				yysetlinenum(body, @3.first_line);
			#endif

				/* Pop the scope */
				CCurrentScope = ILScopeGetParent(CCurrentScope);

				/* Output the finished function */
				CFunctionOutput(&CCCodeGen, $<methodInfo>4, body);

				/* Reset the function name */
				functionName = "";

				/* Flush the code for any pending initializers */
				if(initializers != 0)
				{
					CFunctionFlushInits(&CCCodeGen, initializers);
					initializers = 0;
				}
	  		}
	| DeclarationSpecifiers Declarator OptParamDeclarationList '{'	{
				ILMethod *method;
				CDeclSpec spec;

				/* Flush the code for any pending initializers */
				if(initializers != 0)
				{
					CFunctionFlushInits(&CCCodeGen, initializers);
					initializers = 0;
				}

				/* Finalize the declaration specifier */
				spec = CDeclSpecFinalize
						($1, $2.node, $2.name, C_KIND_GLOBAL_NAME);

				/* Create the method block from the function header */
				method = CFunctionCreate
					(&CCCodeGen, $2.name, $2.node, spec, $2, $3);

				/* Create a new scope to hold the function body */
				CCurrentScope = ILScopeCreate(&CCCodeGen, CCurrentScope);

				/* Declare the parameters into the new scope */
				CFunctionDeclareParams(&CCCodeGen, method);
				$<methodInfo>$ = method;

				/* Set the new function name */
				functionName = $2.name;
			}
	  FunctionBody '}'		{
	  			/* Wrap the function body in a new scope record */
				ILNode *body = ILNode_NewScope_create($6);
				((ILNode_NewScope *)body)->scope = CCurrentScope;

				/* Fix up the line number on the function body */
			#ifdef YYBISON
				yysetlinenum(body, @4.first_line);
			#endif

				/* Pop the scope */
				CCurrentScope = ILScopeGetParent(CCurrentScope);

				/* Output the finished function */
				CFunctionOutput(&CCCodeGen, $<methodInfo>5, body);

				/* Reset the function name */
				functionName = "";

				/* Flush the code for any pending initializers */
				if(initializers != 0)
				{
					CFunctionFlushInits(&CCCodeGen, initializers);
					initializers = 0;
				}
	  		}
	;

OptParamDeclarationList
	: ParamDeclarationList		{ $$ = $1; }
	| /* empty */				{ $$ = ILNode_List_create(); }
	;

ParamDeclarationList
	: ParamDeclaration		{
				if(yyisa($1, ILNode_List))
				{
					$$ = $1;
				}
				else
				{
					$$ = ILNode_List_create();
					ILNode_List_Add($$, $1);
				}
			}
	| ParamDeclarationList ParamDeclaration	{
				CopyParamDecls($1, $2);
				$$ = $1;
			}
	;

ParamDeclaration
	: DeclarationSpecifiers ParamDeclaratorList ';'	{
				CDeclSpec spec;
				char *name;
				ILNode *nameNode;
				ILType *type;
				ILNode_CDeclarator *decl;
				ILNode_ListIter iter;

				/* Get the first name and its node for error reporting */
				if(yyisa($2, ILNode_CDeclarator))
				{
					name = ((ILNode_CDeclarator *)($2))->decl.name;
					nameNode = ((ILNode_CDeclarator *)($2))->decl.node;
				}
				else
				{
					nameNode = ((ILNode_List *)($2))->item1;
					name = ((ILNode_CDeclarator *)nameNode)->decl.name;
					nameNode = ((ILNode_CDeclarator *)nameNode)->decl.node;
				}

				/* Finalize the declaration specifiers */
				spec = CDeclSpecFinalize($1, nameNode, name,
										 C_KIND_PARAMETER_NAME);

				/* Build the formal parameters from the declarators */
				if(yyisa($2, ILNode_CDeclarator))
				{
					decl = (ILNode_CDeclarator *)($2);
					type = CDeclFinalize(&CCCodeGen, spec, decl->decl, 0, 0);
					$$ = ILNode_FormalParameter_create
						(0, ILParamMod_empty, ILNode_MarkType_create(0, type),
						 decl->decl.node);
				}
				else
				{
					$$ = ILNode_List_create();
					ILNode_ListIter_Init(&iter, $2);
					while((decl = (ILNode_CDeclarator *)
							ILNode_ListIter_Next(&iter)) != 0)
					{
						type = CDeclFinalize(&CCCodeGen, spec,
											 decl->decl, 0, 0);
						ILNode_List_Add($$, ILNode_FormalParameter_create
							(0, ILParamMod_empty,
							 ILNode_MarkType_create(0, type),
							 decl->decl.node));
					}
				}
			}
	;

ParamDeclaratorList
	: ParamDeclarator		{
				/* Don't bother creating a list for the common case
				   of there being only one declarator */
				$$ = ILNode_CDeclarator_create($1);
			}
	| ParamDeclaratorList ',' ParamDeclarator	{
				if(yyisa($1, ILNode_List))
				{
					/* Add the declarator to the existing list */
					ILNode_List_Add($1, ILNode_CDeclarator_create($3));
					$$ = $1;
				}
				else
				{
					/* At least two declarators: create a new list */
					$$ = ILNode_List_create();
					ILNode_List_Add($$, $1);
					ILNode_List_Add($$, ILNode_CDeclarator_create($3));
				}
			}
	;

ParamDeclarator
	: Declarator
	;

FunctionBody
	: OptDeclarationList OptStatementList	{
				if($1 != 0 || $2 != 0)
				{
					$$ = ILNode_Compound_CreateFrom($1, $2);
				}
				else
				{
					$$ = ILNode_Empty_create();
				}
			}
	;
