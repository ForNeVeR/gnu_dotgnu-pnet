%{
/*
 * cs_grammar.y - Input file for yacc that defines the syntax of C#.
 *
 * Copyright (C) 2001, 2002, 2003  Southern Storm Software, Pty Ltd.
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
#include "cs_rename.h"

/*#define YYDEBUG 1*/

#include <stdio.h>
#include "il_system.h"
#include "il_opcodes.h"
#include "il_meta.h"
#include "il_utils.h"
#include "cs_internal.h"
#ifdef HAVE_STDARG_H
	#include <stdarg.h>
#else
	#ifdef HAVE_VARARGS_H
		#include <varargs.h>
	#endif
#endif

#define	YYERROR_VERBOSE

/*
 * An ugly hack to work around missing "-lfl" libraries on MacOSX.
 */
#if defined(__APPLE_CC__) && !defined(YYTEXT_POINTER)
	#define	YYTEXT_POINTER 1
#endif

/*
 * Imports from the lexical analyser.
 */
extern int yylex(void);
#ifdef YYTEXT_POINTER
extern char *cs_text;
#else
extern char cs_text[];
#endif

int CSMetadataOnly = 0;

/*
 * Global state used by the parser.
 */
static unsigned long NestingLevel = 0;
static ILIntString CurrNamespace = {"", 0};
static ILNode_Namespace *CurrNamespaceNode = 0;
static ILScope* localScope = 0;
static int HaveDecls = 0;

/*
 * Initialize the global namespace, if necessary.
 */
static void InitGlobalNamespace(void)
{
	if(!CurrNamespaceNode)
	{
		ILNode_UsingNamespace *using;
		CurrNamespaceNode = (ILNode_Namespace *)ILNode_Namespace_create(0, 0);
		using = (ILNode_UsingNamespace *)ILNode_UsingNamespace_create("System");
		using->next = CurrNamespaceNode->using;
		CurrNamespaceNode->using = using;
	}
}

/*
 * Get the global scope.
 */
static ILScope *GlobalScope(void)
{
	if(CCGlobalScope)
	{
		return CCGlobalScope;
	}
	else
	{
		CCGlobalScope = ILScopeCreate(&CCCodeGen, 0);
		ILScopeDeclareNamespace(CCGlobalScope, "System");
		ILScopeUsing(CCGlobalScope, "System");
		return CCGlobalScope;
	}
}

/* 
 * Get the local scope
 */
static ILScope *LocalScope(void)
{
	if(localScope)
	{
		return localScope;
	}
	else
	{
		localScope = ILScopeCreate(&CCCodeGen, CCGlobalScope);
		return localScope;
	}
}

/*
 * Reset the global state ready for the next file to be parsed.
 */
static void ResetState(void)
{
	NestingLevel = 0;
	CurrNamespace = ILInternString("", 0);
	CurrNamespaceNode = 0;
	HaveDecls = 0;
	localScope = 0;
	ILScopeClearUsing(GlobalScope());
}

/*
 * Determine if the current namespace already has a "using"
 * declaration for a particular namespace.
 */
static int HaveUsingNamespace(char *name)
{
	ILNode_UsingNamespace *using = CurrNamespaceNode->using;
	while(using != 0)
	{
		if(!strcmp(using->name, name))
		{
			return 1;
		}
		using = using->next;
	}
	return 0;
}

static void yyerror(char *msg)
{
	CCPluginParseError(msg, cs_text);
}

/*
 * Determine if an extension has been enabled using "-f".
 */
#define	HaveExtension(name)	\
	(CSStringListContains(extension_flags, num_extension_flags, (name)))

/*
 * Make a simple node and put it into $$.
 */
#define	MakeSimple(classSuffix)	\
	do {	\
		yyval.node = \
			ILNode_##classSuffix##_create(); \
	} while (0)

/*
 * Make a unary node and put it into $$.
 */
#define	MakeUnary(classSuffix,expr)	\
	do {	\
		yyval.node = ILNode_##classSuffix##_create((expr)); \
	} while (0)

/*
 * Make a binary node and put it into $$.
 */
#define	MakeBinary(classSuffix,expr1,expr2)	\
	do {	\
		yyval.node = ILNode_##classSuffix##_create((expr1), (expr2)); \
	} while (0)

/*
 * Make a ternary node and put it into $$.
 */
#define	MakeTernary(classSuffix,expr1,expr2,expr3)	\
	do {	\
		yyval.node = ILNode_##classSuffix##_create((expr1), (expr2), (expr3)); \
	} while (0)

/*
 * Make a quaternary node and put it into $$.
 */
#define	MakeQuaternary(classSuffix,expr1,expr2,expr3,expr4)	\
	do {	\
		yyval.node = ILNode_##classSuffix##_create \
							((expr1), (expr2), (expr3), (expr4)); \
	} while (0)

/*
 * Make a system type name node.
 */
#define	MakeSystemType(name)	\
			(ILNode_GlobalNamespace_create(ILNode_SystemType_create(name)))

/*
 * Clone the filename/linenum information from one node to another.
 */
static void CloneLine(ILNode *dest, ILNode *src)
{
	yysetfilename(dest, yygetfilename(src));
	yysetlinenum(dest, yygetlinenum(src));
}

/*
 * Make a list from an existing list (may be NULL), and a new node
 * (which may also be NULL).
 */
static ILNode *MakeList(ILNode *list, ILNode *node)
{
	if(!node)
	{
		return list;
	}
	else if(!list)
	{
		list = ILNode_List_create();
	}
	ILNode_List_Add(list, node);
	return list;
}

/*
 * Negate an integer node.
 */
static ILNode *NegateInteger(ILNode_Integer *node)
{
	if(node->canneg)
	{
		if(yyisa(node, ILNode_Int32))
		{
			node->isneg = !(node->isneg);
			return (ILNode *)node;
		}
		else if(yyisa(node, ILNode_UInt32))
		{
			return ILNode_Int32_create(node->value, 1, 0);
		}
		else if(yyisa(node, ILNode_Int64))
		{
			node->isneg = !(node->isneg);
			return (ILNode *)node;
		}
		else if(yyisa(node, ILNode_UInt64))
		{
			return ILNode_Int64_create(node->value, 1, 0);
		}
	}
	return ILNode_Neg_create((ILNode *)node);
}

/*
 * The class name stack, which is used to verify the names
 * of constructors and destructors against the name of their
 * enclosing classes.  Also used to check if a class has
 * had a constructor defined for it.
 */
static ILNode **classNameStack = 0;
static int     *classNameCtorDefined = 0;
static int		classNameStackSize = 0;
static int		classNameStackMax = 0;

/*
 * Push an item onto the class name stack.
 */
static void ClassNamePush(ILNode *name)
{
	if(classNameStackSize >= classNameStackMax)
	{
		classNameStack = (ILNode **)ILRealloc
			(classNameStack, sizeof(ILNode *) * (classNameStackMax + 4));
		if(!classNameStack)
		{
			CCOutOfMemory();
		}
		classNameCtorDefined = (int *)ILRealloc
			(classNameCtorDefined, sizeof(int) * (classNameStackMax + 4));
		if(!classNameCtorDefined)
		{
			CCOutOfMemory();
		}
		classNameStackMax += 4;
	}
	classNameStack[classNameStackSize] = name;
	classNameCtorDefined[classNameStackSize++] = 0;
}

/*
 * Pop an item from the class name stack.
 */
static void ClassNamePop(void)
{
	--classNameStackSize;
}

/*
 * Record that a constructor was defined for the current class.
 */
static void ClassNameCtorDefined(void)
{
	classNameCtorDefined[classNameStackSize - 1] = 1;
}

/*
 * Determine if a constructor was defined for the current class.
 */
static int ClassNameIsCtorDefined(void)
{
	return classNameCtorDefined[classNameStackSize - 1];
}

/*
 * Determine if an identifier is identical to
 * the top of the class name stack.
 */
static int ClassNameSame(ILNode *name)
{
	return (strcmp(((ILNode_Identifier *)name)->name,
	   ((ILNode_Identifier *)(classNameStack[classNameStackSize - 1]))->name)
	   			== 0);
}

/*
 * Modify an attribute name so that it ends in "Attribute".
 */
static void ModifyAttrName(ILNode *node,int force)
{
	char *name;
	int namelen;
	ILNode_Identifier *ident;
	
	if(yyisa(node,ILNode_QualIdent))
	{
		ModifyAttrName(((ILNode_QualIdent*)node)->right, force);
		return;
	}
	
	ident = (ILNode_Identifier*) node;
	
	name = ident->name;
	namelen = strlen(name);
	if(force || (namelen < 9 || strcmp(name + namelen - 9, "Attribute") != 0))
	{
		ident->name = ILInternAppendedString
			(ILInternString(name, namelen),
			 ILInternString("Attribute", 9)).string;
	}
}

/* A hack to rename the indexer during parsing , damn the C# designers,
 * they had to make the variable names resolved later using an attribute
 * public int <name>[int posn] would have been a cleaner design. But
 * This is an UGLY hack and should be removed as soon as someone figures
 * out how .
 */
static ILNode *GetIndexerName(ILGenInfo *info,ILNode_AttributeTree *attrTree,
								ILNode* prefixName)
{
	ILNode_ListIter iter;
	ILNode_ListIter iter2;
	ILNode *temp;
	ILNode *attr;
	ILNode_List *args;
	ILEvalValue evalValue;
	char* prefix=(prefixName) ? ILQualIdentName(prefixName,0) : NULL;
	int i;

	const char* possibleWays[] = {"IndexerName", "IndexerNameAttribute",
					"System.Runtime.CompilerServices.IndexerNameAttribute",
					"System.Runtime.CompilerServices.IndexerName"};
	int isIndexerName=0;
	
	if(attrTree && attrTree->sections)
	{
		ILNode_ListIter_Init(&iter, attrTree->sections);
		while((temp = ILNode_ListIter_Next(&iter))!=0)
		{	
			if(!(temp != NULL
					&& yyisa(temp, ILNode_AttributeSection) &&
					((ILNode_AttributeSection*)temp)->attrs != NULL))
			{
				continue;
			}
			
			ILNode_ListIter_Init(&iter2, 
				((ILNode_AttributeSection*)(temp))->attrs);
			while((attr = ILNode_ListIter_Next(&iter2))!=0)
			{
				for(i=0;i<sizeof(possibleWays)/sizeof(char*); i++)
				{
					isIndexerName |= !strcmp(
							ILQualIdentName(((ILNode_Attribute*)attr)->name,0)
							,possibleWays[i]);
				}
				if(isIndexerName)
				{
					/* NOTE: we make it 
					[System.Runtime.CompilerServices.IndexerNameAttribute]
					for the sake of resolution...This too is too ugly a 
					hack.
					*/
					ModifyAttrName(((ILNode_Attribute*)attr)->name,0);

					args=(ILNode_List*)((ILNode_AttrArgs*)
						(((ILNode_Attribute*)attr)->args))->positionalArgs;	
					if(yyisa(args->item1, ILNode_ToConst) &&
					   ILNode_EvalConst(args->item1,info,&evalValue))
					{
						if(evalValue.valueType==ILMachineType_String)
						{
							if(!prefix)
							{
								return ILQualIdentSimple(
									ILInternString(evalValue.un.strValue.str,
										evalValue.un.strValue.len).string);
							}
							else 
							{
								return ILNode_QualIdent_create(prefixName,
									ILQualIdentSimple(
									ILInternString(evalValue.un.strValue.str,
										evalValue.un.strValue.len).string));
							}
						}
					}
				}
			}
		}
	}
	if(!prefix)
		return ILQualIdentSimple(ILInternString("Item", 4).string);
	else 
		return ILNode_QualIdent_create(prefixName,
						ILQualIdentSimple(ILInternString("Item",4).string));
}
/*
 * Adjust the name of a property to include a "get_" or "set_" prefix.
 */
static ILNode *AdjustPropertyName(ILNode *name, char *prefix)
{
	ILNode *node;
	if(yykind(name) == yykindof(ILNode_Identifier))
	{
		/* Simple name: just add the prefix */
		node = ILQualIdentSimple
					(ILInternAppendedString
						(ILInternString(prefix, strlen(prefix)),
						 ILInternString(ILQualIdentName(name, 0), -1)).string);
		CloneLine(node, name);
		return node;
	}
	else if(yykind(name) == yykindof(ILNode_QualIdent))
	{
		/* Qualified name: add the prefix to the second component */
		node = ILNode_QualIdent_create(((ILNode_QualIdent *)name)->left,
			AdjustPropertyName(((ILNode_QualIdent *)name)->right, prefix));
		CloneLine(node, name);
		return node;
	}
	else
	{
		/* Shouldn't happen */
		return name;
	}
}

/*
 * Create the methods needed by a property definition.
 */
static void CreatePropertyMethods(ILNode_PropertyDeclaration *property)
{
	ILNode_MethodDeclaration *decl;
	ILNode *name;
	ILNode *params;
	ILNode_ListIter iter;
	ILNode *temp;

	/* Create the "get" method */
	if((property->getsetFlags & 1) != 0)
	{
		name = AdjustPropertyName(property->name, "get_");
		decl = (ILNode_MethodDeclaration *)(property->getAccessor);
		if(!decl)
		{
			/* Abstract interface definition */
			decl = (ILNode_MethodDeclaration *)
				ILNode_MethodDeclaration_create
						(0, property->modifiers, property->type,
						 name, property->params, 0);
			property->getAccessor = (ILNode *)decl;
		}
		else
		{
			/* Regular class definition */
			decl->modifiers = property->modifiers;
			decl->type = property->type;
			decl->name = name;
			decl->params = property->params;
		}
	}

	/* Create the "set" method */
	if((property->getsetFlags & 2) != 0)
	{
		name = AdjustPropertyName(property->name, "set_");
		params = ILNode_List_create();
		ILNode_ListIter_Init(&iter, property->params);
		while((temp = ILNode_ListIter_Next(&iter)) != 0)
		{
			ILNode_List_Add(params, temp);
		}
		ILNode_List_Add(params,
			ILNode_FormalParameter_create(0, ILParamMod_empty, property->type,
					ILQualIdentSimple(ILInternString("value", 5).string)));
		decl = (ILNode_MethodDeclaration *)(property->setAccessor);
		if(!decl)
		{
			/* Abstract interface definition */
			decl = (ILNode_MethodDeclaration *)
				ILNode_MethodDeclaration_create
						(0, property->modifiers, 0, name, params, 0);
			property->setAccessor = (ILNode *)decl;
		}
		else
		{
			/* Regular class definition */
			decl->modifiers = property->modifiers;
			decl->type = 0;
			decl->name = name;
			decl->params = params;
		}
	}
}

/*
 * Create the methods needed by an event declarator.
 */
static void CreateEventDeclMethods(ILNode_EventDeclaration *event,
								   ILNode_EventDeclarator *decl)
{
	ILNode_MethodDeclaration *method;
	ILNode *eventName;
	ILNode *name;
	ILNode *addParams;
	ILNode *removeParams;

	/* Get the name of the event */
	eventName = ((ILNode_FieldDeclarator *)(decl->fieldDeclarator))->name;

	/* Create the parameter information for the "add" and "remove" methods */
	addParams = ILNode_List_create();
	ILNode_List_Add(addParams,
		ILNode_FormalParameter_create(0, ILParamMod_empty, event->type,
				ILQualIdentSimple(ILInternString("value", 5).string)));
	removeParams = ILNode_List_create();
	ILNode_List_Add(removeParams,
		ILNode_FormalParameter_create(0, ILParamMod_empty, event->type,
				ILQualIdentSimple(ILInternString("value", 5).string)));

	/* Create the "add" method */
	name = AdjustPropertyName(eventName, "add_");
	method = (ILNode_MethodDeclaration *)(decl->addAccessor);
	if(!method && event->needFields)
	{
		/* Field-based event that needs a pre-defined body */
		method = (ILNode_MethodDeclaration *)
			ILNode_MethodDeclaration_create
					(0, event->modifiers, 0, name, addParams, 0);
		method->body = ILNode_NewScope_create
							(ILNode_AssignAdd_create
								(ILNode_Add_create(eventName, 
									ILQualIdentSimple
										(ILInternString("value", 5).string))));
		decl->addAccessor = (ILNode *)method;
	}
	else if(!method)
	{
		/* Abstract interface definition */
		method = (ILNode_MethodDeclaration *)
			ILNode_MethodDeclaration_create
					(0, event->modifiers, 0, name, addParams, 0);
		decl->addAccessor = (ILNode *)method;
	}
	else
	{
		/* Regular class definition */
		method->modifiers = event->modifiers;
		method->type = 0;
		method->name = name;
		method->params = addParams;
	}
	method->modifiers |= IL_META_METHODDEF_SPECIAL_NAME;

	/* Create the "remove" method */
	name = AdjustPropertyName(eventName, "remove_");
	method = (ILNode_MethodDeclaration *)(decl->removeAccessor);
	if(!method && event->needFields)
	{
		/* Field-based event that needs a pre-defined body */
		method = (ILNode_MethodDeclaration *)
			ILNode_MethodDeclaration_create
					(0, event->modifiers, 0, name, removeParams, 0);
		method->body = ILNode_NewScope_create
							(ILNode_AssignSub_create
								(ILNode_Sub_create(eventName, 
									ILQualIdentSimple
										(ILInternString("value", 5).string))));
		decl->removeAccessor = (ILNode *)method;
	}
	else if(!method)
	{
		/* Abstract interface definition */
		method = (ILNode_MethodDeclaration *)
			ILNode_MethodDeclaration_create
					(0, event->modifiers, 0, name, removeParams, 0);
		decl->removeAccessor = (ILNode *)method;
	}
	else
	{
		/* Regular class definition */
		method->modifiers = event->modifiers;
		method->type = 0;
		method->name = name;
		method->params = removeParams;
	}
	method->modifiers |= IL_META_METHODDEF_SPECIAL_NAME;
}

/*
 * Create the methods needed by an event definition.
 */
static void CreateEventMethods(ILNode_EventDeclaration *event)
{
	ILNode_ListIter iter;
	ILNode *decl;

	if(yyisa(event->eventDeclarators, ILNode_EventDeclarator))
	{
		/* A single declarator indicates a property-style event */
		event->needFields = 0;

		/* Create the methods for the event declarator */
		CreateEventDeclMethods
			(event, (ILNode_EventDeclarator *)(event->eventDeclarators));
	}
	else
	{
		/* A list of declarators indicates a field-style event */
		event->needFields =
			((event->modifiers & IL_META_METHODDEF_ABSTRACT) == 0);

		/* Scan the list and create the methods that we require */
		ILNode_ListIter_Init(&iter, event->eventDeclarators);
		while((decl = ILNode_ListIter_Next(&iter)) != 0)
		{
			CreateEventDeclMethods(event, (ILNode_EventDeclarator *)decl);
		}
	}
}

%}

/*
 * Define the structure of yylval.
 */
%union {
	struct
	{
		ILUInt64	value;
		int			type;
		int			canneg;
	}					integer;
	struct
	{
		ILDouble	value;
		int			type;
	}					real;
	ILDecimal  			decimal;
	ILUInt16			charValue;
	ILIntString			string;
	char			   *name;
	ILUInt32			count;
	ILUInt32			mask;
	ILNode			   *node;
	struct
	{
		ILNode		   *type;
		char		   *id;
		ILNode         *idNode;
	} catchinfo;
	struct
	{
		ILNode		   *item1;
		ILNode		   *item2;
	} pair;
	ILParameterModifier	pmod;
	struct
	{
		char           *binary;
		char           *unary;

	} opName;
	struct
	{
		ILNode		   *type;
		ILNode		   *ident;
		ILNode		   *params;

	} indexer;
	struct
	{
		ILNode		   *decl;
		ILNode		   *init;

	} varInit;
	struct
	{
		ILNode		   *body;
		ILNode		   *staticCtors;

	} member;
	struct
	{
		ILAttrTargetType targetType;
		ILNode		   *target;

	} target;
	int					partial;
}

/*
 * Primitive lexical tokens.
 */
%token INTEGER_CONSTANT		"an integer value"
%token CHAR_CONSTANT		"a character constant"
%token IDENTIFIER_LEXICAL	"an identifier"
%token STRING_LITERAL		"a string literal"
%token FLOAT_CONSTANT		"a floating point value"
%token DECIMAL_CONSTANT		"a decimal value"
%token DOC_COMMENT			"a documentation comment"

/*
 * Keywords.
 */
%token ABSTRACT				"`abstract'"
%token ADD					"`add'"
%token ARGLIST				"`__arglist'"
%token AS					"`as'"
%token BASE					"`base'"
%token BOOL					"`bool'"
%token BREAK				"`break'"
%token BUILTIN_CONSTANT		"`__builtin_constant'"
%token BYTE					"`byte'"
%token CASE					"`case'"
%token CATCH				"`catch'"
%token CHAR					"`char'"
%token CHECKED				"`checked'"
%token CLASS				"`class'"
%token CONST				"`const'"
%token CONTINUE				"`continue'"
%token DECIMAL				"`decimal'"
%token DEFAULT				"`default'"
%token DELEGATE				"`delegate'"
%token DO					"`do'"
%token DOUBLE				"`double'"
%token ELSE					"`else'"
%token ENUM					"`enum'"
%token EVENT				"`event'"
%token EXPLICIT				"`explicit'"
%token EXTERN				"`extern'"
%token FALSE				"`false'"
%token FINALLY				"`finally'"
%token FIXED				"`fixed'"
%token FLOAT				"`float'"
%token FOR					"`for'"
%token FOREACH				"`foreach'"
%token GET					"`get'"
%token GOTO					"`goto'"
%token IF					"`if'"
%token IMPLICIT				"`implicit'"
%token IN					"`in'"
%token INT					"`int'"
%token INTERFACE			"`interface'"
%token INTERNAL				"`internal'"
%token IS					"`is'"
%token LOCK					"`lock'"
%token LONG					"`long'"
%token LONG_DOUBLE			"`__long_double'"
%token MAKEREF				"`__makeref'"
%token MODULE               "`__module'"
%token NAMESPACE			"`namespace'"
%token NEW					"`new'"
%token NULL_TOK				"`null'"
%token OBJECT				"`object'"
%token OPERATOR				"`operator'"
%token OUT					"`out'"
%token OVERRIDE				"`override'"
%token PARAMS				"`params'"
%token PARTIAL				"`partial'"
%token PRIVATE				"`private'"
%token PROTECTED			"`protected'"
%token PUBLIC				"`public'"
%token READONLY				"`readonly'"
%token REMOVE				"`remove'"
%token REF					"`ref'"
%token REFTYPE				"`__reftype'"
%token REFVALUE				"`__refvalue'"
%token RETURN				"`return'"
%token SBYTE				"`sbyte'"
%token SEALED				"`sealed'"
%token SET					"`set'"
%token SHORT				"`short'"
%token SIZEOF				"`sizeof'"
%token STACKALLOC			"`stackalloc'"
%token STATIC				"`static'"
%token STRING				"`string'"
%token STRUCT				"`struct'"
%token SWITCH				"`switch'"
%token THIS					"`this'"
%token THROW				"`throw'"
%token TRUE					"`true'"
%token TRY					"`try'"
%token TYPEOF				"`typeof'"
%token UINT					"`uint'"
%token ULONG				"`ulong'"
%token UNCHECKED			"`unchecked'"
%token UNSAFE				"`unsafe'"
%token USHORT				"`ushort'"
%token USING				"`using'"
%token VIRTUAL				"`virtual'"
%token VOID					"`void'"
%token VOLATILE				"`volatile'"
%token WHERE				"`where'"
%token WHILE				"`while'"
%token YIELD				"`yield'"

/*
 * Operators.
 */
%token INC_OP				"`++'"
%token DEC_OP				"`--'"
%token LEFT_OP				"`<<'"
%token RIGHT_OP				"`>>'"
%token LE_OP				"`<='"
%token GE_OP				"`>='"
%token EQ_OP				"`=='"
%token NE_OP				"`!='"
%token AND_OP				"`&&'"
%token OR_OP				"`||'"
%token MUL_ASSIGN_OP		"`*='"
%token DIV_ASSIGN_OP		"`/='"
%token MOD_ASSIGN_OP		"`%='"
%token ADD_ASSIGN_OP		"`+='"
%token SUB_ASSIGN_OP		"`-='"
%token LEFT_ASSIGN_OP		"`<<='"
%token RIGHT_ASSIGN_OP		"`>>='"
%token AND_ASSIGN_OP		"`&='"
%token XOR_ASSIGN_OP		"`^='"
%token OR_ASSIGN_OP			"`|='"
%token PTR_OP				"`->'"
%token GENERIC_LT			"`<'"

/*
 * Define the yylval types of the various non-terminals.
 */
%type <name>		IDENTIFIER IDENTIFIER_LEXICAL
%type <integer>		INTEGER_CONSTANT
%type <charValue>	CHAR_CONSTANT
%type <real>		FLOAT_CONSTANT
%type <decimal>		DECIMAL_CONSTANT
%type <string>		STRING_LITERAL DOC_COMMENT NamespaceIdentifier
%type <count>		DimensionSeparators DimensionSeparatorList
%type <mask>		OptModifiers Modifiers Modifier
%type <partial>		OptPartial

%type <node>		Identifier QualifiedIdentifier BuiltinType
%type <node>		QualifiedIdentifierPart

%type <node>		Type NonExpressionType LocalVariableType

%type <node>		TypeDeclaration ClassDeclaration ClassBase TypeList
%type <member>		ClassBody OptClassMemberDeclarations
%type <member>		ClassMemberDeclarations ClassMemberDeclaration
%type <member>		StructBody
%type <node> 		StructDeclaration StructInterfaces ModuleDeclaration

%type <node>		PrimaryExpression UnaryExpression Expression
%type <node>		MultiplicativeExpression AdditiveExpression
%type <node>		ShiftExpression RelationalExpression EqualityExpression
%type <node>		AndExpression XorExpression OrExpression
%type <node>		LogicalAndExpression LogicalOrExpression
%type <node>		ConditionalExpression AssignmentExpression
%type <node>		ParenExpression ConstantExpression BooleanExpression
%type <node>		ParenBooleanExpression LiteralExpression
%type <node>		InvocationExpression ExpressionList
%type <node>		ObjectCreationExpression OptArgumentList ArgumentList
%type <node>		Argument PrefixedUnaryExpression GenericReference
%type <node>		AnonymousMethod

%type <node>		Statement EmbeddedStatement Block OptStatementList
%type <node>		StatementList ExpressionStatement SelectionStatement
%type <node>		SwitchBlock OptSwitchSections SwitchSections
%type <node>		SwitchSection SwitchLabels SwitchLabel IterationStatement
%type <node>		ForInitializer ForInitializerInner ForCondition
%type <node>		ForIterator ForeachExpression ExpressionStatementList
%type <node>		JumpStatement TryStatement CatchClauses LineStatement
%type <node>		OptSpecificCatchClauses SpecificCatchClauses
%type <node>		SpecificCatchClause OptGeneralCatchClause
%type <node>		GeneralCatchClause FinallyClause LockStatement
%type <node>		UsingStatement ResourceAcquisition FixedStatement
%type <node>		FixedPointerDeclarators FixedPointerDeclarator
%type <node>		InnerEmbeddedStatement InnerExpressionStatement
%type <node>		YieldStatement

%type <node>		ConstantDeclaration ConstantDeclarators ConstantDeclarator
%type <node>		FieldDeclaration FieldDeclarators FieldDeclarator
%type <varInit>		VariableDeclarators VariableDeclarator
%type <node>		VariableInitializer LocalVariableDeclaration
%type <node>		LocalConstantDeclaration
%type <node>		EventFieldDeclaration EventDeclaration 
%type <node>		EventPropertyDeclaration EventDeclarators EventDeclarator
%type <pair>		EventAccessorBlock EventAccessorDeclarations

%type <node>		MethodDeclaration MethodBody
%type <node>		OptFormalParameterList FormalParameterList FormalParameter
%type <pmod>		ParameterModifier
%type <node>		PropertyDeclaration 
%type <pair>		AccessorBlock AccessorDeclarations
%type <node>		OptGetAccessorDeclaration GetAccessorDeclaration
%type <node>		OptSetAccessorDeclaration SetAccessorDeclaration
%type <node>		AccessorBody
%type <node>		AddAccessorDeclaration RemoveAccessorDeclaration
%type <node>		IndexerDeclaration
%type <node>		FormalIndexParameters FormalIndexParameter
%type <node>		FormalIndexParameterList
%type <node>		InterfaceDeclaration InterfaceBase InterfaceBody
%type <node>		OptInterfaceMemberDeclarations /*InterfaceMemberDeclarations
%type <node>		InterfaceMemberDeclarations InterfaceMemberDeclaration*/
%type <node>		InterfaceMemberDeclaration InterfaceMemberDeclarations
%type <node>		InterfaceMethodDeclaration InterfacePropertyDeclaration
%type <node>		InterfaceIndexerDeclaration InterfaceEventDeclaration
%type <mask>		InterfaceAccessors OptNew InterfaceAccessorBody
%type <node>		EnumDeclaration EnumBody OptEnumMemberDeclarations
%type <node>		EnumMemberDeclarations EnumMemberDeclaration
%type <node>		EnumBase EnumBaseType
%type <node>		DelegateDeclaration ConstructorInitializer
%type <member>		ConstructorDeclaration
%type <node>		DestructorDeclaration
%type <node>		OperatorDeclaration NormalOperatorDeclaration
%type <node>		ConversionOperatorDeclaration
%type <opName>		OverloadableOperator
%type <node>		TypeSuffix TypeSuffixList TypeSuffixes
%type <node>		OptAttributes AttributeSections AttributeSection
%type <node>		AttributeList Attribute AttributeArguments
%type <node>		NonOptAttributes
%type <node>		PositionalArgumentList PositionalArgument NamedArgumentList
%type <node>		NamedArgument AttributeArgumentExpression
%type <node>		RankSpecifiers RankSpecifierList 
%type <node>		OptArrayInitializer ArrayInitializer
%type <node>		OptVariableInitializerList VariableInitializerList
%type <node>		TypeActuals TypeFormals TypeFormalList
%type <indexer>		IndexerDeclarator
%type <catchinfo>	CatchNameInfo
%type <target>		AttributeTarget

%expect 35

%start CompilationUnit
%%

/*
 * Outer level of the C# input file.
 */

CompilationUnit
	: /* empty */	{
				/* The input file is empty */
				CCTypedWarning("-empty-input",
							   "file contains no declarations");
				ResetState();
			}
	| OuterDeclarationsRecoverable		{
				/* Check for empty input and finalize the parse */
				if(!HaveDecls)
				{
					CCTypedWarning("-empty-input",
								   "file contains no declarations");
				}
				ResetState();
			}
	| OuterDeclarationsRecoverable NonOptAttributes	{
				/* A file that contains declarations and assembly attributes */
				if($2)
				{
					InitGlobalNamespace();
					CCPluginAddStandaloneAttrs
						(ILNode_StandaloneAttr_create
							((ILNode*)CurrNamespaceNode, $2));
				}
				ResetState();
			}
	| NonOptAttributes	{
				/* A file that contains only assembly attributes */
				if($1)
				{
					InitGlobalNamespace();
					CCPluginAddStandaloneAttrs
						(ILNode_StandaloneAttr_create
							((ILNode*)CurrNamespaceNode, $1));
				}
				ResetState();
			}
	;

/*
 * Note: strictly speaking, declarations should be ordered so
 * that using declarations always come before namespace members.
 * We have relaxed this to make error recovery easier.
 */
OuterDeclarations
	: OuterDeclaration
	| OuterDeclarations OuterDeclaration
	;

OuterDeclaration
	: UsingDirective
	| NamespaceMemberDeclaration
	| error			{
				/*
				 * This production recovers from errors at the outer level
				 * by skipping invalid tokens until a namespace, using,
				 * type declaration, or attribute, is encountered.
				 */
			#ifdef YYEOF
				while(yychar != YYEOF)
			#else
				while(yychar >= 0)
			#endif
				{
					if(yychar == NAMESPACE || yychar == USING ||
					   yychar == PUBLIC || yychar == INTERNAL ||
					   yychar == UNSAFE || yychar == SEALED ||
					   yychar == ABSTRACT || yychar == CLASS ||
					   yychar == STRUCT || yychar == DELEGATE ||
					   yychar == ENUM || yychar == INTERFACE ||
					   yychar == '[')
					{
						/* This token starts a new outer-level declaration */
						break;
					}
					else if(yychar == '}' && CurrNamespace.len != 0)
					{
						/* Probably the end of the enclosing namespace */
						break;
					}
					else if(yychar == ';')
					{
						/* Probably the end of an outer-level declaration,
						   so restart the parser on the next token */
						yychar = YYLEX;
						break;
					}
					yychar = YYLEX;
				}
			#ifdef YYEOF
				if(yychar != YYEOF)
			#else
				if(yychar >= 0)
			#endif
				{
					yyerrok;
				}
				NestingLevel = 0;
			}
	;

OuterDeclarationsRecoverable
	: OuterDeclarationRecoverable
	| OuterDeclarationsRecoverable OuterDeclarationRecoverable
	;

OuterDeclarationRecoverable
	: OuterDeclaration
	| '}'				{
				/* Recover from our educated guess that we were at the
				   end of a namespace scope in the error processing code
				   for '}' above.  If the programmer wrote "namespace XXX }"
				   instead of "namespace { XXX }", this code will stop the
				   error processing logic from looping indefinitely */
				if(CurrNamespace.len == 0)
				{
					CCError(_("parse error at or near `}'"));
				}
				else
				{
					CurrNamespace = ILInternString("", 0);
				}
			}
	;

/*
 * Identifiers.
 */

Identifier
	: IDENTIFIER		{
				/* Build an undistinguished identifier node.  At this
				   point, we have no idea of the identifier's type.
				   We leave that up to the semantic analysis phase */
				$$ = ILQualIdentSimple($1);
			}
	;

IDENTIFIER
	: IDENTIFIER_LEXICAL	{ $$ = $1; }
	| GET					{ $$ = ILInternString("get", 3).string; }
	| SET					{ $$ = ILInternString("set", 3).string; }
	| ADD					{ $$ = ILInternString("add", 3).string; }
	| REMOVE				{ $$ = ILInternString("remove", 6).string; }
	| WHERE					{ $$ = ILInternString("where", 5).string; }
	| PARTIAL				{ $$ = ILInternString("partial", 7).string; }
	| YIELD					{ $$ = ILInternString("yield", 5).string; }
	;

QualifiedIdentifier
	: QualifiedIdentifierPart							{ $$ = $1; }
	| QualifiedIdentifier '.' QualifiedIdentifierPart	{
				MakeBinary(QualIdent, $1, $3);
			}
	;

QualifiedIdentifierPart
	: Identifier							{ $$ = $1; }
	| Identifier '<' TypeActuals '>'		{
				MakeBinary(GenericReference, $1, $3);
			}
	| Identifier GENERIC_LT TypeActuals '>'		{
				MakeBinary(GenericReference, $1, $3);
			}
	;

/*
 * Namespaces.
 */

/*
 * Note: strictly speaking, namespaces don't have attributes.
 * The C# standard allows attributes that apply to the assembly
 * to appear at the global level.  To avoid reduce/reduce conflicts
 * in the grammar, we cannot make the attributes a separate rule
 * at the outer level.  So, we parse assembly attributes as if
 * they were attached to types or namespaces, and detach them
 * during semantic analysis.
 */
NamespaceDeclaration
	: OptAttributes NAMESPACE NamespaceIdentifier {
				int posn, len;
				ILScope *oldLocalScope;
				posn = 0;
				if($1)
				{
					InitGlobalNamespace();
					CCPluginAddStandaloneAttrs
						(ILNode_StandaloneAttr_create
							((ILNode*)CurrNamespaceNode, $1));
				}
				while(posn < $3.len)
				{
					/* Extract the next identifier */
					if($3.string[posn] == '.')
					{
						++posn;
						continue;
					}
					len = 0;
					while((posn + len) < $3.len &&
						  $3.string[posn + len] != '.')
					{
						++len;
					}

					/* Push a new identifier onto the end of the namespace */
					if(CurrNamespace.len != 0)
					{
						CurrNamespace = ILInternAppendedString
							(CurrNamespace,
							 ILInternAppendedString
							 	(ILInternString(".", 1),
								 ILInternString($3.string + posn, len)));
					}
					else
					{
						CurrNamespace = ILInternString($3.string + posn, len);
					}

					/* Create the namespace node */
					InitGlobalNamespace();
					
					oldLocalScope=LocalScope();
					
					CurrNamespaceNode = (ILNode_Namespace *)
						ILNode_Namespace_create(CurrNamespace.string,
												CurrNamespaceNode);

					/* Preserve compilation unit specific local scopes 
					 * or maybe I need to create a new scope as child of
					 * this scope (fix when I find a test case) */
					CurrNamespaceNode->localScope=oldLocalScope;

					/* Declare the namespace within the global scope */
					ILScopeDeclareNamespace(GlobalScope(),
											CurrNamespace.string);

					/* Move on to the next namespace component */
					posn += len;
				}
			}
			NamespaceBody OptSemiColon	{
				/* Pop the identifier from the end of the namespace */
				if(CurrNamespace.len == $3.len)
				{
					CurrNamespace = ILInternString("", 0);
					while(CurrNamespaceNode->enclosing != 0)
					{
						CurrNamespaceNode = CurrNamespaceNode->enclosing;
					}
				}
				else
				{
					CurrNamespace = ILInternString
						(CurrNamespace.string, CurrNamespace.len - $3.len - 1);
					while(CurrNamespaceNode->name != CurrNamespace.string)
					{
						CurrNamespaceNode = CurrNamespaceNode->enclosing;
					}
				}
			}
	;

NamespaceIdentifier
	: IDENTIFIER		{ $$ = ILInternString($1, strlen($1)); }
	| NamespaceIdentifier '.' IDENTIFIER	{
				$$ = ILInternAppendedString
					($1, ILInternAppendedString
					 		(ILInternString(".", 1),
							 ILInternString($3, strlen($3))));
			}
	;

OptSemiColon
	: /* empty */
	| ';'
	;

NamespaceBody
	: '{' OptNamespaceMemberDeclarations '}'
	;

UsingDirective
	: USING IDENTIFIER '=' QualifiedIdentifier ';'	{
				ILScope *globalScope = GlobalScope();
				ILScope *scope = LocalScope();
				ILNode *alias;
				if(ILScopeLookup(globalScope, $2, 1))
				{
					CCError("`%s' is already declared", $2);
				}
				else if(ILScopeLookup(localScope, $2, 1))
				{
					CCError("`%s' is already declared", $2);
				}
				alias = ILNode_UsingAlias_create($2, ILQualIdentName($4,0));
				/* NOTE: CSSemGuard is not needed as ILNode_UsingAlias is
				         never Semanalyzed */
				InitGlobalNamespace();
				ILScopeDeclareAlias(scope, $2,alias,$4);
				CurrNamespaceNode->localScope=scope;
			}
	| USING NamespaceIdentifier ';'		{
				ILScope *globalScope = GlobalScope();
				ILNode_UsingNamespace *using;
				if(!ILScopeUsing(globalScope, $2.string))
				{
					CCError("`%s' is not a namespace", $2.string);
				}
				InitGlobalNamespace();
				if(!HaveUsingNamespace($2.string))
				{
					using = (ILNode_UsingNamespace *)
						ILNode_UsingNamespace_create($2.string);
					using->next = CurrNamespaceNode->using;
					CurrNamespaceNode->using = using;
				}
			}
	;

OptNamespaceMemberDeclarations
	: /* empty */
	| OuterDeclarations
	;

NamespaceMemberDeclaration
	: NamespaceDeclaration
	| TypeDeclaration			{ CCPluginAddTopLevel($1); }
	;

TypeDeclaration
	: ClassDeclaration			{ $$ = $1; }
	| ModuleDeclaration			{ $$ = $1; }
	| StructDeclaration			{ $$ = $1; }
	| InterfaceDeclaration		{ $$ = $1; }
	| EnumDeclaration			{ $$ = $1; }
	| DelegateDeclaration		{ $$ = $1; }
	;

/*
 * Types.
 */

Type
	: QualifiedIdentifier	{ $$ = $1; }
	| BuiltinType			{ $$ = $1; }
	| Type '[' DimensionSeparators ']'	{
				MakeBinary(ArrayType, $1, $3);
			}
	| Type '*'			{
				MakeUnary(PtrType, $1);
			}
	| Type '<' TypeActuals '>'	{
				MakeBinary(GenericReference, $1, $3);
			}
	| Type GENERIC_LT TypeActuals '>'	{
				MakeBinary(GenericReference, $1, $3);
			}
	;

NonExpressionType
	: BuiltinType			{ $$ = $1; }
	| NonExpressionType '[' DimensionSeparators ']'	{
				MakeBinary(ArrayType, $1, $3);
			}
	| NonExpressionType '*'	{
				MakeUnary(PtrType, $1);
			}
	| Expression '*'		{ 
				MakeUnary(PtrType, $1);
			}
	| MultiplicativeExpression '*'		{ 
				/* Needed becuase of shift issues that won't be picked
				   up by the "Expression *" case above */
				MakeUnary(PtrType, $1);
			}
	| NonExpressionType '<' TypeActuals '>'	{
				MakeBinary(GenericReference, $1, $3);
			}
	| NonExpressionType GENERIC_LT TypeActuals '>'	{
				MakeBinary(GenericReference, $1, $3);
			}
	;

TypeActuals
	: Type						{ $$ = $1; }
	| TypeActuals ',' Type		{ MakeBinary(TypeActuals, $1, $3); }
	;

/*
 * Types in local variable declarations must be recognized as
 * expressions to prevent reduce/reduce errors in the grammar.
 * The expressions are converted into types during semantic analysis.
 */
LocalVariableType
	: PrimaryExpression TypeSuffixes	{
				MakeBinary(LocalVariableType, $1, $2);
			}
	| PrimaryExpression '<' TypeActuals '>' TypeSuffixes	{
				ILNode *type = ILNode_GenericReference_create($1, $3);
				MakeBinary(LocalVariableType, type, $5);
			}
	| PrimaryExpression GENERIC_LT TypeActuals '>' TypeSuffixes	{
				ILNode *type = ILNode_GenericReference_create($1, $3);
				MakeBinary(LocalVariableType, type, $5);
			}
	| BuiltinType TypeSuffixes			{
				MakeBinary(LocalVariableType, $1, $2);
			}
	;

TypeSuffixes
	: /* empty */		{ $$ = 0; }
	| TypeSuffixList	{ $$ = $1; }
	;

TypeSuffixList
	: TypeSuffix						{
				$$ = ILNode_List_create();
				ILNode_List_Add($$, $1);
			}
	| TypeSuffixList TypeSuffix			{
				ILNode_List_Add($1, $2);
				$$ = $1;
			}
	;

TypeSuffix
	: '[' DimensionSeparators ']'	{ MakeUnary(TypeSuffix, $2); }
	| '*'							{ MakeUnary(TypeSuffix, 0); }
	;

DimensionSeparators
	: /* empty */					{ $$ = 1; }
	| DimensionSeparatorList		{ $$ = $1; }
	;

DimensionSeparatorList
	: ','							{ $$ = 2; }
	| DimensionSeparatorList ','	{ $$ = $1 + 1; }
	;

/*
 * The C# standard does not have "void" here.  It handles void
 * types elsewhere in the grammar.  However, the grammar is a lot
 * simpler if we make "void" a builtin type and then filter it
 * out later in semantic analysis.
 */
BuiltinType
	: VOID			{ MakeUnary(PrimitiveType, IL_META_ELEMTYPE_VOID); }
	| BOOL			{ MakeUnary(PrimitiveType, IL_META_ELEMTYPE_BOOLEAN); }
	| SBYTE			{ MakeUnary(PrimitiveType, IL_META_ELEMTYPE_I1); }
	| BYTE			{ MakeUnary(PrimitiveType, IL_META_ELEMTYPE_U1); }
	| SHORT			{ MakeUnary(PrimitiveType, IL_META_ELEMTYPE_I2); }
	| USHORT		{ MakeUnary(PrimitiveType, IL_META_ELEMTYPE_U2); }
	| INT			{ MakeUnary(PrimitiveType, IL_META_ELEMTYPE_I4); }
	| UINT			{ MakeUnary(PrimitiveType, IL_META_ELEMTYPE_U4); }
	| LONG			{ MakeUnary(PrimitiveType, IL_META_ELEMTYPE_I8); }
	| ULONG			{ MakeUnary(PrimitiveType, IL_META_ELEMTYPE_U8); }
	| CHAR			{ MakeUnary(PrimitiveType, IL_META_ELEMTYPE_CHAR); }
	| FLOAT			{ MakeUnary(PrimitiveType, IL_META_ELEMTYPE_R4); }
	| DOUBLE		{ MakeUnary(PrimitiveType, IL_META_ELEMTYPE_R8); }
	| LONG_DOUBLE	{ MakeUnary(PrimitiveType, IL_META_ELEMTYPE_R); }
	| DECIMAL		{ MakeUnary(SystemType,"Decimal"); }
	| OBJECT		{ MakeUnary(SystemType,"Object"); }
	| STRING		{ MakeUnary(SystemType,"String"); }
	;

/*
 * Expressions.
 */

PrimaryExpression
	: LiteralExpression				{ $$ = $1; }
	| Identifier					{ $$ = $1; }
	| '(' Expression ')'			{ $$ = $2; }
	| PrimaryExpression '.' Identifier	{ MakeBinary(MemberAccess, $1, $3); }
	| BuiltinType '.' Identifier	{ MakeBinary(MemberAccess, $1, $3); }
	| InvocationExpression			{ $$ = $1; }
	| PrimaryExpression '[' ExpressionList ']'	{
				MakeBinary(ArrayAccess, $1, $3);
			}
	| PrimaryExpression '[' ']'		{
				/*
				 * This is actually a type, but we have to recognise
				 * it here to avoid shift/reduce conflicts in the
				 * definition of casts in UnaryExpression.  We would
				 * like to handle this in NonExpressionType, but then it
				 * creates problems for casts to array types like "A[]".
				 */
				MakeBinary(ArrayType, $1, 1);
			}
	| PrimaryExpression '[' DimensionSeparatorList ']'		{
				/* This is also a type */
				MakeBinary(ArrayType, $1, $3);
			}
	| ARGLIST						{ MakeSimple(VarArgList); }
	| THIS							{ MakeSimple(This); }
	| BASE '.' Identifier			{ MakeUnary(BaseAccess, $3); }
	| BASE '[' ExpressionList ']'	{ MakeUnary(BaseElement, $3); }
	| PrimaryExpression INC_OP		{ MakeUnary(PostInc, $1); }
	| PrimaryExpression DEC_OP		{ MakeUnary(PostDec, $1); }
	| ObjectCreationExpression		{ $$ = $1; }
	| NEW Type '[' ExpressionList ']' RankSpecifiers OptArrayInitializer	{
				$$ = ILNode_NewExpression_create($2, $4, $6, $7);
			}
	| NEW Type ArrayInitializer		{
				$$ = ILNode_NewExpression_create($2, 0, 0, $3);
			}
	| TYPEOF '(' Type ')'			{ MakeUnary(TypeOf, $3); }
	| SIZEOF '(' Type ')'			{
				/*
				 * This is only safe if it is used on one of the following
				 * builtin types: sbyte, byte, short, ushort, int, uint,
				 * long, ulong, float, double, char, bool.  We leave the
				 * check to the semantic analysis phase.
				 */
				MakeUnary(SizeOf, $3);
			}
	| CHECKED '(' Expression ')'	{ MakeUnary(Overflow, $3); }
	| UNCHECKED '(' Expression ')'	{ MakeUnary(NoOverflow, $3); }
	| PrimaryExpression PTR_OP Identifier	{
				MakeBinary(DerefField, $1, $3);
			}
	| STACKALLOC Type '[' Expression ']'	{
				MakeBinary(StackAlloc, $2, $4);
			}
	| BUILTIN_CONSTANT '(' STRING_LITERAL ')'	{
				/*
				 * Get the value of a builtin constant.
				 */
				$$ = CSBuiltinConstant($3.string);
			}
	| MAKEREF '(' Expression ')'			{ MakeUnary(MakeRefAny, $3); }
	| REFTYPE '(' Expression ')'			{ MakeUnary(RefType, $3); }
	| REFVALUE '(' Expression ',' Type ')'	{ MakeBinary(RefValue, $3, $5); }
	| MODULE			{ $$ = ILQualIdentSimple("<Module>"); }
	| DELEGATE AnonymousMethod				{ $$ = $2; }
	| PrimaryExpression '.' DEFAULT			{
				$$ = ILNode_DefaultConstructor_create($1, 0, 0);
			}
	| BuiltinType '.' DEFAULT			{
				$$ = ILNode_DefaultConstructor_create($1, 0, 0);
			}
	;

LiteralExpression
	: TRUE						{ MakeSimple(True); }
	| FALSE						{ MakeSimple(False); }
	| NULL_TOK					{ MakeSimple(Null); }
	| INTEGER_CONSTANT			{
				switch($1.type)
				{
					case CS_NUMTYPE_INT32:
					{
						$$ = ILNode_Int32_create($1.value, 0, $1.canneg);
					}
					break;

					case CS_NUMTYPE_UINT32:
					{
						$$ = ILNode_UInt32_create($1.value, 0, $1.canneg);
					}
					break;

					case CS_NUMTYPE_INT64:
					{
						$$ = ILNode_Int64_create($1.value, 0, $1.canneg);
					}
					break;

					default:
					{
						$$ = ILNode_UInt64_create($1.value, 0, $1.canneg);
					}
					break;
				}
			}
	| FLOAT_CONSTANT			{
				if($1.type == CS_NUMTYPE_FLOAT32)
				{
					$$ = ILNode_Float32_create($1.value);
				}
				else
				{
					$$ = ILNode_Float64_create($1.value);
				}
			}
	| DECIMAL_CONSTANT			{
				$$ = ILNode_Decimal_create($1);
			}
	| CHAR_CONSTANT				{
				$$ = ILNode_Char_create((ILUInt64)($1), 0, 1);
			}
	| STRING_LITERAL			{
				$$ = ILNode_String_create($1.string, $1.len);
			}
	;

InvocationExpression
	: PrimaryExpression '(' OptArgumentList ')'		{ 
				/* Check for "__arglist", which is handled specially */
				if(!yyisa($1, ILNode_VarArgList))
				{
					MakeBinary(InvocationExpression, $1, $3); 
				}
				else
				{
					MakeUnary(VarArgExpand, $3); 
				}
			}
	;

ObjectCreationExpression
	: NEW Type '(' OptArgumentList ')'	{ 
				MakeBinary(ObjectCreationExpression, $2, $4); 
			}
	;

OptArgumentList
	: /* empty */						{ $$ = 0; }
	| ArgumentList						{ $$ = $1; }
	;

ArgumentList
	: Argument							{ $$ = $1; }
	| ArgumentList ',' Argument			{ MakeBinary(ArgList, $1, $3); }
	;

Argument
	: Expression			{ MakeBinary(Argument, ILParamMod_empty, $1); }
	| OUT Expression		{ MakeBinary(Argument, ILParamMod_out, $2); }
	| REF Expression		{ MakeBinary(Argument, ILParamMod_ref, $2); }
	;

ExpressionList
	: Expression						{ $$ = $1; }
	| ExpressionList ',' Expression		{ MakeBinary(ArgList, $1, $3); }
	;

RankSpecifiers
	: /* empty */			{ $$ = 0;}
	| RankSpecifierList		{ $$ = $1;}
	;

RankSpecifierList
	: '[' DimensionSeparators ']'			{
					$$ = ILNode_List_create();
					ILNode_List_Add($$, ILNode_TypeSuffix_create($2));
				}
	| RankSpecifierList '[' DimensionSeparators ']'	{
					ILNode_List_Add($1, ILNode_TypeSuffix_create($3));
					$$ = $1;
				}
	;

/*
 * There is a slight ambiguity in the obvious definition of
 * UnaryExpression that creates shift/reduce conflicts when
 * casts are employed.  For example, the parser cannot tell
 * the diference between the following two cases:
 *
 *		(Expr1) - Expr2		-- parse as subtraction.
 *		(Type) -Expr		-- parse as negation and cast.
 *
 * Splitting the definition into two parts fixes the conflict.
 * It is not possible to use one of the operators '-', '+',
 * '*', '&', '++', or '--' after a cast type unless parentheses
 * are involved:
 *
 *		(Type)(-Expr)
 *
 * As a special exception, if the cast involves a builtin type
 * name such as "int", "double", "bool", etc, then the prefix
 * operators can be used.  i.e. the following will be parsed
 * correctly:
 *
 *		(int)-Expr
 *
 * whereas the following requires parentheses because "System"
 * may have been redeclared with a new meaning in a local scope:
 *
 *		(System.Int32)(-Expr)
 *
 * It is very difficult to resolve this in any other way because
 * the compiler does not know if an identifier is a type or not
 * until later.
 */
UnaryExpression
	: PrimaryExpression					{ $$ = $1; }
	| '!' PrefixedUnaryExpression		{ 
				MakeUnary(LogicalNot,ILNode_ToBool_create($2)); 
	}
	| '~' PrefixedUnaryExpression		{ MakeUnary(Not, $2); }
	| '(' Expression ')' UnaryExpression	{
				/*
				 * Note: we need to use a full "Expression" for the type,
				 * so that we don't get a reduce/reduce conflict with the
				 * rule "PrimaryExpression: '(' Expression ')'".  We later
				 * filter out expressions that aren't really types.
				 */
				MakeBinary(UserCast, $2, $4);
			}
	| '(' NonExpressionType ')' PrefixedUnaryExpression	{
				/*
				 * This rule recognizes types that involve non-expression
				 * identifiers such as "int", "bool", "string", etc.
				 */
				MakeBinary(UserCast, $2, $4);
			}
	;

PrefixedUnaryExpression
	: UnaryExpression				{ $$ = $1; }
	| '+' PrefixedUnaryExpression	{ MakeUnary(UnaryPlus, $2); }
	| '-' PrefixedUnaryExpression			{
				/* We create negate nodes carefully so that integer
				   and float constants can be negated in-place */
				if(yyisa($2, ILNode_Integer))
				{
					$$ = NegateInteger((ILNode_Integer *)$2);
				}
				else if(yyisa($2, ILNode_Real))
				{
					((ILNode_Real *)($2))->value =
							-(((ILNode_Real *)($2))->value);
					$$ = $2;
				}
				else if(yyisa($2, ILNode_Decimal))
				{
					ILDecimalNeg(&(((ILNode_Decimal *)($2))->value),
								 &(((ILNode_Decimal *)($2))->value));
					$$ = $2;
				}
				else
				{
					MakeUnary(Neg, $2);
				}
			}
	| INC_OP PrefixedUnaryExpression	{ MakeUnary(PreInc, $2); }
	| DEC_OP PrefixedUnaryExpression	{ MakeUnary(PreDec, $2); }
	| '*' PrefixedUnaryExpression		{ MakeBinary(Deref, $2, 0); }
	| '&' PrefixedUnaryExpression		{ MakeUnary(AddressOf, $2); }
	;

MultiplicativeExpression
	: PrefixedUnaryExpression				{ $$ = $1; }
	| MultiplicativeExpression '*' PrefixedUnaryExpression	{
				MakeBinary(Mul, $1, $3);
			}
	| MultiplicativeExpression '/' PrefixedUnaryExpression	{
				MakeBinary(Div, $1, $3);
			}
	| MultiplicativeExpression '%' PrefixedUnaryExpression	{
				MakeBinary(Rem, $1, $3);
			}
	;

AdditiveExpression
	: MultiplicativeExpression		{ $$ = $1; }
	| AdditiveExpression '+' MultiplicativeExpression	{
				MakeBinary(Add, $1, $3);
			}
	| AdditiveExpression '-' MultiplicativeExpression	{
				MakeBinary(Sub, $1, $3);
			}
	;

ShiftExpression
	: AdditiveExpression			{ $$ = $1; }
	| ShiftExpression LEFT_OP AdditiveExpression	{
				MakeBinary(Shl, $1, $3);
			}
	| ShiftExpression RIGHT_OP AdditiveExpression	{
				MakeBinary(Shr, $1, $3);
			}
	;

/*
 * Relational expressions also recognise generic type references.
 * We have to put them here instead of in the more logical place
 * of "PrimaryExpression" to prevent reduce/reduce conflicts.
 *
 * This has some odd consequences.  An expression such as "A + B<C>"
 * will be parsed as "(A + B)<C>" instead of "A + (B<C>)".  To get
 * around this, we insert the generic type parameters into the
 * right-most part of the sub-expression, which should put the
 * parameters back where they belong.  A similar problem happens
 * with method invocations that involve generic method parameters.
 */
RelationalExpression
	: ShiftExpression				{ $$ = $1; }
	| RelationalExpression '<' ShiftExpression		{
				MakeBinary(Lt, $1, $3);
			}
	| RelationalExpression '>' ShiftExpression		{
				MakeBinary(Gt, $1, $3);
			}
	| RelationalExpression LE_OP ShiftExpression	{
				MakeBinary(Le, $1, $3);
			}
	| RelationalExpression GE_OP ShiftExpression	{
				MakeBinary(Ge, $1, $3);
			}
	| RelationalExpression IS Type					{
				MakeBinary(IsUntyped, $1, $3);
			}
	| RelationalExpression AS Type					{
				MakeBinary(AsUntyped, $1, $3);
			}
	| GenericReference								{
				$$ = $1;
			}
	| GenericReference '(' OptArgumentList ')'		{
				$$ = CSInsertMethodInvocation($1, $3);
			}
	;

GenericReference
	: RelationalExpression GENERIC_LT ShiftExpression '>'		{
				$$ = CSInsertGenericReference($1, $3);
			}
	| RelationalExpression GENERIC_LT ShiftExpression TypeSuffixList '>'	{
				$$ = CSInsertGenericReference
					($1, ILNode_LocalVariableType_create($3, $4));
			}
	| RelationalExpression GENERIC_LT ShiftExpression ',' TypeActuals '>'	{
				$$ = CSInsertGenericReference
					($1, ILNode_TypeActuals_create($3, $5));
			}
	| RelationalExpression GENERIC_LT ShiftExpression TypeSuffixList ',' 
			TypeActuals '>'		{
				$$ = CSInsertGenericReference
					($1, CSInsertTypeActuals
						(ILNode_LocalVariableType_create($3, $4), $6));
			}
	| RelationalExpression GENERIC_LT BuiltinType TypeSuffixes '>'	{
				$$ = CSInsertGenericReference
					($1, ILNode_LocalVariableType_create($3, $4));
			}
	| RelationalExpression GENERIC_LT BuiltinType TypeSuffixes ','
			TypeActuals '>'	{
				$$ = CSInsertGenericReference
					($1, CSInsertTypeActuals
						(ILNode_LocalVariableType_create($3, $4), $6));
			}
	;

EqualityExpression
	: RelationalExpression			{ $$ = $1; }
	| EqualityExpression EQ_OP RelationalExpression	{
				MakeBinary(Eq, $1, $3);
			}
	| EqualityExpression NE_OP RelationalExpression	{
				MakeBinary(Ne, $1, $3);
			}
	;

AndExpression
	: EqualityExpression			{ $$ = $1; }
	| AndExpression '&' EqualityExpression	{
				MakeBinary(And, $1, $3);
			}
	;

XorExpression
	: AndExpression					{ $$ = $1; }
	| XorExpression '^' AndExpression		{
				MakeBinary(Xor, $1, $3);
			}
	;

OrExpression
	: XorExpression					{ $$ = $1; }
	| OrExpression '|' XorExpression		{
				MakeBinary(Or, $1, $3);
			}
	;

LogicalAndExpression
	: OrExpression					{ $$ = $1; }
	| LogicalAndExpression AND_OP OrExpression	{
				MakeBinary(LogicalAnd, $1, $3);
			}
	;

LogicalOrExpression
	: LogicalAndExpression			{ $$ = $1; }
	| LogicalOrExpression OR_OP LogicalAndExpression	{
				MakeBinary(LogicalOr, $1, $3);
			}
	;

ConditionalExpression
	: LogicalOrExpression			{ $$ = $1; }
	| LogicalOrExpression '?' Expression ':' Expression	{
				MakeTernary(Conditional, ILNode_ToBool_create($1), $3, $5);
			}
	;

AssignmentExpression
	: PrefixedUnaryExpression '=' Expression	{
				MakeBinary(Assign, $1, $3);
			}
	| PrefixedUnaryExpression ADD_ASSIGN_OP Expression {
				MakeUnary(AssignAdd, ILNode_Add_create($1, $3));
			}
	| PrefixedUnaryExpression SUB_ASSIGN_OP Expression {
				MakeUnary(AssignSub, ILNode_Sub_create($1, $3));
			}
	| PrefixedUnaryExpression MUL_ASSIGN_OP Expression {
				MakeUnary(AssignMul, ILNode_Mul_create($1, $3));
			}
	| PrefixedUnaryExpression DIV_ASSIGN_OP Expression {
				MakeUnary(AssignDiv, ILNode_Div_create($1, $3));
			}
	| PrefixedUnaryExpression MOD_ASSIGN_OP Expression {
				MakeUnary(AssignRem, ILNode_Rem_create($1, $3));
			}
	| PrefixedUnaryExpression AND_ASSIGN_OP Expression {
				MakeUnary(AssignAnd, ILNode_And_create($1, $3));
			}
	| PrefixedUnaryExpression OR_ASSIGN_OP Expression {
				MakeUnary(AssignOr, ILNode_Or_create($1, $3));
			}
	| PrefixedUnaryExpression XOR_ASSIGN_OP Expression {
				MakeUnary(AssignXor, ILNode_Xor_create($1, $3));
			}
	| PrefixedUnaryExpression LEFT_ASSIGN_OP Expression {
				MakeUnary(AssignShl, ILNode_Shl_create($1, $3));
			}
	| PrefixedUnaryExpression RIGHT_ASSIGN_OP Expression {
				MakeUnary(AssignShr, ILNode_Shr_create($1, $3));
			}
	;

Expression
	: ConditionalExpression		{ $$ = $1; }
	| AssignmentExpression		{ $$ = $1; }
	;

ParenExpression
	: '(' Expression ')'		{ $$ = $2; }
	| '(' error ')'		{
				/*
				 * This production recovers from errors in expressions
				 * that are used with "switch".  Return 0 as the value.
				 */
				MakeTernary(Int32, 0, 0, 1);
				yyerrok;
			}
	;

ConstantExpression
	: Expression		{ MakeUnary(ToConst, $1); }
	;

BooleanExpression
	: Expression		{ MakeUnary(ToBool, $1); }
	;

ParenBooleanExpression
	: '(' BooleanExpression ')'		{ $$ = $2; }
	| '(' error ')'		{
				/*
				 * This production recovers from errors in boolean
				 * expressions that are used with "if", "while", etc.
				 * Default to "false" as the error condition's value.
				 */
				MakeSimple(False);
				yyerrok;
			}
	;

/*
 * Array initialization.
 */

OptArrayInitializer
	: /* empty */			{ $$ = 0; }
	| ArrayInitializer		{ $$ = $1; }
	;

ArrayInitializer
	: '{' OptVariableInitializerList '}' { $$ = $2; }
	| '{' VariableInitializerList ',' '}' { $$ = $2; }
	;

OptVariableInitializerList
	: /* empty */				{ $$ = ILNode_List_create(); }
	| VariableInitializerList	{ $$ = $1; }
	;

VariableInitializerList
	: VariableInitializer {	
				$$ = ILNode_List_create();
				ILNode_List_Add($$, $1);
			}
	| VariableInitializerList ',' VariableInitializer {
				ILNode_List_Add($1, $3);
				$$ = $1;
			}
	;

VariableInitializer
	: Expression				{ $$ = $1; }
	| ArrayInitializer			{ MakeUnary(ArrayInit, $1); }
	;

OptComma
	: /* empty */
	| ','
	;

/*
 * Statements.
 */

Statement
	: Identifier ':' LineStatement		{
				/* Convert the identifier into a "GotoLabel" node */
				ILNode *label = ILNode_GotoLabel_create(ILQualIdentName($1, 0));

				/* Build a compound statement */
				$$ = ILNode_Compound_CreateFrom(label, $3);
			}
	| LocalVariableDeclaration ';'	{ $$ = $1; }
	| LocalConstantDeclaration ';'	{ $$ = $1; }
	| InnerEmbeddedStatement		{ $$ = $1; }
	;

EmbeddedStatement
	: InnerEmbeddedStatement		{
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

InnerEmbeddedStatement
	: Block							{ $$ = $1; }
	| ';'							{ MakeSimple(Empty); }
	| InnerExpressionStatement ';'	{ $$ = $1; }
	| SelectionStatement			{ $$ = $1; }
	| IterationStatement			{ $$ = $1; }
	| JumpStatement					{ $$ = $1; }
	| TryStatement					{ $$ = $1; }
	| CHECKED Block					{ MakeUnary(Overflow, $2); }
	| UNCHECKED Block				{ MakeUnary(NoOverflow, $2); }
	| LockStatement					{ $$ = $1; }
	| UsingStatement				{ $$ = $1; }
	| FixedStatement				{ $$ = $1; }
	| UNSAFE Block					{ MakeUnary(Unsafe, $2); }
	| YieldStatement				{ $$ = $1; }
	| error ';'		{
				/*
				 * This production recovers from parse errors in statements,
				 * by seaching for the end of the current statement.
				 */
				MakeSimple(Empty);
				yyerrok;
			}
	;

LocalVariableDeclaration
	: LocalVariableType VariableDeclarators		{
				/* "VariableDeclarators" has split the declaration into
				   a list of variable names, plus a list of assignment
				   statements to set the initial values.  Turn the result
				   into a local variable declaration followed by the
				   assignment statements */
				if($2.init)
				{
					$$ = ILNode_Compound_CreateFrom
							(ILNode_LocalVarDeclaration_create($1, $2.decl),
							 $2.init);
				}
				else
				{
					$$ = ILNode_LocalVarDeclaration_create($1, $2.decl);
				}
			}
	;

VariableDeclarators
	: VariableDeclarator							{
				$$.decl = ILNode_List_create();
				ILNode_List_Add($$.decl, $1.decl);
				$$.init = $1.init;
			}	
	| VariableDeclarators ',' VariableDeclarator	{
				ILNode_List_Add($1.decl, $3.decl);
				$$.decl = $1.decl;
				if($1.init)
				{
					if($3.init)
					{
						$$.init = ILNode_Compound_CreateFrom($1.init, $3.init);
					}
					else
					{
						$$.init = $1.init;
					}
				}
				else if($3.init)
				{
					$$.init = $3.init;
				}
				else
				{
					$$.init = 0;
				}
			}
	;

VariableDeclarator
	: Identifier							{ $$.decl = $1; $$.init = 0; }
	| Identifier '=' VariableInitializer	{
				$$.decl = $1;
				$$.init = ILNode_Assign_create($1, $3);
			}
	;

LocalConstantDeclaration
	: CONST Type ConstantDeclarators		{
				$$ = ILNode_LocalConstDeclaration_create($2, $3);
			}
	;

Block
	: '{' OptStatementList '}'		{
				ILNode *temp;
			#ifdef YYBISON
				if(yykind($2) == yykindof(ILNode_Empty) && debug_flag)
				{
					temp = ILNode_LineInfo_create($2);
					yysetlinenum(temp, @1.first_line);
				}
				else
			#endif
				{
					temp = $2;
				}

				/* Wrap the block in a new local variable scope */
				$$ = ILNode_NewScope_create(temp);
				yysetfilename($$, yygetfilename(temp));
				yysetlinenum($$, yygetlinenum(temp));
			}
	| '{' error '}'		{
				/*
				 * This production recovers from parse errors in
				 * a block, by closing off the block on error.
				 */
				MakeSimple(Empty);
				yyerrok;
			}
	;

OptStatementList
	: /* empty */				{ MakeSimple(Empty); }
	| StatementList				{ $$ = $1; }
	;

StatementList
	: LineStatement					{ $$ = $1; }
	| StatementList LineStatement	{ $$ = ILNode_Compound_CreateFrom($1, $2); }
	;

LineStatement
	: Statement		{
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

ExpressionStatement
	: InnerExpressionStatement		{
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

InnerExpressionStatement
	: InvocationExpression				{ $$ = $1; }
	| ObjectCreationExpression			{ $$ = $1; }
	| AssignmentExpression				{ $$ = $1; }
	| PrimaryExpression INC_OP			{ MakeUnary(PostInc, $1); }
	| PrimaryExpression DEC_OP			{ MakeUnary(PostDec, $1); }
	| INC_OP PrefixedUnaryExpression	{ MakeUnary(PreInc, $2); }
	| DEC_OP PrefixedUnaryExpression	{ MakeUnary(PreDec, $2); }
	;

SelectionStatement
	: IF ParenBooleanExpression EmbeddedStatement	{
				MakeTernary(If, ILNode_ToBool_create($2), $3,
							ILNode_Empty_create());
			}
	| IF ParenBooleanExpression EmbeddedStatement ELSE EmbeddedStatement	{
				MakeTernary(If, ILNode_ToBool_create($2), $3, $5);
			}
	| SWITCH ParenExpression SwitchBlock	{
				MakeTernary(Switch, $2, $3, 0);
			}
	;

SwitchBlock
	: '{' OptSwitchSections '}'		{ $$ = $2; }
	| '{' error '}'		{
				/*
				 * This production recovers from parse errors in the
				 * body of a switch statement.
				 */
				$$ = 0;
				yyerrok;
			}
	;

OptSwitchSections
	: /* empty */				{ $$ = 0; }
	| SwitchSections			{ $$ = $1; }
	;

SwitchSections
	: SwitchSection					{ 
				$$ = ILNode_SwitchSectList_create();
				ILNode_List_Add($$, $1);
			}
	| SwitchSections SwitchSection	{
				/* Append the new section to the list */
				ILNode_List_Add($1, $2);
				$$ = $1;
			}
	;

SwitchSection
	: SwitchLabels StatementList	{ MakeBinary(SwitchSection, $1, $2); }
	;

SwitchLabels
	: SwitchLabel					{
				/* Create a new label list with one element */
				$$ = ILNode_CaseList_create();
				ILNode_List_Add($$, $1);
			}
	| SwitchLabels SwitchLabel		{
				/* Append the new label to the list */
				ILNode_List_Add($1, $2);
				$$ = $1;
			}
	;

SwitchLabel
	: CASE ConstantExpression ':'	{ MakeUnary(CaseLabel, $2); }
	| DEFAULT ':'					{ MakeSimple(DefaultLabel); }
	;

IterationStatement
	: WHILE ParenBooleanExpression EmbeddedStatement	{
				MakeBinary(While, ILNode_ToBool_create($2), $3);
			}
	| DO EmbeddedStatement WHILE ParenBooleanExpression ';'	{
				MakeBinary(Do, $2, ILNode_ToBool_create($4));
			}
	| FOR '(' ForInitializer ForCondition ForIterator EmbeddedStatement	{
				MakeQuaternary(For, $3, ILNode_ToBool_create($4), $5, $6);
				$$ = ILNode_NewScope_create($$);
			}
	| FOREACH '(' Type Identifier IN ForeachExpression EmbeddedStatement	{
				$$ = ILNode_NewScope_create
					(ILNode_Foreach_create($3, ILQualIdentName($4, 0),
										   $4, $6, $7));
			}
	;

ForInitializer
	: ForInitializerInner ';'	{ $$ = $1; }
	| ';'						{ MakeSimple(Empty); }
	| error ';'		{
				/*
				 * This production recovers from errors in the initializer
				 * of a "for" statement.
				 */
				MakeSimple(Empty);
				yyerrok;
			}
	;

ForInitializerInner
	: LocalVariableDeclaration	{ $$ = $1; }
	| ExpressionStatementList	{ $$ = $1; }
	;

ForCondition
	: BooleanExpression ';'		{ $$ = $1; }
	| ';'						{ MakeSimple(True); }
	| error ';'		{
				/*
				 * This production recovers from errors in the condition
				 * of a "for" statement.
				 */
				MakeSimple(False);
				yyerrok;
			}
	;

ForIterator
	: ExpressionStatementList ')'	{ $$ = $1; }
	| ')'							{ MakeSimple(Empty); }
	| error ')'		{
				/*
				 * This production recovers from errors in the interator
				 * of a "for" statement.
				 */
				MakeSimple(Empty);
				yyerrok;
			}
	;

ForeachExpression
	: Expression ')'			{ $$ = $1; }
	| error ')'		{
				/*
				 * This production recovers from errors in the expression
				 * used within a "foreach" statement.
				 */
				MakeSimple(Null);
				yyerrok;
			}
	;

ExpressionStatementList
	: ExpressionStatement		{ $$ = $1; }
	| ExpressionStatementList ',' ExpressionStatement	{
				$$ = ILNode_Compound_CreateFrom($1, $3);
			}
	;

JumpStatement
	: BREAK ';'					{ MakeSimple(Break); }
	| CONTINUE ';'				{ MakeSimple(Continue); }
	| GOTO Identifier ';'		{
				/* Convert the identifier node into a "Goto" node */
				$$ = ILNode_Goto_create(ILQualIdentName($2, 0));
			}
	| GOTO CASE ConstantExpression ';'	{ MakeUnary(GotoCase, $3); }
	| GOTO DEFAULT ';'					{ MakeSimple(GotoDefault); }
	| RETURN ';'						{ MakeSimple(Return); }
	| RETURN Expression ';'				{ MakeUnary(ReturnExpr, $2); }
	| THROW ';'							{ MakeSimple(Throw); }
	| THROW Expression ';'				{ MakeUnary(ThrowExpr, $2); }
	;

TryStatement
	: TRY Block CatchClauses				{ MakeTernary(Try, $2, $3, 0); }
	| TRY Block FinallyClause				{ MakeTernary(Try, $2, 0, $3); }
	| TRY Block CatchClauses FinallyClause	{ MakeTernary(Try, $2, $3, $4); }
	;

CatchClauses
	: SpecificCatchClauses OptGeneralCatchClause	{
				if($2)
				{
					ILNode_List_Add($1, $2);
				}
				$$ = $1;
			}
	| OptSpecificCatchClauses GeneralCatchClause	{
				if($1)
				{
					ILNode_List_Add($1, $2);
					$$ = $1;
				}
				else
				{
					$$ = ILNode_CatchClauses_create();
					ILNode_List_Add($$, $2);
				}
			}
	;

OptSpecificCatchClauses
	: /* empty */				{ $$ = 0; }
	| SpecificCatchClauses		{ $$ = $1; }
	;

SpecificCatchClauses
	: SpecificCatchClause		{
				$$ = ILNode_CatchClauses_create();
				ILNode_List_Add($$, $1);
			}
	| SpecificCatchClauses SpecificCatchClause	{
				ILNode_List_Add($1, $2);
				$$ = $1;
			}
	;

SpecificCatchClause
	: CATCH CatchNameInfo Block	{
				$$ = ILNode_CatchClause_create($2.type, $2.id, $2.idNode, $3);
			}
	;

CatchNameInfo
	: /* nothing */ {
				$$.type=ILNode_Identifier_create("Exception");
				$$.id = 0;
				$$.idNode = 0;
			}
	| '(' Type Identifier ')' {
				$$.type = $2;
				$$.id = ILQualIdentName($3, 0);
				$$.idNode = $3;
			}
	| '(' Type ')'			  {
				$$.type = $2;
				$$.id = 0;
				$$.idNode = 0;
			}
	| '(' error ')'	{
				/*
				 * This production recovers from errors in catch
				 * variable name declarations.
				 */
				$$.type = ILNode_Error_create();
				$$.id = 0;
				$$.idNode = 0;
				yyerrok;
			}
	;

OptGeneralCatchClause
	: /* empty */				{ $$ = 0; }
	| GeneralCatchClause		{ $$ = $1; }
	;

GeneralCatchClause
	: CATCH Block		{
				$$ = ILNode_CatchClause_create(0, 0, 0, $2);
			}
	;

FinallyClause
	: FINALLY Block		{ MakeUnary(FinallyClause, $2); }
	;

LockStatement
	: LOCK ParenExpression EmbeddedStatement	{
				MakeBinary(Lock, $2, $3);
			}
	;

UsingStatement
	: USING ResourceAcquisition EmbeddedStatement	{
				MakeBinary(Using, $2, $3);
				$$ = ILNode_NewScope_create($$);
			}
	;

ResourceAcquisition
	: '(' LocalVariableType VariableDeclarators ')'	{ 
			MakeTernary(ResourceDeclaration,$2,$3.decl,$3.init); 
		}
	| '(' Expression ')'				{ 
			$$ = $2;
		}
	| '(' error ')'		{
				/*
				 * This production recovers from errors in resource
				 * acquisition declarations.
				 */
				MakeSimple(Error);
				yyerrok;
			}
	;

/* unsafe code */
FixedStatement
	: FIXED '(' Type FixedPointerDeclarators ')' EmbeddedStatement	{
				MakeTernary(Fixed, $3, $4, $6);
			}
	;

FixedPointerDeclarators
	: FixedPointerDeclarator		{
				$$ = ILNode_List_create();
				ILNode_List_Add($$, $1);
			}
	| FixedPointerDeclarators ',' FixedPointerDeclarator	{
				$$ = $1;
				ILNode_List_Add($1, $3);
			}
	;

FixedPointerDeclarator
	: Identifier '=' Expression	{
				/*
				 * Note: we have to handle two cases here.  One where
				 * the expression has the form "&expr", and the other
				 * where it doesn't have that form.  We cannot express
				 * these as two different rules, or it creates a
				 * reduce/reduce conflict with "UnaryExpression".
				 */
				if(yykind($3) == yykindof(ILNode_AddressOf))
				{
					MakeBinary(FixAddress, $1,$3);
				}
				else
				{
					MakeBinary(FixExpr, $1, $3);
				}
			}
	;

YieldStatement
	: YIELD RETURN Expression ';'		{
				$$ = ILNode_Empty_create();
				CCError(_("`yield return' is not yet supported"));
			}
	| YIELD BREAK ';'		{
				$$ = ILNode_Empty_create();
				CCError(_("`yield break' is not yet supported"));
			}
	;

/*
 * Attributes.
 */

OptAttributes
	: /* empty */ 		{ $$ = 0; }
	| AttributeSections	{ CSValidateDocs($1); MakeUnary(AttributeTree, $1); }
	;

NonOptAttributes
	: AttributeSections	{ CSValidateDocs($1); MakeUnary(AttributeTree, $1); }
	;

AttributeSections
	: AttributeSection	{
				$$ = ILNode_List_create();
				if($1)
				{
					ILNode_List_Add($$, $1);
				}
			}
	| AttributeSections AttributeSection	{
				$$ = $1;
				if($2)
				{
					ILNode_List_Add($1, $2);
				}
			}
	;

AttributeSection
	: '[' AttributeList OptComma ']'					{
				MakeTernary(AttributeSection, ILAttrTargetType_None, 0, $2);
			}
	| '[' AttributeTarget AttributeList OptComma ']'	{
				MakeTernary(AttributeSection, $2.targetType, $2.target, $3);
			}
	| DOC_COMMENT		{ MakeBinary(DocComment, $1.string, $1.len); }
	| '[' error ']'		{
				/*
				 * This production recovers from errors in attributes.
				 */
				$$ = 0;
				yyerrok;
			}
	;

AttributeTarget
	: QualifiedIdentifier ':'	{
				$$.targetType = ILAttrTargetType_Named;
				$$.target = $1;
			}
	| EVENT ':'					{
				$$.targetType = ILAttrTargetType_Event;
				$$.target = 0;
			}
	| RETURN ':'				{
				$$.targetType = ILAttrTargetType_Return;
				$$.target = 0;
			}
	;

AttributeList
	: Attribute	{
				$$ = ILNode_List_create();
				ILNode_List_Add($$, $1);
			}
	| AttributeList ',' Attribute	{
				ILNode_List_Add($1, $3);
				$$ = $1;
			}
	;

Attribute
	: QualifiedIdentifier						{ 
				MakeBinary(Attribute, $1, 0);
			}
	| QualifiedIdentifier AttributeArguments	{ 
				MakeBinary(Attribute, $1, $2);
			}
	;

AttributeArguments
	: '(' ')' {	$$=0; /* empty */ }
	| '(' PositionalArgumentList ')'			{
				MakeBinary(AttrArgs, $2, 0);
			}
	| '(' PositionalArgumentList ',' NamedArgumentList ')'	{
				MakeBinary(AttrArgs, $2, $4);
			}
	| '(' NamedArgumentList ')'	{
				MakeBinary(AttrArgs, 0, $2);
			}
	;

PositionalArgumentList
	: PositionalArgument		{
				$$ = ILNode_List_create ();
				ILNode_List_Add ($$, $1);
			}
	| PositionalArgumentList ',' PositionalArgument	{
				ILNode_List_Add($1, $3);
				$$ = $1;
			}
	;

PositionalArgument
	: AttributeArgumentExpression {$$ = $1;}
	;

NamedArgumentList
	: NamedArgument		{
				$$ = ILNode_List_create ();
				ILNode_List_Add($$, $1);
			}
	| NamedArgumentList ',' NamedArgument	{
				ILNode_List_Add($1, $3);
				$$ = $1;
			}
	;

NamedArgument
	: Identifier '=' AttributeArgumentExpression	{
				MakeBinary(NamedArg, $1, $3);
			}
	;

AttributeArgumentExpression
	: Expression			{ $$ = ILNode_ToAttrConst_create($1); }
	;

/*
 * Modifiers.
 */

OptModifiers
	: /* empty */			{ $$ = 0; }
	| Modifiers				{ $$ = $1; }
	;

Modifiers
	: Modifier				{ $$ = $1; }
	| Modifiers Modifier	{
				if(($1 & $2) != 0)
				{
					/* A modifier was used more than once in the list */
					CSModifiersUsedTwice(yycurrfilename(), yycurrlinenum(),
										 ($1 & $2));
				}
				$$ = ($1 | $2);
			}
	;

Modifier
	: NEW			{ $$ = CS_MODIFIER_NEW; }
	| PUBLIC		{ $$ = CS_MODIFIER_PUBLIC; }
	| PROTECTED		{ $$ = CS_MODIFIER_PROTECTED; }
	| INTERNAL		{ $$ = CS_MODIFIER_INTERNAL; }
	| PRIVATE		{ $$ = CS_MODIFIER_PRIVATE; }
	| ABSTRACT		{ $$ = CS_MODIFIER_ABSTRACT; }
	| SEALED		{ $$ = CS_MODIFIER_SEALED; }
	| STATIC		{ $$ = CS_MODIFIER_STATIC; }
	| READONLY		{ $$ = CS_MODIFIER_READONLY; }
	| VIRTUAL		{ $$ = CS_MODIFIER_VIRTUAL; }
	| OVERRIDE		{ $$ = CS_MODIFIER_OVERRIDE; }
	| EXTERN		{ $$ = CS_MODIFIER_EXTERN; }
	| UNSAFE		{ $$ = CS_MODIFIER_UNSAFE; }
	| VOLATILE		{ $$ = CS_MODIFIER_VOLATILE; }
	;

/*
 * Class declarations.
 */

ClassDeclaration
	: OptAttributes OptModifiers OptPartial CLASS Identifier TypeFormals
			ClassBase Constraints {
				/* Enter a new nesting level */
				++NestingLevel;

				/* Push the identifier onto the class name stack */
				ClassNamePush($5);
			}
			ClassBody OptSemiColon	{
				ILNode *classBody = ($10).body;

				/* Validate the modifiers */
				ILUInt32 attrs =
					CSModifiersToTypeAttrs($5, $2, (NestingLevel > 1));

				/* Exit the current nesting level */
				--NestingLevel;

				/* Determine if we need to add a default constructor */
				if(!ClassNameIsCtorDefined())
				{
					ILUInt32 ctorMods =
						(((attrs & IL_META_TYPEDEF_ABSTRACT) != 0)
							? CS_MODIFIER_PROTECTED : CS_MODIFIER_PUBLIC);
					ILNode *cname = ILQualIdentSimple
							(ILInternString(".ctor", 5).string);
					ILNode *body = ILNode_NewScope_create
							(ILNode_Compound_CreateFrom
								(ILNode_NonStaticInit_create(),
								 ILNode_InvocationExpression_create
									(ILNode_BaseInit_create(), 0)));
					ILNode *ctor = ILNode_MethodDeclaration_create
						  (0, CSModifiersToConstructorAttrs(cname, ctorMods),
						   0 /* "void" */, cname,
						   ILNode_Empty_create(), body);
					if(!classBody)
					{
						classBody = ILNode_List_create();
					}
					ILNode_List_Add(classBody, ctor);
				}

				/* Create the class definition */
				InitGlobalNamespace();
				$$ = ILNode_ClassDefn_create
							($1,					/* OptAttributes */
							 attrs,					/* OptModifiers */
							 ILQualIdentName($5, 0),/* Identifier */
							 CurrNamespace.string,	/* Namespace */
							 (ILNode *)CurrNamespaceNode,
							 $6,					/* TypeFormals */
							 $7,					/* ClassBase */
							 classBody,
							 ($10).staticCtors);
				CloneLine($$, $5);

				/* Pop the class name stack */
				ClassNamePop();

				/* We have declarations at the top-most level of the file */
				HaveDecls = 1;
			}
	;

TypeFormals
	: /* empty */						{ $$ = 0; }
	| '<' TypeFormalList '>'			{ $$ = $2; }
	| GENERIC_LT TypeFormalList '>'		{ $$ = $2; }
	;

TypeFormalList
	: Identifier					{
				$$ = ILNode_List_create();
				ILNode_List_Add($$, $1);
			}
	| TypeFormalList ',' Identifier	{
				/* Check for duplicates in the list */
				ILNode_ListIter iter;
				ILNode *node;
				ILNode_ListIter_Init(&iter, $1);
				while((node = ILNode_ListIter_Next(&iter)) != 0)
				{
					if(!strcmp(ILQualIdentName(node, 0),
							   ILQualIdentName($3, 0)))
					{
						CCErrorOnLine(yygetfilename($3), yygetlinenum($3),
						  "`%s' declared multiple times in generic parameters",
						  ILQualIdentName($3, 0));
						break;
					}
				}

				/* Add the identifier to the list */
				ILNode_List_Add($1, $3);
				$$ = $1;
			}
	;

/* TODO: generic parameter constraints */
Constraints
	: /* empty */
	| WHERE ConstraintList
	;

ConstraintList
	: Constraint
	| ConstraintList ',' Constraint
	;

Constraint
	: Identifier ':' Type			{ /* TODO */ }
	| Identifier ':' NEW '(' ')'	{ /* TODO */ }
	;

ModuleDeclaration
	: MODULE {
				/* Enter a new nesting level */
				++NestingLevel;

				/* Push the identifier onto the class name stack */
				$<node>$ = ILQualIdentSimple("<Module>");
				ClassNamePush($<node>$);
			}
			ClassBody OptSemiColon	{
				ILNode *classBody = ($3).body;

				/* Get the default modifiers */
				ILUInt32 attrs = IL_META_TYPEDEF_PUBLIC;

				/* Exit the current nesting level */
				--NestingLevel;

				/* Create the class definition */
				InitGlobalNamespace();
				$$ = ILNode_ClassDefn_create
							(0,						/* OptAttributes */
							 attrs,					/* OptModifiers */
							 ILInternString("<Module>", -1).string,
							 CurrNamespace.string,	/* Namespace */
							 (ILNode *)CurrNamespaceNode,
							 0,						/* TypeFormals */
							 0,						/* ClassBase */
							 classBody,
							 ($3).staticCtors);
				CloneLine($$, $<node>2);

				/* Pop the class name stack */
				ClassNamePop();

				/* We have declarations at the top-most level of the file */
				HaveDecls = 1;
			}
	;

ClassBase
	: /* empty */		{ $$ = 0; }
	| ':' TypeList		{ $$ = $2; }
	;

TypeList
	: Type					{ $$ = $1; }
	| TypeList ',' Type		{ MakeBinary(ArgList, $1, $3); }
	;

ClassBody
	: '{' OptClassMemberDeclarations '}'	{ $$ = $2; }
	| '{' error '}'		{
				/*
				 * This production recovers from errors in class bodies.
				 */
				yyerrok;
				$$.body = 0;
				$$.staticCtors = 0;
			}
	;

OptClassMemberDeclarations
	: /* empty */					{ $$.body = 0; $$.staticCtors = 0; }
	| ClassMemberDeclarations		{ $$ = $1; }
	;

ClassMemberDeclarations
	: ClassMemberDeclaration		{
				$$.body = MakeList(0, $1.body);
				$$.staticCtors = MakeList(0, $1.staticCtors);
			}
	| ClassMemberDeclarations ClassMemberDeclaration	{
				$$.body = MakeList($1.body, $2.body);
				$$.staticCtors = MakeList($1.staticCtors, $2.staticCtors);
			}
	;

ClassMemberDeclaration
	: ConstantDeclaration		{ $$.body = $1; $$.staticCtors = 0; }
	| FieldDeclaration			{ $$.body = $1; $$.staticCtors = 0; }
	| MethodDeclaration			{ $$.body = $1; $$.staticCtors = 0; }
	| PropertyDeclaration		{ $$.body = $1; $$.staticCtors = 0; }
	| EventDeclaration			{ $$.body = $1; $$.staticCtors = 0; }
	| IndexerDeclaration		{ $$.body = $1; $$.staticCtors = 0; }
	| OperatorDeclaration		{ $$.body = $1; $$.staticCtors = 0; }
	| ConstructorDeclaration	{ $$ = $1; }
	| DestructorDeclaration		{ $$.body = $1; $$.staticCtors = 0; }
	| TypeDeclaration			{ $$.body = $1; $$.staticCtors = 0; }
	;

OptPartial
	: /* empty */				{ $$ = 0; }
	| PARTIAL					{
				$$ = 1;
				CCError(_("partial types are not yet supported"));
			}
	;

/*
 * Constants.
 */

ConstantDeclaration
	: OptAttributes OptModifiers CONST Type ConstantDeclarators ';' {
				ILUInt32 attrs = CSModifiersToConstAttrs($4, $2);
				$$ = ILNode_FieldDeclaration_create($1, attrs, $4, $5);
			}
	;

ConstantDeclarators
	: ConstantDeclarator							{
				$$ = ILNode_List_create();
				ILNode_List_Add($$, $1);
			}
	| ConstantDeclarators ',' ConstantDeclarator    {
				ILNode_List_Add($1, $3);
				$$ = $1;
			}
	;

ConstantDeclarator
	: Identifier '=' ConstantExpression				{
				MakeBinary(FieldDeclarator, $1, $3);
			}
	;

/*
 * Fields.
 */

FieldDeclaration
	: OptAttributes OptModifiers Type FieldDeclarators ';'	{
				ILUInt32 attrs = CSModifiersToFieldAttrs($3, $2);
				$$ = ILNode_FieldDeclaration_create($1, attrs, $3, $4);
			}
	;

FieldDeclarators
	: FieldDeclarator						{
				$$ = ILNode_List_create();
				ILNode_List_Add($$, $1);
			}	
	| FieldDeclarators ',' FieldDeclarator {
				ILNode_List_Add($1, $3);
				$$ = $1;
			}
		
	;

FieldDeclarator
	: Identifier							{
				MakeBinary(FieldDeclarator, $1, 0);
			}
	| Identifier '=' VariableInitializer	{
				MakeBinary(FieldDeclarator, $1, $3);
			}
	;

/*
 * Methods.
 */

MethodDeclaration
	: OptAttributes OptModifiers Type QualifiedIdentifier
			'(' OptFormalParameterList ')' MethodBody	{
				ILUInt32 attrs = CSModifiersToMethodAttrs($3, $2);
				if($2 & CS_MODIFIER_PRIVATE  && yyisa($4, ILNode_QualIdent))
				{
					// NOTE: clean this up later
					CCErrorOnLine(yygetfilename($3), yygetlinenum($3),
						"`private' cannot be used in this context");
				}
				$$ = ILNode_MethodDeclaration_create
						($1, attrs, $3, $4, $6, $8);
				CloneLine($$, $4);
			}
	;

MethodBody
	: Block			{ $$ = $1; }
	| ';'			{ $$ = 0; }
	;

OptFormalParameterList
	: /* empty */			{ MakeSimple(Empty); }
	| FormalParameterList	{ $$ = $1; }
	;

FormalParameterList
	: FormalParameter							{
				$$ = ILNode_List_create();
				ILNode_List_Add($$, $1);
			}
	| FormalParameterList ',' FormalParameter	{
				ILNode_List_Add($1, $3);
				$$ = $1;
			}
	;

FormalParameter
	: OptAttributes ParameterModifier Type Identifier		{
				$$ = ILNode_FormalParameter_create($1, $2, $3, $4);
			}
	| ARGLIST	{
				$$ = ILNode_FormalParameter_create(0, ILParamMod_arglist, 0, 0);
			}
	;

ParameterModifier
	: /* empty */	{ $$ = ILParamMod_empty;}
	| REF			{ $$ = ILParamMod_ref;}
	| OUT			{ $$ = ILParamMod_out;}
	| PARAMS		{ $$ = ILParamMod_params;}
	;

/*
 * Properties.
 */

PropertyDeclaration
	: OptAttributes OptModifiers Type QualifiedIdentifier
			StartAccessorBlock AccessorBlock	{
				ILUInt32 attrs;

				/* Create the property declaration */
				attrs = CSModifiersToPropertyAttrs($3, $2);
				$$ = ILNode_PropertyDeclaration_create($1,
								   attrs, $3, $4, 0, $6.item1, $6.item2,
								   (($6.item1 ? 1 : 0) |
								    ($6.item2 ? 2 : 0)));
				CloneLine($$, $4);

				/* Create the property method declarations */
				CreatePropertyMethods((ILNode_PropertyDeclaration *)($$));
			}
	;

StartAccessorBlock
	: '{'
	;

AccessorBlock
	: AccessorDeclarations '}'	{
				$$ = $1;
			}
	| error '}'		{
				/*
				 * This production recovers from errors in accessor blocks.
				 */
				$$.item1 = 0;
				$$.item2 = 0;
				yyerrok;
			}
	;

AccessorDeclarations
	: GetAccessorDeclaration OptSetAccessorDeclaration		{
				$$.item1 = $1; 
				$$.item2 = $2;
			}
	| SetAccessorDeclaration OptGetAccessorDeclaration		{
				$$.item1 = $2; 
				$$.item2 = $1;
			}
	;

OptGetAccessorDeclaration
	: /* empty */				{ $$ = 0; }
	| GetAccessorDeclaration	{ $$ = $1;}
	;

GetAccessorDeclaration
	: OptAttributes GET AccessorBody {
				$$ = ILNode_MethodDeclaration_create
						($1, 0, 0, 0, 0, $3);
			#ifdef YYBISON
				yysetlinenum($$, @2.first_line);
			#endif
			}
	;

OptSetAccessorDeclaration
	: /* empty */				{ $$ = 0; }
	| SetAccessorDeclaration	{ $$ = $1; }
	;

SetAccessorDeclaration
	: OptAttributes SET AccessorBody {
				$$ = ILNode_MethodDeclaration_create
						($1, 0, 0, 0, 0, $3);
			#ifdef YYBISON
				yysetlinenum($$, @2.first_line);
			#endif
			}
	;

AccessorBody
	: Block				{ $$ = $1; }
	| ';'				{ $$ = 0; }
	;

/*
 * Events.
 */

EventDeclaration
	: EventFieldDeclaration
	| EventPropertyDeclaration
	;

EventFieldDeclaration
	: OptAttributes OptModifiers EVENT Type EventDeclarators ';'	{
				ILUInt32 attrs = CSModifiersToEventAttrs($4, $2);
				$$ = ILNode_EventDeclaration_create($1, attrs, $4, $5);
				CreateEventMethods((ILNode_EventDeclaration *)($$));
			}
	;

EventDeclarators
	: EventDeclarator						{
				$$ = ILNode_List_create();
				ILNode_List_Add($$, $1);
			}	
	| EventDeclarators ',' EventDeclarator {
				ILNode_List_Add($1, $3);
				$$ = $1;
			}
		
	;

EventDeclarator
	: Identifier							{
				$$ = ILNode_EventDeclarator_create
						(ILNode_FieldDeclarator_create($1, 0), 0, 0);
			}
	| Identifier '=' VariableInitializer	{
				$$ = ILNode_EventDeclarator_create
						(ILNode_FieldDeclarator_create($1, $3), 0, 0);
			}
	;

EventPropertyDeclaration
	: OptAttributes OptModifiers EVENT Type QualifiedIdentifier
			StartAccessorBlock EventAccessorBlock	{
				ILUInt32 attrs = CSModifiersToEventAttrs($4, $2);
				$$ = ILNode_EventDeclaration_create
					($1, attrs, $4, 
						ILNode_EventDeclarator_create
							(ILNode_FieldDeclarator_create($5, 0),
							 $7.item1, $7.item2));
				CloneLine($$, $5);
				CreateEventMethods((ILNode_EventDeclaration *)($$));
			}
	;

EventAccessorBlock
	: EventAccessorDeclarations '}'	{
				$$ = $1;
			}
	| error '}'		{
				/*
				 * This production recovers from errors in accessor blocks.
				 */
				$$.item1 = 0;
				$$.item2 = 0;
				yyerrok;
			}
	;

EventAccessorDeclarations
	: AddAccessorDeclaration RemoveAccessorDeclaration {
				$$.item1 = $1;
				$$.item2 = $2;
			}
	| RemoveAccessorDeclaration AddAccessorDeclaration {
				$$.item1 = $2;
				$$.item2 = $1;
			}
	;

AddAccessorDeclaration
	: OptAttributes ADD AccessorBody {
				$$ = ILNode_MethodDeclaration_create
						($1, 0, 0, 0, 0, $3);
			#ifdef YYBISION
				yysetlinenum($$, @2.first_line);
			#endif
			}
	;

RemoveAccessorDeclaration
	: OptAttributes REMOVE AccessorBody {
				$$ = ILNode_MethodDeclaration_create
						($1, 0, 0, 0, 0, $3);
			#ifdef YYBISION
				yysetlinenum($$, @2.first_line);
			#endif
			}
	;

/*
 * Indexers.
 */

IndexerDeclaration
	: OptAttributes OptModifiers IndexerDeclarator
			StartAccessorBlock AccessorBlock		{
				ILNode* name=GetIndexerName(&CCCodeGen,(ILNode_AttributeTree*)$1,
							$3.ident);
				ILUInt32 attrs = CSModifiersToPropertyAttrs($3.type, $2);
				$$ = ILNode_PropertyDeclaration_create($1,
								   attrs, $3.type, name, $3.params,
								   $5.item1, $5.item2,
								   (($5.item1 ? 1 : 0) |
								    ($5.item2 ? 2 : 0)));
				CloneLine($$, $3.ident);

				/* Create the property method declarations */
				CreatePropertyMethods((ILNode_PropertyDeclaration *)($$));
			}
	;

IndexerDeclarator
	: Type THIS FormalIndexParameters		{
				$$.type = $1;
				$$.ident = ILQualIdentSimple(NULL);
				$$.params = $3;
			}
	| Type QualifiedIdentifier '.' THIS FormalIndexParameters	{
				$$.type = $1;
				$$.ident = $2;
				$$.params = $5;
			}
	;

FormalIndexParameters
	: '[' FormalIndexParameterList ']'		{ $$ = $2; }
	| '[' error ']'		{
				/*
				 * This production recovers from errors in indexer parameters.
				 */
				$$ = 0;
				yyerrok;
			}
	;

FormalIndexParameterList
	: FormalIndexParameter								{
				$$ = ILNode_List_create ();
				ILNode_List_Add($$, $1);
			}
	| FormalIndexParameterList ',' FormalIndexParameter	{
				ILNode_List_Add($1, $3);
				$$ = $1;
			}
	;

FormalIndexParameter
	: OptAttributes ParameterModifier Type Identifier 					{
				$$ = ILNode_FormalParameter_create($1, $2, $3, $4);
			}
	| ARGLIST	{
				$$ = ILNode_FormalParameter_create(0, ILParamMod_arglist, 0, 0);
			}
	;

/*
 * Operators.
 */

OperatorDeclaration
	: NormalOperatorDeclaration		{ $$ = $1; }
	| ConversionOperatorDeclaration	{ $$ = $1; }
	;

NormalOperatorDeclaration
	: OptAttributes OptModifiers Type OPERATOR OverloadableOperator
			TypeFormals '(' Type Identifier ')'	Block {
				ILUInt32 attrs;
				ILNode *params;

				/* TODO: generic parameters */

				/* Validate the name of the unary operator */
				if($5.unary == 0)
				{
					CCError("overloadable unary operator expected");
					$5.unary = $5.binary;
				}

				/* Get the operator attributes */
				attrs = CSModifiersToOperatorAttrs($3, $2);

				/* Build the formal parameter list */
				params = ILNode_List_create();
				ILNode_List_Add(params,
					ILNode_FormalParameter_create(0, ILParamMod_empty, $8, $9));

				/* Create a method definition for the operator */
				$$ = ILNode_MethodDeclaration_create
						($1, attrs, $3,
						 ILQualIdentSimple(ILInternString($5.unary, -1).string),
						 params, $11);
				CloneLine($$, $3);
			}
	| OptAttributes OptModifiers Type OPERATOR OverloadableOperator
			TypeFormals '(' Type Identifier ',' Type Identifier ')' Block	{
				ILUInt32 attrs;
				ILNode *params;

				/* TODO: generic parameters */

				/* Validate the name of the binary operator */
				if($5.binary == 0)
				{
					CCError("overloadable binary operator expected");
					$5.binary = $5.unary;
				}

				/* Get the operator attributes */
				attrs = CSModifiersToOperatorAttrs($3, $2);

				/* Build the formal parameter list */
				params = ILNode_List_create();
				ILNode_List_Add(params,
					ILNode_FormalParameter_create
						(0, ILParamMod_empty, $8, $9));
				ILNode_List_Add(params,
					ILNode_FormalParameter_create
						(0, ILParamMod_empty, $11, $12));

				/* Create a method definition for the operator */
				$$ = ILNode_MethodDeclaration_create
						($1, attrs, $3,
						 ILQualIdentSimple
						 	(ILInternString($5.binary, -1).string),
						 params, $14);
				CloneLine($$, $3);
			}
	;

OverloadableOperator
	: '+'		{ $$.binary = "op_Addition"; $$.unary = "op_UnaryPlus"; }
	| '-'		{ $$.binary = "op_Subtraction"; $$.unary = "op_UnaryNegation"; }
	| '!'		{ $$.binary = 0; $$.unary = "op_LogicalNot"; }
	| '~'		{ $$.binary = 0; $$.unary = "op_OnesComplement"; }
	| INC_OP	{ $$.binary = 0; $$.unary = "op_Increment"; }
	| DEC_OP	{ $$.binary = 0; $$.unary = "op_Decrement"; }
	| TRUE		{ $$.binary = 0; $$.unary = "op_True"; }
	| FALSE		{ $$.binary = 0; $$.unary = "op_False"; }
	| '*'		{ $$.binary = "op_Multiply"; $$.unary = 0; }
	| '/'		{ $$.binary = "op_Division"; $$.unary = 0; }
	| '%'		{ $$.binary = "op_Modulus"; $$.unary = 0; }
	| '&'		{ $$.binary = "op_BitwiseAnd"; $$.unary = 0; }
	| '|'		{ $$.binary = "op_BitwiseOr"; $$.unary = 0; }
	| '^'		{ $$.binary = "op_ExclusiveOr"; $$.unary = 0; }
	| LEFT_OP	{ $$.binary = "op_LeftShift"; $$.unary = 0; }
	| RIGHT_OP	{ $$.binary = "op_RightShift"; $$.unary = 0; }
	| EQ_OP		{ $$.binary = "op_Equality"; $$.unary = 0; }
	| NE_OP		{ $$.binary = "op_Inequality"; $$.unary = 0; }
	| '>'		{ $$.binary = "op_GreaterThan"; $$.unary = 0; }
	| '<'		{ $$.binary = "op_LessThan"; $$.unary = 0; }
	| GE_OP		{ $$.binary = "op_GreaterThanOrEqual"; $$.unary = 0; }
	| LE_OP		{ $$.binary = "op_LessThanOrEqual"; $$.unary = 0; }
	;

ConversionOperatorDeclaration
	: OptAttributes OptModifiers IMPLICIT OPERATOR TypeFormals Type
			'(' Type Identifier ')' Block	{
				ILUInt32 attrs;
				ILNode *params;

				/* TODO: generic parameters */

				/* Get the operator attributes */
				attrs = CSModifiersToOperatorAttrs($6, $2);

				/* Build the formal parameter list */
				params = ILNode_List_create();
				ILNode_List_Add(params,
					ILNode_FormalParameter_create(0, ILParamMod_empty, $8, $9));

				/* Create a method definition for the operator */
				$$ = ILNode_MethodDeclaration_create
						($1, attrs, $6,
						 ILQualIdentSimple
						 	(ILInternString("op_Implicit", -1).string),
						 params, $11);
				CloneLine($$, $6);
			}
	| OptAttributes OptModifiers EXPLICIT OPERATOR TypeFormals Type
			'(' Type Identifier ')' Block	{
				ILUInt32 attrs;
				ILNode *params;

				/* TODO: generic parameters */

				/* Get the operator attributes */
				attrs = CSModifiersToOperatorAttrs($6, $2);

				/* Build the formal parameter list */
				params = ILNode_List_create();
				ILNode_List_Add(params,
					ILNode_FormalParameter_create(0, ILParamMod_empty, $8, $9));

				/* Create a method definition for the operator */
				$$ = ILNode_MethodDeclaration_create
						($1, attrs, $6,
						 ILQualIdentSimple
						 	(ILInternString("op_Explicit", -1).string),
						 params, $11);
				CloneLine($$, $6);
			}
	;

/*
 * Constructors and destructors.
 */

ConstructorDeclaration
	: OptAttributes OptModifiers QualifiedIdentifierPart
			'(' OptFormalParameterList ')' ConstructorInitializer MethodBody {
				ILUInt32 attrs = CSModifiersToConstructorAttrs($3, $2);
				ILNode *ctorName;
				ILNode *cname;
				ILNode *initializer = $7;
				ILNode *body;
				if((attrs & IL_META_METHODDEF_STATIC) != 0)
				{
					cname = ILQualIdentSimple
								(ILInternString(".cctor", 6).string);
					initializer = 0;
				}
				else
				{
					cname = ILQualIdentSimple
								(ILInternString(".ctor", 5).string);
					ClassNameCtorDefined();
				}
				ctorName = $3;
				if(yyisa(ctorName, ILNode_GenericReference))
				{
					CCErrorOnLine(yygetfilename($3), yygetlinenum($3),
						"constructors cannot have type parameters");
					ctorName = ((ILNode_GenericReference *)ctorName)->type;
				}
				if(!ClassNameSame(ctorName))
				{
					CCErrorOnLine(yygetfilename($3), yygetlinenum($3),
						"constructor name does not match class name");
				}
				if($8 && yykind($8) == yykindof(ILNode_NewScope))
				{
					/* Push the initializer into the body scope */
					body = $8;
					((ILNode_NewScope *)body)->stmt =
						ILNode_Compound_CreateFrom
							(initializer, ((ILNode_NewScope *)body)->stmt);
				}
				else if($8 || (attrs & CS_SPECIALATTR_EXTERN) == 0)
				{
					/* Non-scoped body: create a new scoped body */
					body = ILNode_NewScope_create
								(ILNode_Compound_CreateFrom(initializer, $8));
					CCWarningOnLine(yygetfilename($3), yygetlinenum($3),
						"constructor without body should be declared 'extern'");
				}
				else
				{
					/* Extern constructor with an empty body */
					body = 0;
				}
				if((attrs & IL_META_METHODDEF_STATIC) != 0)
				{
					if(!yyisa($5,ILNode_Empty))
					{
						CCErrorOnLine(yygetfilename($3), yygetlinenum($3),
								"Static constructors cannot have parameters");
					}
					$$.body = 0;
					$$.staticCtors = body;
				}
				else
				{
					$$.body = ILNode_MethodDeclaration_create
						  ($1, attrs, 0 /* "void" */, cname, $5, body);
					CloneLine($$.body, $3);
					$$.staticCtors = 0;
				}
			}
	;

ConstructorInitializer
	: /* empty */							{
				$$ = ILNode_Compound_CreateFrom
						(ILNode_NonStaticInit_create(),
						 ILNode_InvocationExpression_create
							(ILNode_BaseInit_create(), 0));
			}
	| ':' BASE '(' OptArgumentList ')'		{
				$$ = ILNode_Compound_CreateFrom
						(ILNode_NonStaticInit_create(),
						 ILNode_InvocationExpression_create
							(ILNode_BaseInit_create(), $4));
			}
	| ':' THIS '(' OptArgumentList ')'		{
				MakeBinary(InvocationExpression, ILNode_ThisInit_create(), $4);
			}
	;

DestructorDeclaration
	: OptAttributes OptModifiers '~' QualifiedIdentifierPart '(' ')' Block		{
				ILUInt32 attrs;
				ILNode *dtorName;
				ILNode *name;
				ILNode *body;

				/* Destructors cannot have type parameters */
				dtorName = $4;
				if(yyisa(dtorName, ILNode_GenericReference))
				{
					CCErrorOnLine(yygetfilename($4), yygetlinenum($4),
						"destructors cannot have type parameters");
					dtorName = ((ILNode_GenericReference *)dtorName)->type;
				}

				/* Validate the destructor name */
				if(!ClassNameSame(dtorName))
				{
					CCErrorOnLine(yygetfilename($4), yygetlinenum($4),
						"destructor name does not match class name");
				}

				/* Build the list of attributes needed on "Finalize" */
				attrs = CSModifiersToDestructorAttrs($4,$2);

				/* Build the name of the "Finalize" method */
				name = ILQualIdentSimple(ILInternString("Finalize", -1).string);

				/* Destructors must always call their parent finalizer
				   even if an exception occurs.  We force this to happen
				   by wrapping the method body with a try block whose
				   finally clause always calls its parent */
				/* Note: BaseDestructor filters out these calls for 
						 System.Object class */
				body = ILNode_BaseDestructor_create(
							ILNode_InvocationExpression_create
							(ILNode_BaseAccess_create(name), 0));
				body = ILNode_Try_create
							($7, 0, ILNode_FinallyClause_create(body));

				/* Construct the finalizer declaration */
				$$ = ILNode_MethodDeclaration_create
							($1, attrs, 0 /* void */,
							 ILQualIdentSimple
							 	(ILInternString("Finalize", -1).string),
							 0, body);
				CloneLine($$, $4);
			}
	;

/*
 * Structs.
 */

StructDeclaration
	: OptAttributes OptModifiers OptPartial STRUCT Identifier TypeFormals
			StructInterfaces Constraints {
				/* Enter a new nesting level */
				++NestingLevel;

				/* Push the identifier onto the class name stack */
				ClassNamePush($5);
			}
			StructBody OptSemiColon	{
				ILNode *baseList;
				ILUInt32 attrs;

				/* Validate the modifiers */
				attrs = CSModifiersToTypeAttrs($5, $2, (NestingLevel > 1));

				/* Add extra attributes that structs need */
				attrs |= IL_META_TYPEDEF_LAYOUT_SEQUENTIAL |
						 IL_META_TYPEDEF_SERIALIZABLE |
						 IL_META_TYPEDEF_SEALED;

				/* Exit the current nesting level */
				--NestingLevel;

				/* Make sure that we have "ValueType" in the base list */
				baseList = MakeSystemType("ValueType");
				if($7 != 0)
				{
					baseList = ILNode_ArgList_create($7, baseList);
				}

				/* Create the class definition */
				InitGlobalNamespace();
				$$ = ILNode_ClassDefn_create
							($1,					/* OptAttributes */
							 attrs,					/* OptModifiers */
							 ILQualIdentName($5, 0),/* Identifier */
							 CurrNamespace.string,	/* Namespace */
							 (ILNode *)CurrNamespaceNode,
							 $6,					/* TypeFormals */
							 baseList,				/* ClassBase */
							 ($10).body,				/* StructBody */
							 ($10).staticCtors);		/* StaticCtors */
				CloneLine($$, $5);

				/* Pop the class name stack */
				ClassNamePop();

				/* We have declarations at the top-most level of the file */
				HaveDecls = 1;
			}
	;

StructInterfaces
	: /* empty */			{ $$ = 0; }
	| ':' TypeList			{ $$ = $2; }
	;

StructBody
	: '{' OptClassMemberDeclarations '}'	{ $$ = $2; }
	| '{' error '}'		{
				/*
				 * This production recovers from errors in struct declarations.
				 */
				$$.body = 0;
				$$.staticCtors = 0;
				yyerrok;
			}
	;

/*
 * Interfaces.
 */

InterfaceDeclaration
	: OptAttributes OptModifiers OptPartial INTERFACE Identifier TypeFormals
			InterfaceBase Constraints {
				/* Increase the nesting level */
				++NestingLevel;

				/* Push the identifier onto the class name stack */
				ClassNamePush($5);
			}
			InterfaceBody OptSemiColon	{
				/* Validate the modifiers */
				ILUInt32 attrs =
					CSModifiersToTypeAttrs($5, $2, (NestingLevel > 1));

				/* Add extra attributes that interfaces need */
				attrs |= IL_META_TYPEDEF_INTERFACE |
						 IL_META_TYPEDEF_ABSTRACT;

				/* Exit from the current nesting level */
				--NestingLevel;

				/* Create the interface definition */
				InitGlobalNamespace();
				$$ = ILNode_ClassDefn_create
							($1,					/* OptAttributes */
							 attrs,					/* OptModifiers */
							 ILQualIdentName($5, 0),/* Identifier */
							 CurrNamespace.string,	/* Namespace */
							 (ILNode *)CurrNamespaceNode,
							 $6,					/* TypeFormals */
							 $7,					/* ClassBase */
							 $10,					/* InterfaceBody */
							 0);					/* StaticCtors */
				CloneLine($$, $5);

				/* Pop the class name stack */
				ClassNamePop();

				/* We have declarations at the top-most level of the file */
				HaveDecls = 1;
			}
	;

InterfaceBase
	: /* empty */	{ $$ = 0; }
	| ':' TypeList	{ $$ = $2; }
	;

InterfaceBody
	: '{' OptInterfaceMemberDeclarations '}'		{ $$ = $2;}
	| '{' error '}'		{
				/*
				 * This production recovers from errors in interface
				 * declarations.
				 */
				$$ = 0;
				yyerrok;
			}
	;

OptInterfaceMemberDeclarations
	: /* empty */						{ $$ = 0;}
	| InterfaceMemberDeclarations		{ $$ = $1;}
	;

InterfaceMemberDeclarations
	: InterfaceMemberDeclaration		{
				$$ = ILNode_List_create();
				ILNode_List_Add($$, $1);
			}
	| InterfaceMemberDeclarations InterfaceMemberDeclaration	{
				ILNode_List_Add($1, $2);
				$$ = $1;
			}
	;

InterfaceMemberDeclaration
	: InterfaceMethodDeclaration		{ $$ = $1;}
	| InterfacePropertyDeclaration		{ $$ = $1;}
	| InterfaceEventDeclaration			{ $$ = $1;}
	| InterfaceIndexerDeclaration		{ $$ = $1;}
	;

InterfaceMethodDeclaration
	: OptAttributes OptNew Type Identifier '(' OptFormalParameterList ')' ';' {
				ILUInt32 attrs = ($2 ? CS_SPECIALATTR_NEW : 0) |
								 IL_META_METHODDEF_PUBLIC |
								 IL_META_METHODDEF_VIRTUAL |
								 IL_META_METHODDEF_ABSTRACT |
								 IL_META_METHODDEF_HIDE_BY_SIG |
								 IL_META_METHODDEF_NEW_SLOT;
				$$ = ILNode_MethodDeclaration_create
						($1, attrs, $3, $4, $6, 0);
				CloneLine($$, $4);
			}
	;

OptNew
	: /* empty */	{ $$ = 0; }
	| NEW 			{ $$ = 1; }
	;

InterfacePropertyDeclaration
	: OptAttributes OptNew Type Identifier
			StartInterfaceAccessorBody InterfaceAccessorBody	{
				ILUInt32 attrs = ($2 ? CS_SPECIALATTR_NEW : 0) |
								 IL_META_METHODDEF_PUBLIC |
								 IL_META_METHODDEF_VIRTUAL |
								 IL_META_METHODDEF_ABSTRACT |
								 IL_META_METHODDEF_HIDE_BY_SIG |
								 IL_META_METHODDEF_SPECIAL_NAME |
								 IL_META_METHODDEF_NEW_SLOT;
				$$ = ILNode_PropertyDeclaration_create
								($1, attrs, $3, $4, 0, 0, 0, $6);
				CloneLine($$, $4);

				/* Create the property method declarations */
				CreatePropertyMethods((ILNode_PropertyDeclaration *)($$));
			}
	;

StartInterfaceAccessorBody
	: '{'
	;

InterfaceAccessorBody
	: InterfaceAccessors '}'	{
				$$ = $1;
			}
	| error '}'		{
				/*
				 * This production recovers from errors in interface
				 * accessor declarations.
				 */
				$$ = 0;
				yyerrok;
			}
	;

InterfaceAccessors
	: GET ';'				{ $$ = 1; }
	| SET ';'				{ $$ = 2; }
	| GET ';' SET ';'		{ $$ = 3; }
	| SET ';' GET ';'		{ $$ = 3; }
	;

InterfaceEventDeclaration
	: OptAttributes OptNew EVENT Type Identifier ';'		{
				ILUInt32 attrs = ($2 ? CS_SPECIALATTR_NEW : 0) |
								 IL_META_METHODDEF_PUBLIC |
								 IL_META_METHODDEF_VIRTUAL |
								 IL_META_METHODDEF_ABSTRACT |
								 IL_META_METHODDEF_HIDE_BY_SIG |
								 IL_META_METHODDEF_NEW_SLOT;
				$$ = ILNode_EventDeclaration_create
							($1, attrs, $4,
							 ILNode_EventDeclarator_create
							 	(ILNode_FieldDeclarator_create($5, 0), 0, 0));
				CreateEventMethods((ILNode_EventDeclaration *)($$));
			}
	;

InterfaceIndexerDeclaration
	: OptAttributes OptNew Type THIS FormalIndexParameters
			StartInterfaceAccessorBody InterfaceAccessorBody	{
				ILUInt32 attrs = ($2 ? CS_SPECIALATTR_NEW : 0) |
								 IL_META_METHODDEF_PUBLIC |
								 IL_META_METHODDEF_VIRTUAL |
								 IL_META_METHODDEF_ABSTRACT |
								 IL_META_METHODDEF_HIDE_BY_SIG |
								 IL_META_METHODDEF_SPECIAL_NAME |
								 IL_META_METHODDEF_NEW_SLOT;
				ILNode* name=GetIndexerName(&CCCodeGen,(ILNode_AttributeTree*)$1,
								ILQualIdentSimple(NULL));
				$$ = ILNode_PropertyDeclaration_create
								($1, attrs, $3, name, $5, 0, 0, $7);
				CloneLine($$, $3);

				/* Create the property method declarations */
				CreatePropertyMethods((ILNode_PropertyDeclaration *)($$));
			}
	;

/*
 * Enums.
 */

EnumDeclaration
	: OptAttributes OptModifiers ENUM Identifier EnumBase {
				/* Enter a new nesting level */
				++NestingLevel;

				/* Push the identifier onto the class name stack */
				ClassNamePush($4);
			}
			EnumBody OptSemiColon	{
				ILNode *baseList;
				ILNode *bodyList;
				ILNode *fieldDecl;
				ILUInt32 attrs;

				/* Validate the modifiers */
				attrs = CSModifiersToTypeAttrs($4, $2, (NestingLevel > 1));

				/* Add extra attributes that enums need */
				attrs |= IL_META_TYPEDEF_SERIALIZABLE |
						 IL_META_TYPEDEF_SEALED;

				/* Exit the current nesting level */
				--NestingLevel;

				/* Make sure that we have "Enum" in the base list */
				baseList = MakeSystemType("Enum");

				/* Add an instance field called "value__" to the body,
				   which is used to hold the enumerated value */
				bodyList = $7;
				if(!bodyList)
				{
					bodyList = ILNode_List_create();
				}
				fieldDecl = ILNode_List_create();
				ILNode_List_Add(fieldDecl,
					ILNode_FieldDeclarator_create
						(ILQualIdentSimple("value__"), 0));
				MakeBinary(FieldDeclarator, $1, 0);
				ILNode_List_Add(bodyList,
					ILNode_FieldDeclaration_create
						(0, IL_META_FIELDDEF_PUBLIC |
							IL_META_FIELDDEF_SPECIAL_NAME |
							IL_META_FIELDDEF_RT_SPECIAL_NAME, $5, fieldDecl));

				/* Create the class definition */
				InitGlobalNamespace();
				$$ = ILNode_ClassDefn_create
							($1,					/* OptAttributes */
							 attrs,					/* OptModifiers */
							 ILQualIdentName($4, 0),/* Identifier */
							 CurrNamespace.string,	/* Namespace */
							 (ILNode *)CurrNamespaceNode,
							 0,						/* TypeFormals */
							 baseList,				/* ClassBase */
							 bodyList,				/* EnumBody */
							 0);					/* StaticCtors */
				CloneLine($$, $4);

				/* Pop the class name stack */
				ClassNamePop();

				/* We have declarations at the top-most level of the file */
				HaveDecls = 1;
			}
	;

EnumBase
	: /* empty */			{ MakeUnary(PrimitiveType, IL_META_ELEMTYPE_I4); }
	| ':' EnumBaseType		{ $$ = $2; }
	;

EnumBaseType
	: BYTE					{ MakeUnary(PrimitiveType, IL_META_ELEMTYPE_U1); }
	| SBYTE					{ MakeUnary(PrimitiveType, IL_META_ELEMTYPE_I1); }
	| SHORT					{ MakeUnary(PrimitiveType, IL_META_ELEMTYPE_I2); }
	| USHORT				{ MakeUnary(PrimitiveType, IL_META_ELEMTYPE_U2); }
	| INT					{ MakeUnary(PrimitiveType, IL_META_ELEMTYPE_I4); }
	| UINT					{ MakeUnary(PrimitiveType, IL_META_ELEMTYPE_U4); }
	| LONG					{ MakeUnary(PrimitiveType, IL_META_ELEMTYPE_I8); }
	| ULONG					{ MakeUnary(PrimitiveType, IL_META_ELEMTYPE_U8); }
	;

EnumBody
	: '{' OptEnumMemberDeclarations '}'				{
				$$ = $2;
			}
	| '{' EnumMemberDeclarations ',' '}'				{
				$$ = $2;
			}
	| '{' error '}'		{
				/*
				 * This production recovers from errors in enum declarations.
				 */
				$$ = 0;
				yyerrok;
			}
	;

OptEnumMemberDeclarations
	: /* empty */				{ $$ = 0;}
	| EnumMemberDeclarations	{ $$ = $1;}
	;

EnumMemberDeclarations
	: EnumMemberDeclaration		{
			$$ = ILNode_List_create ();
			ILNode_List_Add($$, $1);
		}
	| EnumMemberDeclarations ',' EnumMemberDeclaration	{
			ILNode_List_Add($1, $3);
			$$ = $1;
		}
	;

EnumMemberDeclaration
	: OptAttributes Identifier		{
			$$ = ILNode_EnumMemberDeclaration_create($1, $2, 0);
		}
	| OptAttributes Identifier '=' ConstantExpression	{
			$$ = ILNode_EnumMemberDeclaration_create($1, $2, $4);
		}
	;

/*
 * Delegates.
 */

DelegateDeclaration
	: OptAttributes OptModifiers DELEGATE Type Identifier TypeFormals
				'(' OptFormalParameterList ')' ';'	{
				ILNode *baseList;
				ILNode *bodyList;
				ILUInt32 attrs;

				/* Validate the modifiers */
				attrs = CSModifiersToDelegateAttrs($5, $2, (NestingLevel > 0));

				/* Make sure that we have "MulticastDelegate"
				   in the base list */
				baseList = MakeSystemType("MulticastDelegate");

				/* Construct the body of the delegate class */
				bodyList = ILNode_List_create();
				ILNode_List_Add(bodyList,
					ILNode_DelegateMemberDeclaration_create($4, $8));

				/* Create the class definition */
				InitGlobalNamespace();
				$$ = ILNode_ClassDefn_create
							($1,					/* OptAttributes */
							 attrs,					/* OptModifiers */
							 ILQualIdentName($5, 0),/* Identifier */
							 CurrNamespace.string,	/* Namespace */
							 (ILNode *)CurrNamespaceNode,
							 $6,					/* TypeFormals */
							 baseList,				/* ClassBase */
							 bodyList,				/* Body */
							 0);					/* StaticCtors */
				CloneLine($$, $5);

				/* We have declarations at the top-most level of the file */
				HaveDecls = 1;
			}
	;

/*
 * Anonymous method declarations.
 */

AnonymousMethod
	: Block			{
				$$ = ILNode_Null_create();
				CCError(_("anonymous methods are not yet supported"));
			}
	| '(' OptFormalParameterList ')' Block	{
				$$ = ILNode_Null_create();
				CCError(_("anonymous methods are not yet supported"));
			}
	;
