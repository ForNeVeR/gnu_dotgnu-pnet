/*
 * ResourceReader.cs - Implementation of the
 *		"System.Resources.ResourceReader" class.
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

namespace System.Resources
{

using System;
using System.Collections;
using System.IO;
using System.Text;

public sealed class ResourceReader : IEnumerable, IDisposable, IResourceReader
{
	// Internal state.
	private Stream stream;
	private int numStrings;
	private int[] nameHash;
	private int[] namePosn;
	private long nameStart;
	private long dataStart;

	// Constructors.
	public ResourceReader(Stream stream)
			{
				if(stream == null)
				{
					throw new ArgumentNullException("stream");
				}
				else if(!stream.CanRead)
				{
					throw new ArgumentException
						(_("IO_NotSupp_Read"), "stream");
				}
				else if(!stream.CanSeek)
				{
					throw new ArgumentException
						(_("IO_NotSupp_Seek"), "stream");
				}
				this.stream = stream;
				if(!ReadHeader())
				{
					numStrings = 0;
				}
			}
	public ResourceReader(String fileName)
			: this(new FileStream(fileName, FileMode.Open, FileAccess.Read))
			{
				// Nothing to do here.
			}

	// Implement the IEnumerable interface.
	IEnumerator IEnumerable.GetEnumerator()
			{
				if(stream == null)
				{
					throw new InvalidOperationException
						(_("IO_StreamClosed"));
				}
				return new ResourceEnumerator(this);
			}

	// Implement the IDisposable interface.
	void IDisposable.Dispose()
			{
				Close();
			}

	// Implement the IResourceReader interface.
	public void Close()
			{
				if(stream != null)
				{
					stream.Close();
					stream = null;
				}
			}
	public IDictionaryEnumerator GetEnumerator()
			{
				if(stream == null)
				{
					throw new InvalidOperationException
						(_("IO_StreamClosed"));
				}
				return new ResourceEnumerator(this);
			}

	// Read an unsigned integer value from a buffer.
	private static uint ReadUInt(byte[] buf, int offset)
			{
				return ((uint)(buf[offset])) |
				       (((uint)(buf[offset + 1])) << 8) |
				       (((uint)(buf[offset + 2])) << 16) |
				       (((uint)(buf[offset + 3])) << 24);
			}

	// Read a integer value byte by byte.  Returns -1 if invalid.
	private static int ReadInt(Stream stream)
			{
				int value, byteval;
				byteval = stream.ReadByte();
				if(byteval == -1)
				{
					return -1;
				}
				value = byteval;
				byteval = stream.ReadByte();
				if(byteval == -1)
				{
					return -1;
				}
				value |= (byteval << 8);
				byteval = stream.ReadByte();
				if(byteval == -1)
				{
					return -1;
				}
				value |= (byteval << 16);
				byteval = stream.ReadByte();
				if(byteval == -1)
				{
					return -1;
				}
				value |= (byteval << 24);
				if(value < 0)
				{
					return -1;
				}
				return value;
			}

	// Read a compact length value from a stream.  Returns -1 if invalid.
	private static int ReadLength(Stream stream)
			{
				int byteval = stream.ReadByte();
				int value;
				if(byteval == -1)
				{
					return -1;
				}
				value = (byteval & 0x7F);
				int shift = 7;
				while((byteval & 0x80) != 0)
				{
					byteval = stream.ReadByte();
					if(byteval == -1)
					{
						return -1;
					}
					value |= ((byteval & 0x7F) << shift);
					shift += 7;
				}
				if(value < 0)
				{
					return -1;
				}
				else
				{
					return value;
				}
			}

	// Read a string value from a stream.  Returns null if invalid.
	private static String ReadString(Stream stream)
			{
				int length = ReadLength(stream);
				if(length < 0)
				{
					return null;
				}
				else if(length == 0)
				{
					return String.Empty;
				}
				byte[] buf = new byte [length];
				if(stream.Read(buf, 0, length) != length)
				{
					return null;
				}
				return Encoding.UTF8.GetString(buf, 0, length);
			}

	// Read the resource stream header.  Returns false if
	// the header was invalid in some way.
	private bool ReadHeader()
			{
				byte[] header = new byte [12];
				uint numTypes;
				long start = stream.Position;
				int posn;

				// Read the primary part of the header and validate it.
				if(stream.Read(header, 0, 12) != 12 ||
				   ReadUInt(header, 0) !=
				   		(uint)(ResourceManager.MagicNumber) ||
				   ReadUInt(header, 4) !=
				   		(uint)(ResourceManager.HeaderVersionNumber))
				{
					return false;
				}

				// Skip past the class names.
				stream.Seek((long)(ReadUInt(header, 8)), SeekOrigin.Current);

				// Read the secondary part of the header.
				if(stream.Read(header, 0, 12) != 12 ||
				   ReadUInt(header, 0) != (uint)1)
				{
					return false;
				}
				numStrings = (int)(ReadUInt(header, 4));
				if(numStrings < 0)
				{
					return false;
				}
				numTypes = ReadUInt(header, 8);

				// Skip the type table.  TODO: handle non-string types.
				while(numTypes > 0)
				{
					ReadString(stream);
				}

				// Read the name hash table into memory.
				nameHash = new int [numStrings];
				for(posn = 0; posn < numStrings; ++posn)
				{
					nameHash[posn] = ReadInt(stream);
				}

				// Read the name position table into memory.
				namePosn = new int [numStrings];
				for(posn = 0; posn < numStrings; ++posn)
				{
					namePosn[posn] = ReadInt(stream);
				}

				// Read the offset of the data section.
				if(stream.Read(header, 0, 4) != 4)
				{
					return false;
				}
				dataStart = start + (long)(ReadUInt(header, 0));

				// We have found the start of the name section.
				nameStart = stream.Position;

				// Ready to go.
				return true;
			}

	// Hash a resource name to a resource hash value.
	internal static int Hash(String name)
			{
				int len = name.Length;
				int posn;
				int hash = 0x1505;
				for(posn = 0; posn < len; ++posn)
				{
					hash = ((hash << 5) + hash) ^ (int)(name[posn]);
				}
				return hash;
			}

	// Look up a resource object by name.  This is a short-cut
	// that "ResourceSet" can use to perform quicker lookups in
	// the common case of internal assembly string resources.
	internal Object GetObject(String name)
			{
				int hash = Hash(name);
				int left, right, middle;
				String test;

				// Search for the hash value using a binary search.
				left = 0;
				right = numStrings - 1;
				while(left <= right)
				{
					middle = left + ((right - left) / 2);
					if(hash < nameHash[middle])
					{
						right = middle - 1;
					}
					else if(hash > nameHash[middle])
					{
						left = middle + 1;
					}
					else
					{
						left = middle;
						break;
					}
				}
				if(left > right)
				{
					return null;
				}

				// Find the left-most and right-most extent of strings
				// that hash to the same value.
				right = left;
				while(left > 0 && nameHash[left - 1] == hash)
				{
					--left;
				}
				while(right < (numStrings - 1) && nameHash[right + 1] == hash)
				{
					++right;
				}

				// Scan all strings with the same hash for a name match.
				while(left <= right)
				{
					stream.Seek(nameStart + namePosn[left], SeekOrigin.Begin);
					test = ReadString(stream);
					if(test != null && test.Equals(name))
					{
						// We have found a name match: fetch the value.
						int valuePosn = ReadInt(stream);
						if(valuePosn == -1)
						{
							return null;
						}
						stream.Seek(dataStart + valuePosn, SeekOrigin.Begin);
						ReadLength(stream);	// Skip type value (TODO)
						return ReadString(stream);
					}
					++left;
				}

				// There are no strings with an equal name.
				return null;
			}

	// Private enumerator class for resource readers.
	private sealed class ResourceEnumerator
				: IEnumerator, IDictionaryEnumerator
	{
		// Internal state.
		private ResourceReader reader;
		private int posn;
		private String key;
		private String value;

		// Constructor.
		public ResourceEnumerator(ResourceReader reader)
				{
					this.reader = reader;
					this.posn = -1;
				}

		// Implement the IEnumerator interface.
		public bool MoveNext()
				{
					Stream stream = reader.stream;
					if(stream == null)
					{
						throw new InvalidOperationException
							(_("IO_StreamClosed"));
					}
					++posn;
					if(posn < reader.numStrings)
					{
						stream.Seek(reader.nameStart + reader.namePosn[posn],
									SeekOrigin.Begin);
						key = ReadString(stream);
						if(key == null)
						{
							throw new InvalidOperationException
								(_("IO_ReadFailed"));
						}
						int valuePosn = ReadInt(stream);
						if(valuePosn == -1)
						{
							throw new InvalidOperationException
								(_("IO_ReadFailed"));
						}
						stream.Seek(reader.dataStart + (long)valuePosn,
									SeekOrigin.Begin);
						ReadLength(stream);	// Skip type value (TODO)
						value = ReadString(stream);
						if(value == null)
						{
							throw new InvalidOperationException
								(_("IO_ReadFailed"));
						}
						return true;
					}
					else
					{
						--posn;
						return false;
					}
				}
		public void Reset()
				{
					posn = -1;
				}
		public Object Current
				{
					get
					{
						return Entry;
					}
				}

		// Implement the IDictionaryEnumerator interface.
		public DictionaryEntry Entry
				{
					get
					{
						if(posn >= 0 && posn < reader.numStrings)
						{
							return new DictionaryEntry(key, value);
						}
						else
						{
							throw new InvalidOperationException
								(_("Invalid_BadEnumeratorPosition"));
						}
					}
				}
		public Object Key
				{
					get
					{
						if(posn >= 0 && posn < reader.numStrings)
						{
							return key;
						}
						else
						{
							throw new InvalidOperationException
								(_("Invalid_BadEnumeratorPosition"));
						}
					}
				}
		public Object Value
				{
					get
					{
						if(posn >= 0 && posn < reader.numStrings)
						{
							return value;
						}
						else
						{
							throw new InvalidOperationException
								(_("Invalid_BadEnumeratorPosition"));
						}
					}
				}

	}; // class ResourceEnumerator

}; // class ResourceReader

}; // namespace System.Resources
