/*
 * SystemIcons.cs - Implementation of the "System.Drawing.SystemIcons" class.
 *
 * Copyright (C) 2003  Neil Cawse.
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
namespace System.Drawing
{
using System;

	public sealed class SystemIcons
	{
		private static Icon applicationIcon;
		private static Icon asteriskIcon;
		private static Icon errorIcon;
		private static Icon exclamationIcon;
		private static Icon handIcon;
		private static Icon informationIcon;
		private static Icon questionIcon;
		private static Icon warningIcon;
		private static Icon windowsLogo;

		[TODO]
		public static Icon ApplicationIcon
		{
			get
			{
				if (applicationIcon == null)
					//TODO
					applicationIcon = new Icon("");
				return applicationIcon;
			}
		}

		[TODO]
		public static Icon Asterisk
		{
			get
			{
				if (asteriskIcon == null)
				{
					//TODO
					asteriskIcon = new Icon("");
				}
				return asteriskIcon;
			}
		}

		[TODO]
		public static Icon Error
		{
			get
			{
				if (errorIcon == null)
				{
					//TODO
					errorIcon = new Icon("");
				}
				return errorIcon;
			}
		}

		[TODO]
		public static Icon Exclamation
		{
			get
			{
				if (exclamationIcon == null)
				{
					//TODO
					exclamationIcon = new Icon("");
				}
				return exclamationIcon;
			}
		}

		[TODO]
		public static Icon Hand
		{
			get
			{
				if (handIcon == null)
				{
					//TODO
					handIcon = new Icon("");
				}
				return handIcon;
			}
		}

		[TODO]
		public static Icon InformationIcon
		{
			get
			{
				if (informationIcon == null)
				{
					//TODO
					informationIcon = new Icon("");
				}
				return informationIcon;
			}
		}

		[TODO]
		public static Icon Question
		{
			get
			{
				if (questionIcon == null)
				{
					//TODO
					questionIcon = new Icon("");
				}
				return questionIcon;
			}
		}

		[TODO]
		public static Icon Warning
		{
			get
			{
				if (warningIcon == null)
				{
					//TODO
					warningIcon = new Icon("");
				}
				return warningIcon;
			}
		}

		[TODO]
		public static Icon WinLogo
		{
			get
			{
				if (windowsLogo == null)
				{
					//TODO
					windowsLogo = new Icon("");
				}
				return windowsLogo;
			}
		}
	}
}
