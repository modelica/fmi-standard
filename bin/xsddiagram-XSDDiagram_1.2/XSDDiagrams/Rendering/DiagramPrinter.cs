//    XSDDiagram - A XML Schema Definition file viewer
//    Copyright (C) 2006-2011  Regis COSNIER
//    
//    The content of this file is subject to the terms of either
//    the GNU Lesser General Public License only (LGPL) or
//    the Microsoft Public License (Ms-PL).
//    Please see LICENSE-LGPL.txt and LICENSE-MS-PL.txt files for details.
//
//    Authors:
//      Regis Cosnier (Initial developer)
//      Paul Selormey (Refactoring)

using System;
using System.Windows.Forms;
using System.ComponentModel;

using System.Drawing;
using System.Drawing.Text;
using System.Drawing.Printing;
using System.Drawing.Drawing2D;

namespace XSDDiagram.Rendering
{
    /// <summary>
    /// This manages or controls the output of a displayed diagram to the printer.
    /// </summary>
    /// <remarks>
    /// <para>
    /// The <see cref="System.Drawing.Printing.PrintDocument"/>, required to
    /// print in a typical the Windows Forms applications, is automatically
    /// created and maintained by the instance of this class.
    /// </para>
    /// </remarks>
    public sealed class DiagramPrinter : IDisposable
    {
        #region Private Fields

        private int           _currentPage;

        private Diagram       _diagram;
        private PrintDocument _printDocument;

        #endregion

        #region Constructors and Destructor

        /// <summary>
        /// Initializes a new instance of the <see cref="DiagramPrinter"/> class 
        /// with the specified diagram engine.
        /// </summary>
        /// <param name="chartEngine">The diagram engine.</param>
        public DiagramPrinter()
        {
            _printDocument              = new PrintDocument();

            _printDocument.DocumentName = "XSD Document";

            _printDocument.BeginPrint += new PrintEventHandler(OnBeginPrint);
            _printDocument.PrintPage  += new PrintPageEventHandler(OnPrintPage);
            _printDocument.EndPrint   += new PrintEventHandler(OnEndPrint);
        }

        ~DiagramPrinter()
        {
            this.Dispose(false);
        }

        #endregion

        #region Public Properties

        /// <summary>
        /// Gets or sets the diagram to be printed.
        /// </summary>
        public Diagram Diagram
        {
            get
            {
                return _diagram;
            }
            set
            {
                _diagram = value;
            }
        }

        /// <summary>
        /// Gets or sets the document name to display (for example, in a print 
        /// status dialog box or printer queue) while printing the diagram document.
        /// </summary>
        /// <value>
        /// A <see cref="System.String"/> containing the document name to 
        /// display while printing the diagram document. The default is 
        /// "GowChart Document".
        /// </value>
        /// <seealso cref="PrintDocument.DocumentName"/>
        public string DocumentName
        {
            get
            {
                if (_printDocument != null)
                {
                    return _printDocument.DocumentName;
                }

                return null;
            }

            set
            {
                if (_printDocument != null)
                {
                    _printDocument.DocumentName = value;
                }
            }
        }

        /// <summary>
        /// Gets or sets page settings that are used as defaults for all pages 
        /// to be printed.
        /// </summary>
        /// <value>
        /// A <see cref="System.Drawing.Printing.PageSettings"/> that specifies 
        /// the default page settings for the document.
        /// </value>
        /// <seealso cref="PrintDocument.DefaultPageSettings"/>
        [Browsable(false)]
        public PageSettings PageSettings
        {
            get
            {
                if (_printDocument != null)
                {
                    return _printDocument.DefaultPageSettings;
                }

                return null;
            }

            set
            {
                if (_printDocument != null)
                {
                    _printDocument.DefaultPageSettings = value;
                }
            }
        }

        /// <summary>
        /// Gets or sets the printer that prints the diagram document.
        /// </summary>
        /// <value>
        /// A <see cref="System.Drawing.Printing.PrinterSettings"/> that 
        /// specifies where and how the document is printed. The default is 
        /// a <see cref="System.Drawing.Printing.PrinterSettings"/> with its 
        /// properties set to their default values.
        /// </value>
        /// <seealso cref="PrintDocument.PrinterSettings"/>
        [Browsable(false)]
        public PrinterSettings PrinterSettings
        {
            get
            {
                if (_printDocument != null)
                {
                    return _printDocument.PrinterSettings;
                }

                return null;
            }

            set
            {
                if (_printDocument != null)
                {
                    _printDocument.PrinterSettings = value;
                }
            }
        }

        /// <summary>
        /// Gets or sets the print document object that sends diagram output to 
        /// the printer.
        /// </summary>
        /// <value>
        /// A <see cref="PrintDocument"/> object specifying the print document
        /// that sends the diagram output to the printer.
        /// </value>
        /// <remarks>
        /// You will only need to set this property when you have a custom
        /// <see cref="PrintDocument"/> implementation, since one is created
        /// by default, and you can directly customize its properties.
        /// </remarks>
        /// <exception cref="ArgumentNullException">
        /// If the <paramref name="value"/> being set is <see langword="null"/>.
        /// </exception>
        public PrintDocument Document
        {
            get
            {
                return _printDocument;
            }

            set
            {
                if (value == null)
                {
                    throw new ArgumentNullException("value",
                        "The PrintDocument object cannot be null (or Nothing).");
                }

                if (_printDocument != value)
                {
                    _printDocument.Dispose();
                    _printDocument = null;

                    _printDocument = value;

                    _printDocument.BeginPrint += new PrintEventHandler(OnBeginPrint);
                    _printDocument.PrintPage  += new PrintPageEventHandler(OnPrintPage);
                    _printDocument.EndPrint   += new PrintEventHandler(OnEndPrint);
                }
            }
        }

        #endregion

        #region Public Methods

        /// <summary>
        /// This creates and displays the page setup dialog,
        /// <see cref="PageSetupDialog"/>, which enables users to change 
        /// page-related print settings, including margins and paper 
        /// orientation.
        /// </summary>
        /// <remarks>
        /// The dialog is displayed in the modal state.
        /// </remarks>
        public void PageSetup()
        {
            if (_printDocument == null)
            {
                return;
            }

            PageSetupDialog pageSetup = new PageSetupDialog();

            pageSetup.Document = _printDocument;

            pageSetup.ShowDialog(Form.ActiveForm);
        }

        /// <summary>
        /// This prints the currently displayed diagram, with an option to
        /// display the print dialog, <see cref="PrintDialog"/>.
        /// </summary>
        /// <param name="showDialog">
        /// This indicates whether to display the <see cref="PrintDialog"/>
        /// dialog or not. If <see langword="true"/>, the print dialog is
        /// display to the user to prompt the print operation, and the user
        /// could cancel the print operation.
        /// </param>
        public void Print(bool showDialog, bool isRunningOnMono)
        {
            if (_diagram == null)
            {
                throw new InvalidOperationException("There is no diagram to print");
            }

            if (showDialog)
            {
                PrintDialog printDlg = new PrintDialog();
                printDlg.UseEXDialog = !isRunningOnMono;
                printDlg.Document = _printDocument;

                if (printDlg.ShowDialog(Form.ActiveForm) == DialogResult.OK)
                {
                    _printDocument.Print();
                }
            }
            else
            {
                _printDocument.Print();
            }
        }

        /// <summary>
        /// This creates and displays the print-preview dialog to both preview
        /// and print the diagram.
        /// </summary>
        /// <remarks>
        /// The dialog is displayed in the modal state.
        /// </remarks>
        public void PrintPreview()
        {
            if (_diagram == null)
            {
                throw new InvalidOperationException("There is no diagram to print");
            }

            PrintPreviewDialog previewDlg = new PrintPreviewDialog();

            Rectangle bounds = Screen.PrimaryScreen.Bounds;

            previewDlg.Width = (int)(bounds.Width * 2f / 3f);
            previewDlg.Height = (int)(bounds.Height * 2f / 3f);
            previewDlg.StartPosition = FormStartPosition.CenterParent;
            previewDlg.UseAntiAlias = true;
            previewDlg.Document = _printDocument;

            previewDlg.ShowDialog(Form.ActiveForm);
        }

        #endregion

        #region Private Print Event Handlers

        /// <summary>
        /// 
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void OnBeginPrint(object sender, PrintEventArgs e)
        {
            _currentPage = 0;
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void OnPrintPage(object sender, PrintPageEventArgs e)
        {
            Graphics graphics = e.Graphics;

            SmoothingMode enumSmooth       = graphics.SmoothingMode;
            TextRenderingHint enumHint     = graphics.TextRenderingHint;
            CompositingQuality enumQuality = graphics.CompositingQuality;  

            graphics.SmoothingMode      = SmoothingMode.HighQuality;
            graphics.TextRenderingHint  = TextRenderingHint.AntiAlias;
            graphics.CompositingQuality = CompositingQuality.HighQuality;
       
            _diagram.Layout(graphics);

            Size bbSize = _diagram.BoundingBox.Size + _diagram.Padding + _diagram.Padding;
            Size totalSize = new Size((int)(bbSize.Width * _diagram.Scale), (int)(bbSize.Height * _diagram.Scale));

            int columnNumber = 1 + totalSize.Width / e.MarginBounds.Width;
            int rowNumber = 1 + totalSize.Height / e.MarginBounds.Height;
            int pageNumber = columnNumber * rowNumber;

            int row, column = Math.DivRem(_currentPage, rowNumber, out row);

            Rectangle clipping = new Rectangle(new Point(column * e.MarginBounds.Width, row * e.MarginBounds.Height),
                new Size((column + 1) * e.MarginBounds.Width, (row + 1) * e.MarginBounds.Height));

            graphics.Clip = new Region(e.MarginBounds);

            //Point virtualPoint = this.panelDiagram.VirtualPoint;
            graphics.TranslateTransform(-(float)(clipping.Left - e.MarginBounds.Left), 
                -(float)(clipping.Top - e.MarginBounds.Top));

            DiagramGdiRenderer.Draw(_diagram, graphics, clipping);

            if (_currentPage < pageNumber - 1)
            {
                _currentPage++;
                e.HasMorePages = true;
            }

            graphics.SmoothingMode      = enumSmooth;
            graphics.TextRenderingHint  = enumHint;
            graphics.CompositingQuality = enumQuality;
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void OnEndPrint(object sender, PrintEventArgs e)
        {
        }

        #endregion

        #region IDisposable Members

        /// <summary>
        /// This cleans up any resources being used.
        /// </summary>
        public void Dispose()
        {
            this.Dispose(true);
            GC.SuppressFinalize(this);
        }

        /// <summary>
        /// This cleans up any resources being used.
        /// </summary>
        /// <param name="disposing">
        /// This is <see langword="true"/> if managed resources should be 
        /// disposed; otherwise, <see langword="false"/>.
        /// </param>
        private void Dispose(bool disposing)
        {
            if (disposing)
            {
                if (_printDocument != null)
                {
                    _printDocument.Dispose();
                    _printDocument = null;
                }
            }

            _diagram = null;
        }

        #endregion
    }
}
