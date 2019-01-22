#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

const int INFINITELY_LARGE_SIZE = 64;

struct {
  char array[INFINITELY_LARGE_SIZE] = {0};
  char *ptr = nullptr;
  char *ip = nullptr;
  int unm_br_amount = 0;
  bool ignore_commands = false;
} state;

char *read_file(char const *filename) {
  FILE *f = fopen(filename, "r");
  if (f == nullptr) {
    printf("Failed to open file\n");
    throw "Failed to open file";
  }
  fseek(f, 0, SEEK_END);
  const unsigned long file_size = ftell(f);
  fseek(f, 0, SEEK_SET);
  char *contents = (char *)malloc(file_size * sizeof(*contents));
  const unsigned long nbytes = fread(contents, file_size, 1, f);
  if (nbytes != 1) {
    printf("Unknown file read error: %lu != %lu\n", nbytes, file_size);
    throw "Unknown file read error";
  }
  return contents;
}

inline void read_char() {
  switch (*state.ip) {
  case '>': {
    if (!state.ignore_commands) {
      ++state.ptr;
    }
    break;
  }
  case '<': {
    if (!state.ignore_commands) {
      --state.ptr;
    }
    break;
  }
  case '+': {
    if (!state.ignore_commands) {
      ++*state.ptr;
    }
    break;
  }
  case '-': {
    if (!state.ignore_commands) {
      --*state.ptr;
    }
    break;
  }
  case '.': {
    if (!state.ignore_commands) {
      putchar(*state.ptr);
    }
    break;
  }
  case ',': {
    if (!state.ignore_commands) {
      *state.ptr = getchar();
    }
    break;
  }
  case '[': {
    bool prev_ignore_commands = state.ignore_commands;
    state.ignore_commands = prev_ignore_commands || !(*state.ptr);
    // printf("ADDING (ignore: %d - (%d == 0)): %d -> %d\n",
    // state.ignore_commands,
    //        *state.ptr, state.unm_br_amount, state.unm_br_amount + 1);
    int starting_unm_br_amount = state.unm_br_amount;
    ++state.ip;
    char *loop_beg = state.ip;
    if (state.ignore_commands) {
      ++state.unm_br_amount;
      while (state.unm_br_amount != starting_unm_br_amount) {
        read_char();
      }
      state.ignore_commands = prev_ignore_commands;
    } else {
      while (*state.ptr) {
        state.ip = loop_beg;
        ++state.unm_br_amount;
        while (state.unm_br_amount != starting_unm_br_amount) {
          read_char();
        }
      }
    }
    break;
  }
  case ']': {
    assert(state.unm_br_amount >= 1);
    --state.unm_br_amount;
    break;
  }
  }
  ++state.ip;
}

void reset_state(char *contents) {
  state.ip = contents;
  memset(state.array, 0, sizeof(state.array));
  state.ptr = state.array;
}

void run(char const *filename, bool output_memory = false) {
  char *contents = read_file(filename);
  reset_state(contents);
  while (*state.ip) {
    read_char();
  }
  if (output_memory) {
    for (int i = 0; i < sizeof(state.array); ++i) {
      printf("%d, ", state.array[i]);
    }
  }
  putchar('\n');
}

int main() {
  {
    printf("SIMPLE PROGRAM:\n");
    char *filename = "./programs/simple-program.bf";
    run(filename, true);
  }
  {
    printf("SIMPLE ADDITION PROGRAM:\n");
    char *filename = "./programs/simple-addition-program.bf";
    run(filename, false);
  }
  {
    printf("HELLO WORLD PROGRAM:\n");
    char *filename = "./programs/program.bf";
    run(filename, false);
  }
  return 0;
}
