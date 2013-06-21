#ifndef CHROMOSOMEBTREE_H
#define CHROMOSOMEBTREE_H

#include <fstream>
#include <stdlib.h>
#include <vector>
#include "ChromosomeNode.h"

#define CHROM_B_TREE_MAGIC 0x78CA8C91
#define CHROM_B_TREE_H_SIZE 32


class ChromosomeBTree
{
 public:
  ChromosomeBTree(std::ifstream& in);
  ~ChromosomeBTree();

  void print_summary();
  int error();  // refers to only the tree, no information from the nodes
  void leafNodes(std::vector<ChromInfo>& leaves);

 private:
  // these are the basic properties of the tree
  // in addition to these we need to set up a
  // pointer based tree, but need to inspect a bit
  // to get an idea here
  unsigned int magic;
  unsigned int blockSize; // number of children per block
  unsigned int keySize;  // bytes in key
  unsigned int valSize; // size of value being indexed (8)
  unsigned long itemCount; // number of chromosomes / contigs
  unsigned long reserved; 

  // other useful values
  bool reverseBytes; // determine from the magic
  size_t cbt_headerOffset; // determine from tellg();
  int error_code;

  ChromosomeNode* chromosomeTree;
  //  void exploreTree(); // to see what the tree looks like ... 
  
};

#endif
