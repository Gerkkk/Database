#include "../Result/Result.h"

//This structure will split the initial string into subqueries and then each query into tokens
class Preprocessor {
    std::string source;
    std::vector<std::string> queries;
    std::vector<std::pair<std::string, bool>> prom_res;
    bool ok;
    std::string error;

public:
    explicit Preprocessor(std::string &query);
    ~Preprocessor() = default;
    PreprocessorResult * Parse();
    PreprocessorResult * Preprocess();
};