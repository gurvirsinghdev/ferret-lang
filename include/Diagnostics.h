#ifndef FERRET_LANG_DIAGNOSTICS_H
#define FERRET_LANG_DIAGNOSTICS_H

#include <iostream>
#include <string>

class Diagnostics {
public:
   static void logError(const std::string &filename, const std::size_t line, const std::size_t column,
                        const std::size_t length, const std::string &lineContent, const std::string &errorBrief,
                        const std::string &errorMessage, const std::string &inlayHint) {
      std::cerr << "error: " << errorBrief << std::endl;
      std::cerr << " --> " << filename << ":" << line << ":" << column + length << std::endl;

      std::cerr << "   |" << std::endl;
      std::cerr << " " << line << " | " << lineContent << std::endl;
      std::cerr << "   |";

      for (int i = 0; i < column + 1; i++)
         std::cerr << " ";
      for (int i = 0; i < length; i++)
         std::cerr << "^";
      std::cerr << " " << errorMessage << std::endl;
      std::cerr << "   hint: " << inlayHint << std::endl;

      std::exit(1);
   }
};

#endif // FERRET_LANG_DIAGNOSTICS_H
