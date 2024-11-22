#include "Token.h"
#include <string>

TokenFactory::TokenFactory(){
    tokens = {
            //Math symbols
            {R"([\s]*[:])", ":"},
            {R"([\s]*[+])", "+"},
            {R"([\s]*[-])", "-"},
            {R"([\s]*[*])", "*"},
            {R"([\s]*[/])", "/"},
            {R"([\s]*[%])", "%"},
            {R"([\s]*[<])", "<"},
            {R"([\s]*[>])", ">"},
            {R"([\s]*[=])", "="},
            {R"([\s]*[<][=])", "<="},
            {R"([\s]*[>][=])", ">="},
            {R"([\s]*[!][=])", "!="},
            {R"([\s]*[&][&])", "&&"},
            {R"([\s]*[|][|])", "||"},
            {R"([\s]*[!])", "!"},
            {R"([\s]*[\^][\^])", "^"},
            {R"([\s]*[(])", "("},
            {R"([\s]*[)])", ")"},
            {R"([\s]*[|][A-Za-z]*[|])", "module"},
            //Syntax
            {R"([\s]*[{])", "{"},
            {R"([\s]*[}])", "}"},
            {R"([\s]*[Ii][Nn][Ss][Ee][Rr][Tt])", "insert"},
            {R"([\s]*[Ss][Ee][Ll][Ee][Cc][Tt])", "select"},
            {R"([\s]*[Ff][Rr][Oo][Mm])", "from"},
            {R"([\s]*[Ww][Hh][Ee][Rr][Ee])", "where"},
            {R"([\s]*[Uu][Pp][Dd][Aa][Tt][Ee])", "update"},
            {R"([\s]*[Ss][Ee][Tt])", "set"},
            {R"([\s]*[Dd][Ee][Ll][Ee][Tt][Ee])", "delete"},
            {R"([\s]*[Jj][Oo][Ii][Nn])", "join"},
            {R"([\s]*[Oo][Nn])", "on"},
            {R"([\s]*[Tt][Oo])", "to"},
            {R"([\s]*[Tt][Aa][Bb][Ll][Ee])", "table"},
            {R"([\s]*[Cc][Rr][Ee][Aa][Tt][Ee])", "create"},
            {R"([\s]*[Ii][Nn][Dd][Ee][Xx])", "index"},
            {R"([\s]*[Uu][Nn][Ii][Qq][Uu][Ee][,]?)", "unique"},
            {R"([\s]*[Aa][Uu][Tt][Oo][Ii][Nn][Cc][Rr][Ee][Mm][Ee][Nn][Tt][,]?)", "autoincrement"},
            {R"([\s]*[Kk][Ee][Yy][,]?)", "key"},
            //constants and types
            {R"([\s]*[t][r][u][e][,]?)", "true"},
            {R"([\s]*[f][a][l][s][e][,]?)", "false"},
            {R"([\s]*[+-]?[0-9]+[,]?)", "int32"},
            {R"([\s]*"[\a-zA-Z0-9_]*"[,]?)", "string"},
            {R"([\s]*[0][x][0-9a-f]+[,]?)", "bin"}
    };
}


Token::Token(std::string type, std::string value) {
    this->type = std::move(type);
    this->value = std::move(value);
}

Token * TokenFactory::make_name(std::string &x) {
    std::string ret_type, ret_val;
    if (std::regex_match(x, std::regex(R"([\s]*[_]?[a-zA-Z_]+[0-9]*[,:]?)"))) {
        ret_type = "name";
        ret_val = x;
    } else if (std::regex_match(x, std::regex(R"([\s]*[,:])"))) {
        ret_type = "empty_name";
        ret_val = x;
    } else if (std::regex_match(x, std::regex(R"([\s]*[_]?[a-zA-Z_]+[0-9]*.[a-zA-Z_]+[0-9]*[,:]?)") )) {
        ret_type = "complex_name";
        ret_val = x;
    } else if (std::regex_match(x, std::regex(R"([\s]*[_]?[a-zA-Z_]+[0-9]*\[[0-9]+\][,:]?)"))) {
        ret_type = "type_name";
        ret_val = x;
    }

    int ll = 0, rr = ret_val.size() - 1;

    while(ret_val[ll] == ',' || ret_val[ll] == '|' ||  ret_val[ll] == '\"') {
        ll++;
    }

    while(ret_val[rr] == ',' || ret_val[rr] == '|' ||  ret_val[rr] == '\"') {
        rr--;
    }

    ret_val = ret_val.substr(ll, rr - ll + 1);

    if (ret_val.empty()) {
        return nullptr;
    } else {
        auto ret = new Token(ret_type, ret_val);
        return ret;
    }
}

Token * TokenFactory::make_token(std::string &x) {
    std::string ret_type, ret_val;
    for (const auto &it : TokenFactory::tokens) {
        if (std::regex_match(x, std::regex(it.first))) {
            ret_type = it.second;
            ret_val = x;
        }
    }

    int ll = 0, rr = ret_val.size() - 1;

    while(ret_val[ll] == ',' || ret_val[ll] == '|' ||  ret_val[ll] == '\"') {
        ll++;
    }

    while(ret_val[rr] == ',' || ret_val[rr] == '|' ||  ret_val[rr] == '\"') {
        rr--;
    }

    ret_val = ret_val.substr(ll, rr - ll + 1);

    if (ret_val.empty()) {
        return nullptr;
    } else {
        auto ret = new Token(ret_type, ret_val);
        return ret;
    }
}