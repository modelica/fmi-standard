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
using System.Collections.Generic;

namespace XSDDiagram.Rendering
{
    public sealed class DiagramSvgRenderer : DiagramRenderer
    {
        #region Private Fields

        private TextWriter _writer;
        private Graphics _graphics;
        private Rectangle _boundingBox;

        private int xsdDocCounter = 0;

        #endregion

        #region Constructors and Destructor

		public DiagramSvgRenderer(TextWriter writer)
		{

			if (writer == null)
			{
				throw new ArgumentNullException("writer", "The writer object is required.");
			}

			_writer = writer;
			_graphics = null;
		}

		public DiagramSvgRenderer(TextWriter writer, Graphics referenceGraphics)
		{
			if (writer == null)
			{
				throw new ArgumentNullException("writer", "The writer object is required.");
			}

			_writer = writer;
			_graphics = referenceGraphics;
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

        #endregion

        #region Public Methods

        public override void BeginItemsRender()
        {
            xsdDocCounter = 0;

            _writer.WriteLine(@"<?xml version=""1.0"" standalone=""no""?>");
            _writer.WriteLine(@"<!DOCTYPE svg PUBLIC ""-//W3C//DTD SVG 1.1//EN"" ""http://www.w3.org/Graphics/SVG/1.1/DTD/svg11.dtd"">");
            if(_boundingBox.Width > 0 && _boundingBox.Height > 0)
                _writer.WriteLine("<svg width=\"{0}\" height=\"{1}\" version=\"1.1\" xmlns=\"http://www.w3.org/2000/svg\">", _boundingBox.Width, _boundingBox.Height);
            else
                _writer.WriteLine(@"<svg width=""100%"" height=""100%"" version=""1.1"" xmlns=""http://www.w3.org/2000/svg"">");
        }

        public override void EndItemsRender()
        {
            _writer.WriteLine(@"</svg>");
        }

        public override void Render(Diagram diagram)
        {
            _boundingBox = diagram.ScaleRectangle(diagram.BoundingBox);
            this.BeginItemsRender();

            foreach (DiagramItem element in diagram.RootElements)
            {
                this.Render(element);
            }

            this.EndItemsRender();
        }

        public override void Render(DiagramItem drawingItem)
        {
			bool showDocumentation = drawingItem.Diagram.ShowDocumentation && !drawingItem.DocumentationBox.IsEmpty && _graphics != null;

            //if (drawingItem.diagram.ShowBoundingBox)
            //{
            //    int color = 255 - depth * 8;
            //    g.FillRectangle(new SolidBrush(Color.FromArgb(color, color, color)), drawingItem.ScaleRectangle(drawingItem.boundingBox));
            //    g.DrawRectangle(foregroundPen, drawingItem.ScaleRectangle(drawingItem.boundingBox));
            //}

            // Draw the children
            if (drawingItem.ShowChildElements)
            {
                foreach (DiagramItem element in drawingItem.ChildElements)
                {
                    this.Render(element);
                    _writer.WriteLine();
                }
            }

            string backgroundBrush = "fill:rgb(255,255,255)";
            string foregroundColor = "rgb(0,0,0)";
            string foregroundBrush = "fill:" + foregroundColor;
            string foregroundPen = "stroke:" + foregroundColor + ";stroke-width:1";
            string foregroundRoundPen = foregroundPen + ";stroke-linecap:round";
            string dashed = "stroke-dasharray:4,1";

            Rectangle scaledElementBox = drawingItem.ScaleRectangle(drawingItem.ElementBox);

            // Draw the children lines
            if (drawingItem.ShowChildElements && drawingItem.ChildElements.Count > 0)
            {
                if (drawingItem.ChildElements.Count == 1 && !showDocumentation)
                {
                    int parentMidleY = drawingItem.ScaleInt(drawingItem.Location.Y + drawingItem.Size.Height / 2);
                    this.SVGLine(foregroundRoundPen, 
                        drawingItem.ScaleInt(drawingItem.Location.X + drawingItem.Size.Width), 
                        parentMidleY, drawingItem.ScaleInt(drawingItem.ChildElements[0].Location.X), parentMidleY);
                }
                else if (drawingItem.ChildElements.Count > 1 || showDocumentation)
                {
                    DiagramItem firstElement = drawingItem.ChildElements[0];
                    DiagramItem lastElement  = drawingItem.ChildElements[drawingItem.ChildElements.Count - 1];
                    int verticalLine         = drawingItem.ScaleInt(firstElement.BoundingBox.Left);
                    
                    foreach (DiagramItem element in drawingItem.ChildElements)
                    {
                        if (element.InheritFrom == null)
                        {
                            int currentMidleY = drawingItem.ScaleInt(element.Location.Y + element.Size.Height / 2);
                            SVGLine(foregroundRoundPen, verticalLine, currentMidleY, 
                                drawingItem.ScaleInt(element.Location.X), currentMidleY);
                        }
                    }

                    int parentMidleY = drawingItem.ScaleInt(drawingItem.Location.Y + drawingItem.Size.Height / 2);
                    int firstMidleY  = drawingItem.ScaleInt(firstElement.Location.Y + firstElement.Size.Height / 2);
                    firstMidleY      = Math.Min(firstMidleY, parentMidleY);
                    int lastMidleY   = drawingItem.ScaleInt(lastElement.Location.Y + lastElement.Size.Height / 2);
                    lastMidleY       = Math.Max(lastMidleY, parentMidleY);
                    this.SVGLine(foregroundRoundPen, verticalLine, firstMidleY, verticalLine, lastMidleY);
                    this.SVGLine(foregroundRoundPen, 
                        drawingItem.ScaleInt(drawingItem.Location.X + drawingItem.Size.Width), 
                        parentMidleY, verticalLine, parentMidleY);
                }
            }

            // Draw the inheritor line
            if (drawingItem.InheritFrom != null)
            {
                string foregroundInheritPen = foregroundPen + ";" + dashed;

                Point p1 = new Point(drawingItem.ScaleInt(drawingItem.InheritFrom.Location.X - 5), 
                    drawingItem.ScaleInt(drawingItem.InheritFrom.Location.Y + drawingItem.InheritFrom.Size.Height + 5));
                Point p2 = new Point(drawingItem.ScaleInt(drawingItem.Location.X - 5), 
                    drawingItem.ScaleInt(drawingItem.Location.Y - 5));
                this.SVGLine(foregroundInheritPen, p1, p2);
                this.SVGLine(foregroundInheritPen, p2, 
                    new Point(drawingItem.ScaleInt(drawingItem.Location.X), drawingItem.ScaleInt(drawingItem.Location.Y)));

                Point targetPoint = new Point(drawingItem.ScaleInt(drawingItem.InheritFrom.Location.X - 3), 
                    drawingItem.ScaleInt(drawingItem.InheritFrom.Location.Y + drawingItem.InheritFrom.Size.Height + 3));
                SVGLine(foregroundInheritPen, targetPoint, p1);
                Point[] pathPoint = new Point[5];
                pathPoint[0] = targetPoint;
                pathPoint[1] = targetPoint; 
                pathPoint[1].X += drawingItem.ScaleInt(2); 
                pathPoint[1].Y += drawingItem.ScaleInt(2);
                pathPoint[2] = targetPoint; 
                pathPoint[2].X += drawingItem.ScaleInt(3); 
                pathPoint[2].Y -= drawingItem.ScaleInt(3);
                pathPoint[3] = targetPoint; 
                pathPoint[3].X -= drawingItem.ScaleInt(2); 
                pathPoint[3].Y -= drawingItem.ScaleInt(2);
                pathPoint[4] = targetPoint;

                string path = SVGPolygonToDrawCommand(pathPoint);
                SVGPath(backgroundBrush + ";" + foregroundPen, path);
            }

            switch (drawingItem.ItemType)
            {
                case DiagramItemType.element:
                    {
                        // Draw the main shape following the min/max occurences
                        string foregroundBoxPen = foregroundPen;

                        if (drawingItem.MinOccurrence == 0)
                        {
                            foregroundBoxPen += ";" + dashed;
                        }
                        if (drawingItem.MaxOccurrence == 1)
                        {
                            SVGRectangle(backgroundBrush + ";" + foregroundBoxPen, scaledElementBox);
                        }
                        else
                        {
                            Rectangle elementBoxShifted = scaledElementBox;
                            elementBoxShifted.Offset(drawingItem.ScalePoint(new Point(3, 3)));
                            this.SVGRectangle(backgroundBrush + ";" + foregroundBoxPen, elementBoxShifted);
                            this.SVGRectangle(backgroundBrush + ";" + foregroundBoxPen, scaledElementBox);
                        }
                    }
                    break;

                case DiagramItemType.type:
                    {
                        // Draw the main shape following the min/max occurences
                        int bevel = (int)(scaledElementBox.Height * 0.30);
                        Point[] pathPoint = new Point[6];
                        pathPoint[0]    = pathPoint[5] = scaledElementBox.Location;
                        pathPoint[1]    = scaledElementBox.Location; 
                        pathPoint[1].X  = scaledElementBox.Right;
                        pathPoint[2]    = scaledElementBox.Location + scaledElementBox.Size;
                        pathPoint[3]    = scaledElementBox.Location; 
                        pathPoint[3].Y  = scaledElementBox.Bottom; 
                        pathPoint[4]    = pathPoint[3];
                        pathPoint[0].X += bevel;
                        pathPoint[3].X += bevel;
                        pathPoint[4].Y -= bevel;
                        pathPoint[5].Y += bevel;

                        string path = SVGPolygonToDrawCommand(pathPoint);

                        Point[] pathPointShifted = new Point[6];
                        Size scaledShiftedBevel = drawingItem.ScaleSize(new Size(3, 3));
                        for (int i = 0; i < pathPoint.Length; i++)
                            pathPointShifted[i] = pathPoint[i] + scaledShiftedBevel;

                        string pathShifted = SVGPolygonToDrawCommand(pathPointShifted);

                        string foregroundBoxPen = foregroundPen;
                        if (drawingItem.MinOccurrence == 0)
                        {
                            foregroundBoxPen += ";" + dashed;
                        }
                        if (drawingItem.MaxOccurrence == 1)
                        {
                            SVGPath(backgroundBrush + ";" + foregroundBoxPen, path);
                        }
                        else
                        {
                            Rectangle elementBoxShifted = scaledElementBox;
                            elementBoxShifted.Offset(drawingItem.ScalePoint(new Point(3, 3)));
                            this.SVGPath(backgroundBrush + ";" + foregroundBoxPen, pathShifted);
                            this.SVGPath(backgroundBrush + ";" + foregroundBoxPen, path);
                        }
                    }
                    break;

                case DiagramItemType.group:
                    {
                        // Draw the main shape following the min/max occurences
                        int bevel = (int)(scaledElementBox.Height * 0.30);
                        Point[] pathPoint = new Point[8];
                        pathPoint[0] = pathPoint[7] = scaledElementBox.Location;
                        pathPoint[1] = scaledElementBox.Location; 
                        pathPoint[1].X = scaledElementBox.Right; pathPoint[2] = pathPoint[1];
                        pathPoint[3] = pathPoint[4] = scaledElementBox.Location + scaledElementBox.Size;
                        pathPoint[5] = scaledElementBox.Location; 
                        pathPoint[5].Y = scaledElementBox.Bottom; 
                        pathPoint[6] = pathPoint[5];
                        pathPoint[0].X += bevel;
                        pathPoint[1].X -= bevel;
                        pathPoint[2].Y += bevel;
                        pathPoint[3].Y -= bevel;
                        pathPoint[4].X -= bevel;
                        pathPoint[5].X += bevel;
                        pathPoint[6].Y -= bevel;
                        pathPoint[7].Y += bevel;

                        string path = SVGPolygonToDrawCommand(pathPoint);

                        Point[] pathPointShifted = new Point[8];
                        Size scaledShiftedBevel = drawingItem.ScaleSize(new Size(3, 3));
                        for (int i = 0; i < pathPoint.Length; i++)
                            pathPointShifted[i] = pathPoint[i] + scaledShiftedBevel;

                        string pathShifted = this.SVGPolygonToDrawCommand(pathPointShifted);


                        string foregroundBoxPen = foregroundPen;
                        if (drawingItem.MinOccurrence == 0)
                        {
                            foregroundBoxPen += ";" + dashed;
                        }
                        if (drawingItem.MaxOccurrence == 1)
                        {
                            this.SVGPath(backgroundBrush + ";" + foregroundBoxPen, path);
                        }
                        else
                        {
                            this.SVGPath(backgroundBrush + ";" + foregroundBoxPen, pathShifted);
                            this.SVGPath(backgroundBrush + ";" + foregroundBoxPen, path);
                        }

                        // Draw the group type
                        switch (drawingItem.GroupType)
                        {
                            case DiagramItemGroupType.Sequence:
                                {
                                    Point p0 = drawingItem.Location + new Size(0, drawingItem.ElementBox.Height / 2);
                                    Point p1 = p0 + new Size(3, 0);
                                    Point p2 = p1 + new Size(drawingItem.ElementBox.Width - 6, 0);
                                    SVGLine(foregroundPen, drawingItem.ScalePoint(p1), drawingItem.ScalePoint(p2));
                                    Point point2 = p0 + new Size(drawingItem.ElementBox.Width / 2, 0);
                                    Point point1 = point2 + new Size(-5, 0);
                                    Point point3 = point2 + new Size(+5, 0);
                                    Size pointSize = new Size(4, 4);
                                    Size pointSize2 = new Size(pointSize.Width / 2, pointSize.Height / 2);
                                    point1 -= pointSize2;
                                    point2 -= pointSize2;
                                    point3 -= pointSize2;
                                    pointSize = drawingItem.ScaleSize(pointSize);
                                    SVGEllipse(foregroundColor, new Rectangle(drawingItem.ScalePoint(point1), pointSize));
                                    SVGEllipse(foregroundColor, new Rectangle(drawingItem.ScalePoint(point2), pointSize));
                                    SVGEllipse(foregroundColor, new Rectangle(drawingItem.ScalePoint(point3), pointSize));

                                    //Point p0 = drawingItem.Location + new Size(0, drawingItem.ElementBox.Height / 2);
                                    //Point point0 = p0 + new Size(3, 0);
                                    //Point point2 = p0 + new Size(drawingItem.ElementBox.Width / 2, 0);
                                    //Point point1 = point2 + new Size(-5, 0);
                                    //Point point3 = point2 + new Size(+5, 0);
                                    //Point point4 = point0 + new Size(drawingItem.ElementBox.Width - 6, 0);

                                    //Pen foregroundBallPen = new Pen(foreground);
                                    //foregroundBallPen.EndCap = LineCap.RoundAnchor;
                                    ////foregroundBallPen.ScaleTransform(1.0f / drawingItem.diagram.Scale, 1.0f / drawingItem.diagram.Scale);
                                    //foregroundBallPen.ScaleTransform(drawingItem.diagram.Scale, drawingItem.diagram.Scale);

                                    //SVGDrawLine(result, foregroundBallPen, drawingItem.ScalePoint(point0), drawingItem.ScalePoint(point1));
                                    //SVGDrawLine(result, foregroundBallPen, drawingItem.ScalePoint(point1), drawingItem.ScalePoint(point2));
                                    //SVGDrawLine(result, foregroundBallPen, drawingItem.ScalePoint(point2), drawingItem.ScalePoint(point3));
                                    //foregroundBallPen.EndCap = LineCap.Flat;
                                    //SVGDrawLine(result, foregroundBallPen, drawingItem.ScalePoint(point3), drawingItem.ScalePoint(point4));
                                }
                                break;
                            case DiagramItemGroupType.Choice:
                                {
                                    int yMiddle = drawingItem.ElementBox.Y + drawingItem.ElementBox.Height / 2;
                                    int yUp = yMiddle - 4;
                                    int yDown = yMiddle + 4;
                                    int xMiddle = drawingItem.ElementBox.X + drawingItem.ElementBox.Width / 2;
                                    int xLeft2 = xMiddle - 4;
                                    int xLeft1 = xLeft2 - 4;
                                    int xLeft0 = xLeft1 - 4;
                                    int xRight0 = xMiddle + 4;
                                    int xRight1 = xRight0 + 4;
                                    int xRight2 = xRight1 + 4;

                                    Point point1 = new Point(xMiddle, yUp);
                                    Point point2 = new Point(xMiddle, yMiddle);
                                    Point point3 = new Point(xMiddle, yDown);
                                    Size pointSize = new Size(4, 4);
                                    Size pointSize2 = new Size(pointSize.Width / 2, pointSize.Height / 2);
                                    point1 -= pointSize2;
                                    point2 -= pointSize2;
                                    point3 -= pointSize2;
                                    pointSize = drawingItem.ScaleSize(pointSize);
                                    SVGLine(foregroundPen, 
                                        drawingItem.ScalePoint(new Point(xLeft0, yMiddle)), 
                                        drawingItem.ScalePoint(new Point(xLeft1, yMiddle)));
                                    SVGLine(foregroundPen, 
                                        drawingItem.ScalePoint(new Point(xLeft1, yMiddle)), 
                                        drawingItem.ScalePoint(new Point(xLeft2, yUp)));
                                    SVGLine(foregroundPen, 
                                        drawingItem.ScalePoint(new Point(xRight0, yUp)), 
                                        drawingItem.ScalePoint(new Point(xRight1, yUp)));
                                    SVGLine(foregroundPen, 
                                        drawingItem.ScalePoint(new Point(xRight0, yMiddle)), 
                                        drawingItem.ScalePoint(new Point(xRight2, yMiddle)));
                                    SVGLine(foregroundPen, 
                                        drawingItem.ScalePoint(new Point(xRight0, yDown)), 
                                        drawingItem.ScalePoint(new Point(xRight1, yDown)));
                                    SVGLine(foregroundPen, 
                                        drawingItem.ScalePoint(new Point(xRight1, yUp)), 
                                        drawingItem.ScalePoint(new Point(xRight1, yDown)));
                                    SVGEllipse(foregroundColor, 
                                        new Rectangle(drawingItem.ScalePoint(point1), pointSize));
                                    SVGEllipse(foregroundColor, 
                                        new Rectangle(drawingItem.ScalePoint(point2), pointSize));
                                    SVGEllipse(foregroundColor, 
                                        new Rectangle(drawingItem.ScalePoint(point3), pointSize));
                                }
                                break;
                            case DiagramItemGroupType.All:
                                {
                                    int yMiddle = drawingItem.ElementBox.Y + drawingItem.ElementBox.Height / 2;
                                    int yUp = yMiddle - 4;
                                    int yDown = yMiddle + 4;
                                    int xMiddle = drawingItem.ElementBox.X + drawingItem.ElementBox.Width / 2;
                                    int xLeft2 = xMiddle - 4;
                                    int xLeft1 = xLeft2 - 4;
                                    int xLeft0 = xLeft1 - 4;
                                    int xRight0 = xMiddle + 4;
                                    int xRight1 = xRight0 + 4;
                                    int xRight2 = xRight1 + 4;

                                    Point point1 = new Point(xMiddle, yUp);
                                    Point point2 = new Point(xMiddle, yMiddle);
                                    Point point3 = new Point(xMiddle, yDown);
                                    Size pointSize = new Size(4, 4);
                                    Size pointSize2 = new Size(pointSize.Width / 2, pointSize.Height / 2);
                                    point1 -= pointSize2;
                                    point2 -= pointSize2;
                                    point3 -= pointSize2;
                                    pointSize = drawingItem.ScaleSize(pointSize);
                                    SVGLine(foregroundPen, 
                                        drawingItem.ScalePoint(new Point(xLeft2, yUp)), 
                                        drawingItem.ScalePoint(new Point(xLeft1, yUp)));
                                    SVGLine(foregroundPen, 
                                        drawingItem.ScalePoint(new Point(xLeft2, yMiddle)), 
                                        drawingItem.ScalePoint(new Point(xLeft0, yMiddle)));
                                    SVGLine(foregroundPen, 
                                        drawingItem.ScalePoint(new Point(xLeft2, yDown)), 
                                        drawingItem.ScalePoint(new Point(xLeft1, yDown)));
                                    SVGLine(foregroundPen, 
                                        drawingItem.ScalePoint(new Point(xLeft1, yUp)), 
                                        drawingItem.ScalePoint(new Point(xLeft1, yDown)));

                                    SVGLine(foregroundPen, 
                                        drawingItem.ScalePoint(new Point(xRight0, yUp)), 
                                        drawingItem.ScalePoint(new Point(xRight1, yUp)));
                                    SVGLine(foregroundPen, 
                                        drawingItem.ScalePoint(new Point(xRight0, yMiddle)), 
                                        drawingItem.ScalePoint(new Point(xRight2, yMiddle)));
                                    SVGLine(foregroundPen, 
                                        drawingItem.ScalePoint(new Point(xRight0, yDown)), 
                                        drawingItem.ScalePoint(new Point(xRight1, yDown)));
                                    SVGLine(foregroundPen, 
                                        drawingItem.ScalePoint(new Point(xRight1, yUp)), 
                                        drawingItem.ScalePoint(new Point(xRight1, yDown)));
                                    SVGEllipse(foregroundColor, 
                                        new Rectangle(drawingItem.ScalePoint(point1), pointSize));
                                    SVGEllipse(foregroundColor, 
                                        new Rectangle(drawingItem.ScalePoint(point2), pointSize));
                                    SVGEllipse(foregroundColor, 
                                        new Rectangle(drawingItem.ScalePoint(point3), pointSize));
                                }
                                break;
                        }
                        break;
                    }
            }

            float fontScale = 0.8f;

            // Draw text
            if (drawingItem.Name.Length > 0)
            {
                string style = String.Format(
                    "font-family:{0};font-size:{1}pt;fill:{2};font-weight:bold;text-anchor:middle;dominant-baseline:central", 
                    drawingItem.Font.Name, drawingItem.Font.Size * fontScale, foregroundColor);
                SVGText(drawingItem.Name, style, 
                    new Rectangle(scaledElementBox.X, scaledElementBox.Y, scaledElementBox.Width, scaledElementBox.Height));
            }

            // Draw Documentation
            if (showDocumentation)
            {
                string text = drawingItem.GetTextDocumentation();
                if (text != null)
                {
                    Rectangle scaledDocumentationBox = drawingItem.ScaleRectangle(drawingItem.DocumentationBox);
                    List<string> lines = WrapText(_graphics, drawingItem.DocumentationFont, text, scaledDocumentationBox.Width * fontScale * 0.6f);
                    //stringFormatText.Trimming = StringTrimming.EllipsisCharacter;
                    string style = String.Format(
                        "font-family:{0};font-size:{1}pt;fill:{2};font-weight:normal;text-anchor:start;dominant-baseline:central;inline-size={3}",
                        drawingItem.DocumentationFont.Name, drawingItem.DocumentationFont.Size * fontScale * 0.9, foregroundColor, scaledDocumentationBox.Width);
                    float fontSizeAdjustement = drawingItem.DocumentationFont.Size * fontScale * 1.4f;
                    SVGText(lines, style,
                        new Point(scaledDocumentationBox.X, scaledDocumentationBox.Y), fontSizeAdjustement,
                        new Size(scaledDocumentationBox.Width, scaledDocumentationBox.Height));
                }
            }

            // Draw occurences small text
            if (drawingItem.MaxOccurrence > 1 || drawingItem.MaxOccurrence == -1)
            {
                string occurences = String.Format("{0}..", drawingItem.MinOccurrence) + 
                    (drawingItem.MaxOccurrence == -1 ? "∞" : string.Format("{0}", drawingItem.MaxOccurrence));
                PointF pointOccurences = new PointF();
                pointOccurences.X = drawingItem.Diagram.Scale * (drawingItem.Location.X + drawingItem.Size.Width - 10);
                pointOccurences.Y = drawingItem.Diagram.Scale * (drawingItem.Location.Y + drawingItem.Size.Height + 10);
                string style = String.Format(
                    "font-family:{0};font-size:{1}pt;fill:{2};text-anchor:end;dominant-baseline:central", 
                    drawingItem.SmallFont.Name, drawingItem.SmallFont.Size * fontScale, foregroundColor);
                SVGText(occurences, style, new Point((int)pointOccurences.X, (int)pointOccurences.Y));
            }

            // Draw type
            if (drawingItem.IsSimpleContent)
            {
                Point currentPoint = scaledElementBox.Location + new Size(2, 2);
                SVGLine(foregroundPen, currentPoint, currentPoint + new Size(drawingItem.ScaleInt(8), 0));
                currentPoint += new Size(0, 2);
                SVGLine(foregroundPen, currentPoint, currentPoint + new Size(drawingItem.ScaleInt(6), 0));
                currentPoint += new Size(0, 2);
                SVGLine(foregroundPen, currentPoint, currentPoint + new Size(drawingItem.ScaleInt(6), 0));
                currentPoint += new Size(0, 2);
                SVGLine(foregroundPen, currentPoint, currentPoint + new Size(drawingItem.ScaleInt(6), 0));
            }

            // Draw reference arrow
            if (drawingItem.IsReference)
            {
                string arrowPen = String.Format("stroke:{0};stroke-width:{1}", 
                    foregroundColor, drawingItem.Diagram.Scale * 2.0f);
                Point basePoint = new Point(drawingItem.ElementBox.Left + 1, drawingItem.ElementBox.Bottom - 1);
                Point targetPoint = basePoint + new Size(3, -3);
                basePoint = drawingItem.ScalePoint(basePoint);
                targetPoint = drawingItem.ScalePoint(targetPoint);
                if (drawingItem.ItemType == DiagramItemType.group)
                {
                    int bevel = (int)(scaledElementBox.Height * 0.30);
                    int groupCornerOffset = (int)((double)bevel * 0.424264068713); // 0.6/sqr(2)
                    basePoint.X += groupCornerOffset;
                    basePoint.Y -= groupCornerOffset;
                    targetPoint.X += groupCornerOffset;
                    targetPoint.Y -= groupCornerOffset;
                }
                SVGLine(arrowPen, basePoint, targetPoint);

                Point[] pathPoint = new Point[5];
                pathPoint[0] = targetPoint;
                pathPoint[1] = targetPoint; 
                pathPoint[1].X += drawingItem.ScaleInt(2); 
                pathPoint[1].Y += drawingItem.ScaleInt(2);
                pathPoint[2] = targetPoint; 
                pathPoint[2].X += drawingItem.ScaleInt(3); 
                pathPoint[2].Y -= drawingItem.ScaleInt(3);
                pathPoint[3] = targetPoint; 
                pathPoint[3].X -= drawingItem.ScaleInt(2); 
                pathPoint[3].Y -= drawingItem.ScaleInt(2);
                pathPoint[4] = targetPoint;

                string path = SVGPolygonToDrawCommand(pathPoint);
                SVGPath(foregroundBrush, path);
            }

            // Draw children expand box
            if (drawingItem.HasChildElements)
            {
                Rectangle scaledChildExpandButtonBox = drawingItem.ScaleRectangle(drawingItem.ChildExpandButtonBox);
                SVGRectangle(backgroundBrush + ";" + foregroundPen, scaledChildExpandButtonBox);

                Point middle = new Point(scaledChildExpandButtonBox.Width / 2, scaledChildExpandButtonBox.Height / 2);
                int borderPadding = Math.Max(2, drawingItem.ScaleInt(2));

                Point p1 = scaledChildExpandButtonBox.Location + new Size(borderPadding, middle.Y);
                Point p2 = new Point(scaledChildExpandButtonBox.Right - borderPadding, p1.Y);
                SVGLine(foregroundPen, p1, p2);
                if (!drawingItem.ShowChildElements)
                {
                    p1 = scaledChildExpandButtonBox.Location + new Size(middle.X, borderPadding);
                    p2 = new Point(p1.X, scaledChildExpandButtonBox.Bottom - borderPadding);
                    SVGLine(foregroundPen, p1, p2);
                }
            }
        }

        #endregion

        #region Private Methods

        private void SVGLine(string pen, Point pt1, Point pt2) 
        { 
            this.SVGLine(pen, pt1.X, pt1.Y, pt2.X, pt2.Y); 
        }

        private void SVGLine(string pen, int x1, int y1, int x2, int y2)
        {
            _writer.WriteLine("<line x1=\"{0}\" y1=\"{1}\" x2=\"{2}\" y2=\"{3}\" style=\"{4}\"/>", 
                x1, y1, x2, y2, pen);
        }

        private void SVGRectangle(string pen, Rectangle rect)
        {
            _writer.WriteLine("<rect x=\"{0}\" y=\"{1}\" width=\"{2}\" height=\"{3}\" style=\"{4}\"/>", 
                rect.X, rect.Y, rect.Width, rect.Height, pen);
        }

        private void SVGEllipse(string brush, Rectangle rect)
        {
            _writer.WriteLine("<ellipse cx=\"{0}\" cy=\"{1}\" rx=\"{2}\" ry=\"{3}\" style=\"{4}\"/>", 
                rect.X + rect.Width / 2, rect.Y + rect.Height / 2, rect.Width / 2, rect.Height / 2, brush);
        }

        private void SVGPath(string style, string drawCommand)
        {
            _writer.WriteLine("<path d=\"{0}\" style=\"{1}\"/>", drawCommand, style);
        }

        private void SVGText(string text, string style, Point point)
        {
            _writer.WriteLine("<text x=\"{0}\" y=\"{1}\" style=\"{2}\">{3}</text>", 
                point.X, point.Y, style, text);
        }

        private void SVGText(string text, string style, Rectangle rect)
        {
            _writer.WriteLine("<text x=\"{0}\" y=\"{1}\" style=\"{2}\">{3}</text>", 
                rect.X + rect.Width / 2.0, rect.Y + rect.Height / 2.0, style, text);
        }
        private void SVGText(List<string> text, string style, Point point, float fontSize, Size size)
        {
            _writer.WriteLine("<clipPath id=\"xsddoc{0}\"><rect x=\"{1}\" y=\"{2}\" width=\"{3}\" height=\"{4}\"/></clipPath>",
                xsdDocCounter, point.X, point.Y - fontSize, size.Width * 2, size.Height + 2 * fontSize);
            int y = point.Y + (int)(fontSize * 0.5f);
            _writer.WriteLine("<text x=\"{0}\" y=\"{1}\" style=\"{2}\" clip-path=\"url(#xsddoc{3})\">{4}",
                point.X, y, style, xsdDocCounter, text.Count > 0 ? text[0] : "");
            for (int i = 1; i < text.Count; i++)
                _writer.WriteLine("<tspan x=\"{0}\" y=\"{1}\">{2}</tspan>", point.X, y + i * fontSize, text[i]);
            _writer.WriteLine("</text>");
            xsdDocCounter++;
        }

        private string SVGPolygonToDrawCommand(Point[] pathPoint)
        {
            StringBuilder result = new StringBuilder();
            for (int i = 0; i < pathPoint.Length; i++)
            {
                result.AppendFormat("{0}{1} {2} ", i == 0 ? 'M' : 'L', pathPoint[i].X, pathPoint[i].Y);
            }
            result.Append('Z');

            return result.ToString();
        }

        static List<string> WrapText(Graphics g, Font font, string text, float pixels)
        {
            string[] originalLines = text.Split(new string[] { " " }, StringSplitOptions.None);
            List<string> wrappedLines = new List<string>();
            StringBuilder actualLine = new StringBuilder();
            float actualWidth = 0;

            foreach (var item in originalLines)
            {
                SizeF wordMeasure = g.MeasureString(item, font);
                actualWidth += wordMeasure.Width;
                actualLine.Append(item + " ");
                if (actualWidth > pixels)
                {
                    wrappedLines.Add(actualLine.ToString());
                    actualLine.Length = 0;
                    actualWidth = 0;
                }
            }

            if (actualLine.Length > 0)
                wrappedLines.Add(actualLine.ToString());

            return wrappedLines;
        }

        #endregion

        #region IDisposable Members

        protected override void Dispose(bool disposing)
        {
            _writer = null;
        }

        #endregion
    }
}
