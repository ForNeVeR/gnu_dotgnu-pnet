/*
 * ProcessStartInfo.cs - Implementation of 
 *							"System.Diagnostics.ProcessStartInfo" class
 *
 * Copyright (C) 2002  Southern Storm Software, Pty Ltd.
 * 
 * contributed by Gopal.V 
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
using System.Collections;
using System.Collections.Specialized;

namespace System.Diagnostics
{
	public sealed class ProcessStartInfo
	{
		private String filename=null;
		private String arguments="";
		private bool createNoWindow=false;
		private StringDictionary dict=null;
		private bool errorDialog=false;
		private IntPtr errorDialogHandle=IntPtr.Zero;
		private int redirectFlags=0;
		private bool shellExecute=true;
		private String verb="";
		private ProcessWindowStyle windowStyle=ProcessWindowStyle.Normal;
		private String workingDir=".";
		public ProcessStartInfo()
		{
		}

		public ProcessStartInfo(String filename)
		{
			this.filename=filename;
		}

		public ProcessStartInfo(String filename, String arguments)
		{
			this.filename=filename;
			this.arguments=arguments;
		}

		public String Arguments 
		{
 			get
			{
				return arguments;
			}
 			set
			{
				arguments=value;
			}
 		}

		public bool CreateNoWindow 
		{
 			get
			{
				return createNoWindow;
			}
 			set
			{
				createNoWindow=value;
			}
 		}

		public StringDictionary EnvironmentVariables 
		{
 			get
			{
				if(dict==null)
				{
					dict=new StringDictionary();
					IDictionary env=Environment.GetEnvironmentVariables();
					foreach(String s in env.Keys)
					{
						dict.Add(s,env[s].ToString());
					}
				}
				return this.dict;
			}
 		}

		public bool ErrorDialog 
		{
 			get
			{
				return errorDialog;
			}
 			set
			{
				errorDialog=value;
			}
 		}

		public IntPtr ErrorDialogParentHandle 
		{
 			get
			{
				return errorDialogHandle;
			}
 			set
			{
				errorDialogHandle=value;
			}
 		}

		public String FileName 
		{
 			get
			{
				return filename;
			}
 			set
			{
				filename=value;
			}
 		}

		public bool RedirectStandardError 
		{
 			get
			{
				return ((redirectFlags & 0x04) != 0) ;
			}
 			set
			{
				redirectFlags  = redirectFlags | (value ? 0x04 : 0x00);
			}
 		}

		public bool RedirectStandardInput 
		{
 			get
			{
				return ((redirectFlags & 0x02)!=0);
			}
 			set
			{
				redirectFlags = redirectFlags | (value ? 0x02 : 0x00);
			}
 		}

		public bool RedirectStandardOutput 
		{
 			get
			{
				return ((redirectFlags & 0x01)!=0);
			}
 			set
			{
				redirectFlags = redirectFlags | (value ? 0x01 : 0x00);
			}
 		}

		public bool UseShellExecute 
		{
 			get
			{
				return shellExecute;
			}
 			set
			{
				shellExecute=value;
			}
 		}

		public String Verb 
		{
 			get
			{
				return verb;
			}
 			set
			{
				verb=value;
			}
 		}

		// huh ? ... what does this do ?
		[TODO]
		public String[] Verbs 
		{
 			get
			{
				throw new NotImplementedException("Verbs");
			}
 		}

		public ProcessWindowStyle WindowStyle 
		{
 			get
			{
				return windowStyle;
			}
 			set
			{
				windowStyle=value;
			}
 		}

		public String WorkingDirectory 
		{
 			get
			{
				return workingDir;
			}
 			set
			{
				workingDir=value;
			}
 		}

	}
}//namespace
