/*
 * XmlDTDParserInput.cs - Implementation of the
 *			"System.Xml.XmlDTDParserInput" class.
 *
 * Copyright (C) 2004  Free Software Foundation, Inc.
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

namespace System.Xml
{

using System;
using System.Text;
using System.Collections.Specialized;

internal class XmlDTDParserInput : XmlParserInputBase
{
	// Internals.
#if !ECMA_COMPAT
	private bool valid;
	private bool started;
#endif
	private bool scanForPE;
	private int pePosition;
	private String peValue;
	private NameValueCollection parameterEntities;
	private XmlParserInput input;


	// Constructor.
	public XmlDTDParserInput
				(XmlParserInput input,
				 XmlNameTable nameTable)
			: base(nameTable, null, input.ErrorHandler)
			{
				this.input = input;
				this.peValue = null;
				this.pePosition = -1;
				this.scanForPE = true;
				this.parameterEntities = new NameValueCollection();
			#if !ECMA_COMPAT
				this.valid = true;
				this.started = false;
			#endif
				base.logger = null;
			}


	// Get the current line number.
	public override int LineNumber
			{
				get { return input.LineNumber; }
			}

	// Get the current line position.
	public override int LinePosition
			{
				get { return input.LinePosition; }
			}

	// Get the logger.
	public override LogManager Logger
			{
				get { return input.Logger; }
			}

	// Get or set the list of parameter entities.
	public NameValueCollection ParameterEntities
			{
				get { return parameterEntities; }
				set { parameterEntities = value; }
			}

	// Turn on/off pe scanning.
	public bool ScanForPE
			{
				get { return scanForPE; }
				set { scanForPE = value; }
			}

#if !ECMA_COMPAT
	// Valid DTDs require that all tags started in a pe, end in that pe.

	// Get the valid flag, which is set iff no invalid pe references were found.
	public bool Valid
			{
				get { return valid; }
			}


	//
	public void StartTag()
			{
				started = (peValue != null);
			}

	// 
	public void EndTag()
			{
				if(peValue != null)
				{
					if(!started)
					{
						valid = false;
					}
				}
				else if(started)
				{
					valid = false;
				}
				started = false;
			}
#endif

	// Move to the next character, returning false on EOF.
	public override bool NextChar()
			{
				bool retval;
				if(peValue == null)
				{
					retval = input.NextChar();
					currChar = input.currChar;
					if(!scanForPE) { return retval; }
					if(currChar == '%')
					{
						if(input.PeekChar() &&
						   XmlCharInfo.IsNameInit(input.peekChar))
						{
							String name = input.ReadName();
							input.Expect(';');
							peValue = parameterEntities[name];
							pePosition = 0;
							currChar = ' ';
						}
					}
				}
				else
				{
					retval = true;
					if(pePosition == -1)
					{
						currChar = ' ';
					}
					else if(pePosition < peValue.Length)
					{
						currChar = peValue[pePosition++];
					}
					else
					{
						pePosition = -1;
						peValue = null;
						currChar = ' ';
					}
				}
				return retval;
			}

	// Peek at the next character, returning false on EOF.
	public override bool PeekChar()
			{
				bool retval;
				if(peValue == null)
				{
					retval = input.PeekChar();
					peekChar = input.peekChar;
					if(!scanForPE) { return retval; }
					if(peekChar == '%')
					{
						if(input.ExtraPeekChar() &&
						   XmlCharInfo.IsNameInit(input.extraPeekChar))
						{
							input.NextChar();
							String name = input.ReadName();
							input.Expect(';');
							peValue = parameterEntities[name];
							pePosition = 0;
							peekChar = ' ';
						}
					}
				}
				else
				{
					retval = true;
					if(pePosition+1 < peValue.Length)
					{
						peekChar = peValue[pePosition];
					}
					else
					{
						peekChar = ' ';
					}
				}
				return retval;
			}

	// Exit the current pe, if any.
	public void ResetPE()
			{
				pePosition = -1;
				peValue = null;
			}

}; // class XmlDTDParserInput

}; // namespace System.Xml
