#ifndef LOGGER_HPP_
#define LOGGER_HPP_

namespace logger {
enum Level { ERROR, DEFAULT, DEBUG };

void set_max_level(Level max_level);
void Log(Level level, const char *fmt, ...);
void Debug(const char *fmt, ...);
};

#endif
