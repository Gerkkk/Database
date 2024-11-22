#include "MEMDB/Database/Database.h"
#include "MEMDB/MathExpression//MathExpression.h"

using namespace memdb;

int main() {

    Database db;

    std::shared_ptr<Table> T(new Table("People"));
    //auto ptr = new Column(arg1, arg2, arg4, arg3);
    std::shared_ptr<Column> ptr1(new Column("names", "string", "noname", 100, false, false, false, {"Alex", "Bob", "Cris", "Bob", "Alex", "Alex"}));
    std::shared_ptr<Column> ptr2(new Column("Ages", "int32", "18", 11, false, false, false, {"10", "3", "54", "10", "10", "11"}));
    std::shared_ptr<Column> ptr3(new Column("id", "int32", "-1", 11, false, false, false, {"1", "2", "3", "4", "5", "6"}));
    std::shared_ptr<Column> ptr4(new Column("is_student", "bool", "0", 1, false, false, false, {"true", "false", "true", "true", "false", "true"}));
    T->add_column(ptr1);
    T->add_column(ptr2);
    T->add_column(ptr3);
    T->add_column(ptr4);

    db.add_table(T);

    //Select query1
    std::string s10 = "select names, Ages from People where names = \"Bob\";";
    DBResult dbres1 = db.execute(s10);
    dbres1.data[0].data.print_table();
    std::cout << "===============" << std::endl;


    //Select query2
    s10 = "select names from People where is_student;";
    DBResult dbres2 = db.execute(s10);
    dbres2.data[0].data.print_table();
    std::cout << "===============" << std::endl;

    //Select query3
    s10 = "select names, Ages, is_student from People where Ages % 2 = 0;";
    DBResult dbres3 = db.execute(s10);
    dbres3.data[0].data.print_table();
    std::cout << "===============" << std::endl;


    //Select query4
    s10 = "select id from People where is_student && names = \"Alex\";";
    DBResult dbres4 = db.execute(s10);
    dbres4.data[0].data.print_table();
    std::cout << "===============" << std::endl;


    return 0;
}
