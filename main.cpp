#include "MEMDB/Database/Database.h"

using namespace memdb;

int main() {
    Database db;

    std::shared_ptr<Table> T(new Table("People"));
    //auto ptr = new Column(arg1, arg2, arg4, arg3);
    std::shared_ptr<Column> ptr1(new Column("names", "string", "noname", 100, false, false, false, {"Alex", "Bob", "Cris"}));
    std::shared_ptr<Column> ptr2(new Column("Ages", "int32", "18", 11, false, false, false, {"10", "3", "54"}));
    std::shared_ptr<Column> ptr3(new Column("id", "int32", "-1", 11, false, false, false, {"1", "2", "3"}));
    std::shared_ptr<Column> ptr4(new Column("is_student", "bool", "0", 1, false, false, false, {"0", "1", "1"}));
    T->add_column(ptr1);
    T->add_column(ptr2);
    T->add_column(ptr3);
    T->add_column(ptr4);

    db.add_table(T);

    //Insert Query
    std::string s9 = "insert (\"Danya\", 19, 4, true) to People;";
    DBResult dbres = db.execute(s9);
    db.print_database();

    //Select query
    std::string s10 = "select names, Ages from People where true;";
    DBResult dbres1 = db.execute(s10);
    dbres1.data[0].data.print_table();


    //Delete query
    std::string s11 = "delete People where true;";
    db.execute(s11);
    db.print_database();

    return 0;
}
