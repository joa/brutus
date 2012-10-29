#ifndef _BRUTUS_LIST_H
#define _BRUTUS_LIST_H

#include "brutus.h"
#include "alloc.h"

namespace brutus {
  template<typename T>
  class List {
    private:
      class Entry {
        public:
          explicit Entry() {}

          T m_value;
          Entry* m_prev;
          Entry* m_next;

        private:
          DISALLOW_COPY_AND_ASSIGN(Entry);
      };

    public:
      List() : List(internal::HeapAllocator::instance()) {}
      List(internal::Allocator* allocator)
          : m_first(nullptr),
            m_last(nullptr),
            m_size(0),
            m_alloc(allocator) {}

      ~List() {
        clear();
      }

      T first() const {
        return m_first;
      }

      T last() const {
        return m_last;
      }

      int size() const {
        return m_size;
      }

      bool isEmpty() const {
        return m_size == 0;
      }

      bool nonEmpty() const {
        return m_size != 0;
      }

      void addFirst(T value) {
        Entry* entry = m_alloc->create<Entry>();
        
        if(nullptr != m_first) {
          m_first->m_prev = entry;
        }

        if(nullptr == m_last) {
          m_last = entry;
        }

        entry->m_prev = nullptr;
        entry->m_next = m_first;
        entry->m_value = value;

        m_first = entry;
        ++m_size;
      }

      void addLast(T value) {
        Entry* entry = m_alloc->create<Entry>();

        if(nullptr != m_last) {
          m_last->m_next = entry;
        }

        if(nullptr == m_first) {
          m_first = entry;
        }

        entry->m_prev = m_last;
        entry->m_next = nullptr;
        entry->m_value = value;

        m_last = entry;
        ++m_size;
      }

      int indexOf(T value) {
        int index = 0;
        auto entry = m_first;

        while(entry != nullptr) {
          if(entry->m_value == value) {
            return index;
          }

          ++index;
          entry = entry->m_next;
        }

        return -1;
      }

      void removeFirst() {
        if(nullptr == m_first) {
          return;
        }

        auto oldFirst = m_first;
        auto newFirst = m_first->m_next;

        if(nullptr != newFirst) {
          newFirst->m_prev = nullptr;
        }

        m_alloc->free(oldFirst);
        m_first = newFirst;
        --m_size;
      }

      void removeLast() {
        if(nullptr == m_last) {
          return;
        }

        auto oldLast = m_last;
        auto newLast = m_last->m_prev;

        if(nullptr != newLast) {
          newLast->m_next = nullptr;
        }

        m_alloc->free(oldLast);
        m_last = newLast;
        --m_size;
      }

      void foreach(std::function<void(T)> f) {
        auto entry = m_first;

        while(nullptr != entry) {
          f(entry->m_value);
          entry = entry->m_next;
        }
      }

      bool forall(std::function<bool(T)> f) {
        auto entry = m_first;

        while(nullptr != entry) {
          if(!f(entry->m_value)) {
            return NO;
          }
        }

        return YES;
      }

      bool exists(std::function<bool(T)> f) {
        auto entry = m_first;

        while(nullptr != entry) {
          if(f(entry->m_value)) {
            return YES;
          }
        }

        return NO;
      }

      void remove(T value) {
        if(nullptr != m_first && m_first->m_value == value) {
          removeFirst();
          return;
        }

        if(nullptr != m_last && m_last->m_value == value) {
          removeLast();
          return;
        }

        auto entry = m_first->m_next;

        while(nullptr != entry) {
          if(entry->m_value == value) {
            auto prev = entry->m_prev;
            auto next = entry->m_next;

            prev->m_next = next;
            next->m_prev = prev;

            m_alloc->free(entry);
            --m_size;
          }

          entry = entry->m_next;
        }
      }

      void clear() {
#ifdef DEBUG
        if(nullptr == m_alloc) {
          std::cerr << "Error: Allocator is already null.";
        }
#endif

        auto entry = m_first;

        m_first = nullptr;
        m_last = nullptr;
        m_size = 0;

        if(!m_alloc->freeSupported()) {
          return;
        }

        while(entry != nullptr) {
          auto next = entry->m_next;
          m_alloc->free(entry);
          entry = next;
        }
      }

    private:
      Entry* m_first;
      Entry* m_last;
      int m_size;
      internal::Allocator* m_alloc;
  };
}
#endif