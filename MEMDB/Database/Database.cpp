#include "Database.h"

using namespace memdb;

void Database::add_table(std::shared_ptr<Table> &x) {
    table_names.push_back(x->name);
    tables[x->name] = x;
}


//Uses Table's save_to_file, so it is rather simple
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

//Uses Table's load_from_file so is rather simple
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
        std::cout << "Trying to read from closed file" << std::endl;
        exit(-1);
    }
}

//For convenient and beautiful debug
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


//All these ifs are bad. Really bad. In ideal world we would have an abstract factory for queries
//but I only had time for the easiest solution
DBResult Database::execute(std::string &query) {
    auto db_res = new DBResult();
    Preprocessor P = Preprocessor(query);
    PreprocessorResult *p_res = P.Parse();
    std::vector<QueryResult> prom_res;
    this->help_tables = {};

    if (!p_res->is_ok()) {
        db_res->ok = false;
        db_res->error = p_res->error;
        return *db_res;
    }  else {
        for (auto it : p_res->data) {
            QueryResult *qr;

//            for (auto it1: it.commands) {
//                std::cout << it1->value << " ";
//            }
//            std::cout << std::endl;

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
            } else if (it.commands.size() > 1 && it.commands[1]->type == "join") {
                JoinQuery jqd = JoinQuery(this, it.commands, true);
                qr = jqd.execute();
            }  else {
                qr = new QueryResult();
                qr->ok = false;
                qr->error = "Executor: Uknown command, which starts with " + it.commands[0]->value;
            }

//            qr->data.print_table();
//            std::cout << "^^^^^?????!!!!!!!" << it.flag << std::endl;

            if (!qr->is_ok()) {
                db_res->error = qr->error;
                db_res->ok = false;
                break;
            } else if (it.flag == 0){
                prom_res.push_back(*qr);
            } else {
                std::string temp = "_PREV" + std::to_string(it.flag);
                this->help_tables.insert(std::make_pair(temp, std::make_shared<Table>(qr->data)));
            }
        }

        db_res->data = prom_res;
        return *db_res;
    }
}

DBResult Database::execute(std::ifstream &&in) {
    if(in.is_open()) {
        std::string query, all = "", newall = "";

        while (std::getline(in, query)) {
            newall = all + query;
            all = newall;
            if(!all.empty() && all[all.size() - 1] != ';') {
                all.pop_back();
                all.push_back(' ');
            }
            newall = "";
            query = "";

        }

        return this->execute(all);
    } else {
        std::cout << "Trying to read from closed file" << std::endl;
        exit(-1);
    }
}