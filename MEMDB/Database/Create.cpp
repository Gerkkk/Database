#include "Database.h"

using namespace memdb;

//assigns fields of class that are needed for execution. Finds definitions of all the columns and saves them
Database::CreateQuery::CreateQuery(Database *db, std::vector<Token *> &s, bool need) : Query(db) {
    is_ok = true;

    if (s.size() < 3 || s[0]->type != "create" || s[1]->type != "table" || s[2]->type != "name") {
        is_ok = false;
        err = "Wrong structure of Create query. It should start with \"create table\" and further the name of a new table";
    }

    table_name = s[2];

    int i = 3;

    while (i < s.size() && s[i]->type != ")") {
        Column_description *ptr = new Column_description;
        Column_description cur_col = (*ptr);

        while (s[i]->type == "(") {
            i++;
        }

        while (true) {
            if (s[i]->type == "autoincrement") {
                cur_col.is_autoincrement = true;
            } else if (s[i]->type == "key") {
                cur_col.is_key = true;
            } else if (s[i]->type == "unique") {
                cur_col.is_unique = true;
            } else {
                break;
            }
            i++;
        }


        if (s[i]->type == ")") {
            i++;
        }

        cur_col.name = s[i]->value;
        i += 2;

        if(s[i]->type == "type_name") {
            std::string tn, max_len;

            int j = 0;
            while (j < s[i]->value.size() && s[i]->value[j] != '[') {
                tn.push_back(s[i]->value[j]);
                j++;
            }

            if (j == s[i]->value.size()) {
                max_len = "11";
            } else {
                j++;

                while (s[i]->value[j] != ']') {
                    max_len.push_back(s[i]->value[j]);
                    j++;
                }
            }

            cur_col.max_len = max_len;
            cur_col.type = tn;
        }

        i++;

        if (i < s.size() && s[i]->type == "=") {
            i++;
            cur_col.default_value = s[i]->value;
        }

        i++;

        new_columns.push_back(cur_col);
        delete ptr;
    }
}

//Calling constructor ot table using data parsed in constructor
QueryResult * Database::CreateQuery::execute () {
    auto res = new QueryResult;

    if (!is_ok) {
        res->ok = false;
        res->error = err;
        return res;
    }

    if (db->tables.find(table_name->value) != db->tables.end()) {
        res->ok = false;
        res->error = "Create query: Table with such name already exists";
        return res;
    } else {
        std::shared_ptr<Table> new_table = std::shared_ptr<Table>(new Table(table_name->value));

        for (auto it : new_columns) {
            std::shared_ptr<Column> ptr = std::shared_ptr<Column>(new Column(it.name, it.type, it.default_value, std::stoi(it.max_len), it.is_unique, it.is_autoincrement, it.is_key, {}));
            new_table->add_column(ptr);
        }

        db->add_table(new_table);
    }
    return res;
}