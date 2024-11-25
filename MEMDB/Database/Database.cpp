#include "Database.h"


using namespace memdb;

void Database::add_table(std::shared_ptr<Table> &x) {
    table_names.push_back(x->name);
    tables[x->name] = x;
}

void Database::save_to_file(std::ofstream &&out) {
    if(out.is_open()) {
        out << tables.size() << " ";

        for (auto it : tables) {
            std::basic_ofstream<char> &cur_out = out;
            it.second->save_to_file(std::forward<std::basic_ofstream<char>>(cur_out));
        }

    } else {
        std::cout << "Trying to write to closed file" << std::endl;
        exit(-1);
    }
}

void Database::load_from_file(std::ifstream &&in) {
    if(in.is_open()) {
        int c;
        in >> c;

        for (int i = 0; i < c; i++) {
            std::basic_ifstream<char> &cur_in = in;
            std::shared_ptr<Table> cur_table(new Table(""));
            cur_table->load_from_file(std::forward<std::basic_ifstream<char>>(cur_in));
            add_table(cur_table);
        }
    } else {
        std::cout << "Trying to write to closed file" << std::endl;
        exit(-1);
    }
}

void Database::print_database() {
    std::cout << "=======Database Print=======" << std::endl;

    int i = 0;
    for (auto it: tables) {
        std::cout << "-------Table " << i << "-------" << std::endl;
        (it.second)->print_table();
        i++;
    }

    std::cout << "============================" << std::endl;
}
