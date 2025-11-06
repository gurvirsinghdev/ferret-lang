#include <string>
#include "Lexer.h"
#include "base/FileReader.h"

int main() {
   const std::string sourceFilepath = "../main.fl";
   try {
      base::FileReader fileReader(sourceFilepath);
      frontend::Lexer lexer(fileReader);

      auto const output = lexer.tokenize();
      if (std::holds_alternative<base::Diagnostics>(output)) {
         const auto &diagnostics = std::get<base::Diagnostics>(output);
         diagnostics.logAllBlocks();
         exit(1);
      }

      for (const auto &tokens = std::get<std::vector<base::Token>>(output); const auto &token: tokens) {
         std::cout << static_cast<int>(token.getType()) << ":"
                   << fileReader.getSourceCode().substr(token.getColumn(), token.getLength()) << std::endl;
      }
      return 0;
   } catch (const std::exception &e) {
      std::cerr << e.what() << std::endl;
      return 2;
   }
}
