/*
 * ILGenerator.cs - Implementation of "System.Reflection.Emit.ILGenerator" 
 *
 * Copyright (C) 2002  Southern Storm Software, Pty Ltd.
 * 
 * Contributed by Gopal.V <gopalv82@symonds.net> 
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

using System;
using System.Diagnostics.SymbolStore;

#if !ECMA_COMPAT

namespace System.Reflection.Emit
{
	public class ILGenerator
	{
		[TODO]
		public virtual void BeginCatchBlock(System.Type exceptionType)
		{
			throw new NotImplementedException("BeginCatchBlock");
		}

		[TODO]
		public virtual void BeginExceptFilterBlock()
		{
			throw new NotImplementedException("BeginExceptFilterBlock");
		}

		[TODO]
		public virtual Label BeginExceptionBlock()
		{
			throw new NotImplementedException("BeginExceptionBlock");
		}

		[TODO]
		public virtual void BeginFaultBlock()
		{
			throw new NotImplementedException("BeginFaultBlock");
		}

		[TODO]
		public virtual void BeginFinallyBlock()
		{
			throw new NotImplementedException("BeginFinallyBlock");
		}

		[TODO]
		public virtual void BeginScope()
		{
			throw new NotImplementedException("BeginScope");
		}

		[TODO]
		public LocalBuilder DeclareLocal(Type localType)
		{
			throw new NotImplementedException("DeclareLocal");
		}

		[TODO]
		public virtual Label DefineLabel()
		{
			throw new NotImplementedException("DefineLabel");
		}

		[TODO]
		public virtual void Emit(OpCode opcode)
		{
			throw new NotImplementedException("Emit");
		}

		[TODO]
		public virtual void Emit(OpCode opcode, byte val)
		{
			throw new NotImplementedException("Emit");
		}

		[TODO]
		public virtual void Emit(OpCode opcode, ConstructorInfo constructor)
		{
			throw new NotImplementedException("Emit");
		}

		[TODO]
		public virtual void Emit(OpCode opcode, double val)
		{
			throw new NotImplementedException("Emit");
		}

		[TODO]
		public virtual void Emit(OpCode opcode, FieldInfo field)
		{
			throw new NotImplementedException("Emit");
		}

		[TODO]
		public virtual void Emit(OpCode opcode, short val)
		{
			throw new NotImplementedException("Emit");
		}

		[TODO]
		public virtual void Emit(OpCode opcode, int val)
		{
			throw new NotImplementedException("Emit");
		}

		[TODO]
		public virtual void Emit(OpCode opcode, long val)
		{
			throw new NotImplementedException("Emit");
		}

		[TODO]
		public virtual void Emit(OpCode opcode, Label label)
		{
			throw new NotImplementedException("Emit");
		}

		[TODO]
		public virtual void Emit(OpCode opcode, Label[] labels)
		{
			throw new NotImplementedException("Emit");
		}

		[TODO]
		public virtual void Emit(OpCode opcode, LocalBuilder lbuilder)
		{
			throw new NotImplementedException("Emit");
		}

		[TODO]
		public virtual void Emit(OpCode opcode, MethodInfo method)
		{
			throw new NotImplementedException("Emit");
		}

		[TODO]
		public void Emit(OpCode opcode, sbyte val)
		{
			throw new NotImplementedException("Emit");
		}

		[TODO]
		public virtual void Emit(OpCode opcode, SignatureHelper shelper)
		{
			throw new NotImplementedException("Emit");
		}

		[TODO]
		public virtual void Emit(OpCode opcode, float val)
		{
			throw new NotImplementedException("Emit");
		}

		[TODO]
		public virtual void Emit(OpCode opcode, String val)
		{
			throw new NotImplementedException("Emit");
		}

		[TODO]
		public virtual void Emit(OpCode opcode, Type type)
		{
			throw new NotImplementedException("Emit");
		}

		[TODO]
		public void EmitCall(OpCode opcode, MethodInfo methodinfo, Type[] optionalParamTypes)
		{
			throw new NotImplementedException("EmitCall");
		}

		[TODO]
		public void EmitCalli(OpCode opcode, CallingConventions call_conv, Type returnType, Type[] paramTypes, Type[] optionalParamTypes)
		{
			throw new NotImplementedException("EmitCalli");
		}

		[TODO]
		public virtual void EmitWriteLine(FieldInfo field)
		{
			throw new NotImplementedException("EmitWriteLine");
		}

		[TODO]
		public virtual void EmitWriteLine(LocalBuilder lbuilder)
		{
			throw new NotImplementedException("EmitWriteLine");
		}

		[TODO]
		public virtual void EmitWriteLine(String val)
		{
			throw new NotImplementedException("EmitWriteLine");
		}

		[TODO]
		public virtual void EndExceptionBlock()
		{
			throw new NotImplementedException("EndExceptionBlock");
		}

		[TODO]
		public virtual void EndScope()
		{
			throw new NotImplementedException("EndScope");
		}

		[TODO]
		public virtual void MarkLabel(Label loc)
		{
			throw new NotImplementedException("MarkLabel");
		}

		[TODO]
		public virtual void MarkSequencePoint(ISymbolDocumentWriter document, 
				int startLine, int startColumn, int endLine, int endColumn)
		{
			throw new NotImplementedException("MarkSequencePoint");
		}

		[TODO]
		public virtual void ThrowException(Type exceptionType)
		{
			throw new NotImplementedException("ThrowException");
		}

		[TODO]
		public void UsingNamespace(String usingNamespace)
		{
			throw new NotImplementedException("UsingNamespace");
		}

	}
}//namespace

#endif
