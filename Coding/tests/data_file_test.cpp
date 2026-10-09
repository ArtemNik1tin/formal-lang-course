// Copyright 2026 Artem Nikitin

#include <gtest/gtest.h>

#include <cstdint>
#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <string>
#include <vector>

#include "test_helpers.h"

namespace coding_test {

namespace {

// Чтение и свёртка маленьких грамматик из каталога tests/data.

TEST(DataFileTest, ValidMinimalGrammarEncodes) {
  EXPECT_EQ(encodeFile("valid_minimal.grm"),
            codesToString({11, 1, 51, 4, 1000}));
}

TEST(DataFileTest, ValidFeaturesGrammarEncodes) {
  EXPECT_EQ(
      encodeFile("valid_features.grm"),
      codesToString({11, 1, 2, 51, 6, 52, 3, 5, 9, 101, 53, 54, 10, 4, 1000}));
}

TEST(DataFileTest, InvalidNoEofThrows) {
  expectFileThrowsWith("invalid_no_eof.grm", "Eofgram was not found");
}

TEST(DataFileTest, InvalidUnterminatedLiteralThrows) {
  expectFileThrowsWith("invalid_unterminated.grm",
                       "Unterminated terminal literal");
}

TEST(DataFileTest, InvalidBadCharacterThrows) {
  expectFileThrowsWith("invalid_bad_char.grm", "Unexpected character: @");
}

TEST(DataFileTest, InvalidMissingColonThrows) {
  expectFileThrowsWith("invalid_no_colon.grm", "':' was expected");
}

TEST(DataFileTest, InvalidStrayColonThrows) {
  expectFileThrowsWith("invalid_stray_colon.grm", "':' is unexpected");
}

TEST(DataFileTest, InvalidUnbalancedBracketsThrows) {
  expectFileThrowsWith("invalid_unbalanced.grm", "Unmatched '('");
}

TEST(DataFileTest, InvalidLexemesAfterEofgramThrows) {
  expectFileThrowsWith("invalid_after_eof.grm", "after Eofgram");
}

// Handler::openGrmFile: проверки открытия файлов.

class OpenGrmFileTest : public ::testing::Test {
 protected:
  void TearDown() override {
    for (const std::filesystem::path& path : createdFiles) {
      std::error_code error;
      std::filesystem::remove(path, error);
    }
    createdFiles.clear();
  }

  std::filesystem::path createFile(const std::string& content,
                                   const std::string& extension = ".grm") {
    const std::filesystem::path path =
        std::filesystem::temp_directory_path() /
        ("coding_test_" + std::to_string(++fileCounter) + extension);
    std::ofstream out(path);
    out << content;
    createdFiles.push_back(path);
    return path;
  }

  std::vector<std::filesystem::path> createdFiles;
  static std::uint64_t fileCounter;
};

std::uint64_t OpenGrmFileTest::fileCounter = 0;

TEST_F(OpenGrmFileTest, OpensAndEncodesExistingGrmFile) {
  const std::filesystem::path path = createFile("A : x .\nEofgram\n");
  ASSERT_TRUE(std::filesystem::exists(path));

  std::ifstream file = Handler::openGrmFile(path.string().c_str());
  ASSERT_TRUE(file.is_open());
  Handler handler(file);
  EXPECT_EQ(handler.handle(), codesToString({11, 1, 51, 4, 1000}));
}

TEST_F(OpenGrmFileTest, ThrowsWhenFileDoesNotExist) {
  const std::filesystem::path path =
      std::filesystem::temp_directory_path() / "coding_test_missing.grm";
  EXPECT_THROW(Handler::openGrmFile(path.string().c_str()), std::runtime_error);
}

TEST_F(OpenGrmFileTest, ThrowsWhenExtensionIsWrong) {
  const std::filesystem::path path = createFile("whatever", ".txt");
  EXPECT_THROW(Handler::openGrmFile(path.string().c_str()), std::runtime_error);
}

}  // namespace

}  // namespace coding_test