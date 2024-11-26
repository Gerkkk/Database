#include "Database.h"

using namespace memdb;

//assigns fields of class that are needed for execution. Finds conditions, source table
//also handles basic errors
Database::DeleteQuery::DeleteQuery(Database *db, std::vector<Token *> &s, bool need) : Query(db) {
        is_ok = true;

        int i = 1;

        if (i > s.size()) {
            is_ok = false;
            err = "Wrong structure of Delete query. No name of the source table.";
        }

        source = s[i];

        i += 1;

        if (i >= s.size() || s[i]->type != "where") {
            is_ok = false;
            err = "Wrong structure of Delete query. Expected word \"where\".";
        }

        i++;

        for (; i < s.size(); i++) {
            conditions.push_back(s[i]);
        }
}



//iterating through rows of the source table and deleting those that meet conditions
//the signature of erase of std::list, which returns iterator to next value after the deleted one, is very helpful
QueryResult * Database::DeleteQuery::execute () {
    SyntaxTree st = SyntaxTree(conditions);

    auto res = new QueryResult;

    if (!is_ok) {
        res->ok = false;
        res->error = err;
        return res;
    }

    if (db->tables.find(source->value) == db->tables.end()) {
        res->ok = false;
        res->error = "Delete query: No such table in database";
        return res;
    } else {
        auto table = db->tables[source->value];

        std::map<std::string, std::list<std::string>::iterator> columns_map;
        std::map<std::string, std::string> column_types;

        for(auto it : table->col_names) {
            columns_map[it] = table->columns[it]->data.begin();
            column_types[it] = table->columns[it]->type;
        }

        int counter = 0;


        std::map<std::string, std::map<std::string, std::string>> ct = {make_pair(table->name, column_types)};
        std::map<std::string, std::map<std::string, std::list<std::string>::iterator>> cv = {make_pair(table->name, columns_map)};

        int delta = 0;
        while (counter < table->size) {

            st.execute(ct, cv);
            std::pair<std::string, std::string> st_res = st.get_res();

            if (st_res.first == "bool" && st_res.second == "true") {
                for (auto it : columns_map) {
                    columns_map[it.first] = table->columns[it.first]->data.erase(it.second);
                    cv[table->name][it.first] = columns_map[it.first];
                }
                delta++;
            } else {
                for (auto it : columns_map) {
                    columns_map[it.first]++;
                    cv[table->name][it.first]++;
                }
            }
            counter++;
        }

        table->size -= delta;
    }
    return res;
}