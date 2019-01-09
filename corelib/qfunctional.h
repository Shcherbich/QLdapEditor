#ifndef qfunctional 
#define qfunctional 

#include <QThreadPool>
#include <thread>
#include <future>
#include <functional>
#include <utility>


namespace ldapcore
{

template <typename F>
struct Task : public QRunnable
{
    std::function< void() > task;

    Task(F f):task(std::bind(std::forward<F>(f))){}

    void run() override final
    {
        task();
    }
};

template <typename F>
Task<F>* makeSimpleTask(F f)
{
    return new Task<F>(f);
}

}

#endif // qfunctional 

