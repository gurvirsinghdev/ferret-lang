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
          fileReader_(std::move(fileReader)), line_(1), column_(1), position_(0) {}

      [[nodiscard]] std::variant<base::Diagnostics, std::vector<base::Token>> tokenize() { return parseSourceCode(); }

   private:
      base::FileReader fileReader_;

      std::size_t line_;
      std::size_t column_;
      std::size_t position_;

      [[nodiscard]] bool isEOF() const { return position_ >= fileReader_.getSourceCode().length(); }
      [[nodiscard]] char getCurrentCharacter() const { return fileReader_.getSourceCode()[position_]; }
      [[nodiscard]] std::optional<char> getNextCharacter() const {
         const auto &sourceCode = fileReader_.getSourceCode();
         if (position_ + 1 >= sourceCode.size()) {
            return std::nullopt;
         }
         return sourceCode[position_ + 1];
      }

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

      [[nodiscard]] std::string getCurrentLine() const { return getLine(line_ - 1); }

      [[nodiscard]] std::variant<base::Diagnostics, std::vector<base::Token>> parseSourceCode() {
         base::Diagnostics diagnostics;
         std::vector<base::Token> tokens;

         while (!isEOF()) {
            // Ignore whitespaces and other non-printable characters like (\n, \t).
            if (std::isspace(getCurrentCharacter())) {
               if (getCurrentCharacter() == '\n') {
                  line_++;
                  column_ = 0;
               }
               walkAhead();
               continue;
            }

            // Parse an integer or float from the source code.
            if (std::isdigit(getCurrentCharacter())) {
               bool isFloat = false;
               bool isInvalidFloatLiteral = false;
               std::size_t startingColumn = column_;
               std::size_t startingPosition = position_;

               walkAhead();
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
                        base::TokenLocation{line_, startingColumn, columnLength},
                  };
                  base::LexerError lexerError(base::ErrorCode::INVALID_FLOAT_LITERAL, getCurrentLine(),
                                              "Invalid float literal.", "Numbers can have at most one decimal point.",
                                              "Remove any extra decimal points to form a valid number.");
                  diagnostics.createBlock(diagnosticDetails, lexerError);
               } else {
                  base::TokenLocation tokenLocation{line_, startingPosition, position_ - startingPosition};
                  base::Token token(isFloat ? base::TokenType::TOKEN_FLOAT_LITERAL
                                            : base::TokenType::TOKEN_INTEGER_LITERAL,
                                    tokenLocation);
                  tokens.emplace_back(token);
               }
               continue;
            }

            // Parse an identifier or keyword from the source code.
            if (std::isalpha(getCurrentCharacter()) || getCurrentCharacter() == '_') {
               std::size_t startingColumn = column_;
               std::size_t startingPosition = position_;
               std::size_t startingColumnLength = 0;

               bool invalidVariableName = false;

               walkAhead();
               while (true) {
                  if (std::isspace(getCurrentCharacter()))
                     break;
                  if (std::isalnum(getCurrentCharacter()) || getCurrentCharacter() == '_') {
                     walkAhead();
                     continue;
                  }

                  if (const std::optional nextCharacter = getNextCharacter()) {
                     if (const char validNextCharacter = *nextCharacter;
                         std::isalnum(validNextCharacter) || validNextCharacter == '_') {
                        if (!invalidVariableName) {
                           startingColumn = column_;
                           startingPosition = position_;
                           invalidVariableName = true;
                        }

                        startingColumnLength++;
                        walkAhead();
                     }
                     continue;
                  }

                  break;
               }

               if (invalidVariableName) {
                  base::DiagnosticDetails diagnosticDetails = {base::DIAGNOSTICS_ERROR, fileReader_.getFilepath(),
                                                               base::TokenLocation{line_, startingColumn, 1}};
                  base::LexerError lexerError(base::ErrorCode::INVALID_VARIABLE_NAME, getCurrentLine(),
                                              "Invalid variable name.",
                                              "Variable names may only contain letters, numbers, and underscores.",
                                              "Remove or replace any characters that do not follow this rule.");
                  diagnostics.createBlock(diagnosticDetails, lexerError);
                  continue;
               }

               base::TokenLocation tokenLocation{line_, startingPosition, position_ - startingPosition};
               base::Token token(base::TokenType::TOKEN_IDENTIFIER, tokenLocation);
               tokens.emplace_back(token);
               continue;
            }

            std::size_t startingColumn = column_;
            std::size_t startingPosition = position_;
            walkAhead();
            base::DiagnosticDetails diagnosticDetails = {
                  base::DIAGNOSTICS_ERROR, fileReader_.getFilepath(),
                  base::TokenLocation{line_, startingColumn, position_ - startingPosition}};
            base::LexerError lexerError(base::ErrorCode::INVALID_CHARACTER, getCurrentLine(), "Invalid character.",
                                        "The source contains a character that is not recognized by the language.",
                                        "Remove the invalid character.");
            diagnostics.createBlock(diagnosticDetails, lexerError);
         }

         if (!diagnostics.isEmpty())
            return diagnostics;
         return tokens;
      }
   };
} // namespace frontend

#endif // FERRET_LANG_LEXER_H
