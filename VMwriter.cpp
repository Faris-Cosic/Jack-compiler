#include "VMWriter.hpp"

namespace {
std::string enumToString(const VMWriter::Segment seg) {
  switch (seg) {
  case VMWriter::Segment::Constant:
    return "constant";
  case VMWriter::Segment::Argument:
    return "argument";
  case VMWriter::Segment::Local:
    return "local";
  case VMWriter::Segment::Pointer:
    return "pointer";
  case VMWriter::Segment::Static:
    return "static";
  case VMWriter::Segment::Temp:
    return "temp";
  case VMWriter::Segment::That:
    return "that";
  case VMWriter::Segment::This:
    return "this";
  }
}

std::string enumToString(const VMWriter::Command command) {
  switch (command) {
  case VMWriter::Command::Add:
    return "add";
  case VMWriter::Command::And:
    return "and";
  case VMWriter::Command::Eq:
    return "eq";
  case VMWriter::Command::Gt:
    return "gt";
  case VMWriter::Command::Lt:
    return "lt";
  case VMWriter::Command::Neg:
    return "neg";
  case VMWriter::Command::Not:
    return "not";
  case VMWriter::Command::Or:
    return "or";
  case VMWriter::Command::Sub:
    return "sub";
  }
}

} // namespace

void VMWriter::writePush(const Segment seg, const size_t index) {
  output << "push " << enumToString(seg) << " " << index << "\n";
}

void VMWriter::writePop(const Segment seg, const size_t index) {
  output << "pop " << enumToString(seg) << " " << index << "\n";
}

void VMWriter::writeArithmetic(const Command command) {
  output << enumToString(command) << "\n";
}
