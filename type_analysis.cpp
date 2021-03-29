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

 ta->nodeType(this, ta->getCurrentFnType());
    std::list<const DataType *> * formals = new std::list<const DataType *>();
    for(auto formal : *(this->myFormals))
    {
        auto fType = formal->getTypeNode()->getType();
        formals->push_back(fType);
    }
    auto ret = this->getRetTypeNode()->getType();
    FnType * functionType = new FnType(formals, ret);
    ta->setCurrentFnType(functionType);
    for (auto stmt : *myBody)
    {
        stmt->typeAnalysis(ta);
    }

}

void StmtNode::typeAnalysis(TypeAnalysis * ta){
	TODO("Implement me in the subclass");
}

void AssignStmtNode::typeAnalysis(TypeAnalysis * ta){
	myExp->typeAnalysis(ta);
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

void PostDecStmtNode::typeAnalysis(TypeAnalysis * ta) {
	myLVal->typeAnalysis(ta);
	auto lValType = ta->nodeType(myLVal);
	if(!lValType->isInt())
	{
		ta->errMathOpd(myLVal->line(), myLVal->col());
		ta->nodeType(this,ErrorType::produce());
	}
	ta->nodeType(this, BasicType::produce(VOID));
}

void PostIncStmtNode::typeAnalysis(TypeAnalysis * ta) {
	myLVal->typeAnalysis(ta);
	auto lValType = ta->nodeType(myLVal);
	if(!lValType->isInt())
	{
		ta->errMathOpd(myLVal->line(), myLVal->col());
		ta->nodeType(this,ErrorType::produce());
	}
	ta->nodeType(this, BasicType::produce(VOID));
}

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

void ReturnStmtNode::typeAnalysis(TypeAnalysis * ta){
	auto funcType = ta->getCurrentFnType();
	auto funcReturnType = funcType->getReturnType();

	if(myExp != NULL){
		if(funcReturnType != BasicType::VOID()){
			myExp->typeAnalysis(ta);
			auto subType = ta->nodeType(myExp);
			if((subType != funcReturnType) && !subType->asError()){
				ta->errRetWrong(myExp->line(), myExp->col());
				ta->nodeType(this, ErrorType::produce());
				return;
			}
		}
		else{
			myExp->typeAnalysis(ta);
			ta->extraRetValue(myExp->line(), myExp->col());
			ta->nodeType(this, ErrorType::produce());
			return;
		}
	}
	if(funcReturnType != BasicType::VOID()){
		ta->errRetEmpty(this->line(), this->col());
		ta->nodeType(this, ErrorType::produce());
		return;
	}
	ta->nodeType(this, BasicType::VOID());
}

void CallStmtNode::typeAnalysis(TypeAnalysis * ta){
	myCallExp->typeAnalysis(ta);
	ta->nodeType(this, BasicType::produce(VOID));
}

void ExpNode::typeAnalysis(TypeAnalysis * ta){
	TODO("Override me in the subclass");
}

void AssignExpNode::typeAnalysis(TypeAnalysis * ta){
	myDst->typeAnalysis(ta);
	mySrc->typeAnalysis(ta);
	auto tgtType = ta->nodeType(myDst);
	auto srcType = ta->nodeType(mySrc);

	if(tgtType->asError() || srcType->asError()){
		ta->nodeType(this, ErrorType::produce());
		return;
	}

	if (tgtType == srcType){
		ta->nodeType(this, tgtType);
		return;
	}

	// print "Type check failed" at the end
	ta->errAssignOpr(this->line(), this->col());
	// set the current node type
	ta->nodeType(this, ErrorType::produce());
}

void DeclNode::typeAnalysis(TypeAnalysis * ta){
	TODO("Override me in the subclass");
}

void VarDeclNode::typeAnalysis(TypeAnalysis * ta){
	// VarDecls always pass type analysis, since they
	// are never used in an expression
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

static bool opdTypeAnalysis(TypeAnalysis * ta, ExpNode * opd, std::string opdCase){
	bool validOpd = true;
	opd->typeAnalysis(ta);
	auto type = ta->nodeType(opd);

	if(opdCase == "mathOpd"){
		if(type->isInt() || type->isByte()){
			validOpd = true;
		}
		else{
			ta->errMathOpd(opd->line(), opd->col());
		  ta->nodeType(opd, ErrorType::produce());
			validOpd = false;
		}
	}
	if(opdCase == "logicOpd"){
		if(type->isBool()){
			validOpd = true;
		}
		else{
			ta->errLogicOpd(opd->line(), opd->col());
			ta->nodeType(opd, ErrorType::produce());
			validOpd = false;
		}
	}
	if(opdCase == "eqOpd"){
		if(type->isBool() || type->isByte() || type->isInt()){
			validOpd = true;
		}
		else{
			ta->errEqOpd(opd->line(), opd->col());
			ta->nodeType(opd, ErrorType::produce());
			validOpd = false;
		}
	}
	if(opdCase == "relOpd"){
		if(type->isInt() || type->isByte()){
			validOpd = true;
		}
		else{
			ta->errRelOpd(opd->line(), opd->col());
			ta->nodeType(opd, ErrorType::produce());
			validOpd = false;
		}
	}
	return validOpd;
}

void BinaryExpNode::mathTypeAnalysis(TypeAnalysis * ta){
	bool validExp1 = opdTypeAnalysis(ta, myExp1, "mathOpd");
	bool validExp2 = opdTypeAnalysis(ta, myExp2, "mathOpd");
	if(validExp1 && validExp2){
		auto myExp1Type = ta->nodeType(myExp1);
		auto myExp2Type = ta->nodeType(myExp2);
		if(myExp1Type->isInt() && myExp2Type->isInt()){
			ta->nodeType(this, BasicType::produce(INT));
			return;
		}
		if((myExp1Type->isInt() && myExp2Type->isByte()) || (myExp1Type->isByte() && myExp2Type->isInt())){
			ta->nodeType(this, BasicType::produce(INT));
			return;
		}
		if(myExp1Type->isByte() && myExp2Type->isByte()){
			ta->nodeType(this, BasicType::produce(BYTE));
			return;
		}
	}
	ta->nodeType(this, ErrorType::produce());
}

void PlusNode::typeAnalysis(TypeAnalysis * ta){
	mathTypeAnalysis(ta);
}

void MinusNode::typeAnalysis(TypeAnalysis * ta){
	mathTypeAnalysis(ta);
}

void TimesNode::typeAnalysis(TypeAnalysis * ta){
	mathTypeAnalysis(ta);
}

void DivideNode::typeAnalysis(TypeAnalysis * ta){
	mathTypeAnalysis(ta);
}

void BinaryExpNode::logicTypeAnalysis(TypeAnalysis * ta){
	bool validExp1 = opdTypeAnalysis(ta, myExp1, "logicOpd");
	bool validExp2 = opdTypeAnalysis(ta, myExp2, "logicOpd");
	if(validExp1 && validExp2){
		auto myExp1Type = ta->nodeType(myExp1);
		auto myExp2Type = ta->nodeType(myExp2);
		if(myExp1Type->isBool() && myExp2Type->isBool()){
			ta->nodeType(this, BasicType::produce(BOOL));
			return;
		}
	}
	ta->nodeType(this, ErrorType::produce());
}

void AndNode::typeAnalysis(TypeAnalysis * ta){
	logicTypeAnalysis(ta);
}

void OrNode::typeAnalysis(TypeAnalysis * ta){
	logicTypeAnalysis(ta);
}

void BinaryExpNode::equalityTypeAnalysis(TypeAnalysis * ta){
	bool validExp1 = opdTypeAnalysis(ta, myExp1, "eqOpd");
	bool validExp2 = opdTypeAnalysis(ta, myExp2, "eqOpd");
	if(validExp1 && validExp2){
		auto myExp1Type = ta->nodeType(myExp1);
		auto myExp2Type = ta->nodeType(myExp2);
		if(myExp1Type == myExp2Type){
			ta->nodeType(this, BasicType::produce(BOOL));
			return;
		}
	}
	ta->errEqOpr(this->line(), this->col());
	ta->nodeType(this, ErrorType::produce());
}

void EqualsNode::typeAnalysis(TypeAnalysis * ta){
	equalityTypeAnalysis(ta);
}

void NotEqualsNode::typeAnalysis(TypeAnalysis * ta){
	equalityTypeAnalysis(ta);
}

void BinaryExpNode::relationalTypeAnalysis(TypeAnalysis * ta){
	bool validExp1 = opdTypeAnalysis(ta, myExp1, "relOpd");
	bool validExp2 = opdTypeAnalysis(ta, myExp2, "relOpd");
	if(validExp1 && validExp2){
		auto myExp1Type = ta->nodeType(myExp1);
		auto myExp2Type = ta->nodeType(myExp2);
		if(myExp1Type->isInt() && myExp2Type->isInt()){
			ta->nodeType(this, BasicType::produce(BOOL));
			return;
		}
		if((myExp1Type->isInt() && myExp2Type->isByte()) || (myExp1Type->isByte() && myExp2Type->isInt())){
			ta->nodeType(this, BasicType::produce(BOOL));
			return;
		}
		if(myExp1Type->isByte() && myExp2Type->isByte()){
			ta->nodeType(this, BasicType::produce(BOOL));
			return;
		}
	}
	ta->nodeType(this, ErrorType::produce());
}

void LessNode::typeAnalysis(TypeAnalysis * ta){
	relationalTypeAnalysis(ta);
}

void LessEqNode::typeAnalysis(TypeAnalysis * ta){
	relationalTypeAnalysis(ta);
}

void GreaterNode::typeAnalysis(TypeAnalysis * ta){
	relationalTypeAnalysis(ta);
}

void GreaterEqNode::typeAnalysis(TypeAnalysis * ta ){
	relationalTypeAnalysis(ta);
}

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
	ta->nodeType(this, BasicType::produce(INT));
}

void HavocNode::typeAnalysis(TypeAnalysis * ta){
	ta->nodeType(this, BasicType::produce(BOOL));
}

void StrLitNode::typeAnalysis(TypeAnalysis * ta){
	ArrayType * byteArr = ArrayType::produce(BasicType::produce(BYTE), 1);
	ta->nodeType(this, byteArr);
}

void TrueNode::typeAnalysis(TypeAnalysis * ta){
	ta->nodeType(this, BasicType::produce(BOOL));
}

void FalseNode::typeAnalysis(TypeAnalysis * ta){
	ta->nodeType(this, BasicType::produce(BOOL));
}

}
