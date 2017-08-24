/**
 * Implements a dictionary's functionality.
 */

#include <stdbool.h>
#include <ctype.h>
#include <string.h>
#include <stdio.h>
#include <cs50.h>
#include <stdlib.h>

#include "dictionary.h"

/*
 *  Global variables
 */
 
// Store the amount of words loaded in the dictionary
unsigned int word_count = 0;

// create a node struct for the dictionary hashtable
typedef struct node
{
    char word[LENGTH + 1];
    struct node *next;
}
node;

// create a hashtable of pointers-to-node
// and initialize them to NULL
node *hashtable[HASHTABLE_SIZE];



// hash function by reddit user delipity https://www.reddit.com/r/cs50/comments/1x6vc8/pset6_trie_vs_hashtable/cf9nlkn/
int hash_it(char *needs_hashing)
{
    unsigned int hash = 0;
    
    for (int i = 0, n = strlen(needs_hashing); i < n; i++)
    {
        hash = (hash << 2) ^ needs_hashing[i];
    }
    
    return hash % HASHTABLE_SIZE;
}


/**
 * Returns true if word is in dictionary else false.
 */
bool check(const char *word)
{
    // create temp copy (check) of the word-to-be-checked
    int length = strlen(word);
    char check[length + 1];
    
    // convert the word-to-be-checked fully to lowercase to ignore any case sensitivity
    for(int i = 0; i < length; i++)
    {
        if(isalpha(word[i]))
        {
            check[i] = tolower(word[i]);
        }
        else
            check[i] = word[i];
    }
    
    // add null terminator to the end of word-to-be-checked
    check[length] = '\0';
    
    // find the bucket of the word-to-be-checked
    int table_index = hash_it(check);
    
    // create a cursor to traverse the nodes in correct bucket
    node *cursor = hashtable[table_index];
    
    while(cursor != NULL)
    {
        // compare the word-to-be-checked to the current node in the dictionary
        if(strcmp(cursor->word, check) == 0)
        {
            // if match found, return true
            return true;
        }
        else
        // move to next node
        cursor = cursor->next;
    }
    // if word not found in dictionary
    return false;
}

/**
 * Loads dictionary into memory. Returns true if successful else false.
 */
bool load(const char *dictionary)
{
    // initialize the hashtable indexes to null
    for(int i = 0; i < HASHTABLE_SIZE; i++)
    {
        hashtable[i] = NULL;
    }
    
    // hashing index variable to determine right bucket for the word being checked
    int table_index = 0;
    
    FILE *fp = fopen(dictionary, "r");
    if(fp == NULL)
    {
        printf("Couldn't open dictionary file\n");
        return false;
    }
    
    // for storing the read word from dictionary
    char word[LENGTH + 1];
    
    // scan through the dictionry until eof
    while (fscanf(fp, "%s", word) != EOF)
    {
        // hash the word and assign it to proper bucket
        table_index = hash_it(word);
        
        // allocate memory for a new word node
        node *new_node = malloc(sizeof(node));
        
        // if no more memory available for malloc, terminate
        if(new_node == NULL)
        {
            printf("Couldn't malloc new_node\n");
            unload();
            return false;
        }
        
        // copy the read word into the new node we created
        strcpy(new_node->word, word);
        
        // make the correct hashtable index point to the new node we created
        new_node->next = hashtable[table_index];
        hashtable[table_index] = new_node;
        
        // Add to the number of words loaded in the dictionary
        ++word_count;
    }
    
    if(word_count > 1)
    {
        // if dictionary loaded successfully return true
        fclose(fp);
        return true;
    }
    else
    {
        fclose(fp);
        return false;
    }
}

/**
 * Returns number of words in dictionary if loaded else 0 if not yet loaded.
 */
unsigned int size(void)
{
    if(word_count > 0)
    {
        return word_count;
    }
    return 0;
}

/**
 * Unloads dictionary from memory. Returns true if successful else false.
 */
bool unload(void)
{
    // iterate over all the buckets in the hashtable
    for(int i = 0; i < HASHTABLE_SIZE; i++)
    {
        // create cursor to traverse the current bucket
        node *cursor = hashtable[i];
        
        while(cursor != NULL)
        {
            // create temporary cursor to follow the main cursor
            node *tmp = cursor;
            
            // move the current cursor to next entry
            cursor = cursor->next;
            
            //delete previous node stored in tmp cursor
            free(tmp);
        }
        
        // if this was the last bucket, return true
        if(i == HASHTABLE_SIZE - 1)
        {
            return true;
        }
    }
    return false;
}
