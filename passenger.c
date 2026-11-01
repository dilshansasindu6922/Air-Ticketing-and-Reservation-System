
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>

#define PASSENGER_FILE "passengers.txt"

typedef struct
{
    char passengerID[20];
    char name[50];
    int age;
    char contactNumber[15];

} Passenger;


static int readPassenger(FILE *file, Passenger *p)
{
    char line[150];

    if (fgets(line, sizeof(line), file) == NULL)
        return 0;

    return sscanf(line, "%19[^|]|%49[^|]|%d|%14[^\n]",
                  p->passengerID,
                  p->name,
                  &p->age,
                  p->contactNumber) == 4;
}

static int findPassenger(const char *id, Passenger *found)
{
    FILE *file = fopen(PASSENGER_FILE, "r");
    Passenger p;

    if (file == NULL)
        return 0;

    while (readPassenger(file, &p))
    {
        if (strcmp(p.passengerID, id) == 0)
        {
            if (found != NULL)
                *found = p;

            fclose(file);
            return 1;
        }
    }

    fclose(file);
    return 0;
}


static int generatePassengerID(void)
{
    FILE *file = fopen(PASSENGER_FILE, "r");

    Passenger p;
    int maxID = 1000;
    int currentID;

    if (file == NULL)
        return 1001;

    while (readPassenger(file, &p))
    {
        currentID = atoi(p.passengerID);

        if (currentID > maxID)
            maxID = currentID;
    }

    fclose(file);

    return maxID + 1;
}


static int validContact(const char *number)
{
    int i;

    if (strlen(number) != 10)
        return 0;

    for (i = 0; i < 10; i++)
    {
        if (!isdigit((unsigned char)number[i]))
            return 0;
    }

    return 1;
}


void registerPassenger(void)
{
    Passenger p;
    FILE *file;
    int newID;

    printf("\n====================================\n");
    printf("       REGISTER NEW PASSENGER\n");
    printf("====================================\n");


    newID = generatePassengerID();

    sprintf(p.passengerID, "%d", newID);

    printf("Generated Passenger ID: %s\n", p.passengerID);



    printf("Enter Full Name: ");

    if (scanf(" %49[^\n]", p.name) != 1)
    {
        printf("Invalid name input!\n");
        return;
    }


    printf("Enter Age: ");

    if (scanf("%d", &p.age) != 1 ||
        p.age < 1 || p.age > 120)
    {
        printf("Invalid age!\n");
        return;
    }


    printf("Enter Contact Number: ");

    if (scanf("%14s", p.contactNumber) != 1)
    {
        printf("Invalid contact input!\n");
        return;
    }

    if (!validContact(p.contactNumber))
    {
        printf("Contact number must contain exactly 10 digits!\n");
        return;
    }



    file = fopen(PASSENGER_FILE, "r");

    if (file != NULL)
    {
        Passenger existing;

        while (readPassenger(file, &existing))
        {
            if (strcmp(existing.contactNumber,
                       p.contactNumber) == 0)
            {
                printf("\nThis contact number is already registered!\n");

                fclose(file);
                return;
            }
        }

        fclose(file);
    }

    file = fopen(PASSENGER_FILE, "a");

    if (file == NULL)
    {
        printf("Unable to open passenger file!\n");
        return;
    }

    fprintf(file, "%s|%s|%d|%s\n",
            p.passengerID,
            p.name,
            p.age,
            p.contactNumber);

    fclose(file);

    printf("\n====================================\n");
    printf(" PASSENGER REGISTERED SUCCESSFULLY!\n");
    printf("====================================\n");

    printf("Passenger ID : %s\n", p.passengerID);
    printf("Passenger Name : %s\n", p.name);
    printf("Contact Number : %s\n", p.contactNumber);

    printf("\nPlease remember your Passenger ID!\n");
}



void displayPassengers(void)
{
    FILE *file = fopen(PASSENGER_FILE, "r");
    Passenger p;
    int count = 0;

    if (file == NULL)
    {
        printf("No passenger records found!\n");
        return;
    }

    printf("\n========== REGISTERED PASSENGERS ==========\n");

    printf("%-15s %-25s %-8s %-15s\n",
           "ID", "NAME", "AGE", "CONTACT");

    while (readPassenger(file, &p))
    {
        printf("%-15s %-25s %-8d %-15s\n",
               p.passengerID,
               p.name,
               p.age,
               p.contactNumber);

        count++;
    }

    printf("\nTotal Passengers: %d\n", count);

    fclose(file);
}



void searchPassenger(void)
{
    char searchID[20];
    Passenger p;

    printf("\nEnter Passenger ID: ");

    if (scanf("%19s", searchID) != 1)
        return;

    if (findPassenger(searchID, &p))
    {
        printf("\nPassenger Found!\n");
        printf("Passenger ID : %s\n", p.passengerID);
        printf("Name         : %s\n", p.name);
        printf("Age          : %d\n", p.age);
        printf("Contact      : %s\n", p.contactNumber);
    }
    else
    {
        printf("\nPassenger not found!\n");
    }
}


int passengerExists(const char *id)
{
    return findPassenger(id, NULL);
}
