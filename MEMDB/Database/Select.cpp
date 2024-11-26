#include "Database.h"

using namespace memdb;

//assigns fields of class that are needed for execution. Finds conditions, columns that are searched for, source table
//also handles basic errors
Database::SelectQuery::SelectQuery(Database *db, std::vector<Token *> &s, bool need) : Query(db) {
        is_ok = true;

        bool from_val = false;

        int i = 1;
        for (; i < s.size(); i++) {
            if (s[i]->type == "from") {
                from_val = true;
                break;
            }
            columns.push_back(s[i]);
        }

        if (!from_val) {
            is_ok = false;
            err = "Wrong structure of Select query. Expected word \"from\"";
        }

        if (i + 1 > s.size()) {

            is_ok = false;
            err = "Wrong structure of Select query. No name of the source table.";
        }

        source = s[i + 1];
        i += 2;

        if (i >= s.size() || s[i]->type != "where") {
            is_ok = false;
            err = "Wrong structure of Select query. Expected word \"where\".";
        }

        i++;

        for (; i < s.size(); i++) {
            conditions.push_back(s[i]);
        }

}


//iterating through rows of the source table and checking if the condition is met for each row
QueryResult * Database::SelectQuery::execute() {
    SyntaxTree st = SyntaxTree(conditions);
    auto res = new QueryResult;

    if (!is_ok) {
        res->ok = false;
        res->error = err;
        return res;
    }

    if (db->tables.find(source->value) == db->tables.end()) {
        res->ok = false;
        res->error = "Select query: No such table in database";
        return res;
    } else {
        auto table = db->tables[source->value];

        std::map<std::string, std::list<std::string>::iterator> columns_map;
        std::map<std::string, std::string> column_types;
        for(auto it : columns) {
            col_names.push_back(it->value);
            auto pr_col = table->columns[it->value];

            auto new_col = std::shared_ptr<Column>(new Column(pr_col->name, pr_col->type, pr_col->default_value,
                                                                pr_col->max_len, pr_col->is_unique, pr_col->is_autoincrement,
                                                                pr_col->is_key, {}));
            res->data.add_column(new_col);
        }

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

            if (st_res.first == "bool" && st_res.second == "true") {
                res->data.size++;
                for (auto it : columns_map) {
                    if(std::find(col_names.begin(), col_names.end(), it.first) != col_names.end()) {
                        res->data.columns[it.first]->data.push_back(*it.second);
                    }
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