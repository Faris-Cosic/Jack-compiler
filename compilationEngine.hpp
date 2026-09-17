#pragma once
#include "VMWriter.hpp"
#include "jackTokenizer.hpp"
#include "symbolTable.hpp"
#include <fstream>

class compilationEngine {
public:
  compilationEngine(std::ifstream &&input, std::ofstream &&output)
      : tokenizer{std::move(input)}, vmWriter{std::move(output)} {};

  void compileClass();

  void compileClassVarDec();

  void compileSubroutine();

  void compileParameterList();

  void compileSubroutineBody();

  void compileVarDec();

  void compileStatements();

  void compileLet();

  void compileIf();

  void compileWhile();

  void compileDo();

  void compileReturn();

  void compileExpression();

  void compileTerm();

  int compileExpressionList();

private:
  jackTokenizer tokenizer;
  VMWriter vmWriter;
  std::string className;

  symbolTable classTable;
  symbolTable subroutineTable;

  std::string readToken();

  void compileVar();
};
