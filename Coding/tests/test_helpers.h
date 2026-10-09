// Copyright 2026 Artem Nikitin

#ifndef CODING_TESTS_TEST_HELPERS_H_
#define CODING_TESTS_TEST_HELPERS_H_

#include <gtest/gtest.h>

#include <filesystem>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include "Coding/handler.h"

namespace coding_test {

// Выполняет свёртку грамматики, заданной строкой.
inline std::string encode(const std::string& grammar) {
  std::istringstream stream(grammar);
  Handler handler(stream);
  return handler.handle();
}

// Преобразует последовательность кодов в строку вывода (код на строку).
inline std::string codesToString(const std::vector<int>& codes) {
  std::string result;
  for (const int code : codes) {
    result += std::to_string(code);
    result += '\n';
  }
  return result;
}

// Ожидает исключение std::runtime_error с указанным фрагментом сообщения.
inline void expectThrowsWith(const std::string& grammar,
                             const std::string& messagePart) {
  try {
    static_cast<void>(encode(grammar));
    ADD_FAILURE() << "Expected std::runtime_error, but nothing was thrown";
  } catch (const std::runtime_error& error) {
    EXPECT_NE(std::string(error.what()).find(messagePart), std::string::npos)
        << "Unexpected message: " << error.what();
  } catch (...) {
    ADD_FAILURE()
        << "Expected std::runtime_error, but another exception was thrown";
  }
}

// Путь к грамматике в каталоге tests/data.
inline std::filesystem::path dataPath(const std::string& fileName) {
  return std::filesystem::path(__FILE__).parent_path() / "data" / fileName;
}

// Читает грамматику из tests/data через Handler::openGrmFile и выполняет
// её свёртку.
inline std::string encodeFile(const std::string& fileName) {
  std::ifstream file =
      Handler::openGrmFile(dataPath(fileName).string().c_str());
  Handler handler(file);
  return handler.handle();
}

// Ожидает исключение std::runtime_error с указанным фрагментом сообщения
// при чтении и свёртке файла.
inline void expectFileThrowsWith(const std::string& fileName,
                                 const std::string& messagePart) {
  try {
    static_cast<void>(encodeFile(fileName));
    ADD_FAILURE() << "Expected std::runtime_error for '" << fileName << "'";
  } catch (const std::runtime_error& error) {
    EXPECT_NE(std::string(error.what()).find(messagePart), std::string::npos)
        << "[" << fileName << "] Unexpected message: " << error.what();
  } catch (...) {
    ADD_FAILURE() << "Expected std::runtime_error for '" << fileName << "'";
  }
}

}  // namespace coding_test

#endif  // CODING_TESTS_TEST_HELPERS_H_