#ifndef FUNCTIONCALLCOUNTER_H
#define FUNCTIONCALLCOUNTER_H

#include <iostream>

template<typename R, typename ... Args>
R wrapperFunction(Args...);

template<typename R, typename ... Args>
class FunctionCallCounter;

template<typename R, typename ... Args>
FunctionCallCounter<R, Args ...>* callCounterInstance(R (*function)(Args...));

template<typename R, typename ... Args>
class FunctionCallCounter {
        typedef R(*WatchedFunction)(Args...);
        typedef void(*CounterCallback)(Args...);

        friend R wrapperFunction<R, Args...>(Args...);
        friend FunctionCallCounter<R, Args ...>* callCounterInstance<R,Args...>(R (*function)(Args...));

    private:
        WatchedFunction m_function;
        WatchedFunction m_wrapper;
        CounterCallback m_callback;
        unsigned int m_callCount;

    public:
        FunctionCallCounter() : m_function(nullptr),
        m_wrapper(nullptr), m_callback(nullptr), m_callCount(0) {}

        static FunctionCallCounter<R, Args...>* instance();

    public:
        unsigned int callCount() const { return m_callCount; }
        void resetCounter() { m_callCount = 0; }
        WatchedFunction wrapper() const { return m_wrapper; }
        WatchedFunction function() const { return m_function; }

        void setFunction(R (*function)(Args ...));
        void setWrapper(R (*function)(Args ...));
        void setCallback(void (*callback)(Args ...));

        R callOrigin(Args ... args);
};

#include "functioncallcounter.tpp"

#endif // FUNCTIONCALLCOUNTER_H
