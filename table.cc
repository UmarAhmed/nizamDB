// testing for btree.h


#include "table.h"
#include <iomanip>
#include <iostream>


template <typename T>
inline void align_print(T t, const int width) {
    std::cout << std::left << std::setw(width) << std::setfill(' ');
    std::cout << t;
}


void print_any(const std::string type, std::any v, const int width = 16) {
    if (type == "int") {
        const auto w = std::any_cast<int>(v);
        align_print(w, width);
    } else if (type == "string") {
        const auto w = std::any_cast<std::string>(v);
        align_print(w, width);
    } else if (type == "float") {
        const auto w = std::any_cast<float>(v);
        align_print(w, width);
    } else {
        std::cout << "unknown";
    }
}


void Table::insert(const Row row) {
    rows.push_back(row);
}


void Table::select(const std::vector<std::string> cols) {
    // Print header
    for (auto col: cols) {
        align_print(col, 16);
    }
    std::cout << std::endl;

    // Print each row
    for (const auto row: rows) {
        for (const std::string field: cols) {
            const int index = indices[field];
            print_any(types[field], row.fields[index]);
        }
        std::cout << std::endl;
    }
}


Table::Table(std::vector<std::string> col_names, std::vector<std::string> col_types) {
    if (col_names.size() != col_types.size()) {
        throw "size mismatch between col_names and col_types";
    }
    for (int i = 0; i < col_names.size(); ++i) {
        indices[ col_names[i] ] = i;
        types[ col_names[i] ] = col_types[i];
    }
}


int main() {
    // table creation
    std::vector<std::string> names {"Key", "Name", "Weight"};
    std::vector<std::string> types {"int", "string", "float"};
    Table t {names, types};

    // insertion
    Row r;
    r.fields = {1, std::string {"John"}, float {129.3} };

    Row r2;
    r2.fields = {2, std::string {"Evan"}, float {133.9} };

    Row r3;
    r3.fields = {3, std::string {"Rumi"}, float{0.0} };

    t.insert(r);
    t.insert(r2);
    t.insert(r3);

    // print columns
    t.select( names );
}
