using System;
using System.Drawing;
using System.Windows.Forms;
using System.Drawing.Drawing2D;

namespace FormsTest
{
	public class FormsTest : Form
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
		private TabPage tabPage8;
		private TabPage tabPage9;
		private TabPage tabPage10;
		private TabPage tabPage11;
		private TabPage tabPage12;
		private TabPage tabPage13;
		private TabPage tabPage14;
		private TabPage tabPage15;
		private TabPage tabPage16;
		private TabPage tabPage17;
		private TabPage tabPage18;
		
		// Tab1 Labels Test
		private Label label;
		private Label label2;
		private Label label3;
		private Label label4;
		private Label label5;
		private Label label6;
		private Label label7;
		private Label label8;
		private Label label9;
		private Label label10;
		private Label label11;
		private Label label12;
		private Label label13;
		private Label label14;
		private Label label15;
		private Label label16;
		private Label label17;
		private Label label18;
		private Label label19;
		private Label label20;
		private Label label21;
		private Label label22;
		private Label label23;
		private Label label24;
		private Label label25;

		// Tab2 Button Test
		private Button button;
		private Button button2;
		private Button button3;
		private Button button4;
		private Button button5;
		private Button button6;
		private Button button7;
		private Button button12;
		private Button button13;
		private Button button14;
		private Button button15;
		private Button button16;
		private Button button17;
		private Button button18;
		private Button button19;
		private Button button20;
		private Button button21;
		private Button button22;
		private Button button23;

		// Tab3 TextBox Test
		private TextBox textBox;
		private TextBox textBox1;
		private TextBox textBox2;
		private TextBox textBox3;
		private TextBox textBox4;
		private TextBox textBox5;
		private TextBox textBox6;
		private TextBox textBox7;
		private TextBox textBox8;
		private TextBox textBox9;
		private TextBox textBox10;
		private TextBox textBox14;
		private TextBox textBox11;
		private TextBox textBox12;
		private TextBox textBox13;
		private TextBox textBox15;
		private TextBox textBox16;
		private TextBox textBox17;
		private TextBox textBox18;
		private TextBox textBox19;
		private TextBox textBox20;
		private Button textBoxLinesButton;
		private Button textBoxTextButton;
		private Button textBoxSelectedTextButton;

		// Tab4 RadioButtons Test
		private RadioButton radioButton;
		private RadioButton radioButton2;
		private RadioButton radioButton3;
		private RadioButton radioButton4;
		private RadioButton radioButton5;
		private RadioButton radioButton6;
		private RadioButton radioButton7;
		private RadioButton radioButton8;
		private RadioButton radioButton9;
		private RadioButton radioButton10;
		private RadioButton radioButton11;
		private RadioButton radioButton12;
		private RadioButton radioButton13;
		private RadioButton radioButton14;
		private RadioButton radioButton15;
		private RadioButton radioButton16;
		private RadioButton radioButton17;
		private RadioButton radioButton18;
		private RadioButton radioButton19;
		private RadioButton radioButton20;
		private RadioButton radioButton21;
		private RadioButton radioButton22;
		private RadioButton radioButton23;
		private RadioButton radioButton24;
		private RadioButton radioButton25;
		private RadioButton radioButton26;
		private RadioButton radioButton27;
		private RadioButton radioButton28;
		private RadioButton radioButton29;
		private RadioButton radioButton30;
		private RadioButton radioButton31;
		private RadioButton radioButton32;
		private RadioButton radioButton33;

		//Tab6 TabTest
		private TabPage tabPageT1;
		private TabPage tabPageT2;
		private TabPage tabPageT3;
		private TabPage tabPageT4;
		private TabPage tabPageT5;
		private TabControl tabControlT2;
		private TabPage tabPageT6;
		private TabControl tabControlT3;
		private TabPage tabPageT7;
		private TabPage tabPageT8;
		private Button buttonT1;
		private Label labelT1;
		private Label labelT2;
		private TabPage tabPageT9;
		private TabPage tabPageT10;
		private TabControl tabControlT4;
		private TabPage tabPageT11;
		private TabPage tabPageT12;
		private TabPage tabPageT13;
		private TabPage tabPageT14;
		private TabPage tabPageT15;
		private TabPage tabPageT16;
		private TabPage tabPageT17;
		private TabPage tabPageT18;
		private TabPage tabPageT19;
		private TabPage tabPageT20;
		private TabPage tabPageT21;
		private TabPage tabPageT22;
		private TabPage tabPageT23;
		private TabPage tabPageT24;
		private TabPage tabPageT25;
		private TabControl tabControlT5;
		private TabPage tabPageT26;
		private TabPage tabPageT27;
		private TabPage tabPageT28;
		private TabControl tabControlT6;
		private TabPage tabPageT29;
		private TabPage tabPageT30;
		private TabPage tabPageT31;
		private TabControl tabControlT7;
		private TabPage tabPageT32;
		private TabPage tabPageT33;
		private TabControl Docked;
		private TabPage tabPageT34;
		private TabControl tabControlT1;
		private TabPage tabPageT35;
		private TabPage tabPageT36;
		private TabControl tabControlT8;
		private TabPage tabPageT37;
		private TabPage tabPageT38;
		private Label labelT3;
		private Label labelT4;

		private MainMenu mainMenu;
		private MenuItem fileMenuItem;
		private MenuItem editMenuItem;
		private MenuItem helpMenuItem;
		private MenuItem newMenuItem;
		private MenuItem openMenuItem;
		private MenuItem exitMenuItem;
		private MenuItem thisMenuItem, thatMenuItem, otherMenuItem, otherAMenuItem, otherBMenuItem, otherCMenuItem, cutMenuItem, copyMenuItem, pasteMenuItem, aboutMenuItem, seperatorMenuItem;
		private ContextMenu contextMenu;
		private Label contextMenuLabel1;

		private Image image1;

		private ComboBox comboBox1, comboBox2;

		private TreeView treeView1;

		private ListBox listBox1, listBox2;

		private Button formsButton1;

		private VScrollBar vScrollBar;
		private HScrollBar hScrollBar;
		private Label scroll1LabelMin, scroll1LabelMax, scroll1LabelValue, scroll1LabelLarge, scroll1LabelSmall;
		private Label scroll2LabelMin, scroll2LabelMax, scroll2LabelValue, scroll2LabelLarge, scroll2LabelSmall;
		private TextBox scroll1TextBoxMin, scroll1TextBoxMax, scroll1TextBoxValue, scroll1TextBoxLarge, scroll1TextBoxSmall;
		private TextBox scroll2TextBoxMin, scroll2TextBoxMax, scroll2TextBoxValue, scroll2TextBoxLarge, scroll2TextBoxSmall;

		#endregion
		
		public static void Main(String[] args)
		{
			FormsTest form = new FormsTest();
			Application.Run(form);
		}

		public FormsTest()
		{
			ClientSize = new Size(500, 650);
			Text = "System.Windows.Forms Tests";
			
			SuspendLayout();
			tabControl1 = new TabControl();
			tabControl1.Dock = DockStyle.Fill;
			tabControl1.Multiline = true;
			tabControl1.SizeMode = TabSizeMode.FillToRight;
			
			tabPage18 = new TabPage();
			tabPage18.Text = "Scrollbar";
			tabControl1.Controls.Add(tabPage18);
			tabPage15 = new TabPage();
			tabPage15.Text = "Transform";
			tabControl1.Controls.Add(tabPage15);
			tabPage13 = new TabPage();
			tabPage13.Text = "ListBox";
			tabControl1.Controls.Add(tabPage13);
			tabPage12 = new TabPage();
			tabPage12.Text = "TreeView";
			tabControl1.Controls.Add(tabPage12);
			tabPage11 = new TabPage();
			tabPage11.Text = "ComboBox";
			tabControl1.Controls.Add(tabPage11);
			tabPage1 = new TabPage();
			tabPage1.Text = "Label";
			tabControl1.Controls.Add(tabPage1);
			tabControl1.SelectedIndex = 0;
			tabPage2 = new TabPage();
			tabPage2.Text = "Button";
			tabControl1.Controls.Add(tabPage2);
			tabPage3 = new TabPage();
			tabPage3.Text = "TextBox";
			tabControl1.Controls.Add(tabPage3);
			tabPage4 = new TabPage();
			tabPage4.Text = "RadioButton";
			tabControl1.Controls.Add(tabPage4);
			tabPage5 = new TabPage();
			tabPage5.Text = "Region";
			tabControl1.Controls.Add(tabPage5);
			tabPage6 = new TabPage();
			tabPage6.Text = "TabControl";
			tabControl1.Controls.Add(tabPage6);
			tabPage7 = new TabPage();
			tabPage7.Text = "Primitives";
			tabControl1.Controls.Add(tabPage7);
			tabPage14 = new TabPage();
			tabPage14.Text = "Form";
			tabControl1.Controls.Add(tabPage14);
			tabPage8 = new TabPage();
			tabPage8.Text = "Graphics";
			tabControl1.Controls.Add(tabPage8);
			tabPage16 = new TabPage();
			tabPage16.Text = "GraphicsPath";
			tabControl1.Controls.Add(tabPage16);
			tabPage17 = new TabPage();
			tabPage17.Text = "GraphicsDrawString";
			tabControl1.Controls.Add(tabPage17);
			tabPage9 = new TabPage();
			tabPage9.Text = "ContextMenu";
			tabControl1.Controls.Add(tabPage9);
			tabPage10 = new TabPage();
			tabPage10.Text = "Image";
			tabControl1.Controls.Add(tabPage10);
			
			Controls.Add(tabControl1);

			AddLabelTest(tabPage1);
			AddButtonTest(tabPage2);
			AddTextBoxTest(tabPage3);
			AddRadioButtonsTest(tabPage4);
			AddRegionsTest(tabPage5);
			AddTabControlsTest(tabPage6);
			AddPrimitivesTest(tabPage7);
			AddMenuTest();
			AddGraphicsTest(tabPage8);
			AddGraphicsPathTest(tabPage16);
			AddGraphicsDrawStringTest(tabPage17);
			AddContextTest(tabPage9);
			AddImageTest(tabPage10);
			AddComboTest(tabPage11);
			AddTreeViewTest(tabPage12);
			AddListBoxTest(tabPage13);
			AddFormsTest(tabPage14);
			AddTransformsTest(tabPage15);
			AddScrollbarTest(tabPage18);

			ResumeLayout(false);
		}

		private void AddLabelTest(Control control)
		{
			label22 = new Label();
			label23 = new Label();
			label8 = new Label();
			label9 = new Label();
			label4 = new Label();
			label5 = new Label();
			label6 = new Label();
			label7 = new Label();
			label2 = new Label();
			label3 = new Label();
			label15 = new Label();
			label14 = new Label();
			label17 = new Label();
			label16 = new Label();
			label11 = new Label();
			label10 = new Label();
			label13 = new Label();
			label20 = new Label();
			label21 = new Label();
			label24 = new Label();
			label25 = new Label();
			label19 = new Label();
			label18 = new Label();
			label12 = new Label();
			label = new Label();

			label22.Dock = DockStyle.Top;
			label22.Location = new Point(0, 0);
			label22.Name = "label22";
			label22.Size = new Size(480, 16);
			label22.TabIndex = 21;
			label22.Text = "A normal label : Dock=Top";

			label23.Dock = DockStyle.Bottom;
			label23.Location = new Point(0, 517);
			label23.Name = "label23";
			label23.Size = new Size(480, 16);
			label23.TabIndex = 22;
			label23.Text = "A normal label : Dock=Bottom";

			label8.Location = new Point(10, 125);
			label8.Name = "label8";
			label8.Size = new Size(464, 16);
			label8.TabIndex = 7;
			label8.Text = "A normal label : TextAlign=MiddleLeft";
			label8.TextAlign = ContentAlignment.MiddleLeft;

			label9.Location = new Point(10, 185);
			label9.Name = "label9";
			label9.Size = new Size(464, 16);
			label9.TabIndex = 8;
			label9.Text = "A normal label : TextAlign=BottomLeft";
			label9.TextAlign = ContentAlignment.BottomLeft;

			label4.Location = new Point(10, 85);
			label4.Name = "label4";
			label4.Size = new Size(464, 16);
			label4.TabIndex = 3;
			label4.Text = "A normal label : TextAlign=TopCenter";
			label4.TextAlign = ContentAlignment.TopCenter;

			label5.Location = new Point(10, 145);
			label5.Name = "label5";
			label5.Size = new Size(464, 16);
			label5.TabIndex = 4;
			label5.Text = "A normal label : TextAlign=MiddleCenter";
			label5.TextAlign = ContentAlignment.MiddleCenter;

			label6.Location = new Point(10, 165);
			label6.Name = "label6";
			label6.Size = new Size(464, 16);
			label6.TabIndex = 5;
			label6.Text = "A normal label : TextAlign=MiddleRight";
			label6.TextAlign = ContentAlignment.MiddleRight;

			label7.Location = new Point(10, 105);
			label7.Name = "label7";
			label7.Size = new Size(464, 16);
			label7.TabIndex = 6;
			label7.Text = "A normal label : TextAlign=TopRight";
			label7.TextAlign = ContentAlignment.TopRight;

			label2.BorderStyle = BorderStyle.FixedSingle;
			label2.Location = new Point(10, 45);
			label2.Name = "label2";
			label2.Size = new Size(464, 16);
			label2.TabIndex = 1;
			label2.Text = "A normal label : BorderStyle=FixedSingle";

			label3.BorderStyle = BorderStyle.Fixed3D;
			label3.Location = new Point(10, 65);
			label3.Name = "label3";
			label3.Size = new Size(464, 16);
			label3.TabIndex = 2;
			label3.Text = "A normal label : BorderStyle=Fixed3D";

			label15.Font = new Font("Microsoft Sans Serif", 8.25F, FontStyle.Italic, GraphicsUnit.Point, ((System.Byte)(0)));
			label15.Location = new Point(10, 305);
			label15.Name = "label15";
			label15.Size = new Size(464, 16);
			label15.TabIndex = 14;
			label15.Text = "A normal label : Font.Italic=true";

			label14.Font = new Font("Microsoft Sans Serif", 8.25F, FontStyle.Bold, GraphicsUnit.Point, ((System.Byte)(0)));
			label14.Location = new Point(10, 285);
			label14.Name = "label14";
			label14.Size = new Size(464, 16);
			label14.TabIndex = 13;
			label14.Text = "A normal label : Font.Bold=true";


			label17.Font = new Font("Microsoft Sans Serif", 8.25F, FontStyle.Underline, GraphicsUnit.Point, ((System.Byte)(0)));
			label17.Location = new Point(10, 345);
			label17.Name = "label17";
			label17.Size = new Size(464, 16);
			label17.TabIndex = 16;
			label17.Text = "A normal label : Font.Underline=true";

			label16.Font = new Font("Microsoft Sans Serif", 8.25F, FontStyle.Strikeout, GraphicsUnit.Point, ((System.Byte)(0)));
			label16.Location = new Point(10, 325);
			label16.Name = "label16";
			label16.Size = new Size(464, 16);
			label16.TabIndex = 15;
			label16.Text = "A normal label : Font.Strikeout=true";

			label11.Location = new Point(10, 225);
			label11.Name = "label11";
			label11.Size = new Size(464, 16);
			label11.TabIndex = 10;
			label11.Text = "A normal label : TextAlign=BottomRight";
			label11.TextAlign = ContentAlignment.BottomRight;

			label10.Location = new Point(10, 205);
			label10.Name = "label10";
			label10.Size = new Size(464, 16);
			label10.TabIndex = 9;
			label10.Text = "A normal label : TextAlign=BottomCenter";
			label10.TextAlign = ContentAlignment.BottomCenter;

			label13.ForeColor = Color.Red;
			label13.Location = new Point(10, 265);
			label13.Name = "label13";
			label13.Size = new Size(464, 16);
			label13.TabIndex = 12;
			label13.Text = "A normal label : ForeColor=Red";

			label20.BackColor = SystemColors.Control;
			label20.Enabled = false;
			label20.Location = new Point(10, 405);
			label20.Name = "label20";
			label20.Size = new Size(464, 16);
			label20.TabIndex = 19;
			label20.Text = "A normal label : Enabled=false";

			label21.Anchor = (AnchorStyles)AnchorStyles.Top | AnchorStyles.Bottom | AnchorStyles.Left | AnchorStyles.Right;
			label21.BackColor = SystemColors.Control;
			label21.Location = new Point(10, 495);
			label21.Name = "label21";
			label21.Size = new Size(460, 15);
			label21.TabIndex = 20;
			label21.Text = "A normal label : Anchor=All";

			label24.BorderStyle = BorderStyle.FixedSingle;
			label24.Location = new Point(10, 425);
			label24.Name = "label24";
			label24.Size = new Size(464, 40);
			label24.TabIndex = 23;
			label24.Text = "A normal label : BorderStyle=FixedSingle , Height=40";

			label25.AutoSize = true;
			label25.BorderStyle = BorderStyle.FixedSingle;
			label25.Location = new Point(10, 470);
			label25.Name = "label25";
			label25.Size = new Size(294, 19);
			label25.TabIndex = 24;
			label25.Text = "A normal label : BorderStyle=FixedSingle , AutoSize=true";

			label19.BackColor = Color.Red;
			label19.Location = new Point(10, 385);
			label19.Name = "label19";
			label19.Size = new Size(464, 16);
			label19.TabIndex = 18;
			label19.Text = "A normal label : BackColor=Red";

			label18.Font = new Font("Verdana", 9.75F, FontStyle.Regular, GraphicsUnit.Point, ((System.Byte)(0)));
			label18.Location = new Point(10, 365);
			label18.Name = "label18";
			label18.Size = new Size(464, 16);
			label18.TabIndex = 17;
			label18.Text = "A normal label : Font=Verdana; 9.75pt";

			label12.Location = new Point(10, 245);
			label12.Name = "label12";
			label12.Size = new Size(464, 16);
			label12.TabIndex = 11;
			label12.Text = "A normal label : With a &shortcut letter (Alt+S)";

			label.Location = new Point(10, 25);
			label.Name = "label";
			label.Size = new Size(464, 16);
			label.TabIndex = 0;
			label.Text = "A normal label";

			control.Controls.AddRange(new Control[] {
																			 label25,
																			 label24,
																			 label23,
																			 label22,
																			 label21,
																			 label20,
																			 label19,
																			 label18,
																			 label17,
																			 label16,
																			 label15,
																			 label14,
																			 label13,
																			 label12,
																			 label11,
																			 label10,
																			 label9,
																			 label8,
																			 label7,
																			 label6,
																			 label5,
																			 label4,
																			 label3,
																			 label2,
																			 label});
		}

		private void AddButtonTest(Control control)
		{
			button = new Button();
			button.Dock = DockStyle.Top;
			button.Location = new Point(0, 0);
			button.Size = new Size(496, 24);
			button.TabIndex = 0;
			button.Text = "A normal button : Dock=Top";

			button2 = new Button();
			button2.Location = new Point(8, 32);
			button2.Size = new Size(480, 24);
			button2.TabIndex = 1;
			button2.Text = "A normal button";

			button3 = new Button();
			button3.Dock = DockStyle.Bottom;
			button3.Location = new Point(0, 32);
			button3.Size = new Size(496, 24);
			button3.TabIndex = 2;
			button3.Text = "A normal button : Dock=Bottom";
			
			button4 = new Button();
			button4.Enabled = false;
			button4.Location = new Point(8, 64);
			button4.Size = new Size(480, 24);
			button4.TabIndex = 3;
			button4.Text = "A normal button : Enabled=false";

			button5 = new Button();
			button5.BackColor = Color.Red;
			button5.Location = new Point(8, 96);
			button5.Size = new Size(480, 24);
			button5.TabIndex = 4;
			button5.Text = "A normal button : BackColor=Red";

			button6 = new Button();
			button6.FlatStyle = FlatStyle.Flat;
			button6.Location = new Point(8, 128);
			button6.Size = new Size(480, 24);
			button6.TabIndex = 5;
			button6.Text = "A normal button : FlatStyle=Flat";

			button7 = new Button();
			button7.FlatStyle = FlatStyle.Popup;
			button7.Location = new Point(8, 160);
			button7.Size = new Size(480, 24);
			button7.TabIndex = 6;
			button7.Text = "A normal button : FlatStyle=Popup";
			
			button12 = new Button();
			button12.Font = new Font("Microsoft Sans Serif", 10F, FontStyle.Regular, GraphicsUnit.Point, ((System.Byte)(0)));
			button12.Location = new Point(8, 192);
			button12.Size = new Size(480, 24);
			button12.TabIndex = 11;
			button12.Text = "A normal button : Font.Size=10";

			button13 = new Button();
			button13.Font = new Font("Verdana", 8.25F, FontStyle.Regular, GraphicsUnit.Point, ((System.Byte)(0)));
			button13.Location = new Point(8, 224);
			button13.Size = new Size(480, 24);
			button13.TabIndex = 12;
			button13.Text = "A normal button : Font=Verdana; 8,25pt";
		
			button14 = new Button();
			button14.ForeColor = Color.Red;
			button14.Location = new Point(8, 256);
			button14.Size = new Size(480, 24);
			button14.TabIndex = 13;
			button14.Text = "A normal button : ForeColor=Red";

			button15 = new Button();
			button15.Location = new Point(8, 288);
			button15.Size = new Size(480, 32);
			button15.TabIndex = 14;
			button15.Text = "A normal button : TextAlign=TopLeft";
			button15.TextAlign = ContentAlignment.TopLeft;
			
			button16 = new Button();
			button16.Location = new Point(8, 320);
			button16.Size = new Size(480, 32);
			button16.TabIndex = 15;
			button16.Text = "A normal button : TextAlign=TopCenter";
			button16.TextAlign = ContentAlignment.TopCenter;

			button17 = new Button();
			button17.Location = new Point(8, 352);
			button17.Size = new Size(480, 32);
			button17.TabIndex = 16;
			button17.Text = "A normal button : TextAlign=TopRight";
			button17.TextAlign = ContentAlignment.TopRight;

			button18 = new Button();
			button18.Location = new Point(8, 384);
			button18.Size = new Size(480, 32);
			button18.TabIndex = 17;
			button18.Text = "A normal button : TextAlign=MiddleLeft";
			button18.TextAlign =ContentAlignment.MiddleLeft;

			button19 = new Button();
			button19.Location = new Point(8, 418);
			button19.Size = new Size(480, 32);
			button19.TabIndex = 18;
			button19.Text = "A normal button : TextAlign=MiddleRight";
			button19.TextAlign = ContentAlignment.MiddleRight;

			button20 = new Button();
			button20.Location = new Point(8, 450);
			button20.Size = new Size(480, 32);
			button20.TabIndex = 19;
			button20.Text = "A normal button : TextAlign=BottomLeft";
			button20.TextAlign = ContentAlignment.BottomLeft;

			button21 = new Button();
			button21.Location = new Point(8, 482);
			button21.Size = new Size(480, 32);
			button21.TabIndex = 20;
			button21.Text = "A normal button : TextAlign=BottomCenter";
			button21.TextAlign = ContentAlignment.BottomCenter;
	
			button22 = new Button();
			button22.Location = new Point(8, 514);
			button22.Size = new Size(480, 32);
			button22.TabIndex = 21;
			button22.Text = "A normal button : TextAlign=BottomRight";
			button22.TextAlign = ContentAlignment.BottomRight;

			button23 = new Button();
			button23.Anchor = (AnchorStyles)AnchorStyles.Top | AnchorStyles.Bottom | AnchorStyles.Left | AnchorStyles.Right;
			button23.Location = new Point(8, 546);
			button23.Name = "button23";
			button23.Size = new Size(480, 24);
			button23.TabIndex = 22;
			button23.Text = "A normal button : Anchor=All";

			control.Controls.AddRange(new Control[] { button, button2, button3, button4, button5, button6, button7, button12, button13, button14, button15, button16, button17, button18, button19, button20, button21, button22, button23});
		}
	
		private void AddTextBoxTest(Control control)
		{
			textBox19 = new TextBox();
			textBox3 = new TextBox();
			textBox20 = new TextBox();
			textBox18 = new TextBox();
			textBox2 = new TextBox();
			textBox1 = new TextBox();
			textBox6 = new TextBox();
			textBox7 = new TextBox();
			textBox4 = new TextBox();
			textBox5 = new TextBox();
			textBox10 = new TextBox();
			textBox8 = new TextBox();
			textBox9 = new TextBox();
			textBox13 = new TextBox();
			textBox12 = new TextBox();
			textBox11 = new TextBox();
			textBox = new TextBox();
			textBox17 = new TextBox();
			textBox16 = new TextBox();
			textBox15 = new TextBox();
			textBox14 = new TextBox();
			textBoxLinesButton = new Button();
			textBoxTextButton = new Button();
			textBoxSelectedTextButton = new Button();

			textBox19.Dock = DockStyle.Top;
			textBox19.Location = new Point(0, 0);
			textBox19.Name = "textBox19";
			textBox19.Size = new Size(450, 20);
			textBox19.TabIndex = 19;
			textBox19.Text = "A normal textbox - MS Sans Serif : Dock=Top";

			textBox3.BorderStyle = BorderStyle.FixedSingle;
			textBox3.Location = new Point(8, 95);
			textBox3.Name = "textBox3";
			textBox3.Size = new Size(450, 20);
			textBox3.TabIndex = 3;
			textBox3.Text = "A normal textbox - MS Sans Serif : BorderStyle=FixedSingle";

			textBox20.Enabled = false;
			textBox20.Location = new Point(10, 390);
			textBox20.Name = "textBox20";
			textBox20.Size = new Size(450, 20);
			textBox20.TabIndex = 20;
			textBox20.Text = "A normal textbox - MS Sans Serif : Enabled=false";

			textBox18.Dock = DockStyle.Bottom;
			textBox18.Location = new Point(0, 518);
			textBox18.Name = "textBox18";
			textBox18.Size = new Size(450, 20);
			textBox18.TabIndex = 18;
			textBox18.Text = "A normal textbox - MS Sans Serif : Dock=Bottom";

			textBox2.Location = new Point(8, 70);
			textBox2.Name = "textBox2";
			textBox2.Size = new Size(450, 20);
			textBox2.TabIndex = 2;
			textBox2.Text = "A normal textbox - MS Sans Serif : TextAlign=Center";
			textBox2.TextAlign = HorizontalAlignment.Center;

			textBox1.Location = new Point(8, 45);
			textBox1.Name = "textBox1";
			textBox1.Size = new Size(450, 20);
			textBox1.TabIndex = 0;
			textBox1.Text = "A normal textbox - MS Sans Serif : TextAlign=Right";
			textBox1.TextAlign = HorizontalAlignment.Right;

			textBox6.Font = new Font("Serifa BT", 8.25F, FontStyle.Regular, GraphicsUnit.Point, ((System.Byte)(0)));
			textBox6.Location = new Point(10, 190);
			textBox6.Name = "textBox6";
			textBox6.Size = new Size(450, 20);
			textBox6.TabIndex = 6;
			textBox6.Text = "A normal textbox - Serifa BT";

			textBox7.Font = new Font("Microsoft Sans Serif", 8.25F, FontStyle.Bold, GraphicsUnit.Point, ((System.Byte)(0)));
			textBox7.Location = new Point(8, 215);
			textBox7.Name = "textBox7";
			textBox7.Size = new Size(450, 20);
			textBox7.TabIndex = 7;
			textBox7.Text = "A normal textbox - MS Sans Serif : Font.Bold=true";

			textBox4.Location = new Point(8, 120);
			textBox4.Multiline = true;
			textBox4.Name = "textBox4";
			textBox4.ScrollBars = ScrollBars.Both;
			textBox4.Size = new Size(450, 40);
			textBox4.TabIndex = 4;
			textBox4.Text = "A MultiLine textbox - MS Sans Serif : ScrollBars=Both, Height=40\nOh give me a home\nWhere the buffalo roam\nAnd the deer and 45 (forty-five) antelope play\nWhere seldom is heard\nA discouraging word\nAnd the skies are not cloudy all day.\nHome, Home on the Range\nWhere the deer and the antelope play.\nWhere seldom is heard\nA discouraging word\nAnd the skies are not cloudy all day....";

			textBox5.Font = new Font("Arial", 9F, FontStyle.Regular, GraphicsUnit.Point, ((System.Byte)(0)));
			textBox5.Location = new Point(10, 165);
			textBox5.Name = "textBox5";
			textBox5.Size = new Size(450, 21);
			textBox5.TabIndex = 5;
			textBox5.Text = "A normal textbox - Arial; 9pt";

			textBox10.Font = new Font("Microsoft Sans Serif", 8.25F, FontStyle.Underline);
			textBox10.Location = new Point(10, 290);
			textBox10.Name = "textBox10";
			textBox10.Size = new Size(450, 20);
			textBox10.TabIndex = 10;
			textBox10.Text = "A normal textbox - MS Sans Serif : Font.Underline=true";

			textBox8.Font = new Font("Microsoft Sans Serif", 8.25F, FontStyle.Italic);
			textBox8.Location = new Point(10, 240);
			textBox8.Name = "textBox8";
			textBox8.Size = new Size(450, 20);
			textBox8.TabIndex = 8;
			textBox8.Text = "A normal textbox - MS Sans Serif : Font.Italic=true";

			textBox9.Font = new Font("Microsoft Sans Serif", 8.25F, FontStyle.Strikeout);
			textBox9.Location = new Point(10, 265);
			textBox9.Name = "textBox9";
			textBox9.Size = new Size(450, 20);
			textBox9.TabIndex = 9;
			textBox9.Text = "A normal textbox - MS Sans Serif : Font.StrikeOut=true";

			textBox13.Font = new Font("Microsoft Sans Serif", 8.25F);
			textBox13.Location = new Point(10, 365);
			textBox13.Name = "textBox13";
			textBox13.ReadOnly = true;
			textBox13.Size = new Size(450, 20);
			textBox13.TabIndex = 13;
			textBox13.Text = "A normal textbox - MS Sans Serif : ReadOnly=true";

			textBox12.Font = new Font("Microsoft Sans Serif", 8.25F);
			textBox12.Location = new Point(10, 340);
			textBox12.Name = "textBox12";
			textBox12.PasswordChar = '*';
			textBox12.Size = new Size(450, 20);
			textBox12.TabIndex = 12;
			textBox12.Text = "A normal textbox - MS Sans Serif : PasswordChar=*";

			textBox11.Font = new Font("Microsoft Sans Serif", 8.25F);
			textBox11.ForeColor = Color.Red;
			textBox11.Location = new Point(10, 315);
			textBox11.Name = "textBox11";
			textBox11.Size = new Size(450, 20);
			textBox11.TabIndex = 11;
			textBox11.Text = "A normal textbox - MS Sans Serif : ForeColor=red";

			textBox.Location = new Point(8, 25);
			textBox.Name = "textBox";
			textBox.Size = new Size(450, 20);
			textBox.TabIndex = 1;
			textBox.Text = "A normal textbox - MS Sans Serif";

			textBox17.AcceptsTab = true;
			textBox17.Location = new Point(10, 490);
			textBox17.Name = "textBox17";
			textBox17.Size = new Size(450, 20);
			textBox17.TabIndex = 17;
			textBox17.Text = "A normal textbox - MS Sans Serif : AllowTab=true";

			textBox16.Anchor = ((AnchorStyles)((((AnchorStyles.Top | AnchorStyles.Bottom)
				| AnchorStyles.Left)
				| AnchorStyles.Right)));
			textBox16.Location = new Point(10, 465);
			textBox16.Name = "textBox16";
			textBox16.Size = new Size(450, 20);
			textBox16.TabIndex = 16;
			textBox16.Text = "A normal textbox - MS Sans Serif : Anchor=All";

			textBox15.BackColor = Color.Red;
			textBox15.Location = new Point(10, 440);
			textBox15.Name = "textBox15";
			textBox15.Size = new Size(450, 20);
			textBox15.TabIndex = 15;
			textBox15.Text = "A normal textbox - MS Sans Serif : BackColor=Red";

			textBox14.Font = new Font("Microsoft Sans Serif", 8.25F);
			textBox14.Location = new Point(10, 415);
			textBox14.MaxLength = 50;
			textBox14.Name = "textBox14";
			textBox14.Size = new Size(450, 20);
			textBox14.TabIndex = 14;
			textBox14.Text = "A normal textbox - MS Sans Serif : MaxLength=50";

			textBoxLinesButton.Bounds = new Rectangle( 10, 510, 150, 30);
			textBoxLinesButton.Text = "Lines[] for Multiline";
			textBoxLinesButton.Click+=new EventHandler(textBoxLinesButton_Click);

			textBoxTextButton.Bounds = new Rectangle( 180, 510, 70, 30);
			textBoxTextButton.Text = "Set Text";
			textBoxTextButton.Click+=new EventHandler(textBoxTextButton_Click);

			textBoxSelectedTextButton.Bounds = new Rectangle( 250, 510, 150, 30);
			textBoxSelectedTextButton.Text = "Set Selected Text";
			textBoxSelectedTextButton.Click+=new EventHandler(textBoxSelectedTextButton_Click);

			control.Controls.AddRange(new Control[] {
																			textBox19,
																			textBox20,
																			textBox18,
																			textBox17,
																			textBox16,
																			textBox15,
																			textBox14,
																			textBox13,
																			textBox12,
																			textBox11,
																			textBox10,
																			textBox9,
																			textBox8,
																			textBox7,
																			textBox6,
																			textBox5,
																			textBox4,
																			textBox3,
																			textBox2,
																			textBox,
																			textBox1,
																			textBoxLinesButton,
																			textBoxTextButton,
																			textBoxSelectedTextButton
																		 });
		}

		private void textBoxLinesButton_Click(object sender, EventArgs e)
		{
			foreach(String s in textBox4.Lines)
				Console.WriteLine(s);
		}

		private void textBoxTextButton_Click(object sender, EventArgs e)
		{
			textBox4.Text = "1234 123456 123";
		}
		private void textBoxSelectedTextButton_Click(object sender, EventArgs e)
		{
			textBox4.SelectedText = "aaaa bbbbb cccc";
		}

		private void AddRadioButtonsTest(Control control)
		{
			radioButton18 = new RadioButton();
			radioButton19 = new RadioButton();
			radioButton14 = new RadioButton();
			radioButton15 = new RadioButton();
			radioButton16 = new RadioButton();
			radioButton17 = new RadioButton();
			radioButton10 = new RadioButton();
			radioButton11 = new RadioButton();
			radioButton12 = new RadioButton();
			radioButton13 = new RadioButton();
			radioButton29 = new RadioButton();
			radioButton28 = new RadioButton();
			radioButton27 = new RadioButton();
			radioButton24 = new RadioButton();
			radioButton23 = new RadioButton();
			radioButton32 = new RadioButton();
			radioButton9 = new RadioButton();
			radioButton8 = new RadioButton();
			radioButton5 = new RadioButton();
			radioButton4 = new RadioButton();
			radioButton7 = new RadioButton();
			radioButton6 = new RadioButton();
			radioButton26 = new RadioButton();
			radioButton25 = new RadioButton();
			radioButton3 = new RadioButton();
			radioButton2 = new RadioButton();
			radioButton22 = new RadioButton();
			radioButton21 = new RadioButton();
			radioButton20 = new RadioButton();
			radioButton = new RadioButton();
			radioButton33 = new RadioButton();
			radioButton30 = new RadioButton();
			radioButton31 = new RadioButton();

			radioButton18.Location = new Point(8, 368);
			radioButton18.Name = "radioButton18";
			radioButton18.RightToLeft = RightToLeft.Yes;
			radioButton18.Size = new Size(288, 24);
			radioButton18.TabIndex = 17;
			radioButton18.Text = "RightToLeft=True";

			radioButton19.Font = new Font("Microsoft Sans Serif", 8.25F, FontStyle.Bold, GraphicsUnit.Point, ((System.Byte)(0)));
			radioButton19.Location = new Point(240, 352);
			radioButton19.Name = "radioButton19";
			radioButton19.Size = new Size(200, 24);
			radioButton19.TabIndex = 18;
			radioButton19.Text = "Font.Bold=True";

			radioButton14.CheckAlign = ContentAlignment.TopLeft;
			radioButton14.Location = new Point(8, 128);
			radioButton14.Name = "radioButton14";
			radioButton14.Size = new Size(200, 24);
			radioButton14.TabIndex = 13;
			radioButton14.Text = " CheckAlign=TopLeft";

			radioButton15.Dock = DockStyle.Top;
			radioButton15.Location = new Point(0, 0);
			radioButton15.Name = "radioButton15";
			radioButton15.Size = new Size(608, 24);
			radioButton15.TabIndex = 14;
			radioButton15.Text = "Dock=Top";

			radioButton16.Font = new Font("Microsoft Sans Serif", 8.25F, FontStyle.Underline, GraphicsUnit.Point, ((System.Byte)(0)));
			radioButton16.Location = new Point(240, 448);
			radioButton16.Name = "radioButton16";
			radioButton16.Size = new Size(200, 24);
			radioButton16.TabIndex = 39;
			radioButton16.Text = "Font.Underline=True";

			radioButton17.FlatStyle = FlatStyle.Flat;
			radioButton17.Location = new Point(240, 200);
			radioButton17.Name = "radioButton17";
			radioButton17.Size = new Size(200, 24);
			radioButton17.TabIndex = 16;
			radioButton17.Text = "FlatStyle=Flat";

			radioButton10.BackColor = Color.SeaGreen;
			radioButton10.Location = new Point(8, 400);
			radioButton10.Name = "radioButton10";
			radioButton10.Size = new Size(200, 24);
			radioButton10.TabIndex = 9;
			radioButton10.Text = "BackColor=SeaGreen";

			radioButton11.Location = new Point(240, 32);
			radioButton11.Name = "radioButton11";
			radioButton11.Size = new Size(200, 24);
			radioButton11.TabIndex = 10;
			radioButton11.Text = "TextAlign=MiddleCenter";
			radioButton11.TextAlign = ContentAlignment.MiddleCenter;

			radioButton12.FlatStyle = FlatStyle.Popup;
			radioButton12.Location = new Point(240, 320);
			radioButton12.Name = "radioButton12";
			radioButton12.Size = new Size(200, 24);
			radioButton12.TabIndex = 37;
			radioButton12.Text = "FlatStyle=Popup";

			radioButton13.CheckAlign = ContentAlignment.BottomLeft;
			radioButton13.Location = new Point(8, 232);
			radioButton13.Name = "radioButton13";
			radioButton13.Size = new Size(200, 24);
			radioButton13.TabIndex = 12;
			radioButton13.Text = "CheckAlign=BottomLeft";

			radioButton29.Location = new Point(240, 256);
			radioButton29.Name = "radioButton29";
			radioButton29.Size = new Size(200, 24);
			radioButton29.TabIndex = 36;
			radioButton29.Text = "TextAlign=BottomRight";
			radioButton29.TextAlign = ContentAlignment.BottomRight;

			radioButton28.Location = new Point(240, 224);
			radioButton28.Name = "radioButton28";
			radioButton28.Size = new Size(200, 24);
			radioButton28.TabIndex = 35;
			radioButton28.Text = "TextAlign=BottomCenter";
			radioButton28.TextAlign = ContentAlignment.BottomCenter;

			radioButton27.Location = new Point(240, 192);
			radioButton27.Name = "radioButton27";
			radioButton27.Size = new Size(200, 24);
			radioButton27.TabIndex = 34;
			radioButton27.Text = "TextAlign=BottomLeft";
			radioButton27.TextAlign = ContentAlignment.BottomLeft;

			radioButton24.Location = new Point(240, 160);
			radioButton24.Name = "radioButton24";
			radioButton24.Size = new Size(200, 24);
			radioButton24.TabIndex = 33;
			radioButton24.Text = "TextAlign=TopRight";
			radioButton24.TextAlign = ContentAlignment.TopRight;

			radioButton23.CheckAlign = ContentAlignment.MiddleRight;
			radioButton23.Location = new Point(8, 96);
			radioButton23.Name = "radioButton23";
			radioButton23.Size = new Size(200, 24);
			radioButton23.TabIndex = 26;
			radioButton23.Text = "CheckAlign=MiddleRight";

			radioButton32.ForeColor = Color.Green;
			radioButton32.Location = new Point(8, 432);
			radioButton32.Name = "radioButton32";
			radioButton32.Size = new Size(200, 24);
			radioButton32.TabIndex = 42;
			radioButton32.Text = "ForeColor=Green";

			radioButton9.Font = new Font("Microsoft Sans Serif", 8.25F, FontStyle.Italic, GraphicsUnit.Point, ((System.Byte)(0)));
			radioButton9.Location = new Point(240, 384);
			radioButton9.Name = "radioButton9";
			radioButton9.Size = new Size(200, 24);
			radioButton9.TabIndex = 8;
			radioButton9.Text = "Font.Italic=True";

			radioButton8.Font = new Font("Microsoft Sans Serif", 8.25F, FontStyle.Strikeout, GraphicsUnit.Point, ((System.Byte)(0)));
			radioButton8.Location = new Point(240, 416);
			radioButton8.Name = "radioButton8";
			radioButton8.Size = new Size(200, 24);
			radioButton8.TabIndex = 38;
			radioButton8.Text = "Font.Strikeout=True";

			radioButton5.Dock = DockStyle.Bottom;
			radioButton5.Location = new Point(0, 541);
			radioButton5.Name = "radioButton5";
			radioButton5.Size = new Size(608, 24);
			radioButton5.TabIndex = 4;
			radioButton5.Text = "Dock = Bottom";

			radioButton4.CheckAlign = ContentAlignment.TopRight;
			radioButton4.Location = new Point(8, 200);
			radioButton4.Name = "radioButton4";
			radioButton4.Size = new Size(200, 24);
			radioButton4.TabIndex = 3;
			radioButton4.Text = "CheckAlign=TopRight";

			radioButton7.Checked = true;
			radioButton7.Location = new Point(8, 336);
			radioButton7.Name = "radioButton7";
			radioButton7.Size = new Size(200, 24);
			radioButton7.TabIndex = 6;
			radioButton7.TabStop = true;
			radioButton7.Text = "Checked=True";

			radioButton6.Location = new Point(8, 32);
			radioButton6.Name = "radioButton6";
			radioButton6.Size = new Size(200, 24);
			radioButton6.TabIndex = 22;
			radioButton6.Text = "A normal RadioButton";

			radioButton26.Location = new Point(240, 96);
			radioButton26.Name = "radioButton26";
			radioButton26.Size = new Size(200, 24);
			radioButton26.TabIndex = 31;
			radioButton26.Text = "TextAlign=TopLeft";
			radioButton26.TextAlign = ContentAlignment.TopLeft;

			radioButton25.Location = new Point(240, 128);
			radioButton25.Name = "radioButton25";
			radioButton25.Size = new Size(200, 24);
			radioButton25.TabIndex = 32;
			radioButton25.Text = "TextAlign=TopCenter";
			radioButton25.TextAlign = ContentAlignment.TopCenter;

			radioButton3.CheckAlign = ContentAlignment.BottomRight;
			radioButton3.Location = new Point(8, 304);
			radioButton3.Name = "radioButton3";
			radioButton3.Size = new Size(200, 24);
			radioButton3.TabIndex = 2;
			radioButton3.Text = "CheckAlign=BottomRight";

			radioButton2.CheckAlign = ContentAlignment.TopCenter;
			radioButton2.Location = new Point(8, 160);
			radioButton2.Name = "radioButton2";
			radioButton2.Size = new Size(200, 32);
			radioButton2.TabIndex = 1;
			radioButton2.Text = "CheckAlign=TopCenter";

			radioButton22.CheckAlign = ContentAlignment.MiddleCenter;
			radioButton22.Location = new Point(8, 64);
			radioButton22.Name = "radioButton22";
			radioButton22.Size = new Size(200, 24);
			radioButton22.TabIndex = 25;
			radioButton22.Text = "CheckAlign=MiddleCenter";

			radioButton21.Anchor = ((AnchorStyles)((((AnchorStyles.Top | AnchorStyles.Bottom)
				| AnchorStyles.Left)
				| AnchorStyles.Right)));
			radioButton21.Location = new Point(8, 496);
			radioButton21.Name = "radioButton21";
			radioButton21.Size = new Size(200, 24);
			radioButton21.TabIndex = 21;
			radioButton21.Text = "Anchor=All";

			radioButton20.CheckAlign = ContentAlignment.BottomCenter;
			radioButton20.Location = new Point(8, 264);
			radioButton20.Name = "radioButton20";
			radioButton20.Size = new Size(200, 32);
			radioButton20.TabIndex = 23;
			radioButton20.Text = "CheckAlign=BottomCenter";

			radioButton.Location = new Point(240, 64);
			radioButton.Name = "radioButton";
			radioButton.Size = new Size(200, 24);
			radioButton.TabIndex = 0;
			radioButton.Text = "TextAlign=MiddleRight";
			radioButton.TextAlign = ContentAlignment.MiddleRight;

			radioButton33.Appearance = Appearance.Button;
			radioButton33.Location = new Point(240, 512);
			radioButton33.Name = "radioButton33";
			radioButton33.Size = new Size(200, 24);
			radioButton33.TabIndex = 43;
			radioButton33.Text = "Appearance=Button";

			radioButton30.Font = new Font("Microsoft Sans Serif", 10F, FontStyle.Regular, GraphicsUnit.Point, ((System.Byte)(0)));
			radioButton30.Location = new Point(8, 464);
			radioButton30.Name = "radioButton30";
			radioButton30.Size = new Size(200, 24);
			radioButton30.TabIndex = 40;
			radioButton30.Text = "Font.Size=10";

			radioButton31.Font = new Font("Verdana", 8.25F, FontStyle.Regular, GraphicsUnit.Point, ((System.Byte)(0)));
			radioButton31.Location = new Point(240, 480);
			radioButton31.Name = "radioButton31";
			radioButton31.Size = new Size(200, 24);
			radioButton31.TabIndex = 41;
			radioButton31.Text = "Font=Verdana; 8,25pt";

			control.Controls.AddRange(new Control[] {
																			 radioButton33,
																			 radioButton32,
																			 radioButton31,
																			 radioButton30,
																			 radioButton16,
																			 radioButton8,
																			 radioButton12,
																			 radioButton29,
																			 radioButton28,
																			 radioButton27,
																			 radioButton24,
																			 radioButton25,
																			 radioButton26,
																			 radioButton23,
																			 radioButton22,
																			 radioButton20,
																			 radioButton6,
																			 radioButton21,
																			 radioButton19,
																			 radioButton18,
																			 radioButton17,
																			 radioButton15,
																			 radioButton14,
																			 radioButton13,
																			 radioButton11,
																			 radioButton10,
																			 radioButton9,
																			 radioButton7,
																			 radioButton5,
																			 radioButton4,
																			 radioButton3,
																			 radioButton2,
																			 radioButton});
		}
		private void AddRegionsTest(Control control)
		{
			control.Paint+=new PaintEventHandler(Regions_Paint);
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

		private void AddTabControlsTest(Control control)
		{
			Docked = new TabControl();
			tabPageT1 = new TabPage();
			tabControlT2 = new TabControl();
			tabPageT6 = new TabPage();
			labelT2 = new Label();
			labelT1 = new Label();
			buttonT1 = new Button();
			tabPageT22 = new TabPage();
			tabPageT23 = new TabPage();
			tabPageT21 = new TabPage();
			tabPageT9 = new TabPage();
			tabPageT24 = new TabPage();
			tabPageT25 = new TabPage();
			tabPageT10 = new TabPage();
			tabControlT8 = new TabControl();
			tabPageT37 = new TabPage();
			tabPageT38 = new TabPage();
			tabPageT20 = new TabPage();
			tabControlT5 = new TabControl();
			tabPageT26 = new TabPage();
			tabPageT27 = new TabPage();
			tabPageT28 = new TabPage();
			tabPageT2 = new TabPage();
			tabControlT3 = new TabControl();
			tabPageT7 = new TabPage();
			tabPageT8 = new TabPage();
			tabPageT3 = new TabPage();
			tabControlT4 = new TabControl();
			tabPageT11 = new TabPage();
			tabPageT12 = new TabPage();
			tabPageT13 = new TabPage();
			tabPageT14 = new TabPage();
			tabPageT15 = new TabPage();
			tabPageT16 = new TabPage();
			tabPageT17 = new TabPage();
			tabPageT18 = new TabPage();
			tabPageT4 = new TabPage();
			tabControlT6 = new TabControl();
			tabPageT29 = new TabPage();
			tabPageT30 = new TabPage();
			tabPageT31 = new TabPage();
			tabPageT5 = new TabPage();
			tabControlT7 = new TabControl();
			tabPageT32 = new TabPage();
			tabPageT33 = new TabPage();
			tabPageT19 = new TabPage();
			tabPageT34 = new TabPage();
			tabControlT1 = new TabControl();
			tabPageT35 = new TabPage();
			tabPageT36 = new TabPage();
			labelT3 = new Label();
			labelT4 = new Label();
			
			Docked.Controls.Add(tabPageT1);
			Docked.Controls.Add(tabPageT10);
			Docked.Controls.Add(tabPageT5);
			Docked.Controls.Add(tabPageT19);
			Docked.Controls.Add(tabPageT20);
			Docked.Controls.Add(tabPageT2);
			Docked.Controls.Add(tabPageT3);
			Docked.Controls.Add(tabPageT4);
			Docked.Controls.Add(tabPageT34);
			Docked.Location = new Point(24, 16);
			Docked.Name = "Docked";
			Docked.SelectedIndex = 0;
			Docked.Size = new Size(384, 320);
			Docked.TabIndex = 0;
			
			tabPageT1.Controls.Add(tabControlT2);
			tabPageT1.Location = new Point(4, 22);
			tabPageT1.Name = "tabPage1";
			tabPageT1.Size = new Size(376, 294);
			tabPageT1.TabIndex = 0;
			tabPageT1.Text = "FillToRight";
			
			tabControlT2.Controls.Add(tabPageT6);
			tabControlT2.Controls.Add(tabPageT22);
			tabControlT2.Controls.Add(tabPageT23);
			tabControlT2.Controls.Add(tabPageT21);
			tabControlT2.Controls.Add(tabPageT9);
			tabControlT2.Controls.Add(tabPageT24);
			tabControlT2.Controls.Add(tabPageT25);
			tabControlT2.Location = new Point(16, 24);
			tabControlT2.Multiline = true;
			tabControlT2.Name = "tabControl2";
			tabControlT2.SelectedIndex = 0;
			tabControlT2.Size = new Size(344, 248);
			tabControlT2.SizeMode = TabSizeMode.FillToRight;
			tabControlT2.TabIndex = 1;
			
			tabPageT6.Controls.Add(labelT2);
			tabPageT6.Controls.Add(labelT1);
			tabPageT6.Controls.Add(buttonT1);
			tabPageT6.Location = new Point(4, 40);
			tabPageT6.Name = "tabPage6";
			tabPageT6.Size = new Size(336, 204);
			tabPageT6.TabIndex = 0;
			tabPageT6.Text = "First";
			
			labelT2.Location = new Point(144, 160);
			labelT2.Name = "label2";
			labelT2.Size = new Size(112, 40);
			labelT2.TabIndex = 2;
			labelT2.Text = "label2";
			
			labelT1.Location = new Point(144, 88);
			labelT1.Name = "label1";
			labelT1.Size = new Size(112, 40);
			labelT1.TabIndex = 1;
			labelT1.Text = "label1";
			
			buttonT1.Location = new Point(24, 16);
			buttonT1.Name = "button1";
			buttonT1.Size = new Size(96, 40);
			buttonT1.TabIndex = 0;
			buttonT1.Text = "button1";
			
			tabPageT22.Location = new Point(4, 40);
			tabPageT22.Name = "tabPage22";
			tabPageT22.Size = new Size(336, 204);
			tabPageT22.TabIndex = 3;
			tabPageT22.Text = "tabPage22";
			
			tabPageT23.Location = new Point(4, 40);
			tabPageT23.Name = "tabPage23";
			tabPageT23.Size = new Size(336, 204);
			tabPageT23.TabIndex = 4;
			tabPageT23.Text = "tabPage23";
			
			tabPageT21.Location = new Point(4, 40);
			tabPageT21.Name = "tabPage21";
			tabPageT21.Size = new Size(336, 204);
			tabPageT21.TabIndex = 2;
			tabPageT21.Text = "tabPage21";
			
			tabPageT9.Location = new Point(4, 40);
			tabPageT9.Name = "tabPage9";
			tabPageT9.Size = new Size(336, 204);
			tabPageT9.TabIndex = 1;
			tabPageT9.Text = "Second";
			
			tabPageT24.Location = new Point(4, 40);
			tabPageT24.Name = "tabPage24";
			tabPageT24.Size = new Size(336, 204);
			tabPageT24.TabIndex = 5;
			tabPageT24.Text = "tabPage24";
			
			tabPageT25.Location = new Point(4, 40);
			tabPageT25.Name = "tabPage25";
			tabPageT25.Size = new Size(336, 204);
			tabPageT25.TabIndex = 6;
			tabPageT25.Text = "tabPage25";
			
			tabPageT10.Controls.Add(tabControlT8);
			tabPageT10.Location = new Point(4, 22);
			tabPageT10.Name = "tabPage10";
			tabPageT10.Size = new Size(376, 294);
			tabPageT10.TabIndex = 5;
			tabPageT10.Text = "OwnerDraw";
			
			tabControlT8.Controls.Add(tabPageT37);
			tabControlT8.Controls.Add(tabPageT38);
			tabControlT8.DrawMode = TabDrawMode.OwnerDrawFixed;
			tabControlT8.Location = new Point(32, 24);
			tabControlT8.Name = "tabControl8";
			tabControlT8.SelectedIndex = 0;
			tabControlT8.Size = new Size(328, 248);
			tabControlT8.TabIndex = 0;
			tabControlT8.DrawItem += new DrawItemEventHandler(tabControlT8_DrawItem);
			
			tabPageT37.Location = new Point(4, 22);
			tabPageT37.Name = "tabPage37";
			tabPageT37.Size = new Size(320, 222);
			tabPageT37.TabIndex = 0;
			tabPageT37.Text = "tabPage37";
			
			tabPageT38.Location = new Point(4, 22);
			tabPageT38.Name = "tabPage38";
			tabPageT38.Size = new Size(320, 222);
			tabPageT38.TabIndex = 1;
			tabPageT38.Text = "tabPage38";
			
			tabPageT20.Controls.Add(tabControlT5);
			tabPageT20.Location = new Point(4, 22);
			tabPageT20.Name = "tabPage20";
			tabPageT20.Size = new Size(376, 294);
			tabPageT20.TabIndex = 7;
			tabPageT20.Text = "FixedSize";
			
			tabControlT5.Controls.Add(tabPageT26);
			tabControlT5.Controls.Add(tabPageT27);
			tabControlT5.Controls.Add(tabPageT28);
			tabControlT5.ItemSize = new Size(100, 30);
			tabControlT5.Location = new Point(24, 32);
			tabControlT5.Name = "tabControl5";
			tabControlT5.SelectedIndex = 0;
			tabControlT5.Size = new Size(336, 256);
			tabControlT5.SizeMode = TabSizeMode.Fixed;
			tabControlT5.TabIndex = 0;
			
			tabPageT26.Location = new Point(4, 34);
			tabPageT26.Name = "tabPage26";
			tabPageT26.Size = new Size(328, 218);
			tabPageT26.TabIndex = 0;
			tabPageT26.Text = "a";
			
			tabPageT27.Location = new Point(4, 34);
			tabPageT27.Name = "tabPage27";
			tabPageT27.Size = new Size(328, 218);
			tabPageT27.TabIndex = 1;
			tabPageT27.Text = "second";
			
			tabPageT28.Location = new Point(4, 34);
			tabPageT28.Name = "tabPage28";
			tabPageT28.Size = new Size(328, 218);
			tabPageT28.TabIndex = 2;
			tabPageT28.Text = "3rd";
			
			tabPageT2.Controls.Add(tabControlT3);
			tabPageT2.Location = new Point(4, 22);
			tabPageT2.Name = "tabPage2";
			tabPageT2.Size = new Size(376, 294);
			tabPageT2.TabIndex = 1;
			tabPageT2.Text = "AlignBot";
			
			tabControlT3.Alignment = TabAlignment.Bottom;
			tabControlT3.Controls.Add(tabPageT7);
			tabControlT3.Controls.Add(tabPageT8);
			tabControlT3.Location = new Point(32, 32);
			tabControlT3.Name = "tabControl3";
			tabControlT3.SelectedIndex = 0;
			tabControlT3.Size = new Size(232, 208);
			tabControlT3.TabIndex = 1;
			
			tabPageT7.Location = new Point(4, 4);
			tabPageT7.Name = "tabPage7";
			tabPageT7.Size = new Size(224, 182);
			tabPageT7.TabIndex = 0;
			tabPageT7.Text = "tabPage7";
			
			tabPageT8.Location = new Point(4, 4);
			tabPageT8.Name = "tabPage8";
			tabPageT8.Size = new Size(224, 182);
			tabPageT8.TabIndex = 1;
			tabPageT8.Text = "tabPage8";
			
			tabPageT3.Controls.Add(tabControlT4);
			tabPageT3.Location = new Point(4, 22);
			tabPageT3.Name = "tabPage3";
			tabPageT3.Size = new Size(376, 294);
			tabPageT3.TabIndex = 2;
			tabPageT3.Text = "MultiLine";
			
			tabControlT4.Controls.Add(tabPageT11);
			tabControlT4.Controls.Add(tabPageT12);
			tabControlT4.Controls.Add(tabPageT13);
			tabControlT4.Controls.Add(tabPageT14);
			tabControlT4.Controls.Add(tabPageT15);
			tabControlT4.Controls.Add(tabPageT16);
			tabControlT4.Controls.Add(tabPageT17);
			tabControlT4.Controls.Add(tabPageT18);
			tabControlT4.Location = new Point(16, 16);
			tabControlT4.Multiline = true;
			tabControlT4.Name = "tabControl4";
			tabControlT4.SelectedIndex = 0;
			tabControlT4.Size = new Size(344, 256);
			tabControlT4.TabIndex = 0;
			
			tabPageT11.Location = new Point(4, 40);
			tabPageT11.Name = "tabPage11";
			tabPageT11.Size = new Size(336, 212);
			tabPageT11.TabIndex = 0;
			tabPageT11.Text = "tabPage11";
			
			tabPageT12.Location = new Point(4, 40);
			tabPageT12.Name = "tabPage12";
			tabPageT12.Size = new Size(336, 212);
			tabPageT12.TabIndex = 1;
			tabPageT12.Text = "tabPage12";
			
			tabPageT13.Location = new Point(4, 40);
			tabPageT13.Name = "tabPageT13";
			tabPageT13.Size = new Size(336, 212);
			tabPageT13.TabIndex = 2;
			tabPageT13.Text = "tabPageT13";
			
			tabPageT14.Location = new Point(4, 40);
			tabPageT14.Name = "tabPageT14";
			tabPageT14.Size = new Size(336, 212);
			tabPageT14.TabIndex = 3;
			tabPageT14.Text = "tabPageT14";
			
			tabPageT15.Location = new Point(4, 40);
			tabPageT15.Name = "tabPageT15";
			tabPageT15.Size = new Size(336, 212);
			tabPageT15.TabIndex = 4;
			tabPageT15.Text = "tabPageT15";
			
			tabPageT16.Location = new Point(4, 40);
			tabPageT16.Name = "tabPageT16";
			tabPageT16.Size = new Size(336, 212);
			tabPageT16.TabIndex = 5;
			tabPageT16.Text = "tabPageT16";
			
			tabPageT17.Location = new Point(4, 40);
			tabPageT17.Name = "tabPageT17";
			tabPageT17.Size = new Size(336, 212);
			tabPageT17.TabIndex = 6;
			tabPageT17.Text = "tabPageT17";
			
			tabPageT18.BackColor = Color.Red;
			tabPageT18.Location = new Point(4, 40);
			tabPageT18.Name = "tabPageT18";
			tabPageT18.Size = new Size(336, 212);
			tabPageT18.TabIndex = 7;
			tabPageT18.Text = "tabPageT18";
			
			tabPageT4.Controls.Add(tabControlT6);
			tabPageT4.Location = new Point(4, 22);
			tabPageT4.Name = "tabPageT4";
			tabPageT4.Size = new Size(376, 294);
			tabPageT4.TabIndex = 3;
			tabPageT4.Text = "Hottrack";
			
			tabControlT6.Controls.Add(tabPageT29);
			tabControlT6.Controls.Add(tabPageT30);
			tabControlT6.Controls.Add(tabPageT31);
			tabControlT6.HotTrack = true;
			tabControlT6.Location = new Point(8, 24);
			tabControlT6.Name = "tabControlT6";
			tabControlT6.SelectedIndex = 0;
			tabControlT6.Size = new Size(352, 248);
			tabControlT6.TabIndex = 0;
			
			tabPageT29.Location = new Point(4, 22);
			tabPageT29.Name = "tabPageT29";
			tabPageT29.Size = new Size(344, 222);
			tabPageT29.TabIndex = 0;
			tabPageT29.Text = "tabPageT29";
			
			tabPageT30.Location = new Point(4, 22);
			tabPageT30.Name = "tabPageT30";
			tabPageT30.Size = new Size(344, 222);
			tabPageT30.TabIndex = 1;
			tabPageT30.Text = "tabPageT30";
			
			tabPageT31.Location = new Point(4, 22);
			tabPageT31.Name = "tabPageT31";
			tabPageT31.Size = new Size(344, 222);
			tabPageT31.TabIndex = 2;
			tabPageT31.Text = "tabPageT31";
			
			tabPageT5.Controls.Add(labelT3);
			tabPageT5.Controls.Add(tabControlT7);
			tabPageT5.Location = new Point(4, 22);
			tabPageT5.Name = "tabPageT5";
			tabPageT5.Size = new Size(376, 294);
			tabPageT5.TabIndex = 4;
			tabPageT5.Text = "Right";
			
			tabControlT7.Alignment = TabAlignment.Right;
			tabControlT7.Controls.Add(tabPageT32);
			tabControlT7.Controls.Add(tabPageT33);
			tabControlT7.Location = new Point(8, 48);
			tabControlT7.Multiline = true;
			tabControlT7.Name = "tabControlT7";
			tabControlT7.SelectedIndex = 0;
			tabControlT7.Size = new Size(360, 232);
			tabControlT7.TabIndex = 0;
			
			tabPageT32.Location = new Point(4, 4);
			tabPageT32.Name = "tabPageT32";
			tabPageT32.Size = new Size(333, 224);
			tabPageT32.TabIndex = 0;
			tabPageT32.Text = "tabPageT32";
			
			tabPageT33.Location = new Point(4, 4);
			tabPageT33.Name = "tabPageT33";
			tabPageT33.Size = new Size(333, 256);
			tabPageT33.TabIndex = 1;
			tabPageT33.Text = "tabPageT33";
			
			tabPageT19.Controls.Add(labelT4);
			tabPageT19.Location = new Point(4, 22);
			tabPageT19.Name = "tabPageT19";
			tabPageT19.Size = new Size(376, 294);
			tabPageT19.TabIndex = 6;
			tabPageT19.Text = "ImageList";
			
			tabPageT34.Controls.Add(tabControlT1);
			tabPageT34.Location = new Point(4, 22);
			tabPageT34.Name = "tabPageT34";
			tabPageT34.Size = new Size(376, 294);
			tabPageT34.TabIndex = 8;
			tabPageT34.Text = "Docked";
			
			tabControlT1.Controls.Add(tabPageT35);
			tabControlT1.Controls.Add(tabPageT36);
			tabControlT1.Dock = DockStyle.Fill;
			tabControlT1.Location = new Point(0, 0);
			tabControlT1.Name = "tabControlT1";
			tabControlT1.SelectedIndex = 0;
			tabControlT1.Size = new Size(376, 294);
			tabControlT1.TabIndex = 0;
			
			tabPageT35.Location = new Point(4, 22);
			tabPageT35.Name = "tabPageT35";
			tabPageT35.Size = new Size(368, 268);
			tabPageT35.TabIndex = 0;
			tabPageT35.Text = "tabPageT35";
			
			tabPageT36.Location = new Point(4, 22);
			tabPageT36.Name = "tabPageT36";
			tabPageT36.Size = new Size(208, 158);
			tabPageT36.TabIndex = 1;
			tabPageT36.Text = "tabPageT36";
			
			labelT3.Font = new Font("Microsoft Sans Serif", 12F, FontStyle.Bold, GraphicsUnit.Point, ((System.Byte)(0)));
			labelT3.Location = new Point(112, 8);
			labelT3.Name = "label3";
			labelT3.Size = new Size(104, 24);
			labelT3.TabIndex = 1;
			labelT3.Text = "TODO:";
			
			labelT4.Font = new Font("Microsoft Sans Serif", 12F, FontStyle.Bold, GraphicsUnit.Point, ((System.Byte)(0)));
			labelT4.Location = new Point(128, 80);
			labelT4.Name = "label4";
			labelT4.Size = new Size(104, 24);
			labelT4.TabIndex = 2;
			labelT4.Text = "TODO:";
			
			control.Controls.Add(Docked);

		}
		
		private void tabControlT8_DrawItem(object sender, DrawItemEventArgs e)
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
	
		private void AddPrimitivesTest(Control control)
		{
			control.Paint+=new PaintEventHandler(DrawPrimitives);
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

		private void AddMenuTest()
		{
			seperatorMenuItem = new MenuItem("-");
					thisMenuItem = new MenuItem("This");
					thatMenuItem = new MenuItem("That");
						otherAMenuItem = new MenuItem("OtherA");
						otherBMenuItem = new MenuItem("OtherB");
						otherCMenuItem = new MenuItem("OtherC");
					otherMenuItem = new MenuItem("Other", new MenuItem[]{otherAMenuItem, otherBMenuItem, seperatorMenuItem, otherCMenuItem});
				newMenuItem = new MenuItem("New", new MenuItem[]{thisMenuItem, thatMenuItem, otherMenuItem});
				openMenuItem = new MenuItem("Open");
				exitMenuItem = new MenuItem("Exit");
				exitMenuItem.Click +=new EventHandler(exitMenuItem_Click);
			fileMenuItem = new MenuItem("File",new MenuItem[] {newMenuItem, openMenuItem, exitMenuItem});
				cutMenuItem = new MenuItem("Cut");
				copyMenuItem = new MenuItem("Copy");
				pasteMenuItem = new MenuItem("Paste");
			editMenuItem = new MenuItem("Edit", new MenuItem[] {cutMenuItem, copyMenuItem, seperatorMenuItem, pasteMenuItem});
				aboutMenuItem = new MenuItem("About");
			helpMenuItem = new MenuItem("Help", new MenuItem[] {aboutMenuItem});
			mainMenu = new MainMenu(new MenuItem[] { fileMenuItem, editMenuItem, helpMenuItem });
			Menu = mainMenu;
		}

		private void exitMenuItem_Click(object sender, EventArgs e)
		{
			Close();
		}

		private void AddGraphicsTest(Control c)
		{
			c.Paint+=new PaintEventHandler(GraphicsTestPaint);
		}
		private void GraphicsTestPaint(object sender, PaintEventArgs e)
		{
			Graphics g = e.Graphics;
			boundsX = boundsY = boundsPad;
			Rectangle b;

			using (Brush bb = new SolidBrush(Color.Black), rb = new SolidBrush(Color.Red), gb = new SolidBrush(Color.Green))
				using (Pen bp = new Pen(Color.Black), rp = new Pen(Color.Red), gp = new Pen(Color.Green))
				{
					b = NextBounds(g, "DrawArc");
					g.DrawArc(rp, b.Left, b.Top, b.Width, b.Height, 45, 90);
					b = NextBounds(g, "DrawArc");
					g.DrawArc(rp, b.Left, b.Top, b.Width, b.Height, 45, 270);
					b = NextBounds(g, "DrawBezier");
					g.DrawBezier(rp, b.Left, b.Top, b.Left, b.Top + .1F * b.Height, b.Right, b.Top + .2F * b. Height, b.Right, b.Bottom );
					b = NextBounds(g, "DrawClosedCurve");
					g.DrawClosedCurve(rp, new Point[] {b.Location, new Point(b.Right, b.Top), new Point(b.Right, b.Bottom), new Point(b.Left, b.Bottom)});
					b = NextBounds(g, "DrawCurve");
					g.DrawCurve(rp, new Point[]{b.Location, new Point(b.Right, b.Top), new Point(b.Right, b.Bottom), new Point(b.Left, b.Bottom)}); 
					b = NextBounds(g, "DrawCurve");
					g.DrawCurve(rp, new Point[]{b.Location, new Point(b.Right, b.Top), new Point(b.Right, b.Bottom), new Point(b.Left, b.Bottom)}, 0.5F);
					b = NextBounds(g, "DrawEllipse");
					g.DrawEllipse(rp, b);
					b = NextBounds(g, "DrawIcon");
					//g.DrawIcon()
					b = NextBounds(g, "DrawIconUnstretched");
					//g.DrawIconUnstretched();
					b = NextBounds(g, "DrawImage");
					//g.DrawImage();
					b = NextBounds(g, "DrawImageUnscaled");
					//g.DrawImageUnscaled();
					b = NextBounds(g, "DrawLine");
					g.DrawLine(rp, b.Left, b.Top, b.Right, b.Bottom);
					b = NextBounds(g, "DrawLines");
					g.DrawLines(rp, new Point[] { b.Location, new Point (b.Right, b.Bottom), new Point(b.Right, b.Top), new Point(b.Left, b.Bottom)});
					b = NextBounds(g, "DrawPie");
					g.DrawPie(rp, b, 0, 45);
					b = NextBounds(g, "DrawPie");
					g.DrawPie(rp, b, 135, 270);
					b = NextBounds(g, "DrawPolygon");
					g.DrawPolygon(rp, new Point[] { b.Location, new Point (b.Right, b.Bottom), new Point(b.Right, b.Top), new Point(b.Left, b.Bottom)});
					b = NextBounds(g, "DrawRectangle");
					g.DrawRectangle(rp, b);
					b = NextBounds(g, "DrawRectangles");
					g.DrawRectangles(rp, new Rectangle[] { b, new Rectangle(b.Right - 5, b.Bottom - 5, 10, 10)});
					b = NextBounds(g, "FillClosedCurve");
					g.FillClosedCurve(rb, new Point[] {b.Location, new Point(b.Right, b.Top), new Point(b.Right, b.Bottom), new Point(b.Left, b.Bottom)});
					b = NextBounds(g, "FillEllipse");
					g.FillEllipse(rb, b);
					b = NextBounds(g, "FillPie");
					g.FillPie(rb, b, 135, 270);
					b = NextBounds(g, "FillPolygon");
					g.FillPolygon(rb, new Point[] { b.Location, new Point (b.Right, b.Bottom), new Point(b.Right, b.Top), new Point(b.Left, b.Bottom)});
					b = NextBounds(g, "FillRectangle");
					g.FillRectangle(rb, b);
					b = NextBounds(g, "FillRectangles");
					g.FillRectangles(rb, new Rectangle[] { b, new Rectangle(b.Right - 5, b.Bottom - 5, 10, 10)});
				}

			
				/*g.DrawString("BeginContainer", Font, bb, 0, 0);
				g.SetClip(new Rectangle(20,20, 50, 50));
				g.FillRectangle(rb, 0, 0, 100, 100);
				System.Drawing.Drawing2D.GraphicsContainer gc =  g.BeginContainer();
				g.SetClip(new Rectangle(30, 30, 50, 50));
				g.ResetClip();
				g.FillRectangle(gb, 0, 0, 100, 100);
				g.EndContainer(gc);
				*/
		}

		private void AddGraphicsPathTest(Control c)
		{
			c.Paint+=new PaintEventHandler(GraphicsPathTestPaint);
		}
		private void GraphicsPathTestPaint(object sender, PaintEventArgs e)
		{
			Graphics g = e.Graphics;
			boundsX = boundsY = boundsPad;
			Rectangle b;

			using (Brush bb = new SolidBrush(Color.Black), rb = new SolidBrush(Color.Red), gb = new SolidBrush(Color.Green))
			using (Pen bp = new Pen(Color.Black), rp = new Pen(Color.Red), gp = new Pen(Color.Green))
			{
				GraphicsPath path;
				
				b = NextBounds(g, "DrawPath");
				path = new GraphicsPath();
				path.AddArc(b, 0, 135);
				path.AddLine(b.Left, b.Top, b.Right, b.Bottom);
				g.DrawPath(rp, path);
				b = NextBounds(g, "FillPath");
				path = new GraphicsPath();
				path.AddArc(b, 0, 135);
				path.AddLine(b.Left, b.Top, b.Right, b.Bottom);
				g.FillPath(rb, path);
			}
		}

		private void AddGraphicsDrawStringTest(Control c)
		{
			c.Paint+=new PaintEventHandler(GraphicsDrawStringTest);
		}
		private void GraphicsDrawStringTest(object sender, PaintEventArgs e)
		{
			// TODO
			Graphics g = e.Graphics;
			boundsX = boundsY = boundsPad;
			Rectangle b;

			using (Brush bb = new SolidBrush(Color.Black), rb = new SolidBrush(Color.Red), gb = new SolidBrush(Color.Green))
			using (Pen bp = new Pen(Color.Black), rp = new Pen(Color.Red), gp = new Pen(Color.Green))
			{
				
				string s = "Hello 1234 1&2345 1&&23456 123 12 12345";
				StringFormat sf = new StringFormat();
				Font f = new Font("Arial", 6);
					
				b = NextBounds(g, "DrawString");
				g.DrawString(s, f, rb, b.Left, b.Top);
					
				b = NextBounds(g, "DrawString");
				g.DrawString(s, f, rb, b);
					
				sf = new StringFormat(StringFormatFlags.NoWrap);
				b = NextBounds(g, ":NoWrap");
				g.DrawString(s, f, rb, b, sf);
					
				sf = new StringFormat(StringFormatFlags.NoWrap);
				sf.Alignment = StringAlignment.Far;
				b = NextBounds(g, ":NoWrap, Far");
				g.DrawString(s, f, rb, b, sf);

				sf = new StringFormat();
				sf.Alignment = StringAlignment.Far;
				b = NextBounds(g, ":Far");
				g.DrawString(s, f, rb, b, sf);
					
				sf = new StringFormat();
				sf.Alignment = StringAlignment.Center;
				b = NextBounds(g, ":Center");
				g.DrawString(s, f, rb, b, sf);

				sf = new StringFormat();
				sf.LineAlignment = StringAlignment.Far;
				b = NextBounds(g, ":LineFar");
				g.DrawString(s, f, rb, b, sf);

				sf = new StringFormat();
				sf.LineAlignment = StringAlignment.Center;
				b = NextBounds(g, ":LineCenter");
				g.DrawString(s, f, rb, b, sf);

				sf = new StringFormat();
				sf.LineAlignment = StringAlignment.Far;
				sf.Alignment = StringAlignment.Far;
				b = NextBounds(g, ":LineFar Far");
				g.DrawString(s, f, rb, b, sf);

				sf = new StringFormat();
				sf.LineAlignment = StringAlignment.Far;
				sf.Alignment = StringAlignment.Center;
				b = NextBounds(g, ":LineCenter Center");
				g.DrawString(s, f, rb, b, sf);

				sf = new StringFormat();
				sf.HotkeyPrefix = System.Drawing.Text.HotkeyPrefix.Hide;
				b = NextBounds(g, ":Hotkey Hide");
				g.DrawString(s, f, rb, b, sf);

				sf = new StringFormat();
				sf.HotkeyPrefix = System.Drawing.Text.HotkeyPrefix.None;
				b = NextBounds(g, ":Hotkey None");
				g.DrawString(s, f, rb, b, sf);

				sf = new StringFormat();
				sf.HotkeyPrefix = System.Drawing.Text.HotkeyPrefix.Show;
				b = NextBounds(g, ":Hotkey Show");
				g.DrawString(s, f, rb, b, sf);

				sf = new StringFormat();
				sf.HotkeyPrefix = System.Drawing.Text.HotkeyPrefix.Hide;
				b = NextBounds(g, ":Hotkey Hide");
				g.DrawString(s, f, rb, b, sf);

				sf = new StringFormat();
				sf.Trimming = StringTrimming.Word;
				b = NextBounds(g, ":Trim word");
				g.DrawString(s, f, rb, b, sf);
					
				sf = new StringFormat();
				sf.Trimming = StringTrimming.EllipsisWord;
				b = NextBounds(g, ":Trim ellipsisWord");
				g.DrawString(s, f, rb, b, sf);
					
				sf = new StringFormat();
				sf.Trimming = StringTrimming.Character;
				b = NextBounds(g, ":Character");
				g.DrawString(s, f, rb, b, sf);
					
				sf = new StringFormat();
				sf.Trimming = StringTrimming.EllipsisCharacter;
				b = NextBounds(g, ":EllipisCharacter");
				g.DrawString(s, f, rb, b, sf);
					
				sf = new StringFormat();
				sf.Trimming = StringTrimming.EllipsisPath;
				b = NextBounds(g, ":EllipsisPath");
				g.DrawString(s, f, rb, b, sf);
					
			}
		}

		private const int boundsPad = 15;
		private const int boundsSize = 80;
		private int boundsX;
		private int boundsY;
		private Rectangle NextBounds(Graphics g, string text)
		{
			Rectangle r = new Rectangle(boundsX, boundsY, boundsSize, boundsSize);
			boundsX += boundsSize + boundsPad;
			if (boundsX + boundsSize > Width - 10)
			{
				boundsX = boundsPad;
				boundsY += boundsSize + boundsPad;
			}
			g.DrawRectangle(SystemPens.ControlLightLight, r);
			g.DrawString(text, new Font("Arial", 8),SystemBrushes.ControlLightLight, r.X, r.Bottom);
			return r;
		}

		private void AddContextTest(Control c)
		{
			contextMenuLabel1 = new Label();
			contextMenuLabel1.Location = new Point(0, 0);
			contextMenuLabel1.Text = "Right click in this tab page for context menu.";
			contextMenuLabel1.Dock = DockStyle.Top;
			c.Controls.Add(contextMenuLabel1);
			contextMenu = new ContextMenu( new MenuItem[] { cutMenuItem, copyMenuItem, seperatorMenuItem, pasteMenuItem });
			c.ContextMenu = contextMenu;
		}

		private void AddImageTest(Control c)
		{
			image1 = Image.FromFile("test.bmp");
			c.Paint += new PaintEventHandler(Image_Paint);
		}

		private void Image_Paint(object sender, PaintEventArgs e)
		{
			e.Graphics.DrawImage(image1, 10, 10);
		}

		private void AddComboTest(Control c)
		{
			comboBox1 = new ComboBox();
			comboBox2 = new ComboBox();
			comboBox1.Location = new Point(10, 10);
			comboBox1.Enabled = false;
			comboBox1.Items.AddRange(new object[]
			{
				"Item 1",
				"Item 2",
				"Item 3",
				"Item 4",
				"Item 5"
			});

			comboBox2.Location = new Point(10, 50);
			comboBox2.Items.AddRange(new object[]
			{
				"Item 1",
				"Item 2",
				"Item 3",
				"Item 4",
				"Item 5"
			});
			c.Controls.AddRange(new Control[] {comboBox1, comboBox2});
			comboBox1.Visible = true;
		}

		private void AddTreeViewTest(Control c)
		{
			treeView1 = new TreeView();
			treeView1.BeginUpdate();
			treeView1.Nodes.Add("Node1");
			treeView1.Nodes[0].Nodes.Add("Node11");
			treeView1.Nodes[0].Nodes[0].Nodes.Add("Node111");
			treeView1.Nodes[0].Nodes.Add("Node12");
			treeView1.Nodes.Add("Node2");
			treeView1.Nodes.Add("Node3");
			treeView1.Nodes.Add("Node4");
			treeView1.Nodes[3].Nodes.Add("Node41");
			treeView1.Nodes[3].Nodes[0].Nodes.Add("Node411");
			treeView1.Nodes[3].Nodes[0].Nodes.Add("Node412");
			treeView1.Nodes[3].Nodes.Add("Node42");
			treeView1.Nodes[3].Nodes.Add("Node43");
			
			treeView1.EndUpdate();
			treeView1.Bounds = new Rectangle(10,10, 250, 500);
			treeView1.LabelEdit = true;
			c.Controls.Add(treeView1);
		}

		private void AddListBoxTest(Control c)
		{
			listBox1 = new ListBox();
			listBox1.Bounds = new Rectangle(10, 10, 200, 100);
			listBox2 = new ListBox();
			listBox2.Bounds = new Rectangle(10, 150, 200, 100);
			c.Controls.Add(listBox1);
			c.Controls.Add(listBox2);
			listBox1.MultiColumn = true;
			listBox1.SelectionMode = SelectionMode.MultiExtended;
			listBox1.BeginUpdate();
			for (int x = 1; x <= 50; x++)
			{
				listBox1.Items.Add("Item " + x.ToString());
			}
			listBox1.EndUpdate();
			listBox1.SetSelected(1, true);
			listBox1.SetSelected(3, true);
			listBox1.SetSelected(5, true);

			//Console.WriteLine(listBox1.SelectedItems[1].ToString());
			//Console.WriteLine(listBox1.SelectedIndices[0].ToString());

			listBox2.Items.Add("Item 1");
			listBox2.Items.Add("Item 2aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa");
			listBox2.Enabled = false;

		}

		private void AddFormsTest(Control c)
		{
			formsButton1 = new Button();
			formsButton1.Bounds = new Rectangle(20, 20, 120, 20);
			formsButton1.Text = "New Normal Form";
			formsButton1.Click+=new EventHandler(FormsTestClick);
			c.Controls.Add(formsButton1);
		}

		private void FormsTestClick(object sender, EventArgs e)
		{
			Form f = new Form();
			f.Show();
		}

		private void AddTransformsTest(Control c)
		{
			c.Paint+=new PaintEventHandler(TransformsTestPaint);
		}

		private void TransformsTestPaint(object sender, PaintEventArgs e)
		{
			e.Graphics.DrawString("Coming soon..", Font, SystemBrushes.ControlText, 10, 10);
		}

		private void AddScrollbarTest(Control c)
		{
			vScrollBar = new VScrollBar();
			vScrollBar.Bounds = new Rectangle(10, 10, 20, 400);
			vScrollBar.ValueChanged+=new EventHandler(vScrollBar_ValueChanged);
			c.Controls.Add(vScrollBar);

			hScrollBar = new HScrollBar();
			hScrollBar.Bounds = new Rectangle(200, 10, 250, 20);
			hScrollBar.ValueChanged+=new EventHandler(hScrollBar_ValueChanged);
			c.Controls.Add(hScrollBar);
			scroll1LabelMin = new Label();
			scroll1LabelMin.Text = "min";
			scroll1LabelMin.Bounds = new Rectangle(vScrollBar.Right + 10, vScrollBar.Top + 100, 35, 20);
			c.Controls.Add(scroll1LabelMin);
			scroll1TextBoxMin = new TextBox();
			scroll1TextBoxMin.Text = vScrollBar.Minimum.ToString();
			scroll1TextBoxMin.Bounds = new Rectangle(scroll1LabelMin.Right + 5, scroll1LabelMin.Top, 30, 20);
			scroll1TextBoxMin.TextChanged+=new EventHandler(scrollTextBox_TextChanged);
			c.Controls.Add(scroll1TextBoxMin);

			scroll1LabelMax = new Label();
			scroll1LabelMax.Text = "max";
			scroll1LabelMax.Bounds = new Rectangle(scroll1LabelMin.Left, scroll1LabelMin.Bottom + 10, 35, 20);
			c.Controls.Add(scroll1LabelMax);
			scroll1TextBoxMax = new TextBox();
			scroll1TextBoxMax.Text = vScrollBar.Maximum.ToString();
			scroll1TextBoxMax.Bounds = new Rectangle(scroll1LabelMax.Right + 5, scroll1LabelMax.Top, 30, 20); 
			scroll1TextBoxMax.TextChanged+=new EventHandler(scrollTextBox_TextChanged);
			c.Controls.Add(scroll1TextBoxMax);

			scroll1LabelValue = new Label();
			scroll1LabelValue.Text = "value";
			scroll1LabelValue.Bounds = new Rectangle(scroll1LabelMax.Left, scroll1LabelMax.Bottom + 10, 35, 20);
			c.Controls.Add(scroll1LabelValue);
			scroll1TextBoxValue = new TextBox();
			scroll1TextBoxValue.Text = vScrollBar.Value.ToString();
			scroll1TextBoxValue.Bounds = new Rectangle(scroll1LabelValue.Right + 5, scroll1LabelValue.Top, 30, 20); 
			scroll1TextBoxValue.TextChanged+=new EventHandler(scrollTextBox_TextChanged);
			c.Controls.Add(scroll1TextBoxValue);

			scroll1LabelLarge = new Label();
			scroll1LabelLarge.Text = "large";
			scroll1LabelLarge.Bounds = new Rectangle(scroll1LabelValue.Left, scroll1LabelValue.Bottom + 10, 35, 20);
			c.Controls.Add(scroll1LabelLarge);
			scroll1TextBoxLarge = new TextBox();
			scroll1TextBoxLarge.Text = vScrollBar.LargeChange.ToString();
			scroll1TextBoxLarge.Bounds = new Rectangle(scroll1LabelLarge.Right + 5, scroll1LabelLarge.Top, 30, 20); 
			scroll1TextBoxLarge.TextChanged+=new EventHandler(scrollTextBox_TextChanged);
			c.Controls.Add(scroll1TextBoxLarge);

			scroll1LabelSmall = new Label();
			scroll1LabelSmall.Text = "small";
			scroll1LabelSmall.Bounds = new Rectangle(scroll1LabelLarge.Left, scroll1LabelLarge.Bottom + 10, 35, 20);
			c.Controls.Add(scroll1LabelSmall);
			scroll1TextBoxSmall = new TextBox();
			scroll1TextBoxSmall.Text = vScrollBar.SmallChange.ToString();
			scroll1TextBoxSmall.Bounds = new Rectangle(scroll1LabelSmall.Right + 5, scroll1LabelSmall.Top, 30, 20); 
			scroll1TextBoxSmall.TextChanged+=new EventHandler(scrollTextBox_TextChanged);
			c.Controls.Add(scroll1TextBoxSmall);

			scroll2LabelMin = new Label();
			scroll2LabelMin.Text = "min";
			scroll2LabelMin.Bounds = new Rectangle(hScrollBar.Left + 90, hScrollBar.Bottom + 10, 35, 20);
			c.Controls.Add(scroll2LabelMin);
			scroll2TextBoxMin = new TextBox();
			scroll2TextBoxMin.Text = hScrollBar.Minimum.ToString();
			scroll2TextBoxMin.Bounds = new Rectangle(scroll2LabelMin.Right + 5, scroll2LabelMin.Top, 30, 20); 
			scroll2TextBoxMin.TextChanged+=new EventHandler(scrollTextBox_TextChanged);
			c.Controls.Add(scroll2TextBoxMin);

			scroll2LabelMax = new Label();
			scroll2LabelMax.Text = "max";
			scroll2LabelMax.Bounds = new Rectangle(scroll2LabelMin.Left, scroll2LabelMin.Bottom + 10, 35, 20);
			c.Controls.Add(scroll2LabelMax);
			scroll2TextBoxMax = new TextBox();
			scroll2TextBoxMax.Text = hScrollBar.Maximum.ToString();
			scroll2TextBoxMax.Bounds = new Rectangle(scroll2LabelMax.Right + 5, scroll2LabelMax.Top, 30, 20); 
			scroll2TextBoxMax.TextChanged+=new EventHandler(scrollTextBox_TextChanged);
			c.Controls.Add(scroll2TextBoxMax);

			scroll2LabelValue = new Label();
			scroll2LabelValue.Text = "value";
			scroll2LabelValue.Bounds = new Rectangle(scroll2LabelMax.Left, scroll2LabelMax.Bottom + 10, 35, 20);
			c.Controls.Add(scroll2LabelValue);
			scroll2TextBoxValue = new TextBox();
			scroll2TextBoxValue.Text = hScrollBar.Value.ToString();
			scroll2TextBoxValue.Bounds = new Rectangle(scroll2LabelValue.Right + 5, scroll2LabelValue.Top, 30, 20); 
			scroll2TextBoxValue.TextChanged+=new EventHandler(scrollTextBox_TextChanged);
			c.Controls.Add(scroll2TextBoxValue);

			scroll2LabelLarge = new Label();
			scroll2LabelLarge.Text = "large";
			scroll2LabelLarge.Bounds = new Rectangle(scroll2LabelValue.Left, scroll2LabelValue.Bottom + 10, 35, 20);
			c.Controls.Add(scroll2LabelLarge);
			scroll2TextBoxLarge = new TextBox();
			scroll2TextBoxLarge.Text = vScrollBar.LargeChange.ToString();
			scroll2TextBoxLarge.Bounds = new Rectangle(scroll2LabelLarge.Right + 5, scroll2LabelLarge.Top, 30, 20); 
			scroll2TextBoxLarge.TextChanged+=new EventHandler(scrollTextBox_TextChanged);
			c.Controls.Add(scroll2TextBoxLarge);

			scroll2LabelSmall = new Label();
			scroll2LabelSmall.Text = "small";
			scroll2LabelSmall.Bounds = new Rectangle(scroll2LabelLarge.Left, scroll2LabelLarge.Bottom + 10, 35, 20);
			c.Controls.Add(scroll2LabelSmall);
			scroll2TextBoxSmall = new TextBox();
			scroll2TextBoxSmall.Text = vScrollBar.SmallChange.ToString();
			scroll2TextBoxSmall.Bounds = new Rectangle(scroll2LabelSmall.Right + 5, scroll2LabelSmall.Top, 30, 20); 
			scroll2TextBoxSmall.TextChanged+=new EventHandler(scrollTextBox_TextChanged);
			c.Controls.Add(scroll2TextBoxSmall);

		}

		private void vScrollBar_ValueChanged(object sender, EventArgs e)
		{
			scroll1TextBoxValue.Text = vScrollBar.Value.ToString();
		}

		private void hScrollBar_ValueChanged(object sender, EventArgs e)
		{
			scroll2TextBoxValue.Text = hScrollBar.Value.ToString();
		}

		private void scrollTextBox_TextChanged(object sender, EventArgs e)
		{
			try //Lazy!!
			{
				vScrollBar.Minimum = int.Parse(scroll1TextBoxMin.Text);
				vScrollBar.Maximum = int.Parse(scroll1TextBoxMax.Text);
				vScrollBar.Value = int.Parse(scroll1TextBoxValue.Text);
				vScrollBar.LargeChange = int.Parse(scroll1TextBoxLarge.Text);
				vScrollBar.SmallChange = int.Parse(scroll1TextBoxSmall.Text);
				hScrollBar.Minimum = int.Parse(scroll2TextBoxMin.Text);
				hScrollBar.Maximum = int.Parse(scroll2TextBoxMax.Text);
				hScrollBar.Value = int.Parse(scroll2TextBoxValue.Text);
				hScrollBar.LargeChange = int.Parse(scroll2TextBoxLarge.Text);
				hScrollBar.SmallChange = int.Parse(scroll2TextBoxSmall.Text);
			}
			catch
			{}
			
		}

	}
}
