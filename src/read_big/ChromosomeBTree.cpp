#include "ChromosomeBTree.h"
#include <iostream>
#include "../util/ByteReader.h"
#include "ChromosomeNode.h"

const unsigned int chromeBTreeHSize = CHROM_B_TREE_H_SIZE;
const unsigned int chromeBTreeMagic = CHROM_B_TREE_MAGIC;

ChromosomeBTree::ChromosomeBTree(std::ifstream& in)
{
  error_code = 0;
  cbt_headerOffset = in.tellg();
  char* bytes = new char[chromeBTreeHSize];
  in.read(bytes, chromeBTreeHSize);
  if(!in.good()){
    delete []bytes;
    error_code = 1;
    std::cerr << "Unable to read chromosomeBTree from stream" << std::endl;
    return;
  }
  
  magic = *(unsigned int*)bytes;
  reverseBytes = (magic != chromeBTreeMagic);
  if(reverseBytes) // than reverse it and confirm the right value
    magic = __builtin_bswap32(magic);
  if(magic != chromeBTreeMagic){ // then some error give up..
    delete []bytes;
    error_code = 2;
    std::cerr << "ChromosomeBTree unable to match magic signature\n"
	      << "\twas looking for: " << chromeBTreeMagic << "  but got: " << magic << std::endl;
    return;
  }
  ByteReader br(bytes, chromeBTreeHSize, reverseBytes);
  blockSize = br.ui(4);
  keySize = br.ui(8);
  valSize = br.ui(12);
  itemCount = br.ul(16);
  reserved = br.ul(24);
  
  size_t current_offset = in.tellg();
  chromosomeTree = new ChromosomeNode(in, current_offset, keySize, reverseBytes);

}

ChromosomeBTree::~ChromosomeBTree()
{
  delete chromosomeTree;
}

void ChromosomeBTree::print_summary()
{
  std::cout << "magic       :\t" << magic << "\n"
	    << "reverse     :\t" << reverseBytes << "\n"
	    << "block size: :\t" << blockSize << "\n"
	    << "keySize     :\t" << keySize << "\n"
	    << "valSize     :\t" << valSize << "\n"
	    << "itemCount   :\t" << itemCount << "\n"
	    << "reserved    :\t" << reserved << "\n" << std::endl;
  chromosomeTree->print_summary(0);
}

int ChromosomeBTree::error()
{
  return(error_code);
}

void ChromosomeBTree::leafNodes(std::vector<ChromInfo>& leaves)
{
  if(chromosomeTree)
    chromosomeTree->leafNodes(leaves);
}
  
