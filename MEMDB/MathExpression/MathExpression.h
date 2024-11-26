#ifndef UNTITLED_MATHEXPRESSION_H
#define UNTITLED_MATHEXPRESSION_H
#include "../Token/Token.h"
#include <vector>
#include <string>
#include <list>

//Class for every single piece of math expression that will be stored in a tree
//Since we have only unary and binary operators, two pointers to another node will be enough
//There is an initial value and type of the Node, which are just value and type of it's token
//And return value and type which are the results of it's subtree
class SyntaxTreeNode {
public:
    std::string value, type;
    std::string ret_value, ret_type;
    SyntaxTreeNode *left_son, *right_son;

    SyntaxTreeNode(std::string &inp_value, std::string &inp_type);
    ~SyntaxTreeNode();

    void execute(std::map<std::string, std::map<std::string, std::string>> &column_types,
                 std::map<std::string, std::map<std::string, std::list<std::string>::iterator>> &column_values);
};


//Class which handles math expressions that are given as a vector of tokens.
// It creates a syntax tree for the expression and executes these operations
class SyntaxTree {
public:
    static std::map<std::string, int> operations_priority;
    std::vector<Token *> source;
    SyntaxTreeNode *head;

    explicit SyntaxTree(std::vector<Token *> &expr);
    ~SyntaxTree() = default;

    void execute(std::map<std::string, std::map<std::string, std::string>> &column_types,
                 std::map<std::string, std::map<std::string, std::list<std::string>::iterator>> &column_values);

    std::pair<std::string, std::string> get_res();
private:
    SyntaxTreeNode *build_tree(std::vector<Token *> expr);
    int find_next_operation(std::vector<Token *> expr);
};

#endif //UNTITLED_MATHEXPRESSION_H
