#include "Result.h"

std::string Result::get_error() const {
    return this->error;
}

bool Result::is_ok() const {
    return this->ok;
}

Result::Result() {
    error = "";
    ok = true;
}

PreprocessorResult::PreprocessorResult(): Result() {}
QueryResult::QueryResult() : Result() {}
DBResult::DBResult(): Result() {}