#include "ChromosomeNode.h"
#include <iostream>
#include <string.h>
#include "../util/ByteReader.h"

const unsigned int header_size = 4;

ChromSubNode::ChromSubNode(unsigned int keySize)
{
  key = new char[keySize];
  memset((void*)key, 0, keySize);
  chromId = 0;
  chromSize = 0;
  childOffset = 0;
  parentNode = 0;
  daughterNode = 0;
}

ChromSubNode::~ChromSubNode()
{
  delete daughterNode;
  delete []key;
}

void ChromSubNode::print_summary(unsigned int counter)
{
  if(daughterNode)
    daughterNode->print_summary(counter);
  std::cout << "Chromosome node: " << counter << "\n"
	    << "\tkey      :\t" << key << "\n"
	    << "\tchromId  :\t" << chromId << "\n"
	    << "\tchromSize:\t" << chromSize << "\n"
	    << "\tdaughter :\t" << daughterNode << std::endl;
}

void ChromSubNode::leafNodes(std::vector<ChromInfo>& leaves)
{
  if(daughterNode)
    return( daughterNode->leafNodes(leaves) );
  leaves.push_back( ChromInfo(this) );
}

ChromInfo::ChromInfo()
{
  chromId = chromSize = 0;
  chromName = "NULL";
}

ChromInfo::ChromInfo(ChromSubNode* csnode)
{
  if(!csnode){
    chromId = chromSize = 0;
    chromName = "NULL";
    return;
  }
  chromId = csnode->chromId;
  chromName = std::string(csnode->key);
  chromSize = csnode->chromSize;
}

ChromosomeNode::ChromosomeNode(std::ifstream& in, size_t offset, unsigned int keySize, bool reverse)
{
  parent_sub_node = 0;
  parent_node = 0;
  f_offset = offset;
  key_size = keySize;
  reverseBytes = reverse;
  
  init(in);
}

ChromosomeNode::ChromosomeNode(std::ifstream& in, ChromSubNode* parent)
{
  parent_sub_node = parent;
  parent_node = parent->parentNode;
  f_offset = parent_sub_node->childOffset;
  key_size = parent_node->key_size;
  reverseBytes = parent_node->reverseBytes;

  init(in);
}

ChromosomeNode::~ChromosomeNode()
{
  for(unsigned int i=0; i < count; ++i)
    delete subNodes[i];
  delete []subNodes;
}

void ChromosomeNode::print_summary(unsigned int counter)
{
  counter++;
  for(unsigned int i=0; i < count; ++i)
    subNodes[i]->print_summary(counter);
}

void ChromosomeNode::leafNodes(std::vector<ChromInfo>& leaves)
{
  for(unsigned int i=0; i < count; ++i)
    subNodes[i]->leafNodes(leaves);
}

void ChromosomeNode::init(std::ifstream& in)
{
  in.seekg(f_offset);
  char* header_bytes = new char[header_size];
  in.read(header_bytes, header_size);
  if(!in.good()){
    std::cerr << "ChromosomeNode::init ifstream corrupted" << std::endl;
    delete []header_bytes;
    return;
  }
  ByteReader br(header_bytes, header_size, reverseBytes);
  isLeaf = (*header_bytes != 0);
  count = br.us(2); // *(unsigned short*)(header_bytes + 2);
  subNodes = new ChromSubNode*[count];
  delete []header_bytes;
  readSubNodes(in);
  if(!isLeaf)
    traverseSubNodes(in); // recurses  
}

void ChromosomeNode::readSubNodes(std::ifstream& in)
{
  // both leaf nodes and non-leaf nodes require keySize+ 8 bytes of
  // buffer to read in the data
  size_t buffer_size = 8 + key_size;
  char* read_buffer = new char[buffer_size];
  ByteReader br(read_buffer, buffer_size, reverseBytes);
  for(unsigned int i=0; i < count; ++i){
    //    std::cout << "\treadSubNodes " << i << "\tpos " << in.tellg() << std::endl; 
    in.read(read_buffer, buffer_size);
    if(!in.good()){
      std::cerr << "Unable to read data for Chrom Sub node" << std::endl;
      subNodes[i] = 0;
      continue;
    }
    subNodes[i] = new ChromSubNode(key_size);
    memcpy((void*)subNodes[i]->key, (void*)read_buffer, key_size);
    subNodes[i]->parentNode = this;
    if(!isLeaf){
      subNodes[i]->childOffset = br.ul(key_size);
    }else{
      subNodes[i]->chromId = br.ui(key_size);
      subNodes[i]->chromSize = br.ui(key_size + 4);
    }
  }
  delete []read_buffer;
}

// This is recursive, because this is called from init, which
// is called from the constructor. Which this function then
// calls itself. 
// 
void ChromosomeNode::traverseSubNodes(std::ifstream& in)
{
  for(unsigned int i=0; i < count; ++i){
    if(!subNodes[i])
      continue;
    subNodes[i]->daughterNode = new ChromosomeNode(in, subNodes[i]);
  }
}

