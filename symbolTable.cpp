#include "symbolTable.hpp"

void symbolTable::reset() {
  varCounter = 0, staticCounter = 0, fieldCounter = 0, argCounter = 0;
  table.clear();
}
