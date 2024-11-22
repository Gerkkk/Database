#ifndef UNTITLED_TABLE_H
#define UNTITLED_TABLE_H
#include <iostream>
#include <vector>
#include <list>
#include <map>
#include <memory>
#include <set>


class Column {
public:

    std::string name;

    std::string type;
    int max_len;
    std::string default_value;
    std::list<std::string> data;

    bool is_unique, is_autoincrement, is_key;


    Column(std::string &name, std::string &type, std::string &default_value, int max_len, bool is_unique, bool is_autoincrement, bool is_key, std::list<std::string> data);
    Column(std::string &&name, std::string &&type, std::string &&default_value, int max_len, bool is_unique, bool is_autoincrement, bool is_key,  std::list<std::string> &&data);
    Column(const Column &other);

    Column& operator=(const Column& other) // III. copy assignment
    {
        if (this == &other)
            return *this;

        Column temp(other); // use the copy constructor
        std::swap(temp, (*this)); // exchange the underlying resource

        return *this;
    }

    Column(Column&& other)  noexcept {
        this->name = other.name;
        this->type = other.type;
        this->default_value = other.default_value;
        this->max_len = other.max_len;
        this->data = other.data;
    }

    bool is_int32(){
        if (type == "int32") {
            return true;
        } else {
            return false;
        }
    }

    bool is_string(){
        if (type == "string") {
            return true;
        } else {
            return false;
        }
    }

    bool is_bytes(){
        if (type == "bytes") {
            return true;
        } else {
            return false;
        }
    }

    bool is_bool(){
        if (type == "bool") {
            return true;
        } else {
            return false;
        }
    }

    int get_max_len() {
        return this->max_len;
    }

    size_t get_size() {
        return data.size();
    }

    ~Column() = default;
};

class Table {
public:
    std::string name;

    explicit Table(std::string &&name){
        this->name = std::move(name);
        this->columns = {};
        this->col_names = {};
        this->size = 0;
    }

    explicit Table(std::string &name){
        this->name = name;
        this->columns = {};
        this->col_names = {};
        this->size = 0;
    }

    bool add_column(std::shared_ptr<Column> &x) {
        if (columns.empty() || x->get_size() == this->size) {
//            std::cout << "LOL" << x->get_size() << " " << this->size << " ";
            this->size = x->get_size();
//            std::cout << this->size << std::endl;
            col_names.push_back(x->name);
            columns[col_names.back()] = x;
            this->size = x->get_size();
            return true;
        } else {
            return false;
        }
    }

    void print_table() {
        std::cout << this->name << std::endl;
        for (auto const &col_name: col_names) {
            std::cout << col_name << ": ";
            for (auto &it: (columns[col_name])->data) {
                std::cout << it << " ";
            }
            std::cout << std::endl;
        }
    }

    void create_index(std::string &s);
    void save_to_file(std::ofstream &&out);
    void load_from_file(std::ifstream &&in);

    std::map<std::string, std::set<std::pair<std::string, std::vector<std::string>>>> indexes;
    size_t size;
    std::vector<std::string> col_names;
    std::map<std::string, std::shared_ptr<Column>> columns;
};

#endif //UNTITLED_TABLE_H
