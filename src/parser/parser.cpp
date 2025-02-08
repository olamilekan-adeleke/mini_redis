#include "../../include/parser/parser.hpp"
#include <cstdio>
#include <iostream>
#include <optional>
#include <sstream>

std::vector<std::string> _parseArray();
std::optional<std::string> _parseBulkString();
std::vector<std::string> parse();

RespParser::RespParser(std::vector<uint8_t> &buf) : buffer(std::move(buf)) {};

std::vector<std::string> RespParser::parse() {
  uint8_t type = buffer[position];

  if (type == ASTERISK_SIGN) {
    position++;
    return parseArray();
  } else if (type == DOLLAR_SIGN) {
    position++;
    auto bulkString = parseBulkString();
    if (bulkString.has_value()) {
      return {bulkString.value()};
    } else {
      return {};
    }
  }

  std::stringstream err;
  err << "Unsupported RESP type: " << static_cast<char>(type) << " = " << type;
  throw std::runtime_error(err.str());
}

std::vector<std::string> RespParser::parseArray() {
  std::vector<std::string> arr;

  int byteLength = readInt();
  for (auto i = 0; i < byteLength; i++) {
    std::vector<std::string> parsed = parse();
    arr.insert(arr.end(), parsed.begin(), parsed.end());
  }
  return arr;
}

std::optional<std::string> RespParser::parseBulkString() {
  int byteLength = readInt();
  if (byteLength == -1) {
    return std::nullopt;
  }

  if (position + byteLength > buffer.size()) {
    throw std::runtime_error("Bulk string length exceeds buffer size");
  }

  std::string str(buffer.begin() + position,
                  buffer.begin() + position + byteLength);
  position += byteLength + 2;
  return str;
}

uint8_t RespParser::peekByte() {
  if (position >= buffer.size()) {
    throw std::runtime_error("Buffer underflow: No more bytes to read");
  }
  return buffer[position + 1];
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
  for (size_t i = position; i < buffer.size() - 1; i++) {
    if (buffer[i] == CR && buffer[i + 1] == LF) {
      return static_cast<int>(i);
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
    position = end_of_int + 2;
    return value;
  } catch (const std::invalid_argument &e) {
    throw std::runtime_error("Invalid integer format");
  } catch (const std::out_of_range &e) {
    throw std::runtime_error("Integer out of range");
  }
}
