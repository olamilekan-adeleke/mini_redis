#ifndef PARSER_H
#define PARSER_H

#include <string>
#include <vector>

class RespParser {
private:
  static const int CR = 13;            // /r
  static const int LF = 10;            // /n
  static const int DOLLAR_SIGN = 36;   // $
  static const int ASTERISK_SIGN = 42; // *

  std::vector<uint8_t> buffer;
  int position = 0;

  uint8_t peekByte();
  void eatByte(int expected);
  int locateCRLF();
  int readInt();

  std::optional<std::string> parseBulkString();
  std::vector<std::string> parseArray();

public:
  RespParser(std::vector<uint8_t> &buf);

  std::vector<std::string> parse();
};

#endif // !PARSER_H
