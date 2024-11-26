#include "Database.h"

using namespace memdb;


Database::UpdateQuery::UpdateQuery(Database *db, std::vector<Token *> &s, bool need) : Query(db) {
    is_ok = true;

//                for (auto it : s) {
//                    std::cout << it->value << " ";
//                }
//                std::cout << std::endl;
//                for (auto it : s) {
//                    std::cout << it->type << " ";
//                }
//                std::cout << std::endl;

    if (s.size() < 3 || s[0]->type != "update" || s[1]->type != "name" || s[2]->type != "set") {
        is_ok = false;
        err = "Wrong structure of Update query. It should start with \"update\", then further the name of a table, then \"set\"";
    }

    table_name = s[1];

    int r = 3;

    while(r < s.size() && s[r]->type != "where") r++;

    if (r == s.size()) {
        is_ok = false;
        err = "Wrong structure of Update query. It should start with \"update\", then further the name of a table, then \"set\", then assignments and then \"where\"";
    }

    for (int i = r + 1; i < s.size(); i++) {
        conditions.push_back(s[i]);
    }

    int l = 3;

    //std::cout << "HERE " << l << " " << r << std::endl;
    while (l != r) {
        std::string cur_column;
        std::vector<Token *> comm;

        cur_column = s[l]->value;

        l += 2;
        //std::cout << "HERE " << l << " " << r << std::endl;
        while (l != r && s[l]->type != "=") {
            comm.push_back(s[l]);
            l++;
        }

//                    std::cout << "HERE " << l << " " << r << std::endl;

        if (l != r) {
            l -= 2;
            comm.pop_back();
        }

        SyntaxTree ST = SyntaxTree(comm);
        assignment new_as = assignment{cur_column, std::move(ST)};
        assignments.push_back(new_as);

        if (l != r) {
            l++;
        }
        //std::cout << "HERE " << l << " " << r << std::endl;
    }
    //std::cout << "HERE " << l << " " << r << std::endl;

}


QueryResult * Database::UpdateQuery::execute () {
    SyntaxTree st = SyntaxTree(conditions);
    auto res = new QueryResult;

    if (!is_ok) {
        res->ok = false;
        res->error = err;
        return res;
    }

    if (db->tables.find(table_name->value) == db->tables.end()) {
        res->ok = false;
        res->error = "Update query: No such table in database";
        return res;
    } else {
        auto table = db->tables[table_name->value];

        std::map<std::string, std::list<std::string>::iterator> columns_map;
        std::map<std::string, std::string> column_types;

        for(auto it : table->col_names) {
            columns_map[it] = table->columns[it]->data.begin();
            column_types[it] = table->columns[it]->type;
        }

        int counter = 0;

        std::map<std::string, std::map<std::string, std::string>> ct = {make_pair(table->name, column_types)};
        std::map<std::string, std::map<std::string, std::list<std::string>::iterator>> cv = {make_pair(table->name, columns_map)};
        while (counter < table->size) {
            st.execute(ct, cv);
            std::pair<std::string, std::string> st_res = st.get_res();

//                      std::cout << *cv["People"]["names"] << std::endl;
//                      std::cout << st_res.first << " " << st_res.second << std::endl;
            if (st_res.first == "bool" && st_res.second == "true") {
                for (auto assignment: assignments) {
                    //std::cout << assignment.col_name << std::endl;
                    assignment.column_st.execute(ct, cv);
                    std::pair<std::string, std::string> st_res = assignment.column_st.get_res();
                    //std::cout << st_res.first << " " << st_res.second << std::endl;
                    if (st_res.first != table->columns[assignment.col_name]->type) {
                        std::cout << "Update: Update with wrong type" << std::endl;
                    } else if (st_res.first == "string" && st_res.second.size() > table->columns[assignment.col_name]->max_len) {
                        std::cout << "Update query: trying to insert string that is bigger than max len" << std::endl;
                        exit(-1);
                    } else if (st_res.first == "bytes" && (st_res.second.size() % 2 != 0 || (st_res.second.size() - 2) / 2 != table->columns[assignment.col_name]->max_len)) {
                        std::cout << "Update query: trying to insert bytes that is bigger or smaller than len for column or bytes have wrong structure" << std::endl;
                        exit(-1);
                    } else {
                        (*cv[table->name][assignment.col_name]) = st_res.second;
                    }
                    //std::cout << "AAAAAAA" << std::endl;
                }

                for (auto it : columns_map) {
                    columns_map[it.first]++;
                    cv[table->name][it.first]++;
                }
            } else {
                for (auto it : columns_map) {
                    columns_map[it.first]++;
                    cv[table->name][it.first]++;
                }
            }

            counter++;
        }
    }
    return res;
}