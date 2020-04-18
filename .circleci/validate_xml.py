from lxml import etree
import os

print("Parsing fmiModelDescription schema")
schema = etree.XMLSchema(file='schema/fmi3ModelDescription.xsd')
parser = etree.XMLParser(schema=schema)

xml_files = [
    'alias_example.xml',
    # 'build_configuration.xml',
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
    etree.parse(os.path.join('docs', 'examples', xml_file), parser)

print("Parsing fmiBuildConfiguration schema")
schema = etree.XMLSchema(file='schema/fmi3BuildConfiguration.xsd')
parser = etree.XMLParser(schema=schema)

xml_files = [
    'complex_build_configuration.xml',
    'simple_build_configuration.xml',
]

for xml_file in xml_files:
    print("Parsing %s" % xml_file)
    etree.parse(os.path.join('docs', 'examples', xml_file), parser)
