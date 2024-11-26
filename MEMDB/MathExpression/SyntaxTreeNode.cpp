#include "MathExpression.h"

SyntaxTreeNode::SyntaxTreeNode(std::string &inp_value, std::string &inp_type) {
    value = inp_value;
    type = inp_type;
    left_son = nullptr;
    right_son = nullptr;
}

SyntaxTreeNode::~SyntaxTreeNode() {
    if (left_son != nullptr) {
        delete left_son;
    }

    if (right_son != nullptr) {
        delete right_son;
    }
}

void SyntaxTreeNode::execute(std::map<std::string, std::map<std::string, std::string>> &column_types, std::map<std::string, std::map<std::string, std::list<std::string>::iterator>> &column_values) {
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
    } else if (type == "bytes") {
        ret_value = value;
        ret_type = "bytes";
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
        } else if (left_son->ret_type == "string" || left_son->ret_type == "bytes") {
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
    } else if (type == ">=") {
        if (!left_son || !right_son) {
            std::cout << "Greater-or-equals should have 2 operands" << std::endl;
            exit(-1);
        }

        ret_type = "bool";

        if (left_son->ret_type != right_son->ret_type) {
            std::cout << "Greater-or-equals should have 2 operands of the same type" << std::endl;
            exit(-1);
        } else if (left_son->ret_type == "int32"){
            if (std::stoi(left_son->ret_value) >= std::stoi(right_son->ret_value)) {
                ret_value = "true";
            } else {
                ret_value = "false";
            }
        } else if (left_son->ret_type == "string" || left_son->ret_type == "bytes") {
            if (left_son->ret_value.compare(right_son->ret_value) >= 0) {
                ret_value = "true";
            } else {
                ret_value = "false";
            }
        } else if (left_son->ret_type == "bool") {
            if (left_son->ret_value == "true" || right_son->ret_value == "false") {
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
        } else if (left_son->ret_type == "string" || left_son->ret_type == "bytes") {
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
    } else if (type == "<=") {
        if (!left_son || !right_son) {
            std::cout << "Less-or-equal should have 2 operands" << std::endl;
            exit(-1);
        }

        ret_type = "bool";

        if (left_son->ret_type != right_son->ret_type) {
            std::cout << "Less-or-equal should have 2 operands of the same type" << std::endl;
            exit(-1);
        } else if (left_son->ret_type == "int32"){
            if (std::stoi(left_son->ret_value) <= std::stoi(right_son->ret_value)) {
                ret_value = "true";
            } else {
                ret_value = "false";
            }
        } else if (left_son->ret_type == "string" || left_son->ret_type == "bytes") {
            if (left_son->ret_value.compare(right_son->ret_value) <= 0) {
                ret_value = "true";
            } else {
                ret_value = "false";
            }
        } else if (left_son->ret_type == "bool") {
            if (left_son->ret_value == "false" || right_son->ret_value == "true") {
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
    } else if (type == "||") {
        if (!left_son || !right_son) {
            std::cout << "Logical or should have 2 operands" << std::endl;
            exit(-1);
        }

        if (left_son->ret_type != right_son->ret_type) {
            std::cout << "Logical or should have 2 operands of type bool" << std::endl;
            exit(-1);
        } else {
            ret_type = "bool";
            if (left_son->ret_type == "bool") {
                ret_value = ((left_son->ret_value == "true") || (right_son->ret_value == "true")) ? "true" : "false";
            } else {
                std::cout << "Logical or should have 2 operands of type bool" << std::endl;
                exit(-1);
            }
        }
    } else if (type == "^^") {
        if (!left_son || !right_son) {
            std::cout << "Xor should have 2 operands" << std::endl;
            exit(-1);
        }

        if (left_son->ret_type != right_son->ret_type) {
            std::cout << "Xor should have 2 operands of type bool" << std::endl;
            exit(-1);
        } else {
            ret_type = "bool";
            if (left_son->ret_type == "bool") {
                ret_value = ((left_son->ret_value == "true") ^ (right_son->ret_value == "true")) ? "true" : "false";
            } else {
                std::cout << "Xor should have 2 operands of type bool" << std::endl;
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
    } else if (type == "module") {
        if (column_types.size() == 0) {
            std::cout << "Not enough tables for execute" << std::endl;
            exit(-1);
        } else {
            if ((*column_types.begin()).second.find(value) != (*column_types.begin()).second.end()) {
                if ((*column_types.begin()).second[value] != "string" && (*column_types.begin()).second[value] != "bytes") {
                    std::cout << "We only can take module of a string or bytes" << std::endl;
                    exit(-1);
                } else {
                    ret_type = "int32";
                    if ((*column_types.begin()).second[value] == "string") {
                        ret_value = std::to_string((*(*column_values.begin()).second[value]).size());
                    } else {
                        ret_value = std::to_string(((*(*column_values.begin()).second[value]).size() - 2) / 2);
                    }

                }
            } else {
                std::cout << "No column with name " << value << " in table " << (*column_types.begin()).first << std::endl;
                exit(-1);
            }
        }
    }

    //std::cout << "NODE: " << type << " " << value << " Result: " << ret_type << " " << ret_value << std::endl;
}