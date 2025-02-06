#include "../../include/parser/parser.hpp"
#include <iostream>
#include <sstream>

std::vector<std::any> _parseArray();
std::optional<std::string> _parseBulkString();
std::any parse();

RespParser::RespParser(std::vector<uint8_t> &buf) : buffer(std::move(buf)) {};

std::any RespParser::parse() {
  uint8_t type = peekByte();

  if (type == ASTERISK_SIGN) {
    return parseArray();
  } else if (type == DOLLAR_SIGN) {
    return parseBulkString();
  }

  std::stringstream err;
  err << "Unsupported RESP type: " << static_cast<char>(type) << " = " << type;
  throw std::runtime_error(err.str());
}

std::vector<std::any> RespParser::parseArray() {
  eatByte(ASTERISK_SIGN);

  std::vector<std::any> arr;

  int byteLength = readInt();
  for (auto i = 0; i < byteLength; i++) {
    arr.push_back(parse());
  }
  return arr;
}

std::optional<std::string> RespParser::parseBulkString() {
  eatByte(DOLLAR_SIGN);

  int byteLength = readInt();
  if (byteLength == -1) {
    return nullptr;
  }

  // TODO: check for buffer overflow

  std::string str(buffer.begin() + position,
                  buffer.begin() + position + byteLength);
  position += byteLength;
  eatByte(CR);
  eatByte(LF);
  return str;
}

uint8_t RespParser::peekByte() {
  if (position >= buffer.size()) {
    throw std::runtime_error("Buffer underflow: No more bytes to read");
  }
  return buffer[position];
}

void RespParser::eatByte(int expected) {
  if (position >= buffer.size()) {
    throw std::runtime_error("Unexpected end of buffef");
  }

  int actual = peekByte();

  if (actual != expected) {
    std::stringstream ss;
    ss << "Expected: " << static_cast<char>(expected) << " (0x" << std::hex
       << expected << "), but got: " << static_cast<char>(actual) << " (0x"
       << std::hex << actual << ")";
    throw std::runtime_error(ss.str());
  }
  position++;
}

int RespParser::locateCRLF() {
  for (auto i = position; i < buffer.size() - 1; i++) {
    if (buffer[i] == CR && buffer[i + 1] == LF) {
      return i;
    }
  }
  return -1;
}

int RespParser::readInt() {
  int end_of_int = locateCRLF();
  if (end_of_int == -1) {
    throw std::runtime_error("CRLF not found");
  }

  std::string line(buffer.begin() + position, buffer.begin() + end_of_int);

  try {
    int value = std::stoi(line);
    std::cout << "readInt: " << value << std::endl;
    eatByte(CR);
    eatByte(LF);
    return value;
  } catch (const std::invalid_argument &e) {
    throw std::runtime_error("Invalid integer format");
  } catch (const std::out_of_range &e) {
    throw std::runtime_error("Integer out of range");
  }
}
