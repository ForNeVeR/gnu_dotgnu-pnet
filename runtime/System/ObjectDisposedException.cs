/*
 * ObjectDisposedException.cs - Implementation of the
 *			"System.ObjectDisposedException" class.
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

namespace System
{

public class ObjectDisposedException : InvalidOperationException
{
	// Internal state.
	private String objectName;

	// Constructors.
	public ObjectDisposedException(String objectName)
		: base(_("Exception_Disposed"))
		{ this.objectName = objectName; }
	public ObjectDisposedException(String objectName, String msg)
		: base(msg) { this.objectName = objectName; }

	// Properties.
	public String ObjectName
			{
				get
				{
					return objectName;
				}
			}
	public override String Message
			{
				get
				{
					if(objectName != null)
					{
						return objectName + ": " + base.Message;
					}
					else
					{
						return base.Message;
					}
				}
			}

	// Get the default message to use for this exception type.
	protected internal override String MessageDefault
			{
				get
				{
					return _("Exception_Disposed");
				}
			}

}; // class ObjectDisposedException

}; // namespace System
