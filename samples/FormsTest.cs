using System;
using System.Drawing;
using System.Windows.Forms;

namespace TextBoxTest
{
	public class TextBoxTest : System.Windows.Forms.Form
	{
		#region declares
		private TabControl tabControl1;
		private TabPage tabPage1;
		private TabPage tabPage2;
		private TabPage tabPage3;
		private TabPage tabPage4;
		private TabPage tabPage5;
		private TabPage tabPage6;
		private TabPage tabPage7;
		
		// Tab1 Labels Test
		private System.Windows.Forms.Label label;
		private System.Windows.Forms.Label label2;
		private System.Windows.Forms.Label label3;
		private System.Windows.Forms.Label label4;
		private System.Windows.Forms.Label label5;
		private System.Windows.Forms.Label label6;
		private System.Windows.Forms.Label label7;
		private System.Windows.Forms.Label label8;
		private System.Windows.Forms.Label label9;
		private System.Windows.Forms.Label label10;
		private System.Windows.Forms.Label label11;
		private System.Windows.Forms.Label label12;
		private System.Windows.Forms.Label label13;
		private System.Windows.Forms.Label label14;
		private System.Windows.Forms.Label label15;
		private System.Windows.Forms.Label label16;
		private System.Windows.Forms.Label label17;
		private System.Windows.Forms.Label label18;
		private System.Windows.Forms.Label label19;
		private System.Windows.Forms.Label label20;
		private System.Windows.Forms.Label label21;
		private System.Windows.Forms.Label label22;
		private System.Windows.Forms.Label label23;
		private System.Windows.Forms.Label label24;
		private System.Windows.Forms.Label label25;

		// Tab2 Button Test
		private System.Windows.Forms.Button button;
		private System.Windows.Forms.Button button2;
		private System.Windows.Forms.Button button3;
		private System.Windows.Forms.Button button4;
		private System.Windows.Forms.Button button5;
		private System.Windows.Forms.Button button6;
		private System.Windows.Forms.Button button7;
		private System.Windows.Forms.Button button8;
		private System.Windows.Forms.Button button9;
		private System.Windows.Forms.Button button10;
		private System.Windows.Forms.Button button11;
		private System.Windows.Forms.Button button12;
		private System.Windows.Forms.Button button13;
		private System.Windows.Forms.Button button14;
		private System.Windows.Forms.Button button15;
		private System.Windows.Forms.Button button16;
		private System.Windows.Forms.Button button17;
		private System.Windows.Forms.Button button18;
		private System.Windows.Forms.Button button19;
		private System.Windows.Forms.Button button20;
		private System.Windows.Forms.Button button21;
		private System.Windows.Forms.Button button22;
		private System.Windows.Forms.Button button23;

		// Tab3 TextBox Test
		private System.Windows.Forms.TextBox textBox;
		private System.Windows.Forms.TextBox textBox1;
		private System.Windows.Forms.TextBox textBox2;
		private System.Windows.Forms.TextBox textBox3;
		private System.Windows.Forms.TextBox textBox4;
		private System.Windows.Forms.TextBox textBox5;
		private System.Windows.Forms.TextBox textBox6;
		private System.Windows.Forms.TextBox textBox7;
		private System.Windows.Forms.TextBox textBox8;
		private System.Windows.Forms.TextBox textBox9;
		private System.Windows.Forms.TextBox textBox10;
		private System.Windows.Forms.TextBox textBox14;
		private System.Windows.Forms.TextBox textBox11;
		private System.Windows.Forms.TextBox textBox12;
		private System.Windows.Forms.TextBox textBox13;
		private System.Windows.Forms.TextBox textBox15;
		private System.Windows.Forms.TextBox textBox16;
		private System.Windows.Forms.TextBox textBox17;
		private System.Windows.Forms.TextBox textBox18;
		private System.Windows.Forms.TextBox textBox19;
		private System.Windows.Forms.TextBox textBox20;

		// Tab4 RadioButtons Test
		private System.Windows.Forms.RadioButton radioButton;
		private System.Windows.Forms.RadioButton radioButton2;
		private System.Windows.Forms.RadioButton radioButton3;
		private System.Windows.Forms.RadioButton radioButton4;
		private System.Windows.Forms.RadioButton radioButton5;
		private System.Windows.Forms.RadioButton radioButton6;
		private System.Windows.Forms.RadioButton radioButton7;
		private System.Windows.Forms.RadioButton radioButton8;
		private System.Windows.Forms.RadioButton radioButton9;
		private System.Windows.Forms.RadioButton radioButton10;
		private System.Windows.Forms.RadioButton radioButton11;
		private System.Windows.Forms.RadioButton radioButton12;
		private System.Windows.Forms.RadioButton radioButton13;
		private System.Windows.Forms.RadioButton radioButton14;
		private System.Windows.Forms.RadioButton radioButton15;
		private System.Windows.Forms.RadioButton radioButton16;
		private System.Windows.Forms.RadioButton radioButton17;
		private System.Windows.Forms.RadioButton radioButton18;
		private System.Windows.Forms.RadioButton radioButton19;
		private System.Windows.Forms.RadioButton radioButton20;
		private System.Windows.Forms.RadioButton radioButton21;
		private System.Windows.Forms.RadioButton radioButton22;
		private System.Windows.Forms.RadioButton radioButton23;
		private System.Windows.Forms.RadioButton radioButton24;
		private System.Windows.Forms.RadioButton radioButton25;
		private System.Windows.Forms.RadioButton radioButton26;
		private System.Windows.Forms.RadioButton radioButton27;
		private System.Windows.Forms.RadioButton radioButton28;
		private System.Windows.Forms.RadioButton radioButton29;
		private System.Windows.Forms.RadioButton radioButton30;
		private System.Windows.Forms.RadioButton radioButton31;
		private System.Windows.Forms.RadioButton radioButton32;
		private System.Windows.Forms.RadioButton radioButton33;

		//Tab6 TabTest
		private System.Windows.Forms.TabPage tabPageT1;
		private System.Windows.Forms.TabPage tabPageT2;
		private System.Windows.Forms.TabPage tabPageT3;
		private System.Windows.Forms.TabPage tabPageT4;
		private System.Windows.Forms.TabPage tabPageT5;
		private System.Windows.Forms.TabControl tabControlT2;
		private System.Windows.Forms.TabPage tabPageT6;
		private System.Windows.Forms.TabControl tabControlT3;
		private System.Windows.Forms.TabPage tabPageT7;
		private System.Windows.Forms.TabPage tabPageT8;
		private System.Windows.Forms.Button buttonT1;
		private System.Windows.Forms.Label labelT1;
		private System.Windows.Forms.Label labelT2;
		private System.Windows.Forms.TabPage tabPageT9;
		private System.Windows.Forms.TabPage tabPageT10;
		private System.Windows.Forms.TabControl tabControlT4;
		private System.Windows.Forms.TabPage tabPageT11;
		private System.Windows.Forms.TabPage tabPageT12;
		private System.Windows.Forms.TabPage tabPageT13;
		private System.Windows.Forms.TabPage tabPageT14;
		private System.Windows.Forms.TabPage tabPageT15;
		private System.Windows.Forms.TabPage tabPageT16;
		private System.Windows.Forms.TabPage tabPageT17;
		private System.Windows.Forms.TabPage tabPageT18;
		private System.Windows.Forms.TabPage tabPageT19;
		private System.Windows.Forms.TabPage tabPageT20;
		private System.Windows.Forms.TabPage tabPageT21;
		private System.Windows.Forms.TabPage tabPageT22;
		private System.Windows.Forms.TabPage tabPageT23;
		private System.Windows.Forms.TabPage tabPageT24;
		private System.Windows.Forms.TabPage tabPageT25;
		private System.Windows.Forms.TabControl tabControlT5;
		private System.Windows.Forms.TabPage tabPageT26;
		private System.Windows.Forms.TabPage tabPageT27;
		private System.Windows.Forms.TabPage tabPageT28;
		private System.Windows.Forms.TabControl tabControlT6;
		private System.Windows.Forms.TabPage tabPageT29;
		private System.Windows.Forms.TabPage tabPageT30;
		private System.Windows.Forms.TabPage tabPageT31;
		private System.Windows.Forms.TabControl tabControlT7;
		private System.Windows.Forms.TabPage tabPageT32;
		private System.Windows.Forms.TabPage tabPageT33;
		private System.Windows.Forms.TabControl Docked;
		private System.Windows.Forms.TabPage tabPageT34;
		private System.Windows.Forms.TabControl tabControlT1;
		private System.Windows.Forms.TabPage tabPageT35;
		private System.Windows.Forms.TabPage tabPageT36;
		private System.Windows.Forms.TabControl tabControlT8;
		private System.Windows.Forms.TabPage tabPageT37;
		private System.Windows.Forms.TabPage tabPageT38;
		private System.Windows.Forms.Label labelT3;
		private System.Windows.Forms.Label labelT4;

		#endregion

		public TextBoxTest()
		{
			ClientSize = new System.Drawing.Size(500, 820);
			Text = "System.Windows.Forms Tests";
			
			//Bug when suspending layout!
			//SuspendLayout();
			tabControl1 = new TabControl();
			tabControl1.Dock = DockStyle.Fill;
			tabPage1 = new TabPage();
			tabPage1.Text = "Labels";
			tabControl1.Controls.Add(this.tabPage1);
			tabControl1.SelectedIndex = 0;
			tabPage2 = new TabPage();
			tabPage2.Text = "Buttons";
			tabControl1.Controls.Add(this.tabPage2);
			tabPage3 = new TabPage();
			tabPage3.Text = "TextBoxes";
			tabControl1.Controls.Add(this.tabPage3);
			tabPage4 = new TabPage();
			tabPage4.Text = "RadioButtons";
			tabControl1.Controls.Add(this.tabPage4);
			tabPage5 = new TabPage();
			tabPage5.Text = "Regions";
			tabControl1.Controls.Add(this.tabPage5);
			tabPage6 = new TabPage();
			tabPage6.Text = "TabControls";
			tabControl1.Controls.Add(this.tabPage6);
			tabPage7 = new TabPage();
			tabPage7.Text = "Primitives";
			tabControl1.Controls.Add(this.tabPage7);

			Controls.Add(tabControl1);

			AddLabelTest(tabPage1);
			AddButtonTest(tabPage2);
			AddTextBoxTest(tabPage3);
			AddRadioButtonsTest(tabPage4);
			AddRegionsTest(tabPage5);
			AddTabControlsTest(tabPage6);
			AddPrimitivesTest(tabPage7);

			//ResumeLayout(false);


		}

		private void AddLabelTest(Control control)
		{
			this.label22 = new System.Windows.Forms.Label();
			this.label23 = new System.Windows.Forms.Label();
			this.label8 = new System.Windows.Forms.Label();
			this.label9 = new System.Windows.Forms.Label();
			this.label4 = new System.Windows.Forms.Label();
			this.label5 = new System.Windows.Forms.Label();
			this.label6 = new System.Windows.Forms.Label();
			this.label7 = new System.Windows.Forms.Label();
			this.label2 = new System.Windows.Forms.Label();
			this.label3 = new System.Windows.Forms.Label();
			this.label15 = new System.Windows.Forms.Label();
			this.label14 = new System.Windows.Forms.Label();
			this.label17 = new System.Windows.Forms.Label();
			this.label16 = new System.Windows.Forms.Label();
			this.label11 = new System.Windows.Forms.Label();
			this.label10 = new System.Windows.Forms.Label();
			this.label13 = new System.Windows.Forms.Label();
			this.label20 = new System.Windows.Forms.Label();
			this.label21 = new System.Windows.Forms.Label();
			this.label24 = new System.Windows.Forms.Label();
			this.label25 = new System.Windows.Forms.Label();
			this.label19 = new System.Windows.Forms.Label();
			this.label18 = new System.Windows.Forms.Label();
			this.label12 = new System.Windows.Forms.Label();
			this.label = new System.Windows.Forms.Label();

			//
			// label22
			//
			this.label22.Dock = System.Windows.Forms.DockStyle.Top;
			this.label22.Location = new System.Drawing.Point(0, 0);
			this.label22.Name = "label22";
			this.label22.Size = new System.Drawing.Size(480, 16);
			this.label22.TabIndex = 21;
			this.label22.Text = "A normal label : Dock=Top";

			//
			// label23
			//
			this.label23.Dock = System.Windows.Forms.DockStyle.Bottom;
			this.label23.Location = new System.Drawing.Point(0, 517);
			this.label23.Name = "label23";
			this.label23.Size = new System.Drawing.Size(480, 16);
			this.label23.TabIndex = 22;
			this.label23.Text = "A normal label : Dock=Bottom";

			//
			// label8
			//
			this.label8.Location = new System.Drawing.Point(10, 125);
			this.label8.Name = "label8";
			this.label8.Size = new System.Drawing.Size(464, 16);
			this.label8.TabIndex = 7;
			this.label8.Text = "A normal label : TextAlign=MiddleLeft";
			this.label8.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;

			//
			// label9
			//
			this.label9.Location = new System.Drawing.Point(10, 185);
			this.label9.Name = "label9";
			this.label9.Size = new System.Drawing.Size(464, 16);
			this.label9.TabIndex = 8;
			this.label9.Text = "A normal label : TextAlign=BottomLeft";
			this.label9.TextAlign = System.Drawing.ContentAlignment.BottomLeft;

			//
			// label4
			//
			this.label4.Location = new System.Drawing.Point(10, 85);
			this.label4.Name = "label4";
			this.label4.Size = new System.Drawing.Size(464, 16);
			this.label4.TabIndex = 3;
			this.label4.Text = "A normal label : TextAlign=TopCenter";
			this.label4.TextAlign = System.Drawing.ContentAlignment.TopCenter;

			//
			// label5
			//
			this.label5.Location = new System.Drawing.Point(10, 145);
			this.label5.Name = "label5";
			this.label5.Size = new System.Drawing.Size(464, 16);
			this.label5.TabIndex = 4;
			this.label5.Text = "A normal label : TextAlign=MiddleCenter";
			this.label5.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;

			//
			// label6
			//
			this.label6.Location = new System.Drawing.Point(10, 165);
			this.label6.Name = "label6";
			this.label6.Size = new System.Drawing.Size(464, 16);
			this.label6.TabIndex = 5;
			this.label6.Text = "A normal label : TextAlign=MiddleRight";
			this.label6.TextAlign = System.Drawing.ContentAlignment.MiddleRight;

			//
			// label7
			//
			this.label7.Location = new System.Drawing.Point(10, 105);
			this.label7.Name = "label7";
			this.label7.Size = new System.Drawing.Size(464, 16);
			this.label7.TabIndex = 6;
			this.label7.Text = "A normal label : TextAlign=TopRight";
			this.label7.TextAlign = System.Drawing.ContentAlignment.TopRight;

			//
			// label2
			//
			this.label2.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
			this.label2.Location = new System.Drawing.Point(10, 45);
			this.label2.Name = "label2";
			this.label2.Size = new System.Drawing.Size(464, 16);
			this.label2.TabIndex = 1;
			this.label2.Text = "A normal label : BorderStyle=FixedSingle";

			//
			// label3
			//
			this.label3.BorderStyle = System.Windows.Forms.BorderStyle.Fixed3D;
			this.label3.Location = new System.Drawing.Point(10, 65);
			this.label3.Name = "label3";
			this.label3.Size = new System.Drawing.Size(464, 16);
			this.label3.TabIndex = 2;
			this.label3.Text = "A normal label : BorderStyle=Fixed3D";

			//
			// label15
			//
			this.label15.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Italic, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
			this.label15.Location = new System.Drawing.Point(10, 305);
			this.label15.Name = "label15";
			this.label15.Size = new System.Drawing.Size(464, 16);
			this.label15.TabIndex = 14;
			this.label15.Text = "A normal label : Font.Italic=true";

			//
			// label14
			//
			this.label14.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
			this.label14.Location = new System.Drawing.Point(10, 285);
			this.label14.Name = "label14";
			this.label14.Size = new System.Drawing.Size(464, 16);
			this.label14.TabIndex = 13;
			this.label14.Text = "A normal label : Font.Bold=true";


			//
			// label17
			//
			this.label17.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Underline, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
			this.label17.Location = new System.Drawing.Point(10, 345);
			this.label17.Name = "label17";
			this.label17.Size = new System.Drawing.Size(464, 16);
			this.label17.TabIndex = 16;
			this.label17.Text = "A normal label : Font.Underline=true";

			//
			// label16
			//
			this.label16.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Strikeout, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
			this.label16.Location = new System.Drawing.Point(10, 325);
			this.label16.Name = "label16";
			this.label16.Size = new System.Drawing.Size(464, 16);
			this.label16.TabIndex = 15;
			this.label16.Text = "A normal label : Font.Strikeout=true";

			//
			// label11
			//
			this.label11.Location = new System.Drawing.Point(10, 225);
			this.label11.Name = "label11";
			this.label11.Size = new System.Drawing.Size(464, 16);
			this.label11.TabIndex = 10;
			this.label11.Text = "A normal label : TextAlign=BottomRight";
			this.label11.TextAlign = System.Drawing.ContentAlignment.BottomRight;

			//
			// label10
			//
			this.label10.Location = new System.Drawing.Point(10, 205);
			this.label10.Name = "label10";
			this.label10.Size = new System.Drawing.Size(464, 16);
			this.label10.TabIndex = 9;
			this.label10.Text = "A normal label : TextAlign=BottomCenter";
			this.label10.TextAlign = System.Drawing.ContentAlignment.BottomCenter;

			//
			// label13
			//
			this.label13.ForeColor = System.Drawing.Color.Red;
			this.label13.Location = new System.Drawing.Point(10, 265);
			this.label13.Name = "label13";
			this.label13.Size = new System.Drawing.Size(464, 16);
			this.label13.TabIndex = 12;
			this.label13.Text = "A normal label : ForeColor=Red";

			//
			// label20
			//
			this.label20.BackColor = System.Drawing.SystemColors.Control;
			this.label20.Enabled = false;
			this.label20.Location = new System.Drawing.Point(10, 405);
			this.label20.Name = "label20";
			this.label20.Size = new System.Drawing.Size(464, 16);
			this.label20.TabIndex = 19;
			this.label20.Text = "A normal label : Enabled=false";

			//
			// label21
			//
			this.label21.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
				| System.Windows.Forms.AnchorStyles.Left)
				| System.Windows.Forms.AnchorStyles.Right)));
			this.label21.BackColor = System.Drawing.SystemColors.Control;
			this.label21.Location = new System.Drawing.Point(10, 495);
			this.label21.Name = "label21";
			this.label21.Size = new System.Drawing.Size(460, 15);
			this.label21.TabIndex = 20;
			this.label21.Text = "A normal label : Anchor=All";

			//
			// label24
			//
			this.label24.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
			this.label24.Location = new System.Drawing.Point(10, 425);
			this.label24.Name = "label24";
			this.label24.Size = new System.Drawing.Size(464, 40);
			this.label24.TabIndex = 23;
			this.label24.Text = "A normal label : BorderStyle=FixedSingle , Height=40";

			//
			// label25
			//
			this.label25.AutoSize = true;
			this.label25.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
			this.label25.Location = new System.Drawing.Point(10, 470);
			this.label25.Name = "label25";
			this.label25.Size = new System.Drawing.Size(294, 19);
			this.label25.TabIndex = 24;
			this.label25.Text = "A normal label : BorderStyle=FixedSingle , AutoSize=true";

			//
			// label19
			//
			this.label19.BackColor = System.Drawing.Color.Red;
			this.label19.Location = new System.Drawing.Point(10, 385);
			this.label19.Name = "label19";
			this.label19.Size = new System.Drawing.Size(464, 16);
			this.label19.TabIndex = 18;
			this.label19.Text = "A normal label : BackColor=Red";

			//
			// label18
			//
			this.label18.Font = new System.Drawing.Font("Verdana", 9.75F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
			this.label18.Location = new System.Drawing.Point(10, 365);
			this.label18.Name = "label18";
			this.label18.Size = new System.Drawing.Size(464, 16);
			this.label18.TabIndex = 17;
			this.label18.Text = "A normal label : Font=Verdana; 9.75pt";

			//
			// label12
			//
			this.label12.Location = new System.Drawing.Point(10, 245);
			this.label12.Name = "label12";
			this.label12.Size = new System.Drawing.Size(464, 16);
			this.label12.TabIndex = 11;
			this.label12.Text = "A normal label : With a &shortcut letter (Alt+S)";

			//
			// label
			//
			this.label.Location = new System.Drawing.Point(10, 25);
			this.label.Name = "label";
			this.label.Size = new System.Drawing.Size(464, 16);
			this.label.TabIndex = 0;
			this.label.Text = "A normal label";

			control.Controls.AddRange(new System.Windows.Forms.Control[] {
																			 this.label25,
																			 this.label24,
																			 this.label23,
																			 this.label22,
																			 this.label21,
																			 this.label20,
																			 this.label19,
																			 this.label18,
																			 this.label17,
																			 this.label16,
																			 this.label15,
																			 this.label14,
																			 this.label13,
																			 this.label12,
																			 this.label11,
																			 this.label10,
																			 this.label9,
																			 this.label8,
																			 this.label7,
																			 this.label6,
																			 this.label5,
																			 this.label4,
																			 this.label3,
																			 this.label2,
																			 this.label});
		}

		private void AddButtonTest(Control control)
		{
			this.button20 = new System.Windows.Forms.Button();
			this.button21 = new System.Windows.Forms.Button();
			this.button6 = new System.Windows.Forms.Button();
			this.button18 = new System.Windows.Forms.Button();
			this.button19 = new System.Windows.Forms.Button();
			this.button14 = new System.Windows.Forms.Button();
			this.button17 = new System.Windows.Forms.Button();
			this.button16 = new System.Windows.Forms.Button();
			this.button15 = new System.Windows.Forms.Button();
			this.button8 = new System.Windows.Forms.Button();
			this.button9 = new System.Windows.Forms.Button();
			this.button11 = new System.Windows.Forms.Button();
			this.button10 = new System.Windows.Forms.Button();
			this.button4 = new System.Windows.Forms.Button();
			this.button5 = new System.Windows.Forms.Button();
			this.button = new System.Windows.Forms.Button();
			this.button7 = new System.Windows.Forms.Button();
			this.button13 = new System.Windows.Forms.Button();
			this.button12 = new System.Windows.Forms.Button();
			this.button2 = new System.Windows.Forms.Button();
			this.button3 = new System.Windows.Forms.Button();
			this.button22 = new System.Windows.Forms.Button();
			this.button23 = new System.Windows.Forms.Button();

			//
			// button20
			//
			this.button20.Location = new System.Drawing.Point(8, 616);
			this.button20.Name = "button20";
			this.button20.Size = new System.Drawing.Size(480, 32);
			this.button20.TabIndex = 19;
			this.button20.Text = "A normal button : TextAlign=BottomLeft";
			this.button20.TextAlign = System.Drawing.ContentAlignment.BottomLeft;

			//
			// button21
			//
			this.button21.Location = new System.Drawing.Point(8, 656);
			this.button21.Name = "button21";
			this.button21.Size = new System.Drawing.Size(480, 32);
			this.button21.TabIndex = 20;
			this.button21.Text = "A normal button : TextAlign=BottomCenter";
			this.button21.TextAlign = System.Drawing.ContentAlignment.BottomCenter;

			//
			// button6
			//
			this.button6.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
			this.button6.Location = new System.Drawing.Point(8, 128);
			this.button6.Name = "button6";
			this.button6.Size = new System.Drawing.Size(480, 24);
			this.button6.TabIndex = 5;
			this.button6.Text = "A normal button : FlatStyle=Flat";

			//
			// button18
			//
			this.button18.Location = new System.Drawing.Point(8, 536);
			this.button18.Name = "button18";
			this.button18.Size = new System.Drawing.Size(480, 32);
			this.button18.TabIndex = 17;
			this.button18.Text = "A normal button : TextAlign=MiddleLeft";
			this.button18.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;

			//
			// button19
			//
			this.button19.Location = new System.Drawing.Point(8, 576);
			this.button19.Name = "button19";
			this.button19.Size = new System.Drawing.Size(480, 32);
			this.button19.TabIndex = 18;
			this.button19.Text = "A normal button : TextAlign=MiddleRight";
			this.button19.TextAlign = System.Drawing.ContentAlignment.MiddleRight;

			//
			// button14
			//
			this.button14.ForeColor = System.Drawing.Color.Red;
			this.button14.Location = new System.Drawing.Point(8, 384);
			this.button14.Name = "button14";
			this.button14.Size = new System.Drawing.Size(480, 24);
			this.button14.TabIndex = 13;
			this.button14.Text = "A normal button : ForeColor=Red";

			//
			// button17
			//
			this.button17.Location = new System.Drawing.Point(8, 496);
			this.button17.Name = "button17";
			this.button17.Size = new System.Drawing.Size(480, 32);
			this.button17.TabIndex = 16;
			this.button17.Text = "A normal button : TextAlign=TopRight";
			this.button17.TextAlign = System.Drawing.ContentAlignment.TopRight;

			//
			// button16
			//
			this.button16.Location = new System.Drawing.Point(8, 456);
			this.button16.Name = "button16";
			this.button16.Size = new System.Drawing.Size(480, 32);
			this.button16.TabIndex = 15;
			this.button16.Text = "A normal button : TextAlign=TopCenter";
			this.button16.TextAlign = System.Drawing.ContentAlignment.TopCenter;

			//
			// button15
			//
			this.button15.Location = new System.Drawing.Point(8, 416);
			this.button15.Name = "button15";
			this.button15.Size = new System.Drawing.Size(480, 32);
			this.button15.TabIndex = 14;
			this.button15.Text = "A normal button : TextAlign=TopLeft";
			this.button15.TextAlign = System.Drawing.ContentAlignment.TopLeft;

			//
			// button8
			//
			this.button8.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
			this.button8.Location = new System.Drawing.Point(8, 192);
			this.button8.Name = "button8";
			this.button8.Size = new System.Drawing.Size(480, 24);
			this.button8.TabIndex = 7;
			this.button8.Text = "A normal button : Font.Bold=true";

			//
			// button9
			//
			this.button9.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Italic, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
			this.button9.Location = new System.Drawing.Point(8, 224);
			this.button9.Name = "button9";
			this.button9.Size = new System.Drawing.Size(480, 24);
			this.button9.TabIndex = 8;
			this.button9.Text = "A normal button : Font.Italic=true";

			//
			// button11
			//
			this.button11.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Underline, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
			this.button11.Location = new System.Drawing.Point(8, 288);
			this.button11.Name = "button11";
			this.button11.Size = new System.Drawing.Size(480, 24);
			this.button11.TabIndex = 10;
			this.button11.Text = "A normal button : Font.Underline=true";

			//
			// button10
			//
			this.button10.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Strikeout, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
			this.button10.Location = new System.Drawing.Point(8, 256);
			this.button10.Name = "button10";
			this.button10.Size = new System.Drawing.Size(480, 24);
			this.button10.TabIndex = 9;
			this.button10.Text = "A normal button : Font.Strikeout=true";

			//
			// button4
			//
			this.button4.Enabled = false;
			this.button4.Location = new System.Drawing.Point(8, 64);
			this.button4.Name = "button4";
			this.button4.Size = new System.Drawing.Size(480, 24);
			this.button4.TabIndex = 3;
			this.button4.Text = "A normal button : Enabled=false";

			//
			// button5
			//
			this.button5.BackColor = System.Drawing.Color.Red;
			this.button5.Location = new System.Drawing.Point(8, 96);
			this.button5.Name = "button5";
			this.button5.Size = new System.Drawing.Size(480, 24);
			this.button5.TabIndex = 4;
			this.button5.Text = "A normal button : BackColor=Red";

			//
			// button
			//
			this.button.Dock = System.Windows.Forms.DockStyle.Top;
			this.button.Location = new System.Drawing.Point(0, 0);
			this.button.Name = "button";
			this.button.Size = new System.Drawing.Size(496, 24);
			this.button.TabIndex = 0;
			this.button.Text = "A normal button : Dock=Top";

			//
			// button7
			//
			this.button7.FlatStyle = System.Windows.Forms.FlatStyle.Popup;
			this.button7.Location = new System.Drawing.Point(8, 160);
			this.button7.Name = "button7";
			this.button7.Size = new System.Drawing.Size(480, 24);
			this.button7.TabIndex = 6;
			this.button7.Text = "A normal button : FlatStyle=Popup";

			//
			// button13
			//
			this.button13.Font = new System.Drawing.Font("Verdana", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
			this.button13.Location = new System.Drawing.Point(8, 352);
			this.button13.Name = "button13";
			this.button13.Size = new System.Drawing.Size(480, 24);
			this.button13.TabIndex = 12;
			this.button13.Text = "A normal button : Font=Verdana; 8,25pt";

			//
			// button12
			//
			this.button12.Font = new System.Drawing.Font("Microsoft Sans Serif", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
			this.button12.Location = new System.Drawing.Point(8, 320);
			this.button12.Name = "button12";
			this.button12.Size = new System.Drawing.Size(480, 24);
			this.button12.TabIndex = 11;
			this.button12.Text = "A normal button : Font.Size=10";

			//
			// button2
			//
			this.button2.Location = new System.Drawing.Point(8, 32);
			this.button2.Name = "button2";
			this.button2.Size = new System.Drawing.Size(480, 24);
			this.button2.TabIndex = 1;
			this.button2.Text = "A normal button";

			//
			// button3
			//
			this.button3.Dock = System.Windows.Forms.DockStyle.Bottom;
			this.button3.Location = new System.Drawing.Point(0, 765);
			this.button3.Name = "button3";
			this.button3.Size = new System.Drawing.Size(496, 24);
			this.button3.TabIndex = 2;
			this.button3.Text = "A normal button : Dock=Bottom";

			//
			// button22
			//
			this.button22.Location = new System.Drawing.Point(8, 696);
			this.button22.Name = "button22";
			this.button22.Size = new System.Drawing.Size(480, 32);
			this.button22.TabIndex = 21;
			this.button22.Text = "A normal button : TextAlign=BottomRight";
			this.button22.TextAlign = System.Drawing.ContentAlignment.BottomRight;

			//
			// button23
			//
			this.button23.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
				| System.Windows.Forms.AnchorStyles.Left)
				| System.Windows.Forms.AnchorStyles.Right)));
			this.button23.Location = new System.Drawing.Point(8, 736);
			this.button23.Name = "button23";
			this.button23.Size = new System.Drawing.Size(480, 24);
			this.button23.TabIndex = 22;
			this.button23.Text = "A normal button : Anchor=All";

			control.Controls.AddRange(new System.Windows.Forms.Control[] {
																			 this.button23,
																			 this.button22,
																			 this.button21,
																			 this.button20,
																			 this.button19,
																			 this.button18,
																			 this.button17,
																			 this.button16,
																			 this.button15,
																			 this.button14,
																			 this.button13,
																			 this.button12,
																			 this.button11,
																			 this.button10,
																			 this.button9,
																			 this.button8,
																			 this.button7,
																			 this.button6,
																			 this.button5,
																			 this.button4,
																			 this.button3,
																			 this.button2,
																			 this.button});
		}
	
		private void AddTextBoxTest(Control control)
		{

			this.textBox19 = new System.Windows.Forms.TextBox();
			this.textBox3 = new System.Windows.Forms.TextBox();
			this.textBox20 = new System.Windows.Forms.TextBox();
			this.textBox18 = new System.Windows.Forms.TextBox();
			this.textBox2 = new System.Windows.Forms.TextBox();
			this.textBox1 = new System.Windows.Forms.TextBox();
			this.textBox6 = new System.Windows.Forms.TextBox();
			this.textBox7 = new System.Windows.Forms.TextBox();
			this.textBox4 = new System.Windows.Forms.TextBox();
			this.textBox5 = new System.Windows.Forms.TextBox();
			this.textBox10 = new System.Windows.Forms.TextBox();
			this.textBox8 = new System.Windows.Forms.TextBox();
			this.textBox9 = new System.Windows.Forms.TextBox();
			this.textBox13 = new System.Windows.Forms.TextBox();
			this.textBox12 = new System.Windows.Forms.TextBox();
			this.textBox11 = new System.Windows.Forms.TextBox();
			this.textBox = new System.Windows.Forms.TextBox();
			this.textBox17 = new System.Windows.Forms.TextBox();
			this.textBox16 = new System.Windows.Forms.TextBox();
			this.textBox15 = new System.Windows.Forms.TextBox();
			this.textBox14 = new System.Windows.Forms.TextBox();

			//
			// textBox19
			//
			this.textBox19.Dock = System.Windows.Forms.DockStyle.Top;
			this.textBox19.Location = new System.Drawing.Point(0, 0);
			this.textBox19.Name = "textBox19";
			this.textBox19.Size = new System.Drawing.Size(450, 20);
			this.textBox19.TabIndex = 19;
			this.textBox19.Text = "A normal textbox - MS Sans Serif : Dock=Top";

			//
			// textBox3
			//
			this.textBox3.BorderStyle = System.Windows.Forms.BorderStyle.FixedSingle;
			this.textBox3.Location = new System.Drawing.Point(8, 95);
			this.textBox3.Name = "textBox3";
			this.textBox3.Size = new System.Drawing.Size(450, 20);
			this.textBox3.TabIndex = 3;
			this.textBox3.Text = "A normal textbox - MS Sans Serif : BorderStyle=FixedSingle";

			//
			// textBox20
			//
			this.textBox20.Enabled = false;
			this.textBox20.Location = new System.Drawing.Point(10, 390);
			this.textBox20.Name = "textBox20";
			this.textBox20.Size = new System.Drawing.Size(450, 20);
			this.textBox20.TabIndex = 20;
			this.textBox20.Text = "A normal textbox - MS Sans Serif : Enabled=false";

			//
			// textBox18
			//
			this.textBox18.Dock = System.Windows.Forms.DockStyle.Bottom;
			this.textBox18.Location = new System.Drawing.Point(0, 518);
			this.textBox18.Name = "textBox18";
			this.textBox18.Size = new System.Drawing.Size(450, 20);
			this.textBox18.TabIndex = 18;
			this.textBox18.Text = "A normal textbox - MS Sans Serif : Dock=Bottom";

			//
			// textBox2
			//
			this.textBox2.Location = new System.Drawing.Point(8, 70);
			this.textBox2.Name = "textBox2";
			this.textBox2.Size = new System.Drawing.Size(450, 20);
			this.textBox2.TabIndex = 2;
			this.textBox2.Text = "A normal textbox - MS Sans Serif : TextAlign=Center";
			this.textBox2.TextAlign = System.Windows.Forms.HorizontalAlignment.Center;

			//
			// textBox1
			//
			this.textBox1.Location = new System.Drawing.Point(8, 45);
			this.textBox1.Name = "textBox1";
			this.textBox1.Size = new System.Drawing.Size(450, 20);
			this.textBox1.TabIndex = 0;
			this.textBox1.Text = "A normal textbox - MS Sans Serif : TextAlign=Right";
			this.textBox1.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;

			//
			// textBox6
			//
			this.textBox6.Font = new System.Drawing.Font("Serifa BT", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
			this.textBox6.Location = new System.Drawing.Point(10, 190);
			this.textBox6.Name = "textBox6";
			this.textBox6.Size = new System.Drawing.Size(450, 20);
			this.textBox6.TabIndex = 6;
			this.textBox6.Text = "A normal textbox - Serifa BT";

			//
			// textBox7
			//
			this.textBox7.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
			this.textBox7.Location = new System.Drawing.Point(8, 215);
			this.textBox7.Name = "textBox7";
			this.textBox7.Size = new System.Drawing.Size(450, 20);
			this.textBox7.TabIndex = 7;
			this.textBox7.Text = "A normal textbox - MS Sans Serif : Font.Bold=true";

			//
			// textBox4
			//
			this.textBox4.Location = new System.Drawing.Point(8, 120);
			this.textBox4.Multiline = true;
			this.textBox4.Name = "textBox4";
			this.textBox4.ScrollBars = System.Windows.Forms.ScrollBars.Both;
			this.textBox4.Size = new System.Drawing.Size(450, 40);
			this.textBox4.TabIndex = 4;
			this.textBox4.Text = "A MultiLine textbox - MS Sans Serif : ScrollBars=Both, Height=40";

			//
			// textBox5
			//
			this.textBox5.Font = new System.Drawing.Font("Arial", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
			this.textBox5.Location = new System.Drawing.Point(10, 165);
			this.textBox5.Name = "textBox5";
			this.textBox5.Size = new System.Drawing.Size(450, 21);
			this.textBox5.TabIndex = 5;
			this.textBox5.Text = "A normal textbox - Arial; 9pt";

			//
			// textBox10
			//
			this.textBox10.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Underline);
			this.textBox10.Location = new System.Drawing.Point(10, 290);
			this.textBox10.Name = "textBox10";
			this.textBox10.Size = new System.Drawing.Size(450, 20);
			this.textBox10.TabIndex = 10;
			this.textBox10.Text = "A normal textbox - MS Sans Serif : Font.Underline=true";

			//
			// textBox8
			//
			this.textBox8.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Italic);
			this.textBox8.Location = new System.Drawing.Point(10, 240);
			this.textBox8.Name = "textBox8";
			this.textBox8.Size = new System.Drawing.Size(450, 20);
			this.textBox8.TabIndex = 8;
			this.textBox8.Text = "A normal textbox - MS Sans Serif : Font.Italic=true";

			//
			// textBox9
			//
			this.textBox9.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Strikeout);
			this.textBox9.Location = new System.Drawing.Point(10, 265);
			this.textBox9.Name = "textBox9";
			this.textBox9.Size = new System.Drawing.Size(450, 20);
			this.textBox9.TabIndex = 9;
			this.textBox9.Text = "A normal textbox - MS Sans Serif : Font.StrikeOut=true";

			//
			// textBox13
			//
			this.textBox13.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F);
			this.textBox13.Location = new System.Drawing.Point(10, 365);
			this.textBox13.Name = "textBox13";
			this.textBox13.ReadOnly = true;
			this.textBox13.Size = new System.Drawing.Size(450, 20);
			this.textBox13.TabIndex = 13;
			this.textBox13.Text = "A normal textbox - MS Sans Serif : ReadOnly=true";

			//
			// textBox12
			//
			this.textBox12.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F);
			this.textBox12.Location = new System.Drawing.Point(10, 340);
			this.textBox12.Name = "textBox12";
			this.textBox12.PasswordChar = '*';
			this.textBox12.Size = new System.Drawing.Size(450, 20);
			this.textBox12.TabIndex = 12;
			this.textBox12.Text = "A normal textbox - MS Sans Serif : PasswordChar=*";

			//
			// textBox11
			//
			this.textBox11.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F);
			this.textBox11.ForeColor = System.Drawing.Color.Red;
			this.textBox11.Location = new System.Drawing.Point(10, 315);
			this.textBox11.Name = "textBox11";
			this.textBox11.Size = new System.Drawing.Size(450, 20);
			this.textBox11.TabIndex = 11;
			this.textBox11.Text = "A normal textbox - MS Sans Serif : ForeColor=red";

			//
			// textBox
			//
			this.textBox.Location = new System.Drawing.Point(8, 25);
			this.textBox.Name = "textBox";
			this.textBox.Size = new System.Drawing.Size(450, 20);
			this.textBox.TabIndex = 1;
			this.textBox.Text = "A normal textbox - MS Sans Serif";

			//
			// textBox17
			//
			this.textBox17.AcceptsTab = true;
			this.textBox17.Location = new System.Drawing.Point(10, 490);
			this.textBox17.Name = "textBox17";
			this.textBox17.Size = new System.Drawing.Size(450, 20);
			this.textBox17.TabIndex = 17;
			this.textBox17.Text = "A normal textbox - MS Sans Serif : AllowTab=true";

			//
			// textBox16
			//
			this.textBox16.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
				| System.Windows.Forms.AnchorStyles.Left)
				| System.Windows.Forms.AnchorStyles.Right)));
			this.textBox16.Location = new System.Drawing.Point(10, 465);
			this.textBox16.Name = "textBox16";
			this.textBox16.Size = new System.Drawing.Size(450, 20);
			this.textBox16.TabIndex = 16;
			this.textBox16.Text = "A normal textbox - MS Sans Serif : Anchor=All";

			//
			// textBox15
			//
			this.textBox15.BackColor = System.Drawing.Color.Red;
			this.textBox15.Location = new System.Drawing.Point(10, 440);
			this.textBox15.Name = "textBox15";
			this.textBox15.Size = new System.Drawing.Size(450, 20);
			this.textBox15.TabIndex = 15;
			this.textBox15.Text = "A normal textbox - MS Sans Serif : BackColor=Red";

			//
			// textBox14
			//
			this.textBox14.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F);
			this.textBox14.Location = new System.Drawing.Point(10, 415);
			this.textBox14.MaxLength = 50;
			this.textBox14.Name = "textBox14";
			this.textBox14.Size = new System.Drawing.Size(450, 20);
			this.textBox14.TabIndex = 14;
			this.textBox14.Text = "A normal textbox - MS Sans Serif : MaxLength=50";
			control.Controls.AddRange(new System.Windows.Forms.Control[] {
																			this.textBox19,
																			this.textBox20,
																			this.textBox18,
																			this.textBox17,
																			this.textBox16,
																			this.textBox15,
																			this.textBox14,
																			this.textBox13,
																			this.textBox12,
																			this.textBox11,
																			this.textBox10,
																			this.textBox9,
																			this.textBox8,
																			this.textBox7,
																			this.textBox6,
																			this.textBox5,
																			this.textBox4,
																			this.textBox3,
																			this.textBox2,
																			this.textBox,
																			this.textBox1});
		}

		private void AddRadioButtonsTest(Control control)
		{
			this.radioButton18 = new System.Windows.Forms.RadioButton();
			this.radioButton19 = new System.Windows.Forms.RadioButton();
			this.radioButton14 = new System.Windows.Forms.RadioButton();
			this.radioButton15 = new System.Windows.Forms.RadioButton();
			this.radioButton16 = new System.Windows.Forms.RadioButton();
			this.radioButton17 = new System.Windows.Forms.RadioButton();
			this.radioButton10 = new System.Windows.Forms.RadioButton();
			this.radioButton11 = new System.Windows.Forms.RadioButton();
			this.radioButton12 = new System.Windows.Forms.RadioButton();
			this.radioButton13 = new System.Windows.Forms.RadioButton();
			this.radioButton29 = new System.Windows.Forms.RadioButton();
			this.radioButton28 = new System.Windows.Forms.RadioButton();
			this.radioButton27 = new System.Windows.Forms.RadioButton();
			this.radioButton24 = new System.Windows.Forms.RadioButton();
			this.radioButton23 = new System.Windows.Forms.RadioButton();
			this.radioButton32 = new System.Windows.Forms.RadioButton();
			this.radioButton9 = new System.Windows.Forms.RadioButton();
			this.radioButton8 = new System.Windows.Forms.RadioButton();
			this.radioButton5 = new System.Windows.Forms.RadioButton();
			this.radioButton4 = new System.Windows.Forms.RadioButton();
			this.radioButton7 = new System.Windows.Forms.RadioButton();
			this.radioButton6 = new System.Windows.Forms.RadioButton();
			this.radioButton26 = new System.Windows.Forms.RadioButton();
			this.radioButton25 = new System.Windows.Forms.RadioButton();
			this.radioButton3 = new System.Windows.Forms.RadioButton();
			this.radioButton2 = new System.Windows.Forms.RadioButton();
			this.radioButton22 = new System.Windows.Forms.RadioButton();
			this.radioButton21 = new System.Windows.Forms.RadioButton();
			this.radioButton20 = new System.Windows.Forms.RadioButton();
			this.radioButton = new System.Windows.Forms.RadioButton();
			this.radioButton33 = new System.Windows.Forms.RadioButton();
			this.radioButton30 = new System.Windows.Forms.RadioButton();
			this.radioButton31 = new System.Windows.Forms.RadioButton();

			//
			// radioButton18
			//
			this.radioButton18.Location = new System.Drawing.Point(8, 368);
			this.radioButton18.Name = "radioButton18";
			this.radioButton18.RightToLeft = System.Windows.Forms.RightToLeft.Yes;
			this.radioButton18.Size = new System.Drawing.Size(288, 24);
			this.radioButton18.TabIndex = 17;
			this.radioButton18.Text = "A normal RadioButton : RightToLeft=True";

			//
			// radioButton19
			//
			this.radioButton19.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
			this.radioButton19.Location = new System.Drawing.Point(312, 352);
			this.radioButton19.Name = "radioButton19";
			this.radioButton19.Size = new System.Drawing.Size(288, 24);
			this.radioButton19.TabIndex = 18;
			this.radioButton19.Text = "A normal RadioButton : Font.Bold=True";

			//
			// radioButton14
			//
			this.radioButton14.CheckAlign = System.Drawing.ContentAlignment.TopLeft;
			this.radioButton14.Location = new System.Drawing.Point(8, 128);
			this.radioButton14.Name = "radioButton14";
			this.radioButton14.Size = new System.Drawing.Size(288, 24);
			this.radioButton14.TabIndex = 13;
			this.radioButton14.Text = "A normal RadioButton :  CheckAlign=TopLeft";

			//
			// radioButton15
			//
			this.radioButton15.Dock = System.Windows.Forms.DockStyle.Top;
			this.radioButton15.Location = new System.Drawing.Point(0, 0);
			this.radioButton15.Name = "radioButton15";
			this.radioButton15.Size = new System.Drawing.Size(608, 24);
			this.radioButton15.TabIndex = 14;
			this.radioButton15.Text = "A normal RadioButton: Dock=Top";

			//
			// radioButton16
			//
			this.radioButton16.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Underline, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
			this.radioButton16.Location = new System.Drawing.Point(312, 448);
			this.radioButton16.Name = "radioButton16";
			this.radioButton16.Size = new System.Drawing.Size(288, 24);
			this.radioButton16.TabIndex = 39;
			this.radioButton16.Text = "A normal RadioButton : Font.Underline=True";

			//
			// radioButton17
			//
			this.radioButton17.FlatStyle = System.Windows.Forms.FlatStyle.Flat;
			this.radioButton17.Location = new System.Drawing.Point(312, 288);
			this.radioButton17.Name = "radioButton17";
			this.radioButton17.Size = new System.Drawing.Size(288, 24);
			this.radioButton17.TabIndex = 16;
			this.radioButton17.Text = "A normal RadioButton : FlatStyle=Flat";

			//
			// radioButton10
			//
			this.radioButton10.BackColor = System.Drawing.Color.SeaGreen;
			this.radioButton10.Location = new System.Drawing.Point(8, 400);
			this.radioButton10.Name = "radioButton10";
			this.radioButton10.Size = new System.Drawing.Size(288, 24);
			this.radioButton10.TabIndex = 9;
			this.radioButton10.Text = "A normal RadioButton : BackColor=SeaGreen";

			//
			// radioButton11
			//
			this.radioButton11.Location = new System.Drawing.Point(312, 32);
			this.radioButton11.Name = "radioButton11";
			this.radioButton11.Size = new System.Drawing.Size(288, 24);
			this.radioButton11.TabIndex = 10;
			this.radioButton11.Text = "A normal RadioButton : TextAlign=MiddleCenter";
			this.radioButton11.TextAlign = System.Drawing.ContentAlignment.MiddleCenter;

			//
			// radioButton12
			//
			this.radioButton12.FlatStyle = System.Windows.Forms.FlatStyle.Popup;
			this.radioButton12.Location = new System.Drawing.Point(312, 320);
			this.radioButton12.Name = "radioButton12";
			this.radioButton12.Size = new System.Drawing.Size(288, 24);
			this.radioButton12.TabIndex = 37;
			this.radioButton12.Text = "A normal RadioButton : FlatStyle=Popup";

			//
			// radioButton13
			//
			this.radioButton13.CheckAlign = System.Drawing.ContentAlignment.BottomLeft;
			this.radioButton13.Location = new System.Drawing.Point(8, 232);
			this.radioButton13.Name = "radioButton13";
			this.radioButton13.Size = new System.Drawing.Size(288, 24);
			this.radioButton13.TabIndex = 12;
			this.radioButton13.Text = "A normal RadioButton : CheckAlign=BottomLeft";

			//
			// radioButton29
			//
			this.radioButton29.Location = new System.Drawing.Point(312, 256);
			this.radioButton29.Name = "radioButton29";
			this.radioButton29.Size = new System.Drawing.Size(288, 24);
			this.radioButton29.TabIndex = 36;
			this.radioButton29.Text = "A normal RadioButton : TextAlign=BottomRight";
			this.radioButton29.TextAlign = System.Drawing.ContentAlignment.BottomRight;

			//
			// radioButton28
			//
			this.radioButton28.Location = new System.Drawing.Point(312, 224);
			this.radioButton28.Name = "radioButton28";
			this.radioButton28.Size = new System.Drawing.Size(288, 24);
			this.radioButton28.TabIndex = 35;
			this.radioButton28.Text = "A normal RadioButton : TextAlign=BottomCenter";
			this.radioButton28.TextAlign = System.Drawing.ContentAlignment.BottomCenter;

			//
			// radioButton27
			//
			this.radioButton27.Location = new System.Drawing.Point(312, 192);
			this.radioButton27.Name = "radioButton27";
			this.radioButton27.Size = new System.Drawing.Size(288, 24);
			this.radioButton27.TabIndex = 34;
			this.radioButton27.Text = "A normal RadioButton : TextAlign=BottomLeft";
			this.radioButton27.TextAlign = System.Drawing.ContentAlignment.BottomLeft;

			//
			// radioButton24
			//
			this.radioButton24.Location = new System.Drawing.Point(312, 160);
			this.radioButton24.Name = "radioButton24";
			this.radioButton24.Size = new System.Drawing.Size(288, 24);
			this.radioButton24.TabIndex = 33;
			this.radioButton24.Text = "A normal RadioButton : TextAlign=TopRight";
			this.radioButton24.TextAlign = System.Drawing.ContentAlignment.TopRight;

			//
			// radioButton23
			//
			this.radioButton23.CheckAlign = System.Drawing.ContentAlignment.MiddleRight;
			this.radioButton23.Location = new System.Drawing.Point(8, 96);
			this.radioButton23.Name = "radioButton23";
			this.radioButton23.Size = new System.Drawing.Size(288, 24);
			this.radioButton23.TabIndex = 26;
			this.radioButton23.Text = "A normal RadioButton : CheckAlign=MiddleRight";

			//
			// radioButton32
			//
			this.radioButton32.ForeColor = System.Drawing.Color.Green;
			this.radioButton32.Location = new System.Drawing.Point(8, 432);
			this.radioButton32.Name = "radioButton32";
			this.radioButton32.Size = new System.Drawing.Size(288, 24);
			this.radioButton32.TabIndex = 42;
			this.radioButton32.Text = "A normal RadioButton : ForeColor=Green";

			//
			// radioButton9
			//
			this.radioButton9.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Italic, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
			this.radioButton9.Location = new System.Drawing.Point(312, 384);
			this.radioButton9.Name = "radioButton9";
			this.radioButton9.Size = new System.Drawing.Size(288, 24);
			this.radioButton9.TabIndex = 8;
			this.radioButton9.Text = "A normal RadioButton : Font.Italic=True";

			//
			// radioButton8
			//
			this.radioButton8.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Strikeout, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
			this.radioButton8.Location = new System.Drawing.Point(312, 416);
			this.radioButton8.Name = "radioButton8";
			this.radioButton8.Size = new System.Drawing.Size(288, 24);
			this.radioButton8.TabIndex = 38;
			this.radioButton8.Text = "A normal RadioButton : Font.Strikeout=True";

			//
			// radioButton5
			//
			this.radioButton5.Dock = System.Windows.Forms.DockStyle.Bottom;
			this.radioButton5.Location = new System.Drawing.Point(0, 541);
			this.radioButton5.Name = "radioButton5";
			this.radioButton5.Size = new System.Drawing.Size(608, 24);
			this.radioButton5.TabIndex = 4;
			this.radioButton5.Text = "A normal RadioButton : Dock = Bottom";

			//
			// radioButton4
			//
			this.radioButton4.CheckAlign = System.Drawing.ContentAlignment.TopRight;
			this.radioButton4.Location = new System.Drawing.Point(8, 200);
			this.radioButton4.Name = "radioButton4";
			this.radioButton4.Size = new System.Drawing.Size(288, 24);
			this.radioButton4.TabIndex = 3;
			this.radioButton4.Text = "A normal RadioButton : CheckAlign=TopRight";

			//
			// radioButton7
			//
			this.radioButton7.Checked = true;
			this.radioButton7.Location = new System.Drawing.Point(8, 336);
			this.radioButton7.Name = "radioButton7";
			this.radioButton7.Size = new System.Drawing.Size(288, 24);
			this.radioButton7.TabIndex = 6;
			this.radioButton7.TabStop = true;
			this.radioButton7.Text = "A normal RadioButton : Checked=True";

			//
			// radioButton6
			//
			this.radioButton6.Location = new System.Drawing.Point(8, 32);
			this.radioButton6.Name = "radioButton6";
			this.radioButton6.Size = new System.Drawing.Size(288, 24);
			this.radioButton6.TabIndex = 22;
			this.radioButton6.Text = "A normal RadioButton";

			//
			// radioButton26
			//
			this.radioButton26.Location = new System.Drawing.Point(312, 96);
			this.radioButton26.Name = "radioButton26";
			this.radioButton26.Size = new System.Drawing.Size(288, 24);
			this.radioButton26.TabIndex = 31;
			this.radioButton26.Text = "A normal RadioButton : TextAlign=TopLeft";
			this.radioButton26.TextAlign = System.Drawing.ContentAlignment.TopLeft;

			//
			// radioButton25
			//
			this.radioButton25.Location = new System.Drawing.Point(312, 128);
			this.radioButton25.Name = "radioButton25";
			this.radioButton25.Size = new System.Drawing.Size(288, 24);
			this.radioButton25.TabIndex = 32;
			this.radioButton25.Text = "A normal RadioButton : TextAlign=TopCenter";
			this.radioButton25.TextAlign = System.Drawing.ContentAlignment.TopCenter;

			//
			// radioButton3
			//
			this.radioButton3.CheckAlign = System.Drawing.ContentAlignment.BottomRight;
			this.radioButton3.Location = new System.Drawing.Point(8, 304);
			this.radioButton3.Name = "radioButton3";
			this.radioButton3.Size = new System.Drawing.Size(288, 24);
			this.radioButton3.TabIndex = 2;
			this.radioButton3.Text = "A normal RadioButton : CheckAlign=BottomRight";

			//
			// radioButton2
			//
			this.radioButton2.CheckAlign = System.Drawing.ContentAlignment.TopCenter;
			this.radioButton2.Location = new System.Drawing.Point(8, 160);
			this.radioButton2.Name = "radioButton2";
			this.radioButton2.Size = new System.Drawing.Size(288, 32);
			this.radioButton2.TabIndex = 1;
			this.radioButton2.Text = "A normal RadioButton : CheckAlign=TopCenter";

			//
			// radioButton22
			//
			this.radioButton22.CheckAlign = System.Drawing.ContentAlignment.MiddleCenter;
			this.radioButton22.Location = new System.Drawing.Point(8, 64);
			this.radioButton22.Name = "radioButton22";
			this.radioButton22.Size = new System.Drawing.Size(288, 24);
			this.radioButton22.TabIndex = 25;
			this.radioButton22.Text = "A normal RadioButton : CheckAlign=MiddleCenter";

			//
			// radioButton21
			//
			this.radioButton21.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
				| System.Windows.Forms.AnchorStyles.Left)
				| System.Windows.Forms.AnchorStyles.Right)));
			this.radioButton21.Location = new System.Drawing.Point(8, 496);
			this.radioButton21.Name = "radioButton21";
			this.radioButton21.Size = new System.Drawing.Size(288, 24);
			this.radioButton21.TabIndex = 21;
			this.radioButton21.Text = "A normal RadioButton : Anchor=All";

			//
			// radioButton20
			//
			this.radioButton20.CheckAlign = System.Drawing.ContentAlignment.BottomCenter;
			this.radioButton20.Location = new System.Drawing.Point(8, 264);
			this.radioButton20.Name = "radioButton20";
			this.radioButton20.Size = new System.Drawing.Size(288, 32);
			this.radioButton20.TabIndex = 23;
			this.radioButton20.Text = "A normal RadioButton : CheckAlign=BottomCenter";

			//
			// radioButton
			//
			this.radioButton.Location = new System.Drawing.Point(312, 64);
			this.radioButton.Name = "radioButton";
			this.radioButton.Size = new System.Drawing.Size(288, 24);
			this.radioButton.TabIndex = 0;
			this.radioButton.Text = "A normal RadioButton : TextAlign=MiddleRight";
			this.radioButton.TextAlign = System.Drawing.ContentAlignment.MiddleRight;

			//
			// radioButton33
			//
			this.radioButton33.Appearance = System.Windows.Forms.Appearance.Button;
			this.radioButton33.Location = new System.Drawing.Point(312, 512);
			this.radioButton33.Name = "radioButton33";
			this.radioButton33.Size = new System.Drawing.Size(288, 24);
			this.radioButton33.TabIndex = 43;
			this.radioButton33.Text = "A normal RadioButton : Appearance=Button";

			//
			// radioButton30
			//
			this.radioButton30.Font = new System.Drawing.Font("Microsoft Sans Serif", 10F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
			this.radioButton30.Location = new System.Drawing.Point(8, 464);
			this.radioButton30.Name = "radioButton30";
			this.radioButton30.Size = new System.Drawing.Size(288, 24);
			this.radioButton30.TabIndex = 40;
			this.radioButton30.Text = "A normal RadioButton : Font.Size=10";

			//
			// radioButton31
			//
			this.radioButton31.Font = new System.Drawing.Font("Verdana", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
			this.radioButton31.Location = new System.Drawing.Point(312, 480);
			this.radioButton31.Name = "radioButton31";
			this.radioButton31.Size = new System.Drawing.Size(288, 24);
			this.radioButton31.TabIndex = 41;
			this.radioButton31.Text = "A normal RadioButton : Font=Verdana; 8,25pt";

			control.Controls.AddRange(new System.Windows.Forms.Control[] {
																			 this.radioButton33,
																			 this.radioButton32,
																			 this.radioButton31,
																			 this.radioButton30,
																			 this.radioButton16,
																			 this.radioButton8,
																			 this.radioButton12,
																			 this.radioButton29,
																			 this.radioButton28,
																			 this.radioButton27,
																			 this.radioButton24,
																			 this.radioButton25,
																			 this.radioButton26,
																			 this.radioButton23,
																			 this.radioButton22,
																			 this.radioButton20,
																			 this.radioButton6,
																			 this.radioButton21,
																			 this.radioButton19,
																			 this.radioButton18,
																			 this.radioButton17,
																			 this.radioButton15,
																			 this.radioButton14,
																			 this.radioButton13,
																			 this.radioButton11,
																			 this.radioButton10,
																			 this.radioButton9,
																			 this.radioButton7,
																			 this.radioButton5,
																			 this.radioButton4,
																			 this.radioButton3,
																			 this.radioButton2,
																			 this.radioButton});
		}
		private void AddRegionsTest(Control control)
		{
			control.Paint+=new PaintEventHandler(Regions_Paint);
		}

		private void AddTabControlsTest(Control control)
		{
			this.Docked = new System.Windows.Forms.TabControl();
			this.tabPageT1 = new System.Windows.Forms.TabPage();
			this.tabControlT2 = new System.Windows.Forms.TabControl();
			this.tabPageT6 = new System.Windows.Forms.TabPage();
			this.labelT2 = new System.Windows.Forms.Label();
			this.labelT1 = new System.Windows.Forms.Label();
			this.buttonT1 = new System.Windows.Forms.Button();
			this.tabPageT22 = new System.Windows.Forms.TabPage();
			this.tabPageT23 = new System.Windows.Forms.TabPage();
			this.tabPageT21 = new System.Windows.Forms.TabPage();
			this.tabPageT9 = new System.Windows.Forms.TabPage();
			this.tabPageT24 = new System.Windows.Forms.TabPage();
			this.tabPageT25 = new System.Windows.Forms.TabPage();
			this.tabPageT10 = new System.Windows.Forms.TabPage();
			this.tabControlT8 = new System.Windows.Forms.TabControl();
			this.tabPageT37 = new System.Windows.Forms.TabPage();
			this.tabPageT38 = new System.Windows.Forms.TabPage();
			this.tabPageT20 = new System.Windows.Forms.TabPage();
			this.tabControlT5 = new System.Windows.Forms.TabControl();
			this.tabPageT26 = new System.Windows.Forms.TabPage();
			this.tabPageT27 = new System.Windows.Forms.TabPage();
			this.tabPageT28 = new System.Windows.Forms.TabPage();
			this.tabPageT2 = new System.Windows.Forms.TabPage();
			this.tabControlT3 = new System.Windows.Forms.TabControl();
			this.tabPageT7 = new System.Windows.Forms.TabPage();
			this.tabPageT8 = new System.Windows.Forms.TabPage();
			this.tabPageT3 = new System.Windows.Forms.TabPage();
			this.tabControlT4 = new System.Windows.Forms.TabControl();
			this.tabPageT11 = new System.Windows.Forms.TabPage();
			this.tabPageT12 = new System.Windows.Forms.TabPage();
			this.tabPageT13 = new System.Windows.Forms.TabPage();
			this.tabPageT14 = new System.Windows.Forms.TabPage();
			this.tabPageT15 = new System.Windows.Forms.TabPage();
			this.tabPageT16 = new System.Windows.Forms.TabPage();
			this.tabPageT17 = new System.Windows.Forms.TabPage();
			this.tabPageT18 = new System.Windows.Forms.TabPage();
			this.tabPageT4 = new System.Windows.Forms.TabPage();
			this.tabControlT6 = new System.Windows.Forms.TabControl();
			this.tabPageT29 = new System.Windows.Forms.TabPage();
			this.tabPageT30 = new System.Windows.Forms.TabPage();
			this.tabPageT31 = new System.Windows.Forms.TabPage();
			this.tabPageT5 = new System.Windows.Forms.TabPage();
			this.tabControlT7 = new System.Windows.Forms.TabControl();
			this.tabPageT32 = new System.Windows.Forms.TabPage();
			this.tabPageT33 = new System.Windows.Forms.TabPage();
			this.tabPageT19 = new System.Windows.Forms.TabPage();
			this.tabPageT34 = new System.Windows.Forms.TabPage();
			this.tabControlT1 = new System.Windows.Forms.TabControl();
			this.tabPageT35 = new System.Windows.Forms.TabPage();
			this.tabPageT36 = new System.Windows.Forms.TabPage();
			this.labelT3 = new System.Windows.Forms.Label();
			this.labelT4 = new System.Windows.Forms.Label();
			
			this.Docked.Controls.Add(this.tabPageT1);
			this.Docked.Controls.Add(this.tabPageT10);
			this.Docked.Controls.Add(this.tabPageT5);
			this.Docked.Controls.Add(this.tabPageT19);
			this.Docked.Controls.Add(this.tabPageT20);
			this.Docked.Controls.Add(this.tabPageT2);
			this.Docked.Controls.Add(this.tabPageT3);
			this.Docked.Controls.Add(this.tabPageT4);
			this.Docked.Controls.Add(this.tabPageT34);
			this.Docked.Location = new System.Drawing.Point(24, 16);
			this.Docked.Name = "Docked";
			this.Docked.SelectedIndex = 0;
			this.Docked.Size = new System.Drawing.Size(384, 320);
			this.Docked.TabIndex = 0;
			// 
			// tabPage1
			// 
			this.tabPageT1.Controls.Add(this.tabControlT2);
			this.tabPageT1.Location = new System.Drawing.Point(4, 22);
			this.tabPageT1.Name = "tabPage1";
			this.tabPageT1.Size = new System.Drawing.Size(376, 294);
			this.tabPageT1.TabIndex = 0;
			this.tabPageT1.Text = "FillToRight";
			// 
			// tabControl2
			// 
			this.tabControlT2.Controls.Add(this.tabPageT6);
			this.tabControlT2.Controls.Add(this.tabPageT22);
			this.tabControlT2.Controls.Add(this.tabPageT23);
			this.tabControlT2.Controls.Add(this.tabPageT21);
			this.tabControlT2.Controls.Add(this.tabPageT9);
			this.tabControlT2.Controls.Add(this.tabPageT24);
			this.tabControlT2.Controls.Add(this.tabPageT25);
			this.tabControlT2.Location = new System.Drawing.Point(16, 24);
			this.tabControlT2.Multiline = true;
			this.tabControlT2.Name = "tabControl2";
			this.tabControlT2.SelectedIndex = 0;
			this.tabControlT2.Size = new System.Drawing.Size(344, 248);
			this.tabControlT2.SizeMode = System.Windows.Forms.TabSizeMode.FillToRight;
			this.tabControlT2.TabIndex = 1;
			// 
			// tabPage6
			// 
			this.tabPageT6.Controls.Add(this.labelT2);
			this.tabPageT6.Controls.Add(this.labelT1);
			this.tabPageT6.Controls.Add(this.buttonT1);
			this.tabPageT6.Location = new System.Drawing.Point(4, 40);
			this.tabPageT6.Name = "tabPage6";
			this.tabPageT6.Size = new System.Drawing.Size(336, 204);
			this.tabPageT6.TabIndex = 0;
			this.tabPageT6.Text = "First";
			// 
			// label2
			// 
			this.labelT2.Location = new System.Drawing.Point(144, 160);
			this.labelT2.Name = "label2";
			this.labelT2.Size = new System.Drawing.Size(112, 40);
			this.labelT2.TabIndex = 2;
			this.labelT2.Text = "label2";
			// 
			// label1
			// 
			this.labelT1.Location = new System.Drawing.Point(144, 88);
			this.labelT1.Name = "label1";
			this.labelT1.Size = new System.Drawing.Size(112, 40);
			this.labelT1.TabIndex = 1;
			this.labelT1.Text = "label1";
			// 
			// button1
			// 
			this.buttonT1.Location = new System.Drawing.Point(24, 16);
			this.buttonT1.Name = "button1";
			this.buttonT1.Size = new System.Drawing.Size(96, 40);
			this.buttonT1.TabIndex = 0;
			this.buttonT1.Text = "button1";
			// 
			// tabPage22
			// 
			this.tabPageT22.Location = new System.Drawing.Point(4, 40);
			this.tabPageT22.Name = "tabPage22";
			this.tabPageT22.Size = new System.Drawing.Size(336, 204);
			this.tabPageT22.TabIndex = 3;
			this.tabPageT22.Text = "tabPage22";
			// 
			// tabPage23
			// 
			this.tabPageT23.Location = new System.Drawing.Point(4, 40);
			this.tabPageT23.Name = "tabPage23";
			this.tabPageT23.Size = new System.Drawing.Size(336, 204);
			this.tabPageT23.TabIndex = 4;
			this.tabPageT23.Text = "tabPage23";
			// 
			// tabPage21
			// 
			this.tabPageT21.Location = new System.Drawing.Point(4, 40);
			this.tabPageT21.Name = "tabPage21";
			this.tabPageT21.Size = new System.Drawing.Size(336, 204);
			this.tabPageT21.TabIndex = 2;
			this.tabPageT21.Text = "tabPage21";
			// 
			// tabPage9
			// 
			this.tabPageT9.Location = new System.Drawing.Point(4, 40);
			this.tabPageT9.Name = "tabPage9";
			this.tabPageT9.Size = new System.Drawing.Size(336, 204);
			this.tabPageT9.TabIndex = 1;
			this.tabPageT9.Text = "Second";
			// 
			// tabPage24
			// 
			this.tabPageT24.Location = new System.Drawing.Point(4, 40);
			this.tabPageT24.Name = "tabPage24";
			this.tabPageT24.Size = new System.Drawing.Size(336, 204);
			this.tabPageT24.TabIndex = 5;
			this.tabPageT24.Text = "tabPage24";
			// 
			// tabPage25
			// 
			this.tabPageT25.Location = new System.Drawing.Point(4, 40);
			this.tabPageT25.Name = "tabPage25";
			this.tabPageT25.Size = new System.Drawing.Size(336, 204);
			this.tabPageT25.TabIndex = 6;
			this.tabPageT25.Text = "tabPage25";
			// 
			// tabPage10
			// 
			this.tabPageT10.Controls.Add(this.tabControlT8);
			this.tabPageT10.Location = new System.Drawing.Point(4, 22);
			this.tabPageT10.Name = "tabPage10";
			this.tabPageT10.Size = new System.Drawing.Size(376, 294);
			this.tabPageT10.TabIndex = 5;
			this.tabPageT10.Text = "OwnerDraw";
			// 
			// tabControl8
			// 
			this.tabControlT8.Controls.Add(this.tabPageT37);
			this.tabControlT8.Controls.Add(this.tabPageT38);
			this.tabControlT8.DrawMode = System.Windows.Forms.TabDrawMode.OwnerDrawFixed;
			this.tabControlT8.Location = new System.Drawing.Point(32, 24);
			this.tabControlT8.Name = "tabControl8";
			this.tabControlT8.SelectedIndex = 0;
			this.tabControlT8.Size = new System.Drawing.Size(328, 248);
			this.tabControlT8.TabIndex = 0;
			this.tabControlT8.DrawItem += new System.Windows.Forms.DrawItemEventHandler(this.tabControlT8_DrawItem);
			// 
			// tabPage37
			// 
			this.tabPageT37.Location = new System.Drawing.Point(4, 22);
			this.tabPageT37.Name = "tabPage37";
			this.tabPageT37.Size = new System.Drawing.Size(320, 222);
			this.tabPageT37.TabIndex = 0;
			this.tabPageT37.Text = "tabPage37";
			// 
			// tabPage38
			// 
			this.tabPageT38.Location = new System.Drawing.Point(4, 22);
			this.tabPageT38.Name = "tabPage38";
			this.tabPageT38.Size = new System.Drawing.Size(320, 222);
			this.tabPageT38.TabIndex = 1;
			this.tabPageT38.Text = "tabPage38";
			// 
			// tabPage20
			// 
			this.tabPageT20.Controls.Add(this.tabControlT5);
			this.tabPageT20.Location = new System.Drawing.Point(4, 22);
			this.tabPageT20.Name = "tabPage20";
			this.tabPageT20.Size = new System.Drawing.Size(376, 294);
			this.tabPageT20.TabIndex = 7;
			this.tabPageT20.Text = "FixedSize";
			// 
			// tabControl5
			// 
			this.tabControlT5.Controls.Add(this.tabPageT26);
			this.tabControlT5.Controls.Add(this.tabPageT27);
			this.tabControlT5.Controls.Add(this.tabPageT28);
			this.tabControlT5.ItemSize = new System.Drawing.Size(100, 30);
			this.tabControlT5.Location = new System.Drawing.Point(24, 32);
			this.tabControlT5.Name = "tabControl5";
			this.tabControlT5.SelectedIndex = 0;
			this.tabControlT5.Size = new System.Drawing.Size(336, 256);
			this.tabControlT5.SizeMode = System.Windows.Forms.TabSizeMode.Fixed;
			this.tabControlT5.TabIndex = 0;
			// 
			// tabPage26
			// 
			this.tabPageT26.Location = new System.Drawing.Point(4, 34);
			this.tabPageT26.Name = "tabPage26";
			this.tabPageT26.Size = new System.Drawing.Size(328, 218);
			this.tabPageT26.TabIndex = 0;
			this.tabPageT26.Text = "a";
			// 
			// tabPage27
			// 
			this.tabPageT27.Location = new System.Drawing.Point(4, 34);
			this.tabPageT27.Name = "tabPage27";
			this.tabPageT27.Size = new System.Drawing.Size(328, 218);
			this.tabPageT27.TabIndex = 1;
			this.tabPageT27.Text = "second";
			// 
			// tabPage28
			// 
			this.tabPageT28.Location = new System.Drawing.Point(4, 34);
			this.tabPageT28.Name = "tabPage28";
			this.tabPageT28.Size = new System.Drawing.Size(328, 218);
			this.tabPageT28.TabIndex = 2;
			this.tabPageT28.Text = "3rd";
			// 
			// tabPage2
			// 
			this.tabPageT2.Controls.Add(this.tabControlT3);
			this.tabPageT2.Location = new System.Drawing.Point(4, 22);
			this.tabPageT2.Name = "tabPage2";
			this.tabPageT2.Size = new System.Drawing.Size(376, 294);
			this.tabPageT2.TabIndex = 1;
			this.tabPageT2.Text = "AlignBot";
			// 
			// tabControl3
			// 
			this.tabControlT3.Alignment = System.Windows.Forms.TabAlignment.Bottom;
			this.tabControlT3.Controls.Add(this.tabPageT7);
			this.tabControlT3.Controls.Add(this.tabPageT8);
			this.tabControlT3.Location = new System.Drawing.Point(32, 32);
			this.tabControlT3.Name = "tabControl3";
			this.tabControlT3.SelectedIndex = 0;
			this.tabControlT3.Size = new System.Drawing.Size(232, 208);
			this.tabControlT3.TabIndex = 1;
			// 
			// tabPage7
			// 
			this.tabPageT7.Location = new System.Drawing.Point(4, 4);
			this.tabPageT7.Name = "tabPage7";
			this.tabPageT7.Size = new System.Drawing.Size(224, 182);
			this.tabPageT7.TabIndex = 0;
			this.tabPageT7.Text = "tabPage7";
			// 
			// tabPage8
			// 
			this.tabPageT8.Location = new System.Drawing.Point(4, 4);
			this.tabPageT8.Name = "tabPage8";
			this.tabPageT8.Size = new System.Drawing.Size(224, 182);
			this.tabPageT8.TabIndex = 1;
			this.tabPageT8.Text = "tabPage8";
			// 
			// tabPage3
			// 
			this.tabPageT3.Controls.Add(this.tabControlT4);
			this.tabPageT3.Location = new System.Drawing.Point(4, 22);
			this.tabPageT3.Name = "tabPage3";
			this.tabPageT3.Size = new System.Drawing.Size(376, 294);
			this.tabPageT3.TabIndex = 2;
			this.tabPageT3.Text = "MultiLine";
			// 
			// tabControl4
			// 
			this.tabControlT4.Controls.Add(this.tabPageT11);
			this.tabControlT4.Controls.Add(this.tabPageT12);
			this.tabControlT4.Controls.Add(this.tabPageT13);
			this.tabControlT4.Controls.Add(this.tabPageT14);
			this.tabControlT4.Controls.Add(this.tabPageT15);
			this.tabControlT4.Controls.Add(this.tabPageT16);
			this.tabControlT4.Controls.Add(this.tabPageT17);
			this.tabControlT4.Controls.Add(this.tabPageT18);
			this.tabControlT4.Location = new System.Drawing.Point(16, 16);
			this.tabControlT4.Multiline = true;
			this.tabControlT4.Name = "tabControl4";
			this.tabControlT4.SelectedIndex = 0;
			this.tabControlT4.Size = new System.Drawing.Size(344, 256);
			this.tabControlT4.TabIndex = 0;
			// 
			// tabPage11
			// 
			this.tabPageT11.Location = new System.Drawing.Point(4, 40);
			this.tabPageT11.Name = "tabPage11";
			this.tabPageT11.Size = new System.Drawing.Size(336, 212);
			this.tabPageT11.TabIndex = 0;
			this.tabPageT11.Text = "tabPage11";
			// 
			// tabPage12
			// 
			this.tabPageT12.Location = new System.Drawing.Point(4, 40);
			this.tabPageT12.Name = "tabPage12";
			this.tabPageT12.Size = new System.Drawing.Size(336, 212);
			this.tabPageT12.TabIndex = 1;
			this.tabPageT12.Text = "tabPage12";
			// 
			// tabPage13
			// 
			this.tabPageT13.Location = new System.Drawing.Point(4, 40);
			this.tabPageT13.Name = "tabPageT13";
			this.tabPageT13.Size = new System.Drawing.Size(336, 212);
			this.tabPageT13.TabIndex = 2;
			this.tabPageT13.Text = "tabPageT13";
			// 
			// tabPageT14
			// 
			this.tabPageT14.Location = new System.Drawing.Point(4, 40);
			this.tabPageT14.Name = "tabPageT14";
			this.tabPageT14.Size = new System.Drawing.Size(336, 212);
			this.tabPageT14.TabIndex = 3;
			this.tabPageT14.Text = "tabPageT14";
			// 
			// tabPageT15
			// 
			this.tabPageT15.Location = new System.Drawing.Point(4, 40);
			this.tabPageT15.Name = "tabPageT15";
			this.tabPageT15.Size = new System.Drawing.Size(336, 212);
			this.tabPageT15.TabIndex = 4;
			this.tabPageT15.Text = "tabPageT15";
			// 
			// tabPageT16
			// 
			this.tabPageT16.Location = new System.Drawing.Point(4, 40);
			this.tabPageT16.Name = "tabPageT16";
			this.tabPageT16.Size = new System.Drawing.Size(336, 212);
			this.tabPageT16.TabIndex = 5;
			this.tabPageT16.Text = "tabPageT16";
			// 
			// tabPageT17
			// 
			this.tabPageT17.Location = new System.Drawing.Point(4, 40);
			this.tabPageT17.Name = "tabPageT17";
			this.tabPageT17.Size = new System.Drawing.Size(336, 212);
			this.tabPageT17.TabIndex = 6;
			this.tabPageT17.Text = "tabPageT17";
			// 
			// tabPageT18
			// 
			this.tabPageT18.BackColor = System.Drawing.Color.Red;
			this.tabPageT18.Location = new System.Drawing.Point(4, 40);
			this.tabPageT18.Name = "tabPageT18";
			this.tabPageT18.Size = new System.Drawing.Size(336, 212);
			this.tabPageT18.TabIndex = 7;
			this.tabPageT18.Text = "tabPageT18";
			// 
			// tabPageT4
			// 
			this.tabPageT4.Controls.Add(this.tabControlT6);
			this.tabPageT4.Location = new System.Drawing.Point(4, 22);
			this.tabPageT4.Name = "tabPageT4";
			this.tabPageT4.Size = new System.Drawing.Size(376, 294);
			this.tabPageT4.TabIndex = 3;
			this.tabPageT4.Text = "Hottrack";
			// 
			// tabControlT6
			// 
			this.tabControlT6.Controls.Add(this.tabPageT29);
			this.tabControlT6.Controls.Add(this.tabPageT30);
			this.tabControlT6.Controls.Add(this.tabPageT31);
			this.tabControlT6.HotTrack = true;
			this.tabControlT6.Location = new System.Drawing.Point(8, 24);
			this.tabControlT6.Name = "tabControlT6";
			this.tabControlT6.SelectedIndex = 0;
			this.tabControlT6.Size = new System.Drawing.Size(352, 248);
			this.tabControlT6.TabIndex = 0;
			// 
			// tabPageT29
			// 
			this.tabPageT29.Location = new System.Drawing.Point(4, 22);
			this.tabPageT29.Name = "tabPageT29";
			this.tabPageT29.Size = new System.Drawing.Size(344, 222);
			this.tabPageT29.TabIndex = 0;
			this.tabPageT29.Text = "tabPageT29";
			// 
			// tabPageT30
			// 
			this.tabPageT30.Location = new System.Drawing.Point(4, 22);
			this.tabPageT30.Name = "tabPageT30";
			this.tabPageT30.Size = new System.Drawing.Size(344, 222);
			this.tabPageT30.TabIndex = 1;
			this.tabPageT30.Text = "tabPageT30";
			// 
			// tabPageT31
			// 
			this.tabPageT31.Location = new System.Drawing.Point(4, 22);
			this.tabPageT31.Name = "tabPageT31";
			this.tabPageT31.Size = new System.Drawing.Size(344, 222);
			this.tabPageT31.TabIndex = 2;
			this.tabPageT31.Text = "tabPageT31";
			// 
			// tabPageT5
			// 
			this.tabPageT5.Controls.Add(this.labelT3);
			this.tabPageT5.Controls.Add(this.tabControlT7);
			this.tabPageT5.Location = new System.Drawing.Point(4, 22);
			this.tabPageT5.Name = "tabPageT5";
			this.tabPageT5.Size = new System.Drawing.Size(376, 294);
			this.tabPageT5.TabIndex = 4;
			this.tabPageT5.Text = "Right";
			// 
			// tabControlT7
			// 
			this.tabControlT7.Alignment = System.Windows.Forms.TabAlignment.Right;
			this.tabControlT7.Controls.Add(this.tabPageT32);
			this.tabControlT7.Controls.Add(this.tabPageT33);
			this.tabControlT7.Location = new System.Drawing.Point(8, 48);
			this.tabControlT7.Multiline = true;
			this.tabControlT7.Name = "tabControlT7";
			this.tabControlT7.SelectedIndex = 0;
			this.tabControlT7.Size = new System.Drawing.Size(360, 232);
			this.tabControlT7.TabIndex = 0;
			// 
			// tabPageT32
			// 
			this.tabPageT32.Location = new System.Drawing.Point(4, 4);
			this.tabPageT32.Name = "tabPageT32";
			this.tabPageT32.Size = new System.Drawing.Size(333, 224);
			this.tabPageT32.TabIndex = 0;
			this.tabPageT32.Text = "tabPageT32";
			// 
			// tabPageT33
			// 
			this.tabPageT33.Location = new System.Drawing.Point(4, 4);
			this.tabPageT33.Name = "tabPageT33";
			this.tabPageT33.Size = new System.Drawing.Size(333, 256);
			this.tabPageT33.TabIndex = 1;
			this.tabPageT33.Text = "tabPageT33";
			// 
			// tabPageT19
			// 
			this.tabPageT19.Controls.Add(this.labelT4);
			this.tabPageT19.Location = new System.Drawing.Point(4, 22);
			this.tabPageT19.Name = "tabPageT19";
			this.tabPageT19.Size = new System.Drawing.Size(376, 294);
			this.tabPageT19.TabIndex = 6;
			this.tabPageT19.Text = "ImageList";
			// 
			// tabPageT34
			// 
			this.tabPageT34.Controls.Add(this.tabControlT1);
			this.tabPageT34.Location = new System.Drawing.Point(4, 22);
			this.tabPageT34.Name = "tabPageT34";
			this.tabPageT34.Size = new System.Drawing.Size(376, 294);
			this.tabPageT34.TabIndex = 8;
			this.tabPageT34.Text = "Docked";
			// 
			// tabControlT1
			// 
			this.tabControlT1.Controls.Add(this.tabPageT35);
			this.tabControlT1.Controls.Add(this.tabPageT36);
			this.tabControlT1.Dock = System.Windows.Forms.DockStyle.Fill;
			this.tabControlT1.Location = new System.Drawing.Point(0, 0);
			this.tabControlT1.Name = "tabControlT1";
			this.tabControlT1.SelectedIndex = 0;
			this.tabControlT1.Size = new System.Drawing.Size(376, 294);
			this.tabControlT1.TabIndex = 0;
			// 
			// tabPageT35
			// 
			this.tabPageT35.Location = new System.Drawing.Point(4, 22);
			this.tabPageT35.Name = "tabPageT35";
			this.tabPageT35.Size = new System.Drawing.Size(368, 268);
			this.tabPageT35.TabIndex = 0;
			this.tabPageT35.Text = "tabPageT35";
			// 
			// tabPageT36
			// 
			this.tabPageT36.Location = new System.Drawing.Point(4, 22);
			this.tabPageT36.Name = "tabPageT36";
			this.tabPageT36.Size = new System.Drawing.Size(208, 158);
			this.tabPageT36.TabIndex = 1;
			this.tabPageT36.Text = "tabPageT36";
			// 
			// label3
			// 
			this.labelT3.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
			this.labelT3.Location = new System.Drawing.Point(112, 8);
			this.labelT3.Name = "label3";
			this.labelT3.Size = new System.Drawing.Size(104, 24);
			this.labelT3.TabIndex = 1;
			this.labelT3.Text = "TODO:";
			// 
			// label4
			// 
			this.labelT4.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
			this.labelT4.Location = new System.Drawing.Point(128, 80);
			this.labelT4.Name = "label4";
			this.labelT4.Size = new System.Drawing.Size(104, 24);
			this.labelT4.TabIndex = 2;
			this.labelT4.Text = "TODO:";
			
			control.Controls.Add(this.Docked);

		}

		private void AddPrimitivesTest(Control control)
		{
			control.Paint+=new PaintEventHandler(DrawPrimitives);
		}
		public static void Main(String[] args)
		{
			TextBoxTest form = new TextBoxTest();
			Application.Run(form);
		}

		private void tabControlT8_DrawItem(object sender, System.Windows.Forms.DrawItemEventArgs e)
		{
			Graphics g = e.Graphics;
			using (Pen p = new Pen(Color.Blue))
			{
				using (Font font = new Font("Arial", 9.0f))
				{
					using (SolidBrush brush = new SolidBrush(Color.Red))
					{
						Rectangle b = e.Bounds;
						b.Inflate(-2,-2);
						g.DrawRectangle(p, b);
						g.DrawString("OWNER", font, brush, b);
					}
				}
			}
		
		}

		private void Regions_Paint(object sender, PaintEventArgs e)
		{
			using (Brush b = new SolidBrush(Color.LightGray), bl = new SolidBrush(Color.Blue))
			{
				e.Graphics.FillRectangle(b, new Rectangle(0,0,Width, Height));
				int count = 10;

				Region region;
				Region[] r = new Region[count];
				r[0] = new Region(new Rectangle(0,0,50,50));
				r[1] = new Region(new Rectangle(25,25,20,20));
				r[2] = new Region(new Rectangle(0,0,30,30));
				r[3] = new Region(new Rectangle(20,20,30,30));
				r[4] = new Region(new Rectangle(5,5,10,10));
				r[5] = new Region(new Rectangle(35,35,10,10));
				r[6] = new Region(new Rectangle(0,0,0,0));
				r[7] = new Region(new Rectangle(35,35,10,10));
				r[8] = new Region(new Rectangle(5,5,10,10));
				r[9] = new Region(new Rectangle(0,0,0,0));

				
				for (int i=0;i<count;i+=2)
				{
					region = r[i].Clone();
					region.Union(r[i+1]);
					DrawRegions(e.Graphics, i/2+count/2*0, "Union " + (i/2+1), r[i], r[i+1], region);
				}

				for (int i=0;i<count;i+=2)
				{
					region = r[i].Clone();
					region.Intersect(r[i+1]);
					DrawRegions(e.Graphics, i/2+count/2*1, "Intersect " + (i/2+1), r[i], r[i+1], region);
				}

				for (int i=0;i<count;i+=2)
				{
					region = r[i].Clone();
					region.Complement(r[i+1]);
					DrawRegions(e.Graphics, i/2+count/2*2, "Complement " + (i/2+1), r[i], r[i+1], region);
				}

				for (int i=0;i<count;i+=2)
				{
					region = r[i].Clone();
					region.Exclude(r[i+1]);
					DrawRegions(e.Graphics, i/2+count/2*3, "Exclude " + (i/2+1), r[i], r[i+1], region);
				}

				for (int i=0;i<count;i+=2)
				{
					region = r[i].Clone();
					region.Xor(r[i+1]);
					DrawRegions(e.Graphics, i/2+count/2*4, "Xor " + (i/2+1), r[i], r[i+1], region);
				}

			}
		}

		private void DrawRegions( Graphics g, int offset, string s, Region r1, Region r2, Region r3)
		{
			using (Pen p = new Pen(Color.Black))
			{
				using (Brush b1 = new SolidBrush(Color.Red), b2 = new SolidBrush(Color.Green), bl = new SolidBrush(Color.Blue))
				{
					Region region1 = r1.Clone();
					Region region2 = r2.Clone();
					Region region3 = r3.Clone();
					int x = (offset%4) * 110;
					int y = (int)offset / 4*80;
					g.DrawString(s,Font, bl, x, y);
					region1.Translate(x, y + 20);
					region2.Translate(x, y + 20);
					region3.Translate(x + 55, y + 20);
					g.FillRegion(b1,region1);
					g.DrawRectangle(p, Rectangle.Truncate(region1.GetBounds(g)));
					g.FillRegion(b2,region2);
					g.DrawRectangle(p, Rectangle.Truncate(region2.GetBounds(g)));
					g.FillRegion(b1,region3);
					g.DrawRectangle(p, Rectangle.Truncate(region3.GetBounds(g)));
				}
			}
		}

		private void DrawPrimitives(object sender, PaintEventArgs e)
		{
			using( Brush b = new SolidBrush(Color.Red))
			{
				using( Pen p = new Pen(Color.Red))
				{
					Graphics g= e.Graphics;
					int x, y;
					DrawPrimitive(g, 0, "(0,0)-(0,10)", out x, out y);
					g.DrawLine(p,x, y, x, y+10);

					DrawPrimitive(g, 1, "(0,0)-(0,1)", out x, out y);
					g.DrawLine(p,x, y, x, y+1);

					DrawPrimitive(g, 2, "(0,0)-(0,0)", out x, out y);
					g.DrawLine(p,x, y, x, y);

					DrawPrimitive(g, 3, "DrawRect(0,0,0,0)", out x, out y);
					g.DrawRectangle(p,x, y, 0, 0);

					DrawPrimitive(g, 4, "DrawRect(0,0,0,1)", out x, out y);
					g.DrawRectangle(p,x, y, 0, 1);

					DrawPrimitive(g, 5, "DrawRect(0,0,1,1)", out x, out y);
					g.DrawRectangle(p,x, y, 1, 1);

					DrawPrimitive(g, 6, "FillRect(0,0,1,0)", out x, out y);
					g.FillRectangle(b,x, y, 1, 0);

					DrawPrimitive(g, 7, "FillRect(0,0,1,1)", out x, out y);
					g.FillRectangle(b,x, y, 1, 1);

					DrawPrimitive(g, 8, "FillRect(0,0,2,2)", out x, out y);
					g.FillRectangle(b,x, y, 2, 2);

					DrawPrimitive(g, 9, "FillPoly(0,0,0,0)", out x, out y);
					g.FillPolygon(b, new PointF[4] {new PointF(x,y), new PointF(x,y), new PointF(x,y), new PointF(x,y)});

					DrawPrimitive(g, 10, "FillPoly(0,0,0,1)", out x, out y);
					g.FillPolygon(b, new PointF[4] {new PointF(x,y), new PointF(x,y), new PointF(x,y+1),new PointF(x,y+1)});

					DrawPrimitive(g, 11, "FillPoly(0,0,1,1)", out x, out y);
					g.FillPolygon(b, new PointF[4] {new PointF(x,y), new PointF(x+1,y), new PointF(x+1,y+1),new PointF(x,y+1)});

					DrawPrimitive(g, 12, "FillPoly(0,0,2,2)", out x, out y);
					g.FillPolygon(b, new PointF[4] {new PointF(x,y), new PointF(x+2,y), new PointF(x+2,y+2), new PointF(x,y+2)});

					DrawPrimitive(g, 13, "DrawPoly(0,0,0,0)", out x, out y);
					g.DrawPolygon(p, new PointF[4] {new PointF(x,y), new PointF(x,y), new PointF(x,y), new PointF(x,y)});

					DrawPrimitive(g, 14, "DrawPoly(0,0,0,1)", out x, out y);
					g.DrawPolygon(p, new PointF[4] {new PointF(x,y), new PointF(x,y), new PointF(x,y+1),new PointF(x,y+1)});

					DrawPrimitive(g, 15, "DrawPoly(0,0,1,1)", out x, out y);
					g.DrawPolygon(p, new PointF[4] {new PointF(x,y), new PointF(x+1,y), new PointF(x+1,y+1),new PointF(x,y+1)});

					DrawPrimitive(g, 16, "DrawPoly(0,0,2,2)", out x, out y);
					g.DrawPolygon(p, new PointF[4] {new PointF(x,y), new PointF(x+2,y), new PointF(x+2,y+2), new PointF(x,y+2)});

					Font f = new Font("Arial", 10);
					DrawPrimitive(g, 17, "Arial 10", out x, out y);
					g.DrawString("Height=" + f.Height, f, b, x, y);

					DrawPrimitive(g, 18, "Arial 10", out x, out y);
					StringFormat sf = new StringFormat();
					sf.SetMeasurableCharacterRanges(new CharacterRange[] { new CharacterRange(0,1), new CharacterRange(1,1) });
					RectangleF re = g.MeasureCharacterRanges("jM",f,new Rectangle(0,0,100,100),sf)[0].GetBounds(g);
					g.DrawString("jM Meas:" + re.Left +"," + re.Top + "," + re.Width + "," + re.Height, f, b, x, y);
					y+=20;
					g.DrawString("jM", f, b, x, y, sf);
					re.Offset(x,y);
					g.DrawRectangle(p,Rectangle.Truncate(re));

					DrawPrimitive(g, 19, "Arial 10", out x, out y);
					re = g.MeasureCharacterRanges("jM",f,new Rectangle(0,0,100,100),sf)[1].GetBounds(g);
					g.DrawString("jM Meas:" + re.Left +"," + re.Top + "," + re.Width + "," + re.Height, f, b, x, y);
					y+=20;
					g.DrawString("jM", f, b, x, y, sf);
					re.Offset(x,y);
					g.DrawRectangle(p,Rectangle.Truncate(re));

					
				}
			}

		}

		private void DrawPrimitive( Graphics g, int offset, string s, out int x, out int y)
		{
			x = (offset%4) * 110;
			y = (int)offset / 4*80;
			using (Brush bl = new SolidBrush(Color.Blue))
			{
				g.DrawString(s,Font, bl, x, y);
				y+=20;
			}
		}
	}
}
