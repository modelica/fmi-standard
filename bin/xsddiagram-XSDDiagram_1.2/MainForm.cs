//    XSDDiagram - A XML Schema Definition file viewer
//    Copyright (C) 2006-2016  Regis COSNIER
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
using System.Collections;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Drawing.Imaging;
using System.IO;
using System.Net;
using System.Text;
using System.Text.RegularExpressions;
using System.Windows.Forms;
using System.Xml;
using System.Xml.Serialization;

// To generate the XMLSchema.cs file:
// > xsd.exe XMLSchema.xsd /classes /l:cs /n:XMLSchema /order

using XSDDiagram.Rendering;
using System.Xml.Schema;
using System.Diagnostics;

using System.Security.Principal;

namespace XSDDiagram
{
	public partial class MainForm : Form
	{
        private DiagramPrinter _diagramPrinter;
        private DiagramGdiRenderer _diagramGdiRenderer;
        private Rectangle _renderingClipRectangle = new Rectangle();

        private Diagram diagram = new Diagram();
        private Schema schema = new Schema();
        private Dictionary<string, TabPage> hashtableTabPageByFilename = new Dictionary<string, TabPage>();
		private string originalTitle = "";
		private DiagramItem contextualMenuPointedElement = null;
        //private string currentLoadedSchemaFilename = "";

        private TextBox textBoxAnnotation;
        private WebBrowser webBrowserDocumentation;
		private bool webBrowserSupported = true;
        private string backupUsername = "", backupPassword = "";

        private MRUManager mruManager;

        public MainForm()
		{
			InitializeComponent();

            bool isElevated = false;
            WindowsIdentity identity = null;
            try
            {
                identity = WindowsIdentity.GetCurrent();
                if (identity != null)
                {
                    WindowsPrincipal principal = new WindowsPrincipal(identity);
                    if (principal != null)
                        isElevated = principal.IsInRole(WindowsBuiltInRole.Administrator);
                }
            }
            catch (UnauthorizedAccessException)
            {
            }
            catch (Exception)
            {
            }
            finally
            {
                if (identity != null)
                    identity.Dispose();
            }
            this.toolsToolStripMenuItem.Visible = isElevated && !Options.IsRunningOnMono;
            this.diagram.ShowDocumentation = this.toolStripButtonShowDocumentation.Checked = Options.ShowDocumentation;

            this.originalTitle = Text;

			this.toolStripComboBoxSchemaElement.Sorted = true;
			this.toolStripComboBoxSchemaElement.Items.Add("");

			this.diagram.RequestAnyElement += new Diagram.RequestAnyElementEventHandler(diagram_RequestAnyElement);
            this.panelDiagram.VirtualSize = new Size(0, 0);
            this.panelDiagram.DiagramControl.ContextMenuStrip = this.contextMenuStripDiagram;
			this.panelDiagram.DiagramControl.MouseWheel += new MouseEventHandler(DiagramControl_MouseWheel);
			this.panelDiagram.DiagramControl.MouseClick += new MouseEventHandler(DiagramControl_MouseClick);
			this.panelDiagram.DiagramControl.MouseHover += new EventHandler(DiagramControl_MouseHover);
            this.panelDiagram.DiagramControl.MouseMove += new MouseEventHandler(DiagramControl_MouseMove);
            //this.panelDiagram.DiagramControl.KeyDown += DiagramControl_KeyDown;
            this.panelDiagram.DiagramControl.KeyDown += new KeyEventHandler(DiagramControl_KeyDown);
            this.panelDiagram.DiagramControl.Paint += new PaintEventHandler(DiagramControl_Paint);

            this.schema.RequestCredential += schema_RequestCredential;
            this.backupUsername = Options.Username;
            this.backupPassword = Options.Password;
			if (Options.IsRunningOnMono)
			{
				try
				{
					new WebBrowser().Navigate("about:blank");
				}
				catch
				{
					webBrowserSupported = false;
				}
			}

            UpdateActionsState();
        }

        bool schema_RequestCredential(string url, string realm, int attemptCount, out string username, out string password)
        {
            string label = "The file '" + url + "' requires a username and password.";
            LoginPromptForm dlg = new LoginPromptForm(label);
            dlg.Username = backupUsername;
            dlg.Password = backupPassword;
            if (dlg.ShowDialog(this) == DialogResult.OK)
            {
                backupUsername = username = dlg.Username;
                backupPassword = password = dlg.Password;
                return true;
            }
            username = password = "";
            return false;
        }

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (Options.IsRunningOnMono)
            {
                // Prevent exception with Linux on Mono
                object[] toolStripMenuItems = new object[] { this.fileToolStripMenuItem, this.fileToolStripMenuItem, this.openToolStripMenuItem, this.openToolStripMenuItem, this.openURLToolStripMenuItem, this.openURLToolStripMenuItem, this.saveDiagramToolStripMenuItem, this.saveDiagramToolStripMenuItem, this.validateXMLFileToolStripMenuItem, this.validateXMLFileToolStripMenuItem, this.recentFilesToolStripMenuItem, this.recentFilesToolStripMenuItem, this.closeToolStripMenuItem, this.closeToolStripMenuItem, this.toolStripMenuItem2, this.pageToolStripMenuItem, this.pageToolStripMenuItem, this.printPreviewToolStripMenuItem, this.printPreviewToolStripMenuItem, this.printToolStripMenuItem, this.printToolStripMenuItem, this.toolStripMenuItem1, this.exitToolStripMenuItem, this.exitToolStripMenuItem, this.toolsToolStripMenuItem, this.toolsToolStripMenuItem, this.windowsExplorerRegistrationToolStripMenuItem, this.windowsExplorerRegistrationToolStripMenuItem, this.registerToolStripMenuItem, this.registerToolStripMenuItem, this.unregisterToolStripMenuItem, this.unregisterToolStripMenuItem, this.windowToolStripMenuItem, this.windowToolStripMenuItem, this.nextTabToolStripMenuItem, this.nextTabToolStripMenuItem, this.previousTabToolStripMenuItem, this.previousTabToolStripMenuItem, this.helpToolStripMenuItem, this.helpToolStripMenuItem, this.aboutToolStripMenuItem, this.aboutToolStripMenuItem, this.toolStripMenuItemAttributesCopyLine, this.toolStripMenuItemAttributesCopyLine, this.toolStripMenuItemAttributesCopyList, this.toolStripMenuItemAttributesCopyList, this.toolStripMenuItemEnumerateCopyLine, this.toolStripMenuItemEnumerateCopyLine, this.toolStripMenuItemEnumerateCopyList, this.toolStripMenuItemEnumerateCopyList, this.addToDiagrammToolStripMenuItem, this.addToDiagrammToolStripMenuItem, this.toolStripMenuItem4, this.toolStripMenuItemElementsCopyLine, this.toolStripMenuItemElementsCopyLine, this.toolStripMenuItemElementsCopyList, this.toolStripMenuItemElementsCopyList, this.gotoXSDFileToolStripMenuItem, this.gotoXSDFileToolStripMenuItem, this.expandToolStripMenuItem, this.expandToolStripMenuItem, this.removeFromDiagramToolStripMenuItem, this.removeFromDiagramToolStripMenuItem, this.toolStripMenuItem3, this.addAllToolStripMenuItem, this.addAllToolStripMenuItem, this.removeAllToolStripMenuItem, this.removeAllToolStripMenuItem, this.expandOneLevelToolStripMenuItem, this.expandOneLevelToolStripMenuItem };
                foreach (var toolStripMenuItem in toolStripMenuItems)
                    GC.SuppressFinalize(toolStripMenuItem);
            }

            if (disposing)
            {
                if (components != null)
                {
                    components.Dispose();
                    components = null;
                }

                if (_diagramPrinter != null)
                {
                    _diagramPrinter.Dispose();
                    _diagramPrinter = null;
                }
            }

            base.Dispose(disposing);
        }

		private void MainForm_Load(object sender, EventArgs e)
		{
            this.mruManager = new MRUManager(this.recentFilesToolStripMenuItem, "xsddiagram", this.recentFilesToolStripMenuSubItemFile_Click, this.recentFilesToolStripMenuSubItemClearAll_Click);


            this.toolStripComboBoxZoom.SelectedIndex = 8;
			this.toolStripComboBoxAlignement.SelectedIndex = 1;

			if (!string.IsNullOrEmpty(Options.InputFile))
			{
				LoadSchema(Options.InputFile);
				foreach (var rootElement in Options.RootElements)
				{
                    string elementName = rootElement;
                    string elementNamespace = null;
                    if (!string.IsNullOrEmpty(elementName))
                    {
                        var pos = rootElement.IndexOf("@");
                        if (pos != -1)
                        {
                            elementName = rootElement.Substring(0, pos);
                            elementNamespace = rootElement.Substring(pos + 1);
                        }
                    }

                    foreach (var element in schema.Elements)
                    {
                        if ((elementNamespace != null && elementNamespace == element.NameSpace && element.Name == elementName) ||
                            (elementNamespace == null && element.Name == elementName))
                        {
                            diagram.Add(element.Tag, element.NameSpace);
                        }
                    }
                }
				for (int i = 0; i < Options.ExpandLevel; i++)
				{
					diagram.ExpandOneLevel();
				}
				UpdateDiagram();
			}
		}

		private void openToolStripMenuItem_Click(object sender, EventArgs e)
		{
			OpenFileDialog openFileDialog = new OpenFileDialog();
			openFileDialog.Filter = "xsd files (*.xsd)|*.xsd|All files (*.*)|*.*";
			openFileDialog.FilterIndex = 1;
			openFileDialog.RestoreDirectory = true;
			if (openFileDialog.ShowDialog() == DialogResult.OK)
				LoadSchema(openFileDialog.FileName);
		}

        private void openURLToolStripMenuItem_Click(object sender, EventArgs e)
        {
            OpenURLForm openURLForm = new OpenURLForm("");
            if (openURLForm.ShowDialog() == DialogResult.OK)
                LoadSchema(openURLForm.URL);
        }

        private void closeToolStripMenuItem_Click(object sender, EventArgs e)
        {
            CleanupUserInterface(true);
        }

        private void recentFilesToolStripMenuSubItemFile_Click(object sender, EventArgs evt)
        {
            string filenameOrURL = (sender as ToolStripItem).Text;
            LoadSchema(filenameOrURL);
            //this.mruManager.RemoveRecentFile(filenameOrURL);
        }

        private void recentFilesToolStripMenuSubItemClearAll_Click(object sender, EventArgs evt)
        {
        }

        private void MainForm_DragDrop(object sender, DragEventArgs e)
		{
            if (e.Data.GetDataPresent("UniformResourceLocator"))
            {
                string url = e.Data.GetData(DataFormats.Text, true) as string;
                if (!string.IsNullOrEmpty(url))
                    LoadSchema(url.Trim());
            }
            else if (e.Data.GetDataPresent(DataFormats.FileDrop))
            {
                string[] files = (string[])e.Data.GetData(DataFormats.FileDrop);
                if(files != null && files.Length > 0)
                    LoadSchema(files[0]);
            }
		}

		private void MainForm_DragEnter(object sender, DragEventArgs e)
		{
            if (e.Data.GetDataPresent(DataFormats.FileDrop)
                || e.Data.GetDataPresent("UniformResourceLocator")
                )
				e.Effect = DragDropEffects.Move;
			else
				e.Effect = DragDropEffects.None;
		}

		private void saveDiagramToolStripMenuItem_Click(object sender, EventArgs e)
		{
			SaveFileDialog saveFileDialog = new SaveFileDialog();
            saveFileDialog.Filter = "SVG files (*.svg)|*.svg" + (Options.IsRunningOnMono ? "" : "|EMF files (*.emf)|*.emf") + "|PNG files (*.png)|*.png|JPG files (*.jpg)|*.jpg|TXT files (*.txt)|*.txt|CSV files (*.csv)|*.csv|All files (*.*)|*.*";
			saveFileDialog.FilterIndex = 1;
			saveFileDialog.RestoreDirectory = true;
			if (saveFileDialog.ShowDialog() == DialogResult.OK)
			{
                string outputFilename = saveFileDialog.FileName;
				try
				{
                    DiagramExporter exporter = new DiagramExporter(diagram);
                    Graphics g1 = this.panelDiagram.DiagramControl.CreateGraphics();
                    exporter.Export(outputFilename, g1, new DiagramAlertHandler(SaveAlert), new Dictionary<string, object>() 
                            { 
                                { "TextOutputFields", Options.TextOutputFields }
                                //For future parameters, {}
                            });
                    g1.Dispose();
				}
                catch (System.ArgumentException ex)
                {
                    MessageBox.Show("You have reach the system limit.\r\nPlease remove some element from the diagram to make it smaller.");
                    System.Diagnostics.Trace.WriteLine(ex.ToString());
                }
                catch (System.Runtime.InteropServices.ExternalException ex)
                {
                    MessageBox.Show("You have reach the system limit.\r\nPlease remove some element from the diagram to make it smaller.");
                    System.Diagnostics.Trace.WriteLine(ex.ToString());
                }
                catch (Exception ex)
				{
                    MessageBox.Show(ex.Message);
					System.Diagnostics.Trace.WriteLine(ex.ToString());
				}
			}
		}

        bool SaveAlert(string title, string message)
        {
            return MessageBox.Show(this, message, title, MessageBoxButtons.YesNo) == DialogResult.Yes;
        }

		private void exitToolStripMenuItem_Click(object sender, EventArgs e)
		{
			Close();
		}

		private void aboutToolStripMenuItem_Click(object sender, EventArgs e)
		{
			AboutForm aboutForm = new AboutForm();
			aboutForm.ShowDialog(this);
		}

		private void toolStripComboBoxSchemaElement_SelectedIndexChanged(object sender, EventArgs e)
		{
			if (this.toolStripComboBoxSchemaElement.SelectedItem != null)
			{
				XSDObject xsdObject = this.toolStripComboBoxSchemaElement.SelectedItem as XSDObject;
				if (xsdObject != null)
					SelectSchemaElement(xsdObject);
			}
		}

		private void toolStripButtonAddToDiagram_Click(object sender, EventArgs e)
		{
			if (this.toolStripComboBoxSchemaElement.SelectedItem != null)
            {
                XSDObject xsdObject = this.toolStripComboBoxSchemaElement.SelectedItem as XSDObject;
                if (xsdObject != null)
                {
                    DiagramItem diagramItem = this.diagram.Add(xsdObject.Tag, xsdObject.NameSpace);
                    if(diagramItem != null)
                        SelectDiagramElement(diagramItem, true);
                    else
                        UpdateDiagram();
                }
            }
        }

        private void toolStripButtonAddAllToDiagram_Click(object sender, EventArgs e)
		{
            DiagramItem firstDiagramItem = null;
            foreach (XSDObject xsdObject in this.schema.ElementsByName.Values)
                if (xsdObject != null)
                {
                    DiagramItem diagramItem = this.diagram.Add(xsdObject.Tag, xsdObject.NameSpace);
                    if (firstDiagramItem == null && diagramItem != null)
                        firstDiagramItem = diagramItem;
                }
            if(firstDiagramItem != null)
                SelectDiagramElement(firstDiagramItem, true);
            else
                UpdateDiagram();
        }

		void DiagramControl_Paint(object sender, PaintEventArgs e)
		{
            if (_diagramGdiRenderer == null)
                _diagramGdiRenderer = new DiagramGdiRenderer(e.Graphics);
            else if (e.Graphics != _diagramGdiRenderer.Graphics)
                _diagramGdiRenderer.Graphics = e.Graphics;
            if (_diagramGdiRenderer != null)
            {
                Point virtualPoint = this.panelDiagram.VirtualPoint;
                e.Graphics.TranslateTransform(-(float)virtualPoint.X, -(float)virtualPoint.Y);
                e.Graphics.SmoothingMode = System.Drawing.Drawing2D.SmoothingMode.AntiAlias;
                e.Graphics.TextRenderingHint = System.Drawing.Text.TextRenderingHint.AntiAlias;
                _renderingClipRectangle.Location = virtualPoint;
                _renderingClipRectangle.Size = this.panelDiagram.DiagramControl.ClientRectangle.Size;

                _diagramGdiRenderer.Render(diagram, _renderingClipRectangle);
            }
		}

		private void UpdateDiagram()
		{
			if (this.diagram.RootElements.Count != 0)
			{
				Graphics g = this.panelDiagram.DiagramControl.CreateGraphics();
				this.diagram.Layout(g);
				g.Dispose();
				Size bbSize = this.diagram.BoundingBox.Size + this.diagram.Padding + this.diagram.Padding;
				this.panelDiagram.VirtualSize = new Size((int)(bbSize.Width * this.diagram.Scale), (int)(bbSize.Height * this.diagram.Scale));
			}
			else
				this.panelDiagram.VirtualSize = new Size(0, 0);
		}

		private void UpdateTitle(string filename)
		{
			if (filename.Length > 0)
				Text = this.originalTitle + " - " + filename;
			else
				Text = this.originalTitle;
		}

		private void LoadSchema(string schemaFilename)
        {
            Cursor = Cursors.WaitCursor;

            this.mruManager.AddRecentFile(schemaFilename);

            CleanupUserInterface(false);

            UpdateTitle(schemaFilename);

            schema.LoadSchema(schemaFilename);

            UpdateActionsState();

            foreach (XSDObject xsdObject in schema.Elements)
            {
                this.listViewElements.Items.Add(new ListViewItem(new string[] { xsdObject.Name, xsdObject.Type, xsdObject.NameSpace })).Tag = xsdObject;
                this.toolStripComboBoxSchemaElement.Items.Add(xsdObject);
            }

            Cursor = Cursors.Default;

            if (this.schema.LoadError.Count > 0)
            {
                ErrorReportForm errorReportForm = new ErrorReportForm();
                errorReportForm.Errors = this.schema.LoadError;
                errorReportForm.ShowDialog(this);
            }

            this.diagram.ElementsByName = this.schema.ElementsByName;
            if (this.schema.FirstElement != null)
                this.toolStripComboBoxSchemaElement.SelectedItem = this.schema.FirstElement;
            else
                this.toolStripComboBoxSchemaElement.SelectedIndex = 0;

            tabControlView_Selected(null, null);

            this.tabControlView.SuspendLayout();
            foreach (string filename in this.schema.XsdFilenames)
            {
                string fullPath = filename;
                Control browser = null;
                if (webBrowserSupported)
                    browser = new WebBrowser();
                else
                    browser = new System.Windows.Forms.TextBox() { Multiline = true, ReadOnly = true, ScrollBars = ScrollBars.Both };
                browser.Dock = DockStyle.Fill;
                browser.TabIndex = 0;
                try
                {
                    new Uri(filename);
                }
                catch
                {
                    fullPath = Path.GetFullPath(filename);
                }
                TabPage tabPage = new TabPage(Path.GetFileNameWithoutExtension(filename));
                tabPage.Tag = fullPath;
                tabPage.ToolTipText = fullPath;
                tabPage.Controls.Add(browser);
                tabPage.UseVisualStyleBackColor = true;

                this.tabControlView.TabPages.Add(tabPage);
                this.hashtableTabPageByFilename[filename] = tabPage;

            }
            this.tabControlView.ResumeLayout();

            //currentLoadedSchemaFilename = schemaFilename;
        }

        private void UpdateActionsState()
        {
            bool isSchemaLoaded = schema.IsLoaded();
            toolStripButtonSaveDiagram.Enabled = isSchemaLoaded;
            toolStripButtonPrint.Enabled = isSchemaLoaded;
            toolStripButtonAddToDiagram.Enabled = isSchemaLoaded;
            toolStripButtonAddAllToDiagram.Enabled = isSchemaLoaded;
            toolStripButtonRemoveAllFromDiagram.Enabled = isSchemaLoaded;
            toolStripButtonExpandOneLevel.Enabled = isSchemaLoaded;
            closeToolStripMenuItem.Enabled = isSchemaLoaded;
            saveDiagramToolStripMenuItem.Enabled = isSchemaLoaded;
            validateXMLFileToolStripMenuItem.Enabled = isSchemaLoaded;
            printPreviewToolStripMenuItem.Enabled = isSchemaLoaded;
            printToolStripMenuItem.Enabled = isSchemaLoaded;
        }

        private void CleanupUserInterface(bool fullCleanup)
        {
            this.diagram.Clear();
            this.panelDiagram.VirtualSize = new Size(0, 0);
            this.panelDiagram.VirtualPoint = new Point(0, 0);
            this.panelDiagram.Clear();
            this.hashtableTabPageByFilename.Clear();
            this.listViewElements.Items.Clear();
            this.listViewAttributes.Items.Clear();
            this.toolStripComboBoxSchemaElement.SelectedItem = "";
            this.toolStripComboBoxSchemaElement.Items.Clear();
            this.toolStripComboBoxSchemaElement.Items.Add("");
            this.propertyGridSchemaObject.SelectedObject = null;
            this.textBoxElementPath.Text = "";

            while (this.tabControlView.TabCount > 1)
                this.tabControlView.TabPages.RemoveAt(1);

            ShowDocumentation(null);

            if (fullCleanup)
            {
                UpdateTitle("");
                schema.Cleanup();
                UpdateActionsState();
            }
        }

        void DiagramControl_MouseClick(object sender, MouseEventArgs e)
		{
			Point location = e.Location;
			location.Offset(this.panelDiagram.VirtualPoint);

			DiagramItem resultElement;
			DiagramHitTestRegion resultRegion;
			this.diagram.HitTest(location, out resultElement, out resultRegion);
			if (resultRegion != DiagramHitTestRegion.None)
			{
				if (resultRegion == DiagramHitTestRegion.ChildExpandButton)
				{
					if (resultElement.HasChildElements)
					{
						if (resultElement.ChildElements.Count == 0)
						{
							this.diagram.ExpandChildren(resultElement);
							resultElement.ShowChildElements = true;
						}
						else
                        {
                            this.diagram.ClearSearch();
                            resultElement.ShowChildElements ^= true;
                        }
							

                        //UpdateDiagram();
                        //this.panelDiagram.ScrollTo(this.diagram.ScalePoint(resultElement.Location), true);
                        SelectDiagramElement(resultElement, true);
                    }
				}
				else if (resultRegion == DiagramHitTestRegion.Element)
				{
					if ((ModifierKeys & (Keys.Control | Keys.Shift)) == (Keys.Control | Keys.Shift)) // For Debug
					{
						this.toolStripComboBoxSchemaElement.SelectedItem = "";
						this.propertyGridSchemaObject.SelectedObject = resultElement;
					}
					else
						SelectDiagramElement(resultElement);
				}
				else
					SelectDiagramElement(null);
			}
		}

		private void SelectDiagramElement(DiagramItem element)
		{
			SelectDiagramElement(element, false);
		}

        private void SelectDiagramElement(DiagramItem element, bool scrollToElement)
		{
			this.textBoxElementPath.Text = "";

            if (element == null)
			{
				this.toolStripComboBoxSchemaElement.SelectedItem = "";
				this.propertyGridSchemaObject.SelectedObject = null;
				this.listViewAttributes.Items.Clear();
			}
            else
			{
                XSDObject xsdObject;
                if (this.schema.ElementsByName.TryGetValue(element.FullName, out xsdObject) && xsdObject != null)
                    this.toolStripComboBoxSchemaElement.SelectedItem = xsdObject;
				else
					this.toolStripComboBoxSchemaElement.SelectedItem = null;

				SelectSchemaElement(element);

				string path = '/' + element.Name;
				DiagramItem parentElement = element.Parent;
				while (parentElement != null)
				{
					if (parentElement.ItemType == DiagramItemType.element && !string.IsNullOrEmpty(parentElement.Name))
						path = '/' + parentElement.Name + path;
					parentElement = parentElement.Parent;
				}
				this.textBoxElementPath.Text = path;
			}

            this.diagram.SelectElement(element);
            UpdateDiagram();
            if (scrollToElement)
                this.panelDiagram.ScrollTo(this.diagram.ScalePoint(element.Location), true);
        }

        private void SelectSchemaElement(XSDObject xsdObject)
		{
			SelectSchemaElement(xsdObject.Tag, xsdObject.NameSpace);
		}

		private void SelectSchemaElement(DiagramItem diagramBase)
		{
			SelectSchemaElement(diagramBase.TabSchema, diagramBase.NameSpace);
		}

		private void SelectSchemaElement(XMLSchema.openAttrs openAttrs, string nameSpace)
		{
			this.propertyGridSchemaObject.SelectedObject = openAttrs;
			ShowDocumentation(null);

			XMLSchema.annotated annotated = openAttrs as XMLSchema.annotated;
			if (annotated != null)
			{
				// Element documentation
				if (annotated.annotation != null)
					ShowDocumentation(annotated.annotation);

				// Show the enumeration
				ShowEnumerate(annotated);

				// Attributes enumeration
				List<XSDAttribute> listAttributes = DiagramHelpers.GetAnnotatedAttributes(this.schema, annotated, nameSpace);

                //This part i modify
                this.listViewAttributes.Items.Clear();
                listAttributes.Reverse();
                foreach (XSDAttribute attribute in listAttributes)
                {
                    string s = "";
                    //dgis fix github issue 2 (attribute.Tag == null ???)
                    if (attribute.Tag != null && attribute.Tag.simpleType != null && attribute.Tag.simpleType.Item is XMLSchema.restriction)
                    {
                        XMLSchema.restriction r = attribute.Tag.simpleType.Item as XMLSchema.restriction;
                        if (r.Items != null)
                        {
                            for (int i = 0; i < r.Items.Length; i++)
                            {
                                s += r.ItemsElementName[i].ToString() + "(" + r.Items[i].id + " " + r.Items[i].value + ");";
                            }
                        }
                    }

                    this.listViewAttributes.Items.Add(new ListViewItem(new string[] { attribute.Name, attribute.Type, attribute.Use, attribute.DefaultValue, s })).Tag = attribute;
                }
                //Adrian--
			}
		}

		private void ShowEnumerate(XMLSchema.attribute attribute)
		{
			this.listViewEnumerate.Items.Clear();
			if (attribute != null)
			{
				if (attribute.type != null)
				{
                    //XSDObject xsdObject = this.schema.ElementsByName[DiagramHelpers.QualifiedNameToFullName("type", attribute.type)] as XSDObject;
                    //if (xsdObject != null)
                    XSDObject xsdObject;
                    if (this.schema.ElementsByName.TryGetValue(DiagramHelpers.QualifiedNameToFullName("type", attribute.type), out xsdObject) && xsdObject != null)
                    {
						XMLSchema.annotated annotatedElement = xsdObject.Tag as XMLSchema.annotated;
						if (annotatedElement is XMLSchema.simpleType)
							ShowEnumerate(annotatedElement as XMLSchema.simpleType);
					}
				}
				else if (attribute.simpleType != null)
				{
					ShowEnumerate(attribute.simpleType);
				}
			}
		}

		private void ShowEnumerate(XMLSchema.annotated annotated)
		{
			this.listViewEnumerate.Items.Clear();

			if (annotated != null)
			{
				XMLSchema.element element = annotated as XMLSchema.element;
				if (element != null && element.type != null)
				{
                    //XSDObject xsdObject = this.schema.ElementsByName[DiagramHelpers.QualifiedNameToFullName("type", element.type)] as XSDObject;
                    //if (xsdObject != null)
                    XSDObject xsdObject;
                    if (this.schema.ElementsByName.TryGetValue(DiagramHelpers.QualifiedNameToFullName("type", element.type), out xsdObject) && xsdObject != null)
                    {
						XMLSchema.annotated annotatedElement = xsdObject.Tag as XMLSchema.annotated;
						if (annotatedElement is XMLSchema.simpleType)
							ShowEnumerate(annotatedElement as XMLSchema.simpleType);
					}
				}
			}
		}

		private void ShowEnumerate(XMLSchema.simpleType simpleType)
		{
			if (simpleType != null)
			{
				if (simpleType.Item != null)
				{
					XMLSchema.restriction restriction = simpleType.Item as XMLSchema.restriction;
					if (restriction != null && restriction.ItemsElementName != null)
					{
						for (int i = 0; i < restriction.ItemsElementName.Length; i++)
						{
							if (restriction.ItemsElementName[i] == XMLSchema.ItemsChoiceType.enumeration)
							{
								XMLSchema.facet facet = restriction.Items[i] as XMLSchema.facet;
								if (facet != null)
                                    this.listViewEnumerate.Items.Add(facet.value).Tag = facet;
							}
						}

						if (this.listViewEnumerate.Items.Count != 0)
							this.listViewEnumerate.Columns[0].Width = -1;
					}
				}
			}
		}


		private void ShowDocumentation(XMLSchema.annotation annotation)
		{
            if (this.textBoxAnnotation == null)
            {
                // 
                // webBrowserDocumentation
                // 
                if(webBrowserSupported)
                {
                    this.webBrowserDocumentation = new System.Windows.Forms.WebBrowser();
                    this.webBrowserDocumentation.Dock = System.Windows.Forms.DockStyle.Fill;
                    this.webBrowserDocumentation.Location = new System.Drawing.Point(0, 0);
                    this.webBrowserDocumentation.MinimumSize = new System.Drawing.Size(20, 20);
                    this.webBrowserDocumentation.Name = "webBrowserDocumentation";
                    this.webBrowserDocumentation.Size = new System.Drawing.Size(214, 117);
                    this.webBrowserDocumentation.TabIndex = 1;
                    this.splitContainerDiagramElement.Panel2.Controls.Add(this.webBrowserDocumentation);
                }
                else
                    this.webBrowserDocumentation = null;
                // 
                // textBoxAnnotation
                // 
                this.textBoxAnnotation = new System.Windows.Forms.TextBox();
                this.textBoxAnnotation.Dock = System.Windows.Forms.DockStyle.Fill;
                this.textBoxAnnotation.Location = new System.Drawing.Point(0, 0);
                this.textBoxAnnotation.Multiline = true;
                this.textBoxAnnotation.Name = "textBoxAnnotation";
                this.textBoxAnnotation.ReadOnly = true;
                this.textBoxAnnotation.ScrollBars = System.Windows.Forms.ScrollBars.Vertical;
                this.textBoxAnnotation.Size = new System.Drawing.Size(214, 117);
                this.textBoxAnnotation.TabIndex = 0;
                this.splitContainerDiagramElement.Panel2.Controls.Add(this.textBoxAnnotation);
            }
			if (annotation == null)
			{
				this.textBoxAnnotation.Text = "";
				this.textBoxAnnotation.Visible = true;
                if (this.webBrowserDocumentation != null)
    				this.webBrowserDocumentation.Visible = false;

				return;
			}

            bool isWebDocumentation = false;
            Uri uriResult;
            foreach (object o in annotation.Items)
            {
                if (o is XMLSchema.documentation)
                {
                    XMLSchema.documentation documentation = o as XMLSchema.documentation;
                    if (documentation.Any != null && documentation.Any.Length > 0 && documentation.Any[0].Value != null)
                    {
                    }
                    else if (documentation.source != null && Uri.TryCreate(documentation.source, UriKind.Absolute, out uriResult) && (uriResult.Scheme == Uri.UriSchemeHttp || uriResult.Scheme == Uri.UriSchemeHttps))
                    {
                        if (this.webBrowserDocumentation != null)
                        {
                            isWebDocumentation = true;
                            this.textBoxAnnotation.Visible = false;
                            this.webBrowserDocumentation.Visible = true;
                            this.webBrowserDocumentation.Navigate(documentation.source);
                        }
                    }
                    break;
                }
            }

            if(!isWebDocumentation)
            {
                this.textBoxAnnotation.Text = DiagramHelpers.GetAnnotationText(annotation);
                this.textBoxAnnotation.Visible = true;
                if (this.webBrowserDocumentation != null)
                    this.webBrowserDocumentation.Visible = false;
            }
		}

		private void listViewAttributes_SelectedIndexChanged(object sender, EventArgs e)
		{
			if (this.listViewAttributes.SelectedItems.Count > 0)
			{
				XSDAttribute xsdAttribute = this.listViewAttributes.SelectedItems[0].Tag as XSDAttribute;
				XMLSchema.attribute attribute = xsdAttribute.Tag;
				if (attribute != null && attribute.annotation != null)
					ShowDocumentation(attribute.annotation);
				else
					ShowDocumentation(null);
				ShowEnumerate(attribute);
			}
		}

        private void listViewEnumerate_SelectedIndexChanged(object sender, EventArgs e)
		{
            if (this.listViewEnumerate.SelectedItems.Count > 0)
			{
				XMLSchema.facet facet = this.listViewEnumerate.SelectedItems[0].Tag as XMLSchema.facet;
				if (facet != null && facet.annotation != null)
					ShowDocumentation(facet.annotation);
				else
					ShowDocumentation(null);
			}
		}

		private void toolStripComboBoxZoom_SelectedIndexChanged(object sender, EventArgs e)
		{
			try
			{
				string zoomString = this.toolStripComboBoxZoom.SelectedItem as string;
				zoomString = zoomString.Replace("%", "");

				float zoom = (float)int.Parse(zoomString) / 100.0f;
				if (zoom >= 0.10 && zoom <= 10)
				{
					//Point virtualCenter = this.panelDiagram.VirtualPoint;
					//virtualCenter.Offset(this.panelDiagram.DiagramControl.Width / 2, this.panelDiagram.DiagramControl.Height / 2);
					//Size oldSize = this.panelDiagram.VirtualSize;
					//this.diagram.Scale = zoom;
					//UpdateDiagram();
					//Size newSize = this.panelDiagram.VirtualSize;
					//virtualCenter.X = (int)((float)newSize.Width / (float)oldSize.Width * (float)virtualCenter.X);
					//virtualCenter.Y = (int)((float)newSize.Height / (float)oldSize.Height * (float)virtualCenter.Y);
					//if (virtualCenter.X > this.diagram.BoundingBox.Right)
					//    virtualCenter.X = this.diagram.BoundingBox.Right;
					//if (virtualCenter.Y > this.diagram.BoundingBox.Bottom)
					//    virtualCenter.Y = this.diagram.BoundingBox.Bottom;
					//this.panelDiagram.ScrollTo(virtualCenter, true);

					Point virtualCenter = this.panelDiagram.VirtualPoint;
					virtualCenter.Offset(this.panelDiagram.DiagramControl.Width / 2, this.panelDiagram.DiagramControl.Height / 2);
					Size oldSize = this.panelDiagram.VirtualSize;
					this.diagram.Scale = zoom;
					UpdateDiagram();
					Size newSize = this.panelDiagram.VirtualSize;
					Point newVirtualCenter = new Point();
					newVirtualCenter.X = (int)((float)newSize.Width / (float)oldSize.Width * (float)virtualCenter.X);
					newVirtualCenter.Y = (int)((float)newSize.Height / (float)oldSize.Height * (float)virtualCenter.Y);
					if (newVirtualCenter.X > this.diagram.BoundingBox.Right)
						newVirtualCenter.X = this.diagram.BoundingBox.Right;
					if (newVirtualCenter.Y > this.diagram.BoundingBox.Bottom)
						newVirtualCenter.Y = this.diagram.BoundingBox.Bottom;
					this.panelDiagram.ScrollTo(newVirtualCenter, true);
				}
			}
			catch { }
		}

		private void toolStripComboBoxZoom_TextChanged(object sender, EventArgs e)
		{
			//try
			//{
			//    string zoomString = this.toolStripComboBoxZoom.SelectedItem as string;
			//    zoomString = zoomString.Replace("%", "");

			//    float zoom = (float)int.Parse(zoomString) / 100.0f;
			//    if (zoom >= 0.10 && zoom <= 10)
			//    {
			//        this.diagram.Scale = zoom;
			//        UpdateDiagram();
			//    }
			//}
			//catch { }
		}

		void DiagramControl_MouseWheel(object sender, MouseEventArgs e)
		{
			if ((ModifierKeys & Keys.Control) == Keys.Control)
			{
				if (e.Delta > 0)
				{
					if (this.toolStripComboBoxZoom.SelectedIndex < this.toolStripComboBoxZoom.Items.Count - 1)
						this.toolStripComboBoxZoom.SelectedIndex++;
				}
				else
				{
					if (this.toolStripComboBoxZoom.SelectedIndex > 0)
						this.toolStripComboBoxZoom.SelectedIndex--;
				}
			}
		}

		private void pageToolStripMenuItem_Click(object sender, EventArgs e)
		{
			try
			{
                if (_diagramPrinter == null)
                {
                    _diagramPrinter = new DiagramPrinter();
                }
                _diagramPrinter.Diagram = diagram;

                _diagramPrinter.PageSetup();
			}
			catch (Exception ex)
			{
				MessageBox.Show(ex.Message);
			}
		}

		private void printPreviewToolStripMenuItem_Click(object sender, EventArgs e)
		{
			try
			{
                if (_diagramPrinter == null)
                {
                    _diagramPrinter = new DiagramPrinter();
                }
                _diagramPrinter.Diagram = diagram;

                _diagramPrinter.PrintPreview();
			}
			catch (Exception ex)
			{
				MessageBox.Show(ex.Message);
			}
		}

		private void printToolStripMenuItem_Click(object sender, EventArgs e)
		{
			try
			{
                if (_diagramPrinter == null)
                {
                    _diagramPrinter = new DiagramPrinter();
                }
                _diagramPrinter.Diagram = diagram;

                _diagramPrinter.Print(true, Options.IsRunningOnMono);
			}
			catch (Exception ex)
			{
				MessageBox.Show(ex.Message);
			}
		}

		private void toolStripButtonTogglePanel_Click(object sender, EventArgs e)
		{
			this.splitContainerMain.Panel2Collapsed = !this.toolStripButtonTogglePanel.Checked;
		}

		private void contextMenuStripDiagram_Opened(object sender, EventArgs e)
		{
			this.gotoXSDFileToolStripMenuItem.Enabled = false;
            this.expandToolStripMenuItem.Enabled = false;
            this.removeFromDiagramToolStripMenuItem.Enabled = false;

			Point contextualMenuMousePosition = this.panelDiagram.DiagramControl.PointToClient(MousePosition);
			contextualMenuMousePosition.Offset(this.panelDiagram.VirtualPoint);
			DiagramItem resultElement;
			DiagramHitTestRegion resultRegion;
			this.diagram.HitTest(contextualMenuMousePosition, out resultElement, out resultRegion);
			if (resultRegion != DiagramHitTestRegion.None)
			{
				if (resultRegion == DiagramHitTestRegion.Element) // && resultElement.Parent == null)
				{
					this.contextualMenuPointedElement = resultElement;
                    this.gotoXSDFileToolStripMenuItem.Enabled = this.schema.ElementsByName.ContainsKey(this.contextualMenuPointedElement.FullName);
                    this.expandToolStripMenuItem.Enabled = true;
                    this.removeFromDiagramToolStripMenuItem.Enabled = true;
				}
			}
		}

		private void gotoXSDFileToolStripMenuItem_Click(object sender, EventArgs e)
		{
			if (this.contextualMenuPointedElement != null)
			{
				//XSDObject xsdObject = this.schema.ElementsByName[this.contextualMenuPointedElement.FullName] as XSDObject;
                //if (xsdObject != null)
				XSDObject xsdObject;
                if (this.schema.ElementsByName.TryGetValue(this.contextualMenuPointedElement.FullName, out xsdObject) && xsdObject != null)
				{
                    TabPage tabPage = null;
                    if (this.hashtableTabPageByFilename.TryGetValue(xsdObject.Filename, out tabPage) && tabPage != null)
						this.tabControlView.SelectedTab = tabPage;
				}
			}
			this.contextualMenuPointedElement = null;
		}

        private void expandToolStripMenuItem_Click(object sender, EventArgs e)
        {
            ExpandCollapseElement(this.contextualMenuPointedElement, false);
            this.contextualMenuPointedElement = null;
        }

        private void removeFromDiagramToolStripMenuItem_Click(object sender, EventArgs e)
		{
            RemoveElement(this.contextualMenuPointedElement);
            this.contextualMenuPointedElement = null;
		}

        private void RemoveElement(DiagramItem element)
        {
            DiagramItem parentDiagram = element.Parent;
            this.diagram.Remove(element);
            UpdateDiagram();
            if (parentDiagram != null)
                this.panelDiagram.ScrollTo(this.diagram.ScalePoint(parentDiagram.Location), true);
            else
                this.panelDiagram.ScrollTo(new Point(0, 0));
        }

        private void tabControlView_Selected(object sender, TabControlEventArgs e)
		{
			if (tabControlView.SelectedTab.Tag != null)
			{
				WebBrowser webBrowser = tabControlView.SelectedTab.Controls[0] as WebBrowser;
                if (webBrowser != null)
                {
                    string url = tabControlView.SelectedTab.Tag as string;
                    //if (webBrowser.Url == null || webBrowser.Url != new Uri(url))
                    if (webBrowser.Document == null)
                        webBrowser.Navigate(url);
                    webBrowser.Select();
                }
                else
                {
                    TextBox textBrowser = tabControlView.SelectedTab.Controls[0] as TextBox;
                    if (textBrowser != null)
                    {
                        string url = tabControlView.SelectedTab.Tag as string;
                        if (string.IsNullOrEmpty(textBrowser.Text))
                        {
                            try
                            {
                                //HttpWebRequest webRequestObject = (HttpWebRequest)WebRequest.Create(url);
                                ////WebRequestObject.UserAgent = ".NET Framework/2.0";
                                ////WebRequestObject.Referer = "http://www.example.com/";
                                //WebResponse response = webRequestObject.GetResponse();
                                //Stream webStream = response.GetResponseStream();
                                //StreamReader reader = new StreamReader(webStream);
                                //textBrowser.Text = reader.ReadToEnd();
                                //reader.Close();
                                //webStream.Close();
                                //response.Close();

                                WebClient client = new WebClient();
                                client.Headers.Add("user-agent", "Mozilla/4.0 (compatible; MSIE 6.0; Windows NT 5.2; .NET CLR 1.0.3705;)");
                                Stream data = client.OpenRead(url);
                                StreamReader reader = new StreamReader(data);
                                textBrowser.Text = reader.ReadToEnd().Replace("\r\n", "\n").Replace("\n", "\r\n");
                                data.Close();
                                reader.Close();
                            }
                            catch (Exception ex)
                            {
                                textBrowser.Text = ex.Message;
                            }
                        }
                        textBrowser.Select();
                    }
                }
			}
		}

		private void tabControlView_Click(object sender, EventArgs e)
		{
			if (tabControlView.SelectedTab.Tag != null)
			{
                Control webBrowser = tabControlView.SelectedTab.Controls[0] as Control;
				if (webBrowser != null)
					webBrowser.Select();
			}
		}

		private void tabControlView_Enter(object sender, EventArgs e)
		{
            if (tabControlView.SelectedTab != null && tabControlView.SelectedTab.Tag != null)
			{
                Control webBrowser = tabControlView.SelectedTab.Controls[0] as Control;
				if (webBrowser != null)
					webBrowser.Focus();
			}
			else
				this.panelDiagram.Focus();
		}

		private void registerToolStripMenuItem_Click(object sender, EventArgs e)
		{
			FileShellExtension.Register(Microsoft.Win32.Registry.GetValue("HKEY_CLASSES_ROOT\\.xsd", null, "xsdfile") as string, "XSDDiagram", "XSD Diagram", string.Format("\"{0}\" \"%L\"", Application.ExecutablePath));
		}

		private void unregisterToolStripMenuItem_Click(object sender, EventArgs e)
		{
			FileShellExtension.Unregister(Microsoft.Win32.Registry.GetValue("HKEY_CLASSES_ROOT\\.xsd", null, "xsdfile") as string, "XSDDiagram");
		}

		private void toolStripButtonShowReferenceBoundingBox_Click(object sender, EventArgs e)
		{
			this.diagram.ShowBoundingBox = this.toolStripButtonShowReferenceBoundingBox.Checked;
			UpdateDiagram();
		}

		private void toolStripComboBoxAlignement_SelectedIndexChanged(object sender, EventArgs e)
		{
			switch (this.toolStripComboBoxAlignement.SelectedItem as string)
			{
				case "Top": this.diagram.Alignement = DiagramAlignement.Near; break;
				case "Center": this.diagram.Alignement = DiagramAlignement.Center; break;
				case "Bottom": this.diagram.Alignement = DiagramAlignement.Far; break;
			}
			UpdateDiagram();
		}

		void diagram_RequestAnyElement(DiagramItem diagramElement, out XMLSchema.element element, out string nameSpace)
		{
			element = null;
			nameSpace = "";

			//ElementsForm elementsForm = new ElementsForm();
			//elementsForm.Location = MousePosition; //diagramElement.Location //MousePosition;
			//elementsForm.ListBoxElements.Items.Clear();
			//elementsForm.ListBoxElements.Items.Insert(0, "(Cancel)");
			//foreach (XSDObject xsdObject in this.schema.ElementsByName.Values)
			//    if (xsdObject != null && xsdObject.Type == "element")
			//        elementsForm.ListBoxElements.Items.Add(xsdObject);
			//if (elementsForm.ShowDialog(this.diagramControl) == DialogResult.OK && (elementsForm.ListBoxElements.SelectedItem as XSDObject) != null)
			//{
			//    XSDObject xsdObject = elementsForm.ListBoxElements.SelectedItem as XSDObject;
			//    element = xsdObject.Tag as XMLSchema.element;
			//    nameSpace = xsdObject.NameSpace;
			//}
		}

		private void listViewElement_Click(object sender, EventArgs e)
		{
			if (this.listViewElements.SelectedItems.Count > 0)
				SelectSchemaElement(this.listViewElements.SelectedItems[0].Tag as XSDObject);
		}

		private void listViewElement_DoubleClick(object sender, EventArgs e)
		{
			if (this.listViewElements.SelectedItems.Count > 0)
			{
                DiagramItem firstDiagramItem = null;

                foreach (ListViewItem lvi in this.listViewElements.SelectedItems)
				{
					XSDObject xsdObject = lvi.Tag as XSDObject;
                    DiagramItem diagramItem = this.diagram.Add(xsdObject.Tag as XMLSchema.openAttrs, xsdObject.NameSpace);
                    if (firstDiagramItem == null && diagramItem != null)
                        firstDiagramItem = diagramItem;

                    //switch (xsdObject.Type)
                    //{
                    //    case "element":
                    //        this.diagram.AddElement(xsdObject.Tag as XMLSchema.element, xsdObject.NameSpace);
                    //        break;
                    //    case "group":
                    //        this.diagram.AddCompositors(xsdObject.Tag as XMLSchema.group, xsdObject.NameSpace);
                    //        break;
                    //    case "complexType":
                    //        this.diagram.AddComplexType(xsdObject.Tag as XMLSchema.complexType, xsdObject.NameSpace);
                    //        break;
                    //    case "simpleType":
                    //        this.diagram.Add(xsdObject.Tag as XMLSchema.simpleType, xsdObject.NameSpace);
                    //        break;
                    //}
                }
                if (firstDiagramItem != null)
                    SelectDiagramElement(firstDiagramItem, true);
                else
    				UpdateDiagram();
			}
		}

		private void expandOneLevelToolStripMenuItem_Click(object sender, EventArgs e)
		{
			this.diagram.ExpandOneLevel();
			UpdateDiagram();
		}

		// Implements the manual sorting of items by columns.
		class ListViewItemComparer : IComparer
		{
			private int column;
			private ListView listView;
			public ListViewItemComparer(int column, ListView listView)
			{
				this.column = column;
				this.listView = listView;

				switch (this.listView.Sorting)
				{
					case SortOrder.None: this.listView.Sorting = SortOrder.Ascending; break;
					case SortOrder.Ascending: this.listView.Sorting = SortOrder.Descending; break;
					case SortOrder.Descending: this.listView.Sorting = SortOrder.Ascending; break;
				}
			}
			public int Compare(object x, object y)
			{
				int result = 0;
				if (this.listView.Sorting == SortOrder.Ascending)
					result = String.Compare(((ListViewItem)x).SubItems[this.column].Text, ((ListViewItem)y).SubItems[column].Text);
				if (this.listView.Sorting == SortOrder.Descending)
					result = -String.Compare(((ListViewItem)x).SubItems[this.column].Text, ((ListViewItem)y).SubItems[column].Text);

				return result;
			}
		}

		private void listViewElement_ColumnClick(object sender, ColumnClickEventArgs e)
		{
			this.listViewElements.ListViewItemSorter = new ListViewItemComparer(e.Column, this.listViewElements);
		}

		private void listViewAttributes_ColumnClick(object sender, ColumnClickEventArgs e)
		{
			this.listViewAttributes.ListViewItemSorter = new ListViewItemComparer(e.Column, this.listViewAttributes);
		}

		private void toolStripButtonRemoveAllFromDiagram_Click(object sender, EventArgs e)
		{
            DialogResult dialogResult = MessageBox.Show("Are you sure to remove everything?", "Remove All", MessageBoxButtons.YesNo);
            if (dialogResult == DialogResult.Yes)
            {
                this.diagram.RemoveAll();
                UpdateDiagram();
                this.panelDiagram.VirtualPoint = new Point(0, 0);
                this.panelDiagram.Clear();
            }
        }

		private void listView_AfterLabelEdit(object sender, LabelEditEventArgs e)
		{
			e.CancelEdit = true;
		}

		private void nextTabToolStripMenuItem_Click(object sender, EventArgs e)
		{
			int index = this.tabControlView.SelectedIndex;
			++index;
			this.tabControlView.SelectedIndex = index % this.tabControlView.TabCount;
		}

		private void previousTabToolStripMenuItem_Click(object sender, EventArgs e)
		{
			int index = this.tabControlView.SelectedIndex;
			--index;
			if (index < 0) index = this.tabControlView.TabCount - 1;
			this.tabControlView.SelectedIndex = index;
		}

		private void ListViewToString(ListView listView, bool selectedLineOnly)
		{
			string result = "";
			if (selectedLineOnly)
			{
				if (listView.SelectedItems.Count > 0)
				{
					foreach (ColumnHeader columnHeader in listView.Columns)
					{
						if (columnHeader.Index > 0) result += "\t";
						result += listView.SelectedItems[0].SubItems[columnHeader.Index].Text;
					}
				}
			}
			else
			{
				foreach (ListViewItem lvi in listView.Items)
				{
					foreach (ColumnHeader columnHeader in listView.Columns)
					{
						if (columnHeader.Index > 0) result += "\t";
						result += lvi.SubItems[columnHeader.Index].Text;
					}
					result += "\r\n";
				}
			}
			if (result.Length > 0)
				Clipboard.SetText(result);
		}

		private void toolStripMenuItemAttributesCopyLine_Click(object sender, EventArgs e)
		{
			ListViewToString(this.listViewAttributes, true);
		}

		private void toolStripMenuItemAttributesCopyList_Click(object sender, EventArgs e)
		{
			ListViewToString(this.listViewAttributes, false);
		}

		private void contextMenuStripAttributes_Opening(object sender, CancelEventArgs e)
		{
			this.toolStripMenuItemAttributesCopyLine.Enabled = (this.listViewAttributes.SelectedItems.Count == 1);
			this.toolStripMenuItemAttributesCopyList.Enabled = (this.listViewAttributes.Items.Count > 0);
		}

		private void toolStripMenuItemEnumerateCopyLine_Click(object sender, EventArgs e)
		{
			ListViewToString(this.listViewEnumerate, true);
		}

		private void toolStripMenuItemEnumerateCopyList_Click(object sender, EventArgs e)
		{
			ListViewToString(this.listViewEnumerate, false);
		}

		private void contextMenuStripEnumerate_Opening(object sender, CancelEventArgs e)
		{
			this.toolStripMenuItemEnumerateCopyLine.Enabled = (this.listViewEnumerate.SelectedItems.Count == 1);
			this.toolStripMenuItemEnumerateCopyList.Enabled = (this.listViewEnumerate.Items.Count > 0);
		}

		private void toolStripMenuItemElementsCopyLine_Click(object sender, EventArgs e)
		{
			ListViewToString(this.listViewElements, true);
		}

		private void toolStripMenuItemElementsCopyList_Click(object sender, EventArgs e)
		{
			ListViewToString(this.listViewElements, false);
		}

		private void contextMenuStripElements_Opening(object sender, CancelEventArgs e)
		{
			this.toolStripMenuItemElementsCopyLine.Enabled = (this.listViewElements.SelectedItems.Count == 1);
			this.toolStripMenuItemElementsCopyList.Enabled = (this.listViewElements.Items.Count > 0);
		}

		private void listViewElements_ItemDrag(object sender, ItemDragEventArgs e)
		{
			listViewElements.DoDragDrop(e.Item, DragDropEffects.Copy);
		}

		private void panelDiagram_DragEnter(object sender, DragEventArgs e)
		{
			if (e.Data.GetDataPresent(typeof(ListViewItem)))
			{
				ListViewItem lvi = e.Data.GetData(typeof(ListViewItem)) as ListViewItem;
				if (lvi != null)
				{
					XSDObject xsdObject = lvi.Tag as XSDObject;
					switch (xsdObject.Type)
					{
						case "element":
						case "group":
						case "complexType":
							e.Effect = DragDropEffects.Copy;
							break;
					}
				}
			}
			else if (e.Data.GetDataPresent(DataFormats.FileDrop))
				e.Effect = DragDropEffects.Move;
		}

		private void panelDiagram_DragDrop(object sender, DragEventArgs e)
		{
			if (e.Data.GetDataPresent(typeof(ListViewItem)))
			{
				ListViewItem lvi = e.Data.GetData(typeof(ListViewItem)) as ListViewItem;
				if (lvi != null)
				{
					listViewElement_DoubleClick(sender, e);
				}
			}
			else if (e.Data.GetDataPresent(DataFormats.FileDrop))
				MainForm_DragDrop(sender, e);
		}

		void DiagramControl_MouseMove(object sender, MouseEventArgs e)
		{
			//toolTip.Show("Coucou", panelDiagram.DiagramControl, 200);
		}

		void DiagramControl_MouseHover(object sender, EventArgs e)
		{
		}

        //private void toolTip_Popup(object sender, PopupEventArgs e)
        //{
        //    //toolTip.SetToolTip(e.AssociatedControl, "AAAAAAAAAA");
        //}

        private void DiagramControl_KeyDown(object sender, KeyEventArgs e)
        {
            if (this.diagram.RootElements.Count > 0)
            {
                if (e.KeyCode == Keys.Space || e.KeyCode == Keys.Return || e.KeyCode == Keys.Enter)
                    ExpandCollapseElement(this.diagram.SelectedElement, true);
                if (e.KeyCode == Keys.Delete)
                {
                    DiagramItem parentElement = this.diagram.SelectedElement.Parent;
                    RemoveElement(this.diagram.SelectedElement);
                    if (parentElement != null)
                        SelectDiagramElement(this.diagram.SelectedElement.Parent, true);
                    else
                        SelectDiagramElement(null);
                }
                else if (e.KeyCode == Keys.Right || e.KeyCode == Keys.Left
                    || e.KeyCode == Keys.Down || e.KeyCode == Keys.Up
                    || e.KeyCode == Keys.PageDown || e.KeyCode == Keys.PageUp
                    || e.KeyCode == Keys.Home || e.KeyCode == Keys.End
                    )
                {
                    DiagramItem element = this.diagram.SelectedElement;
                    if (element == null)
                        SelectDiagramElement(this.diagram.RootElements[0], true);
                    else
                    {
                        switch (e.KeyCode)
                        {
                            case Keys.Right:
                                if (element.HasChildElements)
                                {
                                    if (element.ChildElements.Count == 0)
                                        this.diagram.ExpandChildren(element);
                                    element.ShowChildElements = true;
                                    SelectDiagramElement(element.ChildElements[0], true);
                                }
                                break;
                            case Keys.Left:
                                if (element.Parent != null)
                                    SelectDiagramElement(element.Parent, true);
                                break;
                            case Keys.Down:
                                {
                                    IList<DiagramItem> children = element.Parent == null ? this.diagram.RootElements : element.Parent.ChildElements;
                                    if (children != null)
                                    {
                                        var pos = children.IndexOf(element);
                                        if (pos + 1 < children.Count)
                                            SelectDiagramElement(children[pos + 1], true);
                                    }
                                }
                                break;
                            case Keys.Up:
                                {
                                    IList<DiagramItem> children = element.Parent == null ? this.diagram.RootElements : element.Parent.ChildElements;
                                    if (children != null)
                                    {
                                        var pos = children.IndexOf(element);
                                        if (pos - 1 >= 0)
                                            SelectDiagramElement(children[pos - 1], true);
                                    }
                                }
                                break;
                        }
                    }
                }
            }
        }

		private void ExpandCollapseElement(DiagramItem element)
		{
			ExpandCollapseElement(element, false);
		}

        private void ExpandCollapseElement(DiagramItem element, bool scrollToElement)
        {
            if (element != null && element.HasChildElements)
            {
                if (element.ChildElements.Count == 0)
                {
                    this.diagram.ExpandChildren(element);
                    element.ShowChildElements = true;
                }
                else
                {
                    this.diagram.ClearSearch();
                    element.ShowChildElements ^= true;
                }
                UpdateDiagram();
                this.panelDiagram.ScrollTo(this.diagram.ScalePoint(element.Location), true);
            }
        }

        private void toolTip_Draw(object sender, DrawToolTipEventArgs e)
		{
			Point diagramMousePosition = e.AssociatedControl.PointToClient(MousePosition);
			string text = string.Format("AAAA {0} {1}\nA Que\n\nCoucou", diagramMousePosition.X, diagramMousePosition.Y);

			Size textSize = TextRenderer.MeasureText(text, e.Font);
			Rectangle newBound = new Rectangle(e.Bounds.X + 20, e.Bounds.Y - 20, textSize.Width + 10, textSize.Height + 10);

			DrawToolTipEventArgs newArgs = new DrawToolTipEventArgs(e.Graphics,
				e.AssociatedWindow, e.AssociatedControl, newBound, text,
				this.BackColor, this.ForeColor, e.Font);
			newArgs.DrawBackground();
			newArgs.DrawBorder();
			newArgs.DrawText(TextFormatFlags.TextBoxControl);

			//e.DrawBackground();
			//e.DrawBorder();
			//using (StringFormat sf = new StringFormat())
			//{
			//    sf.Alignment = StringAlignment.Center;
			//    sf.LineAlignment = StringAlignment.Center;
			//    sf.HotkeyPrefix = System.Drawing.Text.HotkeyPrefix.None;
			//    sf.FormatFlags = StringFormatFlags.NoWrap;
			//    using (Font f = new Font("Tahoma", 9))
			//    {
			//        e.Graphics.DrawString(text, f,
			//            SystemBrushes.ActiveCaptionText, e.Bounds, sf);
			//    }
			//}
			//e.DrawText();
		}

        private void validateXMLFileToolStripMenuItem_Click(object sender, EventArgs e)
        {
            OpenFileDialog openFileDialog = new OpenFileDialog();
            openFileDialog.Filter = "xml files (*.xml)|*.xml|All files (*.*)|*.*";
            openFileDialog.FilterIndex = 2;
            openFileDialog.RestoreDirectory = true;
            if (openFileDialog.ShowDialog() == DialogResult.OK)
            {
                try
                {
                    Cursor = Cursors.WaitCursor;

                    validationErrorMessages.Clear();

                    StreamReader streamReader = new StreamReader(openFileDialog.FileName);
                    string xmlSource = streamReader.ReadToEnd();
                    streamReader.Close();

					//XmlDocument x = new XmlDocument();
					//x.LoadXml(xmlSource);

                    XmlReaderSettings settings = new XmlReaderSettings();
                    settings.CloseInput = true;
                    settings.ValidationType = ValidationType.Schema;
                    settings.ProhibitDtd = false;
					settings.XmlResolver = null;
                    settings.ValidationEventHandler += new ValidationEventHandler(ValidationHandler);
                    settings.ValidationFlags = XmlSchemaValidationFlags.ReportValidationWarnings |
                                                XmlSchemaValidationFlags.ProcessIdentityConstraints |
                                                XmlSchemaValidationFlags.ProcessInlineSchema |
                                                XmlSchemaValidationFlags.ProcessSchemaLocation
												; //| XmlSchemaValidationFlags.AllowXmlAttributes;
                    //settings.Schemas.Add("http://www.collada.org/2005/11/COLLADASchema", currentLoadedSchemaFilename);
                    //settings.Schemas.Add(null, currentLoadedSchemaFilename); // = sc;
                    List<string> schemas = new List<string>(schema.XsdFilenames);
                    schemas.Reverse();
					foreach (string schemaFilename in schemas)
					{
						try
						{
							settings.Schemas.Add(null, schemaFilename);
						}
						catch (Exception ex)
						{
							validationErrorMessages.Add(string.Format("Error while parsing {0}, Message: {1}",
								schemaFilename, ex.Message));
						}
					}

                    StringReader r = new StringReader(xmlSource);
                    using (XmlReader validatingReader = XmlReader.Create(r, settings))
                    {
                        while (validatingReader.Read()) { /* just loop through document */ }
                    }

                    Cursor = Cursors.Default;

                    ErrorReportForm errorReportForm = new ErrorReportForm();
                    errorReportForm.Errors = validationErrorMessages;
                    errorReportForm.ShowDialog(this);
                }
                catch (Exception ex)
                {
					Cursor = Cursors.Default;

					validationErrorMessages.Add(string.Format("Error while validating {0}, Message: {1}",
						openFileDialog.FileName, ex.Message));
					//MessageBox.Show("Cannot validate: " + ex.Message);
					ErrorReportForm errorReportForm = new ErrorReportForm();
					errorReportForm.Errors = validationErrorMessages;
					errorReportForm.ShowDialog(this);
                }
				Cursor = Cursors.Default;

				if (validationErrorMessages.Count == 0)
					MessageBox.Show("No issue found");
			}
        }

        static List<string> validationErrorMessages = new List<string>();

        public static void ValidationHandler(object sender, ValidationEventArgs e)
        {
            //if (e.Severity == XmlSeverityType.Error || e.Severity == XmlSeverityType.Warning)
            validationErrorMessages.Add(string.Format("{4}: [{3}] Line: {0}, Position: {1} \"{2}\"",
                e.Exception.LineNumber, e.Exception.LinePosition, e.Exception.Message, validationErrorMessages.Count, e.Severity));
        }

        private void generateSampleToolStripMenuItem_Click(object sender, EventArgs e)
        {
        }

        private void inferXSDFromXMLToolStripMenuItem_Click(object sender, EventArgs e)
        {
            OpenFileDialog openFileDialog = new OpenFileDialog();
            openFileDialog.Filter = "xml files (*.xml)|*.xml|All files (*.*)|*.*";
            openFileDialog.FilterIndex = 2;
            openFileDialog.RestoreDirectory = true;
            if (openFileDialog.ShowDialog() == DialogResult.OK)
            {
                try
                {
                    Cursor = Cursors.WaitCursor;

                    //https://msdn.microsoft.com/en-us/library/system.xml.schema.xmlschemainference.aspx
                    XmlReader reader = XmlReader.Create(openFileDialog.FileName);
                    XmlSchemaSet schemaSet = new XmlSchemaSet();
                    XmlSchemaInference schema = new XmlSchemaInference();

                    schemaSet = schema.InferSchema(reader);


                    SaveFileDialog saveFileDialog = new SaveFileDialog();
                    saveFileDialog.Filter = "xsd files (*.xsd) | *.xsd | All files(*.*) | *.* ";
                    saveFileDialog.FilterIndex = 0;
                    saveFileDialog.RestoreDirectory = true;
                    if (saveFileDialog.ShowDialog() == DialogResult.OK)
                    {
                        string outputFilename = saveFileDialog.FileName;
                        using (TextWriter textWriter = File.CreateText(outputFilename))
                        {
                            foreach (XmlSchema xmlSchema in schemaSet.Schemas())
                            {
                                xmlSchema.Write(textWriter);
                            }
                        }

                        if(MessageBox.Show(this, "Would you open the newly inferred XSD file?", "Open XSD file", MessageBoxButtons.YesNo) == DialogResult.Yes)
                        {
                            LoadSchema(outputFilename);
                        }
                    }
                }
                catch (Exception ex)
                {
                    MessageBox.Show(ex.Message);
                }
                Cursor = Cursors.Default;
            }
        }


        private void toolStripButtonShowDocumentation_Click(object sender, EventArgs e)
        {
            this.diagram.ShowDocumentation = this.toolStripButtonShowDocumentation.Checked;
            UpdateDiagram();
        }

        private void toolStripButtonSearch_Click(object sender, EventArgs e)
        {
            Search();
        }

        private void toolStripTextBoxSearch_KeyPress(object sender, KeyPressEventArgs e)
        {
            if ((int)e.KeyChar == (int)Keys.Enter)
            {
                Search();
                e.Handled = true;
            }
        }


        private void Search()
        {
            var searchedItem = this.diagram.Search(toolStripTextBoxSearch.Text);
            if (searchedItem != null)
            {
                SelectDiagramElement(searchedItem);
                UpdateDiagram();
                this.panelDiagram.ScrollTo(this.diagram.ScalePoint(searchedItem.Location), true);
                this.toolStripStatusLabel1.Text = "Search: '" + toolStripTextBoxSearch.Text + "' (" + this.diagram.ActualSearchHit + " of " + this.diagram.SearchHits + " hits)";
            }
            else
            {
                this.toolStripStatusLabel1.Text = "Find: Can't find the text '" + toolStripTextBoxSearch.Text + "'";
            }
        }


        private void MainForm_KeyUp(object sender, KeyEventArgs e)
		{
			if (e.Control && (e.KeyCode == Keys.D0 || e.KeyCode == Keys.NumPad0))
			{
				this.toolStripComboBoxZoom.SelectedIndex = 8;
			}
            else if (e.Control && (e.KeyCode == Keys.F))
            {
                this.toolStripTextBoxSearch.SelectAll();
                this.toolStripTextBoxSearch.Focus();
            }
		}



		//void DiagramControl_MouseMove(object sender, MouseEventArgs e)
		//{
		//System.Diagnostics.Trace.WriteLine("toolTipDiagramElement_Popup");
		//Point contextualMenuMousePosition = this.panelDiagram.DiagramControl.PointToClient(MousePosition);
		//contextualMenuMousePosition.Offset(this.panelDiagram.VirtualPoint);
		//DiagramBase resultElement;
		//DiagramBase.HitTestRegion resultRegion;
		//this.diagram.HitTest(contextualMenuMousePosition, out resultElement, out resultRegion);
		//if (resultRegion != DiagramBase.HitTestRegion.None)
		//{
		//    if (resultRegion == DiagramBase.HitTestRegion.Element) // && resultElement.Parent == null)
		//    {
		//        //this.contextualMenuPointedElement = resultElement;
		//        //toolTipDiagramElement.SetToolTip(this.panelDiagram.DiagramControl, "coucou");
		//        e.Cancel = true;
		//        toolTipElement.Show("Coucou", this);
		//    }
		//}
		//}
	}
}