#ifndef FERRET_LANG_LEXER_H
#define FERRET_LANG_LEXER_H

#include <cstddef>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#include "Diagnostics.h"
#include "base/FileReader.cpp.h"

/**
 * This namespace represents the first part of the compiler.
 * It handles the initial stages, like tokenizing and
 * understanding the source code, before passing it to the
 * middle-end and back-end.
 */
namespace frontend {
   /**
    * These are all the valid token types that can exist
    * in the programming language. Any other type should
    * be considered invalid.
    */
   enum TokenType {
      TOKEN_IDENTIFIER,
      TOKEN_LPAREN,
      TOKEN_RPAREN,
      TOKEN_LBRACE,
      TOKEN_RBRACE,
      TOKEN_SEMICOLON,
   };

   /**
    *
    */
   struct TokenLocation {
      std::size_t line;
      std::size_t column;
      std::size_t length;
   };


   /**
    * A token is the smallest unit recognised by the compiler.
    * They are processed by the Parser to assign structure
    * and meaning to the source code.
    */
   class Token {
   public:
      Token(const TokenType type, const TokenLocation &location) : type_(type), location_(location) {}

      [[nodiscard]] TokenType getType() const { return type_; }
      [[nodiscard]] std::size_t getLine() const { return location_.line; }
      [[nodiscard]] std::size_t getColumn() const { return location_.column; }
      [[nodiscard]] std::size_t getLength() const { return location_.length; }
      [[nodiscard]] TokenLocation getLocation() const { return location_; }

   private:
      TokenType type_;
      TokenLocation location_;
   };


   /**
    * A lexer breaks the source code into tokens,
    * which represent the smallest meaningful elements
    * of the program.
    */
   class Lexer {
   public:
      explicit Lexer(base::FileReader fileReader) :
          fileReader_(std::move(fileReader)), line_(0), column_(0), position_(0) {}

      std::vector<std::unique_ptr<Token>> tokenize() {
         std::vector<std::unique_ptr<Token>> tokens;
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
               const int startingPosition = position_;
               const int startingColumn = column_;
               walkAhead();
               bool isFloat = false;
               while (std::isdigit(getCurrentCharacter()) || getCurrentCharacter() == '.') {
                  if (getCurrentCharacter() == '.') {
                     if (isFloat) {
                        Diagnostics::logError(fileReader_.getFilepath(), line_ + 1, startingColumn, position_ - startingPosition,
                                              getCurrentLine(), "Invalid float literal.",
                                              "Numbers can have at most one decimal point.",
                                              "Remove any extra decimal points to form a valid number.");
                     }
                     isFloat = true;
                  }
                  walkAhead();
               }
               continue;
            }

            walkAhead();
         }
         return tokens;
      }

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
   };
} // namespace frontend

#endif // FERRET_LANG_LEXER_H
