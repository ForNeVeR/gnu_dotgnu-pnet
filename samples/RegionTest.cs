using System;
using System.Drawing;
using System.Windows.Forms;

namespace RegionTest
{
	public class RegionTest : System.Windows.Forms.Form
	{
		
		public RegionTest()
		{
			Width = 450;
			Height = 600;
		}

		public static void Main(String[] args)
		{
			RegionTest form = new RegionTest();
			Application.Run(form);
		}

		protected override void OnPaint(PaintEventArgs e)
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
	}

}
