#include "compilationEngine.hpp"
#include "jackTokenizer.hpp"

void compilationEngine::compileClass() {
  output << "<class>";
  tokenizer.advance();
  writeTag();
  writeTag();
  writeTag();

  while (tokenizer.tokenType() == jackTokenizer::Token::Keyword &&
         (tokenizer.keyword() == jackTokenizer::Keyword::Field ||
          tokenizer.keyword() == jackTokenizer::Keyword::Static)) {
    compileClassVarDec();
  }

  while (tokenizer.tokenType() == jackTokenizer::Token::Keyword &&
         (tokenizer.keyword() == jackTokenizer::Keyword::Constructor ||
          tokenizer.keyword() == jackTokenizer::Keyword::Function ||
          tokenizer.keyword() == jackTokenizer::Keyword::Method)) {
    compileSubroutine();
  }

  writeTag();
  output << "</class>";
}

void compilationEngine::writeTag() {
  if (tokenizer.tokenType() == jackTokenizer::Token::Keyword) {
    output << "<keyword>" << tokenizer.keywordString() << "</keyword>";
  } else if (tokenizer.tokenType() == jackTokenizer::Token::Symbol) {
    output << "<symbol>" << tokenizer.symbol() << "</symbol>";
  } else if (tokenizer.tokenType() == jackTokenizer::Token::Identifier) {
    output << "<identifier>" << tokenizer.identifier() << "</identifier>";
  } else if (tokenizer.tokenType() == jackTokenizer::Token::Int_Const) {
    output << "<integerConstant>" << tokenizer.intVal() << "</integerConstant>";
  } else if (tokenizer.tokenType() == jackTokenizer::Token::String_Const) {
    output << "<stringConstant>" << tokenizer.stringVal()
           << "</stringConstant>";
  }
  if (tokenizer.hasMoreTokens())
    tokenizer.advance();
}
