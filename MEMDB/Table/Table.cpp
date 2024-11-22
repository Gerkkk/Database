#include "Table.h"
#include <fstream>
#include <utility>

Column::Column(std::string &name, std::string &type, std::string &default_value, int max_len=11, bool is_unique=false, bool is_autoincrement=false, bool is_key=false, std::list<std::string> data = {}) {
    this->name = name;
    this->type = type;
    this->default_value = default_value;
    this->data = std::move(data);

    this->is_unique = is_unique;
    this->is_autoincrement = is_autoincrement;
    this->is_key = is_key;

    if (type == "bool") {
        this->max_len = 1;
    } else if (type == "int32") {
        this->max_len = 11;
    } else {
        this->max_len = max_len;
    }
}

Column::Column(std::string &&name, std::string &&type, std::string &&default_value, int max_len = 11, bool is_unique=false, bool is_autoincrement=false, bool is_key=false, std::list<std::string> &&data = {}) {
    this->name = name;
    this->type = type;
    this->default_value = default_value;
    this->data = std::move(data);

    this->is_unique = is_unique;
    this->is_autoincrement = is_autoincrement;
    this->is_key = is_key;

    if (type == "bool") {
        this->max_len = 1;
    } else if (type == "int32") {
        this->max_len = 11;
    } else {
        this->max_len = max_len;
    }
}


Column::Column(const Column& other) {
    this->name = other.name;
    this->type = other.type;
    this->default_value = other.default_value;
    this->data = other.data;
    this->max_len = other.max_len;
}

void Table::create_index(std::string &s) {
    //Finish later
    this->indexes[s] = {};
}


void Table::save_to_file(std::ofstream &&out) {
    if(out.is_open()) {
        out << this->name << " " << this->size << " " << this->col_names.size() << " ";

        for (auto const &col_name: col_names) {
            out << columns[col_name]->name << " " << columns[col_name]->type << " " << std::to_string(columns[col_name]->max_len) << " " << columns[col_name]->default_value << " ";
            for (auto &it: (columns[col_name])->data) {
                out << it << " ";
            }
        }

        out << indexes.size() << " ";
        for (const auto &it: indexes) {
            out << it.first << " ";
        }

    } else {
        std::cout << "Trying to write to closed file" << std::endl;
        exit(-1);
    }
}


void Table::load_from_file(std::ifstream &&in) {
    if(in.is_open()) {
        int c;
        in >> this->name >> this->size >> c;

        for (int i = 0; i < c; i++) {
            std::string col_name, type, max_len, default_value;
            in >> col_name >> type >> max_len >> default_value;

//            std::cout << col_name << " " << type << " " << max_len << " " << default_value << " " << this->size <<  std::endl;
            std::shared_ptr<Column> cur(new Column(col_name, type, default_value, std::stoi(max_len)));
            std::list<std::string> cur_data;
            std::string v;
            for (int j = 0; j < this->size; j++) {
                in >> v;
                cur_data.push_back(v);
            }
            cur->data = cur_data;
            this->col_names.push_back(col_name);
            this->columns[col_name] = cur;
//            std::cout << "-----" << std::endl;
//
//            std::cout << (cur_data).size() <<  " " << this->size << std::endl;
//            std::cout << ((columns[col_name])->data).size() << " "<< c << std::endl;
//            for (auto &it: (columns[cur->name])->data) {
//                std::cout << it << " ";
//            }
        }

        int index_count;
        std::string index_col_name;
        in >> index_count;

        for (int i = 0; i < index_count; i++) {
            in >> index_col_name;
            create_index(index_col_name);
        }

    } else {
        std::cout << "Trying to read from closed file" << std::endl;
        exit(-1);
    }
}