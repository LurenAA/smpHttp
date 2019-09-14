#ifndef RADIX_TREE_HPP_
#define RADIX_TREE_HPP_
#include "radix_tree.h"
#include <memory>
#include <string>

namespace radix {
  class RadixTree {
    public: 
      RadixTree();
      void insert(const std::string &s, void* val);
      void insert(const char* s, int len, void*);
      void* route(const char* s);
      void* route(const std::string &s);
    private:
      std::shared_ptr<radix_tree> tree;
  };
}
#endif //RADIX_TREE_HPP_