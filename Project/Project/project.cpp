/*
* FILENAME      : project.cpp
* PROJECT       : project
* PROGRAMMER    : Zhizheng Dong
* FIRST VERSION : 2024-08-06
* DESCRIPTION	:
*	This file loads data of parcels including a parcel's destination, weight and value from couriers.txt into 
*   memory, then presents a user menu to the user to display relevant information of the parcels given the user's
*   selection.
*/

#pragma warning (disable : 4996)
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define HASH_TABLE_SIZE     127
#define ENTRY_SIZE          50
#define COUNTRY_SIZE        20

typedef struct Parcel 
{
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
void printLighterParcelsInCountry(Parcel* table[], char* country,int wgt);
void printHeavierParcelsInCountry(Parcel* table[], char* country, int wgt);
void printCheapestAndMostExpensiveParcelInCountry(Parcel* table[], char* country);

// functions to process user input
void clearNewLineChar(char* string);
bool validEnteredDestination(Parcel* hashTable[], char* country);

int main(void) 
{
    // variables
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

    // read the data through user menu 
    int choice = 0;
    char userCountry[COUNTRY_SIZE] = "";
    int userWeight = 0;
    int validInput = 0;

    do
    {
        // Display the menu
        printf("\nMenu:\n");
        printf("1. Enter country name and display all the parcels' details\n");
        printf("2. Enter country and weight pair to display parcels with weight higher/lower than entered weight\n");
        printf("3. Display the total parcel load and valuation for the country\n");
        printf("4. Enter the country name and display cheapest and most expensive parcel's details\n");
        printf("5. Enter the country name and display lightest and heaviest parcel for the country\n");
        printf("6. Exit the application\n");
        printf("Enter your choice: ");

        // Check if the user input is an integer
        validInput = scanf_s("%d", &choice);
        while (getchar() != '\n'); // Clear the input buffer

        if (validInput != 1)
        {
            printf("Invalid input. Please enter a number between 1 and 6.\n");
            continue;
        }

        switch (choice)
        {
        case 1: // display all the parcels' details
            printf("Enter country name: ");
            fgets(userCountry, COUNTRY_SIZE, stdin);
            clearNewLineChar(userCountry);
            if (validEnteredDestination(hashTable, userCountry))
            {
                printBSTInOrder(hashTable[generateHash(userCountry)]);
            }
            else
            {
                printf("Not an Existing Destination!\n");
            }
            break;

        case 2: // display parcels with weight higher/lower than entered weight
            printf("Enter country name: ");
            fgets(userCountry, COUNTRY_SIZE, stdin);
            clearNewLineChar(userCountry);
            if (!validEnteredDestination(hashTable, userCountry))
            {
                printf("Not an Existing Destination!\n");
                break;
            }
            printf("Enter weight: ");
            if (scanf_s("%d", &userWeight) != 1)
            {
                printf("Invalid weight. Please enter a weight.\n");
                while (getchar() != '\n'); // Clear the input buffer
                break;
            }
            while (getchar() != '\n'); // Clear the input buffer
            printHeavierParcelsInCountry(hashTable, userCountry, userWeight);
            printLighterParcelsInCountry(hashTable, userCountry, userWeight);
            break;

        case 3: // display the total parcel load and valuation for the country
            printf("Enter country name: ");
            fgets(userCountry, COUNTRY_SIZE, stdin);
            clearNewLineChar(userCountry);
            if (validEnteredDestination(hashTable, userCountry))
            {
                printTotalParcelWgtAndValForCountry(hashTable, userCountry);
            }
            else
            {
                printf("Not an Existing Destination!\n");
            }
            break;

        case 4: // display cheapest and most expensive parcel
            printf("Enter country name: ");
            fgets(userCountry, COUNTRY_SIZE, stdin);
            clearNewLineChar(userCountry);
            if (validEnteredDestination(hashTable, userCountry))
            {
                printCheapestAndMostExpensiveParcelInCountry(hashTable, userCountry);
            }
            else
            {
                printf("Not an Existing Destination!\n");
            }
            break;

        case 5: // display lightest and heaviest parcel for the country
            printf("Enter country name: ");
            fgets(userCountry, COUNTRY_SIZE, stdin);
            clearNewLineChar(userCountry);
            if (validEnteredDestination(hashTable, userCountry))
            {
                printf("\nThe Lightest Parcel:\n");
                printParcel(findMinWeight(hashTable[generateHash(userCountry)]));
                printf("\nThe Heaviest Parcel:\n");
                printParcel(findMaxWeight(hashTable[generateHash(userCountry)]));
            }
            else
            {
                printf("Not an Existing Destination!\n");
            }
            break;

        case 6:
            printf("\nExiting...Bye\n");
            break;

        default:
            printf("Invalid choice. Please enter a number between 1 and 6.\n");
            break;
        }

    } while (choice != 6);

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
/*
* FUNCTION      : createNewParcel
* DESCRIPTION   : this functoin creates a new Parcel node for trees.
* PARAMETERS    : 
*   char* newDest   :   the desetination for the new parcel.
*   int   newWgt    :   the weight of the new parcel
*   float newVal    :   the valuation of the new parcel
* 
* RETURNS       :
*       Parcel*     : a pointer to the new struct Parcel containing the parcel's info.
*/
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


/*
* FUNCTION      : deleteParcel
* DESCRIPTION   : 
*   This functoin frees the dynamically allocated memory of a Parcel node.
* PARAMETERS    :
*   Parcel* toDel   :   a pointer to the parcel node to be deleted. 
* RETURNS       :  void
*/
void deleteParcel(Parcel* toDel)
{
    free(toDel->Dest);
    free(toDel);
}

/*
* FUNCTION      : printParcel
* DESCRIPTION   :
*   This functoin prints out the information of a parcel, including its destination, weight and value, in one line.
* PARAMETERS    :
*   Parcel* toDel   :   a pointer to the parcel node to be printed out
* RETURNS       :  void
*/
void printParcel(Parcel* toPrint)
{
    if (toPrint != NULL)
    {
        printf("Destination:\t%10s\t Weight: %6d gms\t Value: $%8.2f\n", toPrint->Dest, toPrint->Weight, toPrint->Value);
    }
}
/*
* FUNCTION      : insertParcelToBST
* DESCRIPTION   : Inserts a new parcel into a Binary Search Tree
* PARAMETERS    : Parcel* root - The root of the BST
*                 Parcel* newParcel - The parcel to be inserted
* RETURNS       : returns pointer to the parcel, this parcel is inserted into the binary search tree
*/
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
* FUNCTION      : findMaxWeight
* DESCRIPTION   : Finds and returns the maximum weight in a Binary Search Tree
* PARAMETERS    : Parcel* root - searching from the root of BST
*
* RETURNS       : returns pointer to the parcel, this parcel is the maximum weight
*/
Parcel* findMaxWeight(Parcel* root)
{
    if (root == NULL || root->Right == NULL)
    {
        return root;
    }
    return findMaxWeight(root->Right);
}
/*
* FUNCTION      : findMinWeight
* DESCRIPTION   : Finds and returns the minimum weight in a Binary Search Tree
* PARAMETERS    : Parcel* root - searching from the root of BST
*
* RETURNS       : returns pointer to the parcel, this parcel is the minimum weight
*/
Parcel* findMinWeight(Parcel* root)
{
    if (root == NULL || root->Left == NULL)
    {
        return root;
    }
    return findMinWeight(root->Left);
}

/*
* FUNCTION      : findCheapestParcel
* DESCRIPTION   : Finds and returns the cheapest parcel in a Binary Search Tree
* PARAMETERS    : Parcel* parent - searching from the root of BST
*
* RETURNS       : returns a pointer to the parcel, this parcel is the cheapest parcel in the BST
*/
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

/*
* FUNCTION      : findMostExpensiveParcel
* DESCRIPTION   : Finds and returns the most expensive parcel in a Binary Search Tree
* PARAMETERS    : Parcel* parent - searching from the root of BST
*
* RETURNS       : returns a pointer to the parcel, this parcel is the expensive parcel in the BST
*/
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

/*
* FUNCTION      : printSectionLowerThanWgt
* DESCRIPTION   :
*   This functoin prints out all the parcels lighter than a given weight within a BST.
* PARAMETERS    :
*   Parcel* parent  :   the root node of BSTs to display parcels.
*   int partition   :   the weight partitions parcels in the bst to be displayed.
* RETURNS       :  void
*/
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

/*
* FUNCTION      : printSectionHigherThanWgt
* DESCRIPTION   :
*   This functoin prints out all the parcels heavier than a given weight within a BST.
* PARAMETERS    :
*   Parcel* parent  :   the root node of BSTs to display parcels.
*   int partition   :   the weight partitions parcels in the bst to be displayed.
* RETURNS       :  void
*/
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

/*
* FUNCTION      : printBSTInOrder
* DESCRIPTION   :
*   This functoin prints out all the parcels  within a BST in weight ascending order.
* PARAMETERS    :
*   Parcel* parent  :   the root node of BSTs to display parcels.
* RETURNS       :  void
*/
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

/*
* FUNCTION      : deleteBST
* DESCRIPTION   :
*   This functoin frees memory of all parcel nodes in a BST.
* PARAMETERS    :
*   Parcel* parent :  the root node of the BSTs to be deleted.
* RETURNS       :  void
*/
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
/*
* FUNCTION      : insertHashTableWithBST
* DESCRIPTION   :
*   This functoin creates a parcel node based on given data and inserts the node 
*    to a BST within a bucket of a hash table using destination as the key.
* PARAMETERS    :
*   Parcel* table[] :  the hash table to contain BSTs.
*   char* dest      :   the destination of a parcel
*   int weight      :   the weight of a parcel
*   float value     :   the value of a parcel
* RETURNS       :  void
*/
void insertHashTableWithBST(Parcel* table[], char* dest, int weight, float value)
{
    int hash = generateHash(dest);
    Parcel* newParcel = createNewParcel(dest, weight, value);
    table[hash] = insertParcelToBST(table[hash], newParcel);
}

/*
* FUNCTION      : deleteHashTable
* DESCRIPTION   : 
*   This functoin frees memory of all binary trees in the hash table.
* PARAMETERS    :
*   Parcel* table[] :  the hash table containing BSTs to be deleted.  
*   int tableSize   :   the size of the table
* RETURNS       :  void
*/
void deleteHashTable(Parcel* table[], int tableSize)
{
    for (int i = 0; i < tableSize; ++i)
    {
        deleteBST(table[i]);
    }
}
/*
* FUNCTION      : sumOfParcelsWgt
* DESCRIPTION   : Calculates the total weight of all parcels in a (BST)
* PARAMETERS    : Parcel* parent - A pointer to the root of the BST to calculate the total weight
* RETURNS       : int - The total weight of all parcels in the BST
*/
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
/*
* FUNCTION      : sumOfParcelsVal
* DESCRIPTION   : Calculates the total value of all parcels in a BST
* PARAMETERS    : Parcel* parent - A pointer to the root of the BST to calculate the total value
* RETURNS       : float - The total value of all parcels in the BST
*/
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
/*
* FUNCTION      : printTotalParcelWgtAndValForCountry
* DESCRIPTION   :
*   This functoin displays total weight and total value of parcels to a given destination (a country).
*   within the hash table.
* PARAMETERS    :
*   Parcel* table[] :   a pointer to the hash table containing all parcels. 
*   char* country   :   a string representing the destination country of parcels.
* RETURNS       :  void
*/
void printTotalParcelWgtAndValForCountry(Parcel* table[], char* country)
{
    int hash = generateHash(country);
    printf("\nDestination:\t%10s\t Total Weight: %8d gms\t Total: $%10.2f\n", 
        country, sumOfParcelsWgt(table[hash]), sumOfParcelsVal(table[hash]));
}

/*
* FUNCTION      : printLighterParcelsInCountry
* DESCRIPTION   :
*   This functoin displays parcels that are lighter than a given weight being delivered to a given country.
*   within the hash table
* PARAMETERS    :
*   Parcel* table[] :   a pointer to the hash table containing all parcels. 
*   char* country   :   a string representing the destination country of parcels.
*   int     wgt     :   the partition weight of the parcel that all displayed parcels are lighter than.
* RETURNS       :  void
*/
void printLighterParcelsInCountry(Parcel* table[], char* country, int wgt)
{
    int hash = generateHash(country);
    printf("\n/====================== Lighter than %d gms ===================/\n\n", wgt);
    printSectionLowerThanWgt(table[hash], wgt);
    
}

/*
* FUNCTION      : printHeavierParcelsInCountry
* DESCRIPTION   :
*   This functoin displays parcels that are heavier than a given weight being delivered to a given country 
*   within the hash table. 
* PARAMETERS    :
*   Parcel* table[] :   a pointer to the hash table containing all parcels.
*   char* country   :   a string representing the destination country of parcels.
*   int     wgt     :   the partition weight of the parcel that all displayed parcels are heavier than.
* RETURNS       :  void
*/
void printHeavierParcelsInCountry(Parcel* table[], char* country, int wgt)
{
    int hash = generateHash(country);
    printf("\n/====================== Heavier than %d gms ==================/\n\n", wgt);
    printSectionHigherThanWgt(table[hash], wgt);    
}

/*
* FUNCTION      : printHeavierParcelsInCountry
* DESCRIPTION   :
*   This functoin displays the cheapest and the most expensive parcels to a given destination within the hash table  
* PARAMETERS    :
*   Parcel* table[] :   a pointer to the hash table containing all parcels.
*   char* country   :   a string representing the destination country of parcels.
* RETURNS       :  void
*/
void printCheapestAndMostExpensiveParcelInCountry(Parcel* table[], char* country)
{
    int hash = generateHash(country);
    printf("\nThe Cheapest Parcel:\n");
    printParcel(findCheapestParcel(table[hash]));
    printf("\nThe Most Expensive Parcel:\n");
    printParcel(findMostExpensiveParcel(table[hash]));
}

/*
* FUNCTION      : validEnteredDestination
* DESCRIPTION   :
*   This functoin validates if an incoming country string exists in the hash table.
* PARAMETERS    :
*   Parcel* table[] :   a pointer to the hash table containing all parcels.
*   char* country   :   a string representing the destination country of parcels.
* RETURNS       :  
*   bool    : true, if the given country exist in the hash table. otherwise,
*             false.
*/
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
* PARAMETERS	: char newString[] ��a taget string.
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
