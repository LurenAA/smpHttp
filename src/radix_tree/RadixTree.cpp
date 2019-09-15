#include "RadixTree.hpp"

using namespace std;
using namespace radix;

RadixTree::RadixTree() :
  tree(radixNewTree())
{}

void RadixTree::insert(const std::string& s, void* val){
  radixInsert(tree.get(), s.c_str(), s.size(), val);
}

void RadixTree::insert(const char* s, int len, void* val) {
  radixInsert(tree.get(), s, len, val);
}

void* RadixTree::route(const char* s){
  return ::route(tree.get(), s);
}

void* RadixTree::route(const std::string &s) {
  return route(s.c_str());
}