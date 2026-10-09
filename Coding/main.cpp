// Copyright 2026 Artem Nikitin

/// @file main.cpp
/// @brief Точка входа программы Coding.
/// @author Artem Nikitin

#include <exception>
#include <fstream>
#include <iostream>

#include "Coding/handler.h"

/// @brief Запускает свёртку лексем грамматики из указанного файла в числовой
/// код и выводит результат.
/// @param argc Количество аргументов командной строки.
/// @param argv Аргументы командной строки; `argv[1]` — путь к файлу `.grm`.
/// @return 0 при успехе, 1 при ошибке аргументов или обработки.
int main(const int argc, char* argv[]) {
  if (argc < 2) {
    std::cerr << "Usage: " << argv[0] << " <filename>" << std::endl;
    return 1;
  }

  try {
    std::ifstream file = Handler::openGrmFile(argv[1]);
    Handler handler(file);
    std::cout << handler.handle();
  } catch (const std::exception& error) {
    std::cerr << "Error: " << error.what() << std::endl;
    return 1;
  }

  return 0;
}
