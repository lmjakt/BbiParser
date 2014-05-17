#include "R_TreeNode.h"
#include <iostream>
#include "../util/ByteReader.h"
#include <byteswap.h>
#include <string.h>

const unsigned int max_count = 1000;

static unsigned int node_counter = 0;
static unsigned int leaf_node_counter = 0;

R_TreeSubNode::R_TreeSubNode()
{
  daughter = 0;
  parent = 0;
  dataSize = 0;
}

R_TreeSubNode::~R_TreeSubNode()
{
  delete daughter;
}

void R_TreeSubNode::print_summary(unsigned int depth)
{
  std::cout << "\n\tR_Tree Subnode depth : " << depth << "\n"
	    << "\tstartChromIx         : " << startChromIx << "\n"
	    << "\tstartBase            : " << startBase << "\n"
	    << "\tendChromIx           : " << endChromIx << "\n"
	    << "\tendBase              : " << endBase << "\n"
	    << "\tdataOffset           : " << dataOffset << "\n"
	    << "\tdataSize             : " << dataSize << "\n"
	    << "\tdaughter             : " << daughter << std::endl;
  if(daughter)
    daughter->print_summary(depth);
}

// The fact that nodes can contain data from several chromosomes
// makes it difficult to express the required inequality elegantly
bool R_TreeSubNode::contains(unsigned int chrome, unsigned int base)
{
  return(
	 ( chrome >= startChromIx && ( chrome > startChromIx || base >= startBase ))
	 &&
	 ( chrome <= endChromIx && (chrome < endChromIx || base <= endBase) )
	 );
}

bool R_TreeSubNode::overlaps(unsigned int chromBeg, unsigned int start, 
			     unsigned int chromEnd, unsigned int end)
{
  return( less_than(chromEnd, end) && greater_than(chromBeg, start) );
}

unsigned int R_TreeSubNode::node_no()
{
  return(node_counter);
}

unsigned int R_TreeSubNode::leaf_node_no()
{
  return(leaf_node_counter);
}

R_TreeNode::R_TreeNode(std::ifstream& in, R_TreeNode* parent, size_t offset, bool reverse)
{
  file_offset = offset;
  reverse_bytes = reverse;
  error_code = 0;
  subNodes = 0;
  parent_node = parent;
  parent_sub_node = 0;

  in.seekg(file_offset);
  if(!in.good()){
    std::cerr << "R_TreeNode Unable to seek to start position: " << file_offset << std::endl;
    error_code = 1;
    return;
  }
  const unsigned int node_size = 4;
  char node_bytes[node_size];
  in.read(node_bytes, node_size);
  isLeaf = (node_bytes[0] > 0);  // should be 1 or 0
  reserved = (node_bytes[1]);
  count = *(unsigned short*)(node_bytes + 2);
  if(reverse_bytes)
    count = __bswap_16(count);
  
  if(count > max_count){
    std::cerr << "R_TreeNode count is larger than max_count: " << count << " > " << max_count
	      << "\naborting here: " << std::endl;
    error_code = 2;
    return;
  }

  readSubNodes(in);
  if(!isLeaf)
    traverseSubNodes(in);
}

R_TreeNode::~R_TreeNode()
{
  for(unsigned short i=0; i < count; ++i)
    delete subNodes[i];
  delete []subNodes;
}

void R_TreeNode::print_summary(unsigned int depth)
{
  ++depth;
  std::cout << "---------------------------------"
	    << "R_TreeNode Summary" << "\n"
	    << "isLeaf            : " << isLeaf << "\n"
	    << "reserved          : " << reserved << "\n"
	    << "count             : " << count << "\n" 
	    << "Node counter      : " << node_counter << "\n"
	    << "Leaf node counter : " << leaf_node_counter << "\n"
	    << std::endl;
  for(unsigned short i=0; i < count; ++i){
    if(subNodes[i]){
      subNodes[i]->print_summary(depth);
    }else{
      std::cout << "\t\tNULL subnode at : " << i << std::endl;
    }
  }
}

int R_TreeNode::error()
{
  return(error_code);
}

bool R_TreeNode::bytes_reversed()
{
  return(reverse_bytes);
}

// This code can probably be speeded up by doing bisections on the data,
// but to begin with simply iterate looking for the correct one
// 
// but this stupid implementation gives me about half a million lookups / second
// so there probably isn't much point in trying to optimise it.
R_TreeSubNode* R_TreeNode::getLeafNode(unsigned int chrome, unsigned int base){
  for(unsigned short i=0; i < count; ++i){
    if(subNodes[i]->contains(chrome, base))
      return( subNodes[i]->daughter ? 
	      subNodes[i]->daughter->getLeafNode(chrome, base) : subNodes[i] );
  }
  return(0); // no match.
}

void R_TreeNode::getLeafNodes(unsigned int chromBeg, unsigned int start,
			      unsigned int chromEnd, unsigned int end,
			      std::vector<R_TreeSubNode*>& nodes)
{
  for(unsigned short i=0; i < count; ++i){
    if(subNodes[i]->overlaps(chromBeg, start, chromEnd, end)){
      if(subNodes[i]->daughter){
	subNodes[i]->daughter->getLeafNodes(chromBeg, start, chromEnd, end, nodes);
      }else{
	nodes.push_back(subNodes[i]);
      }
    }
  }
}

void R_TreeNode::readSubNodes(std::ifstream& in)
{
  subNodes = new R_TreeSubNode*[count];
  char read_buffer[32];
  for(unsigned short i=0; i < count; ++i){
    unsigned int sub_size = isLeaf ? 32 : 24;
    in.read(read_buffer, sub_size);
    if(!in.good()){
      std::cerr << "R_TreeNode unable to read node from file" << std::endl;
      return;
    }
    ++node_counter;
    if(isLeaf)
      ++leaf_node_counter;

    ByteReader br(read_buffer, sub_size, reverse_bytes);
    subNodes[i] = new R_TreeSubNode();
    subNodes[i]->startChromIx = br.ui(0);
    subNodes[i]->startBase = br.ui(4);
    subNodes[i]->endChromIx = br.ui(8);
    subNodes[i]->endBase = br.ui(12);
    subNodes[i]->dataOffset = br.ul(16);
    if(isLeaf)
      subNodes[i]->dataSize = br.ul(24);
    subNodes[i]->parent = this;
  }
}

void R_TreeNode::traverseSubNodes(std::ifstream& in)
{
  for(unsigned short i=0; i < count; ++i)
    subNodes[i]->daughter = new R_TreeNode(in, this, subNodes[i]->dataOffset, reverse_bytes);
}
