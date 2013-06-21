#ifndef R_TREE_H
#define R_TREE_H

#include <fstream>
#include <vector>

#define R_TREE_MAGIC 0x2468ACE0

class R_TreeNode;
class R_TreeSubNode;

class R_Tree {
 public:
  R_Tree(std::ifstream& in);
  ~R_Tree();
  
  R_TreeSubNode* getLeafNode(unsigned int chrome, unsigned int base);
  void getLeafNodes(unsigned int chromBeg, unsigned int start,
		    unsigned int chromEnd, unsigned int end,
		    std::vector<R_TreeSubNode*>& nodes);
  void print_summary();
  int error();

 private:
  unsigned int header_magic;
  unsigned int blockSize;
  unsigned long itemCount;
  unsigned int startChromIx;
  unsigned int startBase;
  unsigned int endChromIx;
  unsigned int endBase;
  unsigned long endFileOffset;
  unsigned int itemsPerSlot;
  unsigned int reserved;

  int error_code;
  size_t f_offset;
  bool reverse_bytes;
  R_TreeNode* tree;
};

#endif
