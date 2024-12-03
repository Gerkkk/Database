#include <gtest/gtest.h>
#include "../MEMDB/Database/Database.h"
#include <ctime>
#include <fstream>

using namespace memdb;

TEST(PerfomanceTests, Test1) {
Database db;

const std::clock_t c_start = std::clock();

    //Checking column flags. Also checking that names of tables and columns are sensitive to register
    std::string s = "create table People (name : string[32] = noname, Age : int32 = 18, (autoincrement) id : int32 = -2, is_student : bool = false);";
    db.execute(s);

    const std::clock_t c_1 = std::clock();

    std::cout << std::fixed << std::setprecision(2) << "Creation of table took " << 1000 * (c_1 - c_start) / CLOCKS_PER_SEC << "ms" << std::endl;

    for (int i = 0; i < 100; i++) {
        s = "insert (\"Alex\", 19, , true) to People;";
        db.execute(s);
    }
    const std::clock_t c_2 = std::clock();
    std::cout << std::fixed << std::setprecision(2) << "100 inserts took " << 1000 * (c_2 - c_1) / CLOCKS_PER_SEC << "ms" << std::endl;
    EXPECT_EQ(db.tables["People"]->size, 100);


    //Select of the whole table
    s = "select name, Age from People where true;";
    DBResult dbres = db.execute(s);

    const std::clock_t c_3 = std::clock();
    std::cout << std::fixed << std::setprecision(2) << "Select of 100 rows took " << 1000 * (c_3 - c_2) / CLOCKS_PER_SEC << "ms" << std::endl;
    auto sz = dbres.data[0].data.size;
    EXPECT_EQ(sz, 100);

    const std::clock_t c_4 = std::clock();
    //Update with simple condition. Checking that update does not change table size
    s = "update People set is_student = true where true;";
    dbres = db.execute(s);
    const std::clock_t c_5 = std::clock();
    std::cout << std::fixed << std::setprecision(2) << "Update of 100 rows took " << 1000 * (c_5 - c_4) / CLOCKS_PER_SEC << "ms" << std::endl;
    EXPECT_EQ(db.tables["People"]->size, 100);


    //Delete of the whole table
    const std::clock_t c_6 = std::clock();
    s = "delete People where true;";
    db.execute(s);
    EXPECT_EQ(db.tables["People"]->size, 0);

    const std::clock_t c_7 = std::clock();
    std::cout << std::fixed << std::setprecision(2) << "Delete of 100 rows took " << 1000 * (c_7 - c_6) / CLOCKS_PER_SEC << "ms" << std::endl;
}


TEST(QueryTests, Test1) {
    Database db;

    //Checking column flags. Also checking that names of tables and columns are sensitive to register
    std::string s = "create table People ((unique) name : string[10] = noname, Age : int32 = 18, (autoincrement) id : int32 = -2, is_student : bool = false);";
    db.execute(s);
    //Checking correct work of unique and autoincrement, types
    s = "insert (\"Alex\", 19, , true) to People;";
    db.execute(s);
    s = "insert (\"Bob\", 19, , false) to People;";
    db.execute(s);

    s = "insert (\"Chris\", 9, , false) to People;";
    db.execute(s);

    s = "insert (\"Daemon\", 45, , false) to People;";
    db.execute(s);

    s = "insert (\"Eva\", 25, , true) to People;";
    db.execute(s);

    s = "insert (\"Frank\", 10, , true) to People;";
    db.execute(s);

    EXPECT_EQ(db.tables["People"]->size, 6);
    EXPECT_EQ(db.tables["People"]->columns["name"]->data.back(), "Frank");

    //Select of the whole table
    s = "select name, Age from People where true;";
    DBResult dbres = db.execute(s);
    auto sz = dbres.data[0].data.size;
    EXPECT_EQ(sz, 6);

    //Empty Select
    s = "select name, Age from People where false;";
    dbres = db.execute(s);
    sz = dbres.data[0].data.size;
    EXPECT_EQ(sz, 0);

    //Select with > condition
    s = "select name from People where Age > 18;";
    dbres = db.execute(s);
    sz = dbres.data[0].data.size;
    EXPECT_EQ(sz, 4);

    //Select with < condition
    s = "select name from People where Age < 18;";
    dbres = db.execute(s);
    sz = dbres.data[0].data.size;
    EXPECT_EQ(sz, 2);

    //Logical and, expression with bool
    s = "select name from People where Age > 18 && is_student;";
    dbres = db.execute(s);
    sz = dbres.data[0].data.size;
    EXPECT_EQ(sz, 2);

    //<=
    s = "select name from People where Age <= 45;";
    dbres = db.execute(s);
    sz = dbres.data[0].data.size;
    EXPECT_EQ(sz, 6);

    //>=
    s = "select name from People where Age >= 45;";
    dbres = db.execute(s);
    sz = dbres.data[0].data.size;
    EXPECT_EQ(sz, 1);

    s = "select name from People where Age = 19 && !(is_student);";
    dbres = db.execute(s);
    EXPECT_EQ(db.tables["People"]->size, 6);

    //Empty Delete
    s = "delete People where Age = 18;";
    db.execute(s);
    EXPECT_EQ(db.tables["People"]->size, 6);

    //Update with simple condition. Checking that update does not change table size
    s = "update People set is_student = true, name = \"LOL\" where Age < 40;";
    dbres = db.execute(s);
    EXPECT_EQ(db.tables["People"]->size, 6);

    //Update with simple condition. Checking that update does not change table size
    s = "update People set name = \"aaaa\" where Age < 40;";
    dbres = db.execute(s);
    EXPECT_EQ(db.tables["People"]->size, 6);

    //Checking update results
    s = "select name from People where is_student;";
    dbres = db.execute(s);
    sz = dbres.data[0].data.size;
    EXPECT_EQ(sz, 5);

    //Not empty Delete
    s = "delete People where Age = 19;";
    db.execute(s);
    EXPECT_EQ(db.tables["People"]->size, 4);

    //Checking for correct Select after Delete. Also checking if the syntax is correct
    s = "SELECT name FROM People WHERE Age = 19;";
    dbres = db.execute(s);
    sz = dbres.data[0].data.size;
    EXPECT_EQ(sz, 0);

    //Delete of the whole table
    s = "delete People where true;";
    db.execute(s);
    EXPECT_EQ(db.tables["People"]->size, 0);
}


TEST(QueryTests, Test2) {

    //Multiple tables in database
    Database db;

    //Checking column flags. Also checking that names of tables and columns are sensitive to register
    std::string s = "create table Employees ((autoincrement, key) id : int32 = 0, name : string[32] = \"empty_name\", Age : int32 = 18, department_id : int32 = -1);";
    db.execute(s);
    s = "create table Departments ((key) id : int32 = 0, name : string[32] = \"headquarters\", city : string[50] = \"Moscow\");";
    db.execute(s);

    //->Bad, to many commas. Change this
    s = "insert ( , Ivan,  ,  ,) to Employees;";
    db.execute(s);

    s = "insert ( , Denis,  ,  ,) to Employees;";
    db.execute(s);

    s = "insert ( , Maria,  ,  ,) to Employees;";
    db.execute(s);

    s = "insert ( , Janet,  ,  ,) to Employees;";
    db.execute(s);

    s = "insert ( , Samwell,  ,  ,) to Employees;";
    db.execute(s);


    s = "insert ( , Mike, 34, 5) to Employees;";
    db.execute(s);

    s = "insert ( , Tom, 23, 5) to Employees;";
    db.execute(s);

    s = "insert ( , Leo, 46, 5) to Employees;";
    db.execute(s);

    s = "insert ( , Howard, 31, 5) to Employees;";
    db.execute(s);

    s = "insert ( , Constance, 29, 5) to Employees;";
    db.execute(s);

    s = "insert ( , Constance, 19, 3) to Employees;";
    db.execute(s);

    s = "insert ( , George, 20, 3) to Employees;";
    db.execute(s);

    s = "insert (-1, , ,) to Departments;";
    db.execute(s);

    s = "insert (5, PR, Berlin) to Departments;";
    db.execute(s);

    s = "insert (3, IT, Paris) to Departments;";
    db.execute(s);

    //Select with % condition
    s = "select name from Employees where Age % 2 = 0;";
    DBResult dbres = db.execute(s);
    auto sz = dbres.data[0].data.size;
    EXPECT_EQ(sz, 8);

    //Select with |name| condition
    s = "select name from Employees where |name| = 3;";
    dbres = db.execute(s);
    sz = dbres.data[0].data.size;
    EXPECT_EQ(sz, 2);

    //Select with |name| condition
    s = "select name from Employees where (|name| = 3) ^^ (|name| = 4);";
    dbres = db.execute(s);
    sz = dbres.data[0].data.size;
    EXPECT_EQ(sz, 4);

    //Select with / condition
    s = "select name from Employees where Age / 2 = 9;";
    dbres = db.execute(s);
    sz = dbres.data[0].data.size;
    EXPECT_EQ(sz, 6);

    //Update with simple condition. Checking that update does not change table size
    s = "update Employees set department_id = 0, Age = 100 where department_id = -1;";
    db.execute(s);
    EXPECT_EQ(db.tables["Employees"]->size, 12);

    //Update with simple condition. Checking that update does not change table size
    s = "update Employees set department_id = 0 where department_id = -1;";
    db.execute(s);
    EXPECT_EQ(db.tables["Employees"]->size, 12);

    //Checking update result
    s = "select name from Employees where department_id = 0;";
    dbres = db.execute(s);
    sz = dbres.data[0].data.size;
    EXPECT_EQ(sz, 5);

    //Update with simple condition. Checking that update does not change table size
    s = "update Departments set id = 0 where id = -1;";
    db.execute(s);
    EXPECT_EQ(db.tables["Departments"]->size, 3);

    //Checking update result
    s = "select city from Departments where id = 0;";
    dbres = db.execute(s);
    sz = dbres.data[0].data.size;
    EXPECT_EQ(sz, 1);
}


TEST(QueryTests, Test3) {

    //Some errors
    Database db;

    std::string s = "create table People (name : string[32] = noname, Age : int32 = 18, (autoincrement) id : int32 = -2, is_student : bool = false);";
    db.execute(s);

    //Checking uknown word case
    s = "]kek[;";
    DBResult dbres = db.execute(s);
    EXPECT_EQ(dbres.error, "Preprocessor: uknown word ]kek[");

    //Checking uknown word case
    s = "(]kek[);";
    dbres = db.execute(s);
    EXPECT_EQ(dbres.error, "Preprocessor: uknown word ]kek[");

    s = "on;";
    dbres = db.execute(s);
    EXPECT_EQ(dbres.error, "Executor: Uknown command, which starts with on");

    s = "insert;";
    dbres = db.execute(s);
    EXPECT_EQ(dbres.error, "Wrong structure of Insert query. No word \"to\"");

    s = "insert to;";
    dbres = db.execute(s);
    EXPECT_EQ(dbres.error, "Wrong structure of Insert query. No name of the table.");

    s = "insert (1, 2, 3) to Houses;";
    dbres = db.execute(s);
    EXPECT_EQ(dbres.error, "Insert query: No such table in database");

//    s = "select;";
//    dbres = db.execute(s);
//    EXPECT_EQ(dbres.error, "Wrong structure of Select query. Expected word \"from\"");

//    s = "select from where;";
//    dbres = db.execute(s);
//    EXPECT_EQ(dbres.error, "Wrong structure of Select query. No name of the source table.");
//
    s = "select from People;";
    dbres = db.execute(s);
    EXPECT_EQ(dbres.error, "Wrong structure of Select query. Expected word \"where\".");
}

TEST(QueryTests, Test4) {

    //Serialization
    Database db;

    //Checking column flags. Also checking that names of tables and columns are sensitive to register
    std::string s = "create table Employees ((autoincrement, key) id : int32 = 0, name : string[32] = \"empty_name\", Age : int32 = 18, department_id : int32 = -1);";
    db.execute(s);
    s = "create table Departments ((key) id : int32 = 0, name : string[32] = \"headquarters\", city : string[50] = \"Moscow\");";
    db.execute(s);

    //->Bad, to many commas. Change this
    s = "insert ( , Ivan,  ,  ,) to Employees;";
    db.execute(s);

    s = "insert ( , Denis,  ,  ,) to Employees;";
    db.execute(s);

    s = "insert ( , Maria,  ,  ,) to Employees;";
    db.execute(s);

    s = "insert ( , Janet,  ,  ,) to Employees;";
    db.execute(s);

    s = "insert ( , Samwell,  ,  ,) to Employees;";
    db.execute(s);


    s = "insert ( , Mike, 34, 5) to Employees;";
    db.execute(s);

    s = "insert ( , Tom, 23, 5) to Employees;";
    db.execute(s);

    s = "insert ( , Leo, 46, 5) to Employees;";
    db.execute(s);

    s = "insert ( , Howard, 31, 5) to Employees;";
    db.execute(s);

    s = "insert ( , Constance, 29, 5) to Employees;";
    db.execute(s);

    s = "insert ( , Constance, 19, 3) to Employees;";
    db.execute(s);

    s = "insert ( , George, 20, 3) to Employees;";
    db.execute(s);

    s = "insert (-1, , ,) to Departments;";
    db.execute(s);

    s = "insert (5, PR, Berlin) to Departments;";
    db.execute(s);

    s = "insert (3, IT, Paris) to Departments;";
    db.execute(s);

    db.save_to_file(std::ofstream("file.bin", std::ios::binary));
    Database db1;
    db1.load_from_file(std::ifstream("file.bin", std::ios::binary));

    EXPECT_EQ(db.tables.size(), db1.tables.size());

    for (auto it : db.tables) {
        EXPECT_EQ(db.tables[it.first]->size, db1.tables[it.first]->size);
        EXPECT_EQ(db.tables[it.first]->columns.size(), db1.tables[it.first]->columns.size());
    }
}


TEST(QueryTests, Test5) {
    //bytes
    Database db;

    //Checking column flags. Also checking that names of tables and columns are sensitive to register
    std::string s = "create table Users ((autoincrement, key) id : int32 = 0, login : string[32] = \"unnamed\", password : bytes[5] = 0x0000000000);";
    DBResult db1 = db.execute(s);


    //->Bad, to many commas. Change this
    s = "insert ( , \"vasya52\", 0x11aabbccdd) to Users;";
    db.execute(s);

    s = "insert ( , \"oleg228\", 0x22eeb34c52) to Users;";
    db.execute(s);

    s = "insert ( , \"vanyabruh\", 0x0000001234) to Users;";
    db.execute(s);

    s = "select login from Users where password = 0x0000001234;";
    DBResult dbres = db.execute(s);
    auto sz = dbres.data[0].data.size;
    EXPECT_EQ(sz, 1);

    s = "update Users set password = 0x0000000000 where password < 0x00f0000000;";
    dbres = db.execute(s);
    EXPECT_EQ(db.tables["Users"]->size, 3);

    s = "select login from Users where password = 0x0000000000;";
    dbres = db.execute(s);
    sz = dbres.data[0].data.size;
    EXPECT_EQ(sz, 1);

    s = "select login from Users where |password| = 5;";
    dbres = db.execute(s);
    sz = dbres.data[0].data.size;
    EXPECT_EQ(sz, 3);
}

TEST(QueryTests, Test6) {

    //Serialization
    Database db;

    //Checking column flags. Also checking that names of tables and columns are sensitive to register
    std::string s = "create table Employees ((autoincrement, key) id : int32 = 0, name : string[32] = \"empty_name\", Age : int32 = 18, department_id : int32 = -1);";
    db.execute(s);
    s = "create table Departments ((key) id : int32 = 0, name : string[32] = \"headquarters\", city : string[50] = \"Moscow\");";
    db.execute(s);

    //->Bad, to many commas. Change this
    s = "insert ( , Ivan,  ,  ,) to Employees; insert ( , Denis,  ,  ,) to Employees; insert ( , Maria,  ,  ,) to Employees; insert ( , Janet,  ,  ,) to Employees; insert ( , Samwel,  ,  ,) to Employees; insert ( , Mike, 34, 5) to Employees; insert ( , Tom, 23, 5) to Employees; insert ( , Leo, 46, 5) to Employees; insert ( , Howard, 31, 5) to Employees; insert ( , Constance, 29, 5) to Employees; insert ( , Constance, 19, 3) to Employees; insert ( , George, 20, 3) to Employees; insert (-1, , ,) to Departments; insert (5, PR, Berlin) to Departments; insert (3, IT, Paris) to Departments;";
    db.execute(s);


    s = "select Employees_name from {Employees join Departments on Employees.department_id = Departments.id} where Departments_city = \"Berlin\";";
    DBResult dbr = db.execute(s);
    EXPECT_EQ(dbr.data[0].data.size, 5);

    s = "select Employees_name, Departments_city from {select Employees_id, Employees_name, Employees_Age, Departments_city from {Employees join Departments on Employees.department_id = Departments.id} where Departments_city = \"Berlin\"} where true;";
    dbr = db.execute(s);
    dbr.data[0].data.print_table();
    EXPECT_EQ(dbr.data[0].data.size, 5);
}
