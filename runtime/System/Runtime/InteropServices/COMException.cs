/*
 * COMException.cs - Implementation of the
 *			"System.Runtime.InteropServices.COMException" class 
 *
 * Copyright (C) 2003  Southern Storm Software, Pty Ltd.
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

namespace System.Runtime.InteropServices
{

#if !ECMA_COMPAT

using System.Runtime.Serialization;

[Serializable]
public class COMException : ExternalException
{

	private int errorCode;

	// Constructors.
	public COMException()
			: base(_("Exception_COM")) {}
	public COMException(String msg)
			: base(msg) {}
	public COMException(String msg, Exception inner)
			: base(msg, inner) {}
	public COMException(String msg, int errorCode)
			: base(msg, errorCode) {}
	protected COMException(SerializationInfo info, StreamingContext context)
			: base(info, context) {}

	// Convert this object into a string.
	public override String ToString()
			{
				return base.ToString();
			}

	// Get the default message to use for this exception type.
	protected internal override String MessageDefault
			{
				get
				{
					return _("Exception_COM");
				}
			}

}; // class COMException

#endif // !ECMA_COMPAT

}; // namespace System.Runtime.InteropServices
