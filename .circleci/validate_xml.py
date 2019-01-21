from lxml import etree
import os

print("Parsing the XSD schema...")
schema = etree.XMLSchema(file='schema/fmi3ModelDescription.xsd')

parser = etree.XMLParser(schema=schema)

print("Parsing examples...")
etree.parse('docs/examples/co_simulation.xml', parser)
etree.parse('docs/examples/model_exchange.xml', parser)
