#pragma once
#include <unordered_map>
#include <string>

class Node
{
public:
  Node();
  
  Node *Insert(const std::string &tok, bool endJuncture, bool isAWord);
  const Node *Find(char c) const;
  
  bool isAWord;
  bool endJuncture;
protected:
  typedef std::unordered_map<char, Node*> Children;
  Children m_children;

  Node *Insert(const std::string &tok, bool endJuncture, bool isAWord, size_t pos);
  
  Node *GetOrCreateNode(char c);
};

