#include <stdio.h>
#include <string.h>
#include <stdlib.h>

struct nlist
{                             /* table entry: */
    struct nlist *next;       /* next entry in chain */
    unsigned char *key;       /* defined key */
    unsigned short *value;    /* defined value */
};

struct output
{
    struct output *next;
    unsigned short encodedValue;
};

#define HASHSIZE 512
#define MAXSEQLENGTH 100
static struct nlist *hashtab[HASHSIZE]; /* pointer table */
unsigned hash(unsigned char *);
struct nlist    *lookup(unsigned char *);
struct nlist    *install(unsigned char *, unsigned short us);
unsigned char   *dupkey(unsigned char *);
unsigned short  *dupval(unsigned short us);



int main(void)
{
    unsigned char* input = "This is a string I'd like to encode";

    // Initialization of start dict
    for (int c = 0; c < 256; c++)
    {
        unsigned char *cStr = (unsigned char*)malloc(2);
        if (cStr == NULL)
        {
            return 1;
        }
        cStr[0] = c;
        cStr[1] = '\0'; // Null char to terminate string

        struct nlist* newPair = install(cStr, c);
        free(cStr);
    }
    
    unsigned char* currentSeq = "\0";
    
    struct output* result = (struct output*)malloc(sizeof(struct output));
    struct output* firstEntry = result;

    
    for (size_t i = 0; i < strlen(input); i++) 
    {
        unsigned char c = (unsigned char)input[i];
        unsigned char* cStr = (unsigned char*)malloc(2);
        cStr[0] = c;
        cStr[1] = '\0';
        
        unsigned char* newSeq = malloc(strlen(cStr) + strlen(currentSeq) + 1); // Size of both strings plus termination char
        strcat(newSeq, currentSeq);
        strcat(newSeq, cStr);

        struct nlist* entry = lookup(newSeq);

        if (entry != NULL)
        {
            free(currentSeq);
            currentSeq = newSeq;
        }
        else 
        {
            
        }

        printf("This is char '%s'\n", cStr);
        free(cStr);
    }

    free(currentSeq);


    // unsigned char* key = (unsigned char*)malloc(2);
    // key[0] = (unsigned char)255;
    // key[1] = '\0';
    
    // struct nlist* rPair = lookup(key);

    // if (rPair != NULL)
    // {
    //     printf("key '%s' contains value '%d'\n", rPair->key, *rPair->value);
    // }
    // else 
    // {
    //     printf("Could not find an entry for '%s'\n", key);
    // }



    return 0;
}


/* hash: form hash value for string s */
unsigned hash(unsigned char *s)
{
    unsigned hashval;
    for (hashval = 0; *s != '\0'; s++)
        hashval = *s + 31 * hashval;
    return hashval % HASHSIZE;
}

/* lookup: look for s in hashtab */
struct nlist *lookup(unsigned char *s)
{
    struct nlist *np;
    for (np = hashtab[hash(s)]; np != NULL; np = np->next)
        if (strcmp(s, np->key) == 0)
            return np; /* found */
    return NULL;       /* not found */
}

/* install: put (key, value) in hashtab */
struct nlist *install(unsigned char *key, unsigned short value)
{
    struct nlist *np;
    unsigned hashval;
    if ((np = lookup(key)) == NULL)
    { /* not found */
        np = (struct nlist *)malloc(sizeof(*np));
        if (np == NULL || (np->key = dupkey(key)) == NULL)
            return NULL;
        hashval = hash(key);
        np->next = hashtab[hashval];
        hashtab[hashval] = np;
    }
    else                        /* already there */
        free((void *)np->value); /*free previous value */
    if ((np->value = dupval(value)) == NULL)
        return NULL;
    return np;
}

unsigned char *dupkey(unsigned char *s) /* make a duplicate of s */
{
    unsigned char *p;
    p = (char *)malloc(strlen(s) + 1); /* +1 for ’\0’ */
    if (p != NULL)
        strcpy(p, s);
    return p;
}

unsigned short *dupval(unsigned short val) /* make a duplicate of val */
{
    unsigned short *p;
    p = (unsigned short*)malloc(sizeof(val));
    if (p != NULL)
    {
        *p = val;
    }
    return p;
}
