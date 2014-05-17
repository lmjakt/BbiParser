#ifndef R_TREENODE_H
#define R_TREENODE_H

#include <fstream>
#include <vector>

class R_TreeNode;

// a leaf node has dataOffset and dataSize defined
// numbers, but has a NULL daughter pointer.
// a non-leaf node has 0 dataSize, but a daughter
// pointer
struct R_TreeSubNode {
  R_TreeSubNode();
  ~R_TreeSubNode();
  void print_summary(unsigned int depth);
  bool contains(unsigned int chrome, unsigned int base);
  bool overlaps(unsigned int chromBeg, unsigned int start, 
		unsigned int chromEnd, unsigned int end);

  bool less_than(unsigned int chrom, unsigned int base){
    if(chrom != startChromIx) return startChromIx < chrom;
    return( startBase < base );
  }
  bool greater_than(unsigned int chrom, unsigned int base){
    if(chrom != endChromIx) return endChromIx > chrom;
    return( endBase > base );
  }
  unsigned int node_no();
  unsigned int leaf_node_no();
  unsigned int startChromIx;
  unsigned int startBase;
  unsigned int endChromIx;
  unsigned int endBase;
  unsigned long dataOffset;
  unsigned long dataSize;
  R_TreeNode* daughter;
  R_TreeNode* parent;
};

class R_TreeNode {
 public:
  R_TreeNode(std::ifstream& in, R_TreeNode* parent, size_t offset, bool reverse);
  ~R_TreeNode();

  void print_summary(unsigned int depth);
  int error();
  bool bytes_reversed();

  /// DO NOT DELETE the leaf!!
  R_TreeSubNode* getLeafNode(unsigned int chrome, unsigned int base);
  void getLeafNodes(unsigned int chromBeg, unsigned int start,
		    unsigned int chromEnd, unsigned int end, 
		    std::vector<R_TreeSubNode*>& nodes);
					   
  
 private:
  void readSubNodes(std::ifstream& in);
  void traverseSubNodes(std::ifstream& in);

  bool isLeaf;
  char reserved;
  unsigned short count;

  size_t file_offset;
  bool reverse_bytes;
  int error_code;
  R_TreeSubNode** subNodes;

  R_TreeNode* parent_node;
  R_TreeSubNode* parent_sub_node;

};

#endif
