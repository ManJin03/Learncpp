//
// Created by 33550 on 2026/9/30.
//
#include <iostream>
#include <unistd.h>
#include <fcntl.h>

class File
{
    int fd{-1};

public:
    File() = default;

    File(const File& file)
    {
        if (file.fd != -1) {
            fd = dup(file.fd);
        }
    };
    //(path,flag,mode),默认只读，700权限
    File(const std::string& path , const int flags = O_RDONLY , const int mode = S_IRWXU)
    {
        open(path , flags , mode);
    }

    File& operator=(const File& file)
    {
        if (file.fd != -1) {
            fd = dup(file.fd);
        }
        return *this;
    }

    ~File()
    {
        if (fd != -1) {
            close(fd);
        }
    }

    //(path,flags,mode)默认只读，700权限
    bool open(const std::string& path , const int flags = O_RDONLY , const int mode = S_IRWXU)
    {
        if (fd != -1) {
            close(fd);
            fd = -1;
        }
        if (const int t = ::open(path.c_str() , flags , mode) ; t >= 0) {
            this->fd = t;
            return true;
        }
        return false;
    }

    [[nodiscard]] bool is_open() const { return fd != -1; }
    /*  @pos:读取位置
        @bufsize:读取大小*/
    [[nodiscard]] std::string read(const int pos = -1 , const int bufsize = 1024) const
    {
        if (!is_open()) {
            throw std::runtime_error("file not open");
        }
        if (pos >= 0) {
            if (lseek(fd , pos , SEEK_SET) == static_cast<off_t>(-1)) {
                throw std::system_error(errno , std::generic_category() , "lseek");
            }
        }
        std::string buf(bufsize , '\0');
        const auto n = ::read(fd , buf.data() , bufsize);
        if (n < 0) {
            return {};
        }
        buf.resize(n);
        return buf;
    }
    //查看但不移动读取位置
    std::string peek(const int pos = -1 , const int bufsize = 1)
    {
        std::string buf{read(pos , bufsize)};
        (*this) -= bufsize;
        return buf;
    }
    //写入数据
    [[nodiscard]] bool write(const std::string& buf) const
    {
        if (const auto n = ::write(fd , buf.data() , buf.size()) ; n < 0) {
            return false;
        }
        return true;
    }

    [[nodiscard]] File& operator+(const int pos)
    {
        if (fd == -1) {
            throw std::runtime_error("file not open");
        }
        if (lseek(fd , pos , SEEK_CUR) == (off_t) -1) {
            throw std::system_error(errno , std::generic_category() , "lseek");
        }
        return *this;
    }

    File& operator+=(const int pos)
    {
        return operator+(pos);
    }

    [[nodiscard]] File& operator-(const int pos)
    {
        return operator+(-pos);
    }

    File& operator-=(const int pos)
    {
        return operator-(pos);
    }

    File& operator=(const int pos)
    {
        if (fd == -1) {
            throw std::runtime_error("file not open");
        }
        if (lseek(fd , pos , SEEK_SET) == (off_t) -1) {
            throw std::system_error(errno , std::generic_category() , "lseek");
        }
        return *this;
    }

    File& operator<<(const std::string& msg)
    {
        if (fd == -1) {
            throw std::runtime_error("file not open");
        }
        if (write(msg)) {
            return *this;
        }
        throw std::runtime_error("write failed");
    }

    File& operator>>(std::string& msg)
    {
        if (fd == -1) {
            throw std::runtime_error("file not open");
        }
        msg = read();
        return *this;
    }

    friend std::ostream& operator<<(std::ostream& os , const File& file)
    {
        return os << file.read();
    }

    friend std::istream& operator>>(std::istream& is , File& file)
    {
        std::string msg;
        getline(is , msg);
        file << msg;
        return is;
    }
};


int main()
{
    File file{};
    if (file.open("test" ,O_RDWR | O_CREAT | O_APPEND)) {
        std::string msg;
        std::cout << "Input: ";
        std::cin >> msg;
        if (!file.write(msg)) {
            perror("write");
        }
    }
    file << "\nthis\n" << " is a test msg\n";
    std::cin >> file;
    if (file.open("test")) {
        std::cout << "test: " << '\n';
        const auto read = file.read();
        std::cout << read << '\n';
    }
    else {
        perror("open");
    }
    file = 0;
    std::string msg;
    file >> msg;
    std::cout << "msg: " << msg << '\n';
    file -= 10;
    std::cout << file.peek(-1 , 10);
    std::cout << file << '\n';
}
