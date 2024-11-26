## Description of some interesting moments

1) As it may be seen from Performance test, it takes database 3 seconds to execute 100 inserts. This may sound awful for c++, but in fact, as I found out using std::clock, all this time is used by std::regex in tokens parsing. The database itself works fast, even though there are no indexes yet. That is why ORM make sence in this case.