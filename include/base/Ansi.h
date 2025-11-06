#ifndef FERRET_LANG_ANSI_H
#define FERRET_LANG_ANSI_H

#include <string>

namespace base::Ansi {
   static const std::string RESET = "\x1b[0m";
   static const std::string BOLD = "\x1b[1m";

   static const std::string DIM = "\x1b[90m";
   static const std::string RED = "\x1b[31m";
   static const std::string CYAN = "\x1b[36m";
   static const std::string YELLOW = "\x1b[33m";
} // namespace base::Ansi


#endif // FERRET_LANG_ANSI_H
