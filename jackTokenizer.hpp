#pragma once
#include <fstream>
#include <sstream>
#include <string>

class jackTokenizer {
public:
  jackTokenizer(std::ifstream &&);

  enum class Token { Keyword, Symbol, Identifier, Int_Const, String_Const };
  enum class Keyword {
    Class,
    Method,
    Function,
    Constructor,
    Int,
    Boolean,
    Char,
    Void,
    Var,
    Static,
    Field,
    Let,
    Do,
    If,
    Else,
    While,
    Return,
    True,
    False,
    Null,
    This
  };

  bool hasMoreTokens() const { return !nextToken.empty(); };
  void advance();

  Token tokenType() const;
  Keyword keyword() const;

  char symbol() const;
  std::string identifier() const;
  int intVal() const;
  std::string stringVal() const;

private:
  std::ifstream stream;
  std::string currentToken;
  std::string nextToken;
  std::stringstream cleanStream;

  void cleanCode();
  void getNextToken();
};
