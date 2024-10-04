#include "Operations.hpp"
#ifndef NOT_EJUDGE
#include "ProfilerUndef.hpp"
#else
#include "Profiler.hpp"
#endif
#include "StatementBlock.hpp"
#include <cassert>
#include <functional>
#include <iostream>
#include <random>
#include <regex>
#include <sstream>

// ejudge compat
using namespace pn;

const static std::unordered_map<std::string,
                                std::shared_ptr<Statement> (*)(void)>
    builderMap{
      { "dup",
        []() -> std::shared_ptr<Statement> {
          return std::make_shared<DupOp>();
        } },
      { "input",
        []() -> std::shared_ptr<Statement> {
          return std::make_shared<InputOp>();
        } },
      { "abs",
        []() -> std::shared_ptr<Statement> {
          return std::make_shared<AbsOp>();
        } },
      { "+",
        []() -> std::shared_ptr<Statement> {
          return std::make_shared<AddOp>();
        } },
      { "-",
        []() -> std::shared_ptr<Statement> {
          return std::make_shared<SubOp>();
        } },
      { "/",
        []() -> std::shared_ptr<Statement> {
          return std::make_shared<DivOp>();
        } },
      { "*",
        []() -> std::shared_ptr<Statement> {
          return std::make_shared<MulOp>();
        } },
      { "%",
        []() -> std::shared_ptr<Statement> {
          return std::make_shared<ModOp>();
        } },
    };

static auto split(std::string s) -> std::vector<std::string> {
  std::vector<std::string> tokens;
  std::istringstream ss{ s };
  std::string word;

  while (ss >> word) {
    tokens.push_back(word);
  }

  return tokens;
}

class CompileError : public std::runtime_error {
public:
  explicit CompileError(const std::string& msg)
      : std::runtime_error{ "Compilation error: " + msg } {
  }
};

auto compile(std::string_view sv) -> std::shared_ptr<Statement> {
  PROFILER_SCOPE("compile");
  auto res{ std::make_shared<StatementBlock>() };
  std::regex isNumber("^[+-]?[0-9]+$");
  auto tokens{ split(std::string{ sv }) };
  for (auto&& token : tokens) {
    if (std::regex_match(token, isNumber)) {
      try {
        res->add_statement(std::make_shared<ConstOp>(std::stol(token)));
      } catch (const std::out_of_range& e) {
        throw CompileError{ "Number too big" };
      }
    } else if (builderMap.contains(token)) {
      res->add_statement(std::invoke(builderMap.at(token)));
    } else {
      throw CompileError{ "unexpected token " + token };
    }
  }
  return res;
}

auto operator|(std::shared_ptr<Statement> lhs,
               std::shared_ptr<Statement> rhs)
    -> std::shared_ptr<Statement> {
  PROFILER_SCOPE("pipe");
  auto newBlock{ std::make_shared<StatementBlock>() };
  auto lhsBlockPtr{ std::dynamic_pointer_cast<StatementBlock>(lhs) };
  if (!lhsBlockPtr) {
    newBlock->add_statement(lhs);
  } else {
    for (auto&& stmt : lhsBlockPtr->get_statements()) {
      newBlock->add_statement(stmt);
    }
  }
  auto rhsBlockPtr{ std::dynamic_pointer_cast<StatementBlock>(rhs) };
  if (!rhsBlockPtr) {
    newBlock->add_statement(rhs);
  } else {
    for (auto&& stmt : rhsBlockPtr->get_statements()) {
      newBlock->add_statement(stmt);
    }
  }

  return newBlock;
}

auto optimize(std::shared_ptr<Statement> stmt)
    -> std::shared_ptr<Statement> {
  PROFILER_SCOPE("optimize");
  auto block{ std::dynamic_pointer_cast<StatementBlock>(stmt) };
  if (!block) {
    return stmt;
  }
  auto sb{ std::make_shared<StatementBlock>(*block) };

  sb->symbolic_optimize();
  return sb;
}

#ifdef NOT_EJUDGE

// for fuzzing
// int main() {
//   std::string testStr;
//   std::cin >> testStr;
//   auto test{ compile(testStr) };
//   test->apply(std::vector<ValueT>(10000, 1));
// }

auto main() -> int {
  PROFILER_BEGIN_SESSION("main", "profile.json");
  std::mt19937 rng{ std::random_device{}() };
  std::vector<std::string> insert{ "+",   "-", "input", "*", "/", "dup",
                                   "abs", "%", "1",     "2", "3", "4",
                                   "5",   "6", "7",     "8", "9" };
  std::uniform_int_distribution<size_t> dist{ 0, insert.size() - 1 };
  std::string big{};
  for (int i{ 0 }; i < 100000; i++) {
    big += insert[dist(rng)] + " ";
  }
  auto bs{ compile(big) };
  bs = optimize(bs);
  // std::cout << bs->apply({})[0] << std::endl;
  PROFILER_END_SESSION();
}

#endif
