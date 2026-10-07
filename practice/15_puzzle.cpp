//
// Created by 33550 on 2026/10/7.
//
#include "random.h"
#include <iostream>
#include <limits>

class Direction
{
public:
    enum Type
    {
        up ,
        down ,
        left ,
        right ,
        maxnum ,
    };

    Direction() = default;

    Direction(Type t) : m_type(t) {}
    [[nodiscard]] Type getType() const { return m_type; }

    Direction operator-() const
    {
        switch (m_type) {
            case up: return down;
            case down: return up;
            case left: return right;
            case right: return left;
            default: return maxnum;
        }
    }

    static Direction getRandomDirection()
    {
        return Direction{static_cast<Type>(Random::get(0 , Type::maxnum - 1))};
    }

private:
    Type m_type{Type::maxnum};
};

std::ostream& operator<<(std::ostream& out , const Direction& dir)
{
    switch (dir.getType()) {
        case Direction::up: return out << "up";
        case Direction::down: return out << "down";
        case Direction::left: return out << "left";
        case Direction::right: return out << "right";
        default: return out;
    }
}

namespace UserInput
{
    bool isValid(char c)
    {
        switch (c) {
            case 'w':
            case 's':
            case 'a':
            case 'd':
            case 'q': return true;
            default: return false;
        }
    }

    void clear()
    {
        std::cin.ignore(std::numeric_limits<std::streamsize>::max() , '\n');
    }

    char getInput()
    {
        char c{};
        std::cin >> c;
        clear();
        return c;
    }

    Direction getDirection(char c)
    {
        switch (c) {
            case 'w': return Direction::up;
            case 's': return Direction::down;
            case 'a': return Direction::left;
            case 'd': return Direction::right;
            default: return Direction::maxnum;
        }
    }

    char getCommand()
    {
        char c{};
        while (!isValid(c)) {
            c = getInput();
        }
        return c;
    }
}

class Point
{
    int m_x{};
    int m_y{};

public:
    Point() = default;

    Point(int x , int y) : m_x(x), m_y(y) {}
    [[nodiscard]] int getX() const { return m_x; }
    [[nodiscard]] int getY() const { return m_y; }
    bool operator==(const Point& rhs) const { return m_x == rhs.m_x && m_y == rhs.m_y; }
    bool operator!=(const Point& rhs) const { return !(*this == rhs); }

    [[nodiscard]] Point adjacent(const Direction& dir) const
    {
        switch (dir.getType()) {
            case Direction::up: return {m_x - 1 , m_y};
            case Direction::down: return {m_x + 1 , m_y};
            case Direction::left: return {m_x , m_y - 1};
            case Direction::right: return {m_x , m_y + 1};
            default: break;
        }
        return {m_x , m_y};
    }
};

class Tile
{
    int m_num{};

public:
    Tile() = default;

    Tile(const int num)
    {
        if (num < 0 || num > 15) {
            m_num = 0;
        }
        else {
            m_num = num;
        }
    };
    bool operator==(const Tile& rhs) const { return m_num == rhs.m_num; }
    bool operator!=(const Tile& rhs) const { return !(*this == rhs); }
    [[nodiscard]] bool empty() const { return m_num == 0; }
    [[nodiscard]] int getNum() const { return m_num; }

    void setNum(const int num)
    {
        if (num < 0 || num > 15) {
            m_num = 0;
        }
        else {
            m_num = num;
        }
    }

    static void swap(Tile& tile1 , Tile& tile2)
    {
        const int t = tile1.getNum();
        tile1.setNum(tile2.getNum());
        tile2.setNum(t);
    }
};

std::ostream& operator<<(std::ostream& out , const Tile& tile)
{
    if (tile.empty()) {
        out << "    ";
    }
    else if (tile.getNum() < 10) {
        out << "  " << tile.getNum() << ' ';
    }
    else {
        out << ' ' << tile.getNum() << ' ';
    }
    return out;
}

constexpr int g_consoleLines{25};

class Board
{
    Tile m_board[4][4];

    static bool isValidPoint(const Point& point)
    {
        return point.getX() >= 0 && point.getX() < 4 && point.getY() >= 0 && point.getY() < 4;
    }

    [[nodiscard]] Point getEmptyPoint() const
    {
        for (int i = 0 ; i < 4 ; ++i) {
            for (int j = 0 ; j < 4 ; ++j) {
                if (m_board[i][j].getNum() == 0) {
                    return {i , j};
                }
            }
        }
        return {};
    }

    bool swapPoint(const Point& point1 , const Point& point2)
    {
        if (isValidPoint(point1) && isValidPoint(point2)) {
            Tile::swap(getTile(point1) , getTile(point2));
            return true;
        }
        return false;
    }

public:
    Board()
    {
        for (int x = 0 ; x < 4 ; ++x) {
            for (int y = 0 ; y < 4 ; ++y) {
                m_board[x][y].setNum(x * 4 + y + 1);
            }
        }
    }

    bool operator==(const Board& rhs) const
    {
        for (int x = 0 ; x < 4 ; ++x) {
            for (int y = 0 ; y < 4 ; ++y) {
                if (m_board[x][y] != rhs.m_board[x][y]) {
                    return false;
                }
            }
        }
        return true;
    }

    friend std::ostream& operator<<(std::ostream& out , const Board& board)
    {
        for (int i = 0 ; i < g_consoleLines ; ++i) {
            out << '\n';
        }
        for (const auto& x : board.m_board) {
            for (const auto& y : x) {
                out << y;
            }
            out << '\n';
        }
        out << '\n';
        return out;
    }

    Tile& getTile(const Point& point)
    {
        if (isValidPoint(point)) {
            return m_board[point.getX()][point.getY()];
        }
        return m_board[getEmptyPoint().getX()][getEmptyPoint().getY()];
    }

    bool moveTile(Direction dir)
    {
        const Point point1{getEmptyPoint()};
        const Point point2{point1.adjacent(dir)};
        return swapPoint(point1 , point2);
    }

    void initBoard()
    {
        for (int i = 0 ; i < 1000 ; ++i) {
            moveTile(Direction::getRandomDirection());
        }
    }

    [[nodiscard]] bool isWon() const { return (*this) == Board{}; }
};

int main()
{
    Board board;
    board.initBoard();
    std::cout << board;
    while (true) {
        char c{UserInput::getCommand()};
        if (c == 'q') {
            break;
        }
        if (board.moveTile(UserInput::getDirection(c))) {
            std::cout << board;
        }
        if (board.isWon()) {
            std::cout << "You won!" << '\n';
            break;
        }
    }
}
