/*
 * RadioButton.cs - Implementation of the
 *		"System.Windows.Forms.RichTextBoxFinds" class.
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

namespace System.Windows.Forms
{

using System.Drawing;
using System.ComponentModel;

    public class RadioButton : ButtonBase {

		Appearance appearance;
		bool       autoCheck;
		ContentAlignment checkAlign;
		bool       checked_;

		public RadioButton()
		{
			appearance = Appearance.Normal;
			autoCheck  = true;
			checkAlign = ContentAlignment.MiddleLeft;
			checked_   = false;
			TextAlign  = ContentAlignment.MiddleLeft;			
		}

		public Appearance Appearance
		{
			get
			{
				return appearance;
			}

			set
			{
				if ( !Enum.IsDefined ( typeof(Appearance), value ) )
					throw new InvalidEnumArgumentException( "Appearance", (int)value, typeof(Appearance));

				if ( appearance != value ) {
					appearance = value;
					
					if ( IsHandleCreated )
						;

					if ( AppearanceChanged != null )
						AppearanceChanged ( this, EventArgs.Empty );
				}
			}
		}

		public bool AutoCheck {
			get
			{
				return autoCheck;
			}
			set
			{
				if ( autoCheck != value )
				{
					autoCheck = value;

					if ( IsHandleCreated )
						;
				}
			}
		}

		public ContentAlignment CheckAlign {
			get
			{
				return checkAlign;
			}
			set
			{
				if ( !Enum.IsDefined ( typeof(ContentAlignment), value ) )
					throw new InvalidEnumArgumentException( "CheckAlign", (int)value, typeof(Appearance));

				if ( checkAlign != value ) {
					;
				}
			}
		}

		public bool Checked {
			get 
			{
				return checked_;
			}

			set
			{
				if ( checked_ != value )
				{
					checked_ = value;

					UpdateCheck();
					OnCheckedChanged (EventArgs.Empty);
				}
			}
		}

		public override ContentAlignment TextAlign {
			get
			{
				return base.TextAlign;
			}
			
			set
			{
				base.TextAlign = value;
			}
		}

		public void PerformClick()
		{
			Checked = !Checked;
			OnClick ( EventArgs.Empty );
		}

		public override string ToString()
		{
			return GetType().FullName.ToString () + ", Checked: " + Checked.ToString ( );
		}

		public event EventHandler AppearanceChanged;
		public event EventHandler CheckedChanged;

		protected override ImeMode DefaultImeMode {
			get {	return ImeMode.Disable;	}
		}

		protected override Size DefaultSize {
			get {	return new Size(104,24); }
		}

		protected virtual void OnCheckedChanged(EventArgs e)
		{
			if ( CheckedChanged != null )
				CheckedChanged ( this, e );
		}

		protected override void OnClick(EventArgs e)
		{
			bool check = Checked;

			if ( checked_ != check ) {
				checked_ = check;
				OnCheckedChanged ( EventArgs.Empty );
			}

			base.OnClick ( e );
		}

		protected override void OnHandleCreated(EventArgs e)
		{
			base.OnHandleCreated ( e );
			UpdateCheck ( );
		}

		private void UpdateCheck ( ) {
			if ( IsHandleCreated )
				;
		}
	 }
}
