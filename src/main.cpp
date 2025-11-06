#include <string>
#include "Lexer.h"
#include "base/FileReader.h"

int main() {
   const std::string sourceFilepath = "../main.fl";
   try {
      frontend::Lexer lexer(base::FileReader{sourceFilepath});

      auto const output = lexer.tokenize();
      if (std::holds_alternative<base::Diagnostics>(output)) {
         const auto &diagnostics = std::get<base::Diagnostics>(output);
         diagnostics.logAllBlocks();
         exit(1);
      }

      const auto &tokens = std::get<std::vector<base::Token>>(output);
      (void) tokens;
      return 0;
   } catch (const std::exception &e) {
      std::cerr << e.what() << std::endl;
      return 2;
   }
}
