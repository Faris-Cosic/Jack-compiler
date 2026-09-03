#include "jackTokenizer.hpp"

void jackTokenizer::getNextInstruction() {
  std::string line;
  currentInstruction.clear();

  bool openComment = false;
  bool openString = false;
  std::string instruction;

  while (std::getline(stream, line) &&
         (currentInstruction.empty() || openComment)) {

    if (!instruction.empty())
      instruction += ' ';

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

    if (!instruction.empty())
      currentInstruction = instruction;
  }
}
