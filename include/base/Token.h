#ifndef FERRET_LANG_TOKEN_H
#define FERRET_LANG_TOKEN_H

#include <cstddef>

namespace base {
   /**
    * These are all the valid token types that can exist
    * in the programming language. Any other type should
    * be considered invalid.
    */
   enum class TokenType {
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
} // namespace base

#endif // FERRET_LANG_TOKEN_H
