#include "ast.hpp"
#include "symbol_table.hpp"
#include "errors.hpp"
#include "types.hpp"
#include "name_analysis.hpp"
#include "type_analysis.hpp"

namespace crona{

TypeAnalysis * TypeAnalysis::build(NameAnalysis * nameAnalysis){
	//To emphasize that type analysis depends on name analysis
	// being complete, a name analysis must be supplied for
	// type analysis to be performed.
	TypeAnalysis * typeAnalysis = new TypeAnalysis();
	auto ast = nameAnalysis->ast;
	typeAnalysis->ast = ast;

	ast->typeAnalysis(typeAnalysis);
	if (typeAnalysis->hasError){
		return nullptr;
	}

	return typeAnalysis;

}

void ProgramNode::typeAnalysis(TypeAnalysis * ta){

	//pass the TypeAnalysis down throughout
	// the entire tree, getting the types for
	// each element in turn and adding them
	// to the ta object's hashMap
	for (auto global : *myGlobals){
		global->typeAnalysis(ta);
	}

	//The type of the program node will never
	// be needed. We can just set it to VOID
	//(Alternatively, we could make our type
	// be error if the DeclListNode is an error)
	ta->nodeType(this, BasicType::produce(VOID));
}

void FnDeclNode::typeAnalysis(TypeAnalysis * ta){

	//HINT: you might want to change the signature for
	// typeAnalysis on FnBodyNode to take a second
	// argument which is the type of the current
	// function. This will help you to know at a
	// return statement whether the return type matches
	// the current function

	//Note: this function may need extra code

	for (auto stmt : *myBody){
		stmt->typeAnalysis(ta);
	}
}

void StmtNode::typeAnalysis(TypeAnalysis * ta){
	TODO("Implement me in the subclass");
}

void AssignStmtNode::typeAnalysis(TypeAnalysis * ta){
	myExp->typeAnalysis(ta);

	//It can be a bit of a pain to write
	// "const DataType *" everywhere, so here
	// the use of auto is used instead to tell the
	// compiler to figure out what the subType variable
	// should be
	auto subType = ta->nodeType(myExp);

	// As error returns null if subType is NOT an error type
	// otherwise, it returns the subType itself
	if (subType->asError()){
		ta->nodeType(this, subType);
	} else {
		ta->nodeType(this, BasicType::produce(VOID));
	}
}

void ReadStmtNode::typeAnalysis(TypeAnalysis * ta){
	myDst->typeAnalysis(ta);
	auto subType = ta->nodeType(myDst);
	if(subType->asFn()){
		ta->errReadFn(myDst->line(), myDst->col());
		ta->nodeType(this, ErrorType::produce());
	}
	else{
		ta->nodeType(this, BasicType::produce(VOID));
	}
}

void WriteStmtNode::typeAnalysis(TypeAnalysis * ta){
	mySrc->typeAnalysis(ta);
	auto subType = ta->nodeType(mySrc);
	if(subType->asFn()){
		ta->errWriteFn(mySrc->line(), mySrc->col());
		ta->nodeType(this, ErrorType::produce());
	}
	else if(subType->isVoid()){
		ta->errWriteVoid(mySrc->line(), mySrc->col());
		ta->nodeType(this, ErrorType::produce());
	}
	else if(subType->asArray()){
		ta->errWriteArray(mySrc->line(), mySrc->col());
		ta->nodeType(this, ErrorType::produce());
	}
	else{
		ta->nodeType(this, BasicType::produce(VOID));
	}
}

void PostDecStmtNode::typeAnalysis(TypeAnalysis * ta) {}

void PostIncStmtNode::typeAnalysis(TypeAnalysis * ta) {}

void IfStmtNode::typeAnalysis(TypeAnalysis * ta){
	myCond->typeAnalysis(ta);
	auto condType = ta->nodeType(myCond);
	if(!condType->isBool() && !condType->asError()){
		ta->errIfCond(myCond->line(), myCond->col());
		ta->nodeType(this, ErrorType::produce());
	}
	
	for(auto stmt : *myBody){
		stmt->typeAnalysis(ta);
	}
	ta->nodeType(this, BasicType::produce(VOID));
}

void IfElseStmtNode::typeAnalysis(TypeAnalysis * ta){
	myCond->typeAnalysis(ta);
	auto condType = ta->nodeType(myCond);
	if(!condType->isBool() && !condType->asError()){
		ta->errIfCond(myCond->line(), myCond->col());
		ta->nodeType(this, ErrorType::produce());
	}
	
	for(auto stmt : *myBodyTrue){
		stmt->typeAnalysis(ta);
	}

	for(auto stmt : *myBodyFalse){
		stmt->typeAnalysis(ta);
	}
	ta->nodeType(this, BasicType::produce(VOID));
}

void WhileStmtNode::typeAnalysis(TypeAnalysis * ta){
	myCond->typeAnalysis(ta);
	auto condType = ta->nodeType(myCond);
	if(!condType->isBool() && !condType->asError()){
		ta->errWhileCond(myCond->line(), myCond->col());
		ta->nodeType(this, ErrorType::produce());
	}
	
	for(auto stmt : *myBody){
		stmt->typeAnalysis(ta);
	}
	ta->nodeType(this, BasicType::produce(VOID));
}

void ReturnStmtNode::typeAnalysis(TypeAnalysis * ta) {}

void CallStmtNode::typeAnalysis(TypeAnalysis * ta){
	myCallExp->typeAnalysis(ta);
	ta->nodeType(this, BasicType::produce(VOID));
}

void ExpNode::typeAnalysis(TypeAnalysis * ta){
	TODO("Override me in the subclass");
}

void AssignExpNode::typeAnalysis(TypeAnalysis * ta){
	//TODO: Note that this function is incomplete.
	// and needs additional code

	//Do typeAnalysis on the subexpressions
	myDst->typeAnalysis(ta);
	mySrc->typeAnalysis(ta);

	const DataType * tgtType = ta->nodeType(myDst);
	const DataType * srcType = ta->nodeType(mySrc);

	//While incomplete, this gives you one case for
	// assignment: if the types are exactly the same
	// it is usually ok to do the assignment. One
	// exception is that if both types are function
	// names, it should fail type analysis
	if (tgtType == srcType){
		ta->nodeType(this, tgtType);
		return;
	}

	//Some functions are already defined for you to
	// report type errors. Note that these functions
	// also tell the typeAnalysis object that the
	// analysis has failed, meaning that main.cpp
	// will print "Type check failed" at the end
	ta->errAssignOpr(this->line(), this->col());


	//Note that reporting an error does not set the
	// type of the current node, so setting the node
	// type must be done
	ta->nodeType(this, ErrorType::produce());
}

void DeclNode::typeAnalysis(TypeAnalysis * ta){
	TODO("Override me in the subclass");
}

void VarDeclNode::typeAnalysis(TypeAnalysis * ta){
	// VarDecls always pass type analysis, since they 
	// are never used in an expression. You may choose
	// to type them void (like this), as discussed in class
	ta->nodeType(this, BasicType::produce(VOID));
}

void IDNode::typeAnalysis(TypeAnalysis * ta){
	ta->nodeType(this, this->getSymbol()->getDataType());
}

void IndexNode::typeAnalysis(TypeAnalysis * ta) {
	myBase->typeAnalysis(ta);
	myOffset->typeAnalysis(ta);
	auto type_base = ta->nodeType(myBase);
	auto type_offset = ta->nodeType(myOffset);
	if(type_base->asError() || type_offset->asError())
	{
		ta->nodeType(this, ErrorType::produce());
		return;
	}

	if(type_offset->isInt() == false)
	{
		ta->nodeType(this, ErrorType::produce());
		ta->errArrayIndex(myOffset->line(),myOffset->col());
	}

}

void CallExpNode::typeAnalysis(TypeAnalysis * ta) {}

void PlusNode::typeAnalysis(TypeAnalysis * ta) {}

void MinusNode::typeAnalysis(TypeAnalysis * ta) {}

void TimesNode::typeAnalysis(TypeAnalysis * ta) {}

void DivideNode::typeAnalysis(TypeAnalysis * ta) {}

void AndNode::typeAnalysis(TypeAnalysis * ta) {}

void OrNode::typeAnalysis(TypeAnalysis * ta) {}

void EqualsNode::typeAnalysis(TypeAnalysis * ta) {}

void NotEqualsNode::typeAnalysis(TypeAnalysis * ta) {}

void LessNode::typeAnalysis(TypeAnalysis * ta) {}

void LessEqNode::typeAnalysis(TypeAnalysis * ta) {}

void GreaterNode::typeAnalysis(TypeAnalysis * ta) {}

void GreaterEqNode::typeAnalysis(TypeAnalysis * ta ){}

void NegNode::typeAnalysis(TypeAnalysis * ta){
	myExp->typeAnalysis(ta);
	auto subType = ta->nodeType(myExp);
	if(!subType->isInt() && !subType->asError()){
		ta->errMathOpd(myExp->line(), myExp->col());
		ta->nodeType(this, ErrorType::produce());
		return;
	}
	ta->nodeType(this, subType);
}

void NotNode::typeAnalysis(TypeAnalysis * ta){
	myExp->typeAnalysis(ta);
	auto subType = ta->nodeType(myExp);
	if(!subType->isBool() && !subType->asError()){
		ta->errLogicOpd(myExp->line(), myExp->col());
		ta->nodeType(this, ErrorType::produce());
		return;
	}
	ta->nodeType(this, subType);
}

void IntLitNode::typeAnalysis(TypeAnalysis * ta){
	// IntLits never fail their type analysis and always
	// yield the type INT
	ta->nodeType(this, BasicType::produce(INT));
}

void HavocNode::typeAnalysis(TypeAnalysis * ta){
	ta->nodeType(this, BasicType::produce(VOID));
}

void StrLitNode::typeAnalysis(TypeAnalysis * ta){
	ArrayType * byteArr = ArrayType::produce(BasicType::BYTE(), 1);
	ta->nodeType(this, byteArr);
}

void TrueNode::typeAnalysis(TypeAnalysis * ta){
	ta->nodeType(this, BasicType::produce(BOOL));
}

void FalseNode::typeAnalysis(TypeAnalysis * ta){
	ta->nodeType(this, BasicType::produce(BOOL));
}

}
