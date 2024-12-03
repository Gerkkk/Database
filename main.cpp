#include "MEMDB/Database/Database.h"

using namespace memdb;

int main() {
    Database db;

    db.execute(std::ifstream("queries.txt"));//reading queries from file
    //reading queries from file and saving results of the whole query in file
    db.execute(std::ifstream("select.txt")).save_to_file_readable(std::ofstream("res.txt"));

    //or printing one table to file:
    //DBResult dbr = db.execute(std::ifstream("select.txt"));
    //dbr.data[0].data.save_to_file_readable(std::ofstream("res.txt"));

    return 0;
}
