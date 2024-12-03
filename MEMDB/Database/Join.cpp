#include "Database.h"

using namespace memdb;

//assigns fields of class that are needed for execution. Finds conditions, columns that are searched for, source table
//also handles basic errors
Database::JoinQuery::JoinQuery(Database *db, std::vector<Token *> &s, bool need) : Query(db) {
    is_ok = true;
    bool from_val = false;

    if (s.size() < 5) {
        is_ok = false;
        err = "Wrong structure of Join query. Must be at least 5 words";
    }

    if (s[0]->type != "name") {
        is_ok = false;
        err = "Wrong structure of Join query. Must start with the name of first table";
    } else {
        tables.push_back(s[0]);
    }

    if (s[1]->type != "join") {
        is_ok = false;
        err = "Wrong structure of Join query. No word \"join\"";
    }

    if (s[2]->type != "name") {
        is_ok = false;
        err = "Wrong structure of Join query. Must have 2 names of tables";
    } else {
        tables.push_back(s[2]);
    }

    if (s[3]->type != "on") {
        is_ok = false;
        err = "Wrong structure of Join query. Must have word \"on\"";
    }

    for (int i = 4; i < s.size(); i++) {
        conditions.push_back(s[i]);
    }
}


//iterating through rows of the source table and checking if the condition is met for each row
QueryResult * Database::JoinQuery::execute() {

    SyntaxTree st = SyntaxTree(conditions);
    auto res = new QueryResult;

    if (!is_ok) {
        res->ok = false;
        res->error = err;
        return res;
    }

    if ((db->tables.find(tables[0]->value) == db->tables.end() && db->help_tables.find(tables[0]->value) == db->help_tables.end())
        || (db->tables.find(tables[1]->value) == db->tables.end() && db->help_tables.find(tables[1]->value) == db->help_tables.end())) {
        res->ok = false;
        res->error = "Join query: No such table in database";
        return res;
    } else {

        std::shared_ptr<Table> table1, table2;

        if (db->tables.find(tables[0]->value) == db->tables.end()) {
            table1 = db->help_tables[tables[0]->value];
        } else {
            table1 = db->tables[tables[0]->value];
        }

        if (db->tables.find(tables[1]->value) == db->tables.end()) {
            table2 = db->help_tables[tables[1]->value];
        } else {
            table2 = db->tables[tables[1]->value];
        }

        std::map<std::string, std::list<std::string>::iterator> columns_map1, columns_map2;
        std::map<std::string, std::string> column_types1, column_types2;
        for(auto it : table1->col_names) {
            std::string temp = table1->name + "_" + it;
            columns_map1[temp] = table1->columns[it]->data.begin();
            column_types1[temp] = table1->columns[it]->type;
            auto pr_col = table1->columns[it];

            auto new_col = std::shared_ptr<Column>(new Column(temp, pr_col->type, pr_col->default_value,
                                                              pr_col->max_len, pr_col->is_unique, pr_col->is_autoincrement,
                                                              pr_col->is_key, {}));
            res->data.add_column(new_col);
        }

        for(auto it : table2->col_names) {
            std::string temp = table2->name + "_" + it;
            columns_map2[temp] = table2->columns[it]->data.begin();
            column_types2[temp] = table2->columns[it]->type;
            auto pr_col = table2->columns[it];

            auto new_col = std::shared_ptr<Column>(new Column(temp, pr_col->type, pr_col->default_value,
                                                              pr_col->max_len, pr_col->is_unique, pr_col->is_autoincrement,
                                                              pr_col->is_key, {}));
            res->data.add_column(new_col);
        }


        int counter1 = 0, counter2 = 0;

        std::map<std::string, std::map<std::string, std::string>> ct = {make_pair(table1->name, column_types1), make_pair(table2->name, column_types2)};
        std::map<std::string, std::map<std::string, std::list<std::string>::iterator>> cv = {make_pair(table1->name, columns_map1), make_pair(table2->name, columns_map2)};

        while (counter1 < table1->size) {
            while (counter2 < table2->size) {
                st.execute(ct, cv);
                std::pair<std::string, std::string> st_res = st.get_res();

                if (st_res.first == "bool" && st_res.second == "true") {
                    res->data.size++;
                    for (auto it : columns_map2) {
                        res->data.columns[it.first]->data.push_back(*it.second);
                        columns_map2[it.first]++;
                        cv[table2->name][it.first]++;
                    }

                    for (auto it : columns_map1) {
                        res->data.columns[it.first]->data.push_back(*it.second);
                    }
                } else {
                    for (auto it : columns_map2) {
                        columns_map2[it.first]++;
                        cv[table2->name][it.first]++;
                    }
                }

                counter2++;
            }

            for (auto it : columns_map2) {
                std::string cname;
                int i = 0;
                while(it.first[i] != '_') {
                    i++;
                }

                i++;

                while(i < it.first.size()) {
                    cname.push_back(it.first[i]);
                    i++;
                }

                columns_map2[it.first] = table2->columns[cname]->data.begin();

                cv[table2->name][it.first] = table2->columns[cname]->data.begin();
            }

            for (auto it : columns_map1) {
                columns_map1[it.first]++;
                cv[table1->name][it.first]++;
            }

            counter2 = 0;
            counter1++;
        }
    }
    return res;
}