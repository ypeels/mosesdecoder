#pragma once
#include <map>
#include <fstream>
#include <boost/foreach.hpp>
#include "moses/InputFileStream.h"
#include "util/exception.hh"

namespace Moses
{
  template<typename V>
  class Trie
  {
  public:
    
    // PARAMS ///////////////////////////////
    class Parameters
    {
    public:
      std::string m_inPath;
      std::string m_outPath;
      V m_prefixV;
      V m_fullV;
        
      Parameters(const std::string &inPath, const std::string &outPath,
        const V &prefixV, const V &fullV)
        :m_inPath(inPath)
        ,m_outPath(outPath)
        ,m_prefixV(prefixV)
        ,m_fullV(fullV)
        {}
      
    };
    
    // NODE ///////////////////////////////
    class Node
    {
      typedef std::map<char, Node> Children;
      Children m_children;

      void Save(const std::string &line, 
              std::ostream &outStrme, 
              size_t pos,
              const Parameters &params);
      void WriteToDisk(std::ostream &outStrme);
    public:
      V m_value;
      Node *m_prevChild;
      uint64_t m_filePos;
      bool m_saved;
      
      Node()
      :m_prevChild(NULL)
      ,m_saved(false)
      {}
      
      void Save(const Parameters &params);
      
    }; // class Node
    
    // TRIE ///////////////////////
    Trie(const std::string &inPath, const std::string &outPath,
        const V &prefixV, const V &fullV) 
    :m_params(inPath, outPath, prefixV, fullV)    
    { }
    
    void Save()
    {
      m_root.Save(m_params);
    }
  protected:
    Parameters m_params;
    Node m_root;
  }; // class Trie
} // namespace Moses

//#include "Trie.inc"
namespace Moses
{
  template <typename V>
  void Trie<V>::Node::Save(const Parameters &params)
  {
      InputFileStream inStrme(params.m_inPath);
    
      std::ofstream outStrme;
      outStrme.open(params.m_outPath, std::ios::in | std::ios::binary);
      UTIL_THROW_IF(!outStrme.is_open(),
                    util::FileOpenException,
                    std::string("Couldn't open file ") + params.m_outPath);

      std::string line;
      while (getline(inStrme, line)) {
        
        if (line.empty()) {
          continue;
        }
        Save(line, outStrme, 0, params);
      }
  }

  template <typename V>
  void Trie<V>::Node::Save(const std::string &line, 
                    std::ostream &outStrme, size_t pos,
                    const Parameters &params)
  {
    if (pos >= line.size()) {
      m_value = params.m_fullV;
    }
    else {
      char c = line[pos];
      Node &child = m_children[c];

      if (m_prevChild && m_prevChild != &child) {
        // new child
        m_prevChild->WriteToDisk(outStrme);

        child.m_value = params.m_prefixV;
      }
      
      child.Save(line, outStrme, pos + 1, params);
    }
  }
  
  template <typename V>
  void Trie<V>::Node::WriteToDisk(std::ostream &outStrme)
  {
    // recursively write children 1st    
    BOOST_FOREACH(typename Children::value_type mapPair, m_children) {
      Node &node = mapPair.second;
      UTIL_THROW_IF2(node.m_saved, "Child already saved");

      node.WriteToDisk(outStrme);
    }
    
    // THIS NODE 
    // value
    m_filePos = outStrme.tellp();
    outStrme.write((char*)&m_value, sizeof(m_value));
    std::cerr << m_value << "=" << m_filePos << std::endl;
    
    // pointers to children
    uint64_t var = m_children.size();
    outStrme.write((char*)&var, sizeof(uint64_t));
    
    BOOST_FOREACH(typename Children::value_type mapPair, m_children) {
      const char &key = mapPair.first;
      Node &node = mapPair.second;
      UTIL_THROW_IF2(!node.m_saved, "Child not saved");
      
      outStrme.write(&key, sizeof(char));
      outStrme.write((char*) &node.m_filePos, sizeof(node.m_filePos));
    }
    
    m_saved = true;
  }

}