#ifndef FERRET_LANG_DIAGNOSTICS_H
#define FERRET_LANG_DIAGNOSTICS_H

#include <iostream>
#include <string>
#include <vector>

#include "Ansi.h"
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
         for (const auto &[details, lexerError]: blocks) {
            switch (details.level) {
               case DIAGNOSTICS_ERROR:
                  logErrorBlock(details, lexerError);
                  break;
            }
         }

         if (!blocks.empty()) {
            std::cerr << blocks.size() << " errors generated." << std::endl;
         }
      }

      [[nodiscard]] bool isEmpty() const noexcept { return blocks.empty(); }

   private:
      std::vector<DiagnosticBlock> blocks;

      static void logErrorBlock(const DiagnosticDetails &details, const LexerError &lexerError) {
         std::cerr << Ansi::BOLD << Ansi::RED << "error[E001]: " << lexerError.getError() << std::endl;
         logFilepathLine(details.filepath, details.location.line, details.location.column);
         logLineContent(details.location.line, lexerError.getLineContent(), details.location.column,
                        details.location.length);
         for (int i = 0; i < details.location.column; i++)
            std::cerr << " ";
         std::cerr << Ansi::YELLOW;
         for (int i = 0; i < details.location.length; i++)
            std::cerr << "^";
         std::cerr << " " << lexerError.getMessage() << std::endl;
         std::cerr << Ansi::CYAN << "   hint: " << lexerError.getHint() << "\n" << Ansi::RESET << std::endl;
      }

      static void logFilepathLine(const std::string &filepath, const std::size_t line,
                                  const std::size_t column) noexcept {
         std::cerr << Ansi::DIM << " --> " << filepath << ":" << line << ":" << column << std::endl;
      }

      static void logLineContent(const std::size_t line, const std::string &lineContent,
                                 const std::size_t errorStartColumn, const std::size_t errorTokenLength) noexcept {
         std::cerr << "   |" << std::endl;
         std::cerr << " " << line << " | ";
         for (int i = 0; i < lineContent.size(); i++) {
            if (i + 1 == errorStartColumn) {
               std::cerr << Ansi::RESET << Ansi::BOLD;
            }
            if (i + 1 == errorStartColumn + errorTokenLength) {
               std::cerr << Ansi::DIM;
            }
            std::cerr << lineContent[i];
         }
         std::cerr << Ansi::DIM << std::endl;
         std::cerr << "   |";
      }
   };
} // namespace base

#endif // FERRET_LANG_DIAGNOSTICS_H
