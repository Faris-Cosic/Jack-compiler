#pragma once
#include <array>
#include <string>
#include <unordered_map>

class symbolTable {
public:
  symbolTable() {};

  enum class Kind { Static, Field, Arg, Var };

  void reset();

  void define(const std::string &name, const std::string &type, Kind kind);

  int varCount(const Kind) const;

  Kind kindOf(const std::string &) const;

  std::string typeOf(const std::string &) const;

  int indexOf(const std::string &) const;

private:
  std::array<size_t, 4> counters{};

  struct symbol {
    std::string type;
    Kind kind;
    size_t index;
  };

  std::unordered_map<std::string, symbol> table;
};
