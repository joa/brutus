#include "brutus.h"

#include "arena.h"
#include "ast.h"
#include "name.h"
#include "lexer.h"
#include "parser.h"
#include "stopwatch.h"
#include "streams.h"
#include "phases.h"
#include "symbols.h"
#include "compiler.h"

//#define PERF_TEST
const auto numTrials = 10000;

void perf_test(std::function<int()> f) {
  brutus::Stopwatch stopwatch;    

  for(auto trail = 0; trail < numTrials; ++trail) {
    stopwatch.time([&]() {
      std::cout << "Result: " << f() << std::endl;   
    });
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
  perf_test([&]() {
#endif
  withTokenFile([&](FILE* fp) {
    brutus::Stopwatch stopwatch;
    
    auto compiler = new brutus::Compiler();
    compiler->addSource(fp);
    
    stopwatch.time([&]() {
      compiler->compile();
    });

    delete compiler;
  });
#ifdef PERF_TEST
  return 123;
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

  return system("PAUSE");
}
