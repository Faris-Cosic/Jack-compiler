#include "compilationEngine.hpp"
#include "VMWriter.hpp"
#include "symbolTable.hpp"
#include <string>

void compilationEngine::compileVar(symbolTable &table, symbolTable::Kind kind) {
  const std::string type = readToken();
  std::string name = readToken();

  table.define(name, type, kind);
  while (tokenizer.tokenType() == jackTokenizer::Token::Symbol &&
         tokenizer.symbol() == ',') {
    readToken();
    name = readToken();
    table.define(name, type, kind);
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

void compilationEngine::compileClassVarDec() {
  const std::string kindString = readToken();

  symbolTable::Kind kind = kindString == "field" ? symbolTable::Kind::Field
                                                 : symbolTable::Kind::Static;
  compileVar(classTable, kind);
}

void compilationEngine::compileSubroutine() {
  subroutineTable.reset();

  const std::string functionType = readToken();
  readToken();
  const std::string currentFunction = readToken();
  readToken();
  if (functionType == "method") {
    subroutineTable.define("this", className, symbolTable::Kind::Arg);
  }
  compileParameterList();
  readToken();
  compileSubroutineBody(currentFunction, functionType);
}

void compilationEngine::compileParameterList() {
  while (!(tokenizer.tokenType() == jackTokenizer::Token::Symbol &&
           tokenizer.symbol() == ')')) {
    readToken();
  }
}

void compilationEngine::compileSubroutineBody(
    const std::string &currentFunction,
    const std::string &currentFunctionType) {

  readToken(); // {
  while (tokenizer.tokenType() == jackTokenizer::Token::Keyword &&
         tokenizer.keyword() == jackTokenizer::Keyword::Var) {
    compileVarDec();
  }

  const std::string fullFunctionName = className + '.' + currentFunction;
  const size_t varCount = subroutineTable.varCount(symbolTable::Kind::Var);
  vmWriter.writeFunction(fullFunctionName, varCount);
  if (currentFunctionType == "constructor") {
    const size_t fieldCount = classTable.varCount(symbolTable::Kind::Field);
    vmWriter.writePush(VMWriter::Segment::Constant, fieldCount);
    vmWriter.writeCall("Memory.alloc", 1);
    vmWriter.writePop(VMWriter::Segment::Pointer, 0);
  } else if (currentFunctionType == "method") {
    vmWriter.writePush(VMWriter::Segment::Argument, 0);
    vmWriter.writePop(VMWriter::Segment::Pointer, 0);
  }
  compileStatements();
  readToken(); // }
}

void compilationEngine::compileVarDec() {
  readToken();
  compileVar(subroutineTable, symbolTable::Kind::Var);
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
