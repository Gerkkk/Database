#include "../Result/Result.h"
#include <algorithm>

//This structure will split the initial string into subqueries and then each query into tokens
class Preprocessor {
    std::string source;
    std::vector<std::string> queries;
    std::vector<std::pair<std::string, int>> prom_res;

    bool ok;
    std::string error;
    int q_subq;

    void process_query(std::string &cur_command, std::vector<int> &flags, int flag);

public:
    explicit Preprocessor(std::string &query);
    ~Preprocessor() = default;
    PreprocessorResult * Parse();
    PreprocessorResult * Preprocess();
};