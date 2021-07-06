#ifndef TREE_H
#define TREE_H


#include <any>
#include <string>
#include <vector>
#include <unordered_map>


struct Row {
    std::vector<std::any> fields;
};


class Table {
    
    std::unordered_map<std::string, int> indices; // field -> index
    std::unordered_map<std::string, std::string> types; // field -> type
    std::vector<Row> rows; // will be b-tree later

public:
    void insert(const Row row);
    
    void select(const std::vector<std::string> cols);
    
    Table(std::vector<std::string> col_names, std::vector<std::string> col_types);
};




#endif
