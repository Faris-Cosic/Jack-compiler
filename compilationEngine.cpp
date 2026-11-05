#include "compilationEngine.hpp"
#include "VMWriter.hpp"
#include "jackTokenizer.hpp"
#include "symbolTable.hpp"
#include <stdexcept>
#include <string>

namespace {
VMWriter::Segment kindToSegment(symbolTable::Kind kind) {
  switch (kind) {
  case symbolTable::Kind::Static:
    return VMWriter::Segment::Static;
  case symbolTable::Kind::Arg:
    return VMWriter::Segment::Argument;
  case symbolTable::Kind::Field:
    return VMWriter::Segment::This;
  case symbolTable::Kind::Var:
    return VMWriter::Segment::Local;
  case symbolTable::Kind::None:
    throw std::invalid_argument("Error!");
  }
}

struct kindSegment {
  VMWriter::Segment seg;
  size_t index;
};

kindSegment getKindSegment(const symbolTable *table, const std::string &name) {
  kindSegment ks{kindToSegment(table->kindOf(name)), table->indexOf(name)};
  return ks;
}

} // namespace

const symbolTable *compilationEngine::findTable(const std::string &name) const {
  if (subroutineTable.kindOf(name) != symbolTable::Kind::None)
    return &subroutineTable;
  if (classTable.kindOf(name) != symbolTable::Kind::None)
    return &classTable;
  return nullptr;
}

void compilationEngine::pushVariable(const std::string &name) {
  const auto *table = findTable(name);
  if (!table)
    throw std::runtime_error("Error, variable is not initialized!");
  const auto kindSegment = getKindSegment(table, name);
  vmWriter.writePush(kindSegment.seg, kindSegment.index);
}

void compilationEngine::popVariable(const std::string &name) {
  const auto *table = findTable(name);
  if (!table)
    throw std::runtime_error("Error, variable is not initialized!");
  const auto kindSegment = getKindSegment(table, name);
  vmWriter.writePop(kindSegment.seg, kindSegment.index);
}

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
    const std::string type = readToken();
    const std::string name = readToken();
    subroutineTable.define(name, type, symbolTable::Kind::Arg);
    if (tokenizer.tokenType() == jackTokenizer::Token::Symbol &&
        tokenizer.symbol() == ',') {
      readToken();
    }
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
  const std::string varName = readToken();
  bool isArray = false;
  if (tokenizer.tokenType() == jackTokenizer::Token::Symbol &&
      tokenizer.symbol() == '[') {
    isArray = true;
    readToken();
    pushVariable(varName);
    compileExpression();
    readToken();
    vmWriter.writeArithmetic(VMWriter::Command::Add);
  }
  readToken();
  compileExpression();
  readToken();
  if (isArray) {
    vmWriter.writePop(VMWriter::Segment::Temp, 0);
    vmWriter.writePop(VMWriter::Segment::Pointer, 1);
    vmWriter.writePush(VMWriter::Segment::Temp, 0);
    vmWriter.writePop(VMWriter::Segment::That, 0);
  } else
    popVariable(varName);
}

void compilationEngine::compileIf() {
  const size_t currentId = labelCounter++;
  const std::string falseLabel = "IF_FALSE" + std::to_string(currentId);
  const std::string endLabel = "IF_END" + std::to_string(currentId);
  readToken();
  readToken();
  compileExpression();
  readToken();
  readToken();
  vmWriter.writeArithmetic(VMWriter::Command::Not);
  vmWriter.writeIf(falseLabel);
  compileStatements();
  readToken();

  if (tokenizer.tokenType() == jackTokenizer::Token::Keyword &&
      tokenizer.keyword() == jackTokenizer::Keyword::Else) {
    vmWriter.writeGoto(endLabel);
    vmWriter.writeLabel(falseLabel);
    readToken();
    readToken();
    compileStatements();
    readToken();
    vmWriter.writeLabel(endLabel);
  } else {
    vmWriter.writeLabel(falseLabel);
  }
}

void compilationEngine::compileWhile() {
  const size_t currentId = labelCounter++;
  const std::string whileLabel = "WHILE_EXP" + std::to_string(currentId);
  const std::string endWhileLabel = "WHILE_END" + std::to_string(currentId);
  vmWriter.writeLabel(whileLabel);
  readToken(); // while
  readToken(); // (
  compileExpression();
  vmWriter.writeArithmetic(VMWriter::Command::Not);
  readToken(); // )
  readToken(); // {
  vmWriter.writeIf(endWhileLabel);
  compileStatements();
  vmWriter.writeGoto(whileLabel);
  vmWriter.writeLabel(endWhileLabel);
  readToken(); // }
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
  const static std::unordered_map<char, VMWriter::Command> symbolMap = {
      {'+', VMWriter::Command::Add}, {'-', VMWriter::Command::Sub},
      {'=', VMWriter::Command::Eq},  {'>', VMWriter::Command::Gt},
      {'<', VMWriter::Command::Lt},  {'&', VMWriter::Command::And},
      {'|', VMWriter::Command::Or},
  };
  compileTerm();
  while (tokenizer.tokenType() == jackTokenizer::Token::Symbol &&
         (symbolMap.find(tokenizer.symbol()) != symbolMap.cend() ||
          tokenizer.symbol() == '*' || tokenizer.symbol() == '/')) {

    const auto symbol = tokenizer.symbol();
    readToken();
    compileTerm();
    if (symbol == '*')
      vmWriter.writeCall("Math.multiply", 2);
    else if (symbol == '/')
      vmWriter.writeCall("Math.divide", 2);
    else
      vmWriter.writeArithmetic(symbolMap.at(symbol));
  }
}

void compilationEngine::compileTerm() {
  if (tokenizer.tokenType() == jackTokenizer::Token::Keyword) {
    if (tokenizer.keywordString() == "true") {
      vmWriter.writePush(VMWriter::Segment::Constant, 0);
      vmWriter.writeArithmetic(VMWriter::Command::Not);
    } else if (tokenizer.keywordString() == "false") {
      vmWriter.writePush(VMWriter::Segment::Constant, 0);
    } else if (tokenizer.keywordString() == "null")
      vmWriter.writePush(VMWriter::Segment::Constant, 0);
    else if (tokenizer.keywordString() == "this")
      vmWriter.writePush(VMWriter::Segment::Pointer, 0);
    readToken();
  }

  else if (tokenizer.tokenType() == jackTokenizer::Token::Identifier) {
    const std::string name = readToken();
    if (tokenizer.tokenType() == jackTokenizer::Token::Symbol &&
        tokenizer.symbol() == '.') {
      readToken();
      const std::string functionName = readToken();
      readToken();

      std::string callee = name;

      size_t numArgs = 0;

      const auto *table = findTable(name);

      if (table) {
        pushVariable(name);
        callee = table->typeOf(name);
        numArgs++;
      }

      numArgs += compileExpressionList();
      readToken();
      vmWriter.writeCall(callee + "." + functionName, numArgs);
    } else if (tokenizer.tokenType() == jackTokenizer::Token::Symbol &&
               tokenizer.symbol() == '(') {
      readToken();
      vmWriter.writePush(VMWriter::Segment::Pointer, 0);
      const size_t numArgs = compileExpressionList() + 1;
      const std::string fullName = className + "." + name;
      readToken();
      vmWriter.writeCall(fullName, numArgs);
    } else if (tokenizer.tokenType() == jackTokenizer::Token::Symbol &&
               tokenizer.symbol() == '[') {

      pushVariable(name);
      readToken();
      compileExpression();
      readToken();

      vmWriter.writeArithmetic(VMWriter::Command::Add);
      vmWriter.writePop(VMWriter::Segment::Pointer, 1);
      vmWriter.writePush(VMWriter::Segment::That, 0);
    } else {
      pushVariable(name);
    }

  } else if (tokenizer.tokenType() == jackTokenizer::Token::Symbol &&
             tokenizer.symbol() == '(') {
    readToken();
    compileExpression();
    readToken();
  } else if (tokenizer.tokenType() == jackTokenizer::Token::Symbol &&
             (tokenizer.symbol() == '-' || tokenizer.symbol() == '~')) {
    if (tokenizer.symbol() == '-') {
      readToken();
      compileTerm();
      vmWriter.writeArithmetic(VMWriter::Command::Neg);
    } else {
      readToken();
      compileTerm();
      vmWriter.writeArithmetic(VMWriter::Command::Not);
    }
  } else if (tokenizer.tokenType() == jackTokenizer::Token::String_Const) {
    const std::string string = readToken();
    const size_t length = string.length();
    vmWriter.writePush(VMWriter::Segment::Constant, length);
    vmWriter.writeCall("String.new", 1);
    for (const char s : string) {
      vmWriter.writePush(VMWriter::Segment::Constant, s);
      vmWriter.writeCall("String.appendChar", 2);
    }
  }

  else {
    vmWriter.writePush(VMWriter::Segment::Constant, tokenizer.intVal());
    readToken();
  }
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
    token = tokenizer.keywordString();
  } else if (tokenizer.tokenType() == jackTokenizer::Token::Int_Const) {
    token = std::to_string(tokenizer.intVal());
  } else if (tokenizer.tokenType() == jackTokenizer::Token::String_Const) {
    token = tokenizer.stringVal();
  }
  if (tokenizer.hasMoreTokens())
    tokenizer.advance();
  return token;
}
