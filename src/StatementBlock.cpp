#include "Operations.hpp"
#include "StatementBlock.hpp"
#ifndef NOT_EJUDGE
#include "ProfilerUndef.hpp"
#else
#include "Profiler.hpp"
#endif

namespace pn {

StatementBlock::StatementBlock()
    : Statement{ 0, 0, true } {
}

StatementBlock::StatementBlock(Block statements)
    : statements{ std::move(statements) } {
  update_stats();
}

void StatementBlock::add_statement(
    const std::shared_ptr<Statement>& statement) {
  PROFILER_SCOPE("add stmt");
  statements.push_back(statement);
  process_stmt_stats(statement);
}

// for list
// void StatementBlock::symbolic_optimize() {
//   PROFILER_SCOPE("inner optimize");
//   Stack symStack;
//   auto start{ statements.begin() };
//   for (auto it{ statements.begin() }; it != statements.end(); it++) {
//     if ((*it)->get_arguments_count() > symStack.size() ||
//         !(*it)->is_pure()) {
//       if (!symStack.empty()) {
//         statements.erase(start, it);
//       }
//       start = it;
//       start++;
//       dump_stack_to_constants(it, symStack);
//     } else {
//       symStack = (*it)->apply(std::move(symStack));
//     }
//   }
//   if (!symStack.empty()) {
//     statements.erase(start, statements.end());
//     dump_stack_to_constants(statements.end(), symStack);
//   }
// }

void StatementBlock::symbolic_optimize() {
  PROFILER_SCOPE("inner optimize");
  Stack symStack;
  Block newBlock;
  newBlock.reserve(statements.size());
  for (auto&& statement : statements) {
    if (statement->get_arguments_count() > symStack.size() ||
        !statement->is_pure()) {
      dump_stack_to_constants(newBlock, symStack);
      newBlock.push_back(statement);

    } else {
      PROFILER_SCOPE("apply");
      symStack = statement->apply(std::move(symStack));
    }
  }
  dump_stack_to_constants(newBlock, symStack);

  statements = std::move(newBlock);
}

auto StatementBlock::get_statements() const -> const Block& {
  return statements;
}
auto StatementBlock::apply(Stack stack) const -> Stack {
  for (auto&& stmt : statements) {
    stack = stmt->apply(std::move(stack));
  }
  return stack;
}

void StatementBlock::process_stmt_stats(
    const std::shared_ptr<Statement>& stmt) {
  if (stmt->get_arguments_count() > results) {
    arguments += stmt->get_arguments_count() - results;
    results = stmt->get_results_count();
  } else {
    results += stmt->get_results_count();
    results -= stmt->get_arguments_count();
  }
  pure &= stmt->is_pure();
}

void StatementBlock::update_stats() {
  arguments = 0;
  results   = 0;
  pure      = true;
  for (auto&& stmt : statements) {
    process_stmt_stats(stmt);
  }
}

void StatementBlock::dump_stack_to_constants(Block& newBlock,
                                             Stack& stack) {
  PROFILER_SCOPE("dump fold");
  while (!stack.empty()) {
    {
      PROFILER_SCOPE("insert");
      newBlock.push_back(std::make_shared<ConstOp>(stack.back()));
    }
    stack.pop_back();
  }
}

} // namespace pn
