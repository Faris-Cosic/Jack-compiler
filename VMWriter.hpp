#pragma once
#include <fstream>
#include <string>

class VMwriter {
public:
  vmWriter(std::ofstream &&stream) : output(std::move(stream)) {}

  enum class Segment {
    Constant,
    Argument,
    Local,
    Static,
    This,
    That,
    Pointer,
    Temp
  };

  enum class Command { Add, Sub, Neg, Eq, Gt, Lt, And, Or, Not };

  void writePush(const Segment seg, const size_t index);
  void writePop(const Segment seg, const size_t index);

  void writeArithmetic(const Command command);

  void writeLabel(const std::string &label);
  void writeGoto(const std::string &label);
  void writeIf(const std::string &label);

  void writeCall(const std::string &name, const size_t nArgs);
  void writeFunction(const std::string &name, const size_t nArgs);

  void writeReturn();
  void close();

private:
  std::ofstream output;
};
