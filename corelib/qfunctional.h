#ifndef qfunctional
#define qfunctional

#include <QThreadPool>
#include <thread>
#include <future>
#include <functional>
#include <utility>

namespace ldapcore
{

/*!
  * @brief simple generic class that wrap QRunnable class to present simple way to use QThreadPool
  * To use it, just pass lambda to constructor is enough
  *
*/
template <typename F>
struct Task : public QRunnable
{
	std::function< void() > task;

	Task(F f): task(std::bind(std::forward<F>(f))) {}

    /*!
      * @brief override function of QRunnable to start needed activity
      * just will be run task member
      *
    */
	void run() override final
	{
		task();
	}
};

/*!
  * @brief helper function to avoid to use bracet
  * this function will be deduce needed parameters to Task object
  *
*/
template <typename F>
Task<F>* makeSimpleTask(F f)
{
	return new Task<F>(f);
}


}

#endif // qfunctional 

