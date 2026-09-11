//
// Created by 33550 on 2026/9/10.
//
#include <cassert>
#include <iostream>

namespace
{
    class Resources
    {
    public:
        Resources() { std::cout << "Resources created" << std::endl; };

        ~Resources() { std::cout << "~Resources()" << std::endl; };

        void foo() const { std::cout << "Resources functions" << std::endl; }
    };
}

namespace
{
    class unique_ptr
    {
        Resources *m_resources{};

    public:
        unique_ptr() noexcept = default;

        unique_ptr(const unique_ptr &) = delete;

        unique_ptr &operator=(const unique_ptr &) = delete;

        unique_ptr(std::nullptr_t) noexcept {}

        explicit unique_ptr(Resources *resources) noexcept
            : m_resources{resources} {}

        unique_ptr(unique_ptr &&other) noexcept
            : m_resources{other.m_resources} { other.m_resources = nullptr; }

        unique_ptr &operator=(std::nullptr_t) noexcept
        {
            reset();
            return *this;
        }

        unique_ptr &operator=(unique_ptr &&other) noexcept
        {
            if (this != &other) {
                Resources *tmp = m_resources;
                m_resources = other.m_resources;
                other.m_resources = nullptr;
                delete tmp;
            }
            return *this;
        }

        ~unique_ptr() { delete m_resources; }

        explicit operator bool() const noexcept { return m_resources != nullptr; }

        Resources *operator->() const noexcept
        {
            assert(m_resources != nullptr);
            return m_resources;
        }

        Resources &operator*() const noexcept
        {
            assert(m_resources != nullptr);
            return *m_resources;
        }

        [[nodiscard]] Resources *get() const noexcept { return m_resources; }

        [[nodiscard]] Resources *release() noexcept
        {
            Resources *tmp = m_resources;
            m_resources = nullptr;
            return tmp;
        }

        void reset(Resources *resources = nullptr) noexcept
        {
            Resources *tmp = m_resources;
            m_resources = resources;
            delete tmp;
        }

        void swap(unique_ptr &other) noexcept
        {
            Resources *tmp = m_resources;
            m_resources = other.m_resources;
            other.m_resources = tmp;
        }
    };
}

int main()
{
    unique_ptr ptr{new Resources()};
    (*ptr).foo();
    ptr->foo();
}
