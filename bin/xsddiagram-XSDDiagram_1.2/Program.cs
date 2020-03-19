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
using System.Collections.Generic;
using System.Drawing;
using System.IO;
using System.Windows.Forms;
using XSDDiagram.Rendering;
using System.Threading;
using System.Globalization;

namespace XSDDiagram
{
	public static class Program
	{
		//[DllImport("kernel32")]
		//static extern IntPtr GetConsoleWindow();

		//[DllImport("kernel32")]
		//static extern bool AllocConsole();

		static string usage = @"XSD Diagram, version {0}
Usage: {1} [-o output.svg] [-os EXTENSION] [-r RootElement[@namespace]]* [-e N] [-d] [-z N] [-f PATH,NAME,TYPE,NAMESPACE,COMMENT] [-a] [-y] [-u USERNAME] [-p PASSWORD] [file.xsd or URL]

-o FILE
	specifies the output image. '.png','.jpg', '.svg', '.txt', '.csv' ('.emf' on Windows) are allowed.
	If not present, the GUI is shown.
-os EXTENSION
	specifies the output image is streamed through the standard
	output. EXTENSION can be: png, jpg, svg, txt, csv.
	If not present, the GUI is shown.
-r ELEMENT
	specifies the root element of the tree.
	You can put several -r options = several root elements in the tree.
    The element can have a namespace: MyElement@http://mynamespace/path
-e N
	specifies the expand level (from 0 to what you want).
	Be carefull, the result image can be huge.
-d
	Display the documentation.
-z N
	specifies the zoom percentage from 10% to 1000% (only for .png image).
	Work only with the '-o', '-os png' or '-os jpg' option.
-f PATH,NAME,TYPE,NAMESPACE,COMMENT
	specifies the fields you want to output when rendering to a txt or csf file.
-a
	outputs the attributes in text mode only (.txt and .csv).
-y
	force huge image generation without user prompt.
-u USERNAME
	specifies a username to authenticate when a xsd dependency
	(import or include) is a secured url.
-p PASSWORD
	specifies a password to authenticate when a xsd dependency
	(import or include) is a secured url.
-no-gui
	prevents the graphical interface to be opened.

Example 1:
> XSDDiagramConsole.exe -o file.png -r TotoRoot -r TotoComplexType@http://mynamespace -e 3 -d -z 200 ./folder1/toto.xsd
	will generate a PNG image from a diagram with a root elements
	'TotoRoot' and 'TotoComplexType', and expanding the tree
	from the root until the 3rd level, with the documentation.

Example 2:
> XSDDiagram.exe ./folder1/toto.xsd
	will load the xsd file in the GUI window.

Example 3:
> XSDDiagram.exe -r TotoRoot -e 2 ./folder1/toto.xsd
	will load the xsd file in the GUI window with a root element
	'TotoRoot' and expanding the tree from the root until the 2nd level.

Example 4:
> XSDDiagramConsole.exe -os svg -r TotoRoot -e 3 ./folder1/toto.xsd
	will write a SVG image in the standard output from a diagram with a root element
	'TotoRoot' and expanding the tree from the root until the 3rd level.

Example 5:
> XSDDiagramConsole.exe -os txt -r TotoRoot -e 3 -f PATH,TYPE,COMMENT -a ./folder1/toto.xsd
	will write a textual representation in the standard output from a diagram with a root element
	'TotoRoot' and expanding the tree from the root until the 3rd level.
";

		/// <summary>
		/// The main entry point for the application.
		/// </summary>
		[STAThread]
		public static void Main()
		{
            Thread.CurrentThread.CurrentCulture = CultureInfo.InvariantCulture;

			bool streamToOutput = !string.IsNullOrEmpty(Options.OutputFile) || Options.OutputOnStdOut;
			if (Options.NoGUI || Options.RequestHelp || streamToOutput)
            {
				//if(!Options.IsRunningOnMono)
				//{
				//    if (GetConsoleWindow() == IntPtr.Zero)
				//        ; // AllocConsole();
				//}

				if (Options.RequestHelp || string.IsNullOrEmpty(Options.InputFile) || !streamToOutput ||
					Options.RootElements.Count == 0 || Options.ExpandLevel < 0 || Options.Zoom < 10.0 || Options.Zoom > 1000.0)
                {
					string version = typeof(Program).Assembly.GetName().Version.ToString();
					Log(usage, version, Path.GetFileName(Environment.GetCommandLineArgs()[0]));

                    return;
                }

				Log("Loading the file: {0}\n", Options.InputFile);

                Schema schema = new Schema();
                schema.RequestCredential += delegate(string url, string realm, int attemptCount, out string username, out string password)
                {
                    username = password = "";
                    if(!string.IsNullOrEmpty(Options.Username))
                    {
                        if (attemptCount > 1)
                            return false;
                        username = Options.Username;
                        password = Options.Password;
                        return true;
                    }
                    return false;
                };

				schema.LoadSchema(Options.InputFile);

                if (schema.LoadError.Count > 0)
                {
                    LogError("There are errors while loading:\n");
                    foreach (var error in schema.LoadError)
                    {
                        LogError(error);
                    }
                    LogError("\r\n");
                }

                Diagram diagram = new Diagram();
                diagram.ShowDocumentation = Options.ShowDocumentation;
                diagram.ElementsByName = schema.ElementsByName;
				diagram.Scale = Options.Zoom / 100.0f;

				foreach (var rootElement in Options.RootElements)
				{
                    string elementName = rootElement;
                    string elementNamespace = null;
                    if(!string.IsNullOrEmpty(elementName))
                    {
                        var pos = rootElement.IndexOf("@");
                        if(pos != -1)
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
							Log("Adding '{0}' element to the diagram...\n", rootElement);
                            diagram.Add(element.Tag, element.NameSpace);
                        }
                    }
                }
                Form form = new Form();
                Graphics graphics = form.CreateGraphics();
                graphics.SmoothingMode = System.Drawing.Drawing2D.SmoothingMode.AntiAlias;
                graphics.TextRenderingHint = System.Drawing.Text.TextRenderingHint.AntiAlias;

				for (int i = 0; i < Options.ExpandLevel; i++)
                {
					Log("Expanding to level {0}...\n", i + 1);
                    diagram.ExpandOneLevel();
                }
                diagram.Layout(graphics);
				Log("Saving image...\n");
                try
                {
					bool result = false;

                    DiagramExporter exporter = new DiagramExporter(diagram);
                    IDictionary<string, object> specificRendererParameters = new Dictionary<string, object>()
                            {
                                { "TextOutputFields", Options.TextOutputFields },
                                { "DisplayAttributes", Options.DisplayAttributes },
                                { "Schema", schema }
                                //For future parameters, {}
                            };
                    if (Options.OutputOnStdOut)
                    {
                        Stream stream = Console.OpenStandardOutput();
                        result = exporter.Export(stream, "." + Options.OutputOnStdOutExtension.ToLower(), graphics, new DiagramAlertHandler(ByPassSaveAlert), specificRendererParameters);
                        stream.Flush();
                    }
                    else
                    {
                        result = exporter.Export(Options.OutputFile, graphics, new DiagramAlertHandler(SaveAlert), specificRendererParameters);
                    }

					if (result)
						Log("The diagram is now saved in the file: {0}\n", Options.OutputFile);
                    else
						Log("ERROR: The diagram has not been saved!\n");
                }
                catch (Exception ex)
                {
					Log("ERROR: The diagram has not been saved. {0}\n", ex.Message);
                }

                graphics.Dispose();
                form.Dispose();
            }
            else
            {
				if (Options.RequestHelp)
				{
					string version = typeof(Program).Assembly.GetName().Version.ToString();
					MessageBox.Show(string.Format(usage, version, Environment.GetCommandLineArgs()[0]));
				}

                Application.ThreadException += HandleThreadException;
                Application.EnableVisualStyles();
                Application.SetCompatibleTextRenderingDefault(false);
                Application.Run(new MainForm());
            }
        }

        static void Log(string format, params object[] arg)
        {
            if (Options.OutputOnStdOut)
                return;
            Console.Write(format, arg);
        }

        static void LogError(string format, params object[] arg)
        {
            Console.Error.Write(format, arg);
        }

		static bool ByPassSaveAlert(string title, string message)
		{
			return true;
		}

        static bool SaveAlert(string title, string message)
        {
			Log(string.Format("{0}. {1} [Yn] >", title, message));
			if(Options.ForceHugeImageGeneration)
			{
				Log("\nYes\n");
				return true;
			}
			
            ConsoleKeyInfo consoleKeyInfo = Console.ReadKey(false);
			Log("\n");
			if (consoleKeyInfo.Key == ConsoleKey.Y || consoleKeyInfo.Key == ConsoleKey.Enter)
			{
				Log("Ok, relax... It can take time!\n");
				return true;
			}
			else
				return false;
        }

		static void HandleThreadException(object sender, System.Threading.ThreadExceptionEventArgs e)
		{
			System.Diagnostics.Trace.WriteLine(e.ToString());
		}
	}
}