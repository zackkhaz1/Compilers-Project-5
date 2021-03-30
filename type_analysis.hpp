#ifndef CRONA_TYPE_ANALYSIS
#define CRONA_TYPE_ANALYSIS

#include "ast.hpp"
#include "symbol_table.hpp"
#include "types.hpp"

class NameAnalysis;

namespace crona{

// An instance of this class will be passed over the entire
// AST. Rather than attaching types to each node, the 
// TypeAnalysis class contains a map from each ASTNode to it's
// DataType. Thus, instead of attaching a type field to most nodes,
// one can instead map the node to it's type, or lookup the node
// in the map.
class TypeAnalysis {

private:
	//The private constructor here means that the type analysis
	// can only be created via the static build function
	TypeAnalysis(){
		hasError = false;
	}

public:
	static TypeAnalysis * build(NameAnalysis * astRoot);
	//static TypeAnalysis * build();

	//The type analysis has an instance variable to say whether
	// the analysis failed or not. Setting this variable is much
	// less of a pain than passing a boolean all the way up to the
	// root during the TypeAnalysis pass. 
	bool passed(){
		return !hasError;
	}

	void setCurrentFnType(const FnType * type){
		currentFnType = type;
	}

	const FnType * getCurrentFnType(){
		return currentFnType;
	}

	
	//Set the type of a node. Note that the function name is 
	// overloaded: this 2-argument nodeType puts a value into the
	// map with a given type. 
	void nodeType(const ASTNode * node, const DataType * type){
		nodeToType[node] = type;
	}

	//Gets the type of a node already placed in the map. Note
	// that this function name is overloaded: the 1-argument nodeType
	// gets the type of the given node out of the map.
	const DataType * nodeType(const ASTNode * node){
		const DataType * res = nodeToType[node];
		if (res == nullptr){
			const char * msg = "No type for node ";
			throw new InternalError(msg);
		}
		return nodeToType[node];
	}

	//The following functions all report and error and 
	// tell the object that the analysis has failed. 
	void errWriteFn(size_t line, size_t col){
		hasError = true;
		Report::fatal(line, col,
			"Attempt to write a function");
	}
	void errWriteVoid(size_t line, size_t col){
		hasError = true;
		Report::fatal(line, col, 
			"Attempt to write void");
	}
	void errWriteArray(size_t line, size_t col){
		hasError = true;
		Report::fatal(line, col,
			"Attempt to write array");
	}
	void errReadFn(size_t line, size_t col){
		hasError = true;
		Report::fatal(line, col,
			"Attempt to read a function");
	}
	void errCallee(size_t line, size_t col){
		hasError = true;
		Report::fatal(line, col,
			"Attempt to call a "
			"non-function");
	}
	void errArgCount(size_t line, size_t col){
		hasError = true;
		Report::fatal(line, col,
			"Function call with wrong"
			" number of args");
	}
	void errArgMatch(size_t line, size_t col){
		hasError = true;
		Report::fatal(line, col, 
			"Type of actual does not match"
			" type of formal");
	}
	void errRetEmpty(size_t line, size_t col){
		hasError = true;
		Report::fatal(line, col, 
			"Missing return value");
	}
	void extraRetValue(size_t line, size_t col){
		hasError = true;
		Report::fatal(line, col, 
			"Return with a value in void"
			" function");
	}
	void errRetWrong(size_t line, size_t col){
		hasError = true;
		Report::fatal(line, col, 
			"Bad return value");
	}
	void errMathOpd(size_t line, size_t col){
		hasError = true;
		Report::fatal(line, col, 
			"Arithmetic operator applied"
			" to invalid operand");
	}
	void errRelOpd(size_t line, size_t col){
		hasError = true;
		Report::fatal(line, col,
			"Relational operator applied to"
			" non-numeric operand");
	}
	void errLogicOpd(size_t line, size_t col){
		hasError = true;
		Report::fatal(line, col,
			"Logical operator applied to"
			" non-bool operand");
	}
	void errIfCond(size_t line, size_t col){
		hasError = true;
		Report::fatal(line, col, 
			"Non-bool expression used as"
			" an if condition");
	}
	void errWhileCond(size_t line, size_t col){
		hasError = true;
		Report::fatal(line, col,
			"Non-bool expression used as"
			" a while condition");
	}
	void errEqOpd(size_t line, size_t col){
		hasError = true;
		Report::fatal(line, col, 
			"Invalid equality operand");
	}
	void errEqOpr(size_t line, size_t col){
		hasError = true;
		Report::fatal(line, col, 
			"Invalid equality operation");
	}
	void errAssignOpd(size_t line, size_t col){
		hasError = true;
		Report::fatal(line, col, 
			"Invalid assignment operand");
	}
	void errAssignOpr(size_t line, size_t col){
		hasError = true;
		Report::fatal(line, col, 
			"Invalid assignment operation");
	}
	void errArrayID(size_t line, size_t col){
		hasError = true;
		Report::fatal(line, col, "Attempt to index a non-array");
	}

	void errArrayIndex(size_t line, size_t col){
		hasError = true;
		Report::fatal(line, col, "Bad index type");
	}
private:
	HashMap<const ASTNode *, const DataType *> nodeToType;
	const FnType * currentFnType;
	bool hasError;
public:
	ProgramNode * ast;
};

}
#endif
