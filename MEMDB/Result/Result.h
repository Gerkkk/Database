#include "../Token/Token.h"
#include "../Table/Table.h"

class Result {
public:
    bool ok;
    std::string error;

    Result();
    std::string get_error() const;
    bool is_ok() const;

};

class PreprocessorResult: public Result {
public:
    std::vector<CommandsLine> data;
    PreprocessorResult();
};

class QueryResult: public Result {
public:
    Table data = Table("Result");
    QueryResult();
};

class DBResult: public Result {
public:
    std::vector<QueryResult> data;
    DBResult();
};