#ifndef CHROMOSOMENODE_H
#define CHROMOSOMENODE_H

#include <fstream>
#include <string>
#include <vector>

class ChromosomeNode;
struct ChromSubNode;


struct ChromInfo {
  ChromInfo();
  ChromInfo(ChromSubNode* csnode);

  unsigned int chromId;
  std::string chromName;
  unsigned int chromSize;
};

struct ChromSubNode {
  ChromSubNode(unsigned int keySize);
  ~ChromSubNode();
  void print_summary(unsigned int counter);
  void leafNodes(std::vector<ChromInfo>& leaves);
  char* key;
  unsigned int chromId;
  unsigned int chromSize;
  unsigned long childOffset;
  ChromosomeNode* parentNode;  // used in the second constructor
  ChromosomeNode* daughterNode;
};



class ChromosomeNode
{
 public:
  ChromosomeNode(std::ifstream& in, size_t offset, unsigned int keySize, bool reverse); // for the root
  ChromosomeNode(std::ifstream& in, ChromSubNode* parent); // for children
  ~ChromosomeNode();

  void print_summary(unsigned int counter);
  void leafNodes(std::vector<ChromInfo>& leaves);

 private:
  void init(std::ifstream& in);  // does almost everything.
  void readSubNodes(std::ifstream& in);
  void traverseSubNodes(std::ifstream& in);

  ChromSubNode* parent_sub_node; // useful since this holds the offset
  ChromosomeNode* parent_node;
  
  size_t f_offset;
  unsigned int key_size;
  bool reverseBytes;
  bool isLeaf;
  ChromSubNode** subNodes;
  unsigned short count;
};

#endif
