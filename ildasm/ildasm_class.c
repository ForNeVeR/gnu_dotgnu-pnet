/*
 * ildasm_class.c - Disassemble class information.
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

#include "ildasm_internal.h"

#ifdef	__cplusplus
extern	"C" {
#endif

/*
 * Dump a method definition.
 */
static void Dump_MethodDef(ILImage *image, FILE *outstream, int flags,
						   ILMethod *method)
{
	ILUInt32 rva;
	ILPInvoke *pinvoke;
	ILOverride *over;

	/* Dump the header information for the method */
	fputs("\t.method ", outstream);
	if((flags & IL_DUMP_SHOW_TOKENS) != 0)
	{
		fprintf(outstream, "/*%08lX*/ ",
				(unsigned long)(ILMethod_Token(method)));
	}
	ILDumpFlags(outstream, ILMethod_Attrs(method), ILMethodDefinitionFlags, 0);
	if(ILMethod_HasPInvokeImpl(method))
	{
		pinvoke = ILPInvokeFind(method);
		if(pinvoke)
		{
			fputs("pinvokeimpl(", outstream);
			ILDumpString(outstream,
						 ILModule_Name(ILPInvoke_Module(pinvoke)));
			putc(' ', outstream);
			if(strcmp(ILPInvoke_Alias(pinvoke), ILMethod_Name(method)) != 0)
			{
				fputs("as ", outstream);
				ILDumpString(outstream, ILPInvoke_Alias(pinvoke));
				putc(' ', outstream);
			}
			ILDumpFlags(outstream, ILPInvoke_Attrs(pinvoke),
						ILPInvokeImplementationFlags, 0);
			fputs(") ", outstream);
		}
		else
		{
			fputs("pinvokeimpl() ", outstream);
		}
	}
	ILDumpMethodType(outstream, image, ILMethod_Signature(method), flags,
					 0, ILMethod_Name(method), method);
	putc(' ', outstream);
	ILDumpFlags(outstream, ILMethod_ImplAttrs(method),
				ILMethodImplementationFlags, 0);
	rva = ILMethod_RVA(method);
	if(ILProgramItem_HasAttrs(method) || rva)
	{
		fputs("\n\t{\n", outstream);
	}
	else
	{
		fputs("{}\n", outstream);
	}
	ILDAsmDumpCustomAttrs(image, outstream, flags, 2, ILToProgramItem(method));

	/* Dump the security information, if any */
	if((ILMethod_Attrs(method) & IL_META_METHODDEF_HAS_SECURITY) != 0)
	{
		ILDAsmDumpSecurity(image, outstream, (ILProgramItem *)method, flags);
	}
	
	/* If this a body for an override, then declare it */
	over = ILOverrideFromMethod(method);
	if(over)
	{
		ILMethod *decl = ILOverride_Decl(over);
		fputs("\t\t.override ", outstream);
		ILDumpClassName(outstream, image, ILMethod_Owner(decl), flags);
		fputs("::", outstream);
		ILDumpIdentifier(outstream, ILMethod_Name(decl), 0, flags);
		putc('\n', outstream);
	}

	/* If we have an RVA, then we need to dump the method's contents */
	if(rva)
	{
		ILDAsmDumpMethod(image, outstream, method, flags,
						 (ILMethod_Token(method) ==
						 		ILImageGetEntryPoint(image)));
	}

	/* Output the method footer and exit */
	if(ILProgramItem_HasAttrs(method) || rva)
	{
		fputs("\t}\n", outstream);
	}
}

/*
 * Dump a field definition.
 */
static void Dump_FieldDef(ILImage *image, FILE *outstream, int flags,
						  ILField *field)
{
	ILFieldLayout *layout;
	fputs("\t.field ", outstream);
	if((flags & IL_DUMP_SHOW_TOKENS) != 0)
	{
		fprintf(outstream, "/*%08lX*/ ",
				(unsigned long)(ILField_Token(field)));
	}
	layout = ILFieldLayoutGetFromOwner(field);
	if(layout)
	{
		fprintf(outstream, "[%lu] ",
				(unsigned long)(ILFieldLayout_Offset(layout)));
	}
	ILDumpFlags(outstream, ILField_Attrs(field), ILFieldDefinitionFlags, 0);
	if((ILField_Attrs(field) & IL_META_FIELDDEF_HAS_FIELD_MARSHAL) != 0)
	{
		ILFieldMarshal *marshal = ILFieldMarshalGetFromOwner
										((ILProgramItem *)field);
		if(marshal)
		{
			const void *type;
			unsigned long typeLen;
			type = ILFieldMarshalGetType(marshal, &typeLen);
			if(type)
			{
				fputs("marshal(", outstream);
				ILDumpNativeType(outstream, type, typeLen);
				fputs(") ", outstream);
			}
		}
	}
	ILDumpType(outstream, image, ILFieldGetTypeWithPrefixes(field), flags);
	putc(' ', outstream);
	ILDumpIdentifier(outstream, ILField_Name(field), 0, flags);
	if((ILField_Attrs(field) & IL_META_FIELDDEF_HAS_FIELD_RVA) != 0)
	{
		ILFieldRVA *rva = ILFieldRVAGetFromOwner(field);
		if(rva)
		{
			fprintf(outstream, " at 0x%08lX",
					(unsigned long)(ILFieldRVA_RVA(rva)));
		}
	}
	if((ILField_Attrs(field) & IL_META_FIELDDEF_HAS_DEFAULT) != 0)
	{
		ILDumpConstant(outstream, (ILProgramItem *)field, 0);
	}
	putc('\n', outstream);
	if(ILProgramItem_HasAttrs(field))
	{
		ILDAsmDumpCustomAttrs(image, outstream, flags, 1,
							  ILToProgramItem(field));
	}
}

/*
 * Dump a method association for an event or property.
 */
static void DumpMethodAssociation(ILImage *image, FILE *outstream,
								  int flags, ILMethod *method)
{
	ILDumpMethodType(outstream, image, ILMethod_Signature(method), flags,
					 ILMethod_Owner(method), ILMethod_Name(method), method);
}

/*
 * Dump an event definition.
 */
static void Dump_EventDef(ILImage *image, FILE *outstream, int flags,
						  ILEvent *event)
{
	ILMethod *method;

	/* Dump the event header */
	fputs("\t.event ", outstream);
	if((flags & IL_DUMP_SHOW_TOKENS) != 0)
	{
		fprintf(outstream, "/*%08lX*/ ",
				(unsigned long)(ILEvent_Token(event)));
	}
	ILDumpFlags(outstream, ILEvent_Attrs(event), ILEventDefinitionFlags, 0);
	if(ILEvent_Type(event) != ILType_Invalid)
	{
		ILDumpType(outstream, image, ILEvent_Type(event), flags);
		putc(' ', outstream);
	}
	ILDumpIdentifier(outstream, ILEvent_Name(event), 0, flags);
	fputs("\n\t{\n", outstream);

	/* Dump the custom attributes */
	if(ILProgramItem_HasAttrs(event))
	{
		ILDAsmDumpCustomAttrs(image, outstream, flags, 1,
							  ILToProgramItem(event));
	}

	/* Dump the event methods */
	if((method = ILEvent_AddOn(event)) != 0)
	{
		fputs("\t\t.addon ", outstream);
		DumpMethodAssociation(image, outstream, flags, method);
		putc('\n', outstream);
	}
	if((method = ILEvent_RemoveOn(event)) != 0)
	{
		fputs("\t\t.removeon ", outstream);
		DumpMethodAssociation(image, outstream, flags, method);
		putc('\n', outstream);
	}
	if((method = ILEvent_Fire(event)) != 0)
	{
		fputs("\t\t.fire ", outstream);
		DumpMethodAssociation(image, outstream, flags, method);
		putc('\n', outstream);
	}
	if((method = ILEvent_Other(event)) != 0)
	{
		fputs("\t\t.other ", outstream);
		DumpMethodAssociation(image, outstream, flags, method);
		putc('\n', outstream);
	}

	/* Dump the event footer */
	fputs("\t}\n", outstream);
}

/*
 * Dump a property definition.
 */
static void Dump_PropertyDef(ILImage *image, FILE *outstream, int flags,
						     ILProperty *property)
{
	ILMethod *method;

	/* Dump the property header */
	fputs("\t.property ", outstream);
	if((flags & IL_DUMP_SHOW_TOKENS) != 0)
	{
		fprintf(outstream, "/*%08lX*/ ",
				(unsigned long)(ILProperty_Token(property)));
	}
	ILDumpFlags(outstream, ILProperty_Attrs(property),
				ILPropertyDefinitionFlags, 0);

	/* Dump the calling conventions from the get/set method,
	   because the property signature doesn't contain them */
	if((method = ILProperty_Getter(property)) != 0)
	{
		ILDumpFlags(outstream, ILMethod_CallConv(method),
					ILMethodCallConvFlags, 0);
	}
	else if((method = ILProperty_Setter(property)) != 0)
	{
		ILDumpFlags(outstream, ILMethod_CallConv(method),
					ILMethodCallConvFlags, 0);
	}

	/* Dump the property type */
	ILDumpMethodType(outstream, image, ILProperty_Signature(property), flags,
					 0, ILProperty_Name(property), 0);
	fputs("\n\t{\n", outstream);

	/* Dump the custom attributes */
	if(ILProgramItem_HasAttrs(property))
	{
		ILDAsmDumpCustomAttrs(image, outstream, flags, 1,
							  ILToProgramItem(property));
	}

	/* Dump the property methods */
	if((method = ILProperty_Getter(property)) != 0)
	{
		fputs("\t\t.get ", outstream);
		DumpMethodAssociation(image, outstream, flags, method);
		putc('\n', outstream);
	}
	if((method = ILProperty_Setter(property)) != 0)
	{
		fputs("\t\t.set ", outstream);
		DumpMethodAssociation(image, outstream, flags, method);
		putc('\n', outstream);
	}
	if((method = ILProperty_Other(property)) != 0)
	{
		fputs("\t\t.other ", outstream);
		DumpMethodAssociation(image, outstream, flags, method);
		putc('\n', outstream);
	}

	/* Dump the event footer */
	fputs("\t}\n", outstream);
}

/*
 * Dump information about a type definition and its nested classes.
 */
static void Dump_TypeAndNested(ILImage *image, FILE *outstream,
							   int flags, ILClass *info)
{
	ILMember *member;
	ILImplements *impl;
	ILClass *interface;
	ILNestedInfo *nested;
	int first;
	int isModule = 0;
	ILClassLayout *layout;
	unsigned long size;
	ILOverride *over;
	ILMethod *decl;
	ILMethod *body;

	/* Dump the type header, if it is not "<Module>" */
	if(strcmp(ILClass_Name(info), "<Module>") != 0 ||
	   ILClass_Namespace(info) != 0)
	{
		fputs(".class ", outstream);
		ILDumpFlags(outstream, ILClass_Attrs(info), ILTypeDefinitionFlags, 0);
		ILDumpClassName(outstream, image, info, flags);
		if(ILClass_Parent(info))
		{
			fputs(" extends ", outstream);
			ILDumpClassName(outstream, image, ILClass_Parent(info), flags);
		}
		first = 1;
		impl = 0;
		while((impl = ILClassNextImplements(info, impl)) != 0)
		{
			interface = ILImplementsGetInterface(impl);
			if(first)
			{
				fputs(" implements ", outstream);
				first = 0;
			}
			else
			{
				fputs(", ", outstream);
			}
			ILDumpClassName(outstream, image, interface, flags);
		}
		fputs("\n{\n", outstream);

		/* Dump the security information, if any */
		if((ILClass_Attrs(info) & IL_META_TYPEDEF_HAS_SECURITY) != 0)
		{
			ILDAsmDumpSecurity(image, outstream, (ILProgramItem *)info, flags);
		}

		/* Dump the class layout information, if any */
		layout = ILClassLayoutGetFromOwner(info);
		if(layout)
		{
			size = (unsigned long)(ILClassLayout_PackingSize(layout));
			if(size != 0)
			{
				fprintf(outstream, "\t.pack %lu\n", size);
			}
			size = (unsigned long)(ILClassLayout_ClassSize(layout));
			if(size != 0)
			{
				fprintf(outstream, "\t.size %lu\n", size);
			}
		}

		/* Dump the custom attributes for the class */
		if(ILProgramItem_HasAttrs(info))
		{
			ILDAsmDumpCustomAttrs(image, outstream, flags, 1,
								  ILToProgramItem(info));
		}
	}
	else
	{
		isModule = 1;
	}

	/* Dump the nested classes */
	nested = 0;
	while((nested = ILClassNextNested(info, nested)) != 0)
	{
		Dump_TypeAndNested(image, outstream, flags,
						   ILNestedInfoGetChild(nested));
	}

	/* Dump the class members */
	member = 0;
	while((member = ILClassNextMember(info, member)) != 0)
	{
		switch(ILMemberGetKind(member))
		{
			case IL_META_MEMBERKIND_METHOD:
			{
				Dump_MethodDef(image, outstream, flags, (ILMethod *)member);
			}
			break;

			case IL_META_MEMBERKIND_FIELD:
			{
				Dump_FieldDef(image, outstream, flags, (ILField *)member);
			}
			break;

			case IL_META_MEMBERKIND_EVENT:
			{
				Dump_EventDef(image, outstream, flags, (ILEvent *)member);
			}
			break;

			case IL_META_MEMBERKIND_PROPERTY:
			{
				Dump_PropertyDef(image, outstream, flags, (ILProperty *)member);
			}
			break;
		}
	}

	/* Dump overrides that don't have bodies in this class */
	over = 0;
	while((over = (ILOverride *)ILClassNextMemberByKind
				(info, (ILMember *)over, IL_META_MEMBERKIND_OVERRIDE)) != 0)
	{
		body = ILOverride_Body(over);
		if(ILMethod_Owner(body) != info)
		{
			decl = ILOverride_Decl(over);
			fputs("\t.override ", outstream);
			ILDumpClassName(outstream, image, ILMethod_Owner(decl), flags);
			fputs("::", outstream);
			ILDumpIdentifier(outstream, ILMethod_Name(decl), 0, flags);
			fputs(" with ", outstream);
			ILDumpMethodType(outstream, image, ILMethod_Signature(body), flags,
							 ILMethod_Owner(body), ILMethod_Name(body), body);
			putc('\n', outstream);
		}
	}

	/* Dump the type footer, if it is not "<Module>" */
	if(!isModule)
	{
		fputs("}\n", outstream);
	}
}

/*
 * Dump information about a type definition.
 */
static void Dump_TypeDef(ILImage *image, FILE *outstream, int flags,
						 unsigned long token, ILClass *info,
						 unsigned long refToken)
{
	/* Ignore the type if it is nested: we'll get it elsewhere */
	if(ILClass_IsPublic(info) || ILClass_IsPrivate(info))
	{
		Dump_TypeAndNested(image, outstream, flags, info);
	}
}

void ILDAsmDumpClasses(ILImage *image, FILE *outstream, int flags)
{
	ILDAsmWalkTokens(image, outstream, flags,
					 IL_META_TOKEN_TYPE_DEF,
					 (ILDAsmWalkFunc)Dump_TypeDef, 0);
}

#ifdef	__cplusplus
};
#endif
