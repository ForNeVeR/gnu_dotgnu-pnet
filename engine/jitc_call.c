/*
 * jitc_call.c - Coder implementation for JIT method calls.
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

#ifdef IL_JITC_CODE

static void JITCoder_UpConvertArg(ILCoder *coder, ILEngineStackItem *args,
						          ILUInt32 numArgs, ILUInt32 param,
						          ILType *paramType)
{
}

static void JITCoder_DownConvertArg(ILCoder *coder, ILEngineStackItem *args,
						            ILUInt32 numArgs, ILUInt32 param,
						            ILType *paramType)
{
}

static void JITCoder_PackVarArgs(ILCoder *coder, ILType *callSiteSig,
					             ILUInt32 firstParam, ILEngineStackItem *args,
						         ILUInt32 numArgs)
{
}

static void JITCoder_ValueCtorArgs(ILCoder *coder, ILClass *classInfo,
								   ILEngineStackItem *args, ILUInt32 numArgs)
{
}

static void JITCoder_CheckCallNull(ILCoder *coder, ILCoderMethodInfo *info)
{
}

static void JITCoder_CallMethod(ILCoder *coder, ILCoderMethodInfo *info,
								ILEngineStackItem *returnItem,
								ILMethod *methodInfo)
{
}

static void JITCoder_CallIndirect(ILCoder *coder, ILCoderMethodInfo *info,
								  ILEngineStackItem *returnItem)
{
}

static void JITCoder_CallCtor(ILCoder *coder, ILCoderMethodInfo *info,
					   		  ILMethod *methodInfo)
{
}

static void JITCoder_CallVirtual(ILCoder *coder, ILCoderMethodInfo *info,
								 ILEngineStackItem *returnItem,
								 ILMethod *methodInfo)
{
}

static void JITCoder_CallInterface(ILCoder *coder, ILCoderMethodInfo *info,
								   ILEngineStackItem *returnItem,
								   ILMethod *methodInfo)
{
}

static int JITCoder_CallInlineable(ILCoder *coder, int inlineType,
								   ILMethod *methodInfo)
{
	return 0;
}

static void JITCoder_JumpMethod(ILCoder *coder, ILMethod *methodInfo)
{
}

static void JITCoder_ReturnInsn(ILCoder *coder, ILEngineType engineType,
							    ILType *returnType)
{
}

static void JITCoder_LoadFuncAddr(ILCoder *coder, ILMethod *methodInfo)
{
}

static void JITCoder_LoadVirtualAddr(ILCoder *coder, ILMethod *methodInfo)
{
}

static void JITCoder_LoadInterfaceAddr(ILCoder *coder, ILMethod *methodInfo)
{
}

#endif	/* IL_JITC_CODE */
