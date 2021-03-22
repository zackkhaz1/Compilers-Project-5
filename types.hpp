#ifndef CRONA_DATA_TYPES
#define CRONA_DATA_TYPES

#include <list>
#include <sstream>
#include "errors.hpp"

#include <unordered_map>

#ifndef CRONA_HASH_MAP_ALIAS
// Use an alias template so that we can use
// "HashMap" and it means "std::unordered_map"
template <typename K, typename V>
using HashMap = std::unordered_map<K, V>;
#endif


namespace crona{

class ASTNode;

class BasicType;
class FnType;
class ArrayType;
class ErrorType;

enum BaseType{
	INT, VOID, BOOL, BYTE
};

//This class is the superclass for all crona types. You
// can get information about which type is implemented
// concretely using the as<X> functions, or query information
// using the is<X> functions.
class DataType{
public:
	virtual std::string getString() const = 0;
	virtual const BasicType * asBasic() const { return nullptr; }
	virtual const ArrayType * asArray() const { return nullptr; }
	virtual const FnType * asFn() const { return nullptr; }
	virtual const ErrorType * asError() const { return nullptr; }
	virtual bool isVoid() const { return false; }
	virtual bool isInt() const { return false; }
	virtual bool isBool() const { return false; }
	virtual bool isByte() const { return false; }
	virtual bool isArray() const { return false; }
	virtual bool validVarType() const = 0 ;
	virtual size_t getSize() const = 0;
protected:
};

//This DataType subclass is the superclass for all crona types. 
// Note that there is exactly one instance of this 
class ErrorType : public DataType{
public:
	static ErrorType * produce(){
		//Note: this static member will only ever be initialized 
		// ONCE, no matter how many times the function is called.
		// That means there will only ever be 1 instance of errorType
		// in the entire codebase.
		static ErrorType * error = new ErrorType();
		
		return error;
	}
	virtual const ErrorType * asError() const override { return this; }
	virtual std::string getString() const override { 
		return "ERROR";
	}
	virtual bool validVarType() const override { return false; }
	virtual size_t getSize() const override { return 0; }
private:
	ErrorType(){ 
		/* private constructor, can only 
		be called from produce */
	}
	size_t line;
	size_t col;
};

//DataType subclass for all scalar types 
class BasicType : public DataType{
public:
	static BasicType * VOID(){
		return produce(BaseType::VOID);
	}
	static BasicType * BOOL(){
		return produce(BaseType::BOOL);
	}
	static BasicType * BYTE(){
		return produce(BaseType::BYTE);
	}
	static BasicType * INT(){
		return produce(BaseType::INT);
	}

	//Create a scalar type. If that type already exists,
	// return the known instance of that type. Making sure
	// there is only 1 instance of a class for a given set
	// of fields is known as the "flyweight" design pattern
	// and ensures that the memory needs of a program are kept
	// down: rather than having a distinct type for every base
	// INT (for example), only one is constructed and kept in
	// the flyweights list. That type is then re-used anywhere
	// it's needed. 

	//Note the use of the static function declaration, which 
	// means that no instance of BasicType is needed to call
	// the function.
	static BasicType * produce(BaseType base){
		//Note the use of the static local variable, which
		//means that the flyweights variable persists between
		// multiple calls to this function (it is essentially
		// a global variable that can only be accessed
		// in this function).
		static std::list<BasicType *> flyweights;
		for(BasicType * fly : flyweights){
			if (fly->getBaseType() == base){
				return fly;
			}
		}
		BasicType * newType = new BasicType(base);
		flyweights.push_back(newType);
		return newType;
	}
	const BasicType * asBasic() const override {
		return this;
	}
	BasicType * asBasic(){
		return this;
	}
	bool isInt() const override {
		return myBaseType == BaseType::INT;
	}
	bool isByte() const override {
		return myBaseType == BaseType::BYTE;
	}
	bool isBool() const override {
		return myBaseType == BaseType::BOOL;
	}
	virtual bool isVoid() const override { 
		return myBaseType == BaseType::VOID; 
	}
	virtual bool validVarType() const override {
		return !isVoid();
	}
	virtual BaseType getBaseType() const { return myBaseType; }
	virtual std::string getString() const override;
	virtual size_t getSize() const override { 
		if (isBool()){ return 1; }
		else if (isByte()){ return 1; }
		else if (isVoid()){ return 8; }
		else if (isInt()){ return 8; }
		else { return 0; }
	}
private:
	BasicType(BaseType base) 
	: myBaseType(base){ }
	BaseType myBaseType;
};

class ArrayType : public DataType{
public:
	static ArrayType * produce(const BasicType * basicType, int length){
		//Note the use of the static local variable, which
		//means that the flyweights variable persists between
		// multiple calls to this function (it is essentially
		// a global variable that can only be accessed
		// in this function).
		static std::list<ArrayType *> flyweights;
		for(ArrayType * fly : flyweights){
			if (fly->myBasicType == basicType){
				if (fly->myLength == length){
					return fly;
				}
			}
		}
		ArrayType * newType = new ArrayType(basicType, length);
		flyweights.push_back(newType);
		return newType;
	}

	std::string getString() const override{
		std::string res = myBasicType->getString();
		return res + " array [" + std::to_string(myLength) + "]";
	}

	static const DataType * baseType(const DataType * type){
		const ArrayType * asArr = type->asArray();
		if (asArr == nullptr){ return type; }
		return asArr->myBasicType;
	}

	virtual bool validVarType() const override {
		return !myBasicType->isVoid();
	}
	bool isArray() const override { return true; } 
	const ArrayType * asArray() const override { return this; }
	int getLength(){ return myLength; }
	size_t getSize() const override { 
		const size_t lonLength = static_cast<size_t>(myLength);
		return lonLength * myBasicType->getSize(); 
	}
	
private:
	ArrayType(const BasicType * basicType, int length)
	: myBasicType(basicType), myLength(length){
		/* private constructor, can only be called from produce */
	}
	const BasicType * myBasicType;
	int myLength;
};

//DataType subclass to represent the type of a function. It will
// have a list of argument types and a return type. 
class FnType : public DataType{
public:
	FnType(const std::list<const DataType *>* formalsIn, const DataType * retTypeIn) 
	: DataType(),
	  myFormalTypes(formalsIn),
	  myRetType(retTypeIn)
	{
	}
	std::string getString() const override{
		std::string result = "";
		bool first = true;
		for (auto elt : *myFormalTypes){
			if (first) { first = false; }
			else { result += ","; }
			result += elt->getString();
		}
		result += "->";
		result += myRetType->getString();
		return result;
	}
	virtual const FnType * asFn() const override { return this; }

	const DataType * getReturnType() const {
		return myRetType;
	}
	const std::list<const DataType *> * getFormalTypes() const {
		return myFormalTypes;
	}
	virtual bool validVarType() const override { return false; }
	virtual size_t getSize() const override { return 0; }
private:
	const std::list<const DataType *> * myFormalTypes;
	const DataType * myRetType;
};

}

#endif
