#include "StringUtil.h"
#include <algorithm>


std::vector<std::string> splitString(std::string& line, char delim, bool includeEmpty)
{
  std::vector<std::string> words;
  size_t b = 0;
  while(line[b] == delim)
    ++b;
  size_t e = b;
  while(e < line.size()){
    if(line[e] == delim){
      if(e > b || includeEmpty)
	words.push_back( line.substr(b, e-b) );
      b = e+1;
    }
    ++e;
  }
  if(e > b)
    words.push_back( line.substr(b, e-b) );
  return(words);
}

std::string removeChar(std::string& word, char c)
{
  std::string res;
  res.reserve(word.size());
  for(unsigned int i=0; i < word.size(); ++i){
    if(word[i] != c)
      res.append(1, c);
  }
  return(res);
}

void stringToLower(std::string& word)
{
  std::transform(word.begin(), word.end(), word.begin(), ::tolower);
}
