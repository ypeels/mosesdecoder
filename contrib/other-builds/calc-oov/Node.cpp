#include <iostream>
#include <cassert>
#include "Node.h"

using namespace std;

Node::Node()
:isAWord(false)
,endJuncture(false)
{
  
}

Node *Node::Insert(const std::string &tok, bool endJuncture1, bool isAWord1)
{
  return Insert(tok, endJuncture1, isAWord1, 0);
}

Node *Node::Insert(const std::string &tok, bool endJuncture1, bool isAWord1, size_t pos)
{
  if (pos == tok.size()) {
    endJuncture = endJuncture1;
    isAWord = isAWord1;
    return this;
  }
  else {
    char c = tok[pos];
    
    Node *child = GetOrCreateNode(c);
    return child->Insert(tok, endJuncture1, isAWord1, pos + 1);
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


