#include "brutus.h"

#include <iostream>
#include <functional>

const auto numIterations = 100000000;
const auto numTrials = 3;

auto perf_test(std::function<int()> f) -> void {
  brutus::Stopwatch stopwatch;    

  for(auto trail = 0; trail < numTrials; ++trail) {
    stopwatch.start();
    std::cout << u8"Result: " << f() << std::endl;   
    stopwatch.stopAndLog();
  }
}

auto main(int argc, char** argv) -> int {
  (void)argc;
  (void)argv;

  auto fp = fopen(u8"tokens.txt", u8"r");

  if(!fp) {
    return 1;
  }

  auto stream = new brutus::FileCharStream(fp);
  auto lexer = new brutus::Lexer(stream); 

  brutus::tok::Token t;
  auto numT = 0;

  while((t = lexer->nextToken()) != brutus::tok::_EOF) {
    ++numT;
    std::cout << brutus::tok::toString(t) << std::endl;
  }

  std::cout << u8"tokens: " << numT << std::endl;

  fclose(fp);

  return 0;
}

auto main2(int argc, char** argv) -> int {
  (void)argc;
  (void)argv;
  
  auto test0 = [&]() -> int {
    auto sum = 0;
   
    for(auto i = 0; i < numIterations; ++i) {
      sum += i;
    }

    return sum;
  };

  auto test1 = [&]() -> int {
    auto sum = 0;
    auto i = 0;

    auto add = [&]() {
      sum += i;
    };

    for(; i < numIterations; ++i) {
      add();
    }

    return sum;
  };

  std::cout << u8"Test with normal loop:" << std::endl;

  perf_test(test0);

  std::cout << u8"Test with anonymous function:" << std::endl;

  perf_test(test1);

  std::cout << u8"Complete." << std::endl;

  return 0;
}
