#ifndef HASH_TABLE_CPP
#define HASH_TABLE_CPP

#include "hash_table.h"


/*==============================================================================
 *
 *              class oak::Word, Implementation
 *
 *
 /==============================================================================*/
oak::Word::Word(const char *name)
{
    size_t len = strlen(name);

    if (len > oak::MAX_WORD_LEN)
    {
        THROW(LOGIC_ERROR, "Word is too big!", nullptr);
    }
    else
    {
        body_ = new char[len];
    }
}

oak::Word::~Word()
{
    if (body_ != nullptr)
    {
        delete [] body_;
    }
}


inline void oak::Word::re_set(const char *name)
{
    size_t len = strlen(name);

    if (len > oak::MAX_WORD_LEN)
    {
        THROW(LOGIC_ERROR, "Word is too big!", nullptr);
    }
    else
    {
        if (body_ != nullptr)
        {
            delete [] body_;
        }
        body_ = new char[len];
        strcpy(body_, name);
    }
}


#define IS_DIGIT(CANDIDATE)\
    (  ((CANDIDATE >= '0') && (CANDIDATE <= '9')) )

size_t oak::ReadAmount(const char *buffer, int *position)
{
    if (!IS_DIGIT(buffer[*position]))
    {
        THROW(LOGIC_ERROR, "Symbol from dump file is not a namber!", nullptr);
    }

    size_t val = 0;


    while (IS_DIGIT(buffer[*position]))
    {
        val = 10 * val + (buffer[*position] - '0');
        *position ++;
    }

    return val;
}
#undef IS_DIGIT




/*==============================================================================
 *
 *              class oak::Words_dealer, Implementation
 *
 *
 *
 *
 /==============================================================================*/

#define IS_LETTER(CANDIDATE)\
    (  ((CANDIDATE >= 'a') && (CANDIDATE <= 'z'))   \
    || ((CANDIDATE >= 'A') && (CANDIDATE <= 'Z'))   \
    || ((CANDIDATE >= 'а') && (CANDIDATE <= 'я'))   \
    || ((CANDIDATE >= 'А') && (CANDIDATE <= 'Я')) )

oak::Words_dealer::~Words_dealer()
{
    delete [] heap_;
    delete [] text_;

    words_amount_ = 0;
}


/* -------------------------------------------------------
 * @about
 *        Converts text from file to array of strings
 *
 *
 * @input
 *        file with text
 *
 * @output
 *        array of strings (text_)
 *
 /--------------------------------------------------------*/
//TODO запилить вызов new с инициализатором
void oak::Words_dealer::Read_file(const char *filename)
{
    FILE* input = fopen(filename,"r");

    if (input == nullptr)
    {
        THROW(UNKNOWN_ERROR, "Can't open File!", nullptr);
    }


    symbols_amount_ = sizeof_file(input);

    heap_ = new char [symbols_amount_ + 1]{};

    fread (heap_, symbols_amount_, sizeof (char), input);

    words_amount_ = words_am(heap_);

    text_ = new oak::string [words_amount_]{};


    /* splits text from heap_ to
     * sentences and puts it into text_*/
    split_text ();

    fclose(input);
}





/* -------------------------------------------------------
 * @about
 *        Calculates amount of bytes in input file
 *
 *
 /--------------------------------------------------------*/
size_t oak::Words_dealer::sizeof_file(FILE *file)
{
    fseek (file, 0, SEEK_END);
    size_t file_length = ftell (file);

    if (file_length < 0)
    {
        THROW(UNKNOWN_ERROR, "Too big FILE! Change int to long int!", nullptr);
    }

    rewind (file);
    return file_length;
}


/* -------------------------------------------------------
 * @about
 *        Calculates amount of words in the text
 *
 *
 /--------------------------------------------------------*/


size_t oak::Words_dealer::words_am(const char *heap)
{
    size_t words_am = 1;

    if (heap[0] == 0)
    {
        THROW(LOGIC_ERROR, "Empty File!!!", nullptr);
    }

    for(int i = 0; heap[i] != '\0'; i++)
    {
        if ( (!IS_LETTER(heap[i])) && (IS_LETTER(heap[i - 1])) )
        {
            words_am++;
        }

    }
    return words_am;
}



/* -------------------------------------------------------
 * @about
 *        splits text to strings
 *        splits heap_ to text_
 *
 * @input
 *        heap with text
 *
 * @output
 *        array of strings
 *
 /--------------------------------------------------------*/
void oak::Words_dealer::split_text ()
{
    /* Finite machine realisation */
    static enum oak::TEXT_SPLIT_STATE split_state = TRASH;

    if (heap_[0] == 0)
    {
        THROW(LOGIC_ERROR, "Empty File!!!", nullptr);
    }


    int j = 0; //index for array of words
    int i = 0; //index for heap

    /* finding first word in the text*/
    while(!IS_LETTER(heap_[i]))
    {
        i++;
    }

    text_[j].body_ = heap_ + i;
    j++;
    split_state = WORD;


    while (heap_[i] != '\0')
    {
        switch(split_state)
        {
            case WORD:
            {
                if( (i > 0) && IS_LETTER(heap_[i - 1]) && !IS_LETTER(heap_[i]))
                {
                    split_state = TRASH;
                    heap_[i] = '\0';
                }

            }
            break;

            case TRASH:
            {
                if( (i > 0) && !IS_LETTER(heap_[i - 1]) && IS_LETTER(heap_[i]))
                {
                    split_state = WORD;
                    text_[j].body_ = heap_ + i;
                    j++;
                }

            }
            break;

        }

        i++;
    }
}

#undef IS_LETTER


/*==============================================================================
 *
 *              class Node, Implementation
 *
 *
 /==============================================================================*/
Node::Node(const char *word)
{
    word_.re_set(word);
    frequency_ = 1;
};


Node::~Node()
{
    if (next_node_ != nullptr)
    {
        delete  next_node_;
    }

    frequency_ = 0;

}

inline void Node::re_set(const char *word)
{
    word_.re_set(word);
    frequency_ = 1;
    next_node_ = nullptr;
}


/*==============================================================================
 *
 *              class Dictionary, Implementation
 *
 *
 /==============================================================================*/

#define CHECK_HASH_FUNCTION\
    if (hash_func_ == nullptr)\
    {\
        THROW(LOGIC_ERROR, "You have not chosen hash function!", nullptr);\
    }\

Dictionary::Dictionary()
{
    table_ = new Node[oak::DICT_SIZE]{};
    words_amount_ = 0;
}


Dictionary::Dictionary(const char *filename)
{
    oak::Words_dealer dealer;

    dealer.Read_file(filename);

    int i = 0; // index for heap
    int j = 0; // index for words array

    Node* current_node = nullptr;

    while(i < dealer.symbols_amount_ - 1)
    {

        if (dealer.heap_[i] == '\0')
        {
            current_node = fins(dealer.text_[j].body_);
            current_node->frequency_ = oak::ReadAmount(dealer.heap_, &i);
            j++;
        }
        i++;
    }
}



Dictionary::~Dictionary()
{
    delete [] table_;
    table_ = nullptr;

    delete [] sorted_list_;
    sorted_list_ = nullptr;
}


void Dictionary::set_hfunc(size_t (*func)(const char*))
{
    hash_func_ = func;
};



/*-------------------------------------------------------------------------------------------
 * @about
 *          Find word in table
 *
 * @input
 *          word to been found or inserted
 *
 * @output
 *          POINTER at element with given word
 *
 * @note
 *          returns nullptr if wor isn't in table
 *
 /-------------------------------------------------------------------------------------------*/
Node* Dictionary::find(const char *word) const
{
    Node* current_node = &table_[hash_func_(word) % oak::DICT_SIZE];

    if (current_node->frequency_ == 0)
    {
        return nullptr;
    }

    while (current_node != nullptr)
    {
        if (strcmp(word, current_node->word_.body_) == 0)
        {
            return current_node;
        }

        current_node = current_node->next_node_;
    }

    /* to return nullptr if word isn't in table*/
    return nullptr;
}


/*-------------------------------------------------------------------------------------------
 *                          BOSS!!
 * 
 * @about                                                   (This is the BOSS)
 *          Find word in table
 *          abd insert this word if it not in table
 *
 * @input
 *          word to been found or inserted
 *
 * @output
 *          POINTER of this word
 *
 * @note
 *          returns POINTER at new Node in case of inserting
 *
 /-------------------------------------------------------------------------------------------*/
//TODO strlwr look

#define WORD_CORRECTION                                 \
    int i = 0;                                          \
    while(word[i] != '\0')                              \
    {                                                   \
        if ((word[i] >= 'A') && (word[i] <= 'Z'))       \
        {                                               \
            word[i] = 'a' + (word[i] - 'A');            \
        }                                               \
        else if ((word[i] >= 'А') && (word[i] <= 'Я'))  \
        {                                               \
            word[i] = 'а' + (word[i] - 'А');            \
        }                                               \
        i++;                                            \
    }

Node* Dictionary::fins(char* word)
{

    CHECK_HASH_FUNCTION

    WORD_CORRECTION

    Node* current_node = &table_[hash_func_(word) % oak::DICT_SIZE];

    if (current_node->frequency_ == 0)
    {
        words_amount_ ++;
        current_node->re_set(word);
        current_node->frequency_ = 0;
        return current_node;
    }

    while (current_node != nullptr)
    {
        if (strcmp(word, current_node->word_.body_) == 0)
        {
            return current_node;
        }
        else if (current_node->next_node_ == nullptr)
        {
            words_amount_++;
            current_node->next_node_ = new Node(word);
            return current_node;
        }

        current_node = current_node->next_node_;
    }
}

#undef WORD_CORRECTION


uint32_t Dictionary::ret_frequency(const char *word) const
{

    Node* current_node = find(word);
    if (current_node == nullptr)
    {
        return 0;
    }
    else
    {
        return current_node->frequency_;
    }


}


/*-------------------------------------------------------------------------------------------
 * @about
 *          Find word in table
 *          abd insert this word if it not in table
 *
 * @input
 *          word to been found or inserted
 *
 * @output
 *          frequency of this word
 *
 * @note
 *          returns 0 in case of inserting
 *
 /-------------------------------------------------------------------------------------------*/
uint32_t Dictionary::ret_ins(char *word)
{
    return fins(word)->frequency_;
}


void Dictionary::add(char *word)
{

    CHECK_HASH_FUNCTION

    fins(word)->frequency_ ++;

}

void Dictionary::pars_file(const char *file_name)
{
    CHECK_HASH_FUNCTION
    oak::Words_dealer dealer;

    dealer.Read_file(file_name);

    for (int i = 0; i < dealer.words_amount_ - 1; i++)
    {
        add(dealer.text_[i].body_);
    }

}


void Dictionary::save_to_file(const char *filename)
{
    FILE* out = fopen(filename, "w");

    Node* current_node = nullptr;

    for (int i = 0; i < oak::DICT_SIZE; i++)
    {
        current_node = &table_[i];

        while(current_node != nullptr)
        {
            if (current_node->frequency_ != 0)
            {
                //printf("%s -  [%u]\n", current_node->word_.body_, current_node->frequency_ );
                fprintf(out, "%s-%u\n", current_node->word_.body_, current_node->frequency_ );
            }
            current_node = current_node->next_node_;
        }
    }

    fclose(out);
}


void Dictionary::save_to_file()
{
    save_to_file(oak::DEFAULT_DUMP_FILE);
}

void Dictionary::sort_and_save()
{
    sort_and_save(oak::DEFAULT_DUMP_FILE);
}

void Dictionary::sort_and_save(const char* filename)
{
    FILE* out = fopen(filename, "w");

    sorted_list_ = new Node* [words_amount_];
    int pos = 0;

    Node* current_node = nullptr;

    for (int i = 0; i < oak::DICT_SIZE; i++)
    {
        current_node = &table_[i];

        while(current_node != nullptr)
        {
            if (current_node->frequency_ != 0)
            {
                sorted_list_[pos] = current_node;
                pos++;
            }
            current_node = current_node->next_node_;
        }
    }

    qsort(sorted_list_, words_amount_, sizeof(Node*), oak::words_freq_cmp);

    for(int i = 0; i < words_amount_; i++)
    {
        fprintf(out, "%s - [%u]\n", sorted_list_[i]->word_.body_, sorted_list_[i]->frequency_ );
        //printf("%s -  [%u]\n", sorted_list_[i]->word_.body_, sorted_list_[i]->frequency_ );
    }

    fclose(out);
}

int oak::words_freq_cmp(const void* first, const void* second)
{
    return (*(Node **)second)->frequency_ - (*(Node **)first)->frequency_;
}

#define WRITE_TO_FILE(FILENAME)\
    Node* current_node = nullptr;                                                   \
    int len = 0;                                                                    \
                                                                                    \
    fprintf(out,"\\documentclass[a4paper,12pt]{article}\n"                          \
                    "\\usepackage{float}\n"                                         \
                    "\\usepackage{pgfplots}\n"                                      \
                    "% Preamble: \\pgfplotsset{width=1cm,compat=newest}\n"          \
                    "\\begin{document}\n"                                           \
                    "\\begin{tikzpicture}\n"                                        \
                    "\\begin{axis}[height=15cm,width=15cm,grid=major]\n"            \
                    "\\addlegendentry{gpaphic}\n"                                   \
                    "\\addplot[color = blue, mark = * ] coordinates {\n");          \
                                                                                    \
    for (int i = 0; i < oak::DICT_SIZE; i++)                                        \
    {                                                                               \
        len = 0;                                                                    \
        current_node = &table_[i];                                                  \
                                                                                    \
        while(current_node != nullptr)                                              \
        {                                                                           \
            if (current_node->frequency_ != 0)                                      \
            {                                                                       \
                len++;                                                              \
            }                                                                       \
            current_node = current_node->next_node_;                                \
        }                                                                           \
                                                                                    \
    if (len != 0)                                                                   \
        {                                                                           \
            fprintf(out, "(%d;%d)\n", i, len);                                      \
        }                                                                           \
    }                                                                               \
    fprintf(out, "};\n\n"                                                           \
                    "\\addlegendentry{dots}\n"                                      \
                    "\\addplot[color = red, mark = * ] coordinates {\n");           \
    fprintf(out, "};\n\n"                                                           \
                    "\\end{axis}\n"                                                 \
                    "\\end{tikzpicture}\n"                                          \
                    "\\end{document}\n");                                           \






void Dictionary::analise_and_save(const char *filename)
{
    FILE* out = fopen(filename, "w");

    WRITE_TO_FILE(out)

    fclose(out);
}


#endif // HASH_TABLE_CPP