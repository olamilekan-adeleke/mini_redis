#include "../../include/utils/my_utils.hpp"
#include <cstdint>
#include <cstdio>
#include <iostream>

void printByteValue(uint8_t byteValue) {
  if (byteValue == 13) {
    std::cout << "CR";
  } else if (byteValue == 10) {
    std::cout << "LF";
  } else {
    printf("%x", byteValue);
    if (byteValue >= 32 && byteValue <= 126) {
      printf(" --> %c", byteValue);
    } else {
      printf(" --> %x", byteValue);
    }
  }
  std::cout << std::endl;
}
