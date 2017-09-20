#include <assert.h>

template<typename R, typename ... Args>
FunctionCallCounter<R, Args...>* FunctionCallCounter<R, Args ...>::instance() {
    static FunctionCallCounter<R, Args...> instance;
    return &instance;
}

template<typename R, typename ... Args>
FunctionCallCounter<R, Args ...>* callCounterInstance(
        R (*function)(Args...)) {

    FunctionCallCounter<R, Args ...>* instance = FunctionCallCounter<R, Args...>::instance();
    instance->setFunction(function);
    instance->setWrapper(&wrapperFunction<R, Args ...>);

    return instance;
}

template<typename R, typename ...Args>
void FunctionCallCounter<R, Args ...>::setFunction(R (*function)(Args ...))
{
    m_function = function;
}

template<typename R, typename ...Args>
void FunctionCallCounter<R, Args ...>::setWrapper(R (*function)(Args ...))
{
    m_wrapper = function;
}

template<typename R, typename ...Args>
void FunctionCallCounter<R, Args ...>::setCallback(void (*callback)(Args...))
{
    m_callback = callback;
}

template<typename R, typename ... Args>
R wrapperFunction(Args... args)
{
    auto* fcc = FunctionCallCounter<R, Args...>::instance();
    return fcc->callOrigin(args...);
}

template<typename R, typename ... Args>
R FunctionCallCounter<R, Args ...>::callOrigin(Args... args)
{
   assert(m_function != nullptr);

   m_callCount++;

   if(m_callback != nullptr)
       m_callback(args ...);

   return m_function(args ...);
}
