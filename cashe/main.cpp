#include <iostream>
#include <stdio.h>

#include "include/cashe.h"

int get_hits 		(FILE* papers);
int slow_get_page_int 	(int key);

int main (int argc, char* argv[])
{
    if (argc != 2)
    {
        printf ("Error file name\n");
        
        exit (1);
    }
    
    char*   filename = argv[1];
    FILE*   papers = fopen (filename, "r");
    if (!papers)
    {
        printf ("Unknown file \"%s\"", filename);

        exit (1);
    }

    int hits = get_hits (papers);
    printf ("Число совпадений: %d\n", hits);

    return 0; 
}

int get_hits (FILE* source)
{
    int num_keys = 0;
    fscanf (source, "%d", &num_keys);
   
    cashes::cashe_t<int> cashe (5);

    int hits    = 0;
    int key     = 0;
    fscanf (source, "%d", &key);

    for (int i = 0; i < num_keys; i++)
    {
        printf ("%d\n", key);
        if (cashe.lookup_update (key, slow_get_page_int))
            hits++;

        fscanf (source, "%d", &key);
    }

    return hits;
}

int slow_get_page_int (int key)
{
    return key;
}


