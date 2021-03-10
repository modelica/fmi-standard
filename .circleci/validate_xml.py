from lxml import etree
import os

pwd = os.path.dirname(__file__)
pwd = os.path.dirname(pwd)

print("Parsing fmiModelDescription schema")
schema = etree.XMLSchema(file=os.path.join(pwd, 'schema', 'fmi3ModelDescription.xsd'))
parser = etree.XMLParser(schema=schema)

xml_files = [
    'alias_example.xml',
    'co_simulation.xml',
    'co_simulation_early_return.xml',
    'model_exchange.xml',
    'model_structure_example1.xml',
    'model_structure_example2.xml',
    'model_structure_example3.xml',
    'model_structure_example4.xml',
    'se_example_modelDescription.xml',
    'string_vector_example.xml',
    'structural_parameter_example.xml',
    'unit_definition.xml',
    'variable_types.xml',
    'X_network4FMI_modelDescription.xml',
]

for xml_file in xml_files:
    print("Parsing %s" % xml_file)
    etree.parse(os.path.join(pwd, 'docs', 'examples', xml_file), parser)

print("Parsing fmiBuildDescription schema")
schema = etree.XMLSchema(file=os.path.join(pwd, 'schema', 'fmi3BuildDescription.xsd'))
parser = etree.XMLParser(schema=schema)

xml_files = [
    'build_description_complex.xml',
    'build_description_simple.xml',
]

for xml_file in xml_files:
    print("Parsing %s" % xml_file)
    etree.parse(os.path.join(pwd, 'docs', 'examples', xml_file), parser)

print("Parsing fmiTerminalsAndIcons schema")
schema = etree.XMLSchema(file=os.path.join(pwd, 'schema', 'fmi3TerminalsAndIcons.xsd'))
parser = etree.XMLParser(schema=schema)

# TODO: parse "Terminals and Icons" examples
xml_files = [
    'X_network4FMI_terminalsAndIcons.xml',
]

for xml_file in xml_files:
    print("Parsing %s" % xml_file)
    etree.parse(os.path.join(pwd, 'docs', 'examples', xml_file), parser)
