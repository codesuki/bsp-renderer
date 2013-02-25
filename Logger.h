#ifndef _LOGGER_H_
#define _LOGGER_H_

namespace logger
{
  enum Level
  {
    ERROR,
    DEFAULT,
    DEBUG
  };

  void set_max_level(Level max_level);
  void Log(Level level, const char *fmt, ...);
};

#endif