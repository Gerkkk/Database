#include "../Token/Token.h"
#include "../Table/Table.h"

class Result {
public:
    bool ok;
    std::string error;

    Result() {
        error = "";
        ok = true;
    }

    std::string get_error() {
        return this->error;
    }

    bool is_ok() {
        return this->ok;
    }

};

class PreprocessorResult: public Result {
public:
    std::vector<CommandsLine> data;

    PreprocessorResult(): Result() {
        this->data;
    }
};

class QueryResult: public Result {
public:
    Table data = Table("Result");

    QueryResult(): Result() {}
};

class DBResult: public Result {
public:
    std::vector<QueryResult> data;

    DBResult(): Result() {}
};