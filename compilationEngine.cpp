#include "compilationEngine.hpp"
#include "jackTokenizer.hpp"
#include <string>

void compilationEngine::compileVar() {
  readToken();
  readToken();
  readToken();
  while (tokenizer.tokenType() == jackTokenizer::Token::Symbol &&
         tokenizer.symbol() == ',') {
    readToken();
    readToken();
  }
  readToken();
}

void compilationEngine::compileClass() {
  tokenizer.advance();
  readToken();
  className = readToken();
  readToken();

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
  readToken();
}

void compilationEngine::compileClassVarDec() { compileVar(); }

void compilationEngine::compileSubroutine() {
  readToken(); // function type
  readToken(); // return type
  readToken(); // function name
  readToken(); // (
  compileParameterList();
  readToken(); // )
  compileSubroutineBody();
}

void compilationEngine::compileParameterList() {
  while (!(tokenizer.tokenType() == jackTokenizer::Token::Symbol &&
           tokenizer.symbol() == ')')) {
    readToken();
  }
}

void compilationEngine::compileSubroutineBody() {
  readToken(); // {
  while (tokenizer.tokenType() == jackTokenizer::Token::Keyword &&
         tokenizer.keyword() == jackTokenizer::Keyword::Var) {
    compileVarDec();
  }
  compileStatements();
  readToken(); // }
}

void compilationEngine::compileVarDec() { compileVar(); }

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
  readToken();
  readToken();
  if (tokenizer.tokenType() == jackTokenizer::Token::Symbol &&
      tokenizer.symbol() == '[') {
    readToken();
    compileExpression();
    readToken();
  }
  readToken();
  compileExpression();
  readToken();
}

void compilationEngine::compileIf() {
  readToken();
  readToken();
  compileExpression();
  readToken();
  readToken();
  compileStatements();
  readToken();
  if (tokenizer.tokenType() == jackTokenizer::Token::Keyword &&
      tokenizer.keyword() == jackTokenizer::Keyword::Else) {
    readToken();
    readToken();
    compileStatements();
    readToken();
  }
}

void compilationEngine::compileWhile() {
  readToken();
  readToken();
  compileExpression();
  readToken();
  readToken();
  compileStatements();
  readToken();
}

void compilationEngine::compileDo() {
  readToken();
  readToken();
  if (tokenizer.tokenType() == jackTokenizer::Token::Symbol &&
      tokenizer.symbol() == '(') {
    readToken();
    compileExpressionList();
    readToken();
  } else {
    readToken();
    readToken();
    readToken();
    compileExpressionList();
    readToken();
  }

  readToken();
}

void compilationEngine::compileReturn() {
  readToken();
  if (!(tokenizer.tokenType() == jackTokenizer::Token::Symbol &&
        tokenizer.symbol() == ';'))
    compileExpression();
  readToken();
}

void compilationEngine::compileExpression() {
  compileTerm();
  while (tokenizer.tokenType() == jackTokenizer::Token::Symbol &&
         ((tokenizer.symbol() == '+' || tokenizer.symbol() == '-' ||
           tokenizer.symbol() == '*' || tokenizer.symbol() == '/' ||
           tokenizer.symbol() == '&' || tokenizer.symbol() == '|' ||
           tokenizer.symbol() == '<' || tokenizer.symbol() == '>' ||
           tokenizer.symbol() == '='))) {
    readToken();
    compileTerm();
  }
}

void compilationEngine::compileTerm() {
  if (tokenizer.tokenType() == jackTokenizer::Token::Identifier) {
    readToken();
    if (tokenizer.tokenType() == jackTokenizer::Token::Symbol &&
        tokenizer.symbol() == '.') {
      readToken();
      readToken();
      readToken();
      compileExpressionList();
      readToken();
    } else if (tokenizer.tokenType() == jackTokenizer::Token::Symbol &&
               tokenizer.symbol() == '(') {
      readToken();
      compileExpressionList();
      readToken();
    } else if (tokenizer.tokenType() == jackTokenizer::Token::Symbol &&
               tokenizer.symbol() == '[') {
      readToken();
      compileExpression();
      readToken();
    }
  } else if (tokenizer.tokenType() == jackTokenizer::Token::Symbol &&
             tokenizer.symbol() == '(') {
    readToken();
    compileExpression();
    readToken();
  } else if (tokenizer.tokenType() == jackTokenizer::Token::Symbol &&
             (tokenizer.symbol() == '-' || tokenizer.symbol() == '~')) {
    readToken();
    compileTerm();
  }

  else
    readToken();
}

int compilationEngine::compileExpressionList() {
  if (tokenizer.tokenType() == jackTokenizer::Token::Symbol &&
      tokenizer.symbol() == ')') {

    return 0;
  }

  compileExpression();
  size_t counter = 1;
  while (tokenizer.tokenType() == jackTokenizer::Token::Symbol &&
         tokenizer.symbol() == ',') {
    readToken();
    compileExpression();
    counter++;
  }
  return counter;
}

std::string compilationEngine::readToken() {
  std::string token;
  if (tokenizer.tokenType() == jackTokenizer::Token::Keyword) {
    token = tokenizer.keywordString();
  } else if (tokenizer.tokenType() == jackTokenizer::Token::Symbol) {
    token = tokenizer.symbol();
  } else if (tokenizer.tokenType() == jackTokenizer::Token::Identifier) {
    token = tokenizer.identifier();
  } else if (tokenizer.tokenType() == jackTokenizer::Token::Int_Const) {
    token = std::to_string(tokenizer.intVal());
  } else if (tokenizer.tokenType() == jackTokenizer::Token::String_Const) {
    token = tokenizer.stringVal();
  }
  if (tokenizer.hasMoreTokens())
    tokenizer.advance();
  return token;
}
