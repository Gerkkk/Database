#include "MathExpression.h"


//Static map with operations priorities
//keep priorities like in c++ in case we will add some new operators
std::map<std::string ,int> SyntaxTree::operations_priority = {
        {"||", 15},
        {"&&", 14},
        {"^^", 12},
        {"=", 10},
        {"!=", 10},
        {">=", 9},
        {">", 9},
        {"<=", 9},
        {"<", 9},
        {"+", 6},
        {"-", 6},
        {"*", 5},
        {"/", 5},
        {"%", 5},
        {"!", 3},
        {"string", 0},
        {"int32", 0},
        {"bin", 0},
        {"true", 0},
        {"false", 0},
        {"module", 0},
        {"name", 0},
        {"complex_name", 0},
};

SyntaxTree::SyntaxTree(std::vector<Token *> &expr) {
    source = expr;
    head = build_tree(source);
}

//Execute of syntax tree is executing it's head node
void SyntaxTree::execute(std::map<std::string, std::map<std::string, std::string>> &column_types, std::map<std::string, std::map<std::string, std::list<std::string>::iterator>> &column_values) {
    head->execute(column_types, column_values);
};

//returns type and value of return of syntax tree
std::pair<std::string, std::string> SyntaxTree::get_res() {
    return std::make_pair(head->ret_type, head->ret_value);
}

//Building syntax tree. Splits vector of Tokens with less prioritized operation.
//Also handling ( )
//Then creates node of chosen operation and recursively builds subtrees
SyntaxTreeNode *SyntaxTree::build_tree(std::vector<Token *> expr) {
    if (expr.empty()) {
        return nullptr;
    }

    if (expr.size() == 1) {
        auto new_node = new SyntaxTreeNode(expr[0]->value, expr[0]->type);
        return new_node;
    }

    if (expr[0]->type == "!") {
        std::vector <Token *> left, ret_left;

        for (int i = 1; i < expr.size(); i++) {
            left.push_back(expr[i]);
        }

        int ll = 0, lr = left.size() - 1;
        while (true) {
            if(left[ll]->type == "(" && left[lr]->type == ")") {
                ll++;
                lr--;
            } else {
                break;
            }
        }

        for (int i = ll; i <= lr; i++) {
            ret_left.push_back(left[i]);
        }

        auto new_node = new SyntaxTreeNode(expr[0]->value, expr[0]->type);

        if (!left.empty()) {
            new_node->left_son = build_tree(ret_left);
        }

        return new_node;
    }

    int split_point = find_next_operation(expr);

    std::vector <Token *> right, left;

    for (int i = 0; i < split_point; i++) {
        left.push_back(expr[i]);
    }

    for (int i = split_point + 1; i < expr.size(); i++) {
        right.push_back(expr[i]);
    }

    auto new_node = new SyntaxTreeNode(expr[split_point]->value, expr[split_point]->type);

    std::vector <Token *> ret_left, ret_right;

    int ll = 0, lr = left.size() - 1;
    while (true) {
        if(left[ll]->type == "(" && left[lr]->type == ")") {
            ll++;
            lr--;
        } else {
            break;
        }
    }

    for (int i = ll; i <= lr; i++) {
        ret_left.push_back(left[i]);
    }

    ll = 0, lr = right.size() - 1;
    while (true) {
        if(right[ll]->type == "(" && right[lr]->type == ")") {
            ll++;
            lr--;
        } else {
            break;
        }
    }

    for (int i = ll; i <= lr; i++) {
        ret_right.push_back(right[i]);
    }

    if (!left.empty()) {
        new_node->left_son = build_tree(ret_left);
    }

    if (!right.empty()) {
        if (left.empty()) {
            new_node->left_son = build_tree(ret_right);
        } else {
            new_node->right_son = build_tree(ret_right);
        }
    }

    return new_node;
}

//finds less prioritized operation in a vector of tokens
int SyntaxTree::find_next_operation(std::vector<Token *> expr) {
    //add check for parenthesis wrrors
    int ans_ind = -1, max_prior = -1;
    int balance = 0;

    for (int i = 0; i < expr.size(); i++) {
        if(expr[i]->type == "(") {
            balance++;
        } else if (expr[i]->type == ")") {
            balance--;
        } else if (operations_priority[expr[i]->type] >= max_prior && balance == 0) {
            ans_ind = i;
            max_prior = operations_priority[expr[i]->type];
        }
    }

    return ans_ind;
}