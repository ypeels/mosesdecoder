#include <cassert>
#include "Node.h"

using namespace std;

Node::Node()
:isAWord(false)
{
  
}

void Node::Insert(const std::string &tok)
{
  Insert(tok, 0);
}

void Node::Insert(const std::string &tok, size_t pos)
{
  if (pos == tok.size()) {
    isAWord = true;
  }
  else {
    char c = tok[pos];
    
    Node *child = GetOrCreateNode(c);
    child->Insert(tok, pos + 1);
  }
}

bool Node::Find(const std::string &tok) const
{
  bool ret = Find(tok, 0);
  return ret;
}

bool Node::Find(const std::string &tok, size_t pos) const
{
  if (pos == tok.size()) {
    return isAWord;
  }
  else {
    char c = tok[pos];

    Children::const_iterator iter;
    iter = m_children.find(c);
    if (iter == m_children.end()) {
      return false;
    }
    else {
      const Node *child = iter->second;
      return child->Find(tok, pos + 1);
    }
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
		return new Node();
	}
	else {
		Node *child = iter->second;
		assert(child);
		return child;
	}  

}


