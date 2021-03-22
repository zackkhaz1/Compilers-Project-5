#include "tokens.hpp" // Get the class declarations
#include "grammar.hh" // Get the TokenKind definitions

namespace crona{

using TokenKind = crona::Parser::token;
using Lexeme = crona::Parser::semantic_type;

static std::string tokenKindString(int tokKind){
	switch(tokKind){
		case TokenKind::END: return "EOF";
		case TokenKind::AND: return "AND";
		case TokenKind::ARRAY: return "ARRAY";
		case TokenKind::ASSIGN: return "ASSIGN";
		case TokenKind::BOOL: return "BOOL";
		case TokenKind::BYTE: return "BYTE";
		case TokenKind::COLON: return "COLON";
		case TokenKind::COMMA: return "COMMA";
		case TokenKind::CROSS: return "CROSS";
		case TokenKind::CROSSCROSS: return "CROSSCROSS";
		case TokenKind::DASH: return "DASH";
		case TokenKind::DASHDASH: return "DASHDASH";
		case TokenKind::ELSE: return "ELSE";
		case TokenKind::EQUALS: return "EQUALS";
		case TokenKind::FALSE: return "FALSE";
		case TokenKind::HAVOC: return "HAVOC";
		case TokenKind::ID: return "ID";
		case TokenKind::IF: return "IF";
		case TokenKind::INT: return "INT";
		case TokenKind::INTLITERAL: return "INTLIT";
		case TokenKind::GREATER: return "GREATER";
		case TokenKind::GREATEREQ: return "GREATEREQ";
		case TokenKind::LBRACE: return "LBRACE";
		case TokenKind::LCURLY: return "LCURLY";
		case TokenKind::LESS: return "LESS";
		case TokenKind::LESSEQ: return "LESSEQ";
		case TokenKind::LPAREN: return "LPAREN";
		case TokenKind::NOT: return "NOT";
		case TokenKind::NOTEQUALS: return "NOTEQUALS";
		case TokenKind::OR: return "OR";
		case TokenKind::RBRACE: return "RBRACE";
		case TokenKind::RCURLY: return "RCURLY";
		case TokenKind::READ: return "READ";
		case TokenKind::RETURN: return "RETURN";
		case TokenKind::RPAREN: return "RPAREN";
		case TokenKind::SEMICOLON: return "SEMICOLON";
		case TokenKind::SLASH: return "SLASH";
		case TokenKind::STRING: return "STRING";
		case TokenKind::STAR: return "STAR";
		case TokenKind::STRLITERAL: return "STRINGLIT";
		case TokenKind::TRUE: return "TRUE";
		case TokenKind::VOID: return "VOID";
		case TokenKind::WHILE: return "WHILE";
		case TokenKind::WRITE: return "WRITE";
		default:	
			return "OTHER";
	}
	
}

Token::Token(size_t lineIn, size_t columnIn, int kindIn)
  : myLine(lineIn), myCol(columnIn), myKind(kindIn){
}

std::string Token::toString(){
	return tokenKindString(kind())
	+ " [" + std::to_string(line()) 
	+ "," + std::to_string(col()) + "]";
}

size_t Token::line() const { 
	return this->myLine; 
}

size_t Token::col() const { 
	return this->myCol; 
}

int Token::kind() const { 
	return this->myKind; 
}

IDToken::IDToken(size_t lIn, size_t cIn, std::string vIn)
  : Token(lIn, cIn, TokenKind::ID), myValue(vIn){ 
}

std::string IDToken::toString(){
	return tokenKindString(kind()) + ":"
	+ this->myValue
	+ " [" + std::to_string(line()) 
	+ "," + std::to_string(col()) + "]";
}

const std::string IDToken::value() const { 
	return this->myValue; 
}

StrToken::StrToken(size_t lIn, size_t cIn, std::string sIn)
  : Token(lIn, cIn, TokenKind::STRLITERAL), myStr(sIn){
}

std::string StrToken::toString(){
	return tokenKindString(kind()) + ":"
	+ this->myStr
	+ " [" + std::to_string(line()) 
	+ "," + std::to_string(col()) + "]";
}

const std::string StrToken::str() const {
	return this->myStr;
}

IntLitToken::IntLitToken(size_t lIn, size_t cIn, int numIn)
  : Token(lIn, cIn, TokenKind::INTLITERAL), myNum(numIn){}

std::string IntLitToken::toString(){
	return tokenKindString(kind()) + ":"
	+ std::to_string(this->myNum)
	+ " [" + std::to_string(line()) 
	+ "," + std::to_string(col()) + "]";
}

int IntLitToken::num() const {
	return this->myNum;
}

} //End namespace crona
