#ifndef CRONA_NAME_ANALYSIS
#define CRONA_NAME_ANALYSIS

#include "ast.hpp"
#include "symbol_table.hpp"

namespace crona{

class NameAnalysis{
public:
	static NameAnalysis * build(ProgramNode * astIn){
		NameAnalysis * nameAnalysis = new NameAnalysis;
		SymbolTable * symTab = new SymbolTable();
		bool res = astIn->nameAnalysis(symTab);
		delete symTab;
		if (!res){ return nullptr; }

		nameAnalysis->ast = astIn;
		return nameAnalysis;
	}
	ProgramNode * ast;

private:
	NameAnalysis(){
	}
};

}

#endif
