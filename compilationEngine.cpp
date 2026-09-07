#include "compilationEngine.hpp"
#include "jackTokenizer.hpp"

void compilationEngine::compileClass() {
  tokenizer.advance();
  output << "<class><keyword>";
  tokenizer.advance();
  output << tokenizer.identifier();
  output << "</keyword>";
  tokenizer.advance();
  output << "<symbol>{</symbol>";
  tokenizer.advance();
  while (tokenizer.keyword() == jackTokenizer::Keyword::Field ||
         tokenizer.keyword() == jackTokenizer::Keyword::Static) {
    compileClassVarDec();
  }

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
}
