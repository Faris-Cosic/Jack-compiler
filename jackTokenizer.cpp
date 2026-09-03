#include "jackTokenizer.hpp"

namespace {
bool hasNonSpace(const std::string &str) {
  return str.find_first_not_of("\t\n\r ") != std::string::npos;
}
} // namespace

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

void jackTokenizer::getNextToken() {}
