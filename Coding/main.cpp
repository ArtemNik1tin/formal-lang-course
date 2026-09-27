// Copyright 2026 Artem Nikitin

#include <filesystem>
#include <fstream>
#include <iostream>
#include <string>

#include "Coding/handler.h"

int main(const int argc, char* argv[]) {
  if (argc < 2) {
    std::cerr << "Usage: " << argv[0] << " <filename>" << std::endl;
    return 1;
  }

  std::ifstream file = Handler::openGrmFile(argv[1]);

  std::string line;
  while (std::getline(file, line)) {
    std::cout << line << std::endl;
  }
  return 0;
}
