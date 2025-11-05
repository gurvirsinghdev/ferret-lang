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
    * A token is the smallest unit recognised by the compiler.
    * They are processed by the Parser to assign structure
    * and meaning to the source code.
    */
   class Token {
   public:
      Token(const TokenType type, const std::size_t line, const std::size_t column, const std::size_t length) :
          type_(type), line_(line), column_(column), length_(length) {}

      [[nodiscard]] TokenType getType() const { return type_; }
      [[nodiscard]] std::size_t getLine() const { return line_; }
      [[nodiscard]] std::size_t getColumn() const { return column_; }
      [[nodiscard]] std::size_t getLength() const { return length_; }

   private:
      TokenType type_;
      std::size_t line_;
      std::size_t column_;
      std::size_t length_;
   };


   /**
    * A lexer breaks the source code into tokens,
    * which represent the smallest meaningful elements
    * of the program.
    */
   class Lexer {
   public:
      explicit Lexer(std::string filepath) : filepath_(std::move(filepath)), position_(0), line_(0), column_(0) {}

      std::vector<std::unique_ptr<Token>> tokenize() {
         std::ifstream file(filepath_);
         if (!file.is_open()) {
            std::cerr << "Unable to open the source file!" << std::endl;
         }
         source_.assign(std::istreambuf_iterator(file), {});

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
                        Diagnostics::logError(filepath_, line_ + 1, startingColumn, position_ - startingPosition,
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
         file.close();
         return tokens;
      }

   private:
      std::string filepath_;
      std::string source_;
      std::size_t position_;
      std::size_t line_;
      std::size_t column_;

      [[nodiscard]] bool isEOF() const { return position_ >= source_.length(); }
      [[nodiscard]] char getCurrentCharacter() const { return source_[position_]; }

      void walkAhead() {
         column_++;
         position_++;
      }

      [[nodiscard]] std::string getLine(const std::size_t line) const {
         size_t linePtr = 0;
         std::string lineContent;
         std::stringstream sourceStream(source_);
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
