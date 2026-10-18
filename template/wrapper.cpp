//
// Created by 33550 on 2026/10/18.
//
#include <memory>
class function_wrapper
{
    class wrapper_base
    {
    public:
        virtual ~wrapper_base() = default;

        virtual void operator()() =0;
    };

    std::unique_ptr<wrapper_base> m_base;

    template <typename T>
    class model : public wrapper_base
    {
        T m_f;

    public:
        model(T&& value) : m_f{std::move(value)} {}
        void operator()() override { m_f(); }
    };

public:
    template <typename T>
        requires (!std::is_same_v<std::decay_t<T>,function_wrapper>)
    function_wrapper(T&& value) : m_base{std::make_unique<model<T>>(std::forward<T>(value))} {}

    function_wrapper() = default;

    function_wrapper(function_wrapper&& other) noexcept
        : m_base{std::move(other.m_base)} {}

    function_wrapper& operator=(function_wrapper&& other) noexcept
    {
        m_base = std::move(other.m_base);
        return *this;
    }

    function_wrapper(const function_wrapper&) = delete;

    function_wrapper(function_wrapper&) = delete;

    function_wrapper& operator=(const function_wrapper&) = delete;

    void operator()() const { m_base->operator()(); }
};
