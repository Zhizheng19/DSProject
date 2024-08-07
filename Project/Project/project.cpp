/*
* FILENAME      : project.cpp
* PROJECT       : project
* PROGRAMMER    : Zhizheng Dong
* FIRST VERSION : 2024-08-06
* DESCRIPTION	:
*	This file loads ...
*/

#pragma warning (disable : 4996)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define HASH_TABLE_SIZE     127
#define ENTRY_SIZE          50
#define COUNTRY_SIZE        20

typedef struct Parcel{
    int Weight;
    float Value;
    char* Dest;
    Parcel* Left;
    Parcel* Right;
} Parcel;

//prototypes


// functions of Parcel
Parcel* createNewParcel(char* newDest, int newWgt, float newVal);
void deleteParcel(Parcel* toDel);
void printParcel(Parcel* toPrint);
// functions of BST
Parcel* insertParcelToBST(Parcel* root, Parcel* newParcel);
Parcel* findMaxWeight(Parcel* root); 
Parcel* findMinWeight(Parcel* root); 
Parcel* findCheapestParcel(Parcel* root);
Parcel* findMostExpensiveParcel(Parcel* root);
void printBSTInOrder(Parcel* root);
void deleteBST(Parcel* root);

// functions for hash table
int generateHash(char* str);
void insertHashTableWithBST(Parcel* table[], char* dest, int weight, float value);
void deleteHashTable(Parcel* table[HASH_TABLE_SIZE]);
Parcel* searchHashTable(Parcel* table[HASH_TABLE_SIZE], char* key);

void printHashTable(Parcel* table[HASH_TABLE_SIZE]); // for test !!!!

// functions to process user input
void clearNewLineChar(char* string);

int main(void) {
    // variabales
    char parcelEntry[ENTRY_SIZE] = "";
    FILE* fPtr = NULL;
    Parcel* hashTable[HASH_TABLE_SIZE] = { NULL };

    // read the file to load the parcels' information. 
    fPtr = fopen("couriers.txt", "r");
    if (fPtr == NULL)
    {
        printf("**File Open ERROR\n");
        exit(EXIT_FAILURE);
    }

    while (fgets(parcelEntry, ENTRY_SIZE, fPtr) != NULL)
    {
        char country[COUNTRY_SIZE] = "";
        int weight = 0;
        float price = 0.0f;        
        // parse an parcel entry and load it into the Parcel node
        sscanf_s(parcelEntry,"%[^,0-9], %d, %f", country, COUNTRY_SIZE, &weight, &price);
        // insert to the BST inside the hash table
        insertHashTableWithBST(hashTable, country, weight, price);
    }
    if (!feof(fPtr))
    {
        if (ferror(fPtr))
        {
            printf("**File Reading ERROR\n");
            exit(EXIT_FAILURE);
        }
    }

    if (fclose(fPtr) != 0)
    {
        printf("**File Close ERROR\n");
        exit(EXIT_FAILURE);
    }

    printHashTable(hashTable);// for test!!!

    deleteHashTable(hashTable);



	return 0;
}
/*
* FUNCTION      : generateHash
* DESCRIPTION   : this functoin converts a string to a hash value.
* PARAMETERS    : unsigned char* str : a string to be convert to a hash value
* RETURNS       : int : an interger representing the hash value.
*/
int generateHash(char* str)
{
    /* TITLE        : "djb2 hash" source code
    * AUTHOR        : Daniel J. Bernstein
    * DATE          : 1991
    * AVAILABIILTY  : https://theartincode.stanis.me/008-djb2/
    */
    unsigned long hash = 5381;
    int c = 0;
    while (c = *str++)
    {
        hash = ((hash << 5) + hash) + c;
    }
    hash = hash % HASH_TABLE_SIZE;
    return (int)hash;
}


Parcel* createNewParcel(char* newDest, int newWgt, float newVal)
{
    Parcel* newNode = (Parcel*)malloc(sizeof Parcel);
    if (newNode == NULL)
    {
        printf("**ERROR: Out of Memory!\n");
        exit(EXIT_FAILURE);
    }
    newNode->Dest = (char*)malloc(strlen(newDest)+1);
    if (newNode->Dest == NULL)
    {
        printf("**ERROR: Out of Memory!\n");
        exit(EXIT_FAILURE);
    }
    strcpy_s(newNode->Dest, strlen(newDest)+1, newDest);
    newNode->Value = newVal;
    newNode->Weight = newWgt;
    newNode->Left = NULL;
    newNode->Right = NULL;
    return newNode;
}

void deleteParcel(Parcel* toDel)
{
    free(toDel->Dest);
    free(toDel);
}

void printParcel(Parcel* toPrint)
{
    printf("Destination:\t%10s\t Weight: %6d gms\t Value: $%8.2f\n", toPrint->Dest, toPrint->Weight, toPrint->Value);
}
Parcel* insertParcelToBST(Parcel* parent, Parcel* newParcel)
{

    if (parent == NULL)
    {
        parent = newParcel;
    }
    else if (newParcel->Weight > parent->Weight)
    {
       parent->Right = insertParcelToBST(parent->Right, newParcel);
    }
    else if (newParcel->Weight < parent->Weight)
    {
       parent->Left = insertParcelToBST(parent->Left, newParcel);
    }
    return parent;
}
/*
* find the maxium node within a binary tree and return a pointer to it. 
* Parameter: a pointer to the root of the BST to search for 
*/
Parcel* findMaxWeight(Parcel* root)
{
    return NULL;
}

/* find the minimum node within a binary tree and return a pointer to it. Parameter: a pointer to the root of the BST to search for
*/
Parcel* findMinWeight(Parcel* root)
{
    return NULL;
}

// ??? not defined
Parcel* findCheapestParcel(Parcel* root)
{
    return NULL;
}

// ??? not defined
Parcel* findMostExpensiveParcel(Parcel* root)
{
    return NULL;
}

void printBSTInOrder(Parcel* parent)
{
    if (parent == NULL)
    {
        return;
    }
    else
    {
        printBSTInOrder(parent->Left);
        printParcel(parent);
        printBSTInOrder(parent->Right);
    }
}
void deleteBST(Parcel* parent)
{
    if (parent == NULL)
    {
        return;
    }
    else
    {
        deleteBST(parent->Left);
        deleteBST(parent->Right);
        deleteParcel(parent);
    }
}
void insertHashTableWithBST(Parcel* table[], char* dest, int weight, float value)
{
    int hash = generateHash(dest);
    Parcel* newParcel = createNewParcel(dest, weight, value);
    table[hash] = insertParcelToBST(table[hash], newParcel);
}
void deleteHashTable(Parcel* table[HASH_TABLE_SIZE])
{
    for (int i = 0; i < HASH_TABLE_SIZE; ++i)
    {
        deleteBST(table[i]);
    }
}

// ??? not defined
Parcel* searchHashTable(Parcel* table[HASH_TABLE_SIZE], char* key)
{
    return NULL;
}

void printHashTable(Parcel* table[HASH_TABLE_SIZE])
{
    for (int i = 0; i < HASH_TABLE_SIZE; ++i)
    {
        printBSTInOrder(table[i]);
    }
}

/*
* FUNCTION		: clearNewLineChar
* DESCRIPTION	: This function deletes the new line character at the end of a string.
* PARAMETERS	: char newString[] £ºa taget string.
* RETURNS		: Null.
*/
void clearNewLineChar(char* string)
{
    size_t len = strlen(string);
    if (string[len - 1] == '\n')
    {
        string[len - 1] = '\0';
    }
}
