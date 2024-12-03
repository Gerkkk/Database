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
    //Database is just a map of tables
    class Database {
    public:
        std::vector<std::string> table_names;
        std::map<std::string, std::shared_ptr<Table>> tables;
        std::map<std::string, std::shared_ptr<Table>> help_tables;

        Database() = default;
        ~Database() = default;

        void add_table(std::shared_ptr<Table> &x);
        void save_to_file(std::ofstream &&out);
        void load_from_file(std::ifstream &&in);
        void print_database();

        DBResult execute(std::ifstream &&in);

        DBResult execute(std::string &query);

    private:
        //Base class for queries. The main property is the execute method.
        //in fact, we also have to save here stack of recent results for subqueries.
        //But we will deal with it later, when we will implement subqueries.
        class Query {
        public:
            Database *db;

            explicit Query(Database *db) {
                this->db = db;
            };

            virtual QueryResult * execute() = 0;
            ~Query() = default;
        };

        //Here are all the types of queries. They are almost the same.
        //Some interesting moments are commented
        class InsertQuery : public Query {
        public:
            bool is_ok;
            std::vector<Token *> value;
            Token *table_name;
            std::string err;

            InsertQuery(Database *db, std::vector<Token *> &s, bool need);

            QueryResult *execute () override;
        };

        class SelectQuery : public Query {
        public:
            bool is_ok;
            std::vector<Token *> columns;
            std::vector<std::string> col_names;
            Token * source;
            std::vector<Token *> conditions;
            std::string err;

            SelectQuery(Database *db, std::vector<Token *> &s, bool need);
            QueryResult *execute () override;
        };


        class DeleteQuery : public Query {
        public:
            bool is_ok;
            Token * source;
            std::vector<Token *> conditions;
            std::string err;

            DeleteQuery(Database *db, std::vector<Token *> &s, bool need);
            QueryResult *execute () override;
        };


        class CreateQuery : public Query {
        public:
            bool is_ok;
            std::vector<Token *> value;
            Token *table_name;
            std::string err;

            //Structure for convenient storage of information about new column
            struct Column_description {
                std::string name;
                std::string type;
                std::string max_len;
                std::string default_value;
                std::list<std::string> data;

                bool is_unique = false, is_autoincrement = false, is_key = false;
            };

            std::vector<Column_description> new_columns;

            CreateQuery(Database *db, std::vector<Token *> &s, bool need);
            QueryResult *execute () override;
        };


        class UpdateQuery : public Query {
        public:
            bool is_ok;
            std::string err;
            Token *table_name;
            std::vector<Token *> conditions;

            //structure for assignments of update. Stores Syntax tree for math expression which describes assignment
            //and name of column that we change
            struct assignment {
                std::string col_name;
                SyntaxTree column_st;

                assignment(std::string &col, SyntaxTree &&columnSt) : column_st(columnSt) {
                    col_name = col;
                }

                ~assignment() = default;
            };

            std::vector<assignment> assignments;

            UpdateQuery(Database *db, std::vector<Token *> &s, bool need);
            QueryResult *execute () override;
        };

        class JoinQuery : public Query {
        public:
            bool is_ok;
            std::vector<Token *> tables;
            std::vector<Token *> conditions;
            std::string err;

            JoinQuery(Database *db, std::vector<Token *> &s, bool need);
            QueryResult *execute () override;
        };

    };
}

#endif //UNTITLED_DATABASE_H
