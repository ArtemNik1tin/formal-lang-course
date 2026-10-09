// Copyright 2026 Artem Nikitin

#include <gtest/gtest.h>

#include <string>

#include "test_helpers.h"

namespace coding_test {

namespace {

// Короткие корректные грамматики: проверка кодов таблицы на простых примерах.

TEST(CodeTableTest, SymbolCodesCorrespondToTask) {
  // ': 1, ( 2, ) 3, . 4, * 5, ; 6, , 7, # 8, [ 9, ] 10.
  EXPECT_EQ(
      encode("A : ( a ) ; [ b ] , c # d .\nEofgram\n"),
      codesToString({11, 1, 2, 51, 3, 6, 9, 52, 10, 7, 53, 8, 54, 4, 1000}));
}

TEST(CodeTableTest, OperatorsAndGroupsEncode) {
  // Скобки, итерация, альтернатива и опциональная часть.
  EXPECT_EQ(encode("A : ( x ; y )* [ z ] .\nEofgram\n"),
            codesToString({11, 1, 2, 51, 6, 52, 3, 5, 9, 53, 10, 4, 1000}));
}

TEST(NonTerminalTest, CodesStartFromElevenInOrderOfFirstAppearance) {
  EXPECT_EQ(encode("main : item .\nitem : begin ; end .\nEofgram\n"),
            codesToString({11, 1, 12, 4, 12, 1, 51, 6, 52, 4, 1000}));
}

TEST(NonTerminalTest, DefinedAfterFirstUseStillNonTerminal) {
  // other определяется правилом ниже, но первое вхождение уже нетерминал.
  EXPECT_EQ(encode("main : other .\nother : x .\nEofgram\n"),
            codesToString({11, 1, 12, 4, 12, 1, 51, 4, 1000}));
}

TEST(NonTerminalTest, UppercaseWordsAreNonTerminals) {
  // A и C — заглавные, нетерминалы; b — строчное слово, терминал.
  EXPECT_EQ(encode("A : b C .\nEofgram\n"),
            codesToString({11, 1, 51, 12, 4, 1000}));
}

TEST(TerminalTest, CodesStartFromFiftyOne) {
  EXPECT_EQ(encode("A : first second .\nEofgram\n"),
            codesToString({11, 1, 51, 52, 4, 1000}));
}

TEST(TerminalTest, QuotedPunctuationIsTerminalNotSymbol) {
  // ',' '[' ']' в кавычках — терминалы (51..53), а не символы (7, 9, 10).
  EXPECT_EQ(encode("A : ',' '[' ']' .\nEofgram\n"),
            codesToString({11, 1, 51, 52, 53, 4, 1000}));
}

TEST(TerminalTest, UnquotedBracketsAreSymbols) {
  // Заглавная B — нетерминал, [ и ] без кавычек — символы 9 и 10.
  EXPECT_EQ(encode("A : [ B ] .\nEofgram\n"),
            codesToString({11, 1, 9, 12, 10, 4, 1000}));
}

TEST(SemanticTest, CodesStartFrom101AndRepeatForSameLexeme) {
  // $print — первый (101), $out — второй (102). Повторный $print — снова 101.
  EXPECT_EQ(encode("A : $print $out $print .\nEofgram\n"),
            codesToString({11, 1, 101, 102, 101, 4, 1000}));
}

TEST(CodingTest, RepeatedLexemesKeepTheirCode) {
  EXPECT_EQ(encode("a : b b b .\nEofgram\n"),
            codesToString({11, 1, 51, 51, 51, 4, 1000}));
}

TEST(CodingTest, EofgramIsAlwaysCode1000AtTheEnd) {
  EXPECT_EQ(encode("A : x .\nEofgram\n"), codesToString({11, 1, 51, 4, 1000}));
}

TEST(CodingTest, WhitespaceAndLineBreaksDoNotMatter) {
  const std::string compact = "A : x . Eofgram";
  const std::string spread = "\n\n  A \t :\n  x  .\n\nEofgram\n";
  EXPECT_EQ(encode(compact), encode(spread));
  EXPECT_EQ(encode(compact), codesToString({11, 1, 51, 4, 1000}));
}

TEST(CodingTest, SingleQuoteInsideLiteralIsPreserved) {
  // Терминал — произвольная строка между кавычками, включая пробелы.
  EXPECT_EQ(encode("A : 'an apple' .\nEofgram\n"),
            codesToString({11, 1, 51, 4, 1000}));
}

}  // namespace

}  // namespace coding_test