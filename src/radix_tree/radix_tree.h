#ifndef __RADIX_TREE_XX_
#define __RADIX_TREE_XX_
#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <stdint.h>
#include <strings.h>
// #include "simpleQueue.h"

typedef void (*handle)(void*);

// 用位域处理，将四个域总体的大小限制到四个字节

typedef struct radix_node {
  uint32_t is_null: 1;
  uint32_t is_key: 1;
  uint32_t is_compressed: 1;
  uint32_t size: 29;
  char data[];
} radix_node;

typedef struct
{
  radix_node* root;
  int node_numbers;
  int key_numbers;
} radix_tree;

radix_tree* radixNewTree();
radix_node* radixNewNode(int children, bool is_key, bool is_compressed);
uint8_t radixInsert(radix_tree* tree, const char* key, int len, void* val);
int linkLowWalk(radix_tree*, const char* ,int ,radix_node**,radix_node***, int*);
void radixSetData(radix_node* node, void* val);
void* radixGetData(radix_node* node);
// void traversalDebug(radix_tree* tree, handle had);
radix_node* radixAddChild(radix_node* node, const char* key, int len, void* val, int i);
radix_node* radixRealloc(radix_node* ,int, int ); //添加一个data
radix_node* radixRealloc2(radix_node*); //添加val域
void showChildPtr(radix_node* nnew_cur);

void* route(radix_tree* tree, const char* path);
#ifdef __cplusplus
}
#endif
#endif