// Copyright 2026 Artem Nikitin

#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>

static std::ifstream openGrmFile(const char* argumentPath) {
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

int main(const int argc, char* argv[]) {
  if (argc < 2) {
    std::cerr << "Usage: " << argv[0] << " <filename>" << std::endl;
    return 1;
  }

  std::ifstream file = openGrmFile(argv[1]);

  std::string line;
  while (std::getline(file, line)) {
    std::cout << line << std::endl;
  }
  return 0;
}
