#pragma once
#include <unordered_map>
#include <string>

class Node
{
public:
  Node();
  
  void Insert(const std::string &tok);
  bool Find(const std::string &tok) const;
  const Node *Find(char c) const;
  
  bool isAWord;
protected:
  typedef std::unordered_map<char, Node*> Children;
  Children m_children;

  void Insert(const std::string &tok, size_t pos);
  bool Find(const std::string &tok, size_t pos) const;
  
  Node *GetOrCreateNode(char c);
};

