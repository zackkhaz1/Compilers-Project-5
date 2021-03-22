#include <fstream>
#include "scanner.hpp"

using namespace crona;

using TokenKind = crona::Parser::token;
using Lexeme = crona::Parser::semantic_type;

void Scanner::outputTokens(std::ostream& outstream){
	Lexeme lexeme;
	int tokenKind;
	while(true){
		tokenKind = this->yylex(&lexeme);
		if (tokenKind == TokenKind::END){
			outstream << "EOF" 
			  << " [" << this->lineNum 
			  << "," << this->colNum << "]"
			  << std::endl;
			return;
		} else {
			outstream << lexeme.transToken->toString()
			  << std::endl;
		}
	}
}
