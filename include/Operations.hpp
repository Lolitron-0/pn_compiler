#pragma once
#include "Types.hpp"
#include <array>
#include <functional>
#include <iostream>
#include <statement.h>

namespace pn {

template <size_t N>
using StackView = std::array<ValueT, N>;

template <size_t Args, size_t Results>
using OpPolicySignature =
    std::array<ValueT, Results> (*)(const std::array<ValueT, Args>&);

template <size_t Args, size_t Results,
          OpPolicySignature<Args, Results> OpPolicy, bool Pure = true>
class StackOp : public Statement {
public:
  using ArgsPack   = StackView<Args>;
  using ResultPack = StackView<Results>;

  StackOp()
      : Statement{ Args, Results, Pure } {
  }

  [[nodiscard]] auto apply(Stack stack) const -> Stack override {
    ArgsPack args;
    for (int i{ 0 }; i < static_cast<int>(Args); i++) {
      args[i] = stack.back();
      stack.pop_back();
    }

    auto results{ std::invoke(OpPolicy, args) };

    for (int i{ 0 }; i < static_cast<int>(Results); i++) {
      stack.push_back(results[i]);
    }
    return stack;
  }
};

class ConstOp : public Statement {
public:
  explicit ConstOp(ValueT value)
      : Statement{ 0, 1, true },
        value{ value } {
  }

  [[nodiscard]] auto apply(Stack stack) const -> Stack override {
    stack.push_back(value);
    return stack;
  }

private:
  ValueT value;
};

using InputOp = StackOp<0, 1, ([](auto) -> StackView<1> {
                          ValueT inp{};
                          std::cin >> inp;
                          return { inp };
                        }),
                        false>;

using DupOp = StackOp<1, 2, [](const auto& args) -> StackView<2> {
  return { args[0], args[0] };
}>;

using AbsOp = StackOp<1, 1, [](const auto& args) -> StackView<1> {
  return { std::abs(args[0]) };
}>;

template <OpPolicySignature<2, 1> F>
using BinaryOp = StackOp<2, 1, F>;

using AddOp = BinaryOp<[](const auto& args) -> StackView<1> {
  return { args[1] + args[0] };
}>;

using SubOp = BinaryOp<[](const auto& args) -> StackView<1> {
  return { args[1] - args[0] };
}>;

using MulOp = BinaryOp<[](const auto& args) -> StackView<1> {
  return { args[1] * args[0] };
}>;

using DivOp = BinaryOp<[](const auto& args) -> StackView<1> {
  if (args[0] == 0) {
    return { 0 };
  }
  return { args[1] / args[0] };
}>;

using ModOp = BinaryOp<[](const auto& args) -> StackView<1> {
  if (args[0] == 0) {
    return { 0 };
  }
  return { args[1] % args[0] };
}>;

} // namespace pn
