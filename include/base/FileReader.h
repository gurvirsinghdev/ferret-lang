#ifndef FERRET_LANG_FILEREADER_CPP_H
#define FERRET_LANG_FILEREADER_CPP_H

#include <fstream>
#include <iostream>
#include <string>

namespace base {
   /**
    * Handles the opening, reading and closing of the file.
    */
   class FileReader {
   public:
      explicit FileReader(const std::string &filepath) : filepath_(filepath) {
         std::ifstream file(filepath);
         if (!file.is_open()) {
            std::cerr << "Unable to open the source file." << std::endl;
            throw std::runtime_error("Unable to open the source file.");
         }
         sourceCode_.assign(std::istreambuf_iterator(file), {});
         file.close();
      }

      [[nodiscard]] std::string getFilepath() const noexcept { return filepath_; }
      [[nodiscard]] std::string_view getSourceCode() const noexcept { return std::string_view(sourceCode_); }
      [[nodiscard]] const std::string& getRawSourceCode() const noexcept { return sourceCode_; }

   private:
      std::string filepath_;
      std::string sourceCode_;
   };
}

#endif // FERRET_LANG_FILEREADER_CPP_H
