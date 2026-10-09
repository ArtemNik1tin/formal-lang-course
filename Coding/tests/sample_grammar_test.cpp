// Copyright 2026 Artem Nikitin

#include <gtest/gtest.h>

#include <algorithm>
#include <string>
#include <vector>

#include "test_helpers.h"

namespace coding_test {

namespace {

// Исходный текст эталонной грамматики из задания (test.GRM).
constexpr char kSampleGrammar[] =
    R"(expression : ( formula )*( ',' ) .
formula : ( *( $opcode1 'operation' ) operand $operand )*(
      ( $opcode2 'assignation' ; $opcode2 'operation' ) ) .
operand : ( (  $incr1 'increment' $tag 'tag' ;
          $tag 'tag' ( $incr2 'increment' ;
                         ( $array1 '[' formula $array2 ']' )* ;
                         $call1 '(' [ expression $call2 ]
                                 $call3 ')' ;
                         $ident ) ;
          $op1 '(' expression $op2 ')' )
               [ ( $dot '.' ; $arrow '->' ) $field '<tag>' ] ;
    $tag 'label' ;
    $number 'number' ;  $char 'char' ; $string 'string'
           )*( $cond1 '?' expression $cond2 ':' ) .
Eofgram)";

// Эталонная свёртка для kSampleGrammar (108 кодов).
const std::vector<int> kSampleCodes = {
    11,  1,   2,   12,  3,   5,  2,   51,  3,   4,   12, 1,   2,   5,  2,   101,
    52,  3,   13,  102, 3,   5,  2,   2,   103, 53,  6,  103, 52,  3,  3,   4,
    13,  1,   2,   2,   104, 54, 105, 55,  6,   105, 55, 2,   106, 54, 6,   2,
    107, 56,  12,  108, 57,  3,  5,   6,   109, 58,  9,  11,  110, 10, 111, 59,
    6,   112, 3,   6,   113, 58, 11,  114, 59,  3,   9,  2,   115, 60, 6,   116,
    61,  3,   117, 62,  10,  6,  105, 63,  6,   118, 64, 6,   119, 65, 6,   120,
    66,  3,   5,   2,   121, 67, 11,  122, 68,  3,   4,  1000};

TEST(SampleGrammarTest, EncodesToKnownCodeStream) {
  EXPECT_EQ(encode(kSampleGrammar), codesToString(kSampleCodes));
}

TEST(SampleGrammarTest, SameOutputAsTheReferenceFile) {
  // test.GRM в репозитории должен кодироваться так же, как встроенный текст.
  const std::filesystem::path reference =
      std::filesystem::path(__FILE__).parent_path().parent_path() / "test.GRM";
  if (!std::filesystem::exists(reference)) {
    GTEST_SKIP() << "test.GRM not found next to the sources";
  }
  std::ifstream file = Handler::openGrmFile(reference.string().c_str());
  Handler handler(file);
  EXPECT_EQ(handler.handle(), codesToString(kSampleCodes));
}

TEST(SampleGrammarTest, OutputIsSingleCodePerLine) {
  const std::string output = encode(kSampleGrammar);
  EXPECT_EQ(static_cast<size_t>(std::count(output.begin(), output.end(), '\n')),
            kSampleCodes.size());
}

}  // namespace

}  // namespace coding_test