using System;
using System.Drawing;
using System.Windows.Forms;

namespace SWFTest
{
	public class TabTest : System.Windows.Forms.Form
	{
		private System.Windows.Forms.TabPage tabPage1;
		private System.Windows.Forms.TabPage tabPage2;
		private System.Windows.Forms.TabPage tabPage3;
		private System.Windows.Forms.TabPage tabPage4;
		private System.Windows.Forms.TabPage tabPage5;
		private System.Windows.Forms.TabControl tabControl2;
		private System.Windows.Forms.TabPage tabPage6;
		private System.Windows.Forms.TabControl tabControl3;
		private System.Windows.Forms.TabPage tabPage7;
		private System.Windows.Forms.TabPage tabPage8;
		private System.Windows.Forms.Button button1;
		private System.Windows.Forms.Label label1;
		private System.Windows.Forms.Label label2;
		private System.Windows.Forms.TabPage tabPage9;
		private System.Windows.Forms.TabPage tabPage10;
		private System.Windows.Forms.TabControl tabControl4;
		private System.Windows.Forms.TabPage tabPage11;
		private System.Windows.Forms.TabPage tabPage12;
		private System.Windows.Forms.TabPage tabPage13;
		private System.Windows.Forms.TabPage tabPage14;
		private System.Windows.Forms.TabPage tabPage15;
		private System.Windows.Forms.TabPage tabPage16;
		private System.Windows.Forms.TabPage tabPage17;
		private System.Windows.Forms.TabPage tabPage18;
		private System.Windows.Forms.TabPage tabPage19;
		private System.Windows.Forms.TabPage tabPage20;
		private System.Windows.Forms.TabPage tabPage21;
		private System.Windows.Forms.TabPage tabPage22;
		private System.Windows.Forms.TabPage tabPage23;
		private System.Windows.Forms.TabPage tabPage24;
		private System.Windows.Forms.TabPage tabPage25;
		private System.Windows.Forms.TabControl tabControl5;
		private System.Windows.Forms.TabPage tabPage26;
		private System.Windows.Forms.TabPage tabPage27;
		private System.Windows.Forms.TabPage tabPage28;
		private System.Windows.Forms.TabControl tabControl6;
		private System.Windows.Forms.TabPage tabPage29;
		private System.Windows.Forms.TabPage tabPage30;
		private System.Windows.Forms.TabPage tabPage31;
		private System.Windows.Forms.TabControl tabControl7;
		private System.Windows.Forms.TabPage tabPage32;
		private System.Windows.Forms.TabPage tabPage33;
		private System.Windows.Forms.TabControl Docked;
		private System.Windows.Forms.TabPage tabPage34;
		private System.Windows.Forms.TabControl tabControl1;
		private System.Windows.Forms.TabPage tabPage35;
		private System.Windows.Forms.TabPage tabPage36;
		private System.Windows.Forms.TabControl tabControl8;
		private System.Windows.Forms.TabPage tabPage37;
		private System.Windows.Forms.TabPage tabPage38;
		private System.Windows.Forms.Label label3;
		private System.Windows.Forms.Label label4;
		
		private System.ComponentModel.Container components = null;

		public TabTest()
		{
			//
			// Required for Windows Form Designer support
			//
			InitializeComponent();
		}

		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		protected override void Dispose( bool disposing )
		{
			if( disposing )
			{
				if(components != null)
				{
					components.Dispose();
				}
			}
			base.Dispose( disposing );
		}

		#region Windows Form Designer generated code
		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent()
		{
			this.Docked = new System.Windows.Forms.TabControl();
			this.tabPage1 = new System.Windows.Forms.TabPage();
			this.tabControl2 = new System.Windows.Forms.TabControl();
			this.tabPage6 = new System.Windows.Forms.TabPage();
			this.label2 = new System.Windows.Forms.Label();
			this.label1 = new System.Windows.Forms.Label();
			this.button1 = new System.Windows.Forms.Button();
			this.tabPage22 = new System.Windows.Forms.TabPage();
			this.tabPage23 = new System.Windows.Forms.TabPage();
			this.tabPage21 = new System.Windows.Forms.TabPage();
			this.tabPage9 = new System.Windows.Forms.TabPage();
			this.tabPage24 = new System.Windows.Forms.TabPage();
			this.tabPage25 = new System.Windows.Forms.TabPage();
			this.tabPage10 = new System.Windows.Forms.TabPage();
			this.tabControl8 = new System.Windows.Forms.TabControl();
			this.tabPage37 = new System.Windows.Forms.TabPage();
			this.tabPage38 = new System.Windows.Forms.TabPage();
			this.tabPage20 = new System.Windows.Forms.TabPage();
			this.tabControl5 = new System.Windows.Forms.TabControl();
			this.tabPage26 = new System.Windows.Forms.TabPage();
			this.tabPage27 = new System.Windows.Forms.TabPage();
			this.tabPage28 = new System.Windows.Forms.TabPage();
			this.tabPage2 = new System.Windows.Forms.TabPage();
			this.tabControl3 = new System.Windows.Forms.TabControl();
			this.tabPage7 = new System.Windows.Forms.TabPage();
			this.tabPage8 = new System.Windows.Forms.TabPage();
			this.tabPage3 = new System.Windows.Forms.TabPage();
			this.tabControl4 = new System.Windows.Forms.TabControl();
			this.tabPage11 = new System.Windows.Forms.TabPage();
			this.tabPage12 = new System.Windows.Forms.TabPage();
			this.tabPage13 = new System.Windows.Forms.TabPage();
			this.tabPage14 = new System.Windows.Forms.TabPage();
			this.tabPage15 = new System.Windows.Forms.TabPage();
			this.tabPage16 = new System.Windows.Forms.TabPage();
			this.tabPage17 = new System.Windows.Forms.TabPage();
			this.tabPage18 = new System.Windows.Forms.TabPage();
			this.tabPage4 = new System.Windows.Forms.TabPage();
			this.tabControl6 = new System.Windows.Forms.TabControl();
			this.tabPage29 = new System.Windows.Forms.TabPage();
			this.tabPage30 = new System.Windows.Forms.TabPage();
			this.tabPage31 = new System.Windows.Forms.TabPage();
			this.tabPage5 = new System.Windows.Forms.TabPage();
			this.tabControl7 = new System.Windows.Forms.TabControl();
			this.tabPage32 = new System.Windows.Forms.TabPage();
			this.tabPage33 = new System.Windows.Forms.TabPage();
			this.tabPage19 = new System.Windows.Forms.TabPage();
			this.tabPage34 = new System.Windows.Forms.TabPage();
			this.tabControl1 = new System.Windows.Forms.TabControl();
			this.tabPage35 = new System.Windows.Forms.TabPage();
			this.tabPage36 = new System.Windows.Forms.TabPage();
			this.label3 = new System.Windows.Forms.Label();
			this.label4 = new System.Windows.Forms.Label();
			this.Docked.SuspendLayout();
			this.tabPage1.SuspendLayout();
			this.tabControl2.SuspendLayout();
			this.tabPage6.SuspendLayout();
			this.tabPage10.SuspendLayout();
			this.tabControl8.SuspendLayout();
			this.tabPage20.SuspendLayout();
			this.tabControl5.SuspendLayout();
			this.tabPage2.SuspendLayout();
			this.tabControl3.SuspendLayout();
			this.tabPage3.SuspendLayout();
			this.tabControl4.SuspendLayout();
			this.tabPage4.SuspendLayout();
			this.tabControl6.SuspendLayout();
			this.tabPage5.SuspendLayout();
			this.tabControl7.SuspendLayout();
			this.tabPage19.SuspendLayout();
			this.tabPage34.SuspendLayout();
			this.tabControl1.SuspendLayout();
			this.SuspendLayout();
			// 
			// Docked
			// 
			this.Docked.Controls.Add(this.tabPage1);
			this.Docked.Controls.Add(this.tabPage10);
			this.Docked.Controls.Add(this.tabPage5);
			this.Docked.Controls.Add(this.tabPage19);
			this.Docked.Controls.Add(this.tabPage20);
			this.Docked.Controls.Add(this.tabPage2);
			this.Docked.Controls.Add(this.tabPage3);
			this.Docked.Controls.Add(this.tabPage4);
			this.Docked.Controls.Add(this.tabPage34);
			this.Docked.Location = new System.Drawing.Point(24, 16);
			this.Docked.Name = "Docked";
			this.Docked.SelectedIndex = 0;
			this.Docked.Size = new System.Drawing.Size(384, 320);
			this.Docked.TabIndex = 0;
			// 
			// tabPage1
			// 
			this.tabPage1.Controls.Add(this.tabControl2);
			this.tabPage1.Location = new System.Drawing.Point(4, 22);
			this.tabPage1.Name = "tabPage1";
			this.tabPage1.Size = new System.Drawing.Size(376, 294);
			this.tabPage1.TabIndex = 0;
			this.tabPage1.Text = "FillToRight";
			// 
			// tabControl2
			// 
			this.tabControl2.Controls.Add(this.tabPage6);
			this.tabControl2.Controls.Add(this.tabPage22);
			this.tabControl2.Controls.Add(this.tabPage23);
			this.tabControl2.Controls.Add(this.tabPage21);
			this.tabControl2.Controls.Add(this.tabPage9);
			this.tabControl2.Controls.Add(this.tabPage24);
			this.tabControl2.Controls.Add(this.tabPage25);
			this.tabControl2.Location = new System.Drawing.Point(16, 24);
			this.tabControl2.Multiline = true;
			this.tabControl2.Name = "tabControl2";
			this.tabControl2.SelectedIndex = 0;
			this.tabControl2.Size = new System.Drawing.Size(344, 248);
			this.tabControl2.SizeMode = System.Windows.Forms.TabSizeMode.FillToRight;
			this.tabControl2.TabIndex = 1;
			// 
			// tabPage6
			// 
			this.tabPage6.Controls.Add(this.label2);
			this.tabPage6.Controls.Add(this.label1);
			this.tabPage6.Controls.Add(this.button1);
			this.tabPage6.Location = new System.Drawing.Point(4, 40);
			this.tabPage6.Name = "tabPage6";
			this.tabPage6.Size = new System.Drawing.Size(336, 204);
			this.tabPage6.TabIndex = 0;
			this.tabPage6.Text = "First";
			// 
			// label2
			// 
			this.label2.Location = new System.Drawing.Point(144, 160);
			this.label2.Name = "label2";
			this.label2.Size = new System.Drawing.Size(112, 40);
			this.label2.TabIndex = 2;
			this.label2.Text = "label2";
			// 
			// label1
			// 
			this.label1.Location = new System.Drawing.Point(144, 88);
			this.label1.Name = "label1";
			this.label1.Size = new System.Drawing.Size(112, 40);
			this.label1.TabIndex = 1;
			this.label1.Text = "label1";
			// 
			// button1
			// 
			this.button1.Location = new System.Drawing.Point(24, 16);
			this.button1.Name = "button1";
			this.button1.Size = new System.Drawing.Size(96, 40);
			this.button1.TabIndex = 0;
			this.button1.Text = "button1";
			// 
			// tabPage22
			// 
			this.tabPage22.Location = new System.Drawing.Point(4, 40);
			this.tabPage22.Name = "tabPage22";
			this.tabPage22.Size = new System.Drawing.Size(336, 204);
			this.tabPage22.TabIndex = 3;
			this.tabPage22.Text = "tabPage22";
			// 
			// tabPage23
			// 
			this.tabPage23.Location = new System.Drawing.Point(4, 40);
			this.tabPage23.Name = "tabPage23";
			this.tabPage23.Size = new System.Drawing.Size(336, 204);
			this.tabPage23.TabIndex = 4;
			this.tabPage23.Text = "tabPage23";
			// 
			// tabPage21
			// 
			this.tabPage21.Location = new System.Drawing.Point(4, 40);
			this.tabPage21.Name = "tabPage21";
			this.tabPage21.Size = new System.Drawing.Size(336, 204);
			this.tabPage21.TabIndex = 2;
			this.tabPage21.Text = "tabPage21";
			// 
			// tabPage9
			// 
			this.tabPage9.Location = new System.Drawing.Point(4, 40);
			this.tabPage9.Name = "tabPage9";
			this.tabPage9.Size = new System.Drawing.Size(336, 204);
			this.tabPage9.TabIndex = 1;
			this.tabPage9.Text = "Second";
			// 
			// tabPage24
			// 
			this.tabPage24.Location = new System.Drawing.Point(4, 40);
			this.tabPage24.Name = "tabPage24";
			this.tabPage24.Size = new System.Drawing.Size(336, 204);
			this.tabPage24.TabIndex = 5;
			this.tabPage24.Text = "tabPage24";
			// 
			// tabPage25
			// 
			this.tabPage25.Location = new System.Drawing.Point(4, 40);
			this.tabPage25.Name = "tabPage25";
			this.tabPage25.Size = new System.Drawing.Size(336, 204);
			this.tabPage25.TabIndex = 6;
			this.tabPage25.Text = "tabPage25";
			// 
			// tabPage10
			// 
			this.tabPage10.Controls.Add(this.tabControl8);
			this.tabPage10.Location = new System.Drawing.Point(4, 22);
			this.tabPage10.Name = "tabPage10";
			this.tabPage10.Size = new System.Drawing.Size(376, 294);
			this.tabPage10.TabIndex = 5;
			this.tabPage10.Text = "OwnerDraw";
			// 
			// tabControl8
			// 
			this.tabControl8.Controls.Add(this.tabPage37);
			this.tabControl8.Controls.Add(this.tabPage38);
			this.tabControl8.DrawMode = System.Windows.Forms.TabDrawMode.OwnerDrawFixed;
			this.tabControl8.Location = new System.Drawing.Point(32, 24);
			this.tabControl8.Name = "tabControl8";
			this.tabControl8.SelectedIndex = 0;
			this.tabControl8.Size = new System.Drawing.Size(328, 248);
			this.tabControl8.TabIndex = 0;
			this.tabControl8.DrawItem += new System.Windows.Forms.DrawItemEventHandler(this.tabControl8_DrawItem);
			// 
			// tabPage37
			// 
			this.tabPage37.Location = new System.Drawing.Point(4, 22);
			this.tabPage37.Name = "tabPage37";
			this.tabPage37.Size = new System.Drawing.Size(320, 222);
			this.tabPage37.TabIndex = 0;
			this.tabPage37.Text = "tabPage37";
			// 
			// tabPage38
			// 
			this.tabPage38.Location = new System.Drawing.Point(4, 22);
			this.tabPage38.Name = "tabPage38";
			this.tabPage38.Size = new System.Drawing.Size(320, 222);
			this.tabPage38.TabIndex = 1;
			this.tabPage38.Text = "tabPage38";
			// 
			// tabPage20
			// 
			this.tabPage20.Controls.Add(this.tabControl5);
			this.tabPage20.Location = new System.Drawing.Point(4, 22);
			this.tabPage20.Name = "tabPage20";
			this.tabPage20.Size = new System.Drawing.Size(376, 294);
			this.tabPage20.TabIndex = 7;
			this.tabPage20.Text = "FixedSize";
			// 
			// tabControl5
			// 
			this.tabControl5.Controls.Add(this.tabPage26);
			this.tabControl5.Controls.Add(this.tabPage27);
			this.tabControl5.Controls.Add(this.tabPage28);
			this.tabControl5.ItemSize = new System.Drawing.Size(100, 30);
			this.tabControl5.Location = new System.Drawing.Point(24, 32);
			this.tabControl5.Name = "tabControl5";
			this.tabControl5.SelectedIndex = 0;
			this.tabControl5.Size = new System.Drawing.Size(336, 256);
			this.tabControl5.SizeMode = System.Windows.Forms.TabSizeMode.Fixed;
			this.tabControl5.TabIndex = 0;
			// 
			// tabPage26
			// 
			this.tabPage26.Location = new System.Drawing.Point(4, 34);
			this.tabPage26.Name = "tabPage26";
			this.tabPage26.Size = new System.Drawing.Size(328, 218);
			this.tabPage26.TabIndex = 0;
			this.tabPage26.Text = "a";
			// 
			// tabPage27
			// 
			this.tabPage27.Location = new System.Drawing.Point(4, 34);
			this.tabPage27.Name = "tabPage27";
			this.tabPage27.Size = new System.Drawing.Size(328, 218);
			this.tabPage27.TabIndex = 1;
			this.tabPage27.Text = "second";
			// 
			// tabPage28
			// 
			this.tabPage28.Location = new System.Drawing.Point(4, 34);
			this.tabPage28.Name = "tabPage28";
			this.tabPage28.Size = new System.Drawing.Size(328, 218);
			this.tabPage28.TabIndex = 2;
			this.tabPage28.Text = "3rd";
			// 
			// tabPage2
			// 
			this.tabPage2.Controls.Add(this.tabControl3);
			this.tabPage2.Location = new System.Drawing.Point(4, 22);
			this.tabPage2.Name = "tabPage2";
			this.tabPage2.Size = new System.Drawing.Size(376, 294);
			this.tabPage2.TabIndex = 1;
			this.tabPage2.Text = "AlignBot";
			// 
			// tabControl3
			// 
			this.tabControl3.Alignment = System.Windows.Forms.TabAlignment.Bottom;
			this.tabControl3.Controls.Add(this.tabPage7);
			this.tabControl3.Controls.Add(this.tabPage8);
			this.tabControl3.Location = new System.Drawing.Point(32, 32);
			this.tabControl3.Name = "tabControl3";
			this.tabControl3.SelectedIndex = 0;
			this.tabControl3.Size = new System.Drawing.Size(232, 208);
			this.tabControl3.TabIndex = 1;
			// 
			// tabPage7
			// 
			this.tabPage7.Location = new System.Drawing.Point(4, 4);
			this.tabPage7.Name = "tabPage7";
			this.tabPage7.Size = new System.Drawing.Size(224, 182);
			this.tabPage7.TabIndex = 0;
			this.tabPage7.Text = "tabPage7";
			// 
			// tabPage8
			// 
			this.tabPage8.Location = new System.Drawing.Point(4, 4);
			this.tabPage8.Name = "tabPage8";
			this.tabPage8.Size = new System.Drawing.Size(224, 182);
			this.tabPage8.TabIndex = 1;
			this.tabPage8.Text = "tabPage8";
			// 
			// tabPage3
			// 
			this.tabPage3.Controls.Add(this.tabControl4);
			this.tabPage3.Location = new System.Drawing.Point(4, 22);
			this.tabPage3.Name = "tabPage3";
			this.tabPage3.Size = new System.Drawing.Size(376, 294);
			this.tabPage3.TabIndex = 2;
			this.tabPage3.Text = "MultiLine";
			// 
			// tabControl4
			// 
			this.tabControl4.Controls.Add(this.tabPage11);
			this.tabControl4.Controls.Add(this.tabPage12);
			this.tabControl4.Controls.Add(this.tabPage13);
			this.tabControl4.Controls.Add(this.tabPage14);
			this.tabControl4.Controls.Add(this.tabPage15);
			this.tabControl4.Controls.Add(this.tabPage16);
			this.tabControl4.Controls.Add(this.tabPage17);
			this.tabControl4.Controls.Add(this.tabPage18);
			this.tabControl4.Location = new System.Drawing.Point(16, 16);
			this.tabControl4.Multiline = true;
			this.tabControl4.Name = "tabControl4";
			this.tabControl4.SelectedIndex = 0;
			this.tabControl4.Size = new System.Drawing.Size(344, 256);
			this.tabControl4.TabIndex = 0;
			// 
			// tabPage11
			// 
			this.tabPage11.Location = new System.Drawing.Point(4, 40);
			this.tabPage11.Name = "tabPage11";
			this.tabPage11.Size = new System.Drawing.Size(336, 212);
			this.tabPage11.TabIndex = 0;
			this.tabPage11.Text = "tabPage11";
			// 
			// tabPage12
			// 
			this.tabPage12.Location = new System.Drawing.Point(4, 40);
			this.tabPage12.Name = "tabPage12";
			this.tabPage12.Size = new System.Drawing.Size(336, 212);
			this.tabPage12.TabIndex = 1;
			this.tabPage12.Text = "tabPage12";
			// 
			// tabPage13
			// 
			this.tabPage13.Location = new System.Drawing.Point(4, 40);
			this.tabPage13.Name = "tabPage13";
			this.tabPage13.Size = new System.Drawing.Size(336, 212);
			this.tabPage13.TabIndex = 2;
			this.tabPage13.Text = "tabPage13";
			// 
			// tabPage14
			// 
			this.tabPage14.Location = new System.Drawing.Point(4, 40);
			this.tabPage14.Name = "tabPage14";
			this.tabPage14.Size = new System.Drawing.Size(336, 212);
			this.tabPage14.TabIndex = 3;
			this.tabPage14.Text = "tabPage14";
			// 
			// tabPage15
			// 
			this.tabPage15.Location = new System.Drawing.Point(4, 40);
			this.tabPage15.Name = "tabPage15";
			this.tabPage15.Size = new System.Drawing.Size(336, 212);
			this.tabPage15.TabIndex = 4;
			this.tabPage15.Text = "tabPage15";
			// 
			// tabPage16
			// 
			this.tabPage16.Location = new System.Drawing.Point(4, 40);
			this.tabPage16.Name = "tabPage16";
			this.tabPage16.Size = new System.Drawing.Size(336, 212);
			this.tabPage16.TabIndex = 5;
			this.tabPage16.Text = "tabPage16";
			// 
			// tabPage17
			// 
			this.tabPage17.Location = new System.Drawing.Point(4, 40);
			this.tabPage17.Name = "tabPage17";
			this.tabPage17.Size = new System.Drawing.Size(336, 212);
			this.tabPage17.TabIndex = 6;
			this.tabPage17.Text = "tabPage17";
			// 
			// tabPage18
			// 
			this.tabPage18.BackColor = System.Drawing.Color.Red;
			this.tabPage18.Location = new System.Drawing.Point(4, 40);
			this.tabPage18.Name = "tabPage18";
			this.tabPage18.Size = new System.Drawing.Size(336, 212);
			this.tabPage18.TabIndex = 7;
			this.tabPage18.Text = "tabPage18";
			// 
			// tabPage4
			// 
			this.tabPage4.Controls.Add(this.tabControl6);
			this.tabPage4.Location = new System.Drawing.Point(4, 22);
			this.tabPage4.Name = "tabPage4";
			this.tabPage4.Size = new System.Drawing.Size(376, 294);
			this.tabPage4.TabIndex = 3;
			this.tabPage4.Text = "Hottrack";
			// 
			// tabControl6
			// 
			this.tabControl6.Controls.Add(this.tabPage29);
			this.tabControl6.Controls.Add(this.tabPage30);
			this.tabControl6.Controls.Add(this.tabPage31);
			this.tabControl6.HotTrack = true;
			this.tabControl6.Location = new System.Drawing.Point(8, 24);
			this.tabControl6.Name = "tabControl6";
			this.tabControl6.SelectedIndex = 0;
			this.tabControl6.Size = new System.Drawing.Size(352, 248);
			this.tabControl6.TabIndex = 0;
			// 
			// tabPage29
			// 
			this.tabPage29.Location = new System.Drawing.Point(4, 22);
			this.tabPage29.Name = "tabPage29";
			this.tabPage29.Size = new System.Drawing.Size(344, 222);
			this.tabPage29.TabIndex = 0;
			this.tabPage29.Text = "tabPage29";
			// 
			// tabPage30
			// 
			this.tabPage30.Location = new System.Drawing.Point(4, 22);
			this.tabPage30.Name = "tabPage30";
			this.tabPage30.Size = new System.Drawing.Size(344, 222);
			this.tabPage30.TabIndex = 1;
			this.tabPage30.Text = "tabPage30";
			// 
			// tabPage31
			// 
			this.tabPage31.Location = new System.Drawing.Point(4, 22);
			this.tabPage31.Name = "tabPage31";
			this.tabPage31.Size = new System.Drawing.Size(344, 222);
			this.tabPage31.TabIndex = 2;
			this.tabPage31.Text = "tabPage31";
			// 
			// tabPage5
			// 
			this.tabPage5.Controls.Add(this.label3);
			this.tabPage5.Controls.Add(this.tabControl7);
			this.tabPage5.Location = new System.Drawing.Point(4, 22);
			this.tabPage5.Name = "tabPage5";
			this.tabPage5.Size = new System.Drawing.Size(376, 294);
			this.tabPage5.TabIndex = 4;
			this.tabPage5.Text = "Right";
			// 
			// tabControl7
			// 
			this.tabControl7.Alignment = System.Windows.Forms.TabAlignment.Right;
			this.tabControl7.Controls.Add(this.tabPage32);
			this.tabControl7.Controls.Add(this.tabPage33);
			this.tabControl7.Location = new System.Drawing.Point(8, 48);
			this.tabControl7.Multiline = true;
			this.tabControl7.Name = "tabControl7";
			this.tabControl7.SelectedIndex = 0;
			this.tabControl7.Size = new System.Drawing.Size(360, 232);
			this.tabControl7.TabIndex = 0;
			// 
			// tabPage32
			// 
			this.tabPage32.Location = new System.Drawing.Point(4, 4);
			this.tabPage32.Name = "tabPage32";
			this.tabPage32.Size = new System.Drawing.Size(333, 224);
			this.tabPage32.TabIndex = 0;
			this.tabPage32.Text = "tabPage32";
			// 
			// tabPage33
			// 
			this.tabPage33.Location = new System.Drawing.Point(4, 4);
			this.tabPage33.Name = "tabPage33";
			this.tabPage33.Size = new System.Drawing.Size(333, 256);
			this.tabPage33.TabIndex = 1;
			this.tabPage33.Text = "tabPage33";
			// 
			// tabPage19
			// 
			this.tabPage19.Controls.Add(this.label4);
			this.tabPage19.Location = new System.Drawing.Point(4, 22);
			this.tabPage19.Name = "tabPage19";
			this.tabPage19.Size = new System.Drawing.Size(376, 294);
			this.tabPage19.TabIndex = 6;
			this.tabPage19.Text = "ImageList";
			// 
			// tabPage34
			// 
			this.tabPage34.Controls.Add(this.tabControl1);
			this.tabPage34.Location = new System.Drawing.Point(4, 22);
			this.tabPage34.Name = "tabPage34";
			this.tabPage34.Size = new System.Drawing.Size(376, 294);
			this.tabPage34.TabIndex = 8;
			this.tabPage34.Text = "Docked";
			// 
			// tabControl1
			// 
			this.tabControl1.Controls.Add(this.tabPage35);
			this.tabControl1.Controls.Add(this.tabPage36);
			this.tabControl1.Dock = System.Windows.Forms.DockStyle.Fill;
			this.tabControl1.Location = new System.Drawing.Point(0, 0);
			this.tabControl1.Name = "tabControl1";
			this.tabControl1.SelectedIndex = 0;
			this.tabControl1.Size = new System.Drawing.Size(376, 294);
			this.tabControl1.TabIndex = 0;
			// 
			// tabPage35
			// 
			this.tabPage35.Location = new System.Drawing.Point(4, 22);
			this.tabPage35.Name = "tabPage35";
			this.tabPage35.Size = new System.Drawing.Size(368, 268);
			this.tabPage35.TabIndex = 0;
			this.tabPage35.Text = "tabPage35";
			// 
			// tabPage36
			// 
			this.tabPage36.Location = new System.Drawing.Point(4, 22);
			this.tabPage36.Name = "tabPage36";
			this.tabPage36.Size = new System.Drawing.Size(208, 158);
			this.tabPage36.TabIndex = 1;
			this.tabPage36.Text = "tabPage36";
			// 
			// label3
			// 
			this.label3.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
			this.label3.Location = new System.Drawing.Point(112, 8);
			this.label3.Name = "label3";
			this.label3.Size = new System.Drawing.Size(104, 24);
			this.label3.TabIndex = 1;
			this.label3.Text = "TODO:";
			// 
			// label4
			// 
			this.label4.Font = new System.Drawing.Font("Microsoft Sans Serif", 12F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((System.Byte)(0)));
			this.label4.Location = new System.Drawing.Point(128, 80);
			this.label4.Name = "label4";
			this.label4.Size = new System.Drawing.Size(104, 24);
			this.label4.TabIndex = 2;
			this.label4.Text = "TODO:";
			// 
			// Form1
			// 
			this.AutoScaleBaseSize = new System.Drawing.Size(5, 13);
			this.ClientSize = new System.Drawing.Size(416, 347);
			this.Controls.Add(this.Docked);
			this.Name = "Form1";
			this.Text = "TabDemo";
			this.Docked.ResumeLayout(false);
			this.tabPage1.ResumeLayout(false);
			this.tabControl2.ResumeLayout(false);
			this.tabPage6.ResumeLayout(false);
			this.tabPage10.ResumeLayout(false);
			this.tabControl8.ResumeLayout(false);
			this.tabPage20.ResumeLayout(false);
			this.tabControl5.ResumeLayout(false);
			this.tabPage2.ResumeLayout(false);
			this.tabControl3.ResumeLayout(false);
			this.tabPage3.ResumeLayout(false);
			this.tabControl4.ResumeLayout(false);
			this.tabPage4.ResumeLayout(false);
			this.tabControl6.ResumeLayout(false);
			this.tabPage5.ResumeLayout(false);
			this.tabControl7.ResumeLayout(false);
			this.tabPage19.ResumeLayout(false);
			this.tabPage34.ResumeLayout(false);
			this.tabControl1.ResumeLayout(false);
			this.ResumeLayout(false);

		}
		#endregion

		private void tabControl8_DrawItem(object sender, System.Windows.Forms.DrawItemEventArgs e)
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

		public static void Main(String[] args)
		{
			TabTest form = new TabTest();
			Application.Run(form);
		}

	}
}
