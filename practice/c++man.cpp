//
// Created by 33550 on 2026/10/1.
//
#include "random.h"
#include <iostream>
#include <unordered_set>

namespace WordList
{
    using std::vector;
    using std::string_view;
    static vector<string_view> words{
        "mystery" , "broccoli" , "account" ,
        "almost" , "spaghetti" , "opinion" ,
        "beautiful" , "distance" , "luggage"
    };

    static string_view randomWord()
    {
        return words[Random::get<size_t>(0 , words.size() - 1)];
    }
}

class Session
{
    std::string_view m_word{WordList::randomWord()};
    std::unordered_set<char> m_guessed{};

public:
    [[nodiscard]] std::string_view getWord() const { return m_word; }
    [[nodiscard]] size_t getCount() const { return m_word.size() - m_guessed.size() + m_word.size() / 3; }
    [[nodiscard]] bool isGuessed(const char c) const { return m_guessed.count(c); }
    void setGuessed(const char c) { m_guessed.insert(c); }
    bool isInWord(const char c) const { return m_word.find(c) != std::string_view::npos; }

    bool isWon() const
    {
        for (const auto c : m_word) {
            if (!isGuessed(c)) {
                return false;
            }
        }
        return true;
    }
};

static void draw(const Session& session)
{
    using std::cout;
    cout << '\n';
    cout << "The word is ";
    for (auto c : session.getWord()) {
        if (session.isGuessed(c)) {
            cout << c;
        }
        else {
            cout << '_';
        }
    }
    cout << "    Wrong guessed ";
    for (int i = 0 ; i < session.getCount() ; ++i) {
        cout << '+';
    }
    for (char c = 'a' ; c <= 'z' ; ++c) {
        if (session.isGuessed(c) && !session.isInWord(c)) {
            cout << c;
        }
    }
    cout << '\n';
}

char getGuess(const Session& session)
{
    using std::cout;
    using std::cin;
    while (true) {
        cout << "Enter your next letter: ";
        char guess{};
        cin >> guess;
        if (!cin) {
            cin.clear();
            cout << "That wasn't a valid input.  Try again.\n";
            cin.ignore(std::numeric_limits<std::streamsize>::max() , '\n');
            continue;
        }
        cin.ignore(std::numeric_limits<std::streamsize>::max() , '\n');
        if (!isalpha(guess)) {
            cout << "That wasn't a valid input.  Try again.\n\n";
            continue;
        }
        if (session.isGuessed(guess)) {
            cout << "You already guessed that.  Try again.\n\n";
            continue;
        }
        if (session.isInWord(guess)) {
            cout << "Yes, '" << guess << "' is in the word!\n";
        }
        else {
            cout << "No, '" << guess << "' is not in the word!\n";
        }
        return guess;
    }
}

int main()
{
    using std::cout;
    cout << "Welcome to C++man (a variant of Hangman)\n";
    cout << "To win: guess the word.  To lose: run out of pluses.\n";

    Session session{};
    draw(session);
    while (session.getCount()) {
        session.setGuessed(getGuess(session));
        draw(session);
        if (session.isWon()) {
            cout << "You won!";
            return 0;
        }
    }
    cout << "You lose.\n";
    cout << "The word is " << session.getWord() << "\n";
}
