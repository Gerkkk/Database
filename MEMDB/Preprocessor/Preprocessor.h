#include "../Result/Result.h"

class Preprocessor {
    std::string source;
    std::vector<std::string> queries;
    std::vector<std::pair<std::string, bool>> prom_res;
    bool ok;
    std::string error;

public:
    Preprocessor(std::string &query) {
        this->source = query;
        this->ok = true;
    }

    ~Preprocessor() = default;

    PreprocessorResult * Parse();
    PreprocessorResult * Preprocess();
};