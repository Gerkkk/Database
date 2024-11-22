#ifndef UNTITLED_MATHEXPRESSION_H
#define UNTITLED_MATHEXPRESSION_H
#include "../Token/Token.h"
#include <vector>
#include <string>
#include <list>

class SyntaxTreeNode {
public:
    std::string value, type;
    std::string ret_value, ret_type;

    SyntaxTreeNode *left_son, *right_son;

    SyntaxTreeNode(std::string &inp_value, std::string &inp_type) {
        value = inp_value;
        type = inp_type;
        left_son = nullptr;
        right_son = nullptr;
    }

    void execute(std::map<std::string, std::map<std::string, std::string>> &column_types, std::map<std::string, std::map<std::string, std::list<std::string>::iterator>> &column_values) {
        if (left_son) {
            left_son->execute(column_types, column_values);
        }

        if(right_son) {
            right_son->execute(column_types, column_values);
        }

        //cringy ifs
        if (type == "+") {
            if(!left_son || !right_son) {
                std::cout << "plus should have 2 operands" << std::endl;
                exit(-1);
            }

            if (left_son->ret_type != right_son->ret_type) {
                std::cout << "plus should have 2 operands of the same type" << std::endl;
                exit(-1);
            } else {
                ret_type = left_son->ret_type;
                if (left_son->ret_type == "int32") {
                    ret_value = std::to_string(std::stoi(left_son->ret_value) + std::stoi(right_son->ret_value));
                } else if (left_son->ret_type == "string") {
                    ret_value = left_son->ret_value + right_son->ret_value;
                } else {
                    std::cout << "plus should have 2 operands of type string/int32" << std::endl;
                    exit(-1);
                }
            }
        } else if (type == "int32") {
            ret_value = value;
            ret_type = "int32";
        } else if (type == "string") {
            ret_value = value;
            ret_type = "string";
        } else if (type == "true") {
            ret_value = value;
            ret_type = "bool";
        } else if (type == "false") {
            ret_value = value;
            ret_type = "bool";
        } else if (type == "*") {
            if(!left_son || !right_son) {
                std::cout << "multiplication should have 2 operands" << std::endl;
                exit(-1);
            }

            if (left_son->ret_type != right_son->ret_type) {
                std::cout << "multiplication should have 2 operands of type int32" << std::endl;
                exit(-1);
            } else {
                ret_type = "int32";
                if (left_son->ret_type == "int32") {
                    ret_value = std::to_string(std::stoi(left_son->ret_value) * std::stoi(right_son->ret_value));
                } else {
                    std::cout << "multiplication should have 2 operands of type int32" << std::endl;
                    exit(-1);
                }
            }
        } else if (type == "-") {
            if(!left_son || !right_son) {
                std::cout << "minus should have 2 operands" << std::endl;
                exit(-1);
            }

            if (left_son->ret_type != right_son->ret_type) {
                std::cout << "minus should have 2 operands of type int32" << std::endl;
                exit(-1);
            } else {
                ret_type = "int32";
                if (left_son->ret_type == "int32") {
                    ret_value = std::to_string(std::stoi(left_son->ret_value) - std::stoi(right_son->ret_value));
                } else {
                    std::cout << "minus should have 2 operands of type int32" << std::endl;
                    exit(-1);
                }
            }
        } else if (type == "/") {
            if (!left_son || !right_son) {
                std::cout << "division should have 2 operands" << std::endl;
                exit(-1);
            }

            if (left_son->ret_type != right_son->ret_type) {
                std::cout << "division should have 2 operands of type int32" << std::endl;
                exit(-1);
            } else {
                ret_type = "int32";
                if (left_son->ret_type == "int32") {
                    ret_value = std::to_string(std::stoi(left_son->ret_value) / std::stoi(right_son->ret_value));
                } else {
                    std::cout << "division should have 2 operands of type int32" << std::endl;
                    exit(-1);
                }
            }
        } else if (type == "%") {
            if (!left_son || !right_son) {
                std::cout << "mod should have 2 operands" << std::endl;
                exit(-1);
            }

            if (left_son->ret_type != right_son->ret_type) {
                std::cout << "mod should have 2 operands of type int32" << std::endl;
                exit(-1);
            } else {
                ret_type = "int32";
                if (left_son->ret_type == "int32") {
                    ret_value = std::to_string(std::stoi(left_son->ret_value) % std::stoi(right_son->ret_value));
                } else {
                    std::cout << "mod should have 2 operands of type int32" << std::endl;
                    exit(-1);
                }
            }
        } else if (type == "=") {
            if (!left_son || !right_son) {
                std::cout << "equality should have 2 operands" << std::endl;
                exit(-1);
            }

            if (left_son->ret_type != right_son->ret_type) {
                std::cout << "equality should have 2 operands of the same type" << std::endl;
                exit(-1);
            } else {
                ret_type = "bool";
                if (left_son->ret_value == right_son->ret_value) {
                    ret_value = "true";
                } else {
                    ret_value = "false";
                }
            }
        } else if (type == ">") {
            if (!left_son || !right_son) {
                std::cout << "Greater should have 2 operands" << std::endl;
                exit(-1);
            }

            ret_type = "bool";

            if (left_son->ret_type != right_son->ret_type) {
                std::cout << "Greater should have 2 operands of the same type" << std::endl;
                exit(-1);
            } else if (left_son->ret_type == "int32"){
                if (std::stoi(left_son->ret_value) > std::stoi(right_son->ret_value)) {
                    ret_value = "true";
                } else {
                    ret_value = "false";
                }
            } else if (left_son->ret_type == "string") {
                if (left_son->ret_value.compare(right_son->ret_value) == 1) {
                    ret_value = "true";
                } else {
                    ret_value = "false";
                }
            } else if (left_son->ret_type == "bool") {
                if (left_son->ret_value == "true" && right_son->ret_value == "false") {
                    ret_value = "true";
                } else {
                    ret_value = "false";
                }
            }
        } else if (type == "<") {
            if (!left_son || !right_son) {
                std::cout << "Less should have 2 operands" << std::endl;
                exit(-1);
            }

            ret_type = "bool";

            if (left_son->ret_type != right_son->ret_type) {
                std::cout << "Less should have 2 operands of the same type" << std::endl;
                exit(-1);
            } else if (left_son->ret_type == "int32"){
                if (std::stoi(left_son->ret_value) < std::stoi(right_son->ret_value)) {
                    ret_value = "true";
                } else {
                    ret_value = "false";
                }
            } else if (left_son->ret_type == "string") {
                if (left_son->ret_value.compare(right_son->ret_value) < 0) {
                    ret_value = "true";
                } else {
                    ret_value = "false";
                }
            } else if (left_son->ret_type == "bool") {
                if (left_son->ret_value == "false" && right_son->ret_value == "true") {
                    ret_value = "true";
                } else {
                    ret_value = "false";
                }
            }
        } else if (type == "!=") {
            if (!left_son || !right_son) {
                std::cout << "Not equals operator should have 2 operands" << std::endl;
                exit(-1);
            }

            ret_type = "bool";

            if (left_son->ret_type != right_son->ret_type) {
                std::cout << "Not equals operator should have 2 operands of the same type" << std::endl;
                exit(-1);
            } else {
                ret_type = "bool";
                if (left_son->ret_value != right_son->ret_value) {
                    ret_value = "true";
                } else {
                    ret_value = "false";
                }
            }
        } else if (type == "!") {
            if (!left_son) {
                std::cout << "Not operator should have 1 operand of type bool" << std::endl;
                exit(-1);
            }

            ret_type = "bool";

            if (left_son->ret_type != "bool") {
                std::cout << "Not operator should have 1 operand of type bool" << std::endl;
                exit(-1);
            } else {
                if (left_son->ret_value == "true") {
                    ret_value = "false";
                } else {
                    ret_value = "true";
                }
            }
        } else if (type == "&&") {
            if (!left_son || !right_son) {
                std::cout << "Logical and should have 2 operands" << std::endl;
                exit(-1);
            }

            if (left_son->ret_type != right_son->ret_type) {
                std::cout << "Logical and should have 2 operands of type bool" << std::endl;
                exit(-1);
            } else {
                ret_type = "bool";
                if (left_son->ret_type == "bool") {
                    ret_value = (left_son->ret_value == "true" && right_son->ret_value == "true") ? "true" : "false";
                } else {
                    std::cout << "Logical and should have 2 operands of type bool" << std::endl;
                    exit(-1);
                }
            }
        } else if (type == "name") {
            if(column_types.size() > 1) {
                std::cout << "There is more than one table in expression. Use the name of the table in addition to name of the column" << std::endl;
                exit(-1);
            } else if (column_types.size() == 0) {
                std::cout << "Not enough tables for execute" << std::endl;
                exit(-1);
            } else {
                if ((*column_types.begin()).second.find(value) != (*column_types.begin()).second.end()) {
                    ret_type = (*column_types.begin()).second[value];
                } else {
                    std::cout << "No column with name " << value << " in table " << (*column_types.begin()).first << std::endl;
                    exit(-1);
                }

                ret_value = (*(*column_values.begin()).second[value]);
            }
        }

        //std::cout << "NODE: " << type << " " << value << " Result: " << ret_type << " " << ret_value << std::endl;
    }
};

class SyntaxTree {
public:
    static std::map<std::string, int> operations_priority;
    std::vector<Token *> source;
    SyntaxTreeNode *head;

    SyntaxTree(std::vector<Token *> &expr) {
        source = expr;
        head = build_tree(source);
    }

    void execute(std::map<std::string, std::map<std::string, std::string>> &column_types, std::map<std::string, std::map<std::string, std::list<std::string>::iterator>> &column_values) {
        head->execute(column_types, column_values);
    };

    //type, value
    std::pair<std::string, std::string> get_res() {
        return std::make_pair(head->ret_type, head->ret_value);
    }

private:
    SyntaxTreeNode *build_tree(std::vector<Token *> expr) {
//        std::cout << "NODE: ";
//        for (auto it : expr) {
//            std::cout << it->value << " ";
//        }
//        std::cout << std::endl;

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

//        std::cout << "root:" << std::endl;
//        std::cout << "right" << std::endl;
//        for (auto it : ret_right) {
//            std::cout << it->value << " ";
//        }
//        std::cout << std::endl;
//        std::cout << "left" << std::endl;
//        for (auto it : ret_left) {
//            std::cout << it->value << " ";
//        }
//        std::cout << std::endl;

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

    int find_next_operation(std::vector<Token *> expr) {
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

};

#endif //UNTITLED_MATHEXPRESSION_H
