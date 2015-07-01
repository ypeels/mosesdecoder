#pragma once
#include <string>
#include <boost/iostreams/device/mapped_file.hpp>
#include "util/exception.hh"

namespace Moses
{
    // QUERY ///////////////////////////////
  template<typename V>
  class NodeSearch
  {
  public:
    const char *m_ptr;
    NodeSearch *m_child;

    void SetPtr(const char *data, uint64_t filePos)
    {
      m_ptr = data + filePos;
    }
    
    NodeSearch()
    :m_child(NULL)
    {}
    
    NodeSearch(const char *data, uint64_t filePos)
    :m_child(NULL)
    {
      SetPtr(data, filePos);
    }
    
    ~NodeSearch()
    {
      Clear();
    }
    
    void Clear()
    {
      delete m_child;
      m_child = NULL;
    }
    
    bool Find(V &value, const std::string &str, size_t pos, const char *data)
    {
      if (pos >= str.size()) {
        value = *((const V *)m_ptr);
        return true;
      }
      else {
        // find child
        const char *numChildrenPtr = m_ptr + sizeof(V);
        uint64_t numChildren = *((const uint64_t *)numChildrenPtr);
        
        const char *childrenPtr = numChildrenPtr + sizeof(uint64_t);
        
        char keySought = str[pos];
        //std::cerr << "searching for " << keySought << " at " << (size_t) m_ptr << std::endl;
        
        m_child = Search(numChildren, data, childrenPtr, keySought);
        if (m_child) {
          return m_child->Find(value, str, pos + 1, data);
        }
        else {
          return false;
        }
      }
    }
      
    NodeSearch *Search(uint64_t numChildren, const char *data, const char *childrenPtr, const char &keySought)
    {
      NodeSearch *ret = new NodeSearch();

      int l = 0;
      int r = numChildren - 1;
      int x;

      bool found = false;
      while (r >= l) {
        x = (l + r) / 2;

        char keyFound;
        GetChild(keyFound, *ret, data, childrenPtr, x);
        
        if (keySought == keyFound) {
          //std::cerr << "found " << keyFound << " at " << (size_t) ret->m_ptr << std::endl;
          found = true;
          break;
        }
        if (keySought < keyFound)
          r = x - 1;
        else
          l = x + 1;
      }

      if (!found) {
        delete ret;
        ret = NULL;
      }
        
      return ret;

    }
    
    void GetChild(char &keyFound, NodeSearch &nodeFound, const char *data, const char *childrenPtr, int x)
    {
      static const size_t nodeSize = sizeof(V) + sizeof(uint64_t);
      
      const char *childPtr = childrenPtr + x * nodeSize;
      keyFound = childPtr[0];
      
      const char *offsetPtr = childPtr + 1;
      uint64_t filePos = *((const uint64_t*)offsetPtr);
      nodeFound.SetPtr(data, filePos);
      
    }

    static std::pair<NodeSearch<bool>*, const char *> Create(const std::string &inPath)
    {    	
    	boost::iostreams::mapped_file_source *file = new boost::iostreams::mapped_file_source();
			file->open(inPath.c_str());
			UTIL_THROW_IF(!file->is_open(),
											 util::FileOpenException,
											 std::string("Couldn't open file ") + inPath);

			size_t size = file->size();
			std::cerr << "size=" << size << std::endl;
	
			const char *data = file->data();
	
			uint64_t rootPos = size - sizeof(uint64_t);
			std::cerr << "BEFORE rootPos=" << rootPos << std::endl;
	
			const uint64_t *ptr = (const uint64_t*) (data + rootPos);
			rootPos = ptr[0];
			NodeSearch<bool> *rootNode = new NodeSearch<bool>(data, rootPos);
	
			std::cerr << "AFTER rootPos=" << rootPos << std::endl;

    	std::pair<NodeSearch<bool>*, const char *> ret(rootNode, data);
			return ret;
    }
    
  };


} // namespce