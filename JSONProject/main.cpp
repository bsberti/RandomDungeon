#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

#include <iostream>
#include <fstream>
#include <sstream>

int main(int argc, char* argv[]) {

	std::ifstream file("entityLoader.json");
	std::stringstream buffer;
	buffer << file.rdbuf();
	std::string content = buffer.str();

	rapidjson::Document doc;
	doc.Parse(content.c_str());

	const rapidjson::Value& className = doc["className"];
	const rapidjson::Value& attributes = doc["attributes"];
	std::cout << "Classname: " << className.GetString() << std::endl;

	for (rapidjson::SizeType i = 0; i < attributes.Size(); i++) {
		const rapidjson::Value& attribute = attributes[i];
		std::string attrName = attribute["name"].GetString();
		int attrValue = attribute["value"].GetInt();
		
		std::cout << "Attribute: " << attrName << " | Value: " << attrValue << std::endl;
	}

	return 0;
}