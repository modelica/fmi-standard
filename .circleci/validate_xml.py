from lxml import etree
import os

top = os.path.abspath(__file__)
top = os.path.dirname(top)
top = os.path.dirname(top)

print("Parsing the XSD schema")
schema = etree.XMLSchema(file=top + '/schema/fmi3ModelDescription.xsd')

parser = etree.XMLParser(schema=schema)

xml_files = [
    'alias_example.xml',
    'build_configuration.xml',
    'co_simulation.xml',
    'co_simulation_early_return.xml',
    'model_exchange.xml',
    'model_structure_example1.xml',
    'model_structure_example2.xml',
    'model_structure_example3.xml',
    'model_structure_example4.xml',
    'unit_definition.xml',
    'variable_types.xml',
]

for xml_file in xml_files:
    print("Parsing %s" % xml_file)
    etree.parse(os.path.join(top, 'docs', 'examples', xml_file), parser)
