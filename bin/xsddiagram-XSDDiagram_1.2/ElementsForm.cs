//    XSDDiagram - A XML Schema Definition file viewer
//    Copyright (C) 2006-2011  Regis COSNIER
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program; if not, write to the Free Software
//    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

using System;
using System.Windows.Forms;

namespace XSDDiagram
{
	public partial class ElementsForm : Form
	{
		public ListBox ListBoxElements { get { return this.listBoxElements; } }

		public ElementsForm()
		{
			InitializeComponent();
		}

		private void listBoxElements_SelectedIndexChanged(object sender, EventArgs e)
		{
			DialogResult = DialogResult.OK;
			Close();
		}

		private void listBoxElements_PreviewKeyDown(object sender, PreviewKeyDownEventArgs e)
		{
			if (e.KeyCode == Keys.Escape)
			{
				DialogResult = DialogResult.Cancel;
				Close();
			}
		}

		//private const int WM_LBUTTONDOWN = 0x0201;

		//protected override void WndProc(ref Message m)
		//{
		//    if (m.Msg == WM_LBUTTONDOWN)
		//    {
		//        int x = (m.LParam.ToInt32() & 0x0000ffff);
		//        int y = (int)(((uint)m.LParam.ToInt32() & (uint)0xffff0000) >> 16);
		//    }

		//    base.WndProc(ref m);
		//}
	}
}