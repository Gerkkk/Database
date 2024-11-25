#include <gtest/gtest.h>
#include "../MEMDB/Database/Database.h"
#include <ctime>

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
    std::string s = "create table People ((unique) name : string[32] = noname, Age : int32 = 18, (autoincrement) id : int32 = -2, is_student : bool = false);";
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

    //Logical not in condition, = in condition
    s = "select name from People where Age = 19 && !(is_student);";
    dbres = db.execute(s);
    sz = dbres.data[0].data.size;
    EXPECT_EQ(sz, 1);

    //Empty Delete
    s = "delete People where Age = 18;";
    db.execute(s);
    EXPECT_EQ(db.tables["People"]->size, 6);

    //Update with simple condition. Checking that update does not change table size
    s = "update People set is_student = true where Age < 40;";
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

    //Select with / condition
    s = "select name from Employees where Age / 2 = 9;";
    dbres = db.execute(s);
    sz = dbres.data[0].data.size;
    EXPECT_EQ(sz, 6);

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
