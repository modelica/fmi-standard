using System;
using System.Collections.Generic;
using System.Text;
using System.Text.RegularExpressions;

namespace XSDDiagram
{
    /// <summary>
    /// Helper functions to work with the diagram.
    /// </summary>
    public class DiagramHelpers
    {

        /// <summary>
        /// Gives the display string of a type.
        /// </summary>
        /// <param name="type">The type.</param>
        /// <param name="xmlQualifiedName">Name of the XML qualified.</param>
        /// <returns></returns>
        public static string QualifiedNameToFullName(string type, System.Xml.XmlQualifiedName xmlQualifiedName)
        {
            return xmlQualifiedName.Namespace + ':' + type + ':' + xmlQualifiedName.Name;
        }

        private static string QualifiedNameToAttributeTypeName(System.Xml.XmlQualifiedName xmlQualifiedName)
        {
            return xmlQualifiedName.Name + "  : " + xmlQualifiedName.Namespace;
        }


        private static void ParseComplexTypeAttributes(Schema schema, string nameSpace, List<XSDAttribute> listAttributes, XMLSchema.complexType complexType, bool isRestriction)
        {
            if (complexType.ItemsElementName != null)
            {
                for (int i = 0; i < complexType.ItemsElementName.Length; i++)
                {
                    switch (complexType.ItemsElementName[i])
                    {
                        case XMLSchema.ItemsChoiceType4.attribute:
                            {
                                XMLSchema.attribute attribute = complexType.Items[i] as XMLSchema.attribute;
                                ParseAttribute(schema, nameSpace, listAttributes, attribute, false);
                            }
                            break;
                        case XMLSchema.ItemsChoiceType4.attributeGroup:
                            {
                                XMLSchema.attributeGroup attributeGroup = complexType.Items[i] as XMLSchema.attributeGroup;
                                ParseAttributeGroup(schema, nameSpace, listAttributes, attributeGroup, false);
                            }
                            break;
                        case XMLSchema.ItemsChoiceType4.anyAttribute:
                            XMLSchema.wildcard wildcard = complexType.Items[i] as XMLSchema.wildcard;
                            XSDAttribute xsdAttribute = new XSDAttribute("", "*", wildcard.@namespace, "", false, null, null, null);
                            listAttributes.Add(xsdAttribute);
                            break;
                        case XMLSchema.ItemsChoiceType4.simpleContent:
                        case XMLSchema.ItemsChoiceType4.complexContent:
                            XMLSchema.annotated annotatedContent = null;
                            if (complexType.Items[i] is XMLSchema.complexContent)
                            {
                                XMLSchema.complexContent complexContent = complexType.Items[i] as XMLSchema.complexContent;
                                annotatedContent = complexContent.Item;
                            }
                            else if (complexType.Items[i] is XMLSchema.simpleContent)
                            {
                                XMLSchema.simpleContent simpleContent = complexType.Items[i] as XMLSchema.simpleContent;
                                annotatedContent = simpleContent.Item;
                            }
                            if (annotatedContent is XMLSchema.extensionType)
                            {
                                XMLSchema.extensionType extensionType = annotatedContent as XMLSchema.extensionType;
                                //XSDObject xsdExtensionType = this.schema.ElementsByName[QualifiedNameToFullName("type", extensionType.@base)] as XSDObject;
                                //if (xsdExtensionType != null)
                                XSDObject xsdExtensionType;
                                if (schema.ElementsByName.TryGetValue(QualifiedNameToFullName("type", extensionType.@base), out xsdExtensionType) && xsdExtensionType != null)
                                {
                                    XMLSchema.annotated annotatedExtension = xsdExtensionType.Tag as XMLSchema.annotated;
                                    if (annotatedExtension != null)
                                    {
                                        if (annotatedExtension is XMLSchema.complexType)
                                            ParseComplexTypeAttributes(schema, extensionType.@base.Namespace, listAttributes, annotatedExtension as XMLSchema.complexType, false);
                                    }
                                }
                                if (extensionType.Items != null)
                                {
                                    foreach (XMLSchema.annotated annotated in extensionType.Items)
                                    {
                                        if (annotated is XMLSchema.attribute)
                                        {
                                            ParseAttribute(schema, nameSpace, listAttributes, annotated as XMLSchema.attribute, false);
                                        }
                                        else if (annotated is XMLSchema.attributeGroup)
                                        {
                                            ParseAttributeGroup(schema, nameSpace, listAttributes, annotated as XMLSchema.attributeGroup, false);
                                        }
                                    }
                                }
                            }
                            else if (annotatedContent is XMLSchema.restrictionType)
                            {
                                XMLSchema.restrictionType restrictionType = annotatedContent as XMLSchema.restrictionType;
                                //XSDObject xsdRestrictionType = this.schema.ElementsByName[QualifiedNameToFullName("type", restrictionType.@base)] as XSDObject;
                                //if (xsdRestrictionType != null)
                                XSDObject xsdRestrictionType;
                                if (schema.ElementsByName.TryGetValue(QualifiedNameToFullName("type", restrictionType.@base), out xsdRestrictionType) && xsdRestrictionType != null)
                                {
                                    XMLSchema.annotated annotatedRestriction = xsdRestrictionType.Tag as XMLSchema.annotated;
                                    if (annotatedRestriction != null)
                                    {
                                        if (annotatedRestriction is XMLSchema.complexType)
                                            ParseComplexTypeAttributes(schema, restrictionType.@base.Namespace, listAttributes, annotatedRestriction as XMLSchema.complexType, false);
                                    }
                                }
                                if (restrictionType.Items1 != null)
                                {
                                    foreach (XMLSchema.annotated annotated in restrictionType.Items1)
                                    {
                                        if (annotated is XMLSchema.attribute)
                                        {
                                            ParseAttribute(schema, nameSpace, listAttributes, annotated as XMLSchema.attribute, true);
                                        }
                                        else if (annotated is XMLSchema.attributeGroup)
                                        {
                                            ParseAttributeGroup(schema, nameSpace, listAttributes, annotated as XMLSchema.attributeGroup, true);
                                        }
                                    }
                                }
                            }
                            break;
                    }
                }
            }
            else
            {
            }
        }

        private static XSDAttribute ParseAttribute(Schema schema, string nameSpace, List<XSDAttribute> listAttributes, XMLSchema.attribute attribute, bool isRestriction)
        {
            bool isReference = false;
            string filename = "";
            string name = attribute.name;
            string type = "";
            if (attribute.@ref != null)
            {
                object o = null;
                schema.AttributesByName.TryGetValue(QualifiedNameToFullName("attribute", attribute.@ref), out o);
                if (o is XSDAttribute)
                {
                    XSDAttribute xsdAttributeInstance = o as XSDAttribute;
                    XSDAttribute refXSDAttribute = ParseAttribute(schema, nameSpace, listAttributes, xsdAttributeInstance.Tag, isRestriction);
                    if (refXSDAttribute != null)
                    {
                        // Override the "use" field with 
                        refXSDAttribute.Use = attribute.use.ToString();
                    }
                    return null;
                }
                else // Reference not found!
                {
                    type = QualifiedNameToAttributeTypeName(attribute.@ref);
                    name = attribute.@ref.Name;
                    nameSpace = attribute.@ref.Namespace;
                    isReference = true;
                }
            }
            else if (attribute.type != null)
            {
                type = QualifiedNameToAttributeTypeName(attribute.type);
                nameSpace = attribute.type.Namespace;
            }
            else if (attribute.simpleType != null)
            {
                XMLSchema.simpleType simpleType = attribute.simpleType as XMLSchema.simpleType;
                if (simpleType.Item is XMLSchema.restriction)
                {
                    XMLSchema.restriction restriction = simpleType.Item as XMLSchema.restriction;
                    type = QualifiedNameToAttributeTypeName(restriction.@base);
                    nameSpace = restriction.@base.Namespace;
                }
                else if (simpleType.Item is XMLSchema.list)
                {
                    XMLSchema.list list = simpleType.Item as XMLSchema.list;
                    type = QualifiedNameToAttributeTypeName(list.itemType);
                    nameSpace = list.itemType.Namespace;
                }
                else
                {
                }
            }
            else
            {

            }
            if (string.IsNullOrEmpty(attribute.name) && string.IsNullOrEmpty(name))
            {
            }
            if (isRestriction)
            {
                if (attribute.use == XMLSchema.attributeUse.prohibited)
                {
                    foreach (XSDAttribute xsdAttribute in listAttributes)
                    {
                        if (xsdAttribute.Name == name)
                        {
                            //listAttributes.Remove(xsdAttribute);
                            xsdAttribute.Use = attribute.use.ToString();
                            break;
                        }
                    }
                }
            }
            else
            {
                XSDAttribute xsdAttribute = new XSDAttribute(filename, name, nameSpace, type, isReference, attribute.@default, attribute.use.ToString(), attribute);
                listAttributes.Insert(0, xsdAttribute);
                return xsdAttribute;

            }
            return null;
        }

        private static void ParseAttributeGroup(Schema schema, string nameSpace, List<XSDAttribute> listAttributes, XMLSchema.attributeGroup attributeGroup, bool isRestriction)
        {
            if (attributeGroup is XMLSchema.attributeGroupRef && attributeGroup.@ref != null)
            {
                object o = null;
                schema.AttributesByName.TryGetValue(QualifiedNameToFullName("attributeGroup", attributeGroup.@ref), out o);
                if (o is XSDAttributeGroup)
                {
                    XSDAttributeGroup xsdAttributeGroup = o as XSDAttributeGroup;
                    XMLSchema.attributeGroup attributeGroupInstance = xsdAttributeGroup.Tag;

                    foreach (XMLSchema.annotated annotated in attributeGroupInstance.Items)
                    {
                        if (annotated is XMLSchema.attribute)
                        {
                            ParseAttribute(schema, nameSpace, listAttributes, annotated as XMLSchema.attribute, isRestriction);
                        }
                        else if (annotated is XMLSchema.attributeGroup)
                        {
                            ParseAttributeGroup(schema, nameSpace, listAttributes, annotated as XMLSchema.attributeGroup, isRestriction);
                        }
                    }
                }
            }
            else
            {

            }
        }

        private static List<XSDAttribute> GetComplexTypeAttributes(Schema schema, XMLSchema.complexType complexType, string nameSpace)
        {
            List<XSDAttribute> listAttributes = new List<XSDAttribute>();
            DiagramHelpers.ParseComplexTypeAttributes(schema, nameSpace, listAttributes, complexType, false);
            return listAttributes;
        }

        /// <summary>
        /// Gets the annotated attributes.
        /// </summary>
        /// <param name="schema">The schema.</param>
        /// <param name="annotated">The annotated.</param>
        /// <param name="nameSpace">The name space.</param>
        /// <returns>The list of all the attributes.</returns>
        public static List<XSDAttribute> GetAnnotatedAttributes(Schema schema, XMLSchema.annotated annotated, string nameSpace)
        {
            // Attributes enumeration
            List<XSDAttribute> listAttributes = new List<XSDAttribute>();
            if (annotated is XMLSchema.element)
            {
                XMLSchema.element element = annotated as XMLSchema.element;

                if (element.Item is XMLSchema.complexType)
                {
                    XMLSchema.complexType complexType = element.Item as XMLSchema.complexType;
                    listAttributes.AddRange(GetComplexTypeAttributes(schema, complexType, nameSpace));
                }
                else if (element.type != null)
                {
                    //XSDObject xsdObject = this.schema.ElementsByName[DiagramHelpers.QualifiedNameToFullName("type", element.type)] as XSDObject;
                    //if (xsdObject != null)
                    XSDObject xsdObject;
                    if (schema.ElementsByName.TryGetValue(DiagramHelpers.QualifiedNameToFullName("type", element.type), out xsdObject) && xsdObject != null)
                    {
                        XMLSchema.annotated annotatedElement = xsdObject.Tag as XMLSchema.annotated;
                        if (annotatedElement is XMLSchema.complexType)
                        {
                            XMLSchema.complexType complexType = annotatedElement as XMLSchema.complexType;
                            listAttributes.AddRange(GetComplexTypeAttributes(schema, complexType, nameSpace));
                        }
                        else
                        {
                        }
                    }
                    else
                    {
                    }
                }
                else
                {
                }
            }
            else if (annotated is XMLSchema.complexType)
            {
                XMLSchema.complexType complexType = annotated as XMLSchema.complexType;
                listAttributes.AddRange(GetComplexTypeAttributes(schema, complexType, nameSpace));
            }
            else if (annotated is XMLSchema.simpleType)
            {
                XMLSchema.attribute attr = new XMLSchema.attribute();
                XMLSchema.localSimpleType def = new XMLSchema.localSimpleType();
                def.Item = (annotated as XMLSchema.simpleType).Item;
                attr.simpleType = def;
                string type = "";
                if (def.Item is XMLSchema.restriction) type = (def.Item as XMLSchema.restriction).@base.Name;
                XSDAttribute XSDattr = new XSDAttribute("filename", (annotated as XMLSchema.simpleType).name, "namespace", type, false, "", "", attr);
                listAttributes.Add(XSDattr);

            }
            return listAttributes;
        }


        public static string GetAnnotationText(XMLSchema.annotation annotation)
        {
            string textDocumentation = "";
            foreach (object o in annotation.Items)
            {
                if (o is XMLSchema.documentation)
                {
                    string text = "";
                    XMLSchema.documentation documentation = o as XMLSchema.documentation;
                    if (documentation.Any != null && documentation.Any.Length > 0 && documentation.Any[0].Value != null)
                    {
                        text = documentation.Any[0].Value;
                        text = text.Replace("\n", " ");
                        text = text.Replace("\t", " ");
                        text = text.Replace("\r", "");
                        text = Regex.Replace(text, " +", " ");
                        text = text.Trim();
                    }
                    else if (documentation.source != null)
                    {
                        text = documentation.source;
                    }
                    if(!string.IsNullOrEmpty(textDocumentation))
                        textDocumentation += "\r\n";
                    textDocumentation += text;
                    if (!string.IsNullOrEmpty(documentation.lang) || !string.IsNullOrEmpty(documentation.source))
                    {
                        textDocumentation += " (";
                        if (!string.IsNullOrEmpty(documentation.lang))
                            textDocumentation += documentation.lang;
                        if (!string.IsNullOrEmpty(documentation.lang) && !string.IsNullOrEmpty(documentation.source))
                            textDocumentation += ", ";
                        if (!string.IsNullOrEmpty(documentation.source))
                            textDocumentation += documentation.source;
                        textDocumentation += ")";
                    }
                }
            }

            return textDocumentation;
        }
    }
}
