// Copyright 2026 Artem Nikitin

#include "Coding/handler.h"

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>
#include <unordered_map>

enum class Handler::State {
  TerminalIsExpected,
  DelimiterIsExpected,
  NonTerminalIsExpected,
  FinalState,
  FurtherProcessingIsNotPossible,
};

Handler(std::ifstream& h_file) {
  currentState = Handler::State::TerminalIsExpected;
  file = h_file;
}

std::string& Handler::handle() { throw std::runtime_error("Not implemented"); }

std::ifstream Handler::openGrmFile(const char* argumentPath) {
  std::filesystem::path filePath(argumentPath);

  if (!std::filesystem::exists(filePath)) {
    throw std::runtime_error("File does not exist" + filePath.string());
  }

  std::string extension = filePath.extension().string();
  std::ranges::transform(extension, extension.begin(), ::tolower);

  if (extension != ".grm") {
    throw std::runtime_error("Extension does not match" + extension);
  }

  std::ifstream file(filePath);
  if (!file || !file.is_open()) {
    throw std::runtime_error("Could not open " + filePath.string());
  }

  return file;
}
