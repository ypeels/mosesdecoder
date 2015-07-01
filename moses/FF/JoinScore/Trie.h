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
      m_root.m_value = m_params.m_prefixV;
      m_root.Save(m_params);
    }
  protected:
    Parameters m_params;
    Node m_root;
  }; // class Trie
} // namespace Moses

// IMPLEMENTATION .//////////////////////////////////
//#include "Trie.inc"
namespace Moses
{
  template <typename V>
  void Trie<V>::Node::Save(const Parameters &params)
  {
      InputFileStream inStrme(params.m_inPath);
    
      std::ofstream outStrme;
      outStrme.open(params.m_outPath.c_str(), std::ios::out | std::ios::in | std::ios::binary | std::ios::ate | std::ios::trunc);
      UTIL_THROW_IF(!outStrme.is_open(),
                    util::FileOpenException,
                    std::string("Couldn't open file ") + params.m_outPath);

      std::string line;
      size_t lineNum = 0;
      while (getline(inStrme, line)) {
        lineNum++;
        if (lineNum%100000 == 0) std::cerr << lineNum << " " << std::flush;
        //std::cerr << lineNum << " " << std::flush;
        
        if (line.empty() || line.size() > 150) {
          continue;
        }
        
        Save(line, outStrme, 0, params);
      }
      
      // write root node;
      size_t numChildren = m_children.size();
      WriteToDisk(outStrme);
      std::cerr << "Saved root " 
                << m_filePos << "=" 
                << m_value << "=" 
                << numChildren << std::endl;
                
      outStrme.write((char*)&m_filePos, sizeof(m_filePos));

      outStrme.close();
      inStrme.Close();
  }

  template <typename V>
  void Trie<V>::Node::Save(const std::string &line, 
                    std::ostream &outStrme, size_t pos,
                    const Parameters &params)
  {
    //std::cerr << "line=" << line << std::endl;
    if (pos >= line.size()) {
      m_value = params.m_fullV;
    }
    else {
      char c = line[pos];
      Node &child = m_children[c];

      if (m_prevChild != &child) {
        // new child
        child.m_value = params.m_prefixV;
        
        if (m_prevChild) {
          m_prevChild->WriteToDisk(outStrme);
        }

        m_prevChild = &child;
      }
      
      //std::cerr << "Saving " << child.m_value << std::endl;
      // recursively add next char
      child.Save(line, outStrme, pos + 1, params);
    } // if (pos >= line.size()) {
  } // void Trie<V>::Node::Save(...)
  
  template <typename V>
  void Trie<V>::Node::WriteToDisk(std::ostream &outStrme)
  {
    // recursively write children 1st    
    BOOST_FOREACH(typename Children::value_type &mapPair, m_children) {
      Node &node = mapPair.second;
      if (!node.m_saved) {
        node.WriteToDisk(outStrme);
      }
    }
    
    // THIS NODE 
    // value
    m_filePos = outStrme.tellp();
    outStrme.write((char*)&m_value, sizeof(m_value));
    
    // pointers to children
    uint64_t numChildren = m_children.size();
    outStrme.write((char*)&numChildren, sizeof(uint64_t));

    //std::cerr << m_filePos << "=" << m_value << "=" << numChildren << ": " << std::flush;
    
    BOOST_FOREACH(typename Children::value_type &mapPair, m_children) {
      const char &key = mapPair.first;
      Node &node = mapPair.second;
      UTIL_THROW_IF2(!node.m_saved, "Child not saved");
      
      outStrme.write(&key, sizeof(char));
      outStrme.write((char*) &node.m_filePos, sizeof(node.m_filePos));
      
      //std::cerr << key << "=" << node.m_filePos << " " << std::flush;
    }
    
    //std::cerr << std::endl;

    m_saved = true;
    
    // done writing. Discard children to save mem
    m_children.clear();
    
  }

}
