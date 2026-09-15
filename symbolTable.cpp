#include "symbolTable.hpp"
#include <stdexcept>

void symbolTable::reset() {
  counters = {};
  table.clear();
}

void symbolTable::define(const std::string &name, const std::string &type,
                         symbolTable::Kind kind) {
  auto &index = counters[static_cast<size_t>(kind)];
  symbol s{type, kind, index};
  table.insert({name, s});
  index++;
}

size_t symbolTable::varCount(const Kind kind) const {
  return counters[static_cast<size_t>(kind)];
}

symbolTable::Kind symbolTable::kindOf(const std::string &name) const {
  try {
    const auto &symbol = table.at(name);
    return symbol.kind;
  } catch (std::out_of_range &e) {
    return symbolTable::Kind::None;
  }
}
