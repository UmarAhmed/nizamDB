
This is a lightweight database implementation (in progress)

Will support these SQL queries:
* CREATE TABLE
* INSERT
* SELECT cols FROM table WHERE cond

The backend of the database is a generic B+ Tree

TODO:
* add private method searchnode and refactor 
* add public method search(key)
* add range query
* add a better printing function?
* might want to keep track of min and max key
