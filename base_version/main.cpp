#include <iostream>

#include "hash_table.h"


int main()
{
    try {


        Dictionary dict;

        dict.set_hfunc(hash_func_6);

        dict.pars_file("bible.txt");

        printf("total differ words - %d\n", dict.words_amount_);

        dict.sort_and_save();

    }
    catch (Acorn* acorn)
    {
        acorn->tell_user();
    }
    return 0;
}
