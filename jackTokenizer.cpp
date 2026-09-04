#include "jackTokenizer.hpp"

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
} // namespace

jackTokenizer::jackTokenizer(std::ifstream &&ifstream)
    : stream{std::move(ifstream)} {
  cleanCode();
  getNextToken();
}

void jackTokenizer::advance() {
  getNextToken();
  currentToken = nextToken;
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
  }

  else {
    while (!isSymbol(nextChar) && !std::isspace(nextChar) &&
           cleanStream.good()) {
      nextToken += cleanStream.get();
      nextChar = cleanStream.peek();
    }
  }
}
