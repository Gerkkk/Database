#ifndef UNTITLED_TOKEN_H
#define UNTITLED_TOKEN_H

#include <iostream>
#include <regex>
#include <map>
#include <vector>


class Token {
public:
    std::string type;
    std::string value;

    Token (std::string type, std::string value);
    ~Token() = default;
};


class TokenFactory {
public:
    TokenFactory();
    Token * make_token(std::string &x);
    Token * make_name(std::string &x);
private:
    std::vector<std::pair<std::string, std::string>> tokens;
};


struct CommandsLine {
public:
    bool flag;
    std::vector<Token *> commands;

    CommandsLine() {
        this->commands = {};
        this->flag = false;
    }

    CommandsLine(std::vector<Token *> &commands, bool flag) {
        this->commands = commands;
        this->flag = flag;
    }
};

#endif //UNTITLED_TOKEN_H