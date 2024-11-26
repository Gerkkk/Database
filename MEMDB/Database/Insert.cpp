#include "Database.h"

using namespace memdb;

//assigns fields of class that are needed for execution. Finds values for each table
//also handles basic errors
//NB: Only one type of insert is implemented: the one, that requires all the columns to be assigned
Database::InsertQuery::InsertQuery(Database *db, std::vector<Token *> &s, bool need) : Query(db) {
        is_ok = true;

        bool end_val = false;

        int i = 1;
        for (; i < s.size(); i++) {
            if (s[i]->type == "to") {
                end_val = true;
                break;
            }

            if(s[i]->type != "(" && s[i]->type != ")") {
                value.push_back(s[i]);
            }
        }

        if (!end_val) {
            is_ok = false;
            err = "Wrong structure of Insert query. No word \"to\"";
        }

        if (is_ok  && i + 1 >= s.size()) {
            is_ok = false;
            err = "Wrong structure of Insert query. No name of the table.";
        }

        table_name = s[i + 1];
}

//iterating through columns and pushing back new values
//Checks sizes of new elements
QueryResult * Database::InsertQuery::execute () {
    auto res = new QueryResult;

    if (!is_ok) {
        res->ok = false;
        res->error = err;
        return res;
    }

    if (db->tables.find(table_name->value) == db->tables.end()) {
        res->ok = false;
        res->error = "Insert query: No such table in database";
        return res;
    } else {

        auto table = db->tables[table_name->value];
        table->size++;
        for (int i = 0; i < table->col_names.size(); i++) {
            //std::cout << value[i]->value << " " << value[i]->type << std::endl;
            if(value[i]->type == "empty_name") {
                if (table->columns[table->col_names[i]]->is_autoincrement) {
                    if (table->columns[table->col_names[i]]->data.size()) {
                        table->columns[table->col_names[i]]->data.push_back(std::to_string(std::stoi(table->columns[table->col_names[i]]->data.back()) + 1));
                    } else {
                        //std::cout << "Yes we can" << std::endl;
                        table->columns[table->col_names[i]]->data.push_back("0");
                    }
                } else {
                    table->columns[table->col_names[i]]->data.push_back(table->columns[table->col_names[i]]->default_value);
                }
            } else {
                if (table->columns[table->col_names[i]]->is_unique || table->columns[table->col_names[i]]->is_key || table->columns[table->col_names[i]]->is_autoincrement) {
                    if (value[i]->type == "string" && value[i]->value.size() > table->columns[table->col_names[i]]->max_len) {
                        std::cout << "trying to insert string that is bigger than max len" << std::endl;
                        exit(-1);
                    }

                    if (value[i]->type == "bytes" && ( value[i]->type.size() % 2 != 0 || (value[i]->value.size() - 2) / 2 != table->columns[table->col_names[i]]->max_len)) {
                        // size() - 2 because of 0x, len of such string should be even because each byte is coded with 2 symbols
                        std::cout << value[i]->type.size() << std::endl;
                        std::cout << "Insert query: trying to insert bytes that is bigger or smaller than len for column or has wrong structure" << std::endl;
                        exit(-1);
                    }

                    if (std::count(table->columns[table->col_names[i]]->data.begin(), table->columns[table->col_names[i]]->data.end(), value[i]->value) > 0) {
                        std::cout << "trying to insert existing value to table with attribute unique/autoincrement/key" << std::endl;
                        exit(-1);
                    } else {
                        table->columns[table->col_names[i]]->data.push_back(value[i]->value);
                    }
                } else if (value[i]->type == "string" && value[i]->value.size() > table->columns[table->col_names[i]]->max_len) {
                    std::cout << "trying to insert string that is bigger than max len" << std::endl;
                    exit(-1);
                } else if (value[i]->type == "bytes" && ( value[i]->value.size() % 2 != 0 || (value[i]->value.size() - 2) / 2 != table->columns[table->col_names[i]]->max_len)) {
                    // size() - 2 because of 0x, len of such string should be even because each byte is coded with 2 symbols
                    std::cout << value[i]->type.size() << std::endl;
                    std::cout << "Insert query: trying to insert bytes that is bigger or smaller than len for column or has wrong structure" << std::endl;
                    exit(-1);
                } else {
                    table->columns[table->col_names[i]]->data.push_back(value[i]->value);
                }
            }
        }
    }
    return res;
}