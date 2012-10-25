#include "brutus.h"

#include "arena.h"
#include "ast.h"
#include "name.h"
#include "lexer.h"
#include "parser.h"
#include "stopwatch.h"
#include "streams.h"

//#define PERF_TEST
const auto numIterations = 100000000;
const auto numTrials = 10000;

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
#ifdef PERF_TEST
  perf_test([&]() -> int {
    int size;
#endif
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
      
#ifndef PERF_TEST
      printer->print(ast);
      std::cout << std::endl;
      std::cout << "Names: " << names->size() << std::endl;
#else
      size = names->size();
#endif

      delete printer;
      delete parser;
      delete lexer;
      delete names;
      delete stream;
      delete arena;
    });
#ifdef PERF_TEST
    return size;
  });
#endif
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

    delete lexer;
    delete stream;
  });
#endif

  system("PAUSE");
  return 0;
}
