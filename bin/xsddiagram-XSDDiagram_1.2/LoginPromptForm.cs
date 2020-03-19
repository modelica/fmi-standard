using System;
using System.Windows.Forms;

namespace XSDDiagram
{
    public partial class LoginPromptForm : Form
    {
        public LoginPromptForm(string label)
        {
            InitializeComponent();

            this.textBoxLabel.Text = label;
        }

        public string Username { get; set; }
        public string Password { get; set; }

        protected override void OnLoad(EventArgs e)
        {
            base.OnLoad(e);

            this.textBoxUsername.Text = Username;
            this.textBoxPassword.Text = Password;
        }

        protected override void OnClosed(EventArgs e)
        {
            if (this.DialogResult == System.Windows.Forms.DialogResult.OK)
            {
                Username = this.textBoxUsername.Text;
                Password = this.textBoxPassword.Text;
            }
            base.OnClosed(e);
        }
    }
}
