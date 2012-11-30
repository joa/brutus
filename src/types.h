#ifndef _BRUTUS_TYPES_H
#define _BRUTUS_TYPES_H

#include "brutus.h"
#include "arena.h"
#include "scopes.h"
#include "symbols.h"

namespace brutus {
  namespace internal {
    namespace types {
      enum class TypeKind {
        kClass,
        kFunction
      };

      class Type : public ArenaMember {
        public:
          void* operator new(size_t size, Arena* arena) {
            return arena->alloc(size);
          }

          explicit Type(
            syms::Symbol* symbol,
            syms::Scope* scope,
            int numBaseTypes,
            Type** baseTypes,
            int numTypeParameters,
            Type** typeParameters) 
              : m_symbol(symbol), 
                m_scope(scope),
                m_numBaseTypes(numBaseTypes),
                m_baseTypes(baseTypes),
                m_numTypeParameters(numTypeParameters),
                m_typeParameters(typeParameters) {}
          virtual ~Type() {}
          virtual TypeKind kind() const = 0;

          syms::Symbol* symbol() const {
            return m_symbol;
          }

          syms::Scope* scope() const {
            return m_scope;
          }

          int numBaseTypes() const {
            return m_numBaseTypes;
          }

          Type** baseTypes() const {
            return m_baseTypes;
          }

          int numTypeParameters() const {
            return m_numTypeParameters;
          }

          Type** typeParameters() const {
            return m_typeParameters;
          }

        private:
          void* operator new(size_t size);

          syms::Symbol* m_symbol;
          syms::Scope* m_scope;
          int m_numBaseTypes;
          Type** m_baseTypes;
          int m_numTypeParameters;
          Type** m_typeParameters;

          DISALLOW_COPY_AND_ASSIGN(Type);
      };
      
      class ClassType : public Type {
        public:
          ClassType(
            syms::Symbol* symbol,
            syms::Scope* scope,
            int numBaseTypes,
            Type** baseTypes,
            int numTypeParameters,
            Type** typeParameters) 
              : Type(symbol, scope, numBaseTypes, baseTypes, numTypeParameters, typeParameters) {}

          TypeKind kind() const override final {
            return TypeKind::kClass;
          }

        private:
          DISALLOW_COPY_AND_ASSIGN(ClassType);
      };

      class FunctionType : public Type {
        public:
          FunctionType(
            syms::Symbol* symbol,
            syms::Scope* scope,
            int numTypeParameters,
            Type** typeParameters,
            int numParameters,
            syms::Symbol** parameters,
            Type* returnType)
              : Type(symbol, scope, 0, nullptr, numTypeParameters, typeParameters),
                m_numParameters(numParameters),
                m_parameters(parameters),
                m_returnType(returnType) {}

          TypeKind kind() const override final {
            return TypeKind::kFunction;
          }

          int numParameters() const {
            return m_numParameters;
          }

          syms::Symbol** parameters() const {
            return m_parameters;
          }

          Type* returnType() const {
            return m_returnType;
          }

        private:
          int m_numParameters;
          syms::Symbol** m_parameters;
          Type* m_returnType;

          DISALLOW_COPY_AND_ASSIGN(FunctionType);
      };
    }
  }
}

#endif