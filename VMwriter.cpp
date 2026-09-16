#include "VMWriter.hpp"

namespace {
std::string segmentToString(const VMWriter::Segment seg) {
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
} // namespace

void VMWriter::writePush(const Segment seg, const size_t index) {
  output << "push " << segmentToString(seg) << " " << index << "\n";
}
