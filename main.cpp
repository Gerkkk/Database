#include "MEMDB/Database/Database.h"
#include "MEMDB/MathExpression//MathExpression.h"

using namespace memdb;

int main() {

    Database db;


    //Create query1
    std::string s8 = "create table People ((unique) names : string[32] = noname, Ages : int32 = 18, (autoincrement) id : int32 = -2, is_student : bool = false);";
    DBResult dbres = db.execute(s8);
    std::cout << dbres.get_error() << std::endl;
    db.print_database();

    //Insert query1
    std::string s9 = "insert (\"Alex\", 19, , true) to People;";
    DBResult dbres0 = db.execute(s9);
    std::cout << dbres0.get_error() << std::endl;
    db.print_database();

    //Insert query2
    std::string s10 = "insert (\"Bob\", 19, , false) to People;";
    DBResult dbres1 = db.execute(s10);
    std::cout << dbres0.get_error() << std::endl;
    db.print_database();

    //Update query
    std::string s11 = "update People set names = names + \"_stud\", Ages = 123 where is_student;";
    DBResult dbres2 = db.execute(s11);
    std::cout << dbres2.get_error() << std::endl;
    db.print_database();

//    //Insert query3 (returns error because of not unique name
//    std::string s11 = "insert (\"Alex\", 19, , false) to People;";
//    DBResult dbres3 = db.execute(s10);
//    std::cout << dbres0.get_error() << std::endl;
//    db.print_database();
//    std::cout << "===============" << std::endl;



    return 0;
}
