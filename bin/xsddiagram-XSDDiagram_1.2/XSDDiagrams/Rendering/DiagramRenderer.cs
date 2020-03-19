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

namespace XSDDiagram.Rendering
{
    public abstract class DiagramRenderer : IDisposable
    {
        #region Constructors and Destructor

        protected DiagramRenderer()
        {   
        }

        ~DiagramRenderer()
        {
            this.Dispose(false);
        }

        #endregion

        #region Public Properties

        public abstract string Name
        {
            get;
        }

        #endregion

        #region Public Methods

        public abstract void BeginItemsRender();

        public abstract void Render(Diagram diagram);
        public abstract void Render(DiagramItem item);

        public abstract void EndItemsRender();

        #endregion

        #region IDisposable Members

        public void Dispose()
        {
            this.Dispose(true);
            GC.SuppressFinalize(this);
        }

        protected virtual void Dispose(bool disposing)
        {
        }

        #endregion
    }
}
