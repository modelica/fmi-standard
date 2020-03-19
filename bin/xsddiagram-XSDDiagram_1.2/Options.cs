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

namespace XSDDiagram
{
    public class Options
    {
		public static string[] Arguments { get; private set; }
		public static string InputFile { get; private set; }
		public static string OutputFile { get; private set; }
        public static bool OutputOnStdOut { get; private set; }
        public static string OutputOnStdOutExtension { get; private set; }
		public static IList<string> RootElements { get; private set; }
		public static int ExpandLevel { get; private set; }
        public static bool ShowDocumentation { get; private set; }
        public static float Zoom { get; private set; }
		public static bool ForceHugeImageGeneration { get; private set; }
		public static bool RequestHelp { get; private set; }
		public static bool IsRunningOnMono { get; private set; }
        public static string Username { get; private set; }
        public static string Password { get; private set; }
        public static IList<string> TextOutputFields { get; private set; }
        public static bool DisplayAttributes { get; private set; }
        public static bool NoGUI { get; private set; }

        static Options()
		{
			InputFile = null;
			OutputFile = null;
			OutputOnStdOut = false;
			OutputOnStdOutExtension = "png";
			RootElements = new List<string>();
			ExpandLevel = 0;
            ShowDocumentation = false;
            Zoom = 100.0f;
			ForceHugeImageGeneration = false;
			RequestHelp = false;
            TextOutputFields = new List<string>();
            DisplayAttributes = false;
            NoGUI = false;

            IsRunningOnMono = Type.GetType("Mono.Runtime") != null;

			string[] args = Environment.GetCommandLineArgs();
			List<string> arguments = new List<string>();
			// Convert "--option:params" or "/option params" to "-option params"
			foreach (var argument in args)
			{
				string command = null;
				if (/*argument.StartsWith("/") ||*/ argument.StartsWith("-"))
					command = argument.Substring(1);
				else if (argument.StartsWith("--"))
					command = argument.Substring(2);
				if (!string.IsNullOrEmpty(command))
				{
					int indexOfColon = command.IndexOf(':');
					if (indexOfColon > 0)
					{
						string parameter = command.Substring(indexOfColon + 1);
						command = command.Substring(0, indexOfColon);
						arguments.Add("-" + command);
						arguments.Add(parameter);
					}
					else
						arguments.Add("-" + command);
				}
				else
					arguments.Add(argument);
			}
			Arguments = arguments.ToArray();

			int currentArgument = 1;
			while (currentArgument < arguments.Count)
			{
				string argument = arguments[currentArgument++];
				if (string.Compare("-h", argument, true) == 0)
				{
					RequestHelp = true;
				}
				else if (string.Compare("-o", argument, true) == 0)
				{
					if (currentArgument < arguments.Count)
						OutputFile = args[currentArgument++];
				}
				else if (string.Compare("-os", argument, true) == 0)
				{
					OutputOnStdOut = true;
					if (currentArgument < arguments.Count)
						OutputOnStdOutExtension = args[currentArgument++];
				}
				else if (string.Compare("-r", argument, true) == 0)
				{
					if (currentArgument < arguments.Count)
						RootElements.Add(args[currentArgument++]);
				}
                else if (string.Compare("-e", argument, true) == 0)
                {
                    if (currentArgument < arguments.Count)
                    {
                        try
                        {
                            ExpandLevel = int.Parse(args[currentArgument++]);
                        }
                        catch { }
                    }
                }
                else if(string.Compare("-d", argument, true) == 0)
                {
                    ShowDocumentation = true;
                }
				else if (string.Compare("-z", argument, true) == 0)
				{
					if (currentArgument < arguments.Count)
					{
						try
						{
							Zoom = (float)int.Parse(args[currentArgument++]);
						}
						catch { }
					}
				}
				else if (string.Compare("-y", argument, true) == 0)
				{
					ForceHugeImageGeneration = true;
				}
                else if (string.Compare("-u", argument, true) == 0)
				{
					if (currentArgument < arguments.Count)
						Username = args[currentArgument++];
				}
                else if (string.Compare("-p", argument, true) == 0)
				{
					if (currentArgument < arguments.Count)
						Password = args[currentArgument++];
				}
                else if (string.Compare("-f", argument, true) == 0)
                {
                    if (currentArgument < arguments.Count)
                    {
                        string textOutputFields = args[currentArgument++];
                        foreach (string field in textOutputFields.Split(new char[] { ',' }))
                            TextOutputFields.Add(field.Trim());
                    }
                }
                else if (string.Compare("-a", argument, true) == 0)
                {
                    DisplayAttributes = true;
                }
                else if (string.Compare("-no-gui", argument, true) == 0)
                {
                    NoGUI = true;
                }
                else
					InputFile = argument;
			}
		}
    }
}
