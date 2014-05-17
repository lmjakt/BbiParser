#ifndef STRINGUTIL_H
#define STRINGUTIL_H

#include <vector>
#include <string>

std::vector<std::string> splitString(std::string& line, char delim, bool includeEmpty=false);
std::string removeChar(std::string& word, char c);
void stringToLower(std::string& word);

#endif
