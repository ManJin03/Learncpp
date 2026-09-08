//
// Created by 33550 on 2026/9/8.
//
#include <iostream>
#include <sys/wait.h>
#include <fcntl.h>//open()
#include <cstring>//strdup()
#include <unistd.h>//close(),read(),write()
using std::cout;
using std::cin;


static void test1()
{
    int x{10};
    const int rc = fork();
    if (rc == 0) {
        cout << "child x=" << x << '\n';
        x += 20;
        cout << "child x=" << x << '\n';
    }
    else {
        cout << "parent x=" << x << '\n';
        x -= 20;
        cout << "parent x=" << x << '\n';
    }
}

static void test2()
{
    int fd = open("./test",O_RDWR | O_CREAT);
    if (fd == -1) { return; }
    int rc = fork();
    if (rc == 0) {
        cout << "child fd=" << fd << '\n';
        char buf[16]{"child"};
        write(fd, buf, 5);
        char rd[16]{};
        read(fd, rd, 8);
        cout << "child read=" << rd << '\n';
    }
    else {
        wait(NULL);
        cout << "parent fd=" << fd << '\n';
        char buf[16]{"111"};
        write(fd, buf, 3);
        char rd[16]{};
        read(fd, rd, 8);
        cout << "parent read=" << rd << '\n';
    }
    close(fd);
    cout << "file closed" << '\n';
}

void handler(int) {} // 空信号处理函数

void test3()
{
    signal(SIGUSR1, handler);

    pid_t pid = fork();
    if (pid == 0) {
        std::cout << "hello" << std::endl;
        kill(getppid(), SIGUSR1); // 向父进程发送信号
        exit(0);
    }
    else {
        pause(); // 父进程挂起，直到收到信号
        std::cout << "goodbye" << std::endl;
    }
}

void test4()
{
    int rc = fork();
    if (rc == 0) {
        execl("/usr/bin/ls", "ls", NULL);
        execle("/usr/bin/ls", "ls", "-a", NULL);
        execlp("ls", "ls", "-l", NULL);
        char *args[4];
        args[0] = strdup("ls");
        args[1] = strdup("-a");
        args[2] = strdup("-l");
        args[3] = NULL;
        execv("/usr/bin/ls", args);
        execvp(args[0], args);
    }
    else {
        wait(NULL);
        std::cout << "goodbye" << std::endl;
    }
}

void test5()
{
    int rc = fork();
    if (rc == 0) {
        int wc = wait(NULL);
        if (wc == -1)
            cout << "hello" << std::endl;
    }
    else {
        waitpid(rc,NULL, 0);
        cout << "goodbye" << std::endl;
    }
}

void test6()
{
    int rc = fork();
    if (rc == 0) {
        close(STDOUT_FILENO);
        open("./test",O_WRONLY | O_CREAT);
        printf("hello\n");
    }
    else {
        wait(NULL);
        std::cout << "goodbye" << std::endl;
    }
}

void test7()
{
    int pipefd[2];
    pipe(pipefd);
    int rc = fork();
    if (rc == 0) {
        close(pipefd[0]); //关闭读端
        write(pipefd[1], "hello", 5);
        close(pipefd[1]); //关闭写端
        exit(0);
    }
    wait(NULL);
    rc = fork();
    if (rc == 0) {
        close(pipefd[1]); //关闭写端
        char buf[8]{};
        read(pipefd[0], buf, 8);
        cout << buf << '\n';
        close(pipefd[0]); //关闭读端
        exit(0);
    }
    wait(NULL);
    cout << "goodbye" << std::endl;
}

void test8()
{
    close(STDOUT_FILENO);
    close(STDIN_FILENO);
    int pipefd[2];
    pipe(pipefd);
    int rc = fork();
    if (rc == 0) {
        close(pipefd[0]); //关闭读端
        printf("hello");
        fflush(stdout); //刷新缓冲区以让数据进入管道
        close(pipefd[1]); //关闭写端，也可省略这两行让exit自动刷新并关闭管道
        exit(0);
    }
    wait(NULL);
    rc = fork();
    if (rc == 0) {
        close(pipefd[1]); //关闭写端
        char buf[8]{};
        scanf("%s", buf);
        close(pipefd[0]); //关闭读端
        exit(0);
    }
    wait(NULL);
    close(pipefd[0]);
    close(pipefd[1]);
}

int main() { test8(); }
