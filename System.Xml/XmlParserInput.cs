/*
 * XmlParserInput.cs - Implementation of the
 *			"System.Xml.XmlParserInput" class.
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
using System.IO;
using System.Text;

internal class XmlParserInput : XmlParserInputBase
{
	// Internal state.
	private int lineNumber;
	private int linePosition;
	private int bufferPos;
	private int bufferLen;
	private char[] buffer;
	private TextReader reader;

	private const int BUFSIZE = 1024;


	// Field.
	internal char extraPeekChar; // don't touch.. for XmlDTDParserInput use only


	// Constructors.
	public XmlParserInput
				(TextReader reader,
				 XmlNameTable nameTable)
			: this(reader, nameTable, null, null)
			{
			}
	public XmlParserInput
				(TextReader reader,
				 XmlNameTable nameTable,
				 EOFHandler eof)
			: this(reader, nameTable, eof, null)
			{
			}
	public XmlParserInput
				(TextReader reader,
				 XmlNameTable nameTable,
				 ErrorHandler error)
			: this(reader, nameTable, null, error)
			{
			}
	public XmlParserInput
				(TextReader reader,
				 XmlNameTable nameTable,
				 EOFHandler eof,
				 ErrorHandler error)
			: base(nameTable, eof, error)
			{
				this.lineNumber = 1;
				this.linePosition = 1;
				this.bufferPos = 0;
				this.bufferLen = 0;
				this.buffer = new char[BUFSIZE];
				this.reader = reader;
				this.extraPeekChar = '\0';
			}


	// Get the current line number.
	public override int LineNumber
			{
				get { return lineNumber; }
			}

	// Get the current line position.
	public override int LinePosition
			{
				get { return linePosition; }
			}

	// Get or set the reader.
	public TextReader Reader
			{
				get { return reader; }
				set { reader = value; }
			}


	// Close the reader.
	public void Close()
			{
				if(reader != null)
				{
					reader.Close();
					reader = null;
				}
			}

	// Move to the next character, returning false on EOF.
	public override bool NextChar()
			{
				if(bufferPos == bufferLen)
				{
					bufferLen = reader.Read(buffer, 0, BUFSIZE);
					bufferPos = 0;
					if(bufferLen == 0)
					{
						EOF();
						return false;
					}
				}
				currChar = buffer[bufferPos++];
				if(currChar == '\n')
				{
					++lineNumber;
					linePosition = 1;
				}
				else
				{
					++linePosition;
				}
				logger.Append(currChar);
				return true;
			}

	// Peek at the next character, returning false on EOF.
	public override bool PeekChar()
			{
				if(bufferPos == bufferLen)
				{
					bufferLen = reader.Read(buffer, 0, BUFSIZE);
					bufferPos = 0;
					if(bufferLen == 0)
					{
						EOF();
						return false;
					}
				}
				peekChar = buffer[bufferPos];
				return true;
			}

	// This is a hack for the double look-ahead in the subset reader.
	internal bool ExtraPeekChar()
			{
				if(bufferPos+1 >= bufferLen)
				{
					if(!PeekChar()) { return false; }
					if(bufferPos+1 == bufferLen)
					{
						buffer[0] = peekChar;
						bufferLen = reader.Read(buffer, 1, BUFSIZE-1) + 1;
						bufferPos = 0;
						if(bufferLen == 1)
						{
							EOF();
							return false;
						}
					}
				}
				extraPeekChar = buffer[bufferPos+1];
				return true;
			}

}; // class XmlParserInput

}; // namespace System.Xml
