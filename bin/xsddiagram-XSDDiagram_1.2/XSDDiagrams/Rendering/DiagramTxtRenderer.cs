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
using System.IO;
using System.Text;
using System.Drawing;
using System.Text.RegularExpressions;
using System.Collections.Generic;

namespace XSDDiagram.Rendering
{
    public sealed class DiagramTxtRenderer : DiagramRenderer
    {
        #region Private Fields

        private TextWriter _writer;
        private bool _isCSV;
        private string _fieldSeparator;
        private IList<string> _textOutputFields;
        private IList<string> _finalTextOutputFields;
        private bool _displayAttributes;
        private Schema _schema;

        private static List<string> fields = new List<string>() {
            "PATH", "NAME", "TYPE", "NAMESPACE", "COMMENT"
        };

        #endregion

        #region Constructors and Destructor

        public DiagramTxtRenderer(TextWriter writer)
        {
            if (writer == null)
            {
                throw new ArgumentNullException("writer", "The writer object is required.");
            }

            _writer = writer;
        }

        #endregion

        #region Public Properties

        public override string Name
        {
            get
            {
                return "SVG";
            }
        }

        public TextWriter Writer
        {
            get
            {
                return _writer;
            }
        }

        public bool IsCSV
        {
            get
            {
                return _isCSV;
            }
            set
            {
                _isCSV = value;
            }
        }

        public IList<string> TextOutputFields
        {
            get
            {
                return _textOutputFields;
            }
            set
            {
                _textOutputFields = value;
            }
        }

        public bool DisplayAttributes
        {
            get
            {
                return _displayAttributes;
            }
            set
            {
                _displayAttributes = value;
            }
        }

        public Schema Schema
        {
            get
            {
                return _schema;
            }
            set
            {
                _schema = value;
            }
        }


        #endregion

        #region Public Methods

        public override void BeginItemsRender()
        {
            //PATH,NAME,TYPE,NAMESPACE,COMMENT,ATTRIBUT_NAME,ATTRIBUT_TYPE,ATTRIBUT_COMMENT
            //foreach (string field in _finalTextOutputFields)
            for (int i = 0; i < _finalTextOutputFields.Count; i++)
            {
                if(i > 0)
                    _writer.Write(_fieldSeparator);
                string field = _finalTextOutputFields[i];
                _writer.Write(field);
            }
            _writer.WriteLine();
        }

        public override void EndItemsRender()
        {
        }

        /// <summary>
        /// Renders the specified diagram.
        /// </summary>
        /// <param name="diagram">The diagram.</param>
        public override void Render(Diagram diagram)
        {
            _fieldSeparator = IsCSV ? "," : "\t";

            _finalTextOutputFields = new List<string>();
            if (TextOutputFields.Count == 0)
                _finalTextOutputFields = fields;
            else
                foreach(string field in TextOutputFields)
                    if(fields.Contains(field))
                        _finalTextOutputFields.Add(field);

            this.BeginItemsRender();

            foreach (DiagramItem element in diagram.RootElements)
            {
                this.Render(element);
            }

            this.EndItemsRender();
        }

        /// <summary>
        /// Renders the specified drawing item.
        /// </summary>
        /// <param name="drawingItem">The drawing item.</param>
        public override void Render(DiagramItem drawingItem)
        {
            string type = "";
            if (drawingItem.TabSchema is XMLSchema.element)
                type = "element";
            else if (drawingItem.TabSchema is XMLSchema.simpleType)
                type = "simpleType";
            else if (drawingItem.TabSchema is XMLSchema.complexType)
                type = "complexType";

            if (type.Length > 0)
            {
                string path = '/' + drawingItem.Name;
                DiagramItem parentElement = drawingItem.Parent;
                while (parentElement != null)
                {
                    //if (parentElement.ItemType == DiagramItemType.element && !string.IsNullOrEmpty(parentElement.Name))
                    if ((type == "element" || type == "simpleType" || type == "complexType") && !string.IsNullOrEmpty(parentElement.Name))
                            path = '/' + parentElement.Name + path;
                    parentElement = parentElement.Parent;
                }

                // Get the comment/documentation
                string comment = "";
                XMLSchema.annotated annotated = drawingItem.TabSchema as XMLSchema.annotated;
                if (annotated != null && annotated.annotation != null)
                {
                    foreach (object o in annotated.annotation.Items)
                    {
                        if (o is XMLSchema.documentation)
                        {
                            XMLSchema.documentation documentation = o as XMLSchema.documentation;
                            if (documentation.Any != null && documentation.Any.Length > 0 && documentation.Any[0].Value != null)
                            {
                                string text = documentation.Any[0].Value;
                                text = text.Replace("\n", " ");
                                text = text.Replace("\t", " ");
                                text = text.Replace("\r", "");
                                text = Regex.Replace(text, " +", " ");
                                text = text.Trim();
                                comment = text;
                            }
                            else if (documentation.source != null)
                            {
                                comment = documentation.source;
                            }
                            break;
                        }
                    }
                }

                // Output the item
                for (int i = 0; i < _finalTextOutputFields.Count; i++)
                {
                    if (i > 0)
                        _writer.Write(_fieldSeparator);
                    string field = _finalTextOutputFields[i];
                    switch (field)
                    {
                        case "PATH": _writer.Write(path); break;
                        case "NAME": _writer.Write(drawingItem.Name); break;
                        case "TYPE": _writer.Write(type); break;
                        case "NAMESPACE": _writer.Write(drawingItem.NameSpace); break;
                        case "COMMENT": _writer.Write(comment); break;
                    }
                }
                _writer.WriteLine();


                // Output the item attributes
                if (this.Schema != null && this.DisplayAttributes)
                {
                    string nameSpace = drawingItem.NameSpace;
                    if (annotated != null)
                    {
                        // Attributes enumeration
                        List<XSDAttribute> listAttributes = DiagramHelpers.GetAnnotatedAttributes(this.Schema, annotated, nameSpace);
                        listAttributes.Reverse();
                        for (int a = 0; a < listAttributes.Count; a++)
                        {
                            XSDAttribute xsdAttribute = listAttributes[a];
                            if(xsdAttribute != null)
                            {
                                string commentAttribute = "";
                                XMLSchema.attribute attribute = xsdAttribute.Tag;
                                if (attribute != null && attribute.annotation != null)
                                    commentAttribute = DiagramHelpers.GetAnnotationText(attribute.annotation);

                                for (int i = 0; i < _finalTextOutputFields.Count; i++)
                                {
                                    if (i > 0)
                                        _writer.Write(_fieldSeparator);
                                    string field = _finalTextOutputFields[i];
                                    switch (field)
                                    {
                                        case "PATH": _writer.Write(path + "@" + xsdAttribute.Name); break;
                                        case "NAME": _writer.Write(drawingItem.Name + "@" + xsdAttribute.Name); break;
                                        case "TYPE": _writer.Write(xsdAttribute.Type); break;
                                        case "NAMESPACE": _writer.Write(xsdAttribute.NameSpace); break;
                                        case "COMMENT": _writer.Write(commentAttribute); break;
                                    }
                                }
                                _writer.WriteLine();
                            }

                            //this.listViewAttributes.Items.Add(new ListViewItem(new string[] { attribute.Name, attribute.Type, attribute.Use, attribute.DefaultValue, s })).Tag = attribute;
                        }
                    }
                }


                //// Draw the inheritor line
                //if (drawingItem.InheritFrom != null)
                //{
                //}

                //switch (drawingItem.ItemType)
                //{
                //    case DiagramItemType.element:
                //        break;

                //    case DiagramItemType.type:
                //        break;

                //    case DiagramItemType.group:
                //        {
                //            // Draw the main shape following the min/max occurences

                //            // Draw the group type
                //            switch (drawingItem.GroupType)
                //            {
                //                case DiagramItemGroupType.Sequence: break;
                //                case DiagramItemGroupType.Choice: break;
                //                case DiagramItemGroupType.All: break;
                //            }
                //            break;
                //        }
                //}

                //// Draw text
                //if (drawingItem.Name.Length > 0)
                //{
                //    //drawingItem.Name;
                //}

                //// Draw occurences small text
                //if (drawingItem.MaxOccurrence > 1 || drawingItem.MaxOccurrence == -1) {}

                //// Draw type
                //if (drawingItem.IsSimpleContent) {}

                //// Draw reference arrow
                //if (drawingItem.IsReference) {}
            }

            // Draw children expand box
            if (drawingItem.HasChildElements && drawingItem.ShowChildElements)
            {
                foreach (DiagramItem element in drawingItem.ChildElements)
                {
                    this.Render(element);
                    //_writer.WriteLine();
                }
            }
        }

        #endregion

        #region Private Methods

        #endregion

        #region IDisposable Members

        protected override void Dispose(bool disposing)
        {
            _writer = null;
        }

        #endregion
    }
}
