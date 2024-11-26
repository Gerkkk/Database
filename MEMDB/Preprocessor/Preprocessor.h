#include "../Result/Result.h"

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