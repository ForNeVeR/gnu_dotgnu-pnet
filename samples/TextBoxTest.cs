using System;
using System.Drawing;
using System.Windows.Forms;

namespace TextBoxTest
{
	public class TextBoxTest : System.Windows.Forms.Form
	{
		private TabControl tabControl1;
		private TabPage tabPage1;
		private TabPage tabPage2;
		private TextBox[] textBoxes = new TextBox[50];
		private Button button1;

		public TextBoxTest()
		{
			tabControl1 = new TabControl();
			tabControl1.Dock = DockStyle.Fill;
			tabPage1 = new TabPage();
			tabPage1.Text = "Test 1";
			tabControl1.Controls.Add(this.tabPage1);
			tabControl1.SelectedIndex = 0;
			tabPage2 = new TabPage();
			this.tabPage2.Text = "Test 2";
			tabControl1.Controls.Add(this.tabPage2);

			int x = 5;
			int y = 5;
			int i = 0;

			TextBox textBox = new TextBox();
			textBox.Text = "pnet" + (char)10 + "is great";
			textBox.TextAlign = HorizontalAlignment.Left;
			textBox.Multiline = true;
			AddTest( ref i, "Multiline=true", textBox, ref x, ref y);

			textBox = new TextBox();
			textBox.Text = "";
			textBox.TextAlign = HorizontalAlignment.Left;
			textBox.Multiline = false;
			AddTest( ref i, "Multiline=false", textBox, ref x, ref y);

			textBox = new TextBox();
			textBox.Text = "";
			textBox.TextAlign = HorizontalAlignment.Center;
			textBox.Multiline = true;
			AddTest( ref i, "Alignment.Center", textBox, ref x, ref y);

			textBox = new TextBox();
			textBox.Text = "";
			textBox.TextAlign = HorizontalAlignment.Right;
			textBox.Multiline = true;
			AddTest( ref i, "Alignment.Right", textBox, ref x, ref y);

			button1 = new Button();
			button1.Text = "Test";
			button1.Location = new Point(x,y);
			tabPage1.Controls.Add(button1);
			button1.Click+= new EventHandler(Button1_Click);



			Controls.Add(tabControl1);

		}

		private void AddTest(ref int index, string text, TextBox textBox, ref int x, ref int y)
		{
			Label label = new Label();
			label.Bounds = new Rectangle(x, y, 50, 20);
			label.AutoSize = true;
			label.Text = text;
			int x1 = x+ label.Width+10;
			tabPage1.Controls.Add(label);
			
			textBox.Bounds = new Rectangle(x1, y, 100, 40);
			y+= textBox.Height+10;
			tabPage1.Controls.Add(textBox);
			textBoxes[index++] = textBox;
		}

		public static void Main(String[] args)
		{
			TextBoxTest form = new TextBoxTest();
			Application.Run(form);
		}

		private void Button1_Click(object sender, EventArgs e)
		{
			Region r1 = new Region(new Rectangle(0,0,5,5));
			Region r2 = new Region(new Rectangle(0,0,0,0));
			r1.Intersect(r2);
			Invalidate(r1);

		}
	}
}
