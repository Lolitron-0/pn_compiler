#pragma once
#include "Operations.hpp"
#include "Profiler.hpp"
#include "Types.hpp"
#include <list>
#include <memory>
#include <statement.h>
#include <utility>

namespace pn {

class StatementBlock : public Statement {
public:
  using Block = std::vector<std::shared_ptr<Statement>>;

  StatementBlock();
  explicit StatementBlock(Block statements);

  void add_statement(const std::shared_ptr<Statement>& statement);

  void symbolic_optimize();

  [[nodiscard]] auto get_statements() const -> const Block&;

  [[nodiscard]] auto apply(Stack stack) const -> Stack override;

private:
  void process_stmt_stats(const std::shared_ptr<Statement>& stmt);
  void update_stats();

  inline void dump_stack_to_constants(Block& newBlock, Stack& stack)
	{
    PROFILER_SCOPE("dump fold");
    while (!stack.empty()) {
      {
        PROFILER_SCOPE("insert");
        newBlock.push_back(std::make_shared<ConstOp>(stack.back()));
      }
      stack.pop_back();
    }
  }

  Block statements;
};

} // namespace pn
