#pragma once
#include <unordered_map>
#include <string>

class Node
{
public:
  Node();
  
  void Insert(const std::string &tok);
  
  bool isAWord;
protected:
  std::unordered_map<char, Node> m_children;

  void Insert(const std::string &tok, size_t pos);
  
};

