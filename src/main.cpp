#include <string>
#include "base/FileReader.cpp.h"
#include "Lexer.h"

int main() {
   const std::string filepath = "../main.fl";
   const base::FileReader fileReader(filepath);
   frontend::Lexer lexer(fileReader);
   lexer.tokenize();
   return 0;
}
