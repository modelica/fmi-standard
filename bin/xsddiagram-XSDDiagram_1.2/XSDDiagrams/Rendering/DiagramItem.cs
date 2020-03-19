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
using System.Drawing;
using System.Collections.Generic;
using System.Text.RegularExpressions;
using System.Text;

namespace XSDDiagram.Rendering
{
    public sealed class DiagramItem
    {
        #region Private Methods

        private int _depth;
        private int _childExpandButtonSize;
        private int _minOccurrence;
        private int _maxOccurrence;
        
        private bool _isReference;
        private bool _isSimpleContent;
        private bool _isDisabled;
        private bool _isSelected;
        private bool _hasChildElements;
        private bool _showChildElements;

        private Size _size;
        private Size _margin;
        private Size _padding;
        private Point _location;

        private string _name;
        private string _nameSpace;

        private Rectangle _elementBox;
        private Rectangle _childExpandButtonBox;
        private Rectangle _documentationBox;
        private Rectangle _boundingBox;
        private int _documentationMinWidth;

        private Diagram _diagram;
        private DiagramItem _parent;
        private DiagramItem _inheritFrom;
        private DiagramItemType _itemType;
        private DiagramItemGroupType _groupType;

        private List<DiagramItem>   _childElements;

        private XMLSchema.openAttrs _tabSchema;

        #endregion

        #region Constructors and Destructor

        public DiagramItem()
        {
            _name                  = String.Empty;
            _nameSpace             = String.Empty;
            _minOccurrence         = -1;
            _maxOccurrence         = -1;   
            _location              = new Point(0, 0);
            _childExpandButtonSize = 10;
            _depth                 = 0;              
            _elementBox            = Rectangle.Empty;
            _documentationBox      = Rectangle.Empty;
            _childExpandButtonBox  = Rectangle.Empty;
            _boundingBox           = Rectangle.Empty;
            _documentationMinWidth = 100;
            _size                  = new Size(50, 25);
            _margin                = new Size(10, 5);
            _padding               = new Size(10, 15);
            _itemType              = DiagramItemType.element;
            _childElements         = new List<DiagramItem>();
        }

        #endregion

        #region Public Properties

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

        public DiagramItem Parent 
        { 
            get 
            { 
                return _parent; 
            } 
            set 
            { 
                _parent = value; 
            } 
        }
        
        public DiagramItem InheritFrom 
        { 
            get 
            { 
                return _inheritFrom; 
            } 
            set 
            { 
                _inheritFrom = value; 
            } 
        }
        
        public string Name 
        { 
            get 
            { 
                return _name; 
            } 
            set 
            { 
                _name = value; 
            } 
        }
        
        public string NameSpace 
        { 
            get 
            { 
                return _nameSpace; 
            } 
            set 
            { 
                _nameSpace = value; 
            } 
        }
        
        public DiagramItemType ItemType 
        { 
            get 
            { 
                return _itemType; 
            } 
            set 
            { 
                _itemType = value; 
            } 
        }
        
        public DiagramItemGroupType GroupType 
        { 
            get 
            { 
                return _groupType; 
            } 
            set 
            { 
                _groupType = value; 
            } 
        }
        
        public bool IsReference 
        { 
            get 
            { 
                return _isReference; 
            } 
            set 
            { 
                _isReference = value; 
            } 
        }

        public bool IsSimpleContent 
        { 
            get 
            { 
                return _isSimpleContent; 
            } 
            set 
            { 
                _isSimpleContent = value; 
            } 
        }

        public bool IsDisabled
        {
            get
            {
                return _isDisabled;
            }
            set
            {
                _isDisabled = value;
            }
        }

        public bool IsSelected
        {
            get
            {
                return _isSelected;
            }
            set
            {
                _isSelected = value;
            }
        }

        public int MinOccurrence 
        { 
            get 
            { 
                return _minOccurrence; 
            } 
            set 
            { 
                _minOccurrence = value; 
            } 
        }
        
        public int MaxOccurrence 
        { 
            get 
            { 
                return _maxOccurrence; 
            } 
            set 
            { 
                _maxOccurrence = value; 
            } 
        }
        
        public bool HasChildElements 
        { 
            get 
            { 
                return _hasChildElements; 
            } 
            set 
            { 
                _hasChildElements = value; 
            } 
        }
        
        public bool ShowChildElements 
        { 
            get 
            { 
                return _showChildElements; 
            } 
            set 
            {
                if (_showChildElements != value)
                {
                    _showChildElements = value;
                    this._diagram.ClearSearch();
                }
            } 
        }
        
        public string FullName 
        { 
            get 
            { 
                return _nameSpace + ':' + _itemType + ':' + _name; 
            } 
        }
        
        public Font Font 
        { 
            get 
            { 
                return _diagram.Font; 
            } 
        }
        public Font FontScaled
        {
            get
            {
                return _diagram.FontScaled;
            }
        }

        public Font SmallFont
        {
            get
            {
                return _diagram.SmallFont;
            }
        }
        public Font SmallFontScaled
        {
            get
            {
                return _diagram.SmallFontScaled;
            }
        }

        public Font DocumentationFont
        {
            get
            {
                return _diagram.DocumentationFont;
            }
        }
        public Font DocumentationFontScaled
        {
            get
            {
                return _diagram.DocumentationFontScaled;
            }
        }

        public Point Location 
        { 
            get 
            { 
                return _location; 
            } 
            set 
            { 
                _location = value; 
            } 
        }
        
        public Size Size 
        { 
            get 
            { 
                return _size; 
            } 
            set 
            { 
                _size = value; 
            } 
        }
        
        public Size Margin 
        { 
            get 
            { 
                return _margin; 
            } 
            set 
            { 
                _margin = value; 
            } 
        }
        
        public Size Padding 
        { 
            get 
            { 
                return _padding; 
            } 
            set 
            { 
                _padding = value; 
            } 
        }

        public Rectangle ElementBox
        {
            get
            {
                return _elementBox;
            }
            set
            {
                _elementBox = value;
            }
        }

        public Rectangle ChildExpandButtonBox 
        { 
            get 
            { 
                return _childExpandButtonBox; 
            } 
            set 
            { 
                _childExpandButtonBox = value; 
            } 
        }

        public Rectangle DocumentationBox
        {
            get
            {
                return _documentationBox;
            }
            set
            {
                _documentationBox = value;
            }
        }

        public Rectangle BoundingBox 
        { 
            get 
            { 
                return _boundingBox; 
            } 
            set 
            { 
                _boundingBox = value; 
            } 
        }
        
        public int ChildExpandButtonSize 
        { 
            get 
            { 
                return _childExpandButtonSize; 
            } 
            set 
            { 
                _childExpandButtonSize = value; 
            } 
        }
        
        public int Depth 
        { 
            get 
            { 
                return _depth; 
            } 
            set 
            { 
                _depth = value; 
            } 
        }

        public IList<DiagramItem> ChildElements 
        { 
            get 
            { 
                return _childElements; 
            } 
        }

        public XMLSchema.openAttrs TabSchema 
        { 
            get 
            { 
                return _tabSchema; 
            } 
            set 
            { 
                _tabSchema = value; 
            } 
        }

        #endregion

        #region Public Methods

        public int ScaleInt(int integer) 
        { 
            return _diagram.ScaleInt(integer); 
        }

        public Point ScalePoint(Point point) 
        { 
            return _diagram.ScalePoint(point); 
        }

        public Size ScaleSize(Size point) 
        { 
            return _diagram.ScaleSize(point); 
        }

        public Rectangle ScaleRectangle(Rectangle rectangle) 
        { 
            return _diagram.ScaleRectangle(rectangle); 
        }

        public string GetTextDocumentation()
        {
            string text = null;
            XMLSchema.annotated annotated = this.TabSchema as XMLSchema.annotated;
            if (annotated != null && annotated.annotation != null)
            {
                text = DiagramHelpers.GetAnnotationText(annotated.annotation);

                //foreach (object o in annotated.annotation.Items)
                //{
                //    if (o is XMLSchema.documentation)
                //    {
                //        XMLSchema.documentation documentation = o as XMLSchema.documentation;
                //        if (documentation.Any != null && documentation.Any.Length > 0 && documentation.Any[0].Value != null)
                //        {
                //            text = documentation.Any[0].Value;
                //            text = text.Replace("\n", " ");
                //            text = text.Replace("\t", " ");
                //            text = text.Replace("\r", "");
                //            text = Regex.Replace(text, " +", " ");
                //            text = text.Trim();
                //        }
                //        else if (documentation.source != null)
                //        {
                //            text = documentation.source;
                //        }
                //
                //        break;
                //    }
                //}
            }

            return text;
        }

        public void GenerateMeasure(Graphics g)
        {
            if (_parent != null)
                _depth = _parent.Depth + 1;

            if (_itemType == DiagramItemType.group)
            {
                _size = new Size(40, 20);
            }
            //else
            //    size = new Size(50, 25);

            if (_name.Length > 0)
            {
                SizeF sizeF = g.MeasureString(_name, Font);
                //MONOFIX size = sizeF.ToSize();
                _size = new Size((int)sizeF.Width, (int)sizeF.Height);
                _size = _size + new Size(2 * Margin.Width + (_hasChildElements ? ChildExpandButtonSize : 0), 2 * Margin.Height);
            }

            int childBoundingBoxHeight = 0;
            int childBoundingBoxWidth = 0;
            if (_showChildElements)
            {
                // Measure the children
                foreach (DiagramItem element in _childElements)
                {
                    //MONOFIX GenerateMeasure not supported???
                    element.GenerateMeasure(g);
                    childBoundingBoxWidth = Math.Max(childBoundingBoxWidth, element.BoundingBox.Size.Width);
                    childBoundingBoxHeight += element.BoundingBox.Size.Height;
                }
            }
            _boundingBox.Width = _size.Width + 2 * _padding.Width + childBoundingBoxWidth;
            _boundingBox.Height = Math.Max(_size.Height + 2 * _padding.Height, childBoundingBoxHeight);

            if (_diagram.ShowDocumentation)
            {
                string text = GetTextDocumentation();
                if (text != null)
                {
                    if (_size.Width < _documentationMinWidth)
                    {
                        int widthOffset = _documentationMinWidth - _size.Width;
                        _size.Width += widthOffset;
                        _boundingBox.Width += widthOffset;
                    }

                    SizeF sizeF = g.MeasureString(text, DocumentationFont);
                    double documentationWidth = Math.Max(1.0, _size.Width + _padding.Width); // * 2.0);
                    double documentationHeight = (Math.Ceiling(sizeF.Width / documentationWidth) + 1.8) * sizeF.Height;
                    _documentationBox = new Rectangle(new Point(0, 0), new Size((int)documentationWidth, (int)documentationHeight));
                    _boundingBox.Height = Math.Max(_size.Height + 2 * _padding.Height + _documentationBox.Height + 2 * _padding.Height, childBoundingBoxHeight);
                }
            }

            _elementBox = new Rectangle(new Point(0, 0), _size - new Size(_hasChildElements ? _childExpandButtonSize / 2 : 0, 0));

            if (_hasChildElements)
            {
                _childExpandButtonBox.X      = _elementBox.Width - _childExpandButtonSize / 2;
                _childExpandButtonBox.Y      = (_elementBox.Height - _childExpandButtonSize) / 2;
                _childExpandButtonBox.Width  = _childExpandButtonSize;
                _childExpandButtonBox.Height = _childExpandButtonSize;
            }
        }

        public void GenerateLocation()
        {
            _location.X = _boundingBox.X + _padding.Width;

            switch (_diagram.Alignement)
            {
                case DiagramAlignement.Center:
                    _location.Y = _boundingBox.Y +
                        (_boundingBox.Height - _size.Height) / 2;
				    if(_diagram.ShowDocumentation && !_documentationBox.IsEmpty)
                    {
                        _location.Y = _boundingBox.Y +
                            (_boundingBox.Height - (2 * _padding.Height + _documentationBox.Height)) / 2;
                    }
                    break;
                case DiagramAlignement.Near:
                    if (_itemType == DiagramItemType.group && _parent != null && _parent.ChildElements.Count == 1)
                        _location.Y = _parent.Location.Y + (_parent._elementBox.Height - _elementBox.Height) / 2;
                    else
                        _location.Y = _boundingBox.Y + _padding.Height;
                    break;
                case DiagramAlignement.Far:
                    if (_itemType == DiagramItemType.group && _parent != null && _parent.ChildElements.Count == 1)
                        _location.Y = _parent.Location.Y + (_parent._elementBox.Height - _elementBox.Height) / 2;
                    else
                        _location.Y = _boundingBox.Y +
                            _boundingBox.Height - _size.Height - _padding.Height;
                    break;
            }

            if (_showChildElements)
            {
                int childrenHeight = 0;
                foreach (DiagramItem element in _childElements)
                    childrenHeight += element.BoundingBox.Height;

                int childrenX = _boundingBox.X + 2 * _padding.Width + Size.Width;
                int childrenY = _boundingBox.Y + Math.Max(0, (_boundingBox.Height - childrenHeight) / 2);

                foreach (DiagramItem element in _childElements)
                {
                    Rectangle elementBoundingBox = element.BoundingBox;
                    elementBoundingBox.X = childrenX;
                    elementBoundingBox.Y = childrenY;
                    element.BoundingBox  = elementBoundingBox;
                    element.GenerateLocation();

                    childrenY += element.BoundingBox.Height;
                }
            }

            if (_hasChildElements)
                _childExpandButtonBox.Offset(_location);

            _elementBox.Offset(_location);

			if (!_documentationBox.IsEmpty)
            {
                if(_diagram.Alignement == DiagramAlignement.Far)
                    _documentationBox.Offset(_location.X, _location.Y - _documentationBox.Height - _padding.Height);
                else
                    _documentationBox.Offset(_location.X, _location.Y + _elementBox.Height + _padding.Height);
            }
        }

        public void HitTest(Point point, out DiagramItem element, out DiagramHitTestRegion region)
        {
            element = null;
            if (ScaleRectangle(_boundingBox).Contains(point))
            {
                element = this;
                if (ScaleRectangle(new Rectangle(_location, _size)).Contains(point))
                {
                    if (_hasChildElements && ScaleRectangle(_childExpandButtonBox).Contains(point))
                        region = DiagramHitTestRegion.ChildExpandButton;
                    else
                        region = DiagramHitTestRegion.Element;
                }
                else
                {
                    region = DiagramHitTestRegion.BoundingBox;
                    if (_showChildElements)
                    {
                        foreach (DiagramItem childElement in _childElements)
                        {
                            DiagramItem resultElement;
                            DiagramHitTestRegion resultRegion;
                            childElement.HitTest(point, out resultElement, out resultRegion);
                            if (resultRegion != DiagramHitTestRegion.None)
                            {
                                element = resultElement;
                                region = resultRegion;
                                break;
                            }
                        }
                    }
                }
            }
            else
            {
                region = DiagramHitTestRegion.None;
            }
        }

        #endregion
    }    
}
