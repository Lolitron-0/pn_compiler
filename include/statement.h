#pragma once
#include "Types.hpp"
#include <cstdint>

namespace pn {

class Statement {
public:
  [[nodiscard]] virtual auto apply(Stack in) const -> Stack = 0;

  Statement() = default;
  Statement(uint32_t arguments, uint32_t results, bool pure)
      : arguments(arguments),
        results(results),
        pure(pure) {
  }

  virtual ~Statement() = default;

  [[nodiscard]] auto is_pure() const -> bool {
    return pure;
  }

  [[nodiscard]] auto get_arguments_count() const -> uint32_t {
    return arguments;
  }

  [[nodiscard]] auto get_results_count() const -> uint32_t {
    return results;
  }

protected:
  uint32_t arguments;
  uint32_t results;
  bool pure;
};
} // namespace pn
