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
    Node &child = m_children[c];
    child.Insert(tok, pos + 1);
  }
}
