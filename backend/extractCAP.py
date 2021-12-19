from flask_restful import Api, Resource
from xml.etree import ElementTree
# transform CAP XML data to python dictionary (variable name, value)
def extract_cap_(data):
    values = ElementTree.fromstring(data).findall('info/parameter') # to find the env values from the xml data
    timestamp = ElementTree.fromstring(data).find('sent').text.strip() # to find the timestmp from the xml file (not pass as a parameter)
    data_dict = {}
    for value in values:
        data_dict[value.find('name').text.strip()] = value.find('value').text.strip()
    return values
    