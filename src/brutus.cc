#include "brutus.h"

#include "arena.h"
#include "ast.h"
#include "name.h"
#include "lexer.h"
#include "parser.h"
#include "stopwatch.h"
#include "streams.h"

const auto numIterations = 100000000;
const auto numTrials = 3;

void perf_test(std::function<int()> f) {
  brutus::internal::Stopwatch stopwatch;    

  for(auto trail = 0; trail < numTrials; ++trail) {
    stopwatch.start();
    std::cout << "Result: " << f() << std::endl;   
    stopwatch.stopAndLog();
  }
}

void withTokenFile(std::function<void(FILE*)> f) {
  auto fp = fopen("tokens.txt", "r");

  if(!fp) {
    std::cout << "Could not read tokens.txt file." << std::endl;
    return;
  }

  f(fp);

  fclose(fp);
}

int main(int argc, char** argv) {
  (void)argc;
  (void)argv;

#if 1
  withTokenFile([&](FILE* fp) {
    auto arena = new brutus::internal::Arena(
      /*initialCapacity = */512 * brutus::consts::KiloByte,
      /*blockSize = */brutus::consts::PageSize,
      /*alignment = */brutus::consts::Alignment);
    arena->init();
    auto stream = new brutus::internal::FileCharStream(fp);
    auto names = new brutus::internal::NameTable(arena);
    auto lexer = new brutus::internal::Lexer(stream); 
    auto parser = new brutus::internal::Parser(lexer, names, arena);
    auto ast = parser->parseProgram();
    auto printer = new brutus::internal::ast::ASTPrinter(std::cout);
    
    printer->print(ast);
    std::cout << std::endl;

    delete parser;
    delete lexer;
    delete stream;
    delete arena;
  });
#endif

#if 0
  withTokenFile([&](FILE* fp) {
    auto stream = new brutus::internal::FileCharStream(fp);
    auto lexer = new brutus::internal::Lexer(stream);

    brutus::internal::tok::Token t;
    auto numT = 0;

    while((t = lexer->nextToken()) != brutus::internal::tok::_EOF) {
      ++numT;
      std::cout << '(' << lexer->posLine() << ':' << lexer->posColumn() << ')' << ' ' << brutus::internal::tok::toString(t) << std::endl;

      if(brutus::internal::tok::hasValue(t)) {
        std::cout << lexer->value() << std::endl;
      }
    }

    std::cout << "Tokens: " << numT << std::endl;
  });
#endif

  return 0;
}

int main2(int argc, char** argv) {
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

  std::cout << "Test with normal loop:" << std::endl;

  perf_test(test0);

  std::cout << "Test with anonymous function:" << std::endl;

  perf_test(test1);

  std::cout << "Complete." << std::endl;

  return 0;
}
