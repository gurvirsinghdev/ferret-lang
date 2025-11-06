#ifndef FERRET_LANG_DIAGNOSTICS_H
#define FERRET_LANG_DIAGNOSTICS_H

#include <iostream>
#include <string>
#include <vector>

#include "Token.h"

namespace base {
   class LexerError {
   public:
      LexerError(std::string lineContent, std::string error, std::string message, std::string hint) :
          lineContent_(std::move(lineContent)), error_(std::move(error)), message_(std::move(message)),
          hint_(std::move(hint)) {}

      [[nodiscard]] std::string getLineContent() const { return lineContent_; }
      [[nodiscard]] std::string getError() const { return error_; }
      [[nodiscard]] std::string getMessage() const { return message_; }
      [[nodiscard]] std::string getHint() const { return hint_; }

   private:
      std::string lineContent_;
      std::string error_;
      std::string message_;
      std::string hint_;
   };

   enum LogLevel {
      DIAGNOSTICS_ERROR,
   };

   struct DiagnosticDetails {
      LogLevel level;
      std::string filepath;
      TokenLocation location;
   };

   struct DiagnosticBlock {
      DiagnosticDetails details;
      LexerError lexerError;
   };

   class Diagnostics {
   public:
      void createBlock(const DiagnosticDetails &details, const LexerError &lexerError) {
         const DiagnosticBlock block(details, lexerError);
         blocks.emplace_back(block);
      }

      void logAllBlocks() const {
         if (!blocks.empty()) {
            std::cerr << blocks.size() << " errors." << std::endl;
         }
         for (const auto &[details, lexerError]: blocks) {
            switch (details.level) {
               case DIAGNOSTICS_ERROR:
                  logErrorBlock(details, lexerError);
                  break;
            }
         }
      }

      [[nodiscard]] bool isEmpty() const noexcept { return blocks.empty(); }

   private:
      std::vector<DiagnosticBlock> blocks;

      static void logErrorBlock(const DiagnosticDetails &details, const LexerError &lexerError) {
         std::cerr << "error: " << lexerError.getError() << std::endl;
         logFilepathLine(details.filepath, details.location.line, details.location.column, details.location.length);
         logLineContent(details.location.line, lexerError.getLineContent());
         for (int i = 0; i < details.location.column; i++)
            std::cerr << " ";
         for (int i = 0; i < details.location.length; i++)
            std::cerr << "^";
         std::cerr << " " << lexerError.getMessage() << std::endl;
         std::cerr << "   hint: " << lexerError.getHint() << "\n" << std::endl;
      }

      static void logFilepathLine(const std::string &filepath, const std::size_t line, const std::size_t column,
                                  const std::size_t length) noexcept {
         const std::size_t endingColumn = column + length;
         std::cerr << " --> " << filepath << ":" << line << ":" << endingColumn << std::endl;
      }

      static void logLineContent(const std::size_t line, const std::string &lineContent) noexcept {
         std::cerr << "   |" << std::endl;
         std::cerr << " " << line << " | " << lineContent << std::endl;
         std::cerr << "   |";
      }
   };
} // namespace base

#endif // FERRET_LANG_DIAGNOSTICS_H
