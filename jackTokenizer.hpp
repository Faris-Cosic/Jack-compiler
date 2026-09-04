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

  char symbol() const { return currentToken[0]; };
  std::string identifier() const { return currentToken; };
  int intVal() const { return std::stoi(currentToken); };
  std::string stringVal() const {
    return currentToken.substr(1, currentToken.size() - 2);
  };

private:
  std::ifstream stream;
  std::string currentToken;
  std::string nextToken;
  std::stringstream cleanStream;

  void cleanCode();
  void getNextToken();
};
