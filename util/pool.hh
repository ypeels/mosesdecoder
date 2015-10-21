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
		if (m_currInd >= m_incrNum) {
			// time to go to the next list
			++m_listInd;
			if (m_listInd >= free_list_.size()) {
				More();
			}
			m_currInd = 0;
		}
		else {
			++m_currInd;
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
		for (iter = free_list_.begin(); iter != iterLast; ++iter) {
			T *list = (T*) *iter;
			for (size_t i = 0; i < m_incrNum; ++i) {
				T *obj = list + i;

			}
		}

		// last list
		T *list = (T*) free_list_.back();
		for (size_t i = 0; i < m_currInd; ++i) {
			T *obj = list + i;

		}

		// reset variables
		m_currInd = 0;
		m_listInd = 0;
	}
};

} // namespace util

#endif // UTIL_POOL_H
