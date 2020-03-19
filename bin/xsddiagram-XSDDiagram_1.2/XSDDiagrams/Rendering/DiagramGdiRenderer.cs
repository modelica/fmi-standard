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
using System.Drawing.Imaging;
using System.Drawing.Drawing2D;
using System.Text.RegularExpressions;

namespace XSDDiagram.Rendering
{
    public sealed class DiagramGdiRenderer : DiagramRenderer
    {
        #region Private Fields

        private Graphics _graphics;

        #endregion

        #region Constructors and Destructor

        public DiagramGdiRenderer(Graphics graphics)
        {
            if (graphics == null)
            {
                throw new ArgumentNullException("graphics", "The graphics object is required.");
            }

            _graphics = graphics;
        }

        #endregion

        #region Public Properties

        public override string Name
        {
            get
            {
                return "GDI";
            }
        }

        public Graphics Graphics
        {
            get
            {
                return _graphics;
            }
            set
            {
                _graphics = value;
            }
        }

        #endregion

        #region Public Methods

        public override void BeginItemsRender()
        {
        }

        public override void EndItemsRender()
        {
        }

        public override void Render(Diagram diagram)
        {
            this.Render(diagram, null);
        }

        public void Render(Diagram diagram, Rectangle? clipRectangle)
        {
            this.BeginItemsRender();

            Rectangle clipBox = clipRectangle.HasValue ? clipRectangle.Value : Rectangle.Empty;

            if (!clipBox.IsEmpty)
            {
                clipBox.X = (int)((float)clipBox.X / diagram.Scale);
                clipBox.Y = (int)((float)clipBox.Y / diagram.Scale);
                clipBox.Width = (int)((float)clipBox.Width / diagram.Scale);
                clipBox.Height = (int)((float)clipBox.Height / diagram.Scale);

                foreach (DiagramItem element in diagram.RootElements)
                {
                    if (element.BoundingBox.IntersectsWith(clipBox))
                    {
                        this.Render(element, clipBox);
                    }
                }
            }
            else
            {
                foreach (DiagramItem element in diagram.RootElements)
                {
                    this.Render(element);
                }
            }

            this.EndItemsRender();
        }

        public override void Render(DiagramItem item)
        {
            Render(item, null);
        }

        public void Render(DiagramItem drawingItem, Rectangle? clipRectangle)
        {
            //System.Diagnostics.Trace.WriteLine("DiagramElement.Paint\n\tName: " + drawingItem.Name);

            Color backgroundColor = Color.White;
            if(drawingItem.IsSelected)
                backgroundColor = Color.FromArgb(0xA6, 0xCA, 0xF0);

            Brush background = new SolidBrush(backgroundColor);
            Brush backgroundExpandBox = new SolidBrush(Color.White);
            SolidBrush foreground = new SolidBrush(Color.Black);
            if (drawingItem.IsDisabled)
            {
                background = new HatchBrush(HatchStyle.BackwardDiagonal, Color.Gray, backgroundColor);
                foreground = new SolidBrush(Color.Gray);
            }
            Pen foregroundPen = new Pen(foreground);
            float[] dashPattern = new float[] { Math.Max(2f, drawingItem.ScaleInt(5)), Math.Max(1f, drawingItem.ScaleInt(2)) };

            //if (drawingItem.IsReference && drawingItem.diagram.ShowBoundingBox)
            if (drawingItem.Diagram.ShowBoundingBox)
            {
                int color = 255 - drawingItem.Depth * 8;
                _graphics.FillRectangle(new SolidBrush(Color.FromArgb(color, color, color)), drawingItem.ScaleRectangle(drawingItem.BoundingBox));
                _graphics.DrawRectangle(foregroundPen, drawingItem.ScaleRectangle(drawingItem.BoundingBox));
            }

            // Draw the children
            if (drawingItem.ShowChildElements)
            {
                if (clipRectangle.HasValue)
                {
                    foreach (DiagramItem element in drawingItem.ChildElements)
                    {
                        if (element.BoundingBox.IntersectsWith(clipRectangle.Value))
                        {
                            this.Render(element, clipRectangle);
                        }
                    }
                }
                else
                {
                    foreach (DiagramItem element in drawingItem.ChildElements)
                    {
                        this.Render(element);
                    }
                }
            }

            Rectangle scaledElementBox = drawingItem.ScaleRectangle(drawingItem.ElementBox);

            // Draw the children lines
            if (drawingItem.ShowChildElements && drawingItem.ChildElements.Count > 0)
            {
                Pen foregroundInheritPen = new Pen(foreground);
                foregroundInheritPen.StartCap = LineCap.Round;
                foregroundInheritPen.EndCap = LineCap.Round;

                bool showDocumentation = (drawingItem.Diagram.ShowDocumentation); // && !drawingItem.DocumentationBox.IsEmpty);
                if (drawingItem.ChildElements.Count == 1 && !showDocumentation)
                {
                    int parentMidleY = drawingItem.ScaleInt(drawingItem.Location.Y + drawingItem.Size.Height / 2);
                    _graphics.DrawLine(foregroundInheritPen, drawingItem.ScaleInt(drawingItem.Location.X + drawingItem.Size.Width), parentMidleY, drawingItem.ScaleInt(drawingItem.ChildElements[0].Location.X), parentMidleY);
                }
                else if (drawingItem.ChildElements.Count > 1 || showDocumentation)
                {
                    DiagramItem firstElement = drawingItem.ChildElements[0];
                    DiagramItem lastElement = drawingItem.ChildElements[drawingItem.ChildElements.Count - 1];
                    int verticalLine = drawingItem.ScaleInt(firstElement.BoundingBox.Left);
                    foreach (DiagramItem element in drawingItem.ChildElements)
                    {
                        if (element.InheritFrom == null)
                        {
                            int currentMidleY = drawingItem.ScaleInt(element.Location.Y + element.Size.Height / 2);
                            _graphics.DrawLine(foregroundInheritPen, verticalLine, currentMidleY, drawingItem.ScaleInt(element.Location.X), currentMidleY);
                        }
                    }
                    int parentMidleY = drawingItem.ScaleInt(drawingItem.Location.Y + drawingItem.Size.Height / 2);
                    int firstMidleY = drawingItem.ScaleInt(firstElement.Location.Y + firstElement.Size.Height / 2);
                    firstMidleY = Math.Min(firstMidleY, parentMidleY);
                    int lastMidleY = drawingItem.ScaleInt(lastElement.Location.Y + lastElement.Size.Height / 2);
                    lastMidleY = Math.Max(lastMidleY, parentMidleY);
                    _graphics.DrawLine(foregroundInheritPen, verticalLine, firstMidleY, verticalLine, lastMidleY);
                    _graphics.DrawLine(foregroundInheritPen, drawingItem.ScaleInt(drawingItem.Location.X + drawingItem.Size.Width), parentMidleY, verticalLine, parentMidleY);
                }
            }


            // Draw the inheritor line
            if (drawingItem.InheritFrom != null)
            {
                Pen foregroundInheritPen = new Pen(foreground);
                foregroundInheritPen.DashStyle = DashStyle.Dash;
                foregroundInheritPen.DashPattern = dashPattern;

                Point p1 = new Point(drawingItem.ScaleInt(drawingItem.InheritFrom.Location.X - 5), drawingItem.ScaleInt(drawingItem.InheritFrom.Location.Y + drawingItem.InheritFrom.Size.Height + 5));
                Point p2 = new Point(drawingItem.ScaleInt(drawingItem.Location.X - 5), drawingItem.ScaleInt(drawingItem.Location.Y - 5));
                _graphics.DrawLine(foregroundInheritPen, p1, p2);
                _graphics.DrawLine(foregroundInheritPen, p2, new Point(drawingItem.ScaleInt(drawingItem.Location.X), drawingItem.ScaleInt(drawingItem.Location.Y)));

                Point targetPoint = new Point(drawingItem.ScaleInt(drawingItem.InheritFrom.Location.X), drawingItem.ScaleInt(drawingItem.InheritFrom.Location.Y + drawingItem.InheritFrom.Size.Height));
                _graphics.DrawLine(foregroundInheritPen, targetPoint, p1);

                Point[] pathPoint = new Point[4];
                pathPoint[0] = targetPoint;
                pathPoint[1] = targetPoint; pathPoint[1].Y += drawingItem.ScaleInt(5);
                pathPoint[2] = targetPoint; pathPoint[2].X -= drawingItem.ScaleInt(5);
                pathPoint[3] = targetPoint;

                GraphicsPath path = new GraphicsPath();
                path.StartFigure();
                path.AddPolygon(pathPoint);
                path.CloseFigure();

                Pen foregroundBoxPen = new Pen(foreground);
                _graphics.FillPath(background, path);
                _graphics.DrawPath(foregroundBoxPen, path);
            }

            switch (drawingItem.ItemType)
            {
                case DiagramItemType.element:
                    {
                        // Draw the main shape following the min/max occurences
                        Pen foregroundBoxPen = new Pen(foreground);
                        if (drawingItem.MinOccurrence == 0)
                        {
                            foregroundBoxPen.DashStyle = System.Drawing.Drawing2D.DashStyle.Dash;
                            foregroundBoxPen.DashPattern = dashPattern;
                        }
                        if (drawingItem.MaxOccurrence == 1)
                        {
                            _graphics.FillRectangle(background, scaledElementBox);
                            _graphics.DrawRectangle(foregroundBoxPen, scaledElementBox);
                        }
                        else
                        {
                            Rectangle elementBoxShifted = scaledElementBox;
                            elementBoxShifted.Offset(drawingItem.ScalePoint(new Point(3, 3)));
                            _graphics.FillRectangle(background, elementBoxShifted);
                            _graphics.DrawRectangle(foregroundBoxPen, elementBoxShifted);
                            _graphics.FillRectangle(background, scaledElementBox);
                            _graphics.DrawRectangle(foregroundBoxPen, scaledElementBox);
                        }
                    }
                    break;

                case DiagramItemType.type:
                    {
                        // Draw the main shape following the min/max occurences
                        int bevel = (int)(scaledElementBox.Height * 0.30);
                        Point[] pathPoint = new Point[6];
                        pathPoint[0] = pathPoint[5] = scaledElementBox.Location;
                        pathPoint[1] = scaledElementBox.Location; pathPoint[1].X = scaledElementBox.Right;
                        pathPoint[2] = scaledElementBox.Location + scaledElementBox.Size;
                        pathPoint[3] = scaledElementBox.Location; pathPoint[3].Y = scaledElementBox.Bottom; pathPoint[4] = pathPoint[3];
                        pathPoint[0].X += bevel;
                        pathPoint[3].X += bevel;
                        pathPoint[4].Y -= bevel;
                        pathPoint[5].Y += bevel;

                        GraphicsPath path = new GraphicsPath();
                        path.StartFigure();
                        path.AddPolygon(pathPoint);
                        path.CloseFigure();

                        Point[] pathPointShifted = new Point[6];
                        Size scaledShiftedBevel = drawingItem.ScaleSize(new Size(3, 3));
                        for (int i = 0; i < pathPoint.Length; i++)
                            pathPointShifted[i] = pathPoint[i] + scaledShiftedBevel;

                        GraphicsPath pathShifted = new GraphicsPath();
                        pathShifted.StartFigure();
                        pathShifted.AddPolygon(pathPointShifted);
                        pathShifted.CloseFigure();

                        Pen foregroundBoxPen = new Pen(foreground);
                        if (drawingItem.MinOccurrence == 0)
                        {
                            foregroundBoxPen.DashStyle = System.Drawing.Drawing2D.DashStyle.Dash;
                            foregroundBoxPen.DashPattern = dashPattern;
                        }
                        if (drawingItem.MaxOccurrence == 1)
                        {
                            _graphics.FillPath(background, path);
                            _graphics.DrawPath(foregroundBoxPen, path);
                        }
                        else
                        {
                            Rectangle elementBoxShifted = scaledElementBox;
                            elementBoxShifted.Offset(drawingItem.ScalePoint(new Point(3, 3)));
                            _graphics.FillPath(background, pathShifted);
                            _graphics.DrawPath(foregroundBoxPen, pathShifted);
                            _graphics.FillPath(background, path);
                            _graphics.DrawPath(foregroundBoxPen, path);
                        }
                    }
                    break;

                case DiagramItemType.group:
                    {
                        // Draw the main shape following the min/max occurences
                        int bevel = (int)(scaledElementBox.Height * 0.30);
                        Point[] pathPoint = new Point[8];
                        pathPoint[0] = pathPoint[7] = scaledElementBox.Location;
                        pathPoint[1] = scaledElementBox.Location; pathPoint[1].X = scaledElementBox.Right; pathPoint[2] = pathPoint[1];
                        pathPoint[3] = pathPoint[4] = scaledElementBox.Location + scaledElementBox.Size;
                        pathPoint[5] = scaledElementBox.Location; pathPoint[5].Y = scaledElementBox.Bottom; pathPoint[6] = pathPoint[5];
                        pathPoint[0].X += bevel;
                        pathPoint[1].X -= bevel;
                        pathPoint[2].Y += bevel;
                        pathPoint[3].Y -= bevel;
                        pathPoint[4].X -= bevel;
                        pathPoint[5].X += bevel;
                        pathPoint[6].Y -= bevel;
                        pathPoint[7].Y += bevel;

                        GraphicsPath path = new GraphicsPath();
                        path.StartFigure();
                        path.AddPolygon(pathPoint);
                        path.CloseFigure();

                        Point[] pathPointShifted = new Point[8];
                        Size scaledShiftedBevel = drawingItem.ScaleSize(new Size(3, 3));
                        for (int i = 0; i < pathPoint.Length; i++)
                            pathPointShifted[i] = pathPoint[i] + scaledShiftedBevel;

                        GraphicsPath pathShifted = new GraphicsPath();
                        pathShifted.StartFigure();
                        pathShifted.AddPolygon(pathPointShifted);
                        pathShifted.CloseFigure();

                        Pen foregroundBoxPen = new Pen(foreground);
                        if (drawingItem.MinOccurrence == 0)
                        {
                            foregroundBoxPen.DashStyle = System.Drawing.Drawing2D.DashStyle.Dash;
                            foregroundBoxPen.DashPattern = dashPattern;
                        }
                        if (drawingItem.MaxOccurrence == 1)
                        {
                            _graphics.FillPath(background, path);
                            _graphics.DrawPath(foregroundBoxPen, path);
                        }
                        else
                        {
                            Rectangle elementBoxShifted = scaledElementBox;
                            elementBoxShifted.Offset(drawingItem.ScalePoint(new Point(3, 3)));
                            _graphics.FillPath(background, pathShifted);
                            _graphics.DrawPath(foregroundBoxPen, pathShifted);
                            _graphics.FillPath(background, path);
                            _graphics.DrawPath(foregroundBoxPen, path);
                        }

                        // Draw the group type
                        //Pen foregroundPointPen = new Pen(foreground, 4.0f);
                        switch (drawingItem.GroupType)
                        {
                            case DiagramItemGroupType.Sequence:
                                {
                                    Point p0 = drawingItem.Location + new Size(0, drawingItem.ElementBox.Height / 2);
                                    Point p1 = p0 + new Size(3, 0);
                                    Point p2 = p1 + new Size(drawingItem.ElementBox.Width - 6, 0);
                                    _graphics.DrawLine(foregroundPen, drawingItem.ScalePoint(p1), drawingItem.ScalePoint(p2));
                                    Point point2 = p0 + new Size(drawingItem.ElementBox.Width / 2, 0);
                                    Point point1 = point2 + new Size(-5, 0);
                                    Point point3 = point2 + new Size(+5, 0);
                                    Size pointSize = new Size(4, 4);
                                    Size pointSize2 = new Size(pointSize.Width / 2, pointSize.Height / 2);
                                    point1 -= pointSize2;
                                    point2 -= pointSize2;
                                    point3 -= pointSize2;
                                    pointSize = drawingItem.ScaleSize(pointSize);
                                    _graphics.FillEllipse(foreground, new Rectangle(drawingItem.ScalePoint(point1), pointSize));
                                    _graphics.FillEllipse(foreground, new Rectangle(drawingItem.ScalePoint(point2), pointSize));
                                    _graphics.FillEllipse(foreground, new Rectangle(drawingItem.ScalePoint(point3), pointSize));

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

                                    //g.DrawLine(foregroundBallPen, drawingItem.ScalePoint(point0), drawingItem.ScalePoint(point1));
                                    //g.DrawLine(foregroundBallPen, drawingItem.ScalePoint(point1), drawingItem.ScalePoint(point2));
                                    //g.DrawLine(foregroundBallPen, drawingItem.ScalePoint(point2), drawingItem.ScalePoint(point3));
                                    //foregroundBallPen.EndCap = LineCap.Flat;
                                    //g.DrawLine(foregroundBallPen, drawingItem.ScalePoint(point3), drawingItem.ScalePoint(point4));
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
                                    _graphics.DrawLine(foregroundPen, drawingItem.ScalePoint(new Point(xLeft0, yMiddle)), drawingItem.ScalePoint(new Point(xLeft1, yMiddle)));
                                    _graphics.DrawLine(foregroundPen, drawingItem.ScalePoint(new Point(xLeft1, yMiddle)), drawingItem.ScalePoint(new Point(xLeft2, yUp)));
                                    _graphics.DrawLine(foregroundPen, drawingItem.ScalePoint(new Point(xRight0, yUp)), drawingItem.ScalePoint(new Point(xRight1, yUp)));
                                    _graphics.DrawLine(foregroundPen, drawingItem.ScalePoint(new Point(xRight0, yMiddle)), drawingItem.ScalePoint(new Point(xRight2, yMiddle)));
                                    _graphics.DrawLine(foregroundPen, drawingItem.ScalePoint(new Point(xRight0, yDown)), drawingItem.ScalePoint(new Point(xRight1, yDown)));
                                    _graphics.DrawLine(foregroundPen, drawingItem.ScalePoint(new Point(xRight1, yUp)), drawingItem.ScalePoint(new Point(xRight1, yDown)));
                                    _graphics.FillEllipse(foreground, new Rectangle(drawingItem.ScalePoint(point1), pointSize));
                                    _graphics.FillEllipse(foreground, new Rectangle(drawingItem.ScalePoint(point2), pointSize));
                                    _graphics.FillEllipse(foreground, new Rectangle(drawingItem.ScalePoint(point3), pointSize));
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
                                    _graphics.DrawLine(foregroundPen, drawingItem.ScalePoint(new Point(xLeft2, yUp)), drawingItem.ScalePoint(new Point(xLeft1, yUp)));
                                    _graphics.DrawLine(foregroundPen, drawingItem.ScalePoint(new Point(xLeft2, yMiddle)), drawingItem.ScalePoint(new Point(xLeft0, yMiddle)));
                                    _graphics.DrawLine(foregroundPen, drawingItem.ScalePoint(new Point(xLeft2, yDown)), drawingItem.ScalePoint(new Point(xLeft1, yDown)));
                                    _graphics.DrawLine(foregroundPen, drawingItem.ScalePoint(new Point(xLeft1, yUp)), drawingItem.ScalePoint(new Point(xLeft1, yDown)));

                                    _graphics.DrawLine(foregroundPen, drawingItem.ScalePoint(new Point(xRight0, yUp)), drawingItem.ScalePoint(new Point(xRight1, yUp)));
                                    _graphics.DrawLine(foregroundPen, drawingItem.ScalePoint(new Point(xRight0, yMiddle)), drawingItem.ScalePoint(new Point(xRight2, yMiddle)));
                                    _graphics.DrawLine(foregroundPen, drawingItem.ScalePoint(new Point(xRight0, yDown)), drawingItem.ScalePoint(new Point(xRight1, yDown)));
                                    _graphics.DrawLine(foregroundPen, drawingItem.ScalePoint(new Point(xRight1, yUp)), drawingItem.ScalePoint(new Point(xRight1, yDown)));
                                    _graphics.FillEllipse(foreground, new Rectangle(drawingItem.ScalePoint(point1), pointSize));
                                    _graphics.FillEllipse(foreground, new Rectangle(drawingItem.ScalePoint(point2), pointSize));
                                    _graphics.FillEllipse(foreground, new Rectangle(drawingItem.ScalePoint(point3), pointSize));
                                }
                                break;
                        }
                        break;
                    }
            }

            // Draw text
            if (drawingItem.Name.Length > 0)
            {
                StringFormat stringFormatText = new StringFormat();
                stringFormatText.Alignment = StringAlignment.Center;
                stringFormatText.LineAlignment = StringAlignment.Center;
                stringFormatText.FormatFlags |= StringFormatFlags.NoClip; //MONOFIX
                _graphics.DrawString(drawingItem.Name, drawingItem.FontScaled, foreground, new RectangleF(scaledElementBox.X, scaledElementBox.Y, scaledElementBox.Width, scaledElementBox.Height), stringFormatText);
            }

            // Draw Documentation
			if (drawingItem.Diagram.ShowDocumentation && !drawingItem.DocumentationBox.IsEmpty)
            {
                string text = drawingItem.GetTextDocumentation();
                if (text != null)
                {
                    StringFormat stringFormatText = new StringFormat();
                    stringFormatText.Alignment = StringAlignment.Near;
                    stringFormatText.LineAlignment = StringAlignment.Near;
                    stringFormatText.Trimming = StringTrimming.EllipsisCharacter;
                    stringFormatText.FormatFlags |= StringFormatFlags.NoClip; //MONOFIX

                    Rectangle scaledDocumentationBox = drawingItem.ScaleRectangle(drawingItem.DocumentationBox);
                    if (drawingItem.Diagram.ShowBoundingBox)
                    {
                        int color = 255 - drawingItem.Depth * 8;
                        _graphics.FillRectangle(new SolidBrush(Color.FromArgb(color, 255 - color, color)), scaledDocumentationBox);
                        _graphics.DrawRectangle(foregroundPen, scaledDocumentationBox);
                    }
                    _graphics.DrawString(text, drawingItem.DocumentationFontScaled, foreground
                        , new RectangleF(scaledDocumentationBox.X, scaledDocumentationBox.Y, scaledDocumentationBox.Width, scaledDocumentationBox.Height)
                        , stringFormatText);
                }
            }

            // Draw occurences small text
            if (drawingItem.MaxOccurrence > 1 || drawingItem.MaxOccurrence == -1)
            {
                StringFormat stringFormatOccurences = new StringFormat();
                stringFormatOccurences.Alignment = StringAlignment.Far;
                stringFormatOccurences.LineAlignment = StringAlignment.Center;
                stringFormatOccurences.FormatFlags |= StringFormatFlags.NoClip; //MONOFIX
                //string occurences = string.Format("{0}..", drawingItem.MinOccurrence) + (drawingItem.MaxOccurrence == -1 ? "\u0066∞" : string.Format("{0}", drawingItem.MaxOccurrence));
                string occurences = string.Format("{0}..", drawingItem.MinOccurrence) + (drawingItem.MaxOccurrence == -1 ? "\u221E" : string.Format("{0}", drawingItem.MaxOccurrence));
                PointF pointOccurences = new PointF();
                pointOccurences.X = drawingItem.Diagram.Scale * (drawingItem.Location.X + drawingItem.Size.Width - 10);
                pointOccurences.Y = drawingItem.Diagram.Scale * (drawingItem.Location.Y + drawingItem.Size.Height + 10);
                _graphics.DrawString(occurences, drawingItem.SmallFontScaled, foreground, pointOccurences, stringFormatOccurences);
            }

            // Draw type
            if (drawingItem.IsSimpleContent)
            {
                Point currentPoint = scaledElementBox.Location + new Size(2, 2);
                _graphics.DrawLine(foregroundPen, currentPoint, currentPoint + new Size(drawingItem.ScaleInt(8), 0));
                currentPoint += new Size(0, 2);
                _graphics.DrawLine(foregroundPen, currentPoint, currentPoint + new Size(drawingItem.ScaleInt(6), 0));
                currentPoint += new Size(0, 2);
                _graphics.DrawLine(foregroundPen, currentPoint, currentPoint + new Size(drawingItem.ScaleInt(6), 0));
                currentPoint += new Size(0, 2);
                _graphics.DrawLine(foregroundPen, currentPoint, currentPoint + new Size(drawingItem.ScaleInt(6), 0));
            }

            // Draw reference arrow
            if (drawingItem.IsReference)
            {
                Pen arrowPen = new Pen(foreground, drawingItem.Diagram.Scale * 2.0f);
                //arrowPen.EndCap = LineCap.ArrowAnchor;
                //Point basePoint = new Point(drawingItem.ElementBox.Left + 2, drawingItem.ElementBox.Bottom - 2);
                //g.DrawLine(arrowPen, drawingItem.ScalePoint(basePoint), drawingItem.ScalePoint(basePoint + new Size(4, -4)));
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
                _graphics.DrawLine(arrowPen, basePoint, targetPoint);

                Point[] pathPoint = new Point[5];
                pathPoint[0] = targetPoint;
                pathPoint[1] = targetPoint; pathPoint[1].X += drawingItem.ScaleInt(2); pathPoint[1].Y += drawingItem.ScaleInt(2);
                pathPoint[2] = targetPoint; pathPoint[2].X += drawingItem.ScaleInt(3); pathPoint[2].Y -= drawingItem.ScaleInt(3);
                pathPoint[3] = targetPoint; pathPoint[3].X -= drawingItem.ScaleInt(2); pathPoint[3].Y -= drawingItem.ScaleInt(2);
                pathPoint[4] = targetPoint;

                GraphicsPath path = new GraphicsPath();
                path.StartFigure();
                path.AddPolygon(pathPoint);
                path.CloseFigure();
                _graphics.FillPath(foreground, path);
            }

            // Draw children expand box
            if (drawingItem.HasChildElements)
            {
                Rectangle scaledChildExpandButtonBox = drawingItem.ScaleRectangle(drawingItem.ChildExpandButtonBox);
                _graphics.FillRectangle(backgroundExpandBox, scaledChildExpandButtonBox);
                _graphics.DrawRectangle(foregroundPen, scaledChildExpandButtonBox);

                Point middle = new Point(scaledChildExpandButtonBox.Width / 2, scaledChildExpandButtonBox.Height / 2);
                int borderPadding = Math.Max(2, drawingItem.ScaleInt(2));

                Point p1 = scaledChildExpandButtonBox.Location + new Size(borderPadding, middle.Y);
                Point p2 = new Point(scaledChildExpandButtonBox.Right - borderPadding, p1.Y);
                _graphics.DrawLine(foregroundPen, p1, p2);
                if (!drawingItem.ShowChildElements)
                {
                    p1 = scaledChildExpandButtonBox.Location + new Size(middle.X, borderPadding);
                    p2 = new Point(p1.X, scaledChildExpandButtonBox.Bottom - borderPadding);
                    _graphics.DrawLine(foregroundPen, p1, p2);
                }
            }
        }

        #endregion

        #region Public Static Methods

        public static void Draw(Diagram diagram, Graphics g)
        {
            Draw(diagram, g, null);
        }

        public static void Draw(Diagram diagram, Graphics g, Rectangle? clipRectangle)
        {
            using (DiagramGdiRenderer renderer = new DiagramGdiRenderer(g))
            {
                renderer.Render(diagram, clipRectangle);
            }
        }

        #endregion

        #region IDisposable Members

        protected override void Dispose(bool disposing)
        {
            _graphics = null;
        }

        #endregion
    }
}
