#pragma once
#include "jackTokenizer.hpp"
#include <fstream>

class compilationEngine {
public:
  compilationEngine(std::ifstream &&input, std::ofstream &&output)
      : tokenizer{std::move(input)}, output{std::move(output)} {};

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

  void compileExpressionList();

private:
  jackTokenizer tokenizer;
  std::ofstream output;
};
