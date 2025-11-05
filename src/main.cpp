#include "Lexer.h"

int main() {
   frontend::Lexer lexer("../main.fl");
   const std::vector<std::unique_ptr<frontend::Token>> tokens = lexer.tokenize();

   for (const auto &token: tokens) {
      std::cout << token->getLine() << ":" << token->getColumn() << std::endl;
   }

   return 0;
}
