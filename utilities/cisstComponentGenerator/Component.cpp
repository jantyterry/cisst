/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */
/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */
/* $Id: Component.cpp 1030 2010-06-11 jkriss1 $ */

#include <string>
#include "Component.h"

#include <iostream>
#include <fstream>
using std::cout; using std::endl;
using std::string;

Component::Component()
{

}

string Component::writeStateTableData()
{
	string s = "StateTable.AddData(";
	STData data = table.data.front();
	table.data.pop_front();
	
	s += data.name + ", \"" + data.name + "\");";

	return s;
}

string Component::writeProvInterface()
{
	InterfaceProv interf = provInterfaces.front();
	string n = interf.name.substr(1, interf.name.length()-2);

	string s = "\t\tprovided = AddProvidedInterface(" + interf.name + ");\n";

	int size = interf.commands.size();
	for(int i=0; i < size; i++)
	{
		Command command = interf.commands.front();
		
		s += "\t\t\tprovided->AddCommand";
		if(command.type == "Void" || command.type == "Write" || command.type == "QualifiedRead"){
			s += command.type + "(&" + name + "::" + command.func + ", this, " + command.name;

			if(command.type == "Write" && command.arg1 != "void")
				s += ", " + command.arg1;
			
			s += ");\n";
		}
		else if(command.type == "Read") {
			if(command.arg2 == "void")
				s += command.type + "(&" + name + "::" + command.func + ", " + command.arg1 + ", " + command.name + ");\n";
			else
				s += command.type + "(&" + command.arg2 + "::" + command.func + ", " + command.arg1 + ", " + command.name + ");\n";
		}
		else if(command.type == "ReadState")
			s += command.type + "(StateTable, " + command.func + ", " + command.name + ");\n";

		interf.commands.pop_front();
	}

	size = interf.events.size();
	for(int i=0; i < size; i++)
	{
		Event e = interf.events.front();

//		if(e.type == "Write")
//			s += "\t\t" + e.arg1 + " " + e.arg2 + ";\n";
		
		s += "\t\t\tprovided->AddEvent";
		
			s += e.type + "(" + e.function + ", " + e.name;
		if(e.type == "Write")
			s += ", " + e.arg1 + ");\n";
		else
			s += ");\n";

		interf.events.pop_front();
	}

	provInterfaces.pop_front();
	return s;
}

string Component::writeReqInterface()
{
	InterfaceReq interf = reqInterfaces.front();
	string n = interf.name.substr(1, interf.name.length()-2);

	string s = "\t\trequired = AddRequiredInterface(" + interf.name + ");\n";

	int size = interf.functions.size();
	for(int i=0; i < size; i++)
	{
		Function function = interf.functions.front();
		
		s += "\t\trequired->AddFunction";
		s += "(" + function.name + ", " + interf.type + "." + function.type + ");\n";

		interf.functions.pop_front();
	}

	size = interf.handlers.size();
	for(int i=0; i < size; i++)
	{
		EventHandler handler = interf.handlers.front();
		
		s += "\t\trequired->AddEventHandler" + handler.type;
		s += "(&" + name + "::" + handler.function + ", this, " + handler.name;
		if(handler.arg != "void")
			s += ", " + handler.arg;
		s += ");\n";

		interf.handlers.pop_front();
	}

	reqInterfaces.pop_front();

	return s;
}

void Component::generateCode(string filename)
	{
		std::ofstream file;

		cout << filename << endl;

		file.open(filename.c_str());

		file << "/* -*- Mode: C++; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*-    */" << endl;
		file << "/* ex: set filetype=cpp softtabstop=4 shiftwidth=4 tabstop=4 cindent expandtab: */" << endl;
		file << "/* File " << getName() << "_init.cpp generated by CisstBuilder*/\n" << endl;

		file << "#include \"" << getName() << ".h\"\n" << endl;

		file << "\tvoid " << getName() << "::InitComponent(void) {\n" << endl;
		int size = stateTableSize();
		if(size > 0)
			file << "\t//State Table variables" << endl;
		for(int i=0; i < size; i++)
			file << "\t\t" << writeStateTableData() << endl;

		size = getNumProvInt();

		if(size > 0) {
			file << "\t//Provided Interfaces" << endl;
			file << "\t\tmtsProvidedInterface * provided;" << endl;
		}

		for(int i=0; i < size; i++)
			file << writeProvInterface() << endl;
		
		size = getNumReqInt();

		if(size > 0) {
			file << "\t//Required Interfaces" << endl;
			file << "\t\tmtsRequiredInterface * required;" << endl;
		}

		for(int i=0; i < size; i++)
			file << writeReqInterface() << endl;

		file << "\n};" << endl;

		file.close();
	}