#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#include <iostream>
#include <cstring>

#include "acorn.h"
/*==============================================================================
 *
 *          Frequency Dictionary
 *
 *
 /==============================================================================*/

#include "hash_functions.h"


/*==============================================================================
 *
 *          My namespace
 *          to not been disappointed with enormous amount of inside data
 *
 *
 /==============================================================================*/
struct Node;

namespace oak
{
    enum TEXT_SPLIT_STATE
    {
        WORD = 0,
        TRASH,
        TEXT_SPLIT_STATE_DEFAULT
    };

    const uint16_t DICT_SIZE = 10001;

    const uint8_t MAX_WORD_LEN = 40;

    const char* DEFAULT_DUMP_FILE = "hash_table_save.txt";


    struct Word
    {
        char* body_ = nullptr;


        Word(){};

        explicit Word(const char* name);

        ~Word();

        Word operator = (const Word& that) = delete;
        
        inline void re_set(const char* name);
    };

    struct string
    {
        char* body_ = nullptr;
        int len_ = 0;
    };

    struct Words_dealer
    {
        /* heap to put symbols from the file*/
        char* heap_ = nullptr;

        /* array of strings from input file*/
        oak::string* text_ = nullptr;

        int words_amount_ = 0;

        int symbols_amount_ = 0;


        Words_dealer(){}

        ~Words_dealer();

        void Read_file(const char* filename);

        size_t sizeof_file(FILE* file);

        size_t words_am(const char* heap);

        void split_text();
    };


    size_t ReadAmount(const char* buffer, int *position);

    int words_freq_cmp(const void* first, const void* second);

}



/*==============================================================================
 *
 *              class Node
 *
 *                  @param1 - word
 *                  @param2 - frequency
 *
 *
 /==============================================================================*/
struct Node
{
    oak::Word word_;
    uint32_t frequency_ = 0;

    Node* next_node_ = nullptr;

    friend class Dictionary;

public:

    Node(){};

    Node(const char* word);

    ~Node();

    inline void re_set(const char* word);

};


/*==============================================================================
 *
 *              class Dictionary
 *
 *                  Imitation of big array of data with words - indexing
 *
 *
 /==============================================================================*/
struct Dictionary
{
    Node* table_ = nullptr;

    Node** sorted_list_ = nullptr;

    size_t size_ = oak::DICT_SIZE;

    int words_amount_ = 0;

    size_t (*hash_func_)(const char* ) = nullptr;

public:

    Dictionary();

    Dictionary(const char* filename);

    ~Dictionary();

    void set_hfunc(size_t (*func)(const char*));

    Node* find(const char* word) const;

    Node* fins(char* word);

    void add(char* word);

    /* returns frequency*/
    uint32_t ret_frequency(const char* word) const;

    /* returns frequency and inserts if it doesnt in table*/
    uint32_t ret_ins(char* word);

    void sort_and_save();

    void sort_and_save(const char* filename);

    void pars_file(const char* file_name);

    void save_to_file();

    void save_to_file(const char* filename);

    void analise_and_save(const char* filename);
};

#include "hash_table.cpp"

#endif // HASH_TABLE_H