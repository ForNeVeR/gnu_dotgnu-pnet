/*
 * ILGenerator.cs - Implementation of "System.Reflection.Emit.ILGenerator" 
 *
 * Copyright (C) 2002  Southern Storm Software, Pty Ltd.
 * 
 * Contributions from Gopal.V <gopalv82@symonds.net> 
 *                    Rhys Weatherley <rweather@southern-storm.com.au>
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
	// Internal state.
	private ModuleBuilder module;
	private byte[] code;
	private int offset;
	private int height;
	private int maxHeight;
	private LabelInfo[] labels;
	private int numLabels;

	// Information about a label in the current method.
	private struct LabelInfo
	{
		public int       offset;
		public int       height;
		public LabelRef  refs;

	}; // struct LabelInfo

	// Reference information for back-patching a label.
	private class LabelRef
	{
		public LabelRef	next;
		public int		address;
		public int		switchEnd;

	}; // class LabelRef

	// Constructor.
	internal ILGenerator(ModuleBuilder module)
			{
				this.module = module;
				code = new byte [32];
				offset = 0;
				height = 0;
				maxHeight = 0;
				labels = null;
				numLabels = 0;
			}

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

	// Enter a lexical naming scope for debug information.
	public virtual void BeginScope()
			{
				// Scopes are not currently used in this implementation.
			}

	// Declare a local variable within the current method.
	public LocalBuilder DeclareLocal(Type localType)
			{
				throw new NotImplementedException("DeclareLocal");
			}

	// Declare a label within the current method.
	public virtual Label DefineLabel()
			{
				if(labels == null)
				{
					labels = new LabelInfo [8];
				}
				else if(numLabels >= labels.Length)
				{
					LabelInfo[] newLabels = new LabelInfo [numLabels * 2];
					Array.Copy(labels, 0, newLabels, 0, numLabels);
					newLabels = labels;
				}
				return new Label(numLabels++);
			}

	// Emit a single byte to the current method's code.
	private void EmitByte(int value)
			{
				if(offset >= code.Length)
				{
					byte[] newCode = new byte [code.Length * 2];
					Array.Copy(code, 0, newCode, 0, code.Length);
					code = newCode;
				}
				code[offset++] = (byte)value;
			}

	// Emit a token value to the current method's code.
	private void EmitToken(int token)
			{
				EmitByte(token);
				EmitByte(token >> 8);
				EmitByte(token >> 16);
				EmitByte(token >> 24);
			}

	// Emit a token value to the current method's code and register it
	// to have a token fixup at the end of the assembly output process.
	private void EmitTokenWithFixup(int token)
			{
				// TODO: fixups
				EmitByte(token);
				EmitByte(token >> 8);
				EmitByte(token >> 16);
				EmitByte(token >> 24);
			}

	// Emit an opcode value to the current method's code and then
	// adjust the stack height information accordingly.  We use a
	// "ref" parameter to avoid unnecessary data copies in the
	// methods that call this one.
	private void EmitOpcode(ref OpCode opcode)
			{
				// Output the opcode to the instruction stream.
				int value = opcode.value;
				if(value < 0x0100)
				{
					EmitByte(value);
				}
				else
				{
					EmitByte(value);
					EmitByte(value & 0xFF);
				}

				// Adjust the stack requirements.
				switch((StackBehaviour)(opcode.stackPop))
				{
					case StackBehaviour.Pop0:
					case StackBehaviour.Varpop:
						break;

					case StackBehaviour.Pop1:
					case StackBehaviour.Popi:
					case StackBehaviour.Popref:
						--height;
						break;

					case StackBehaviour.Pop1_pop1:
					case StackBehaviour.Popi_pop1:
					case StackBehaviour.Popi_popi:
					case StackBehaviour.Popi_popi8:
					case StackBehaviour.Popi_popr4:
					case StackBehaviour.Popi_popr8:
					case StackBehaviour.Popref_pop1:
					case StackBehaviour.Popref_popi:
						height -= 2;
						break;

					case StackBehaviour.Popi_popi_popi:
					case StackBehaviour.Popref_popi_popi:
					case StackBehaviour.Popref_popi_popi8:
					case StackBehaviour.Popref_popi_popr4:
					case StackBehaviour.Popref_popi_popr8:
					case StackBehaviour.Popref_popi_popref:
						height -= 3;
						break;

					default: break;
				}
				switch((StackBehaviour)(opcode.stackPush))
				{
					case StackBehaviour.Push0:
						break;

					case StackBehaviour.Push1:
					case StackBehaviour.Pushi:
					case StackBehaviour.Pushi8:
					case StackBehaviour.Pushr4:
					case StackBehaviour.Pushr8:
					case StackBehaviour.Pushref:
					case StackBehaviour.Varpush:
						++height;
						break;

					case StackBehaviour.Push1_push1:
						height += 2;
						break;

					default: break;
				}

				// Update the maximum stack height appropriately.
				if(height > maxHeight)
				{
					maxHeight = height;
				}
			}

	// Emit simple opcodes.
	public virtual void Emit(OpCode opcode)
			{
				EmitOpcode(ref opcode);
			}
	public virtual void Emit(OpCode opcode, byte val)
			{
				EmitOpcode(ref opcode);
				EmitByte(val);
			}
	public virtual void Emit(OpCode opcode, short val)
			{
				EmitOpcode(ref opcode);
				EmitByte(val);
				EmitByte(val >> 8);
			}
	public virtual void Emit(OpCode opcode, int val)
			{
				EmitOpcode(ref opcode);
				EmitByte(val);
				EmitByte(val >> 8);
				EmitByte(val >> 16);
				EmitByte(val >> 24);
			}
	public virtual void Emit(OpCode opcode, long val)
			{
				EmitOpcode(ref opcode);
				EmitByte((int)val);
				EmitByte((int)(val >> 8));
				EmitByte((int)(val >> 16));
				EmitByte((int)(val >> 24));
				EmitByte((int)(val >> 32));
				EmitByte((int)(val >> 40));
				EmitByte((int)(val >> 48));
				EmitByte((int)(val >> 56));
			}
	public void Emit(OpCode opcode, sbyte val)
			{
				EmitOpcode(ref opcode);
				EmitByte(val);
			}
	public virtual void Emit(OpCode opcode, float val)
			{
				byte[] bytes;
				EmitOpcode(ref opcode);
				bytes = BitConverter.GetLittleEndianBytes(val);
				EmitByte(bytes[0]);
				EmitByte(bytes[1]);
				EmitByte(bytes[2]);
				EmitByte(bytes[3]);
			}
	public virtual void Emit(OpCode opcode, double val)
			{
				byte[] bytes;
				EmitOpcode(ref opcode);
				bytes = BitConverter.GetLittleEndianBytes(val);
				EmitByte(bytes[0]);
				EmitByte(bytes[1]);
				EmitByte(bytes[2]);
				EmitByte(bytes[3]);
				EmitByte(bytes[4]);
				EmitByte(bytes[5]);
				EmitByte(bytes[6]);
				EmitByte(bytes[7]);
			}
	public virtual void Emit(OpCode opcode, String val)
			{
				StringToken token = module.GetStringConstant(val);
				EmitOpcode(ref opcode);
				EmitToken(token.Token);
			}


	[TODO]
	public virtual void Emit(OpCode opcode, ConstructorInfo constructor)
	{
		throw new NotImplementedException("Emit");
	}

	[TODO]
	public virtual void Emit(OpCode opcode, FieldInfo field)
	{
		throw new NotImplementedException("Emit");
	}

	// Emit code for a branch instruction.  Note: unlike other implementations,
	// we always output the branch in such a way that an out of range error
	// can never occur.  This makes it easier to process forward references
	// in a single pass through the code.
	public virtual void Emit(OpCode opcode, Label label)
			{
				int index = label.index;
				int shortForm, longForm;
				if(index < 0 || index >= numLabels)
				{
					return;
				}
				if((OperandType)(opcode.operandType) ==
						OperandType.ShortInlineBrTarget)
				{
					// Convert a short opcode into its long form.
					shortForm = opcode.value;
					if(shortForm >= 0x2B && shortForm <= 0x37)
					{
						longForm = shortForm - 0x2B + 0x38;
					}
					else
					{
						longForm = 0xDD;
					}
				}
				else if((OperandType)(opcode.operandType) ==
							OperandType.InlineBrTarget)
				{
					// Convert a long opcode into its short form.
					longForm = opcode.value;
					if(longForm >= 0x38 && longForm <= 0x44)
					{
						shortForm = longForm - 0x2B + 0x38;
					}
					else
					{
						shortForm = 0xDE;
					}
				}
				else
				{
					// Ignore non-branch opcodes.
					return;
				}
				if(labels[index].offset != 0)
				{
					// The label is already defined.  Determine if the
					// branch is long or short.
					int dest = (labels[index].offset - 1) - (offset + 2);
					if(dest >= -128 && dest <= 127)
					{
						EmitByte(shortForm);
						EmitByte(dest);
					}
					else
					{
						dest = (labels[index].offset - 1) - (offset + 5);
						EmitByte(longForm);
						EmitByte(dest);
						EmitByte(dest >> 8);
						EmitByte(dest >> 16);
						EmitByte(dest >> 24);
					}
				}
				else
				{
					// Output the long form and add a reference to the label.
					EmitByte(longForm);
					EmitByte(0);
					EmitByte(0);
					EmitByte(0);
					EmitByte(0);
					LabelRef newRef = new LabelRef();
					newRef.next = labels[index].refs;
					newRef.address = offset - 4;
					newRef.switchEnd = -1;
					labels[index].refs = newRef;
				}
				switch((StackBehaviour)(opcode.stackPop))
				{
					case StackBehaviour.Popi:
						--height;
						break;

					case StackBehaviour.Pop1_pop1:
						height -= 2;
						break;

					default: break;
				}
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
	public virtual void Emit(OpCode opcode, SignatureHelper shelper)
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
	public virtual void EndExceptionBlock()
	{
		throw new NotImplementedException("EndExceptionBlock");
	}

	// Exit a lexical naming scope for debug information.
	public virtual void EndScope()
			{
				// Scopes are not currently used in this implementation.
			}

	// Mark a label as existing at this point within the code.
	public virtual void MarkLabel(Label loc)
			{
				// Validate the label identifier.
				int index = loc.index;
				if(index < 0 || index >= numLabels)
				{
					throw new ArgumentException(_("Emit_InvalidLabel"));
				}
				else if(labels[index].offset != 0)
				{
					throw new ArgumentException(_("Emit_LabelAlreadyDefined"));
				}

				// Update the label information.
				if(labels[index].height > height)
				{
					height = labels[index].height;
				}
				labels[index].offset = offset + 1;

				// Perform fixups on all existing references to the label.
				LabelRef refs = labels[index].refs;
				labels[index].refs = null;
				int address, switchEnd, dest;
				while(refs != null)
				{
					address = refs.address;
					switchEnd = refs.switchEnd;
					if(switchEnd == -1)
					{
						// Back-patch an ordinary long jump.
						dest = offset - (address + 4);
					}
					else
					{
						// Back-patch an entry in a switch table.
						dest = offset - switchEnd;
					}
					code[address]     = (byte)dest;
					code[address + 1] = (byte)(dest >> 8);
					code[address + 2] = (byte)(dest >> 16);
					code[address + 3] = (byte)(dest >> 24);
					refs = refs.next;
				}
			}

	// Mark a sequence point within the debug information.
	public virtual void MarkSequencePoint
				(ISymbolDocumentWriter document, int startLine,
				 int startColumn, int endLine, int endColumn)
			{
				// Sequence points are not currently used.
			}

	// Short-cut helper methods.  Not yet implemented or used.
	public virtual void EmitWriteLine(FieldInfo field)
			{
				throw new NotImplementedException("EmitWriteLine");
			}
	public virtual void EmitWriteLine(LocalBuilder lbuilder)
			{
				throw new NotImplementedException("EmitWriteLine");
			}
	public virtual void EmitWriteLine(String val)
			{
				throw new NotImplementedException("EmitWriteLine");
			}
	public virtual void ThrowException(Type exceptionType)
			{
				throw new NotImplementedException("ThrowException");
			}
	public void UsingNamespace(String usingNamespace)
			{
				throw new NotImplementedException("UsingNamespace");
			}

}; // class ILGenerator.cs

}; // namespace System.Reflection.Emit

#endif
