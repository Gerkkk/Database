#ifndef UNTITLED_DATABASE_H
#define UNTITLED_DATABASE_H
#include <fstream>
#include <list>
#include <map>
#include <algorithm>
#include "../Preprocessor/Preprocessor.h"
#include "../Token/Token.h"
#include "../Table/Table.h"
#include "../MathExpression/MathExpression.h"

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
                     } else if (it.commands[0]->type == "delete") {
                         DeleteQuery del = DeleteQuery(this, it.commands, true);
                         qr = del.execute();

                     } else if (it.commands[0]->type == "create") {
                         CreateQuery cr = CreateQuery(this, it.commands, true);
                         qr = cr.execute();
                     } else if (it.commands[0]->type == "update") {
                         UpdateQuery upd = UpdateQuery(this, it.commands, true);
                         qr = upd.execute();
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

                    if(s[i]->type != "(" && s[i]->type != ")") {
                        value.push_back(s[i]);
                    }
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
                                if (std::count(table->columns[table->col_names[i]]->data.begin(), table->columns[table->col_names[i]]->data.end(), value[i]->value) > 0) {
                                    std::cout << "trying to insert existing value to table with attribute unique/autoincrement/key" << std::endl;
                                    exit(-1);
                                } else {
                                    table->columns[table->col_names[i]]->data.push_back(value[i]->value);
                                }
                            } else {
                                table->columns[table->col_names[i]]->data.push_back(value[i]->value);
                            }
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

                i++;

                for (; i < s.size(); i++) {
                    conditions.push_back(s[i]);
                }

            }

            QueryResult *execute () override {
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

//                        std::cout << *cv["People"]["names"] << std::endl;
//                        std::cout << st_res.first << " " << st_res.second << std::endl;
                        if (st_res.first == "bool" && st_res.second == "true") {
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

                i++;

                for (; i < s.size(); i++) {
                    conditions.push_back(s[i]);
                }

            }

            QueryResult *execute () override {
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
                        column_types[it] = table->columns[it]->type;
                    }

                    int counter = 0;


                    std::map<std::string, std::map<std::string, std::string>> ct = {make_pair(table->name, column_types)};
                    std::map<std::string, std::map<std::string, std::list<std::string>::iterator>> cv = {make_pair(table->name, columns_map)};

                    while (counter < table->size) {
//                        for (auto it : columns_map) {
//                            //add condition for delete
//                            columns_map[it.first] = table->columns[it.first]->data.erase(it.second);
//                        }
//
//                        counter++;
//
                        //add math expression checker
                        st.execute(ct, cv);
                        std::pair<std::string, std::string> st_res = st.get_res();

//                        std::cout << *cv["People"]["names"] << std::endl;
//                        std::cout << st_res.first << " " << st_res.second << std::endl;
                        if (st_res.first == "bool" && st_res.second == "true") {
                            for (auto it : columns_map) {
                                columns_map[it.first] = table->columns[it.first]->data.erase(it.second);
                                cv[table->name][it.first] = columns_map[it.first];
                            }
                            table->size--;
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
        };


        class CreateQuery : public Query {
        public:
            bool is_ok;
            std::vector<Token *> value;
            Token *table_name;

            std::string err;

            struct Column_description {
                std::string name;

                std::string type;
                std::string max_len;
                std::string default_value;
                std::list<std::string> data;

                bool is_unique = false, is_autoincrement = false, is_key = false;
            };

            std::vector<Column_description> new_columns;


            CreateQuery(Database *db, std::vector<Token *> s, bool need) : Query(db) {
                is_ok = true;

//                for (auto it : s) {
//                    std::cout << it->value << " ";
//                }
//                std::cout << std::endl;
//                for (auto it : s) {
//                    std::cout << it->type << " ";
//                }
//                std::cout << std::endl;

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
                            //std::cout << "AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA" << std::endl;
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
//                    std::cout << "@@@@@" << std::endl;
//                    std::cout << cur_col.name << "\n" << cur_col.type <<  "\n" << cur_col.default_value << "\n";
//                    std::cout << cur_col.max_len << "\n" << cur_col.is_key << " " << cur_col.is_autoincrement << " " << cur_col.is_unique << "\n";
//                    std::cout << "@@@@@" << std::endl;

                    new_columns.push_back(cur_col);
                    delete ptr;
                }
            }

            QueryResult *execute () override {
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
        };


        class UpdateQuery : public Query {
        public:
            bool is_ok;
            std::string err;

            Token *table_name;
            std::vector<Token *> conditions;

            struct assignment {
                std::string col_name;
                SyntaxTree column_st;

                assignment(std::string &col, SyntaxTree &&columnSt) : column_st(columnSt) {
                    col_name = col;
                }

                ~assignment() = default;
            };

            std::vector<assignment> assignments;


            UpdateQuery(Database *db, std::vector<Token *> s, bool need) : Query(db) {
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

            QueryResult *execute () override {
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
        };


        ~Database() = default;
    };
}

#endif //UNTITLED_DATABASE_H
