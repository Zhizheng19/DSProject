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
#include <stdbool.h>

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
int sumOfParcelsWgt(Parcel* root);
float sumOfParcelsVal(Parcel* root);
void printBSTInOrder(Parcel* root);

void printSectionLowerThanWgt(Parcel* root, int partitionWgt);
void printSectionHigherThanWgt(Parcel* root, int partitionWgt);
void deleteBST(Parcel* root);

// functions for hash table
int generateHash(char* str);
void insertHashTableWithBST(Parcel* table[], char* dest, int weight, float value);
void deleteHashTable(Parcel* table[], int tableSize);
void printTotalParcelWgtAndValForCountry(Parcel* hashTable[], char* country);
void printLigherParcelsInCountry(Parcel* table[], char* country,int wgt);
void printHeavierParcelsInCountry(Parcel* table[], char* country, int wgt);
void printCheapestAndMostExpensiveParcelInCountry(Parcel* table[], char* country);

// functions to process user input
void clearNewLineChar(char* string);
bool validEnteredDestination(Parcel* hashTable[], char* country);
int main(void) 
{
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

    char country[] = "Japan";
    int partitionWgt = 50000;
    if (validEnteredDestination(hashTable, country))
    {
        // display the cheapest and the most expensive parcel
        printCheapestAndMostExpensiveParcelInCountry(hashTable, country);

        // display parcels lower / higher than a given weight. 
        printLigherParcelsInCountry(hashTable, country, partitionWgt);
        printHeavierParcelsInCountry(hashTable, country, partitionWgt);

        // display the total weight and value of parcels for a given destination
        printTotalParcelWgtAndValForCountry(hashTable, country);
    }
    else
    {
       printf("Destination Not Found!\n");
    }

    // free dynamically allocated memory
    deleteHashTable(hashTable, HASH_TABLE_SIZE);


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
    if (toPrint != NULL)
    {
        printf("Destination:\t%10s\t Weight: %6d gms\t Value: $%8.2f\n", toPrint->Dest, toPrint->Weight, toPrint->Value);
    }
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
* find and returns the node with the maximum weight in a Binary Search Tree by navigating to the rightmost node
* Parameter: a pointer to the root of the BST to search for 
*/
Parcel* findMaxWeight(Parcel* root)
{
    if (root == NULL || root->Right == NULL)
    {
        return root;
    }
    return findMaxWeight(root->Right);
}

/* find and returns the node with the minimum weight in a Binary Search Tree by navigating to the leftmost node
* Parameter: a pointer to the root of the BST to search for
*/
Parcel* findMinWeight(Parcel* root)
{
    if (root == NULL || root->Left == NULL)
    {
        return root;
    }
    return findMinWeight(root->Left);
}
// return null if the tree is empty.
// traverse all the node to find the cheapest one.
Parcel* findCheapestParcel(Parcel* parent)
{
    Parcel* cheapest = parent;
    Parcel* cheapestInLeft = NULL;
    Parcel* cheapestInRight = NULL;

    if (parent != NULL)
    {
        if (parent->Left == NULL && parent->Right == NULL)
        { }
        else if (parent->Left == NULL && parent->Right != NULL)
        {
            cheapestInRight = findCheapestParcel(parent->Right);
            if (cheapest->Value > cheapestInRight->Value)
            {
                cheapest = cheapestInRight;
            }
        }
        else if (parent->Left != NULL && parent->Right == NULL)
        {
            cheapestInLeft = findCheapestParcel(parent->Left);
            if (cheapest->Value > cheapestInLeft->Value)
            {
                cheapest = cheapestInLeft;
            }
        }
        else
        {
            cheapestInLeft = findCheapestParcel(parent->Left);
            cheapestInRight = findCheapestParcel(parent->Right);
            if (cheapestInLeft->Value < cheapestInRight->Value && 
                cheapestInLeft->Value < parent->Value)
            {
                cheapest = cheapestInLeft;
            }
            if (cheapestInRight->Value < cheapestInLeft->Value &&
                cheapestInRight->Value < parent->Value) 
            {
                cheapest = cheapestInRight;
            }
        }
    }
    return cheapest;
}

// return null if the tree is empty.
// traverse all the node to find the most expensive one.
Parcel* findMostExpensiveParcel(Parcel* parent)
{
    Parcel* maxValue = parent;
    Parcel* maxValueInLeft = NULL;
    Parcel* maxValueInRight = NULL;

    if (parent != NULL)
    {
        if (parent->Left == NULL && parent->Right == NULL)
        { }
        else if (parent->Left == NULL && parent->Right != NULL)
        {
            maxValueInRight = findMostExpensiveParcel(parent->Right);
            if (maxValue->Value < maxValueInRight->Value)
            {
                maxValue = maxValueInRight;
            }
        }
        else if (parent->Left != NULL && parent->Right == NULL)
        {
            maxValueInLeft = findMostExpensiveParcel(parent->Left);
            if (maxValue->Value < maxValueInLeft->Value)
            {
                maxValue = maxValueInLeft;
            }
        }
        else
        {
            maxValueInLeft = findMostExpensiveParcel(parent->Left);
            maxValueInRight = findMostExpensiveParcel(parent->Right);
            if (maxValueInLeft->Value > maxValueInRight->Value &&
                maxValueInLeft->Value > parent->Value)
            {
                maxValue = maxValueInLeft;
            }
            if (maxValueInRight->Value > maxValueInLeft->Value &&
                maxValueInRight->Value > parent->Value)
            {
                maxValue = maxValueInRight;
            }
        }
    }
    return maxValue;

}


void printSectionLowerThanWgt(Parcel* parent, int partition)
{
    if (parent != NULL)
    {
        if (parent->Weight < partition)
        {
            printBSTInOrder(parent->Left);
            printParcel(parent);
            printSectionLowerThanWgt(parent->Right, partition);
        }
        else
        {
            printSectionLowerThanWgt(parent->Left, partition);
        }
    }
}

void printSectionHigherThanWgt(Parcel* parent, int partition)
{
    if (parent != NULL)
    {
        if (parent->Weight > partition)
        {
            printSectionHigherThanWgt(parent->Left, partition);
            printParcel(parent);
            printBSTInOrder(parent->Right);
        }
        else
        {
            printSectionHigherThanWgt(parent->Right, partition);
        }
    }
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
void deleteHashTable(Parcel* table[], int tableSize)
{
    for (int i = 0; i < tableSize; ++i)
    {
        deleteBST(table[i]);
    }
}

int sumOfParcelsWgt(Parcel* parent)
{
    int sum = 0;
    if (parent != NULL)
    {
        sum += parent->Weight;
        sum += sumOfParcelsWgt(parent->Left);
        sum += sumOfParcelsWgt(parent->Right);
    }
    return sum;
}

float sumOfParcelsVal(Parcel* parent)
{
    float sum = 0;
    if (parent!= NULL)
    {
        sum += parent->Value;
        sum += sumOfParcelsVal(parent->Left);
        sum += sumOfParcelsVal(parent->Right);
    }
    return sum;
}

void printTotalParcelWgtAndValForCountry(Parcel* table[], char* country)
{
    int hash = generateHash(country);
    printf("\nDestination:\t%10s\t Total Weight: %8d gms\t Total: $%10.2f\n", 
        country, sumOfParcelsWgt(table[hash]), sumOfParcelsVal(table[hash]));
}

void printLigherParcelsInCountry(Parcel* table[], char* country, int wgt)
{
    int hash = generateHash(country);
    printf("\n/====================== Lighter than %d gms ===================/\n\n", wgt);
    /*
    if (wgt <= findMinWeight(table[hash])->Weight) 
    {
        printf("Parcel not found\n");
        return;
    }
    */
    printSectionLowerThanWgt(table[hash], wgt);
    
}

void printHeavierParcelsInCountry(Parcel* table[], char* country, int wgt)
{
    int hash = generateHash(country);
    printf("\n/====================== Heavier than %d gms ==================/\n\n", wgt);
    /*
    if (wgt >= findMaxWeight(table[hash])->Weight)
    {
        printf("Parcel not found\n");
        return;
    }
    */
    printSectionHigherThanWgt(table[hash], wgt);
    
}

//display the cheapest and the most expensive parcel.
void printCheapestAndMostExpensiveParcelInCountry(Parcel* table[], char* country)
{
    int hash = generateHash(country);
    printParcel(findCheapestParcel(table[hash]));
    printParcel(findMostExpensiveParcel(table[hash]));
}

bool validEnteredDestination(Parcel* table[], char* country)
{
    bool retCode = true;
    int hash = generateHash(country);
    if (table[hash] == NULL || strcmp(country, table[hash]->Dest) != 0)
    {
        retCode = false;
    }
    return retCode;
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
