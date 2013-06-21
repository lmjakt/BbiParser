#include "R_Tree.h"
#include "R_TreeNode.h"
#include "../util/ByteReader.h"
#include <iostream>

R_Tree::R_Tree(std::ifstream& in)
{
  error_code = 0;
  tree = 0;
  reverse_bytes = false;
  f_offset = in.tellg();
  
  const unsigned int header_size = 48;
  char* header_bytes = new char[header_size];

  in.read(header_bytes, header_size);
  if(!in.good()){
    error_code = 1;
    std::cerr << "R_Tree Unable to read in index from file " << std::endl;
    delete []header_bytes;
    return;
  }
  
  header_magic = *(unsigned int*)header_bytes;
  if(header_magic != R_TREE_MAGIC){
    reverse_bytes = true;
    header_magic = __builtin_bswap32(header_magic);
  }
  if(header_magic != R_TREE_MAGIC){
    std::cerr << "R_Tree Unable to find correct magic got: " << header_magic
	      << "Wanted : " << R_TREE_MAGIC << std::endl;
    error_code = 2;
    delete []header_bytes;
    return;
  }
  
  ByteReader br(header_bytes, header_size, reverse_bytes);
  blockSize = br.ui(4);
  itemCount = br.ul(8);
  startChromIx = br.ui(16);
  startBase = br.ui(20);
  endChromIx = br.ui(24);
  endBase = br.ui(28);
  endFileOffset = br.ul(32);
  itemsPerSlot = br.ui(40);
  reserved = br.ui(44);
  
  delete []header_bytes;
  tree = new R_TreeNode(in, 0, in.tellg(), reverse_bytes);
}

R_Tree::~R_Tree()
{
  delete tree;
}

R_TreeSubNode* R_Tree::getLeafNode(unsigned int chrome, unsigned int base)
{
  return( tree->getLeafNode(chrome, base));
}

void R_Tree::getLeafNodes(unsigned int chromBeg, unsigned int start,
			  unsigned int chromEnd, unsigned int end,
			  std::vector<R_TreeSubNode*>& nodes)
{
  tree->getLeafNodes(chromBeg, start, chromEnd, end, nodes);
}

void R_Tree::print_summary()
{
  std::cout << "R_Tree Summary:\n"
	    << "magic         :\t" << header_magic << "\n"
	    << "blockSize     :\t" << blockSize << "\n"
	    << "itemCount     :\t" << itemCount << "\n"
	    << "startChromIx  :\t" << startChromIx << "\n"
	    << "startBase     :\t" << startBase << "\n"
	    << "endChromIx    :\t" << endChromIx << "\n"
	    << "endBase       :\t" << endBase << "\n"
	    << "endFileOffset :\t" << endFileOffset << "\n"
	    << "itemsPerSlot  :\t" << itemsPerSlot << "\n"
	    << "reserved      :\t" << reserved << std::endl;
  tree->print_summary(0);
}

int R_Tree::error()
{
  return(error_code);
}
