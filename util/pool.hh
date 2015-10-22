// Very simple pool.  It can only allocate memory.  And all of the memory it
// allocates must be freed at the same time.

#ifndef UTIL_POOL_H
#define UTIL_POOL_H

#include <vector>
#include <stdint.h>
#include <stdlib.h>
#include <iostream>
#include "util/exception.hh"
#include "util/scoped.hh"

namespace util {

class Pool {
  public:
    Pool();

    ~Pool();

    void *Allocate(std::size_t size) {
      void *ret = current_;
      current_ += size;
      if (current_ < current_end_) {
        return ret;
      } else {
        return More(size);
      }
    }

    void FreeAll();

  protected:
    void *More(std::size_t size);

    std::vector<void *> free_list_;

    uint8_t *current_, *current_end_;

    // no copying
    Pool(const Pool &);
    Pool &operator=(const Pool &);
};

template<typename T>
class ObjectPool
{
  std::vector<void *> free_list_;
  std::size_t m_incrNum;
  std::size_t m_currInd;
  std::size_t m_listInd;
public:
  ObjectPool(std::size_t incrNum)
	:m_incrNum(incrNum)
	,m_currInd(0)
	,m_listInd(0)
	{
	  More();
	}

	void More()
	{
	  std::size_t size = m_incrNum * sizeof(T);
	  std::cerr << "More=" << free_list_.size() << " " << size << std::endl;
	  //void *ret = malloc(size);
          void *ret = MallocOrThrow(size);
          UTIL_THROW_IF2(ret == NULL, "Couldn't allocate mem");
	  std::cerr << "ret=" << ret << std::endl;
	  free_list_.push_back(ret);
	}

	T *Get()
	{
		if (m_currInd >= m_incrNum) {
			// time to go to the next list
			++m_listInd;
			if (m_listInd >= free_list_.size()) {
				More();
			}
			m_currInd = 0;
		}

		T* ret = (T*)free_list_[m_listInd];
		ret += m_currInd;

		++m_currInd;

		return ret;
	}

	void DestroyAll()
	{
		// call all destructors
		for (std::size_t listInd = 0; listInd <= m_listInd; ++listInd) {
			T *list = (T*) free_list_[listInd];
			std::size_t maxEleInd = (listInd == m_listInd) ? m_currInd : m_incrNum;
			for (std::size_t eleInd = 0; eleInd < maxEleInd; ++eleInd) {
				T *obj = list + eleInd;
				obj->~T();
			}
		}

		// reset variables
		m_currInd = 0;
		m_listInd = 0;
	}
};

} // namespace util

#endif // UTIL_POOL_H
