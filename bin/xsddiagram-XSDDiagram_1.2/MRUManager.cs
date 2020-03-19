using System;
using System.Collections.Generic;
using System.Text;
using System.Windows.Forms;
using System.IO;
using Microsoft.Win32;


namespace XSDDiagram
{
	public class MRUManager
	{
        public delegate void EventDelegate(object sender, EventArgs e);

        private string nameOfProgram;
        private string subKeyName;
        private ToolStripMenuItem parentMenuItem;
        private EventDelegate onRecentFileClick;
        private EventDelegate onClearRecentFilesClick;

		public MRUManager(ToolStripMenuItem parentMenuItem, string nameOfProgram, EventDelegate onRecentFileClick, EventDelegate onClearRecentFilesClick)
        {
            if (parentMenuItem == null || onRecentFileClick == null ||
                nameOfProgram == null || nameOfProgram.Length == 0 || nameOfProgram.Contains("\\"))
                throw new ArgumentException("Bad argument.");

            this.parentMenuItem = parentMenuItem;
            this.nameOfProgram = nameOfProgram;
            this.onRecentFileClick = onRecentFileClick;
            this.onClearRecentFilesClick = onClearRecentFilesClick;
            this.subKeyName = string.Format("Software\\{0}\\MRU", this.nameOfProgram);

            this._refreshRecentFilesMenu();
        }

        public void AddRecentFile(string fileNameWithFullPath)
        {
            string value;
            try
            {
                RegistryKey registryKey = Registry.CurrentUser.CreateSubKey(this.subKeyName, RegistryKeyPermissionCheck.ReadWriteSubTree);
                for (int i = 0; true; i++)
                {
                    value = registryKey.GetValue(i.ToString(), null) as string;
                    if (value == null)
                    {
                        registryKey.SetValue(i.ToString(), fileNameWithFullPath);
                        registryKey.Close();
                        break;
                    }
                    else if (value == fileNameWithFullPath)
                    {
                        registryKey.Close();
                        break;
                    }
                }
            }
            catch (Exception ex)
            {
                Console.WriteLine(ex.ToString());
            }
            this._refreshRecentFilesMenu();
        }

        public void RemoveRecentFile(string fileNameWithFullPath)
        {
            try
            {
                RegistryKey registryKey = Registry.CurrentUser.OpenSubKey(this.subKeyName, true);
                string[] valuesNames = registryKey.GetValueNames();
                foreach (string valueName in valuesNames)
                {
                    if ((registryKey.GetValue(valueName, null) as string) == fileNameWithFullPath)
                    {
                        registryKey.DeleteValue(valueName, true);
                        this._refreshRecentFilesMenu();
                        break;
                    }
                }
            }
            catch (Exception ex)
            {
                Console.WriteLine(ex.ToString());
            }
            this._refreshRecentFilesMenu();
        }

        private void _onClearRecentFiles_Click(object sender, EventArgs e)
		{
			try
			{
				RegistryKey registryKey = Registry.CurrentUser.OpenSubKey(this.subKeyName, true);
				if (registryKey == null)
					return;
				string[] values = registryKey.GetValueNames();
				foreach (string valueName in values)
					registryKey.DeleteValue(valueName, true);
				registryKey.Close();
				this.parentMenuItem.DropDownItems.Clear();
				this.parentMenuItem.Enabled = false;
			}
			catch (Exception ex)
			{
				Console.WriteLine(ex.ToString());
			}
			if (onClearRecentFilesClick != null)
				this.onClearRecentFilesClick(sender, e);
		}
		
		private void _refreshRecentFilesMenu()
		{
			RegistryKey registryKey;
			string value;
			ToolStripItem toolStripItem;

			try
			{
				registryKey = Registry.CurrentUser.OpenSubKey(this.subKeyName, false);
				if (registryKey == null)
				{
					this.parentMenuItem.Enabled = false;
					return;
				}
			}
			catch (Exception ex)
			{
				Console.WriteLine("Cannot open recent files registry key:\n" + ex.ToString());
				return;
			}

			this.parentMenuItem.DropDownItems.Clear();
			string[] valueNames = registryKey.GetValueNames();
			foreach (string valueName in valueNames)
			{
				value = registryKey.GetValue(valueName, null) as string;
				if (value == null)
					continue;
				toolStripItem = this.parentMenuItem.DropDownItems.Add(value);
				toolStripItem.Click += new EventHandler(this.onRecentFileClick);
			}

			if (this.parentMenuItem.DropDownItems.Count == 0)
			{
				this.parentMenuItem.Enabled = false;
				return;
			}

			this.parentMenuItem.DropDownItems.Add("-");
			toolStripItem = this.parentMenuItem.DropDownItems.Add("Clear list");
			toolStripItem.Click += new EventHandler(this._onClearRecentFiles_Click);
			this.parentMenuItem.Enabled = true;
		}
	}
}
