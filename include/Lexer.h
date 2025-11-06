#ifndef FERRET_LANG_LEXER_H
#define FERRET_LANG_LEXER_H

#include <cstddef>
#include <sstream>
#include <string>
#include <utility>
#include <variant>
#include <vector>

#include "base/Diagnostics.h"
#include "base/FileReader.h"
#include "base/Token.h"

/**
 * This namespace represents the first part of the compiler.
 * It handles the initial stages, like tokenizing and
 * understanding the source code, before passing it to the
 * middle-end and back-end.
 */
namespace frontend {
   /**
    * A lexer breaks the source code into tokens,
    * which represent the smallest meaningful elements
    * of the program.
    */
   class Lexer {
   public:
      explicit Lexer(base::FileReader fileReader) :
          fileReader_(std::move(fileReader)), line_(0), column_(0), position_(0) {}

      [[nodiscard]] std::variant<base::Diagnostics, std::vector<base::Token>> tokenize() { return parseSourceCode(); }

   private:
      base::FileReader fileReader_;

      std::size_t line_;
      std::size_t column_;
      std::size_t position_;

      [[nodiscard]] bool isEOF() const { return position_ >= fileReader_.getSourceCode().length(); }
      [[nodiscard]] char getCurrentCharacter() const { return fileReader_.getSourceCode()[position_]; }

      void walkAhead() {
         column_++;
         position_++;
      }

      [[nodiscard]] std::string getLine(const std::size_t line) const {
         size_t linePtr = 0;
         std::string lineContent;
         std::stringstream sourceStream(fileReader_.getRawSourceCode());
         while (linePtr <= line) {
            std::getline(sourceStream, lineContent);
            linePtr++;
         }
         return lineContent;
      }

      [[nodiscard]] std::string getCurrentLine() const { return getLine(line_); }

      [[nodiscard]] std::variant<base::Diagnostics, std::vector<base::Token>> parseSourceCode() {
         base::Diagnostics diagnostics;
         std::vector<base::Token> tokens;

         while (!isEOF()) {
            if (std::isspace(getCurrentCharacter())) {
               if (getCurrentCharacter() == '\n') {
                  line_++;
                  column_ = 0;
               }
               walkAhead();
               continue;
            }

            if (std::isdigit(getCurrentCharacter())) {
               walkAhead();

               bool isFloat = false;
               bool isInvalidFloatLiteral = false;
               std::size_t startingColumn = column_;
               std::size_t startingPosition = position_;

               while (std::isdigit(getCurrentCharacter()) || getCurrentCharacter() == '.') {
                  if (getCurrentCharacter() == '.') {
                     if (isFloat) {
                        if (!isInvalidFloatLiteral) {
                           isInvalidFloatLiteral = true;
                           startingColumn = column_;
                           startingPosition = position_;
                        }
                     }
                     isFloat = true;
                  }
                  walkAhead();
               }

               if (isInvalidFloatLiteral) {
                  const std::size_t columnLength = position_ - startingPosition;
                  base::DiagnosticDetails diagnosticDetails = {
                        base::DIAGNOSTICS_ERROR,
                        fileReader_.getFilepath(),
                        base::TokenLocation{line_ + 1, startingColumn, columnLength},
                  };
                  base::LexerError lexerError(getCurrentLine(), "Invalid float literal.",
                                              "Numbers can have at most one decimal point.",
                                              "Remove any extra decimal points to form a valid number.");
                  diagnostics.createBlock(diagnosticDetails, lexerError);
               }

               continue;
            }

            walkAhead();
         }

         if (!diagnostics.isEmpty())
            return diagnostics;
         return tokens;
      }
   };
} // namespace frontend

#endif // FERRET_LANG_LEXER_H
