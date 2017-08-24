/**
 * helpers.c
 *
 * Helper functions for Problem Set 3.
 */
 
#include <cs50.h>

#include "helpers.h"

/**
 * Returns true if value is in array of n values, else false.
 */
bool search(int value, int values[], int n)
{
    
    if(n < 0)
    {
        return false;
    }
    
    // Length of list
    int length = n;
    
    // Left and right boundary indexing in the array
    int left = 0;
    int right = n - 1;
    
    while(length > 0)
    {
        
        int mid = (left + right) / 2;

        if(value == values[mid])
        {
            return true;
        }
        else if(value > values[mid])
        {
            left = mid;
        }
        else if(value < values[mid])
        {
            right = mid;
        }
        
        length = right - left;
        
        if(length == 1)
        {
            if(value == values[right] || value == values[left])
            {
                return true;
            }
            else
            {
                break;
            }
        }

    }
    
    return false;
}

/**
 * Sorts array of n values.
 */
 // TODO: implement a sorting algorithm
void sort(int values[], int n)
{
    
    int temp;
    
    for (int i = 0; i < n; i++)
    {
        int smallest_index = i;
        
        for (int index_to_be_checked = i + 1; index_to_be_checked < n; index_to_be_checked++)
        {
            if (values[index_to_be_checked] < values[smallest_index])
                smallest_index = index_to_be_checked;
        }
        
        // Swap values;
        temp = values[smallest_index];
        values[smallest_index] = values[i];
        values[i] = temp;
    }
}
