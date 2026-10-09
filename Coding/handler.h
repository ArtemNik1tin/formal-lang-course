// Copyright 2026 Artem Nikitin

/// @file handler.h
/// @brief Свёртка лексем грамматики в регулярной форме в числовой код.
/// @author Artem Nikitin

#ifndef CODING_HANDLER_H_
#define CODING_HANDLER_H_

#include <fstream>
#include <string>
#include <unordered_map>
#include <vector>

/// @brief Выполняет свёртку лексем грамматики в регулярной форме в числовой
/// код в соответствии с таблицей кодов из задания.
class Handler {
 public:
  /// @brief Создаёт обработчик и читает исходный текст грамматики.
  /// @param h_file Входной поток с текстом грамматики.
  /// @throws std::runtime_error Если поток находится в ошибочном состоянии.
  explicit Handler(std::istream& h_file);

  /// @brief Выполняет разбор грамматики и кодирование её лексем.
  /// @return Последовательность числовых кодов лексем, по одному в строке.
  /// @throws std::runtime_error При синтаксической ошибке во входных данных.
  std::string& handle();

  /// @brief Открывает файл грамматики с расширением `.grm`.
  /// @param argumentPath Путь к файлу грамматики.
  /// @return Открытый входной поток с содержимым файла.
  /// @throws std::runtime_error Если файл не найден, имеет другое расширение
  /// или не может быть открыт.
  static std::ifstream openGrmFile(const char* argumentPath);

 private:
  /// @brief Состояние автомата, проверяющего структуру правил грамматики.
  enum class State;

  /// @brief Лексема грамматики.
  struct Lexeme {
    /// @brief Вид лексемы.
    enum class Kind {
      Word,         ///< Слово без кавычек (вид уточняется позже).
      NonTerminal,  ///< Нетерминал.
      Terminal,     ///< Терминал (в кавычках или строчное слово).
      Semantic,     ///< Семантическое действие (`$...`).
      Symbol,       ///< Знак операции или другой символ грамматики.
      Eof,          ///< Признак конца потока лексем (`Eofgram`).
    };

    Kind kind;         ///< Вид лексемы.
    std::string text;  ///< Исходный текст лексемы.
  };

  /// @brief Разбивает исходный текст грамматики на лексемы.
  void tokenize();

  /// @brief Определяет, какие слова являются нетерминалами, терминалами
  /// и признаком конца `Eofgram`.
  void resolveWords();

  /// @brief Строит таблицу кодов и формирует результат свёртки.
  /// @throws std::runtime_error При ошибке структуры грамматики или
  /// переполнении диапазона кодов.
  void encode();

  /// @brief Проверяет структуру грамматики: парность скобок, порядок лексем
  /// в правилах, завершение правила точкой и наличие Eofgram.
  /// @throws std::runtime_error При нарушении структуры грамматики.
  void validateStructure();

  /// @brief Контролирует парность скобок `(`/`)` и `[`/`]`.
  /// @param brackets Стек открытых скобок (изменяется).
  /// @param kind Вид лексемы.
  /// @param text Текст лексемы.
  /// @throws std::runtime_error При непарной или несовпадающей скобке.
  static void checkBrackets(std::vector<std::string>& brackets, Lexeme::Kind kind,
                     const std::string& text);

  /// @brief Переводит автомат в следующее состояние согласно лексеме.
  /// @param kind Вид лексемы.
  /// @param text Текст лексемы.
  /// @param brackets Стек открытых скобок текущего правила.
  /// @throws std::runtime_error При нарушении порядка лексем в правиле.
  void advanceState(Lexeme::Kind kind, const std::string& text,
                    const std::vector<std::string>& brackets);

  /// @brief Присваивает лексемам коды по таблице и формирует строку вывода.
  /// @throws std::runtime_error При переполнении диапазона кодов.
  void encodeCodes();

  /// @brief Возвращает уже присвоенный код лексемы или присваивает следующий.
  /// @param key Ключ лексемы в таблице кодов (с префиксом категории).
  /// @param nextCode Следующий свободный код категории (изменяется).
  /// @param lastCode Последний допустимый код категории.
  /// @return Числовой код лексемы.
  /// @throws std::runtime_error Если диапазон кодов категории исчерпан.
  int assignCode(const std::string& key, int& nextCode, int lastCode);

  std::string source;           ///< Исходный текст грамматики.
  std::vector<Lexeme> lexemes;  ///< Лексемы грамматики.
  std::unordered_map<std::string, int> CodeTable;  ///< Таблица кодов лексем.
  State currentState;  ///< Текущее состояние автомата.
  std::string result;  ///< Результат свёртки.
};

#endif  // CODING_HANDLER_H_
