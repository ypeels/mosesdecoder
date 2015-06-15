#include <iostream>
#include <cassert>
#include "Node.h"

using namespace std;

Node::Node()
:isAWord(false)
,endJuncture(false)
{
  
}

Node *Node::Insert(const std::string &tok, bool endJuncture, bool isAWord)
{
  return Insert(tok, endJuncture, isAWord, 0);
}

Node *Node::Insert(const std::string &tok, bool endJuncture, bool isAWord, size_t pos)
{
  if (pos == tok.size()) {
    this->endJuncture = endJuncture;
    this->isAWord = isAWord;
    return this;
  }
  else {
    char c = tok[pos];
    
    Node *child = GetOrCreateNode(c);
    return child->Insert(tok, endJuncture, isAWord, pos + 1);
  }
}

const Node *Node::Find(char c) const
{
    Children::const_iterator iter;
    iter = m_children.find(c);
    if (iter == m_children.end()) {
      return NULL;
    }
    else {
      const Node *child = iter->second;
      assert(child);
      return child;
    }  
}

Node *Node::GetOrCreateNode(char c)
{
	Children::iterator iter;
	iter = m_children.find(c);
	if (iter == m_children.end()) {
		Node *node = new Node();
    m_children[c] = node;
    return node;
	}
	else {
		Node *child = iter->second;
		assert(child);
		return child;
	}  

}


