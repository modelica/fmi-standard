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
using System.Collections.Generic;
using System.IO;
using System.Net;
using System.Xml.Serialization;
using System.Text.RegularExpressions;
using System.Text;

namespace XSDDiagram
{
    public class Schema
    {
        private static XmlSerializer schemaSerializer = new XmlSerializer(typeof(XMLSchema.schema));

        private List<XSDObject> elements = new List<XSDObject>();
        private Dictionary<string, XSDObject> hashtableElementsByName = new Dictionary<string, XSDObject>(); // StringComparer.OrdinalIgnoreCase);
        private Dictionary<string, object> hashtableAttributesByName = new Dictionary<string, object>(); // StringComparer.OrdinalIgnoreCase);
        private XSDObject firstElement = null;
        private List<string> loadError = new List<string>();
        private List<string> listOfXsdFilename = new List<string>();

        public Schema()
        {
            this.hashtableElementsByName[""] = null;
            this.hashtableAttributesByName[""] = null;

            schemaSerializer.UnreferencedObject += new UnreferencedObjectEventHandler(schemaSerializer_UnreferencedObject);
            schemaSerializer.UnknownNode += new XmlNodeEventHandler(schemaSerializer_UnknownNode);
            schemaSerializer.UnknownElement += new XmlElementEventHandler(schemaSerializer_UnknownElement);
            schemaSerializer.UnknownAttribute += new XmlAttributeEventHandler(schemaSerializer_UnknownAttribute);

            //Trust all certificates
            System.Net.ServicePointManager.ServerCertificateValidationCallback = ((sender, certificate, chain, sslPolicyErrors) => true);
        }

        public IList<XSDObject> Elements { get { return elements; } }
        public IDictionary<string, XSDObject> ElementsByName { get { return hashtableElementsByName; } }
        public IDictionary<string, object> AttributesByName { get { return hashtableAttributesByName; } }
        public XSDObject FirstElement { get { return firstElement; } set { firstElement = value; } }
        public IList<string> LoadError { get { return loadError; } }
        public IList<string> XsdFilenames { get { return listOfXsdFilename; } }

        public delegate bool RequestCredentialEventHandler(string url, string realm, int attemptCount, out string username, out string password);
   		public event RequestCredentialEventHandler RequestCredential;

        public void LoadSchema(string fileName)
        {
            Cleanup();

            string url = fileName.Trim(), baseUrl = "";
            if (url.IndexOf("http://") == 0 || url.IndexOf("https://") == 0)
            {
                string basePath = Path.GetTempPath(), lf, bu;
                if (LoadSchemaFromUrl(basePath, url, out lf, out bu))
                {
                    fileName = lf;
                    baseUrl = bu;
                }
            }

            ImportSchema(fileName, baseUrl);
        }

        public void Cleanup()
        {
            this.firstElement = null;
            this.elements.Clear();
            this.hashtableElementsByName.Clear();
            this.hashtableElementsByName[""] = null;
            this.hashtableAttributesByName.Clear();
            this.hashtableAttributesByName[""] = null;
            this.loadError.Clear();
            this.listOfXsdFilename.Clear();
        }

        public bool IsLoaded()
        {
            return this.listOfXsdFilename.Count > 0;
        }

        private void ImportSchema(string fileName, string baseUrl)
        {
            System.Diagnostics.Trace.WriteLine("ImportSchema: " + fileName);

            fileName = Path.GetFullPath(fileName);

            // Check if the file is already loaded to avoid the cycle
            if (this.listOfXsdFilename.Contains(fileName))
                return;

            FileStream fileStream = null;
            try
            {
                fileStream = new FileStream(fileName, FileMode.Open, FileAccess.Read, FileShare.ReadWrite);
                this.loadError.Clear();
                XMLSchema.schema schemaDOM = (XMLSchema.schema)schemaSerializer.Deserialize(fileStream);

                this.listOfXsdFilename.Add(fileName);

                ParseSchema(fileName, baseUrl, schemaDOM);
            }
            catch (IOException ex)
            {
                this.loadError.Add(ex.Message);
            }
            catch (NotSupportedException ex)
            {
                this.loadError.Add(ex.Message + " (" + fileName + ")");
            }
            catch (InvalidOperationException ex)
            {
                this.loadError.Add(ex.Message + "\r\n" + (ex.InnerException != null ? ex.InnerException.Message : ex.Message));
            }
            catch (UriFormatException ex)
            {
                this.loadError.Add(ex.Message + "\r\n" + (ex.InnerException != null ? ex.InnerException.Message : ex.Message));
            }
            catch (Exception ex)
            {
                this.loadError.Add(ex.Message + "\r\n" + (ex.InnerException != null ? ex.InnerException.Message : ex.Message));
            }
            finally
            {
                if (fileStream != null)
                    fileStream.Close();
            }
        }

        private void ParseSchema(string fileName, string baseUrl, XMLSchema.schema schemaDOM)
        {
            string basePath = Path.GetDirectoryName(fileName);
            if (schemaDOM.Items != null)
            {
                foreach (XMLSchema.openAttrs openAttrs in schemaDOM.Items)
                {
                    string loadedFileName = "";
                    string schemaLocation = "";

                    if (openAttrs is XMLSchema.include)
                    {
                        XMLSchema.include include = openAttrs as XMLSchema.include;
                        if (include.schemaLocation != null)
                            schemaLocation = include.schemaLocation;
                    }
                    else if (openAttrs is XMLSchema.import)
                    {
                        XMLSchema.import import = openAttrs as XMLSchema.import;
                        if (import.schemaLocation != null)
                            schemaLocation = import.schemaLocation;
                    }

                    if (!string.IsNullOrEmpty(schemaLocation))
                    {
                        schemaLocation = Uri.UnescapeDataString(schemaLocation);

                        loadedFileName = basePath + Path.DirectorySeparatorChar + schemaLocation.Replace('/', Path.DirectorySeparatorChar);

                        string url = schemaLocation.Trim();
                        if (url.IndexOf("http://") == 0 || url.IndexOf("https://") == 0)
                        {
                            string lf, bu;
                            if (LoadSchemaFromUrl(basePath, url, out lf, out bu))
                            {
                                loadedFileName = lf;
                                baseUrl = bu; // The baseUrl change for this file
                            }
                        }
                        else if (!File.Exists(loadedFileName))
                        {
                            // The relative file does not exist, so try to download it from the web with the baseUrl
                            url = baseUrl + "/" + schemaLocation;

                            string lf, bu;
                            if (LoadSchemaFromUrl(basePath, url, out lf, out bu))
                            {
                                loadedFileName = lf;
                                baseUrl = bu; // The baseUrl change for this file
                            }
                        }
                    }

                    if (!string.IsNullOrEmpty(loadedFileName))
                        ImportSchema(loadedFileName, baseUrl);
                }
            }

            string nameSpace = schemaDOM.targetNamespace;

            if (schemaDOM.Items1 != null)
            {
                foreach (XMLSchema.openAttrs openAttrs in schemaDOM.Items1)
                {
                    if (openAttrs is XMLSchema.element)
                    {
                        XMLSchema.element element = openAttrs as XMLSchema.element;
                        XSDObject xsdObject = new XSDObject(fileName, element.name, nameSpace, "element", element);
                        this.hashtableElementsByName[xsdObject.FullName] = xsdObject;

                        if (this.firstElement == null)
                            this.firstElement = xsdObject;

                        elements.Add(xsdObject);
                    }
                    else if (openAttrs is XMLSchema.group)
                    {
                        XMLSchema.group group = openAttrs as XMLSchema.group;
                        XSDObject xsdObject = new XSDObject(fileName, group.name, nameSpace, "group", group);
                        this.hashtableElementsByName[xsdObject.FullName] = xsdObject;

                        elements.Add(xsdObject);
                    }
                    else if (openAttrs is XMLSchema.simpleType)
                    {
                        XMLSchema.simpleType simpleType = openAttrs as XMLSchema.simpleType;
                        XSDObject xsdObject = new XSDObject(fileName, simpleType.name, nameSpace, "simpleType", simpleType);
                        this.hashtableElementsByName[xsdObject.FullName] = xsdObject;

                        elements.Add(xsdObject);
                    }
                    else if (openAttrs is XMLSchema.complexType)
                    {
                        XMLSchema.complexType complexType = openAttrs as XMLSchema.complexType;
                        XSDObject xsdObject = new XSDObject(fileName, complexType.name, nameSpace, "complexType", complexType);
                        this.hashtableElementsByName[xsdObject.FullName] = xsdObject;

                        elements.Add(xsdObject);
                    }
                    else if (openAttrs is XMLSchema.attribute)
                    {
                        XMLSchema.attribute attribute = openAttrs as XMLSchema.attribute;
                        XSDAttribute xsdAttribute = new XSDAttribute(fileName, attribute.name, nameSpace, "attribute", attribute.@ref != null, attribute.@default, attribute.use.ToString(), attribute);
                        this.hashtableAttributesByName[xsdAttribute.FullName] = xsdAttribute;
                    }
                    else if (openAttrs is XMLSchema.attributeGroup)
                    {
                        XMLSchema.attributeGroup attributeGroup = openAttrs as XMLSchema.attributeGroup;
                        XSDAttributeGroup xsdAttributeGroup = new XSDAttributeGroup(fileName, attributeGroup.name, nameSpace, "attributeGroup", attributeGroup is XMLSchema.attributeGroupRef, attributeGroup);
                        this.hashtableAttributesByName[xsdAttributeGroup.FullName] = xsdAttributeGroup;
                    }
                }
            }
        }

        private bool LoadSchemaFromUrl(string basePath, string url, out string loadedFilename, out string baseUrl)
        {
            loadedFilename = null;
            baseUrl = "";
            Uri uri;
            try
            {
                uri = new Uri(url);
            }
            catch (UriFormatException ex)
            {
                throw new Exception("Cannot read the URL '" + url + "'!");
            }
            if (uri.Segments.Length > 0)
            {
                string fileNameToImport = uri.Segments[uri.Segments.Length - 1];
                int pos = url.LastIndexOf("/");
                if (pos != -1)
                {
                    baseUrl = url.Substring(0, pos);
                }
                string loadedFileName = Path.Combine(basePath, fileNameToImport);
                if (!File.Exists(loadedFileName))
                {
                    WebClient webClient = new WebClient();
                    webClient.Encoding = Encoding.UTF8;
                    bool tryAgain = false;
                    int attemptCount = 0;
                    do
                    {
                        try
                        {
                            //webClient.DownloadFile(uri, loadedFileName);
                            tryAgain = false;
                            string importedXsdFile = webClient.DownloadString(uri);

                            string importedXsdFileWithoutDTD = new Regex(@"<!DOCTYPE[^>]*>", RegexOptions.Singleline | RegexOptions.IgnoreCase).Replace(importedXsdFile, String.Empty);

                            using (StreamWriter outfile = new StreamWriter(loadedFileName))
                            {
                                outfile.Write(importedXsdFileWithoutDTD);
                            }
                        }
                        catch (WebException ex)
                        {
                            if (ex.Response is HttpWebResponse)
                            {
                                HttpWebResponse response = ex.Response as HttpWebResponse;
                                if (response != null && RequestCredential != null && (response.StatusCode == HttpStatusCode.Unauthorized || response.StatusCode == HttpStatusCode.Forbidden))
                                {
                                    string username = "", password = "";
                                    if (RequestCredential(url, "", ++attemptCount, out username, out password))
                                    {
                                        webClient.Credentials = new System.Net.NetworkCredential(username, password);
                                        tryAgain = true;
                                    }
                                }
                            }
                            if (!tryAgain)
                            {
                                this.loadError.Add("Cannot load the dependency: " + uri.ToString() + ", error: " + ex.ToString());
                                loadedFileName = null;
                            }
                        }
                        catch (Exception ex)
                        {
                            this.loadError.Add("Cannot load the dependency: " + uri.ToString() + ", error: " + ex.ToString());
                            loadedFileName = null;
                        }
                    }
                    while (tryAgain);
                }
                loadedFilename = loadedFileName;
                return true;
            }
            return false;
        }

        //private void ParseSchema(string fileName, XMLSchema.schema schemaDOM)
        //{
        //    string basePath = Path.GetDirectoryName(fileName);
        //    if (schemaDOM.Items != null)
        //    {
        //        foreach (XMLSchema.openAttrs openAttrs in schemaDOM.Items)
        //        {
        //            string loadedFileName = "";
        //            string schemaLocation = "";

        //            if (openAttrs is XMLSchema.include)
        //            {
        //                XMLSchema.include include = openAttrs as XMLSchema.include;
        //                if (include.schemaLocation != null)
        //                    schemaLocation = include.schemaLocation;
        //            }
        //            else if (openAttrs is XMLSchema.import)
        //            {
        //                XMLSchema.import import = openAttrs as XMLSchema.import;
        //                if (import.schemaLocation != null)
        //                    schemaLocation = import.schemaLocation;
        //            }

        //            if (!string.IsNullOrEmpty(schemaLocation))
        //            {
        //                loadedFileName = basePath + Path.DirectorySeparatorChar + schemaLocation.Replace('/', Path.DirectorySeparatorChar);

        //                string url = schemaLocation.Trim();
        //                if (url.IndexOf("http://") == 0 || url.IndexOf("https://") == 0)
        //                {
        //                    Uri uri = new Uri(url);
        //                    if (uri.Segments.Length > 0)
        //                    {
        //                        string fileNameToImport = uri.Segments[uri.Segments.Length - 1];
        //                        loadedFileName = basePath + Path.DirectorySeparatorChar + fileNameToImport;
        //                        if (!File.Exists(loadedFileName))
        //                        {
        //                            WebClient webClient = new WebClient();
        //                            bool tryAgain = false;
        //                            int attemptCount = 0;
        //                            do
        //                            {
        //                                try
        //                                {
        //                                    //webClient.DownloadFile(uri, loadedFileName);
        //                                    tryAgain = false;
        //                                    string importedXsdFile = webClient.DownloadString(uri);

        //                                    string importedXsdFileWithoutDTD = new Regex(@"<!DOCTYPE[^>]*>", RegexOptions.Singleline | RegexOptions.IgnoreCase).Replace(importedXsdFile, String.Empty);

        //                                    using (StreamWriter outfile = new StreamWriter(loadedFileName))
        //                                    {
        //                                        outfile.Write(importedXsdFileWithoutDTD);
        //                                    }
        //                                }
        //                                catch (WebException ex)
        //                                {
        //                                    if (ex.Response is HttpWebResponse)
        //                                    {
        //                                        HttpWebResponse response = ex.Response as HttpWebResponse;
        //                                        if (response != null && RequestCredential != null && (response.StatusCode == HttpStatusCode.Unauthorized || response.StatusCode == HttpStatusCode.Forbidden))
        //                                        {
        //                                            string username = "", password = "";
        //                                            if (RequestCredential(url, "", ++attemptCount, out username, out password))
        //                                            {
        //                                                webClient.Credentials = new System.Net.NetworkCredential(username, password);
        //                                                tryAgain = true;
        //                                            }
        //                                        }
        //                                    }
        //                                    if (!tryAgain)
        //                                    {
        //                                        this.loadError.Add("Cannot load the dependency: " + uri.ToString() + ", error: " + ex.ToString());
        //                                        loadedFileName = null;
        //                                    }
        //                                }
        //                                catch (Exception ex)
        //                                {
        //                                    this.loadError.Add("Cannot load the dependency: " + uri.ToString() + ", error: " + ex.ToString());
        //                                    loadedFileName = null;
        //                                }
        //                            }
        //                            while (tryAgain);
        //                        }
        //                    }
        //                }
        //            }

        //            if (!string.IsNullOrEmpty(loadedFileName))
        //                ImportSchema(loadedFileName);
        //        }
        //    }

        //    string nameSpace = schemaDOM.targetNamespace;

        //    if (schemaDOM.Items1 != null)
        //    {
        //        foreach (XMLSchema.openAttrs openAttrs in schemaDOM.Items1)
        //        {
        //            if (openAttrs is XMLSchema.element)
        //            {
        //                XMLSchema.element element = openAttrs as XMLSchema.element;
        //                XSDObject xsdObject = new XSDObject(fileName, element.name, nameSpace, "element", element);
        //                this.hashtableElementsByName[xsdObject.FullName] = xsdObject;

        //                if (this.firstElement == null)
        //                    this.firstElement = xsdObject;

        //                elements.Add(xsdObject);
        //            }
        //            else if (openAttrs is XMLSchema.group)
        //            {
        //                XMLSchema.group group = openAttrs as XMLSchema.group;
        //                XSDObject xsdObject = new XSDObject(fileName, group.name, nameSpace, "group", group);
        //                this.hashtableElementsByName[xsdObject.FullName] = xsdObject;

        //                elements.Add(xsdObject);
        //            }
        //            else if (openAttrs is XMLSchema.simpleType)
        //            {
        //                XMLSchema.simpleType simpleType = openAttrs as XMLSchema.simpleType;
        //                XSDObject xsdObject = new XSDObject(fileName, simpleType.name, nameSpace, "simpleType", simpleType);
        //                this.hashtableElementsByName[xsdObject.FullName] = xsdObject;

        //                elements.Add(xsdObject);
        //            }
        //            else if (openAttrs is XMLSchema.complexType)
        //            {
        //                XMLSchema.complexType complexType = openAttrs as XMLSchema.complexType;
        //                XSDObject xsdObject = new XSDObject(fileName, complexType.name, nameSpace, "complexType", complexType);
        //                this.hashtableElementsByName[xsdObject.FullName] = xsdObject;

        //                elements.Add(xsdObject);
        //            }
        //            else if (openAttrs is XMLSchema.attribute)
        //            {
        //                XMLSchema.attribute attribute = openAttrs as XMLSchema.attribute;
        //                XSDAttribute xsdAttribute = new XSDAttribute(fileName, attribute.name, nameSpace, "attribute", attribute.@ref != null, attribute.@default, attribute.use.ToString(), attribute);
        //                this.hashtableAttributesByName[xsdAttribute.FullName] = xsdAttribute;
        //            }
        //            else if (openAttrs is XMLSchema.attributeGroup)
        //            {
        //                XMLSchema.attributeGroup attributeGroup = openAttrs as XMLSchema.attributeGroup;
        //                XSDAttributeGroup xsdAttributeGroup = new XSDAttributeGroup(fileName, attributeGroup.name, nameSpace, "attributeGroup", attributeGroup is XMLSchema.attributeGroupRef, attributeGroup);
        //                this.hashtableAttributesByName[xsdAttributeGroup.FullName] = xsdAttributeGroup;
        //            }
        //        }
        //    }
        //}

        void schemaSerializer_UnknownAttribute(object sender, XmlAttributeEventArgs e)
        {
            this.loadError.Add("Unknown attribute (" + e.LineNumber + ", " + e.LinePosition + "): " + e.Attr.Name);
        }

        void schemaSerializer_UnknownElement(object sender, XmlElementEventArgs e)
        {
            this.loadError.Add("Unknown element (" + e.LineNumber + ", " + e.LinePosition + "): " + e.Element.Name);
        }

        void schemaSerializer_UnknownNode(object sender, XmlNodeEventArgs e)
        {
            this.loadError.Add("Unknown node (" + e.LineNumber + ", " + e.LinePosition + "): " + e.Name);
        }

        void schemaSerializer_UnreferencedObject(object sender, UnreferencedObjectEventArgs e)
        {
            this.loadError.Add("Unreferenced object: " + e.UnreferencedId);
        }
    }

    public class XSDObject
    {
        private string filename = "";
        private string name = "";
        private string nameSpace = "";
        private string type = "";
        private string fullNameType = "";
        private XMLSchema.openAttrs tag = null;

        public string Filename { get { return this.filename; } set { this.filename = value; } }
        public string Name { get { return this.name; } set { this.name = value; } }
        public string NameSpace { get { return this.nameSpace; } set { this.nameSpace = value; } }
        public string Type 
        { 
            get 
            { 
                return this.type; 
            } 
            set 
            { 
                this.type = value; 
            } 
        }
        public XMLSchema.openAttrs Tag { get { return this.tag; } set { this.tag = value; } }

        public string FullName { get { return this.nameSpace + ':' + this.fullNameType + ':' + this.name; } }

        public XSDObject(string filename, string name, string nameSpace, string type, XMLSchema.openAttrs tag)
        {
            this.filename = filename;
            this.name = name;
            this.nameSpace = (nameSpace == null ? "" : nameSpace);
            this.type = type;
            if (this.type == "simpleType" || this.type == "complexType")
                this.fullNameType = "type";
            else
                this.fullNameType = this.type;
            this.tag = tag;
        }

        public override string ToString()
        {
            return this.type + ": " + this.name + " (" + this.nameSpace + ")";
        }
    }

    public class XSDAttribute
    {
        private string filename = "";
        private string name = "";
        private string nameSpace = "";
        private string type = "";
        private bool isReference = false;
        private string defaultValue = "";
        private string use = "";
        private XMLSchema.attribute tag = null;

        public string Filename { get { return this.filename; } set { this.filename = value; } }
        public string Name { get { return this.name; } set { this.name = value; } }
        public string NameSpace { get { return this.nameSpace; } set { this.nameSpace = value; } }
        public string Type 
        { 
            get 
            { 
                return this.type; 
            } 
            set 
            { 
                this.type = value; 
            } 
        }
        public bool IsReference { get { return this.isReference; } set { this.isReference = value; } }
        public string DefaultValue { get { return this.defaultValue; } set { this.defaultValue = value; } }
        public string Use { get { return this.use; } set { this.use = value; } }
        public XMLSchema.attribute Tag { get { return this.tag; } set { this.tag = value; } }

        public string FullName { get { return this.nameSpace + ":attribute:" + this.name; } }

        public XSDAttribute(string filename, string name, string nameSpace, string type, bool isReference, string defaultValue, string use, XMLSchema.attribute attribute)
        {
            this.filename = filename;
            this.name = name;
            this.nameSpace = (nameSpace == null ? "" : nameSpace);
            this.type = type;
            this.isReference = isReference;
            this.defaultValue = defaultValue;
            this.use = use;
            this.tag = attribute;
        }

        public override string ToString()
        {
            return this.name + " (" + this.nameSpace + ")";
        }
    }

    public class XSDAttributeGroup
    {
        private string filename = "";
        private string name = "";
        private string nameSpace = "";
        private string type = "";
        private bool isReference = false;
        private XMLSchema.attributeGroup tag = null;

        public string Filename { get { return this.filename; } set { this.filename = value; } }
        public string Name { get { return this.name; } set { this.name = value; } }
        public string NameSpace { get { return this.nameSpace; } set { this.nameSpace = value; } }
        public string Type { get { return this.type; } set { this.type = value; } }
        public bool IsReference { get { return this.isReference; } set { this.isReference = value; } }
        public XMLSchema.attributeGroup Tag { get { return this.tag; } set { this.tag = value; } }

        public string FullName { get { return this.nameSpace + ":attributeGroup:" + this.name; } }

        public XSDAttributeGroup(string filename, string name, string nameSpace, string type, bool isReference, XMLSchema.attributeGroup attributeGroup)
        {
            this.filename = filename;
            this.name = name;
            this.nameSpace = (nameSpace == null ? "" : nameSpace);
            this.type = type;
            this.isReference = isReference;
            this.tag = attributeGroup;
        }

        public override string ToString()
        {
            return this.name + " (" + this.nameSpace + ")";
        }
    }
}
