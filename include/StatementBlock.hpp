#pragma once
#include "Types.hpp"
#include <memory>
#include <statement.h>

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

  static void dump_stack_to_constants(Block& newBlock, Stack& stack);
  Block statements;
};

} // namespace pn
