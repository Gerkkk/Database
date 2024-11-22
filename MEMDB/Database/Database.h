#ifndef UNTITLED_DATABASE_H
#define UNTITLED_DATABASE_H
#include <fstream>
#include <list>
#include <map>
#include "../Preprocessor/Preprocessor.h"
#include "../Token/Token.h"
#include "../Table/Table.h"

namespace memdb {
    class Database {
    public:
        std::vector<std::string> table_names;
        std::map<std::string, std::shared_ptr<Table>> tables;

        Database() = default;

        void add_table(std::shared_ptr<Table> &x);

        void save_to_file(std::ofstream &&out);

        void load_from_file(std::ifstream &&in);

        void print_database();

         DBResult execute(std::string &query) {
             auto db_res = new DBResult();
             Preprocessor P = Preprocessor(query);
             PreprocessorResult *p_res = P.Parse();
             std::vector<QueryResult> prom_res;

             if (!p_res->is_ok()) {
                 db_res->ok = false;
                 db_res->error = p_res->error;
                 return *db_res;
             }  else {
                 for (auto it : p_res->data) {
                     QueryResult *qr;
                     if (it.commands[0]->type == "select") {
                         SelectQuery sel = SelectQuery(this, it.commands, true);
                         qr = sel.execute();
                     } else if (it.commands[0]->type == "insert") {
                         InsertQuery ins = InsertQuery(this, it.commands, true);
                         qr = ins.execute();
                     }  else if (it.commands[0]->type == "delete") {
                         DeleteQuery del = DeleteQuery(this, it.commands, true);
                         qr = del.execute();
                     } else {
                         db_res->ok = false;
                         db_res->error = "Uknown command";
                     }

                     if (!qr->is_ok()) {
                         db_res->error = qr->error;
                         db_res->ok = false;
                     } else {
                         prom_res.push_back(*qr);
                     }
                 }

                 db_res->data = prom_res;
                 return *db_res;
             }
        }

        class Query {
        public:
            Database *db;

            explicit Query(Database *db) {
                this->db = db;
            };

            virtual QueryResult * execute() = 0;

            ~Query() = default;
        };

//in fact, we also have to save here stack of recent results for subqueries. but
        class InsertQuery : public Query {
        public:
            bool is_ok;
            std::vector<Token *> value;
            Token *table_name;

            std::string err;


            InsertQuery(Database *db, std::vector<Token *> s, bool need) : Query(db) {
                is_ok = true;

                bool end_val = false;

                int i = 1;
                for (; i < s.size(); i++) {
                    if (s[i]->type == "to") {
                        end_val = true;
                        break;
                    }
                    value.push_back(s[i]);
                }



                if (!end_val) {
                    is_ok = false;
                    err = "Wrong structure of Insert query. No word \"to\"";
                }

                if (i + 1 > s.size()) {
                    is_ok = false;
                    err = "Wrong structure of Insert query. No name of the table.";
                }

                table_name = s[i + 1];
            }

            QueryResult *execute () override {
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
                        if(value[i]->type == "empty_name") {
                            table->columns[table->col_names[i]]->data.push_back(table->columns[table->col_names[i]]->default_value);
                        } else {
                            //check for type, autoincrement and so on
                            table->columns[table->col_names[i]]->data.push_back(value[i]->value);
                        }
                    }
                }
                return res;
            }
        };

        class SelectQuery : public Query {
        public:
            bool is_ok;
            std::vector<Token *> columns;
            std::vector<std::string> col_names;
            Token * source;
            std::vector<Token *> conditions;

            std::string err;


            SelectQuery(Database *db, std::vector<Token *> s, bool need) : Query(db) {
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

                for (; i < s.size(); i++) {
                    conditions.push_back(s[i]);
                }

            }

            QueryResult *execute () override {
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
                    }

                    int counter = 0;

                    while (counter < table->size) {
                        //add math expression checker
                        for (auto it : columns_map) {
                            if(std::find(col_names.begin(), col_names.end(), it.first) != col_names.end()) {
                                res->data.columns[it.first]->data.push_back(*it.second);
                            }
                            columns_map[it.first]++;
                        }

                        counter++;
                    }
                }
                return res;
            }
        };


        class DeleteQuery : public Query {
        public:
            bool is_ok;

            Token * source;
            std::vector<Token *> conditions;

            std::string err;


            DeleteQuery(Database *db, std::vector<Token *> s, bool need) : Query(db) {
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

                for (; i < s.size(); i++) {
                    conditions.push_back(s[i]);
                }

            }

            QueryResult *execute () override {
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
//                for(auto it : columns) {
//                    col_names.push_back(it->value);
//                    auto pr_col = table->columns[it->value];
//
//                    auto new_col = std::shared_ptr<Column>(new Column(pr_col->name, pr_col->type, pr_col->default_value,
//                                                                      pr_col->max_len, pr_col->is_unique, pr_col->is_autoincrement,
//                                                                      pr_col->is_key, {}));
//                    res->data.add_column(new_col);
//                }

                    for(auto it : table->col_names) {
                        columns_map[it] = table->columns[it]->data.begin();
                    }

                    int counter = 0;

                    while (counter < table->size) {
                        for (auto it : columns_map) {
                            //add condition for delete
                            columns_map[it.first] = table->columns[it.first]->data.erase(it.second);
                        }

                        counter++;
                    }
                }
                return res;
            }
        };


        ~Database() = default;
    };
}

#endif //UNTITLED_DATABASE_H
