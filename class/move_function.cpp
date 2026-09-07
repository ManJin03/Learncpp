//
// Created by 33550 on 2026/9/7.
//
#include <cstring>
#include <iostream>
#include <utility>
using std::cout;

class Book
{
private:
    int m_id{};
    char *m_name{nullptr};

    void copyName(const char *name)
    {
        delete m_name;
        m_name = new char[strlen(name) + 1];
        if (m_name == nullptr) { perror("Error allocating memory for new name"); }
        strcpy(m_name, name);
        //指示创建了一片空间
        printf("Copied name '%s'\n", m_name);
    }

    void checkID()
    {
        if (m_id < 0) {
            printf("Book '%s' id(%d) invalid,default set to 0\n", m_name, m_id);
            m_id = 0;
        }
    }

public:
    Book() = default;

    Book(const int &id, const char *name)
        : m_id(id)
    {
        copyName(name);
        checkID();
        printf("Book '%s' (id:%d) constructed\n", m_name, m_id);
    }

    Book(const Book &s)
        : m_id(s.m_id)
    {
        copyName(s.m_name);
        printf("Book '%s' (id:%d) copied\n", m_name, m_id);
    }

    Book &operator=(const Book &s)
    {
        if (this == &s) return *this;
        m_id = s.m_id;
        copyName(s.m_name);
        printf("Book '%s' (id:%d) assigned\n", m_name, m_id);
        return *this;
    }

    /*noexcept对异常强保证，允许编译器在move_if_noexcept时选择效率更高的移动构造
     *对于移动语义，必须（最优）确保内部不会抛出异常，否则因为移动语义修改了源数据，
     *发生异常后程序不仅移动数据失败，还会丢失源数据。（复制构造不会丢失源数据）
     *应对移动构造，移动赋值构造，交换函数标记实现noexcept。
     */
    //&&表示函数参数是右值，右值是临时对象，使用移动语义不会进行复制，节省了开销
    Book(Book &&s) noexcept
        : m_id(s.m_id), m_name(s.m_name)
    {
        s.m_name = nullptr;
        printf("Book '%s' (id:%d) move copied\n", m_name, m_id);
    }

    Book &operator=(Book &&s) noexcept
    {
        if (this == &s) return *this;
        m_id = s.m_id;
        m_name = s.m_name;
        s.m_name = nullptr;
        printf("Book '%s' (id:%d) move assigned\n", m_name, m_id);
        return *this;
    }

    ~Book()
    {
        printf("Book '%s' (id:%d) destroyed\n", m_name, m_id);
        delete m_name;
    }

    [[nodiscard]] const char *getName() const { return m_name; }
    [[nodiscard]] int getID() const { return m_id; }
    void setName(const char *name) { copyName(name); }
    void setID(const int &id) { m_id = id; }
};

void printBook(const Book &book) { printf("Book '%s' (id:%d)\n", book.getName(), book.getID()); }
//swap函数必须尽量进行noexpect规范
void swap(Book &first, Book &second) noexcept
{
    const Book temp{first};
    first = second;
    second = temp;
    cout << "Book swaped\n";
}

void swap_move(Book &first, Book &second) noexcept
{
    //std::move()会强转左值为右值以调用移动语义，使用std::move_if_noexcept()进行异常强保证检查
    Book temp{std::move_if_noexcept(first)};
    first = std::move_if_noexcept(second);
    second = std::move_if_noexcept(temp);
    cout << "Book moved\n";
}

Book createBook() { return Book{0, "new book"}; }

int main()
{
    cout << "book1正常构造\n";
    Book book1{1, "first book"};
    cout << "book2复制省略\n";
    Book book2{createBook()};
    cout << "book3使用右值移动构造\n";
    Book book3;
    book3 = createBook();
    book2.setName("second book");
    book2.setID(2);

    printBook(book1);
    printBook(book2);
    cout << "复制交换，多次请求分配空间\n";
    swap(book1, book2);
    printBook(book1);
    printBook(book2);
    cout << "移动交换，只移动现有资源，省略了复制开销\n";
    swap_move(book1, book2);
    printBook(book1);
    printBook(book2);
}
