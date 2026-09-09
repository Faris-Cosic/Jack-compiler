#include "compilationEngine.hpp"
#include "jackTokenizer.hpp"
#include <ctime>

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

void compilationEngine::compileClassVarDec() { compileVarDec(); }

void compilationEngine::compileSubroutine() {
  writeTag(); // function type
  writeTag(); // return type
  writeTag(); // function name
  writeTag(); // (
  compileParameterList();
  writeTag(); // )
  compileSubroutineBody();
}

void compilationEngine::compileParameterList() {
  while (!(tokenizer.tokenType() == jackTokenizer::Token::Symbol &&
           tokenizer.symbol() == ')')) {
    writeTag();
  }
}

void compilationEngine::compileSubroutineBody() {
  writeTag(); // {
  while (tokenizer.tokenType() == jackTokenizer::Token::Keyword &&
         tokenizer.keyword() == jackTokenizer::Keyword::Var) {
    compileVarDec();
  }
  compileStatements();
  writeTag(); // }
}

void compilationEngine::compileVarDec() {
  writeTag();
  writeTag();
  writeTag();
  while (tokenizer.tokenType() == jackTokenizer::Token::Symbol &&
         tokenizer.symbol() == ',') {
    writeTag();
    writeTag();
  }
  writeTag();
}

void compilationEngine::compileStatements() {
  while (!(tokenizer.tokenType() == jackTokenizer::Token::Symbol &&
           tokenizer.symbol() == '}')) {
    if (tokenizer.keyword() == jackTokenizer::Keyword::Let)
      compileLet();
    else if (tokenizer.keyword() == jackTokenizer::Keyword::If)
      compileIf();
    else if (tokenizer.keyword() == jackTokenizer::Keyword::While)
      compileWhile();
    else if (tokenizer.keyword() == jackTokenizer::Keyword::Do)
      compileDo();
    else if (tokenizer.keyword() == jackTokenizer::Keyword::Return)
      compileReturn();
  }
}

void compilationEngine::compileLet() {
  writeTag();
  writeTag();
  if (tokenizer.tokenType() == jackTokenizer::Token::Symbol &&
      tokenizer.symbol() == '[') {
    writeTag();
    compileExpression();
    writeTag();
  }
  writeTag();
  compileExpression();
  writeTag();
}

void compilationEngine::compileIf() {
  writeTag();
  writeTag();
  compileExpression();
  writeTag();
  writeTag();
  compileStatements();
  writeTag();
  if (tokenizer.tokenType() == jackTokenizer::Token::Keyword &&
      tokenizer.keyword() == jackTokenizer::Keyword::Else) {
    writeTag();
    writeTag();
    compileStatements();
    writeTag();
  }
}

void compilationEngine::compileWhile() {
  writeTag();
  writeTag();
  compileExpression();
  writeTag();
  writeTag();
  compileStatements();
  writeTag();
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
