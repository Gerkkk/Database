#include "../Token/Token.h"
#include "../Table/Table.h"


//Base class for all the results
class Result {
public:
    bool ok;
    std::string error;

    Result();
    std::string get_error() const;
    bool is_ok() const;

};

//Preprocessor will return CommandsLine for every query into which Preprocessor have split the query
class PreprocessorResult: public Result {
public:
    std::vector<CommandsLine> data;
    PreprocessorResult();
};

//Execute of each Query will return Table named results
class QueryResult: public Result {
public:
    Table data = Table("Result");
    QueryResult();
};

//Execute of DB queries will return vector of Query results for each of the queries in
//user's queries
class DBResult: public Result {
public:
    std::vector<QueryResult> data;
    DBResult();

    void save_to_file_readable(std::ofstream &&out);
};