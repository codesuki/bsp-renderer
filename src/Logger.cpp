#include "logger.hpp"

#include <iostream>
#include <stdarg.h>
#include <stdio.h>

namespace logger {
namespace {
Level max_level_ = DEBUG;
}

void set_max_level(Level max_level) {}

void Log(Level level, const char *fmt, va_list arg) {
  if (level > max_level_)
    return;
  vprintf(fmt, arg);
  std::cout << std::endl;
}

void Log(Level level, const char *fmt, ...) {
  va_list arg;
  va_start(arg, fmt);
  Log(level, fmt, arg);
  va_end(arg);
}

void Debug(const char *fmt, ...) {
  va_list arg;
  va_start(arg, fmt);
  Log(DEBUG, fmt, arg);
  va_end(arg);
}
}
