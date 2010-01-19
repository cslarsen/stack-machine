#include "sm-core.hpp"

machine_t compile(FILE* f, void (*compile_error)(const char* message) = NULL);
