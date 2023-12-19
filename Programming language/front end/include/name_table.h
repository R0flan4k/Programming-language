#ifndef NAME_TABLE_H
    #define NAME_TABLE_H

    const size_t NAME_TABLE_CAPACITY = 128;

    struct NameTableElem {
        const char * name;
        size_t length;
    };

    struct NameTable {
        NameTableElem names[NAME_TABLE_CAPACITY];
        size_t size;
    };

    bool is_in_name_table(const NameTable * name_table, const char * name, size_t * index);
    bool add_name_table_elem(NameTable * name_table, const char * name, const size_t length, size_t * index);

#endif
