#include "radix_tree.h"

/**
 * 是否显示debug信息
 **/
#ifdef RADIX_DEBUG_MSG
#define debugf(...)                                                        \
  fprintf(stderr, "debug: %s:%s:%d:\t", __FILE__, __FUNCTION__, __LINE__); \
  fprintf(stderr, __VA_ARGS__);                                            \
  fprintf(stderr, "\n");                                                   \
  fflush(NULL);
#else
#define debugf(...)
#endif

/**
 * 充填字节
 **/
#define padding(n) \
  ((sizeof(void *) - (sizeof(radix_node) + n) % sizeof(void *)) & (sizeof(void *) - 1))

/**
 * 获取当前长度
 **/
#define radixLength(h)                                                          \
  (sizeof(radix_node) + padding(h->size) + h->size +                            \
   (h->is_compressed ? sizeof(radix_node *) : h->size * sizeof(radix_node *)) + \
   (h->is_key ? sizeof(void *) : 0))

/**
 * 获取值
 **/
#define radixGetVal(h) \
  (void **)((char *)h + radixLength(h) - sizeof(void *))

/**
 * 第一个子节点
 **/
#define radixFirstChild(h) \
  (radix_node **)((char *)&h->data + h->size + padding(h->size))

/**
 * 第n个子节点
 **/
#define radixNthChild(h, n) \
  (radix_node **)((char *)&h->data + h->size + padding(h->size) + n * sizeof(void *))

/**
 * 初始化一个基数树
 * @param null
 **/
radix_tree *radixNewTree()
{
  radix_tree *tree = (radix_tree *)malloc(sizeof(radix_tree));
  if (!tree)
  {
    debugf("%s", strerror(errno));
    return NULL;
  }
  tree->root = radixNewNode(0, false, false);
  if (!tree->root)
  {
    debugf("%s", strerror(errno));
    return NULL;
  }
  tree->key_numbers = 0;
  tree->node_numbers = 0;

  debugf("init a radix_tree");
  return tree;
}

/**
 * 初始化一个基数树的节点
 *  @param：children 子节点数量
 *  @param: is_key 是否有键
 *  @param: is_compressed 
 **/
radix_node *radixNewNode(int children, bool is_key, bool is_compressed)
{
  int size = sizeof(radix_node) + children +
             padding(children) +
             (is_compressed ? sizeof(radix_node *) : sizeof(radix_node *) * children);
  debugf("padding: %ld", padding(children));
  if (is_key)
    size += sizeof(void *);
  radix_node *node = (radix_node *)malloc(size);
  if (!node)
  {
    debugf("%s", strerror(errno));
    return NULL;
  }
  node->is_compressed = is_compressed;
  node->size = children;
  node->is_null = 0;
  node->is_key = is_key;

  debugf("init a radix_node(%d)", size);
  return node;
}

/**
 * 插入一个节点
 * @param: key 键
 * @param: len 键长
 * @param: val 值
 * @return: 1插入成功，0插入失败,2覆盖原来的
 **/
uint8_t radixInsert(radix_tree *tree, const char *key, int len, void *val)
{
  int i = 0;                //当前key的结束点
  int j = 0;                //在树中节点的结束处
  radix_node *cur, **plink; //cur当前节点， plink父节点
  i = linkLowWalk(tree, key, len, &cur, &plink, &j);

  debugf("linkLowWalk(i:j): %d:%d", i, j);
  /**
   * 1.
   *  alligator -> []             l -》igator -> []
   *                   ->    al ->
   *  alien                       i -》en -> []        
   * 2.
   *  alligator          a  ->   lligator -> []   
   *              ->        
   *  pilipili           p  ->  ilipili  -> []
   * 3.
   *  abc                c  -> []
   *       ->     ab -> 
   *  abe                e -> []
   **/
  if (cur->is_compressed && i != len)
  {
    debugf("cur->Data: %.*s", cur->size, (char *)cur->data);
    radix_node **p2next = radixFirstChild(cur),
               *next;
    memcpy(&next, p2next, sizeof(void *)); //save next

    bool isBegin = (j == 0);
    //创建cur代替节点
    radix_node *new_cur;
    if (isBegin)
    {
      new_cur = radixNewNode(2, cur->is_key, false);
    }
    else
      new_cur = radixNewNode(j, cur->is_key, j > 1);
    if (!new_cur)
    {
      debugf("%s", strerror(errno));
      return -1;
    }
    if (cur->is_key)
    {
      void *data = radixGetData(cur);
      void **new_cur_data = radixGetVal(new_cur);
      memcpy(new_cur_data, &data, sizeof(void *));
      debugf("new_cur_data: %s", (char *)*new_cur_data);
    }
    memcpy(plink, &new_cur, sizeof(void *)); //代替cur

    //创建cur后缀节点
    int suffix_len = cur->size - j - 1;
    radix_node *new_cur_suffix;
    if (suffix_len)
    { //可能不存在后继节点
      new_cur_suffix = radixNewNode(suffix_len, false, suffix_len > 1);
      if (!new_cur_suffix)
      {
        debugf("%s", strerror(errno));
        return -1;
      }
      memcpy(&new_cur_suffix->data, cur->data + j + 1, suffix_len);
      debugf("new_cur_suffix: %s", (char *)new_cur_suffix->data);
      memcpy(radixGetVal(new_cur_suffix), p2next, sizeof(void *));
      debugf("new_cur_suffix->key: %p", *radixFirstChild(new_cur_suffix));
    }

    //创建新来的插入节点的后继部分
    int insert_dif_len = len - i - 1;
    //可能不存在后续部分
    radix_node *insert_node;
    if (insert_dif_len)
    {
      insert_node = radixNewNode(insert_dif_len, false, insert_dif_len > 1);
      if (!insert_node)
      {
        debugf("%s", strerror(errno));
        return -1;
      }
      memcpy(insert_node->data, key + i + 1, insert_dif_len);
      debugf("insert_node->data: %s", (char *)&insert_node->data);
    }

    //创建插入节点后继的key值节点
    radix_node *insert_key = radixNewNode(0, true, false);
    if (!insert_key)
    {
      debugf("%s", strerror(errno));
      return -1;
    }
    void **insert_key_val = radixGetVal(insert_key);
    memcpy(insert_key_val, &val, sizeof(void *));
    debugf("insert_key_val: %s", (char *)radixGetData(insert_key));
    if (insert_dif_len)
    {
      radix_node **insert_pos = radixFirstChild(insert_node);
      memcpy(insert_pos, &insert_key, sizeof(radix_node *)); //放入父节点insert_node
      debugf("insert_node->key_ptr: %p", *radixFirstChild(insert_node));
    }

    //创建middleware节点
    radix_node *midwar;
    if (j != 0)
    {
      memcpy(new_cur->data, cur->data, j);
      midwar = radixNewNode(2, false, false);
      if (!midwar)
      {
        debugf("%s", strerror(errno));
        return -1;
      }
      midwar->data[0] = cur->data[j];
      midwar->data[1] = key[i];
      /**
       * 针对 omane->[]  -->   oman ->  e -> []
       * 插入romanus                    u -> s -> []     
       */
      if (suffix_len)
      {
        memcpy(radixNthChild(midwar, 0), &new_cur_suffix, sizeof(void *));
      }
      else
      {
        memcpy(radixNthChild(midwar, 0), p2next, sizeof(void *));
      }
      /**
       * 针对bens插入   -->   be -> n -> s -> [] 
       *  ruber                    r -> []
       **/
      if (!insert_dif_len)
      {
        memcpy(radixNthChild(midwar, 1), &insert_key, sizeof(void *));
      }
      else
        memcpy(radixNthChild(midwar, 1), &insert_node, sizeof(void *));
      memcpy(radixFirstChild(new_cur), &midwar, sizeof(void *));
    }
    else
    {
      new_cur->data[0] = cur->data[j];
      new_cur->data[1] = key[i];
      memcpy(radixNthChild(new_cur, 0), &new_cur_suffix, sizeof(void *));
      memcpy(radixNthChild(new_cur, 1), &insert_node, sizeof(void *));
    }
    free(cur);
    return 1;
  }
  /**
   * 1.
   *   annibbb
   *      |
   *     \/
   *  anni -> [bbb]
   * 2. abc  ->  abc -> []
   **/
  else if (i == len)
  {
    if (j == cur->size || !cur->is_compressed)
    { //情况2
      if (cur->is_compressed)
      {
        radix_node *keyOne = *radixFirstChild(cur);
        memcpy(radixGetVal(keyOne), &val, sizeof(void *));
      }
      else
      {
        debugf("cur->data: %.*s", cur->size, (char *)cur->data);
        //可能需要申请val的内存
        if (!cur->is_key)
        {
          radix_node *newOne = radixRealloc2(cur);
          if (!newOne)
          {
            debugf("error in radixRealloc2");
            return -1;
          }
          memcpy(plink, &cur, sizeof(void *));
        }
        memcpy(radixGetVal(cur), &val, sizeof(void *));
      }
    }
    else
    {                                                  //情况1
      radix_node *pre_one = radixNewNode(j, false, j); //创建前继
      if (!pre_one)
      {
        debugf("error in radixNewNode");
        return -1;
      }
      memcpy(pre_one->data, cur->data, j);
      debugf("preone->data: %.*s", pre_one->size, pre_one->data);
      //创建后继
      int restLetters = cur->size - j;
      radix_node *post_one = radixNewNode(restLetters, true, restLetters);
      if (!post_one)
      {
        debugf("error in radixNewNode");
        return -1;
      }
      memcpy(post_one->data, cur->data + j, restLetters);                      // 复制剩下的data
      memcpy(radixFirstChild(pre_one), &post_one, sizeof(void *));             //pre post链接
      memcpy(radixGetVal(post_one), &val, sizeof(void *));                     //post加值
      memcpy(radixFirstChild(post_one), radixFirstChild(cur), sizeof(void *)); //post接上cur的后继
      debugf("val: %s", (char *)radixGetData(post_one));
      memcpy(plink, &pre_one, sizeof(void *));
      free(cur);
    }
    return 1;
  }
  else
  {
    if (!cur->size)
    { //避免形成链式
      bool pIsKey = cur->is_key ? true : false;
      /**
       *  null -> any string
       **/
      if (!pIsKey)
      {
        radix_node *node = radixNewNode(len, pIsKey, true);
        if (!node)
        {
          debugf("insert error: %s", strerror(errno));
          return 0;
        }
        memcpy(&node->data, key, len);
        debugf("node->data: %.*s", len, (char *)&node->data);
        memcpy(plink, &node, sizeof(void *));
        radix_node *end = radixNewNode(0, true, false);
        if (!end)
        {
          debugf("insert error: %s", strerror(errno));
          return 0;
        }
        radixSetData(end, val);
        debugf("end->val: %s", (char *)radixGetData(end));
        radix_node **firstChild = radixFirstChild(node);
        memcpy(firstChild, &end, sizeof(radix_node *));
        debugf("first_child: 0x%p", *firstChild);
        return 1;
      }
      /**
       * abc -> []
       *    |
       *   \/
       * abc -> [asd] -> []
       **/
      else
      {
        int dif = len - i;
        radix_node *new_cur = radixRealloc(cur, dif, 1);
        if (new_cur == NULL)
        {
          debugf("error in radixRealloc");
          return -1;
        }
        memcpy(plink, &new_cur, sizeof(void *));
        debugf("plink->child: %p", *plink);
        memcpy(new_cur->data, key + i, dif);
        debugf("data: %.*s", dif, new_cur->data);
        //构造新的key节点
        radix_node *new_key = radixNewNode(0, true, false);
        memcpy(radixGetVal(new_key), &val, sizeof(void *));
        debugf("val: %p", *radixGetVal(new_key));
        memcpy(radixFirstChild(new_cur), &new_key, sizeof(void *));
        debugf("first_child: %p", *radixFirstChild(new_cur));
        return 1;
      }
    }
    /**
     *  asd-> a -> bac -> kkk -> []
     *        |
     *        b -> asdsd -> []
     **/
    else
    {
      showChildPtr(cur);
      radix_node *nnew_cur = radixAddChild(cur, key, len, val, i);
      // debugf("newOne->data: %.*s", nnew_cur->size, nnew_cur->data);
      memcpy(plink, &nnew_cur, sizeof(radix_node *));
      showChildPtr(nnew_cur);
      return 1;
    }
  }

  return 0;
}

/**
 *  遍历到不匹配的地方
 *  @param：tree
 *  @param：key 键
 *  @param：cur 当前遍历节点
 *  @param：plink 指向cur的指针
 *  @param：j 记录在cur中的索引
 *  @return: i key的索引
 **/
int linkLowWalk(radix_tree *tree, const char *key, int len,
                radix_node **cur, radix_node ***plink, int *j)
{
  int i = 0, lj = 0;
  radix_node *lcur = tree->root,
             **lplink = &tree->root;
  while (lcur->size && i < len)
  {
    debugf("lcur->data: %.*s", lcur->size, lcur->data);
    if (lcur->is_compressed)
    {
      for (; lj < lcur->size && i < len; lj++)
      {
        if (lcur->data[lj] == key[i])
        {
          i++;
        }
        else
          break;
      }
      if (i == len || lcur->size != lj)
        break;
    }
    else
    {
      for (; lj < lcur->size && i < len; ++lj)
      {
        if (lcur->data[lj] == key[i])
        {
          ++i;
          break;
        }
      }
      if (lj == lcur->size)
        break;
    }
    radix_node **node = lcur->is_compressed ? radixFirstChild(lcur) : radixNthChild(lcur, lj);
    lplink = node;
    memcpy(&lcur, node, sizeof(void *));
    lj = 0;
  }
  *cur = lcur;
  *plink = lplink;
  if (lcur->is_compressed)
    *j = lj;
  return i;
}

/**
 * 设置值
 **/
void radixSetData(radix_node *node, void *val)
{
  if (!node->is_key)
    return;
  void **valPtr = radixGetVal(node);
  debugf("%ld:%p", radixLength(node), *valPtr);
  memcpy(valPtr, &val, sizeof(void *));
}

/**
 * 获取值
 **/
void *radixGetData(radix_node *node)
{
  if (!node || !node->is_key)
    return NULL;
  void **data = radixGetVal(node);
  void *res;
  memcpy(&res, data, sizeof(void *));
  // res = *data;
  return res;
}

/**
 * 在非压缩节点中添加子节点
 * @param: i 字符不同点
 **/
radix_node *radixAddChild(radix_node *node,const char *key, int len, void *val, int i)
{
  int size = len - i;
  //创建要添加的节点
  radix_node *new_cur = radixRealloc(node, 1, 0);
  if (new_cur == NULL)
  {
    debugf("error in radixRealloc");
    return new_cur;
  }
  // debugf("newOne->chlid: %p:%p:%p:%ld", radixNthChild(new_cur, (new_cur->size - 1))
  //   , new_cur, &new_cur->data, padding(new_cur->size));
  new_cur->data[new_cur->size - 1] = key[i];
  debugf("newOne->data: %.*s", new_cur->size, new_cur->data);
  //创建后继节点
  radix_node *suffixOne;
  if (size > 1)
  {
    suffixOne = radixNewNode(size - 1, false, size - 1 > 1);
    if (!suffixOne)
    {
      debugf("insert error: %s", strerror(errno));
      return NULL;
    }
    memcpy(suffixOne->data, key + 1, size - 1);
    debugf("suffixOne->data: %.*s", size - 1, suffixOne->data);
    memcpy(radixNthChild(new_cur, (new_cur->size - 1)), &suffixOne, sizeof(void *));
  }
  //key值node
  radix_node *keyOne = radixNewNode(0, true, false);
  if (!keyOne)
  {
    debugf("insert error: %s", strerror(errno));
    return NULL;
  }
  memcpy(radixGetVal(keyOne), &val, sizeof(void *));
  debugf("key->val: %s", (char *)radixGetData(keyOne));
  if (size > 1)
  { //是否存在suffixOne这个后继节点
    memcpy(radixFirstChild(suffixOne), &keyOne, sizeof(void *));
    debugf("suffixOne->child: %p", *radixFirstChild(suffixOne));
  }
  else
  {
    memcpy(radixNthChild(new_cur, (new_cur->size - 1)), &keyOne, sizeof(void *));
    debugf("newOne->child: %p", *radixNthChild(new_cur, (new_cur->size - 1)));
  }

  return new_cur;
}

/**
 * 修改一个节点的大小
 * @param: num 添加的节点数量
 **/
radix_node *radixRealloc(radix_node *node, int num, int compr)
{
  // debugf("node->data: %p", node->data);
  int old_size = radixLength(node);                             //初始大小
  int old_firch = (char *)radixFirstChild(node) - (char *)node; //第一个孩子的位置
  int old_val = -1;                                             //键值的位置
  if (node->is_key)
    old_val = (char *)radixGetVal(node) - (char *)node;
  node->is_compressed = compr;
  node->size += num;
  int new_size = sizeof(radix_node) + padding(node->size) + node->size + (compr ? sizeof(radix_node *) : sizeof(radix_node *) * (node->size)) + (node->is_key ? sizeof(void *) : 0);
  radix_node *t = (radix_node *)realloc(node, new_size);
  if (!t)
  {
    debugf("realloc error: %s", strerror(errno));
    return node;
  }
  memmove(radixFirstChild(t), (char *)t + old_firch, (old_size - old_firch));
  if (t->is_key)
  {
    memmove(radixGetVal(t), (char *)t + old_val, sizeof(void *));
    debugf("old_key: %s", (char *)radixGetData(t));
  }
  // showChildPtr(t);

  return t;
}

/**
 * 添加一个val的位置
 **/
radix_node *radixRealloc2(radix_node *node)
{
  if (!node)
    return NULL;
  if (node->is_key)
    return node;
  int new_size = radixLength(node) + sizeof(void *);
  node->is_key = true;
  radix_node *newNode = (radix_node *)realloc(node, new_size);
  return newNode;
}

void showChildPtr(radix_node *nnew_cur)
{
  for (int i = 0; i < nnew_cur->size; i++)
  {
    debugf("nnew_cur->child%d_ptr: %p", i, *radixNthChild(nnew_cur, i));
  }
}

/**
 *  debug
 *  @param: had 处理key值的函数
 **/
// void traversalDebug(radix_tree *tree, handle had)
// {
// #define RAX_LEN 100
//   fprintf(stderr, "**debug radix_tree**\n");
//   if (!tree)
//     return;
//   simpleQueue *q = simpleQueueFactory(RAX_LEN);
//   radix_node *tar = NULL;
//   push(q, tree->root);
//   int num = 1, ch_num = 0, count = 1;
//   fprintf(stderr, "level_%d:\n", count);
//   while (size(q))
//   {
//     tar = (radix_node *)front(q);
//     pop(q);
//     --num;
//     fprintf(stderr, "[ addr: %p|", tar);
//     fprintf(stderr, "is_key: %s|", tar->is_key ? "1" : "0");
//     fprintf(stderr, "is_cpd: %s|", tar->is_compressed ? "1" : "0");
//     if (tar->is_compressed)
//     {
//       fprintf(stderr, "data: %.*s|ptr:%p|", tar->size, tar->data, *radixFirstChild(tar));
//       push(q, *radixFirstChild(tar));
//       ++ch_num;
//     }
//     else
//     {
//       fprintf(stderr, "data:");
//       for (int i = 0; i < tar->size; i++)
//       {
//         fprintf(stderr, "%c|", *(tar->data + i));
//         push(q, *radixNthChild(tar, i));
//         ++ch_num;
//       }
//       if (!tar->size)
//         fprintf(stderr, " null|");
//       fprintf(stderr, "ptr: ");
//       for (int i = 0; i < tar->size; i++)
//       {
//         fprintf(stderr, "%p|", *radixNthChild(tar, i));
//       }
//     }
//     if (tar->is_key && had)
//     {
//       had(radixGetData(tar));
//     }
//     fprintf(stderr, "]\n");
//     if (!num && ch_num)
//     {
//       fprintf(stderr, "\nlevel_%d:\n", ++count);
//       num = ch_num;
//       ch_num = 0;
//     }
//   }
//   fprintf(stderr, "\n**End Debug**\n");
//   free(q);
// }

void* route(radix_tree* tree, const char* path) {
  if(!tree) 
    return NULL;
  radix_node* root = tree->root;
  int pathLen = strlen(path);
  int i = 0, 
    j = 0;
  while(i != pathLen && root) {
    if(root->is_compressed) {
      int cmpRes = strncasecmp(path + i, root->data, root->size);
      if(cmpRes) {
        return NULL;
      }
      i += root->size; 
    } else {
      for(j = 0; j < root->size; ++j) {
        if(*(path + i) == root->data[j]) {
          break;
        }
      }
      if(j == root->size) 
        return NULL;
      i += 1;
    }
    root = (root->is_compressed) ? *radixFirstChild(root) : *radixNthChild(root, j);
  }
  if(i != pathLen)
    return NULL;
  return (root && root->is_key) ? radixGetData(root) : NULL;
}
