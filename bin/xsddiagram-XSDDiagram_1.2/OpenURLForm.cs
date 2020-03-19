using System;
using System.Windows.Forms;

namespace XSDDiagram
{
    public partial class OpenURLForm : Form
    {
        public OpenURLForm(string url)
        {
            InitializeComponent();

            this.textBoxURL.Text = url;
        }

        public string URL { get; set; }

        protected override void OnLoad(EventArgs e)
        {
            base.OnLoad(e);

            this.textBoxURL.Text = URL;
        }

        protected override void OnClosed(EventArgs e)
        {
            if (this.DialogResult == System.Windows.Forms.DialogResult.OK)
            {
                URL = this.textBoxURL.Text;
            }
            base.OnClosed(e);
        }
    }
}
