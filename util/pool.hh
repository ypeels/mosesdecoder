// Very simple pool.  It can only allocate memory.  And all of the memory it
// allocates must be freed at the same time.

#ifndef UTIL_POOL_H
#define UTIL_POOL_H

#include <vector>
#include <stdint.h>

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
class ObjectPool : public Pool
{
  std::size_t m_incrNum;
  std::size_t m_currInd;
  std::size_t m_listInd;
public:
  ObjectPool(std::size_t incrNum)
	:m_incrNum(incrNum)
	,m_currInd(0)
	,m_listInd(0)
	{
	  std::size_t size = incrNum * sizeof(T);
	  Allocate(size);
	}

	void More()
	{
	  std::size_t size = m_incrNum * sizeof(T);
	  Pool::More(size);
	}

	T *Get()
	{
		++m_currInd;
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
		return ret;
	}

	void DestroyAll()
	{
		// call all destructors
		// every list, except last
		std::vector<void *>::const_iterator iterLast = free_list_.begin() + m_listInd;

		std::vector<void *>::const_iterator iter;
		for (std::size_t listInd = 0; listInd < free_list_.size(); ++listInd) {
			T *list = (T*) free_list_[listInd];
			std::size_t maxEleInd = (listInd < free_list_.size() - 1) ? m_incrNum : m_currInd;
			for (std::size_t eleInd = 0; eleInd < m_incrNum; ++eleInd) {
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
