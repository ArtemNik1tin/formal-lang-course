// Copyright 2026 Artem Nikitin

#include <gtest/gtest.h>

#include <string>

#include "test_helpers.h"

namespace coding_test {

namespace {

// Короткие грамматики с синтаксическими ошибками и ошибками структуры.

TEST(ErrorTest, ThrowsWhenEofgramIsMissing) {
  expectThrowsWith("A : b .\n", "Eofgram was not found");
}

TEST(ErrorTest, ThrowsOnEmptyInput) {
  expectThrowsWith("", "Eofgram was not found");
  expectThrowsWith(" \t \n ", "Eofgram was not found");
}

TEST(ErrorTest, ThrowsOnLexemesAfterEofgram) {
  expectThrowsWith("A : b .\nEofgram\nc .\n", "after Eofgram");
}

TEST(ErrorTest, ThrowsOnUnterminatedTerminalLiteral) {
  expectThrowsWith("A : 'open .\nEofgram\n", "Unterminated terminal literal");
}

TEST(ErrorTest, ThrowsOnUnexpectedCharacter) {
  expectThrowsWith("A : b @ .\nEofgram\n", "Unexpected character: @");
}

TEST(ErrorTest, ThrowsWhenColonIsMissingAfterNonTerminal) {
  expectThrowsWith("A b c .\nEofgram\n", "':' was expected");
}

TEST(ErrorTest, ThrowsWhenRuleDoesNotStartWithNonTerminal) {
  // Строчное a нигде не объявлено правилом — это терминал, не нетерминал.
  expectThrowsWith("a b c .\nEofgram\n", "A nonterminal was expected");
}

TEST(ErrorTest, ThrowsOnStrayColonInRuleBody) {
  expectThrowsWith("A : x B : y .\nEofgram\n", "':' is unexpected");
}

TEST(ErrorTest, ThrowsWhenRuleEndsWithoutDotBeforeNextRule) {
  // Имя B — не операнд, а нетерминал нового правила: точка у A потеряна.
  expectThrowsWith("A : x\nB : y .\nEofgram\n", "':' is unexpected");
}

TEST(ErrorTest, ThrowsWhenEofgramFollowsUnfinishedRule) {
  expectThrowsWith("A : x Eofgram", "Expected '.' before Eofgram");
}

TEST(ErrorTest, ThrowsOnUnmatchedClosingBracket) {
  expectThrowsWith("A : x ) .\nEofgram\n", "Unmatched ')'");
  expectThrowsWith("A : x ] .\nEofgram\n", "Unmatched ']'");
}

TEST(ErrorTest, ThrowsOnMismatchedBrackets) {
  expectThrowsWith("A : ( x ] .\nEofgram\n", "Mismatched ']'");
  expectThrowsWith("A : [ x ) .\nEofgram\n", "Mismatched ')'");
}

TEST(ErrorTest, ThrowsOnUnclosedBracketAtRuleEnd) {
  expectThrowsWith("A : ( x .\nEofgram\n", "Unmatched '('");
  expectThrowsWith("A : [ x .\nEofgram\n", "Unmatched '['");
}

TEST(ErrorTest, ThrowsOnUnclosedBracketInNestedRule) {
  expectThrowsWith("A : ( ( x ) .\nEofgram\n", "Unmatched '('");
}

}  // namespace

}  // namespace coding_test