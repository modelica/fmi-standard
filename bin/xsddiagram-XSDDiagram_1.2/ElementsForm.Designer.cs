namespace XSDDiagram
{
	partial class ElementsForm
	{
		/// <summary>
		/// Required designer variable.
		/// </summary>
		private System.ComponentModel.IContainer components = null;

		/// <summary>
		/// Clean up any resources being used.
		/// </summary>
		/// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
		protected override void Dispose(bool disposing)
		{
			if (disposing && (components != null))
			{
				components.Dispose();
			}
			base.Dispose(disposing);
		}

		#region Windows Form Designer generated code

		/// <summary>
		/// Required method for Designer support - do not modify
		/// the contents of this method with the code editor.
		/// </summary>
		private void InitializeComponent()
		{
			this.listBoxElements = new System.Windows.Forms.ListBox();
			this.SuspendLayout();
			// 
			// listBoxElements
			// 
			this.listBoxElements.Dock = System.Windows.Forms.DockStyle.Fill;
			this.listBoxElements.FormattingEnabled = true;
			this.listBoxElements.Location = new System.Drawing.Point(0, 0);
			this.listBoxElements.Name = "listBoxElements";
			this.listBoxElements.Size = new System.Drawing.Size(115, 121);
			this.listBoxElements.Sorted = true;
			this.listBoxElements.TabIndex = 0;
			this.listBoxElements.PreviewKeyDown += new System.Windows.Forms.PreviewKeyDownEventHandler(this.listBoxElements_PreviewKeyDown);
			this.listBoxElements.SelectedIndexChanged += new System.EventHandler(this.listBoxElements_SelectedIndexChanged);
			// 
			// ElementsForm
			// 
			this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
			this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
			this.ClientSize = new System.Drawing.Size(115, 121);
			this.ControlBox = false;
			this.Controls.Add(this.listBoxElements);
			this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.None;
			this.MaximizeBox = false;
			this.MinimizeBox = false;
			this.Name = "ElementsForm";
			this.ShowIcon = false;
			this.ShowInTaskbar = false;
			this.StartPosition = System.Windows.Forms.FormStartPosition.Manual;
			this.Text = "ElementsForm";
			this.ResumeLayout(false);

		}

		#endregion

		private System.Windows.Forms.ListBox listBoxElements;
	}
}