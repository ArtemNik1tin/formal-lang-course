// Copyright 2026 Artem Nikitin

#include <gtest/gtest.h>

#include <string>

#include "test_helpers.h"

namespace coding_test {

namespace {

// Переполнение диапазонов кодов таблицы.

TEST(OverflowTest, ThrowsWhenTooManyNonTerminals) {
  std::string grammar;
  for (int i = 1; i <= 41; ++i) {  // коды 11..50 — всего 40 нетерминалов.
    grammar += "n" + std::to_string(i) + " : .\n";
  }
  grammar += "Eofgram\n";
  expectThrowsWith(grammar, "Code table overflow for 'N:n41'");
}

TEST(OverflowTest, ThrowsWhenTooManyTerminals) {
  std::string grammar = "A :";
  for (int i = 1; i <= 51; ++i) {  // коды 51..100 — всего 50 терминалов.
    grammar += " t" + std::to_string(i);
  }
  grammar += " .\nEofgram\n";
  expectThrowsWith(grammar, "Code table overflow for 'T:t51'");
}

TEST(OverflowTest, ThrowsWhenTooManySemantics) {
  std::string grammar = "A :";
  for (int i = 1; i <= 51; ++i) {  // коды 101..150 — всего 50 семантик.
    grammar += " $s" + std::to_string(i);
  }
  grammar += " .\nEofgram\n";
  expectThrowsWith(grammar, "Code table overflow for 'S:$s51'");
}

TEST(OverflowTest, ExactUpperBoundariesAreAccepted) {
  // Ровно 40 нетерминалов (n1..n40) и 50 терминалов (t1..t50) — без исключений.
  std::string grammar;
  for (int i = 1; i <= 40; ++i) {
    grammar += "n" + std::to_string(i) + " : t" + std::to_string(i) + " .\n";
  }
  grammar += "n40 :";
  for (int i = 41; i <= 50; ++i) {
    grammar += " t" + std::to_string(i);
  }
  grammar += " .\nEofgram\n";
  EXPECT_NO_THROW(static_cast<void>(encode(grammar)));
}

}  // namespace

}  // namespace coding_test