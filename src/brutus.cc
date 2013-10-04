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

void withFile(const char* path, std::function<void(FILE*)> f) {
  auto fp = fopen(path, "r");

  if(!fp) {
    std::cout << "Could not read \"" << path << "\"." << std::endl;
    return;
  }

  f(fp);

  fclose(fp);

}

void withTokenFile(std::function<void(FILE*)> f) {
  withFile("tokens.txt", f);
}

int main(int argc, char** argv) {
  (void)argc;
  (void)argv;


#if 1
#ifdef PERF_TEST
  perf_test([&]() {
#endif
    withFile("lang.b", [&](FILE* lang) {
      withTokenFile([&](FILE* tokens) {
        brutus::Stopwatch stopwatch;
    
        auto compiler = new brutus::Compiler();
        compiler->addSource(lang);
        compiler->addSource(tokens);
    
        stopwatch.time([&]() {
          compiler->compile();
        });

        delete compiler;
      });    
    });
#ifdef PERF_TEST
  return 123;
  });
#endif
#endif

#if 0
  withFile("lang.b", [&](FILE* fp) {
    auto stream = new brutus::internal::FileCharStream(fp);
    auto lexer = new brutus::internal::Lexer();
    
    lexer->init(stream);

    brutus::internal::Token t;
    auto numT = 0;

    while((t = lexer->nextToken()) != brutus::internal::Token::kEof) {
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

  return 0;
}
