#include "compiler.h"

namespace brutus {
Compiler::Compiler() {
  m_arena = new internal::Arena(
    /*initialCapacity = */512 * consts::KiloByte,
    /*blockSize = */consts::PageSize * 4,
    /*alignment = */consts::Alignment);
  m_arena->init();
  m_arenaAlloc = new internal::ArenaAllocator(m_arena);
  m_names = new internal::NameTable(m_arena);
  m_symbolTable = new (m_arena) internal::syms::SymbolTable(m_names, m_arena);
  m_phases = new List<internal::Phase*>(m_arenaAlloc);
  m_units = new List<CompilationUnit*>(m_arenaAlloc);
  m_phases->addLast(new internal::ParsePhase(this));
  m_phases->addLast(new internal::SymbolsPhase(this));
  m_phases->addLast(new internal::LinkPhase(this));
  m_phase = 0;
}

Compiler::~Compiler() {
  m_units->foreach([](CompilationUnit* unit) {
    delete unit;
  });

  m_phases->foreach([](internal::Phase* phase) {
    delete phase;
  });

  delete m_units;
  delete m_phases;
  delete m_names;
  delete m_arenaAlloc;
  m_arena->deleteAll();
  delete m_arena;
}

void Compiler::addSource(FILE* fp) {
  auto unit = new CompilationUnit();
  unit->source(new FileSource(fp));

  m_units->addLast(unit);
}

void Compiler::compile() {
  m_phase = 0;

  m_phases->foreach([&](internal::Phase* phase) {
    phase->stopwatch()->start();

    m_units->foreach([&](CompilationUnit* unit) {
      phase->apply(unit);
    });

    phase->stopwatch()->pause();
    phase->log(); //TODO(joa): of course not here
    ++m_phase;
  });
}

internal::ast::Node* CompilationUnit::ast() const {
  return m_ast;
}

void CompilationUnit::ast(internal::ast::Node* value) {
  m_ast = value;
}

Source* CompilationUnit::source() const {
  return m_source;
}

void CompilationUnit::source(Source* value) {
  m_source = value;
}
}
