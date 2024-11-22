#include "MEMDB/Database/Database.h"
#include "MEMDB/MathExpression//MathExpression.h"

using namespace memdb;

int main() {

    //Insert Query
    std::string s9 = "true && !(false) && true;";
    Preprocessor P = Preprocessor(s9);

    PreprocessorResult *PR = P.Parse();
    std::cout << PR->is_ok() << " " <<  PR->get_error() << " " << PR->data.size() << std::endl;
    std::vector<Token *> res = PR->data[0].commands;
    //std::cout << "ok by now, lol" << std::endl;

    for (auto it : PR->data[0].commands) {
        std::cout << it->type << " ";
    }
    std::cout << std::endl;

    for (auto it : PR->data[0].commands) {
        std::cout << it->value << " ";
    }
    std::cout << std::endl;

    SyntaxTree ST = SyntaxTree(res);
    //std::cout << "good by now" << std::endl;
    ST.execute();
    std::pair<std::string, std::string> output = ST.get_res();
    std::cout << output.first << " " << output.second << std::endl;



    return 0;
}
