/*
 * Interaction.cs - Implementation of the
 *			"Microsoft.VisualBasic.Interaction" class.
 *
 * Copyright (C) 2003, 2004  Southern Storm Software, Pty Ltd.
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
using System.IO;
using System.Text;
using System.Collections;
using System.ComponentModel;
using System.Runtime.InteropServices;
using System.Windows.Forms;
using System.Diagnostics;
using Microsoft.VisualBasic.CompilerServices;
using Microsoft.Win32;

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
	public static Object CallByName
				(Object ObjectRef, String ProcName,
				 CallType UseCallType, Object[] Args)
			{
				switch(UseCallType)
				{
					case CallType.Method:
					{
						return LateBinding.LateCallWithResult
							(ObjectRef, null, ProcName, Args, null, null);
					}
					// Not reached.

					case CallType.Get:
					{
						return LateBinding.LateGet
							(ObjectRef, null, ProcName, Args, null, null);
					}
					// Not reached.

					case CallType.Set:
					case CallType.Let:
					{
						LateBinding.LateSet
							(ObjectRef, null, ProcName, Args, null);
						return null;
					}
					// Not reached.
				}
				throw new ArgumentException(S._("VB_InvalidCallType"));
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
				String[] args = Environment.GetCommandLineArgs();
				if(args == null || args.Length == 0)
				{
					return String.Empty;
				}
				else
				{
					return String.Join(" ", args, 1, args.Length - 1);
				}
			}

	// Create a COM object.
	public static Object CreateObject
				(String ProgId,
				 [Optional] [DefaultValue("")] String ServerName)
			{
				throw new NotImplementedException();
			}

#if CONFIG_WIN32_SPECIFICS

	// Get the registry key corresponding to an app and section.
	private static String GetKey(String AppName, String Section)
			{
				String subkey;
				subkey = "Software\\VB and VBA Program Settings";
				if(AppName != null && AppName.Length != 0)
				{
					subkey += "\\";
					subkey += AppName;
					if(Section != null && Section.Length != 0)
					{
						subkey += "\\";
						subkey += Section;
					}
				}
				return subkey;
			}

	// Delete a registry setting.
	public static void DeleteSetting
				(String AppName,
				 [Optional] [DefaultValue(null)] String Section,
				 [Optional] [DefaultValue(null)] String Key)
			{
				String subkey = GetKey(AppName, Section);
				if(Key != null && Key.Length != 0)
				{
					RegistryKey key = Registry.CurrentUser.OpenSubKey
						(subkey, true);
					try
					{
						key.DeleteValue(Key);
					}
					finally
					{
						key.Close();
					}
				}
				else
				{
					Registry.CurrentUser.DeleteSubKeyTree(subkey);
				}
			}

	// Get all settings within a particular registry section.
	public static String[,] GetAllSettings(String AppName, String Section)
			{
				String subkey = GetKey(AppName, Section);
				RegistryKey key = Registry.CurrentUser.OpenSubKey(subkey);
				try
				{
					String[] names = key.GetValueNames();
					String[,] result = new String [names.Length, 2];
					int posn;
					for(posn = 0; posn < names.Length; ++posn)
					{
						result[posn, 0] = names[posn];
						result[posn, 1] =
							StringType.FromObject(key.GetValue(names[posn]));
					}
					return result;
				}
				finally
				{
					key.Close();
				}
			}

	// Get a particular registry setting.
	public static String GetSetting
				(String AppName, String Section, String Key,
				 [Optional] [DefaultValue(null)] String Default)
			{
				String subkey = GetKey(AppName, Section);
				RegistryKey key = Registry.CurrentUser.OpenSubKey(subkey);
				try
				{
					return StringType.FromObject(key.GetValue(Key, Default));
				}
				finally
				{
					key.Close();
				}
			}

	// Save a particular registry setting.
	public static void SaveSetting(String AppName, String Section,
								   String Key, String Setting)
			{
				String subkey = GetKey(AppName, Section);
				RegistryKey key = Registry.CurrentUser.CreateSubKey(subkey);
				try
				{
					key.SetValue(Key, Setting);
				}
				finally
				{
					key.Close();
				}
			}

#else // !CONFIG_WIN32_SPECIFICS

	// Delete a registry setting.
	public static void DeleteSetting
				(String AppName,
				 [Optional] [DefaultValue(null)] String Section,
				 [Optional] [DefaultValue(null)] String Key)
			{
				// Nothing to do here.
			}

	// Get all settings within a particular registry section.
	public static String[,] GetAllSettings(String AppName, String Section)
			{
				// Nothing to do here.
				return null;
			}

	// Get a particular registry setting.
	public static String GetSetting
				(String AppName, String Section, String Key,
				 [Optional] [DefaultValue(null)] String Default)
			{
				// Nothing to do here.
				return Default;
			}

	// Save a particular registry setting.
	public static void SaveSetting(String AppName, String Section,
								   String Key, String Setting)
			{
				// Nothing to do here.
			}

#endif // !CONFIG_WIN32_SPECIFICS

	// Get a value from the environment.
	public static String Environ(int Expression)
			{
			#if !ECMA_COMPAT
				SortedList list = new SortedList
					(Environment.GetEnvironmentVariables());
				if(Expression > 0 && Expression <= list.Count)
				{
					String key = (list.GetKey(Expression - 1) as String);
					if(key != null)
					{
						String value = (list[key] as String);
						if(value != null)
						{
							return key + "=" + value;
						}
					}
				}
			#endif
				return String.Empty;
			}
	public static String Environ(String Expression)
			{
				return Environment.GetEnvironmentVariable
					(Strings.Trim(Expression));
			}

	// Get a reference to a COM object.
	public static Object GetObject
				([Optional] [DefaultValue(null)] String PathName,
				 [Optional] [DefaultValue(null)] String Class)
			{
				throw new NotImplementedException();
			}

	// Perform an "if".
	public static Object IIf
				(bool Expression, Object TruePart, Object FalsePart)
			{
				return (Expression ? TruePart : FalsePart);
			}

	// Pop up an input box and ask the user a question.
	[TODO]
	public static String InputBox
				(String Prompt,
				 [Optional] [DefaultValue("")] String Title,
				 [Optional] [DefaultValue("")] String DefaultResponse,
				 [Optional] [DefaultValue(-1)] int XPos,
				 [Optional] [DefaultValue(-1)] int YPos)
			{
				// TODO
				return DefaultResponse;
			}

	// Pop up a message box and ask the user for a response.
	public static MsgBoxResult MsgBox
				(Object Prompt,
				 [Optional] [DefaultValue(MsgBoxStyle.OKOnly)]
				 		MsgBoxStyle Buttons,
				 [Optional] [DefaultValue(null)] Object Title)
			{
			#if CONFIG_REFLECTION

				// Consult the host to find the message box's parent window.
				IVbHost host;
				IWin32Window parent;
				host = HostServices.VBHost;
				if(host != null)
				{
					parent = host.GetParentWindow();
				}
				else
				{
					parent = null;
				}

				// Convert the message box style into its WinForms equivalent.
				MessageBoxButtons buttons = MessageBoxButtons.OK;
				MessageBoxIcon icon = MessageBoxIcon.None;
				MessageBoxDefaultButton def = MessageBoxDefaultButton.Button1;
				MessageBoxOptions options = (MessageBoxOptions)0;
				switch(Buttons & (MsgBoxStyle)0x0F)
				{
					case MsgBoxStyle.OKCancel:
						buttons = MessageBoxButtons.OKCancel; break;
					case MsgBoxStyle.AbortRetryIgnore:
						buttons = MessageBoxButtons.AbortRetryIgnore; break;
					case MsgBoxStyle.YesNoCancel:
						buttons = MessageBoxButtons.YesNoCancel; break;
					case MsgBoxStyle.YesNo:
						buttons = MessageBoxButtons.YesNo; break;
					case MsgBoxStyle.RetryCancel:
						buttons = MessageBoxButtons.RetryCancel; break;
				}
				if((Buttons & MsgBoxStyle.Critical) != 0)
				{
					icon = MessageBoxIcon.Hand;
				}
				else if((Buttons & MsgBoxStyle.Question) != 0)
				{
					icon = MessageBoxIcon.Question;
				}
				else if((Buttons & MsgBoxStyle.Exclamation) != 0)
				{
					icon = MessageBoxIcon.Exclamation;
				}
				else if((Buttons & MsgBoxStyle.Information) != 0)
				{
					icon = MessageBoxIcon.Asterisk;
				}
				if((Buttons & MsgBoxStyle.DefaultButton2) != 0)
				{
					def = MessageBoxDefaultButton.Button2;
				}
				else if((Buttons & MsgBoxStyle.DefaultButton3) != 0)
				{
					def = MessageBoxDefaultButton.Button3;
				}
				if((Buttons & MsgBoxStyle.MsgBoxRight) != 0)
				{
					options = MessageBoxOptions.RightAlign;
				}
				if((Buttons & MsgBoxStyle.MsgBoxRtlReading) != 0)
				{
					options = MessageBoxOptions.RtlReading;
				}

				// Pop up the message box and get the WinForms result.
				DialogResult result;
				result = MessageBox.Show
					(parent, StringType.FromObject(Prompt),
					 StringType.FromObject(Title),
					 buttons, icon, def, options);
				return (MsgBoxResult)result;

			#else
				// If there is no reflection, then we don't have WinForms.
				return MsgBoxResult.Cancel;
			#endif
			}

	// Create a string that describes a partition.
	[TODO]
	public static String Partition(long Number, long Start,
								   long Stop, long Interval)
			{
				// TODO
				return null;
			}

	// Execute a program using the Windows shell.
	public static int Shell
				(String Pathname,
				 [Optional] [DefaultValue(AppWinStyle.MinimizedFocus)]
				 		AppWinStyle Style,
				 [Optional] [DefaultValue(false)] bool Wait,
				 [Optional] [DefaultValue(-1)] int Timeout)
			{
			#if CONFIG_EXTENDED_DIAGNOSTICS
				// Split "Pathname" into filename and arguments.
				StringBuilder builder = new StringBuilder();
				if(Pathname == null)
				{
					Pathname = String.Empty;
				}
				int posn = 0;
				bool quote = false;
				while(posn < Pathname.Length)
				{
					if(Char.IsWhiteSpace(Pathname[posn]) && !quote)
					{
						++posn;
						break;
					}
					else if(Pathname[posn] == '"')
					{
						quote = !quote;
					}
					else
					{
						builder.Append(Pathname[posn]);
						++posn;
					}
				}
				String filename = builder.ToString();
				if(filename == null || filename.Length == 0)
				{
					throw new FileNotFoundException
						(String.Format(S._("VB_CouldntExecute"), "(empty)"));
				}

				// Create the process and start it.
				Process process = new Process();
				ProcessStartInfo info = process.StartInfo;
				info.FileName = filename;
				info.Arguments = Pathname.Substring(posn);
				info.UseShellExecute = true;
				switch(Style)
				{
					case AppWinStyle.Hide:
						info.WindowStyle = ProcessWindowStyle.Hidden;
						break;

					case AppWinStyle.MinimizedFocus:
					case AppWinStyle.MinimizedNoFocus:
						info.WindowStyle = ProcessWindowStyle.Minimized;
						break;

					case AppWinStyle.MaximizedFocus:
						info.WindowStyle = ProcessWindowStyle.Maximized;
						break;

					case AppWinStyle.NormalFocus:
					case AppWinStyle.NormalNoFocus:
						info.WindowStyle = ProcessWindowStyle.Normal;
						break;

					default:
						throw new ArgumentException
							(S._("VB_InvalidAppWinStyle"));
				}
				if(!process.Start())
				{
					throw new FileNotFoundException
						(String.Format(S._("VB_CouldntExecute"), Pathname));
				}
				if(Wait)
				{
					if(process.WaitForExit(Timeout))
					{
						return 0;
					}
					else
					{
						return process.Id;
					}
				}
				else
				{
					return process.Id;
				}
			#else
				throw new NotImplementedException();
			#endif
			}

	// Find a true element in an array and return the corresponding object.
	public static Object Switch(params Object[] VarExpr)
			{
				if(VarExpr == null)
				{
					return null;
				}
				int len = VarExpr.Length;
				if((len & 1) != 0)
				{
					throw new ArgumentException(S._("VB_MustBeEvenSized"));
				}
				int posn;
				for(posn = 0; posn < len; posn += 2)
				{
					if(BooleanType.FromObject(VarExpr[posn]))
					{
						return VarExpr[posn + 1];
					}
				}
				return null;
			}

}; // class Interaction

}; // namespace Microsoft.VisualBasic
