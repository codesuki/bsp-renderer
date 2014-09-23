#include "logger.hpp"

#include <stdio.h>
#include <stdarg.h>
#include <iostream>

namespace logger
{
  namespace {
    Level max_level_ = DEBUG;
  }

  void set_max_level(Level max_level)
  {
  }

  void Log(Level level, const char *fmt, ...)
  {
    if (level > max_level_)
      return;

    va_list arg;
    va_start(arg, fmt);
    vprintf(fmt, arg);
    va_end(arg);

    std::cout << std::endl;
  }

}
