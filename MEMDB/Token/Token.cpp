#include "Token.h"
#include <string>

//Static map with tokens. I believe we do not need derived classes for each token type
//since the structure of the token is the same. Token is fully defined by regex
TokenFactory::TokenFactory(){
    tokens = {
            {R"([+])", "+"},
            {R"([-])", "-"},
            {R"([*])", "*"},
            {R"([/])", "/"},
            {R"([(])", "("},
            {R"([)])", ")"},
            {R"([%])", "%"},
            {R"([<])", "<"},
            {R"([>])", ">"},
            {R"([=])", "="},
            {R"([!])", "!"},
            {R"([:])", ":"},
            {R"([{])", "{"},
            {R"([}])", "}"},
            {R"([+-]?[0-9]+[,]?)", "int32"},
            {R"("[\a-zA-Z0-9_]*"[,]?)", "string"},
            {R"([<][=])", "<="},
            {R"([>][=])", ">="},
            {R"([!][=])", "!="},
            {R"([&][&])", "&&"},
            {R"([|][|])", "||"},
            {R"([\^][\^])", "^^"},
            {R"([Oo][Nn])", "on"},
            {R"([Tt][Oo])", "to"},
            {R"([+-]?[0-9]+[,]?)", "int32"},
            {R"([0][x][0-9a-f]+[,]?)", "bytes"},
            {R"("[\a-zA-Z0-9_]*"[,]?)", "string"},
            {R"([|][A-Za-z]*[|])", "module"},
            {R"([Ss][Ee][Tt])", "set"},//26
            {R"([Kk][Ee][Yy][,]?)", "key"},
            {R"([Ff][Rr][Oo][Mm])", "from"},//28
            {R"([Jj][Oo][Ii][Nn])", "join"},
            {R"([t][r][u][e][,]?)", "true"},
            {R"([+-]?[0-9]+[,]?)", "int32"},
            {R"([0][x][0-9a-f]+[,]?)", "bytes"},
            {R"("[\a-zA-Z0-9_]*"[,]?)", "string"},
            {R"([|][A-Za-z]*[|])", "module"},
            {R"([f][a][l][s][e][,]?)", "false"},//35
            {R"([Ii][Nn][Dd][Ee][Xx])", "index"},
            {R"([Ww][Hh][Ee][Rr][Ee])", "where"},
            {R"([Tt][Aa][Bb][Ll][Ee])", "table"},
            {R"([t][r][u][e][,]?)", "true"},
            {R"([+-]?[0-9]+[,]?)", "int32"},
            {R"([0][x][0-9a-f]+[,]?)", "bytes"},
            {R"("[\a-zA-Z0-9_]*"[,]?)", "string"},
            {R"([|][A-Za-z]*[|])", "module"},
            {R"([Uu][Pp][Dd][Aa][Tt][Ee])", "update"},//43
            {R"([Dd][Ee][Ll][Ee][Tt][Ee])", "delete"},
            {R"([Cc][Rr][Ee][Aa][Tt][Ee])", "create"},
            {R"([Uu][Nn][Ii][Qq][Uu][Ee][,]?)", "unique"},
            {R"([Ii][Nn][Ss][Ee][Rr][Tt])", "insert"},
            {R"([Ss][Ee][Ll][Ee][Cc][Tt])", "select"},
            {R"([f][a][l][s][e][,]?)", "false"},
            //constants and types
            {R"([+-]?[0-9]+[,]?)", "int32"},//49
            {R"([0][x][0-9a-f]+[,]?)", "bytes"},
            {R"("[\a-zA-Z0-9_]*"[,]?)", "string"},
            {R"([Aa][Uu][Tt][Oo][Ii][Nn][Cc][Rr][Ee][Mm][Ee][Nn][Tt][,]?)", "autoincrement"},
            {R"([|][A-Za-z]*[|])", "module"}
    };
}


Token * TokenFactory::make_token(std::string &x) {
    std::string ret_type, ret_val;
    int l = 0, r = TokenFactory::tokens.size();

    //All these weird numbers are needed for optimization of searching for token type
    //Since calling 50 regexes takes too much time
    //This is the weakest point of the program, searching for correct token takes too much time
    if (x.size() > 6) {
        l = TokenFactory::tokens.size() - 5;
    } else if (x.size() == 6) {
        l = 45;
    } else if (x.size() == 5) {
        l = 37;
        r = 45;
    } else if (x.size() == 4) {
        l = 30;
        r = 37;
    } else if (x.size() == 3) {
        l = 28;
        r = 37;
    } else if (x.size() == 2) {
        l = 16;
        r = 28;
    } else if (x.size() == 1) {
        r = 16;
    }

    //const std::clock_t c_start = std::clock();
    for (int i = l; i < r; i++) {
        auto it = TokenFactory::tokens[i];
        if (std::regex_match(x, std::regex(it.first))) {
            ret_type = it.second;
            ret_val = x;
            break;
        }
    }

    //Clearing string
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

Token::Token(std::string type, std::string value) {
    this->type = std::move(type);
    this->value = std::move(value);
}

Token * TokenFactory::make_name(std::string &x) {
    std::string ret_type, ret_val;

    if (std::regex_match(x, std::regex(R"([s][t][r][i][n][g][0-9]*\[[0-9]+\][,:]?)"))
        || std::regex_match(x, std::regex(R"([i][n][t][3][2])"))
           || std::regex_match(x, std::regex(R"([b][o][o][l])"))
           || std::regex_match(x, std::regex(R"([b][y][t][e][s]\[[0-9]+\][,:]?)"))) {
        ret_type = "type_name";
        ret_val = x;
    } else if (std::regex_match(x, std::regex(R"([_]?[a-zA-Z_]+[0-9]*[,:]?)"))) {
        ret_type = "name";
        ret_val = x;
    } else if (x == "," || std::regex_match(x, std::regex(R"([,][\s]*)"))) {
        ret_type = "empty_name";
        ret_val = x;
    } else if (std::regex_match(x, std::regex(R"([_]?[a-zA-Z_]+[0-9]*.[a-zA-Z_]+[0-9]*[,:]?)") )) {
        ret_type = "complex_name";
        ret_val = x;
    }

    //Clearing string
    int ll = 0, rr = ret_val.size() - 1;

    while(ret_val[ll] == ',' || ret_val[ll] == '|' ||  ret_val[ll] == '\"') {
        ll++;
    }

    while(ret_val[rr] == ',' || ret_val[rr] == '|' ||  ret_val[rr] == '\"') {
        rr--;
    }

    ret_val = ret_val.substr(ll, rr - ll + 1);

    if (ret_type.empty()) {
        return nullptr;
    } else {
        auto ret = new Token(ret_type, ret_val);
        return ret;
    }
}

