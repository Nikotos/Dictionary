#ifndef HASH_FUNCTIONS_H
#define HASH_FUNCTIONS_H

#include<iostream>
#include<cstring>


size_t hash_func_1(const char* data)
{
    return 1;
}


size_t hash_func_2(const char* data)
{
    return *data;
}

size_t hash_func_3(const char* data)
{
    return strlen(data);
}

size_t hash_func_4(const char* data)
{
    int sum = 0;
    int i = 0;
    while (data[i] != '\0')
    {
        sum += data[i];
        i++;
    }
    return sum;
}

size_t hash_func_5(const char* data)
{
    int sum = 0;
    int i = 0;
    while (data[i] != '\0')
    {
        sum += data[i];
        i++;
    }
    return (sum/strlen(data));
}


/*I think, that it is a GNU hash*/
inline size_t hash_func_6(const char* data)
{
    int hash = 0;
    for(int i = 0; data[i] != '\0'; i++)
    {
        hash = hash * 31 + data[i];
    }
    return hash;
}

#endif //HASH_FUNCTIONS_H
