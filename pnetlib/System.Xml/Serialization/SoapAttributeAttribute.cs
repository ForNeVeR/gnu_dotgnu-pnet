//
// SoapAttributeAttribute.cs: 
//
// Author:
//   John Donagher (john@webmeta.com)
//
// (C) 2002 John Donagher
//

//
// Permission is hereby granted, free of charge, to any person obtaining
// a copy of this software and associated documentation files (the
// "Software"), to deal in the Software without restriction, including
// without limitation the rights to use, copy, modify, merge, publish,
// distribute, sublicense, and/or sell copies of the Software, and to
// permit persons to whom the Software is furnished to do so, subject to
// the following conditions:
// 
// The above copyright notice and this permission notice shall be
// included in all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
// EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
// LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
// OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
// WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//

using System;
using System.Text;

namespace System.Xml.Serialization
{
	/// <summary>
	/// Summary description for SoapAttributeAttribute.
	/// </summary>
	[AttributeUsage(AttributeTargets.Property | AttributeTargets.Field
		 | AttributeTargets.Parameter | AttributeTargets.ReturnValue)]
	public class SoapAttributeAttribute : Attribute
	{
		private string attrName;
		private string dataType;
		private string ns;

		public SoapAttributeAttribute ()
		{
		}

		public SoapAttributeAttribute (string attrName) 
		{
			AttributeName = attrName;
		}

		public string AttributeName {
			get	{
				return attrName;
			} 
			set	{
				attrName = value;
			}
		}

		public string DataType {
			get {
				return dataType;
			} 
			set {
				dataType = value;
			}
		}

		public string Namespace {
			get {
				return ns;
			} 
			set {
				ns = value;
			}
		}
		
		internal void AddKeyHash (System.Text.StringBuilder sb)
		{
			sb.Append ("SAA ");
			KeyHelper.AddField (sb, 1, attrName);
			KeyHelper.AddField (sb, 2, dataType);
			KeyHelper.AddField (sb, 3, ns);
			sb.Append ("|");
		}
	}
}
