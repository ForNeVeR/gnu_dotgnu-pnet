using System;
using System.Drawing;

namespace System.Windows.Forms
{
	public class HelpProvider : System.ComponentModel.Component
	{
		private String helpNamespace;
		private System.Collections.Hashtable fields;

		public HelpProvider()
		{
			this.fields = new System.Collections.Hashtable();
		}

		public virtual String HelpNameSpace
		{
			get
			{
				return this.helpNamespace;
			}
			set
			{
				this.helpNamespace = value;
			}
		}

		public virtual String GetHelpKeyword(Control ctl)
		{
			Field field = (Field)this.fields[ctl];
			if(field == null)
			{
				return null;
			}
			return field.HelpKeyword;
		}

		public virtual void SetHelpKeyword(Control ctl, String helpKeyword)
		{
			Field field = this.getField(ctl);
			field.HelpKeyword = helpKeyword;
			field.ShowHelp = true;
		}

		public virtual HelpNavigator GetHelpHelpNavigator(Control ctl)
		{
			Field field = (Field)this.fields[ctl];
			if(field == null)
			{
				return HelpNavigator.AssociateIndex;
			}
			return field.Navigator;
		}

		public virtual void SetHelpHelpNavigator(Control ctl, HelpNavigator helpNavigator)
		{
			this.getField(ctl).Navigator = helpNavigator;
		}

		public virtual String GetHelpString(Control ctl)
		{
			Field field = (Field)this.fields[ctl];
			if(field == null)
			{
				return null;
			}
			return field.HelpString;
		}

		public virtual void SetHelpString(Control ctl, String helpString)
		{
			Field field = this.getField(ctl);
			field.HelpString = helpString;
			field.ShowHelp = true;
		}

		public virtual bool GetShowHelp(Control ctl)
		{
			Field field = (Field)this.fields[ctl];
			if(field == null)
			{
				return false;
			}
			return field.ShowHelp;
		}

		public virtual void SetShowHelp(Control ctl, bool showHelp)
		{
			this.getField(ctl).ShowHelp = showHelp;
		}

		private Field getField(Control ctl)
		{
			Field field = (Field)this.fields[ctl];
			if(field == null)
			{
				field = new Field(ctl);
				this.fields[ctl] = field;
			}
			return field;
		}

		private class Field
		{
			public readonly Control		Control;
			public String			HelpKeyword;
			public HelpNavigator		Navigator;
			public String			HelpString;
			public bool			ShowHelp;

			public Field(Control control)
			{
				this.Control = control;
				this.Navigator = HelpNavigator.AssociateIndex;
				this.ShowHelp = false;
			}
		}

	}

}
