#include "compilationEngine.hpp"
#include <filesystem>
#include <iostream>

std::string getNewFileName(const std::filesystem::path &path);

void compileFile(const std::filesystem::path &);

int main(int argc, char *argv[]) {
  if (argc != 2) {
    std::cout << "\nInvalid argument";
    return 1;
  }
  const std::filesystem::path path(argv[1]);
  if (!std::filesystem::exists(path)) {
    std::cout << "Error, path does not exists\n";
    return 1;
  }

  if (std::filesystem::is_regular_file(path)) {
    compileFile(path);
  } else if (std::filesystem::is_directory(path)) {
    bool hasJackFiles = false;
    for (const auto &entry : std::filesystem::directory_iterator(path)) {
      if (std::filesystem::is_regular_file(entry.path()) &&
          entry.path().extension().string() == ".jack") {
        compileFile(entry.path());
        hasJackFiles = true;
      }
    }

    if (!hasJackFiles) {
      std::cout << "Error! The directory has no '.jack' files!";
      return 1;
    }
  }
  return 0;
}

void compileFile(const std::filesystem::path &filePath) {
  std::ifstream input{filePath.string()};

  const std::string newFileName = getNewFileName(filePath);
  std::ofstream output{newFileName};

  compilationEngine compilation{std::move(input), std::move(output)};
  compilation.compileClass();
}

std::string getNewFileName(const std::filesystem::path &path) {
  const auto fileName = path.filename();
  std::string newFileName;
  if (std::filesystem::is_regular_file(path)) {
    if (!path.has_extension())
      throw std::invalid_argument("Error, file has to end wit '.jack'");
    if (path.extension().string() != ".jack")
      throw std::invalid_argument("Error, file has to end wit '.jack'");

    newFileName = fileName.stem().string() + ".xml";
  } else if (std::filesystem::is_directory(path)) {
    newFileName = fileName.string() + ".xml";
  } else {
    throw std::invalid_argument(
        "Error in provided file, please provide a single .jack file or "
        "a folder containing .jack files");
  }
  return newFileName;
}
