#include "jackTokenizer.hpp"
#include <unordered_map>

namespace {
bool hasNonSpace(const std::string &str) {
  return str.find_first_not_of("\t\n\r ") != std::string::npos;
}

bool isSymbol(const char c) {
  return c == '{' || c == '}' || c == '(' || c == ')' || c == '[' || c == ']' ||
         c == ';' || c == '.' || c == ',' || c == '+' || c == '-' || c == '*' ||
         c == '/' || c == '&' || c == '|' || c == '<' || c == '>' || c == '=' ||
         c == '~';
}
bool isKeyword(const std::string &token) {
  return token == "class" || token == "constructor" || token == "function" ||
         token == "method" || token == "field" || token == "static" ||
         token == "var" || token == "int" || token == "char" ||
         token == "boolean" || token == "void" || token == "true" ||
         token == "false" || token == "null" || token == "this" ||
         token == "let" || token == "do" || token == "if" || token == "else" ||
         token == "while" || token == "return";
}
} // namespace

jackTokenizer::jackTokenizer(std::ifstream &&ifstream)
    : stream{std::move(ifstream)} {
  cleanCode();
  getNextToken();
}

void jackTokenizer::advance() {
  currentToken = nextToken;
  getNextToken();
}

jackTokenizer::Token jackTokenizer::tokenType() const {
  if (currentToken.size() == 1 && isSymbol(currentToken[0])) {
    return jackTokenizer::Token::Symbol;
  }
  if (std::isdigit(currentToken[0])) {
    return jackTokenizer::Token::Int_Const;
  }
  if (currentToken[0] == '"')
    return jackTokenizer::Token::String_Const;
  if (isKeyword(currentToken))
    return jackTokenizer::Token::Keyword;
  return jackTokenizer::Token::Identifier;
}

jackTokenizer::Keyword jackTokenizer::keyword() const {
  const static std::unordered_map<std::string_view, jackTokenizer::Keyword>
      keywordMap = {
          {"class", jackTokenizer::Keyword::Class},
          {"method", jackTokenizer::Keyword::Method},
          {"constructor", jackTokenizer::Keyword::Constructor},
          {"function", jackTokenizer::Keyword::Function},
          {"boolean", jackTokenizer::Keyword::Boolean},
          {"int", jackTokenizer::Keyword::Int},
          {"char", jackTokenizer::Keyword::Char},
          {"void", jackTokenizer::Keyword::Void},
          {"var", jackTokenizer::Keyword::Var},
          {"static", jackTokenizer::Keyword::Static},
          {"field", jackTokenizer::Keyword::Field},
          {"let", jackTokenizer::Keyword::Let},
          {"do", jackTokenizer::Keyword::Do},
          {"if", jackTokenizer::Keyword::If},
          {"else", jackTokenizer::Keyword::Else},
          {"while", jackTokenizer::Keyword::While},
          {"return", jackTokenizer::Keyword::Return},
          {"true", jackTokenizer::Keyword::True},
          {"false", jackTokenizer::Keyword::False},
          {"null", jackTokenizer::Keyword::Null},
          {"this", jackTokenizer::Keyword::This},
      };
  return keywordMap.at(currentToken);
}

void jackTokenizer::cleanCode() {
  std::string line;
  bool openComment = false;
  bool openString = false;

  while (std::getline(stream, line)) {
    std::string instruction;

    for (size_t i = 0; i < line.size(); ++i) {
      const auto &currentLetter = line[i];
      const auto &nextLetter = i < line.size() - 1 ? line[i + 1] : '\0';

      if (!openString && !openComment) {
        if (currentLetter == '"')
          openString = true;
        else if (currentLetter == '/' && nextLetter == '/')
          break;
        else if (currentLetter == '/' && nextLetter == '*') {
          openComment = true;
          ++i;
          continue;
        }
      }

      else if (openString && currentLetter == '"')
        openString = false;
      else if (openComment && currentLetter == '*' && nextLetter == '/') {
        openComment = false;
        ++i;
        continue;
      } else if (openComment)
        continue;

      instruction += currentLetter;
    }

    instruction += " ";
    if (!hasNonSpace(instruction))
      instruction.clear();

    cleanStream << instruction;
  }
}

void jackTokenizer::getNextToken() {
  nextToken.clear();

  cleanStream >> std::ws;

  if (cleanStream.eof())
    return;

  char nextChar = cleanStream.peek();

  if (isSymbol(nextChar)) {
    nextToken = cleanStream.get();
  }

  else if (nextChar == '"') {
    cleanStream.get();
    for (char c = cleanStream.get(); c != '"' && cleanStream.good();
         c = cleanStream.get()) {
      nextToken += c;
    }
    nextToken += '"';
  }

  else {
    while (!isSymbol(nextChar) && !std::isspace(nextChar) &&
           cleanStream.good()) {
      nextToken += cleanStream.get();
      nextChar = cleanStream.peek();
    }
  }
}
