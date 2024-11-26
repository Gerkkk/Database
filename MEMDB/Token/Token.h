#ifndef UNTITLED_TOKEN_H
#define UNTITLED_TOKEN_H

#include <iostream>
#include <regex>
#include <map>
#include <vector>

//Class for words in a query. Storing type of word and value of the string
class Token {
public:
    std::string type;
    std::string value;

    Token (std::string type, std::string value);
    ~Token() = default;
};

//Class that can create Tokens of two types: name and regular
class TokenFactory {
public:
    TokenFactory();
    Token * make_token(std::string &x);
    Token * make_name(std::string &x);
private:
    std::vector<std::pair<std::string, std::string>> tokens;
};

//Class for one query as a tuple of tokens. Needed for two things: handling several queries in one execute and handling nested queries
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