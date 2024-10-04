#include "afl-fuzz.h"
#include <iostream>
#include <random>
#include <string>
#include <vector>

struct MutatorState {
  afl_state_t* afl;

  std::mt19937 rng;
  std::string mutated;
};

extern "C" auto afl_custom_init(afl_state_t* afl,
                                unsigned int seed) -> void* {
  srand(seed);

  auto* state{ new MutatorState() };
  state->afl = afl;
  state->rng = std::mt19937(std::random_device{}());

  return state;
}

extern "C" auto afl_custom_fuzz(
    MutatorState* data, uint8_t* buf, size_t buf_size, u8** out_buf,
    uint8_t* add_buf,
    size_t add_buf_size, // add_buf can be null
    size_t max_size) -> size_t {
  std::string insert{ "+-*/%" };
  for (int i{ 0 }; i < buf_size;) {
    if (buf[i] == 'd' && (i + 1 < buf_size && buf[i + 1] == 'u') &&
        (i + 2 < buf_size && buf[i + 2] == 'p')) {
      i += 3;
    } else if (buf[i] == 'a' && (i + 1 < buf_size && buf[i + 1] == 'b') &&
               (i + 2 < buf_size && buf[i + 2] == 's')) {
      i += 3;
    } else if (insert.find(buf[i]) != std::string::npos ||
               std::isdigit(data->mutated[i])) {
      i++;
    } else {
      buf[i] = ' ';
      i++;
    }
  }

  *out_buf = buf;
  return buf_size;
}

extern "C" void afl_custom_deinit(MutatorState* data) {
  delete data;
}
