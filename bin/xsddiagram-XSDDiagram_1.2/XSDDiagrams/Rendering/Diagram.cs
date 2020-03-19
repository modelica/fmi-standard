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
//      Edu Serna (Add Search Functionality)

using System;
using System.Drawing;
using System.Collections.Generic;

namespace XSDDiagram.Rendering
{
	public sealed class Diagram
    {
        #region Private Fields

        private bool _showBoundingBox;
        private bool _showDocumentation;
        private Size _size;
		private Size _padding;
        private float _scale;
        private float _lastScale;

        private Font _font;
        private Font _fontScaled;
        private Font _smallFont;
        private Font _smallFontScaled;
        private Font _documentationFont;
        private Font _documentationFontScaled;

        private Rectangle _boundingBox;
		private DiagramAlignement _alignement;

        private IDictionary<string, XSDObject> _elementsByName;
        private List<DiagramItem> _rootElements;
        private DiagramItem _selectedElement;
        private String _lastSearchText;
        private int _lastSearchHitElementIndex;
        private List<DiagramItem> _lastSearchHitElements;

        private XMLSchema.any _fakeAny;

        #endregion

        #region Constructors and Destructor

        public Diagram()
        {
            _scale = 1.0f;
            _lastScale = 1.0f;
            _size = new Size(100, 100);
            _padding = new Size(10, 10);
            _boundingBox = Rectangle.Empty;
            _alignement = DiagramAlignement.Center;
            _rootElements = new List<DiagramItem>();
            _selectedElement = null;
            _elementsByName = new Dictionary<string, XSDObject>(); // StringComparer.OrdinalIgnoreCase);
            _lastSearchText = String.Empty;
            _lastSearchHitElementIndex = 0;
            _lastSearchHitElements = new List<DiagramItem>();

        }

        #endregion

        #region Public Properties

        public Size Size { get { return _size; } set { _size = value; } }
		public Size Padding { get { return _padding; } set { _padding = value; } }
		public Rectangle BoundingBox { get { return _boundingBox; } }
		public float Scale { get { return _scale; } set { _scale = value; } }
		public DiagramAlignement Alignement { get { return _alignement; } set { _alignement = value; } }
        public bool ShowBoundingBox { get { return _showBoundingBox; } set { _showBoundingBox = value; } }
        public bool ShowDocumentation { get { return _showDocumentation; } set { _showDocumentation = value; } }

        public Font Font { get { return _font; } set { _font = value; } }
        public Font FontScaled { get { return _fontScaled; } set { _fontScaled = value; } }
        public Font SmallFont { get { return _smallFont; } set { _smallFont = value; } }
        public Font SmallFontScaled { get { return _smallFontScaled; } set { _smallFontScaled = value; } }
        public Font DocumentationFont { get { return _documentationFont; } set { _documentationFont = value; } }
        public Font DocumentationFontScaled { get { return _documentationFontScaled; } set { _documentationFontScaled = value; } }

        public IDictionary<string, XSDObject> ElementsByName { get { return _elementsByName; } set { _elementsByName = value; } }
		public List<DiagramItem> RootElements { get { return _rootElements; } }
        public DiagramItem SelectedElement { get { return _selectedElement; } }

        public int SearchHits {  get { return _lastSearchHitElements.Count; } }
        public int ActualSearchHit {  get { return _lastSearchHitElementIndex + 1; } }

        #endregion

        #region Public Events

        public delegate void RequestAnyElementEventHandler(DiagramItem diagramElement, 
            out XMLSchema.element element, out string nameSpace);

		public event RequestAnyElementEventHandler RequestAnyElement;

        #endregion

        #region Public Methods

        public DiagramItem Add(XMLSchema.openAttrs childElement, string nameSpace)
		{
			if (childElement is XMLSchema.element)
				return AddElement(childElement as XMLSchema.element, nameSpace);
			else if (childElement is XMLSchema.group)
				return AddCompositors(childElement as XMLSchema.group, nameSpace);
			else if (childElement is XMLSchema.complexType)
				return AddComplexType(childElement as XMLSchema.complexType, nameSpace);

			return null;
		}

		public DiagramItem AddElement(XMLSchema.element childElement, string nameSpace)
		{
			return AddElement(null, childElement, nameSpace);
		}

		public DiagramItem AddElement(DiagramItem parentDiagramElement, XMLSchema.element childElement, string nameSpace)
		{
            ClearSearch();
            if (childElement != null)
			{
				DiagramItem childDiagramElement = new DiagramItem();

				XMLSchema.element referenceElement = null;
				if (childElement.@ref != null)
				{
					if (!childElement.@ref.IsEmpty)
					{
						childDiagramElement.IsReference = true;

                        XSDObject objectReferred = null;
                        if(_elementsByName.TryGetValue(childElement.@ref.Namespace + ":element:" + childElement.@ref.Name, out objectReferred) && objectReferred != null)
						{
							XMLSchema.element elementReferred = objectReferred.Tag as XMLSchema.element;
							if (elementReferred != null)
							{
								referenceElement = childElement;
								childElement = elementReferred;
							}
						}
						else
							childElement.name = childElement.@ref.Name;
					}
				}

				childDiagramElement.Diagram = this;
				childDiagramElement.TabSchema = childElement;
				childDiagramElement.Name = childElement.name != null ? childElement.name : "";
				childDiagramElement.NameSpace = nameSpace;
				childDiagramElement.ItemType = DiagramItemType.element;
				int occurrence;
				if (int.TryParse(referenceElement != null ? referenceElement.minOccurs : childElement.minOccurs, out occurrence))
					childDiagramElement.MinOccurrence = occurrence;
				else
					childDiagramElement.MinOccurrence = -1;
				//try { childDiagramElement.MinOccurrence = int.Parse(referenceElement != null ? referenceElement.minOccurs : childElement.minOccurs); }
				//catch { childDiagramElement.MinOccurrence = -1; }
				if (int.TryParse(referenceElement != null ? referenceElement.maxOccurs : childElement.maxOccurs, out occurrence))
					childDiagramElement.MaxOccurrence = occurrence;
				else
					childDiagramElement.MaxOccurrence = -1;
				//try { childDiagramElement.MaxOccurrence = int.Parse(referenceElement != null ? referenceElement.maxOccurs : childElement.maxOccurs); }
				//catch { childDiagramElement.MaxOccurrence = -1; }

				bool hasChildren;
				bool isSimpleType;
				GetChildrenInfo(childElement, out hasChildren, out isSimpleType);
				childDiagramElement.HasChildElements = hasChildren;
				childDiagramElement.IsSimpleContent = isSimpleType;

                if (parentDiagramElement == null)
                {
                    _rootElements.Add(childDiagramElement);
                }
                else
                {
                    childDiagramElement.Parent = parentDiagramElement;
                    parentDiagramElement.ChildElements.Add(childDiagramElement);
                }

				if (childElement.@abstract)
				{
					string abstractElementFullName = childDiagramElement.FullName;
					foreach(XSDObject xsdObject in _elementsByName.Values)
					{
						if (xsdObject != null && xsdObject.Tag is XMLSchema.element)
						{
							XMLSchema.element element = xsdObject.Tag as XMLSchema.element;
							if (element.substitutionGroup != null)
							{
								string elementFullName = element.substitutionGroup.Namespace + ":element:" + element.substitutionGroup.Name;
								if (elementFullName == abstractElementFullName)
								{
									DiagramItem diagramBase = AddElement(parentDiagramElement, element, xsdObject.NameSpace);
									if (diagramBase != null)
										diagramBase.InheritFrom = childDiagramElement;
								}
							}
						}
					}
				}

				return childDiagramElement;
			}

			return null;
		}

		public DiagramItem AddComplexType(XMLSchema.complexType childElement, 
            string nameSpace)
		{
			return AddComplexType(null, childElement, false, nameSpace);
		}

		public DiagramItem AddComplexType(DiagramItem parentDiagramElement, 
            XMLSchema.complexType childElement, string nameSpace)
		{
			return AddComplexType(parentDiagramElement, childElement, 
                false, nameSpace);
		}

		public DiagramItem AddComplexType(DiagramItem parentDiagramElement, 
            XMLSchema.complexType childElement, bool isReference, 
            string nameSpace)
		{
            ClearSearch();
            if (childElement != null)
			{
				DiagramItem childDiagramElement = new DiagramItem();
				childDiagramElement.Diagram = this;
				childDiagramElement.TabSchema = childElement;
				childDiagramElement.Name = childElement.name != null ? childElement.name : "";
				childDiagramElement.NameSpace = nameSpace;
				childDiagramElement.ItemType = DiagramItemType.type;
				childDiagramElement.MinOccurrence = 1;
				childDiagramElement.MaxOccurrence = 1;
				childDiagramElement.IsReference = isReference;
				childDiagramElement.IsSimpleContent = false;
				childDiagramElement.HasChildElements = false;

				if (childElement.Items != null)
				{
					for (int i = 0; i < childElement.Items.Length; i++)
					{
						if (childElement.Items[i] is XMLSchema.group ||
							childElement.Items[i] is XMLSchema.complexType ||
							childElement.Items[i] is XMLSchema.complexContent)
						{
							childDiagramElement.HasChildElements = true;
							break;
						}
					}
				}

				if (parentDiagramElement == null)
					_rootElements.Add(childDiagramElement);
				else
				{
					childDiagramElement.Parent = parentDiagramElement;
					parentDiagramElement.ChildElements.Add(childDiagramElement);
				}

				return childDiagramElement;
			}
			return null;
		}

		public DiagramItem AddAny(DiagramItem parentDiagramElement, 
            XMLSchema.any childElement, string nameSpace)
		{
            bool isDisabled = false;
            if (childElement == null)
            {
                isDisabled = true;
                if (_fakeAny == null)
                {
                    _fakeAny = new XMLSchema.any();
                    _fakeAny.minOccurs = "0";
                    _fakeAny.maxOccurs = "unbounded";
                }
                childElement = _fakeAny;
            }
			if (childElement != null)
			{
				DiagramItem childDiagramElement = new DiagramItem();
                childDiagramElement.IsDisabled = isDisabled;
				childDiagramElement.Diagram = this;
				childDiagramElement.TabSchema = childElement;
				childDiagramElement.Name = "any  " + childElement.@namespace;
				childDiagramElement.NameSpace = nameSpace;
				childDiagramElement.ItemType = DiagramItemType.group;  //DiagramBase.TypeEnum.element;
				int occurrence;
				if (int.TryParse(childElement.minOccurs, out occurrence))
					childDiagramElement.MinOccurrence = occurrence;
				else
					childDiagramElement.MinOccurrence = -1;
				//try { childDiagramElement.MinOccurrence = int.Parse(childElement.minOccurs); }
				//catch { childDiagramElement.MinOccurrence = -1; }
				if (int.TryParse(childElement.maxOccurs, out occurrence))
					childDiagramElement.MaxOccurrence = occurrence;
				else
					childDiagramElement.MaxOccurrence = -1;
				//try { childDiagramElement.MaxOccurrence = int.Parse(childElement.maxOccurs); }
				//catch { childDiagramElement.MaxOccurrence = -1; }

				childDiagramElement.IsReference = false;
				childDiagramElement.IsSimpleContent = false;
				childDiagramElement.HasChildElements = false; // true;

                if (parentDiagramElement == null)
                {
                    _rootElements.Add(childDiagramElement);
                }
                else
                {
                    childDiagramElement.Parent = parentDiagramElement;
                    parentDiagramElement.ChildElements.Add(childDiagramElement);
                }

				return childDiagramElement;
			}

			return null;
		}

		public DiagramItem AddCompositors(XMLSchema.group childElement, 
            string nameSpace)
		{
			return AddCompositors(null, childElement, 
                DiagramItemGroupType.Group, nameSpace);
		}

		public DiagramItem AddCompositors(DiagramItem parentDiagramElement, 
            XMLSchema.group childElement, string nameSpace)
		{
			return AddCompositors(parentDiagramElement, 
                childElement, DiagramItemGroupType.Group, nameSpace);
		}

		public DiagramItem AddCompositors(DiagramItem parentDiagramElement, 
            XMLSchema.group childGroup, DiagramItemGroupType type, 
            string nameSpace)
		{
            ClearSearch();
            if (childGroup != null)
			{
				DiagramItem childDiagramGroup = new DiagramItem();
				childDiagramGroup.ItemType = DiagramItemType.group;

				XMLSchema.group referenceGroup = null;
				if (childGroup.@ref != null)
				{
					childDiagramGroup.IsReference = true;
					childDiagramGroup.Name = childGroup.@ref.Name != null ? childGroup.@ref.Name : "";
					childDiagramGroup.NameSpace = childGroup.@ref.Namespace != null ? childGroup.@ref.Namespace : "";
                    XSDObject grpObject = null;
                    if (_elementsByName.TryGetValue(childDiagramGroup.FullName, out grpObject) && grpObject != null)
                    {
                        XMLSchema.group group = grpObject.Tag as XMLSchema.group;
                        if (group != null)
                        {
                            referenceGroup = childGroup;
                            childGroup = group;
                        }
                    }
				}
				else if (type == DiagramItemGroupType.Group)
				{
					childDiagramGroup.Name = childGroup.name != null ? childGroup.name : "";
					childDiagramGroup.NameSpace = nameSpace;
				}
				else
				{
					childDiagramGroup.NameSpace = nameSpace;
				}

				childDiagramGroup.Diagram = this;
				childDiagramGroup.TabSchema = childGroup;
				int occurrence;
				if (int.TryParse(referenceGroup != null ? referenceGroup.minOccurs : childGroup.minOccurs, out occurrence))
					childDiagramGroup.MinOccurrence = occurrence;
				else
					childDiagramGroup.MinOccurrence = -1;
				//try { childDiagramGroup.MinOccurrence = int.Parse(childGroup.minOccurs); }
				//catch { childDiagramGroup.MinOccurrence = -1; }
				if (int.TryParse(referenceGroup != null ? referenceGroup.maxOccurs : childGroup.maxOccurs, out occurrence))
					childDiagramGroup.MaxOccurrence = occurrence;
				else
					childDiagramGroup.MaxOccurrence = -1;
				//try { childDiagramGroup.MaxOccurrence = int.Parse(childGroup.maxOccurs); }
				//catch { childDiagramGroup.MaxOccurrence = -1; }
				childDiagramGroup.HasChildElements = true;
				childDiagramGroup.GroupType = type;

				if (parentDiagramElement == null)
					_rootElements.Add(childDiagramGroup);
				else
				{
					childDiagramGroup.Parent = parentDiagramElement;
					parentDiagramElement.ChildElements.Add(childDiagramGroup);
				}

				return childDiagramGroup;
			}
			return null;
		}

		public void Remove(DiagramItem element)
		{
            ClearSearch();
			if (element.Parent == null)
				_rootElements.Remove(element);
			else
			{
				element.Parent.ChildElements.Remove(element);
				if (element.Parent.ChildElements.Count == 0)
					element.Parent.ShowChildElements = false;
			}
		}

		public void RemoveAll()
		{
            ClearSearch();
			_rootElements.Clear();
		}

		public void ExpandOneLevel()
		{
			foreach (DiagramItem item in _rootElements)
			{
                this.ExpandOneLevel(item);

                if (item.HasChildElements && item.ChildElements.Count == 0)
                {
                    this.ExpandChildren(item);
                }
			}
		}

		public void Clear()
		{
            _elementsByName.Clear();
            _rootElements.Clear();
            _selectedElement = null;
            ClearSearch();
        }

        public void ClearSearch()
        {
            _lastSearchText = String.Empty;
            _lastSearchHitElements.Clear();
        }

        public DiagramItem Search(String text)
        {
            if (String.IsNullOrEmpty(text)) return null;
            text = text.ToLowerInvariant();
            if (text == _lastSearchText)
            {
                _lastSearchHitElementIndex++;
                if (_lastSearchHitElementIndex >= _lastSearchHitElements.Count)
                {
                    _lastSearchHitElementIndex = 0;
                }
                return _lastSearchHitElements[_lastSearchHitElementIndex];
            }
            else
            {
                _lastSearchHitElementIndex = 0;
                _lastSearchHitElements.Clear();
                Search(text, _rootElements);
                if (_lastSearchHitElements.Count == 0)
                {
                    return null;
                }
                else
                {
                    _lastSearchText = text;
                    return _lastSearchHitElements[_lastSearchHitElementIndex];
                }
            }
        }

        private void Search(String text, IList<DiagramItem> items)
        {
            foreach (var item in items)
            {
                if (item.Name.ToLowerInvariant().Contains(text))
                {
                    _lastSearchHitElements.Add(item);
                }
                if (item.HasChildElements && item.ShowChildElements)
                {
                    Search(text, item.ChildElements);
                }
            }
        }

        public void Layout(Graphics g)
		{
 			string fontName = "Arial"; // "Verdana"; // "Arial";

            if (_font == null)
            {
                _font = new Font(fontName, 10.0f, FontStyle.Bold, GraphicsUnit.Pixel);
                _smallFont = new Font(fontName, 9.0f, GraphicsUnit.Pixel);
                _documentationFont = new Font(fontName, 10.0f, GraphicsUnit.Pixel);
            }
            if (_fontScaled == null || _lastScale != _scale)
            {
                //float fontScale = (float)Math.Pow(_scale, 2.0);
                float fontScale = _scale;
                if (_fontScaled != null) _fontScaled.Dispose();
                _fontScaled = new Font(fontName, 10.0f * fontScale, FontStyle.Bold, GraphicsUnit.Pixel);
                if (_smallFontScaled != null) _smallFontScaled.Dispose();
                _smallFontScaled = new Font(fontName, 9.0f * fontScale, GraphicsUnit.Pixel);
                if (_documentationFontScaled != null) _documentationFontScaled.Dispose();
                _documentationFontScaled = new Font(fontName, 10.0f * fontScale, GraphicsUnit.Pixel);
                _lastScale = _scale;
            }

            foreach (DiagramItem element in _rootElements)
				element.GenerateMeasure(g);

			_boundingBox = new Rectangle(0, 0, 100, 0);

			int currentY = _padding.Height;
			foreach (DiagramItem element in _rootElements)
			{
				Rectangle elementBoundingBox = element.BoundingBox;
				elementBoundingBox.X = _padding.Width;
				elementBoundingBox.Y = currentY;
				element.BoundingBox = elementBoundingBox;
				element.GenerateLocation();
				currentY += element.BoundingBox.Height;

				_boundingBox = Rectangle.Union(_boundingBox, element.BoundingBox);
			}
        }

        public void HitTest(Point point, out DiagramItem element, out DiagramHitTestRegion region)
		{
			element = null;
			region = DiagramHitTestRegion.None;

			foreach (DiagramItem childElement in _rootElements)
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

		public int ScaleInt(int integer) 
        { 
            return (int)(integer * this.Scale); 
        }
		
        public Point ScalePoint(Point point)
		{
			return new Point((int)Math.Round(point.X * this.Scale), 
                (int)Math.Round(point.Y * this.Scale));
		}
		
        public Size ScaleSize(Size point)
		{
			return new Size((int)Math.Round(point.Width * this.Scale), 
                (int)Math.Round(point.Height * this.Scale));
		}

		public Rectangle ScaleRectangle(Rectangle rectangle)
		{
			return new Rectangle((int)Math.Round(rectangle.X * this.Scale), 
                (int)Math.Round(rectangle.Y * this.Scale),
				(int)Math.Round(rectangle.Width * this.Scale), 
                (int)Math.Round(rectangle.Height * this.Scale));
		}

		public void ExpandChildren(DiagramItem parentDiagramElement)
		{
            ClearSearch();
            if (parentDiagramElement.ItemType == DiagramItemType.element || parentDiagramElement.ItemType == DiagramItemType.type)
			{
				DiagramItem diagramElement = parentDiagramElement;
				if (diagramElement.TabSchema is XMLSchema.element)
				{
					XMLSchema.element element = diagramElement.TabSchema as XMLSchema.element;

					if (element.Item is XMLSchema.complexType)
					{
						XMLSchema.complexType complexTypeElement = element.Item as XMLSchema.complexType;
						ExpandComplexType(diagramElement, complexTypeElement);
					}
					else if (element.type != null)
					{
                        XSDObject objectAnnotated = null;
                        if(_elementsByName.TryGetValue(element.type.Namespace + ":type:" + element.type.Name, out objectAnnotated) && objectAnnotated != null)
                        {
                            XMLSchema.annotated annotated = objectAnnotated.Tag as XMLSchema.annotated;
                            ExpandAnnotated(diagramElement, annotated, element.type.Namespace);
                        }
					}
				}
				else if (diagramElement.TabSchema is XMLSchema.any)
				{
					//XMLSchema.any any = diagramElement.TabSchema as XMLSchema.any;

					if (RequestAnyElement != null)
					{
						XMLSchema.element requestElement;
						string requestNameSpace;
						RequestAnyElement(diagramElement, out requestElement, out requestNameSpace);
						if(requestElement != null)
						{
							AddElement(diagramElement, requestElement, requestNameSpace);
							diagramElement.ShowChildElements = true;
						}
					}
				}
				else if (diagramElement.TabSchema is XMLSchema.complexType)
				{
					XMLSchema.complexType complexTypeElement = diagramElement.TabSchema as XMLSchema.complexType;
					ExpandComplexType(diagramElement, complexTypeElement);
				}
			}
			else if (parentDiagramElement.ItemType == DiagramItemType.group)
			{
				DiagramItem diagramCompositors = parentDiagramElement;
				XMLSchema.group group = diagramCompositors.TabSchema as XMLSchema.group;

                if (group.Items != null)
                {
                    for (int i = 0; i < group.Items.Length; i++)
                    {
                        switch (group.ItemsElementName[i])
                        {
                            case XMLSchema.ItemsChoiceType2.element:
                                if (group.Items[i] is XMLSchema.element)
                                    AddElement(diagramCompositors, group.Items[i] as XMLSchema.element, diagramCompositors.NameSpace);
                                break;
                            case XMLSchema.ItemsChoiceType2.any:
                                if (group.Items[i] is XMLSchema.any)
                                    AddAny(diagramCompositors, group.Items[i] as XMLSchema.any, diagramCompositors.NameSpace);
                                break;
                            case XMLSchema.ItemsChoiceType2.group:
                                if (group.Items[i] is XMLSchema.group)
                                    AddCompositors(diagramCompositors, group.Items[i] as XMLSchema.group, DiagramItemGroupType.Group, diagramCompositors.NameSpace);
                                break;
                            case XMLSchema.ItemsChoiceType2.all:
                                if (group.Items[i] is XMLSchema.group)
                                    AddCompositors(diagramCompositors, group.Items[i] as XMLSchema.group, DiagramItemGroupType.All, diagramCompositors.NameSpace);
                                break;
                            case XMLSchema.ItemsChoiceType2.choice:
                                if (group.Items[i] is XMLSchema.group)
                                    AddCompositors(diagramCompositors, group.Items[i] as XMLSchema.group, DiagramItemGroupType.Choice, diagramCompositors.NameSpace);
                                break;
                            case XMLSchema.ItemsChoiceType2.sequence:
                                if (group.Items[i] is XMLSchema.group)
                                    AddCompositors(diagramCompositors, group.Items[i] as XMLSchema.group, DiagramItemGroupType.Sequence, diagramCompositors.NameSpace);
                                break;
                        }
                    }
                    parentDiagramElement.ShowChildElements = true;
                }
                else
                {
                    AddAny(diagramCompositors, null, diagramCompositors.NameSpace);
                }
			}
        }

        public void SelectElement(DiagramItem element)
        {
            if(_selectedElement != null)
                _selectedElement.IsSelected = false;
            if (element != null)
            {
                _selectedElement = element;
                element.IsSelected = true;
            }
        }

        #endregion

        #region Private Methods

        private void GetChildrenInfo(XMLSchema.complexType complexTypeElement, 
            out bool hasChildren, out bool isSimpleType)
		{
			bool hasSimpleContent = false;
			if (complexTypeElement.Items != null)
			{
				for (int i = 0; i < complexTypeElement.Items.Length; i++)
				{
					if (complexTypeElement.Items[i] is XMLSchema.group ||
						complexTypeElement.Items[i] is XMLSchema.complexType ||
						complexTypeElement.Items[i] is XMLSchema.complexContent)
					{
						hasChildren = true;
						isSimpleType = complexTypeElement.mixed;
						if (complexTypeElement.Items[i] is XMLSchema.complexContent)
						{
                            //hasChildren = false;
							XMLSchema.complexContent complexContent = complexTypeElement.Items[i] as XMLSchema.complexContent;
							if (complexContent.Item is XMLSchema.extensionType)
							{
                                hasChildren = false;
								XMLSchema.extensionType extensionType = complexContent.Item as XMLSchema.extensionType;
								if (extensionType.all != null || extensionType.group != null || extensionType.choice != null || extensionType.sequence != null)
									hasChildren = true;
								else if (extensionType.@base != null)
								{
                                    XSDObject xsdObject = null;
                                    if(_elementsByName.TryGetValue(extensionType.@base.Namespace + ":type:" + extensionType.@base.Name, out xsdObject) && xsdObject != null)
									{
                                        XMLSchema.complexType baseComplexType = xsdObject.Tag as XMLSchema.complexType;
										if (baseComplexType != null)
                                            GetChildrenInfo(baseComplexType, out hasChildren, out isSimpleType);
									}
								}
							}
                            //else if (complexContent.Item is XMLSchema.complexRestrictionType)
                            //{
                            //    XMLSchema.complexRestrictionType complexRestrictionType = complexContent.Item as XMLSchema.complexRestrictionType;

                            //}
						}
						return;
					}
					else if (complexTypeElement.Items[i] is XMLSchema.simpleContent)
					{
						hasSimpleContent = true;
					}
				}
			}
			hasChildren = false;
			isSimpleType = (hasSimpleContent ? true : complexTypeElement.mixed);
		}

		private void GetChildrenInfo(XMLSchema.element childElement, 
            out bool hasChildren, out bool isSimpleType)
		{
			if (childElement.Item is XMLSchema.complexType)
			{
				XMLSchema.complexType complexTypeElement = childElement.Item as XMLSchema.complexType;
				GetChildrenInfo(complexTypeElement, out hasChildren, out isSimpleType);
				return;
				//if (complexTypeElement.Items != null)
				//{
				//    for (int i = 0; i < complexTypeElement.Items.Length; i++)
				//    {
				//        if (complexTypeElement.Items[i] is XMLSchema.group ||
				//            complexTypeElement.Items[i] is XMLSchema.complexType ||
				//            complexTypeElement.Items[i] is XMLSchema.complexContent)
				//        {
				//            return true;
				//        }
				//    }
				//}
			}
			else if (childElement.type != null)
			{
                XSDObject xsdObject = null;
                if(_elementsByName.TryGetValue(childElement.type.Namespace + ":type:" + childElement.type.Name, out xsdObject) && xsdObject != null)
				//if (_elementsByName.ContainsKey(childElement.type.Namespace + ":type:" + childElement.type.Name))
				{
                    XMLSchema.annotated annotated = xsdObject.Tag as XMLSchema.annotated;
					if (annotated is XMLSchema.simpleType)
					{
						hasChildren = false;
						isSimpleType = true;
					}
					else
						GetChildrenInfo(annotated as XMLSchema.complexType, out hasChildren, out isSimpleType);
					return;
				}
			}
			hasChildren = false;
			isSimpleType = true;
		}

		private void ExpandAnnotated(DiagramItem parentDiagramElement, 
            XMLSchema.annotated annotated, string nameSpace)
		{
			if (annotated is XMLSchema.element)
			{
				AddElement(parentDiagramElement, annotated as XMLSchema.element, nameSpace);
				parentDiagramElement.ShowChildElements = true;
			}
			else if (annotated is XMLSchema.group)
			{
				AddCompositors(parentDiagramElement, annotated as XMLSchema.group, nameSpace);
				parentDiagramElement.ShowChildElements = true;
			}
			else if (annotated is XMLSchema.complexType)
			{
				ExpandComplexType(parentDiagramElement, annotated as XMLSchema.complexType);
				parentDiagramElement.ShowChildElements = true;
			}
		}

		private void ExpandComplexType(DiagramItem parentDiagramElement, 
            XMLSchema.complexType complexTypeElement)
		{
			if (complexTypeElement.Items != null)
			{
                XMLSchema.annotated[] items = complexTypeElement.Items;
                XMLSchema.ItemsChoiceType4[] itemsChoiceType = complexTypeElement.ItemsElementName;

				for (int i = 0; i < items.Length; i++)
				{
					if (items[i] is XMLSchema.group)
					{
						XMLSchema.group group = items[i] as XMLSchema.group;
                        DiagramItem diagramCompositors = AddCompositors(parentDiagramElement, 
                            group, (DiagramItemGroupType)Enum.Parse(typeof(DiagramItemGroupType), itemsChoiceType[i].ToString(), true), parentDiagramElement.NameSpace);
						parentDiagramElement.ShowChildElements = true;
						if (diagramCompositors != null)
							ExpandChildren(diagramCompositors);
					}
					else if (items[i] is XMLSchema.complexContent)
					{
						XMLSchema.complexContent complexContent = items[i] as XMLSchema.complexContent;
						if (complexContent.Item is XMLSchema.extensionType)
						{
							XMLSchema.extensionType extensionType = complexContent.Item as XMLSchema.extensionType;

							XSDObject xsdObject = null;
                            if(_elementsByName.TryGetValue(extensionType.@base.Namespace + ":type:" + extensionType.@base.Name, out xsdObject) && xsdObject != null)
							{
								XMLSchema.annotated annotated = xsdObject.Tag as XMLSchema.annotated;
								ExpandAnnotated(parentDiagramElement, annotated, extensionType.@base.Namespace);
							}

							XMLSchema.group group = extensionType.group as XMLSchema.group;
							if (group != null)
							{
								DiagramItem diagramCompositors = AddCompositors(parentDiagramElement, group, DiagramItemGroupType.Group, extensionType.@base.Namespace);
								parentDiagramElement.ShowChildElements = true;
								if (diagramCompositors != null)
									ExpandChildren(diagramCompositors);
							}

							XMLSchema.group groupSequence = extensionType.sequence as XMLSchema.group;
							if (groupSequence != null)
							{
								DiagramItem diagramCompositors = AddCompositors(parentDiagramElement, groupSequence, DiagramItemGroupType.Sequence, extensionType.@base.Namespace);
								parentDiagramElement.ShowChildElements = true;
								if (diagramCompositors != null)
									ExpandChildren(diagramCompositors);
							}

							XMLSchema.group groupChoice = extensionType.choice as XMLSchema.group;
							if (groupChoice != null)
							{
								DiagramItem diagramCompositors = AddCompositors(parentDiagramElement, groupChoice, DiagramItemGroupType.Choice, extensionType.@base.Namespace);
								parentDiagramElement.ShowChildElements = true;
								if (diagramCompositors != null)
									ExpandChildren(diagramCompositors);
							}

                            XMLSchema.group groupAll = extensionType.all as XMLSchema.group;
                            if (groupAll != null)
                            {
                                DiagramItem diagramCompositors = AddCompositors(parentDiagramElement, groupAll, DiagramItemGroupType.All, extensionType.@base.Namespace);
                                parentDiagramElement.ShowChildElements = true;
                                if (diagramCompositors != null)
                                    ExpandChildren(diagramCompositors);
                            }
                        }
						else if (complexContent.Item is XMLSchema.restrictionType)
						{
							XMLSchema.restrictionType restrictionType = complexContent.Item as XMLSchema.restrictionType;
							XSDObject xsdObject = null;
                            if(_elementsByName.TryGetValue(restrictionType.@base.Namespace + ":type:" + restrictionType.@base.Name, out xsdObject) && xsdObject != null)
                            {
                                XMLSchema.annotated annotated = xsdObject.Tag as XMLSchema.annotated;
                                ExpandAnnotated(parentDiagramElement, annotated, restrictionType.@base.Namespace);
                            }
                            else
                            {
                                //dgis fix github issue 2
                                if(restrictionType.Items != null)
                                {
                                    //for (int j = 0; j < items.Length; j++)
                                    for (int j = 0; j < restrictionType.Items.Length; j++)
                                    {
                                        if (restrictionType.Items[j] is XMLSchema.group)
                                        {
                                            XMLSchema.group group = restrictionType.Items[j] as XMLSchema.group;
                                            DiagramItem diagramCompositors = AddCompositors(parentDiagramElement, group,
                                                (DiagramItemGroupType)Enum.Parse(typeof(DiagramItemGroupType), restrictionType.ItemsElementName[j].ToString(), true), parentDiagramElement.NameSpace);
                                            parentDiagramElement.ShowChildElements = true;
                                            if (diagramCompositors != null)
                                                ExpandChildren(diagramCompositors);
                                        }
                                    }
                                }
                            }
                        }
					}
				}
			}
		}

        private void ExpandOneLevel(DiagramItem parentItem)
        {
            ClearSearch();
            foreach (DiagramItem item in parentItem.ChildElements)
            {
                this.ExpandOneLevel(item);

                if (item.HasChildElements && item.ChildElements.Count == 0)
                {
                    this.ExpandChildren(item);
                }
            }
        }

        #endregion
    }
}
