// Copyright 2026 Artem Nikitin

/// @file handler.cpp
/// @brief Реализация свёртки лексем грамматики в числовой код.
/// @author Artem Nikitin

#include "Coding/handler.h"

#include <algorithm>
#include <cctype>
#include <filesystem>
#include <iterator>
#include <stdexcept>
#include <unordered_set>

// Состояние автомата, проверяющего структуру правил грамматики: краткое
// описание в handler.h, значения перечисления определены в handler.cpp.
enum class Handler::State {
  TerminalIsExpected,              ///< Ожидается лексема тела правила.
  DelimiterIsExpected,             ///< Ожидается `:` после нетерминала.
  NonTerminalIsExpected,           ///< Ожидается нетерминал в начале правила.
  FinalState,                      ///< Встречен `Eofgram`, разбор завершён.
  FurtherProcessingIsNotPossible,  ///< Дальнейшая обработка невозможна.
};

namespace {

/// @brief Числовые коды знаков операций и других символов грамматики.
/// @return Ссылка на неизменяемую таблицу «символ — код».
const std::unordered_map<std::string, int>& symbolCodes() {
  static const std::unordered_map<std::string, int> codes = {
      {":", 1}, {"(", 2}, {")", 3}, {".", 4}, {"*", 5},
      {";", 6}, {",", 7}, {"#", 8}, {"[", 9}, {"]", 10},
  };
  return codes;
}

/// @brief Проверяет, является ли символ знаком операции грамматики.
/// @param symbol Проверяемый символ.
/// @return true, если символ есть в таблице кодов символов.
bool isSymbol(const char symbol) {
  return symbolCodes().contains(std::string(1, symbol));
}

/// @brief Проверяет, может ли символ входить в слово грамматики.
/// @param symbol Проверяемый символ.
/// @return true для латинской буквы, цифры или подчёркивания.
bool isWordChar(const char symbol) {
  return std::isalnum(static_cast<unsigned char>(symbol)) || symbol == '_';
}

/// @brief Проверяет, состоит ли слово только из заглавных латинских букв.
/// @param word Проверяемое слово.
/// @return true, если в слове есть буквы и все они заглавные.
bool isAllUpper(const std::string& word) {
  bool hasLetter = false;
  for (const char symbol : word) {
    if (std::isalpha(static_cast<unsigned char>(symbol))) {
      hasLetter = true;
      if (!std::isupper(static_cast<unsigned char>(symbol))) {
        return false;
      }
    }
  }
  return hasLetter;
}

}  // namespace

Handler::Handler(std::istream& h_file) {
  if (!h_file) {
    throw std::runtime_error("Input stream is not open");
  }

  source.assign(std::istreambuf_iterator(h_file),
                std::istreambuf_iterator<char>());
  currentState = State::NonTerminalIsExpected;
}

std::string& Handler::handle() {
  tokenize();
  resolveWords();
  encode();
  return result;
}

void Handler::tokenize() {
  lexemes.clear();
  const std::size_t size = source.size();
  std::size_t index = 0;

  while (index < size) {
    const auto current = static_cast<unsigned char>(source[index]);

    if (std::isspace(current)) {
      ++index;
      continue;
    }

    if (current == '\'') {
      const std::size_t start = index++;
      while (index < size && source[index] != '\'') {
        ++index;
      }
      if (index == size) {
        throw std::runtime_error("Unterminated terminal literal: " +
                                 source.substr(start));
      }
      ++index;
      lexemes.push_back({.kind = Lexeme::Kind::Terminal,
                         .text = source.substr(start, index - start)});
      continue;
    }

    if (current == '$') {
      const std::size_t start = index++;
      while (index < size && isWordChar(source[index])) {
        ++index;
      }
      lexemes.push_back({.kind = Lexeme::Kind::Semantic,
                         .text = source.substr(start, index - start)});
      continue;
    }

    if (isSymbol(static_cast<char>(current))) {
      lexemes.push_back({.kind = Lexeme::Kind::Symbol,
                         .text = std::string(1, static_cast<char>(current))});
      ++index;
      continue;
    }

    if (std::isalpha(current) || current == '_') {
      const std::size_t start = index;
      while (index < size && isWordChar(source[index])) {
        ++index;
      }
      lexemes.push_back({.kind = Lexeme::Kind::Word,
                         .text = source.substr(start, index - start)});
      continue;
    }

    throw std::runtime_error(std::string("Unexpected character: ") +
                             static_cast<char>(current));
  }
}

void Handler::resolveWords() {
  std::unordered_set<std::string> nonterminals;
  for (std::size_t index = 0; index < lexemes.size(); ++index) {
    if (lexemes[index].kind != Lexeme::Kind::Word) {
      continue;
    }

    const bool definesRule = index + 1 < lexemes.size() &&
                             lexemes[index + 1].kind == Lexeme::Kind::Symbol &&
                             lexemes[index + 1].text == ":";
    if (definesRule || isAllUpper(lexemes[index].text)) {
      nonterminals.insert(lexemes[index].text);
    }
  }

  for (auto& [kind, text] : lexemes) {
    if (kind != Lexeme::Kind::Word) {
      continue;
    }

    if (text == "Eofgram") {
      kind = Lexeme::Kind::Eof;
    } else if (nonterminals.contains(text)) {
      kind = Lexeme::Kind::NonTerminal;
    } else {
      kind = Lexeme::Kind::Terminal;
    }
  }
}

int Handler::assignCode(const std::string& key, int& nextCode,
                        const int lastCode) {
  if (const auto found = CodeTable.find(key); found != CodeTable.end()) {
    return found->second;
  }

  if (nextCode > lastCode) {
    throw std::runtime_error("Code table overflow for '" + key + "'");
  }

  const int assigned = nextCode++;
  CodeTable[key] = assigned;
  return assigned;
}

void Handler::encode() {
  CodeTable.clear();
  result.clear();
  currentState = State::NonTerminalIsExpected;

  int nextNonTerminal = 11;
  int nextTerminal = 51;
  int nextSemantic = 101;

  std::vector<std::string> brackets;

  for (const auto& [kind, text] : lexemes) {
    if (kind == Lexeme::Kind::Symbol) {
      if (text == "(" || text == "[") {
        brackets.push_back(text);
      } else if (text == ")" || text == "]") {
        if (brackets.empty()) {
          throw std::runtime_error("Unmatched '" + text + "'");
        }
        const bool closes = (text == ")" && brackets.back() == "(") ||
                            (text == "]" && brackets.back() == "[");
        if (!closes) {
          throw std::runtime_error("Mismatched '" + text + "': '" +
                                   brackets.back() + "' is not closed by it");
        }
        brackets.pop_back();
      }
    }

    switch (currentState) {
      case State::NonTerminalIsExpected:
        if (kind == Lexeme::Kind::Eof) {
          currentState = State::FinalState;
        } else if (kind == Lexeme::Kind::NonTerminal) {
          currentState = State::DelimiterIsExpected;
        } else {
          throw std::runtime_error("A nonterminal was expected but '" + text +
                                   "' found");
        }
        break;
      case State::DelimiterIsExpected:
        if (kind != Lexeme::Kind::Symbol || text != ":") {
          throw std::runtime_error("':' was expected but '" + text + "' found");
        }
        currentState = State::TerminalIsExpected;
        break;
      case State::TerminalIsExpected:
        if (kind == Lexeme::Kind::Symbol && text == ".") {
          if (!brackets.empty()) {
            throw std::runtime_error("Unmatched '" + brackets.back() +
                                     "' before the end of the rule");
          }
          currentState = State::NonTerminalIsExpected;
        } else if (kind == Lexeme::Kind::Symbol && text == ":") {
          throw std::runtime_error("':' is unexpected inside a rule body");
        } else if (kind == Lexeme::Kind::Eof) {
          throw std::runtime_error("Expected '.' before Eofgram");
        }
        break;
      case State::FinalState:
        throw std::runtime_error("Unexpected lexeme after Eofgram: " + text);
      case State::FurtherProcessingIsNotPossible:
        throw std::runtime_error("Further processing is not possible");
    }

    int code = 0;
    switch (kind) {
      case Lexeme::Kind::Symbol:
        code = symbolCodes().at(text);
        break;
      case Lexeme::Kind::NonTerminal:
        code = assignCode("N:" + text, nextNonTerminal, 50);
        break;
      case Lexeme::Kind::Terminal:
        code = assignCode("T:" + text, nextTerminal, 100);
        break;
      case Lexeme::Kind::Semantic:
        code = assignCode("S:" + text, nextSemantic, 150);
        break;
      case Lexeme::Kind::Eof:
        code = 1000;
        break;
      case Lexeme::Kind::Word:
        throw std::runtime_error("Unresolved lexeme: " + text);
    }

    result += std::to_string(code);
    result += '\n';
  }

  if (currentState != State::FinalState) {
    throw std::runtime_error("Eofgram was not found");
  }
}

std::ifstream Handler::openGrmFile(const char* argumentPath) {
  std::filesystem::path filePath(argumentPath);

  if (!std::filesystem::exists(filePath)) {
    throw std::runtime_error("File does not exist: " + filePath.string());
  }

  std::string extension = filePath.extension().string();
  std::ranges::transform(extension, extension.begin(), ::tolower);

  if (extension != ".grm") {
    throw std::runtime_error("Extension does not match: " + extension);
  }

  std::ifstream file(filePath);
  if (!file || !file.is_open()) {
    throw std::runtime_error("Could not open " + filePath.string());
  }

  return file;
}
