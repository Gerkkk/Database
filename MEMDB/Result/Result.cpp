#include "Result.h"
#include <fstream>

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

void DBResult::save_to_file_readable(std::ofstream &&out) {
    if(out.is_open()) {
        if (!this->is_ok()) {
            out << "There was an error!" << std::endl;
            out << "Error type: " << this->error << std::endl;
            out << "Results that were eventually calculated:" << std::endl;
        } else {
            out << "The query was performed successfully" << std::endl;

            for (int i = 0; i < this->data.size(); i++) {
                out << "====================Result" << std::to_string(i) << "====================" << std::endl;
                if (!data[i].is_ok()) {
                    out << "There was an error:" << data[i].error << std::endl;
                } else {
                    std::ofstream &cur_out = out;
                    data[i].data.save_to_file_readable(std::forward<std::ofstream>(cur_out));
                }
            }
            out << "==============================================" << std::endl;
        }

    } else {
        std::cout << "Trying to write to closed file" << std::endl;
        exit(-1);
    }
}