// Copyright 2026 Artem Nikitin

#ifndef CODING_HANDLER_H_
#define CODING_HANDLER_H_

#include <fstream>
#include <string>
#include <unordered_map>

class Handler {
  enum class State;
  std::unordered_map<std::string, int> CodeTable;
  State currentState;
  std::ifstream file;
public:
  std::string& handle();
  static std::ifstream openGrmFile(const char* argumentPath);
};


#endif  // CODING_HANDLER_H_
