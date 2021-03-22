#ifndef __CRONA_SCANNER_HPP__
#define __CRONA_SCANNER_HPP__ 1

#if ! defined(yyFlexLexerOnce)
#include <FlexLexer.h>
#endif

#include "grammar.hh"
#include "errors.hpp"

using TokenKind = crona::Parser::token;

namespace crona{

class Scanner : public yyFlexLexer{
public:
   
   Scanner(std::istream *in) : yyFlexLexer(in)
   {
	lineNum = 1;
	colNum = 1;
	hasError = false;
   };
   virtual ~Scanner() {
   };

   //get rid of override virtual function warning
   using FlexLexer::yylex;

   // YY_DECL defined in the flex crona.l
   virtual int yylex( crona::Parser::semantic_type * const lval);

   int makeBareToken(int tagIn){
        this->yylval->transToken = new Token(
	  this->lineNum, this->colNum, tagIn);
        colNum += static_cast<size_t>(yyleng);
        return tagIn;
   }

   void errIllegal(size_t l, size_t c, std::string match){
	Report::fatal(l, c, "Illegal character "
		+ match);
	hasError = true;
   }

   void errStrEsc(size_t l, size_t c){
	Report::fatal(l, c, "String literal with bad"
	" escape sequence ignored");
	hasError = true;
   }

   void errStrUnterm(size_t l, size_t c){
	Report::fatal(l, c, "Unterminated string"
	" literal ignored");
	hasError = true;
	
   }

   void errStrEscAndUnterm(size_t l, size_t c){
	Report::fatal(l, c, "Unterminated string literal"
	" with bad escape sequence ignored");
	hasError = true;
   }

   void errIntOverflow(size_t l, size_t c){
	Report::fatal(l, c, "Integer literal too large;"
	" using max value");
	hasError = true;
   }

   void warn(int lineNumIn, int colNumIn, std::string msg){
	std::cerr << lineNumIn << ":" << colNumIn 
		<< " ***WARNING*** " << msg << std::endl;
   }

   void error(int lineNumIn, int colNumIn, std::string msg){
	std::cerr << lineNumIn << ":" << colNumIn 
		<< " ***ERROR*** " << msg << std::endl;
   }

   static std::string tokenKindString(int tokenKind);

   void outputTokens(std::ostream& outstream);

private:
   crona::Parser::semantic_type *yylval = nullptr;
   size_t lineNum;
   size_t colNum;
   bool hasError;
};

} /* end namespace */

#endif /* END __CRONA_SCANNER_HPP__ */
