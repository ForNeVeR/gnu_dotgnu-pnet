/*
 * XmlStreamReader.cs - Implementation of the "System.Xml.XmlStreamReader" class.
 *
 * Copyright (C) 2001, 2002  Southern Storm Software, Pty Ltd.
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
using System.Globalization;
using System.Text;
using System.IO;

internal class XmlStreamReader : TextReader
{
#if !ECMA_COMPAT
	// The null stream reader.
	public new static readonly XmlStreamReader Null
			= new XmlStreamReader(Stream.Null);
#endif

	// Default and minimum buffer sizes to use for streams.
	private const int STREAM_BUFSIZ = 1024;
	private const int FILE_BUFSIZ   = 4096;
	private const int MIN_BUFSIZ    = 128;

	// Internal state.
	private Stream 		stream;
	private Encoding	encoding;
	private Decoder		decoder;
	private int    		bufferSize;
	private byte[] 		inBuffer;
	private int    		inBufferPosn;
	private int    		inBufferLen;
	private char[] 		outBuffer;
	private int    		outBufferPosn;
	private int    		outBufferLen;
	private bool   		sawEOF;
	internal TextReader TxtReader;
	internal bool canseek;

	// Constructors that are based on a stream.
	public XmlStreamReader(Stream stream)
			: this(stream, Encoding.UTF8, true, STREAM_BUFSIZ) {}

	internal XmlStreamReader(TextReader txtReader)
			{
				this.TxtReader = txtReader;
				StreamReader strReader = txtReader as StreamReader;
				if(strReader != null)
				{
					canseek = strReader.BaseStream.CanSeek;
				}
				else
				{
					canseek = txtReader != null && txtReader.Peek() != -1;
				}
				
			}
	public XmlStreamReader(Stream stream, bool detectEncodingFromByteOrderMarks)
			: this(stream, Encoding.UTF8,
				   detectEncodingFromByteOrderMarks, STREAM_BUFSIZ) {}
	public XmlStreamReader(Stream stream, Encoding encoding)
			: this(stream, encoding, true, STREAM_BUFSIZ) {}
	public XmlStreamReader(Stream stream, Encoding encoding,
						bool detectEncodingFromByteOrderMarks)
			: this(stream, encoding,
				   detectEncodingFromByteOrderMarks, STREAM_BUFSIZ) {}
	public XmlStreamReader(Stream stream, Encoding encoding,
						bool detectEncodingFromByteOrderMarks,
						int bufferSize)
			{
				// Validate the parameters.
				if(stream == null)
				{
					throw new ArgumentNullException("stream");
				}
				if(encoding == null)
				{
					throw new ArgumentNullException("encoding");
				}
				if(!stream.CanRead)
				{
					throw new ArgumentException(S._("IO_NotSupp_Read"));
				}
				if(bufferSize <= 0)
				{
					throw new ArgumentOutOfRangeException
						("bufferSize",S._("ArgRange_BufferSize"));
				}
				if(bufferSize < MIN_BUFSIZ)
				{
					bufferSize = MIN_BUFSIZ;
				}

				// Initialize this object.
				this.stream = stream;
				this.encoding = encoding;
				this.bufferSize = bufferSize;
				this.inBuffer = new byte [bufferSize];
				this.inBufferPosn = 0;
				this.inBufferLen = 0;
				this.outBuffer = new char [bufferSize];
				this.outBufferPosn = 0;
				this.outBufferLen = 0;
				this.sawEOF = false;

				// Should we change encodings based on a byte order mark?
				if(detectEncodingFromByteOrderMarks)
				{
					DetectByteOrder();
				}

				// Get a decoder for the encoding.
				decoder = encoding.GetDecoder();
			}

	// Constructors that are based on a filename.
	public XmlStreamReader(String path)
			: this(path, Encoding.UTF8, true, STREAM_BUFSIZ) {}
	public XmlStreamReader(String path, bool detectEncodingFromByteOrderMarks)
			: this(path, Encoding.UTF8,
				   detectEncodingFromByteOrderMarks, STREAM_BUFSIZ) {}
	public XmlStreamReader(String path, Encoding encoding)
			: this(path, encoding, true, STREAM_BUFSIZ) {}
	public XmlStreamReader(String path, Encoding encoding,
						bool detectEncodingFromByteOrderMarks)
			: this(path, encoding,
				   detectEncodingFromByteOrderMarks, STREAM_BUFSIZ) {}
	public XmlStreamReader(String path, Encoding encoding,
						bool detectEncodingFromByteOrderMarks,
						int bufferSize)
			{
				// Validate the parameters.
				if(path == null)
				{
					throw new ArgumentNullException("path");
				}
				if(encoding == null)
				{
					throw new ArgumentNullException("encoding");
				}
				if(bufferSize <= 0)
				{
					throw new ArgumentOutOfRangeException
						("bufferSize", S._("ArgRange_BufferSize"));
				}
				if(bufferSize < MIN_BUFSIZ)
				{
					bufferSize = MIN_BUFSIZ;
				}

				// Attempt to open the file.
				Stream stream = new FileStream(path, FileMode.Open,
											   FileAccess.Read,
											   FileShare.Read,
											   FILE_BUFSIZ);

				// Initialize this object.
				this.stream = stream;
				this.encoding = encoding;
				this.bufferSize = bufferSize;
				this.inBuffer = new byte [bufferSize];
				this.inBufferPosn = 0;
				this.inBufferLen = 0;
				this.outBuffer = new char [bufferSize];
				this.outBufferPosn = 0;
				this.outBufferLen = 0;
				this.sawEOF = false;

				// Should we change encodings based on a byte order mark?
				if(detectEncodingFromByteOrderMarks)
				{
					DetectByteOrder();
				}

				// Get a decoder for the encoding.
				decoder = encoding.GetDecoder();
			}

	// Destructor.
	~XmlStreamReader()
			{
				Dispose(false);
			}

	// Detect the byte order by inspecting the first few bytes.
	private void DetectByteOrder()
			{
				// Pre-read the first full buffer of input data.
				inBufferLen = stream.Read(inBuffer, 0, bufferSize);
				if(inBufferLen <= 0)
				{
					sawEOF = true;
					inBufferLen = 0;
				}

				// Check for recognized byte order marks.
				if(inBufferLen >= 2 &&
				   inBuffer[0] == 0xFF &&
				   inBuffer[1] == 0xFE)
				{
					// Little-endian UTF-16.
					encoding = Encoding.Unicode;
					inBufferPosn = 2;
				}
				else if(inBufferLen >= 2 &&
				        inBuffer[0] == 0xFE &&
				        inBuffer[1] == 0xFF)
				{
					// Big-endian UTF-16.
					encoding = Encoding.BigEndianUnicode;
					inBufferPosn = 2;
				}
				else if(inBufferLen >= 3 &&
						inBuffer[0] == 0xEF &&
						inBuffer[1] == 0xBB &&
						inBuffer[2] == 0xBF)
				{
					// UTF-8.
					encoding = Encoding.UTF8;
					inBufferPosn = 3;
				}
			}

	// Close this stream reader.
	public override void Close()
			{
				Dispose(true);
			}

	// Discard any data that was buffered by this stream reader.
	public void DiscardBufferedData()
			{
				// Empty the buffers.
				inBufferPosn = 0;
				inBufferLen = 0;
				outBufferPosn = 0;
				outBufferLen = 0;
				sawEOF = false;

				// Create a new decoder.  We cannot reuse the
				// old one because we have no way to reset the
				// state to the default.
				decoder = encoding.GetDecoder();
			}

	// Dispose this stream reader.
	protected override void Dispose(bool disposing)
			{
				if(stream != null)
				{
					stream.Close();
					stream = null;
				}
				inBuffer = null;
				inBufferPosn = 0;
				inBufferLen = 0;
				outBuffer = null;
				outBufferPosn = 0;
				outBufferLen = 0;
				bufferSize = 0;
				sawEOF = true;
				base.Dispose(disposing);
			}

	// Read byte data from the stream and convert it into characters.
	private void ReadChars()
			{
				int len, outLen;

				while(outBufferPosn >= outBufferLen && !sawEOF)
				{
					// Move the previous left-over buffer contents down.
					if((inBufferLen - inBufferPosn) < bufferSize)
					{
						if(inBufferPosn < inBufferLen)
						{
							Array.Copy
								(inBuffer, inBufferPosn,
							     inBuffer, 0, inBufferLen - inBufferPosn);
							inBufferLen -= inBufferPosn;
						}
						else
						{
							inBufferLen = 0;
						}
						inBufferPosn = 0;

						// Read new bytes into the buffer.
						if(stream == null)
						{
							throw new IOException(S._("IO_StreamClosed"));
						}
						len = stream.Read(inBuffer, inBufferPosn,
										  bufferSize - inBufferPosn);
						if(len <= 0)
						{
							sawEOF = true;
						}
						else
						{
							inBufferLen += len;
						}
					}

					// Determine the maximum number of bytes that
					// we can afford to convert into characters.
					len = encoding.GetMaxByteCount(bufferSize);
					if(len > inBufferLen)
					{
						len = inBufferLen;
					}

					// Convert the bytes into characters.
					outLen = decoder.GetChars(inBuffer, inBufferPosn, len,
											  outBuffer, 0);
					outBufferPosn = 0;
					outBufferLen = outLen;
					inBufferPosn += len;
				}
			}

	// Peek at the next character from this stream reader.
	public override int Peek()
			{
				if(outBufferPosn < outBufferLen)
				{
					// We already have a character available.
					return (int)(outBuffer[outBufferPosn]);
				}
				else
				{
					// Read another buffer of characters.
					ReadChars();
					if(outBufferPosn < outBufferLen)
					{
						return (int)(outBuffer[outBufferPosn]);
					}
					else
					{
						return -1;
					}
				}
			}

	// Read a single character from this stream reader.
	public override int Read()
			{
				if(outBufferPosn < outBufferLen)
				{
					// We already have a character available.
					return (int)(outBuffer[outBufferPosn++]);
				}
				else
				{
					// Read another buffer of characters.
					ReadChars();
					if(outBufferPosn < outBufferLen)
					{
						return (int)(outBuffer[outBufferPosn++]);
					}
					else
					{
						return -1;
					}
				}
			}

	// Read a buffer of characters from this stream reader.
	public override int Read(char[] buffer, int index, int count)
			{
				// Validate the parameters.
				if(buffer == null)
				{
					throw new ArgumentNullException("buffer");
				}
				if(index < 0)
				{
					throw new ArgumentOutOfRangeException
						("index", S._("ArgRange_Array"));
				}
				if(count < 0)
				{
					throw new ArgumentOutOfRangeException
						("count", S._("ArgRange_Array"));
				}
				if((buffer.Length - index) < count)
				{
					throw new ArgumentException
						(S._("Arg_InvalidArrayRange"));
				}

				// Read data from the input stream into the buffer.
				int len = 0;
				int templen;
				while(count > 0)
				{
					// Re-fill the character buffer if necessary.
					if(outBufferPosn >= outBufferLen)
					{
						ReadChars();
						if(outBufferPosn >= outBufferLen)
						{
							break;
						}
					}

					// Copy data to the result buffer.
					templen = outBufferLen - outBufferPosn;
					if(templen > count)
					{
						templen = count;
					}
					Array.Copy(outBuffer, outBufferPosn,
							   buffer, index, templen);
					outBufferPosn += templen;
					index += templen;
					count -= templen;
					len += templen;
				}
				return len;
			}

	// Read a line of characters from this stream reader.
	public override String ReadLine()
			{
				StringBuilder builder = new StringBuilder();
				int ch;
				for(;;)
				{
					// Re-fill the character buffer if necessary.
					if(outBufferPosn >= outBufferLen)
					{
						ReadChars();
						if(outBufferPosn >= outBufferLen)
						{
							break;
						}
					}

					// Process characters until we reach a line terminator.
					while(outBufferPosn < outBufferLen)
					{
						ch = outBuffer[outBufferPosn++];
						if(ch == 13)
						{
							// Peek at the next character to determine
							// if this is a CRLF or CR line terminator.
							if(outBufferPosn >= outBufferLen)
							{
								ReadChars();
							}
							if(outBufferPosn < outBufferLen &&
							   outBuffer[outBufferPosn] == '\u000A')
							{
								++outBufferPosn;
							}
							return builder.ToString();
						}
						else if(ch == 10)
						{
							// This is an LF line terminator.
							return builder.ToString();
						}
						else
						{
							builder.Append((char)ch);
						}
					}
				}
				if(builder.Length != 0)
				{
					return builder.ToString();
				}
				else
				{
					return null;
				}
			}

	// Read the entire contents of this stream reader until EOF.
	public override String ReadToEnd()
			{
				StringBuilder builder = new StringBuilder();
				for(;;)
				{
					// Re-fill the character buffer if necessary.
					if(outBufferPosn >= outBufferLen)
					{
						ReadChars();
						if(outBufferPosn >= outBufferLen)
						{
							break;
						}
					}

					// Append the character buffer to the builder.
					builder.Append(outBuffer, outBufferPosn,
								   outBufferLen - outBufferPosn);
					outBufferPosn = outBufferLen;
				}
				if(builder.Length != 0)
				{
					return builder.ToString();
				}
				else
				{
					return String.Empty;
				}
			}

	// Get the base stream underlying this stream reader.
	public virtual Stream BaseStream
			{
				get
				{
					return stream;
				}
			}

	// Get the current encoding in use by this stream reader.
	public virtual Encoding CurrentEncoding
			{
				get
				{
					return encoding;
				}
			}

}; // class XmlStreamReader

}; // namespace System.Xml
