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
		private Label label1;
		private TextBox textBox1;
		private Button button1;

		public TextBoxTest()
		{
			InitializeComponent();
		}

		private void InitializeComponent()
		{
			tabControl1 = new TabControl();
			tabPage1 = new TabPage();
			tabPage2 = new TabPage();
			label1 = new Label();
			textBox1 = new TextBox();
			// 
			// tabControl1
			// 
			tabControl1.Controls.Add(this.tabPage1);
			tabControl1.Controls.Add(this.tabPage2);
			tabControl1.Location = new System.Drawing.Point(4, 4);
			tabControl1.Name = "TextBoxTest";
			tabControl1.SelectedIndex = 0;
			tabControl1.Size = new System.Drawing.Size(384, 320);
			// 
			// tabPage1
			// 
			tabPage1.Name = "test1";
			tabPage1.Text = "test1";
			tabPage1.Controls.Add(label1);
			// 
			// tabPage2
			// 
			this.tabPage2.Name = "test2";
			this.tabPage2.Text = "test2";
			
			// label1
			label1.Bounds = new Rectangle(10,10,50,20);
			label1.Text = "TODO:";
			
			// textBox1
			textBox1.Bounds = new Rectangle(10, 40, 100, 100);
			textBox1.Text = "pnet" + (char)10 + "is great";
			textBox1.TextAlign = HorizontalAlignment.Left;
			textBox1.Multiline = true;
			tabPage1.Controls.Add(textBox1);

			Controls.Add(tabControl1);

			button1 = new Button();
			button1.Text = "test";
			button1.Location = new Point(150,100);
			tabPage1.Controls.Add(button1);
			button1.Click+= new EventHandler(Button1_Click);

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
