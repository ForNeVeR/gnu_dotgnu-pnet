/*
 * Interaction.cs - Implementation of the
 *			"Microsoft.VisualBasic.Interaction" class.
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

namespace Microsoft.VisualBasic
{

using System;
using System.ComponentModel;
using System.Runtime.InteropServices;
using Microsoft.VisualBasic.CompilerServices;

// Many of the defintions in this class are not portable or are insecure,
// so they are stubbed out.

[StandardModule]
public sealed class Interaction
{
	// This class cannot be instantiated.
	private Interaction() {}

	// Activate the main window for a particular process.
	public static void AppActivate(int ProcessID) {}

	// Activate a particular main window, given its title.
	public static void AppActivate(String Title) {}

	// Make the system beep.
	public static void Beep() {}

	// Perform a late bound call.
	[TODO]
	public static Object CallByName
				(Object ObjectRef, String ProcName,
				 CallType UseCallType, Object[] Args)
			{
				// TODO
				return null;
			}

	// Choice a particular item from an array.
	public static Object Choose(double Index, params Object[] Choice)
			{
				int i = checked((int)(Math.Round(Conversion.Fix(Index))));
				if(i >= 0 && i < Choice.Length)
				{
					return Choice[i];
				}
				else
				{
					return null;
				}
			}

	// Get the command that was used to launch this process.
	public static String Command()
			{
				// TODO
				return null;
			}

	// Create a COM object.
	public static Object CreateObject
				(String ProgId,
				 [Optional] [DefaultValue("")] String ServerName)
			{
				throw new NotImplementedException();
			}

	// Delete a registry setting.
	[TODO]
	public static void DeleteSetting
				(String AppName,
				 [Optional] [DefaultValue(null)] String Section,
				 [Optional] [DefaultValue(null)] String Key)
			{
				// TODO
			}

	// Get a value from the environment.
	[TODO]
	public static String Environ(int Expression)
			{
				// TODO
				return null;
			}
	public static String Environ(String Expression)
			{
				return Environment.GetEnvironmentVariable
					(Strings.Trim(Expression));
			}

	// Get all settings within a particular registry section.
	[TODO]
	public static String[,] GetAllSettings(String AppName, String Section)
			{
				// TODO
				return null;
			}

	// Get a reference to a COM object.
	public static Object GetObject
				([Optional] [DefaultValue(null)] String PathName,
				 [Optional] [DefaultValue(null)] String Class)
			{
				throw new NotImplementedException();
			}

	// Get a particular registry setting.
	[TODO]
	public static String GetSetting
				(String AppName, String Section, String Key,
				 [Optional] [DefaultValue(null)] String Default)
			{
				// TODO
				return Default;
			}

	// Perform an "if".
	public static Object IIf
				(bool Expression, Object TruePart, Object FalsePart)
			{
				return (Expression ? TruePart : FalsePart);
			}

	// Pop up an input box and ask the user a question.
	public static String InputBox
				(String Prompt,
				 [Optional] [DefaultValue("")] String Title,
				 [Optional] [DefaultValue("")] String DefaultResponse,
				 [Optional] [DefaultValue(-1)] int XPos,
				 [Optional] [DefaultValue(-1)] int YPos)
			{
				return DefaultResponse;
			}

	// Pop up a message box and ask the user for a response.
	public static MsgBoxResult MsgBox
				(Object Prompt,
				 [Optional] [DefaultValue(MsgBoxStyle.OKOnly)]
				 		MsgBoxStyle Buttons,
				 [Optional] [DefaultValue(null)] String Title)
			{
				return MsgBoxResult.Cancel;
			}

	// Create a string that describes a partition.
	[TODO]
	public static String Partition(long Number, long Start,
								   long Stop, long Interval)
			{
				// TODO
				return null;
			}

	// Save a particular registry setting.
	[TODO]
	public static void SaveSetting(String AppName, String Section,
								   String Key, String Setting)
			{
				// TODO
			}

	// Execute a program using the Windows shell.
	public static int Shell
				(String Pathname,
				 [Optional] [DefaultValue(AppWinStyle.MinimizedFocus)]
				 		AppWinStyle Style,
				 [Optional] [DefaultValue(false)] bool Wait,
				 [Optional] [DefaultValue(-1)] int Timeout)
			{
				return 0;
			}

	// Find a true element in an array and return the corresponding object.
	[TODO]
	public static Object Switch(params Object[] VarExpr)
			{
				// TODO
				return null;
			}

}; // class Interaction

}; // namespace Microsoft.VisualBasic
