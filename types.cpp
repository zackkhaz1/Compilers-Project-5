#include <list>
#include <sstream>

#include "types.hpp"
#include "ast.hpp"

namespace crona{

std::string BasicType::getString() const{
	std::string res = "";
	switch(myBaseType){
	case BaseType::INT:
		res += "int";
		break;
	case BaseType::BOOL:
		res += "bool";
		break;
	case BaseType::VOID:
		res += "void";
		break;
	case BaseType::BYTE:
		res += "byte";
		break;
	}
	return res;
}

DataType * ByteTypeNode::getType() { 
	return BasicType::BYTE(); 
}

DataType * BoolTypeNode::getType() { 
	return BasicType::BOOL(); 
}

DataType * IntTypeNode::getType() { 
	return BasicType::INT(); 
}


} //End namespace
