#include "brutus.h"

#include <iostream>
#include <functional>

const int numIterations = 100000000;
const int numTrials = 3;

void perf_test(std::function< int()> f) {  
  brutus::Stopwatch stopwatch;    

  for(int trail = 0; trail < numTrials; ++trail) {
    stopwatch.start();
    std::cout << "Result: " << f() << std::endl;   
    stopwatch.stopAndLog();
  }
}

int main(int argc, char** argv) {
  (void)argc;
  (void)argv;

  auto fp = fopen("tokens.txt", "r");

  if(!fp) {
    return 1;
  }

  auto stream = new brutus::FileCharStream(fp);
  auto lexer = new brutus::Lexer(stream); 

  brutus::tok::Token t;
  int numT = 0;

  while((t = lexer->nextToken()) != brutus::tok::_EOF) {
    ++numT;
    printf("%s\n", brutus::tok::toString(t));
  }

  printf("tokens: %d\n", numT);

  fclose(fp);

  return 0;
}

int unused(int argc, char** argv) {
  (void)argc;
  (void)argv;
  
  auto test0 = [&]() -> int {
    int sum = 0;
   
    for(int i = 0; i < numIterations; ++i) {
      sum += i;
    }

    return sum;
  };

  auto test1 = [&]() -> int {
    int sum = 0;
    int i = 0;

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
