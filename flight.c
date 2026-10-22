
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_FLIGHTS 100
#define FILE_NAME "flights.txt"

typedef struct {
    int flightID;
    char airline[50];
    char origin[50];
    char destination[50];
    char date[20];
    char time[10];
    int totalSeats;
    float ticketPrice;
} Flight;

static Flight flights[MAX_FLIGHTS];
static int flightCount = 0;

/* Function declarations */
void loadFlights(void);
void saveFlights(void);
void addFlight(void);
void viewFlights(void);
void searchFlight(void);
void updateFlight(void);
void deleteFlight(void);

static void clearInput(void)
{
    int ch;

    while ((ch = getchar()) != '\n' && ch != EOF) {
    }
}

static int findFlightIndex(int id)
{
    int i;

    for (i = 0; i < flightCount; i++) {
        if (flights[i].flightID == id) {
            return i;
        }
    }

    return -1;
}

/* Load flight details from file */
void loadFlights(void)
{
    FILE *file = fopen(FILE_NAME, "r");

    if (file == NULL) {
        flightCount = 0;
        return;
    }

    flightCount = 0;

    while (flightCount < MAX_FLIGHTS) {
        Flight f;

        int result = fscanf(
            file,
            "%d|%49[^|]|%49[^|]|%49[^|]|%19[^|]|%9[^|]|%d|%f",
            &f.flightID,
            f.airline,
            f.origin,
            f.destination,
            f.date,
            f.time,
            &f.totalSeats,
            &f.ticketPrice
        );

        if (result != 8) {
            break;
        }

        flights[flightCount] = f;
        flightCount++;
    }

    fclose(file);
}

/* Save flight details to file */
void saveFlights(void)
{
    FILE *file = fopen(FILE_NAME, "w");
    int i;

    if (file == NULL) {
        printf("Error: Cannot save flight data.\n");
        return;
    }

    for (i = 0; i < flightCount; i++) {
        fprintf(
            file,
            "%d|%s|%s|%s|%s|%s|%d|%.2f\n",
            flights[i].flightID,
            flights[i].airline,
            flights[i].origin,
            flights[i].destination,
            flights[i].date,
            flights[i].time,
            flights[i].totalSeats,
            flights[i].ticketPrice
        );
    }

    fclose(file);
}

/* Add a new flight */
void addFlight(void)
{
    Flight f;

    printf("\n--- ADD NEW FLIGHT ---\n");

    if (flightCount >= MAX_FLIGHTS) {
        printf("Flight storage is full.\n");
        return;
    }

    printf("Enter Flight ID: ");

    if (scanf("%d", &f.flightID) != 1) {
        printf("Invalid Flight ID.\n");
        clearInput();
        return;
    }
    clearInput();

    if (f.flightID <= 0) {
        printf("Flight ID must be positive.\n");
        return;
    }

    if (findFlightIndex(f.flightID) != -1) {
        printf("This Flight ID already exists.\n");
        return;
    }

    printf("Enter Airline Name: ");
    if (scanf(" %49[^\n]", f.airline) != 1) {
        printf("Invalid airline name.\n");
        clearInput();
        return;
    }
    clearInput();

    printf("Enter Origin: ");
    if (scanf(" %49[^\n]", f.origin) != 1) {
        printf("Invalid origin.\n");
        clearInput();
        return;
    }
    clearInput();

    printf("Enter Destination: ");
    if (scanf(" %49[^\n]", f.destination) != 1) {
        printf("Invalid destination.\n");
        clearInput();
        return;
    }
    clearInput();

    printf("Enter Date (YYYY-MM-DD): ");
    if (scanf("%19s", f.date) != 1) {
        printf("Invalid date.\n");
        clearInput();
        return;
    }
    clearInput();

    printf("Enter Time (HH:MM): ");
    if (scanf("%9s", f.time) != 1) {
        printf("Invalid time.\n");
        clearInput();
        return;
    }
    clearInput();

    printf("Enter Total Seats: ");
    if (scanf("%d", &f.totalSeats) != 1 ||
        f.totalSeats <= 0) {
        printf("Seats must be greater than zero.\n");
        clearInput();
        return;
    }
    clearInput();

    printf("Enter Ticket Price: ");
    if (scanf("%f", &f.ticketPrice) != 1 ||
        f.ticketPrice < 0) {
        printf("Ticket price cannot be negative.\n");
        clearInput();
        return;
    }
    clearInput();

    flights[flightCount] = f;
    flightCount++;

    saveFlights();

    printf("\nFlight added successfully!\n");
}

/* View all flights */
void viewFlights(void)
{
    int i;

    if (flightCount == 0) {
        printf("\nNo flights available.\n");
        return;
    }

    printf("\n========== ALL FLIGHTS ==========\n");

    for (i = 0; i < flightCount; i++) {
        printf("\nFlight ID    : %d\n", flights[i].flightID);
        printf("Airline      : %s\n", flights[i].airline);
        printf("Origin       : %s\n", flights[i].origin);
        printf("Destination  : %s\n", flights[i].destination);
        printf("Date         : %s\n", flights[i].date);
        printf("Time         : %s\n", flights[i].time);
        printf("Total Seats  : %d\n", flights[i].totalSeats);
        printf("Ticket Price : %.2f\n", flights[i].ticketPrice);
        printf("---------------------------------\n");
    }
}

/* Search for a flight */
void searchFlight(void)
{
    int id;
    int index;

    printf("\nEnter Flight ID to search: ");

    if (scanf("%d", &id) != 1) {
        printf("Invalid Flight ID.\n");
        clearInput();
        return;
    }
    clearInput();

    index = findFlightIndex(id);

    if (index == -1) {
        printf("Flight not found.\n");
        return;
    }

    printf("\n--- FLIGHT FOUND ---\n");
    printf("Flight ID    : %d\n", flights[index].flightID);
    printf("Airline      : %s\n", flights[index].airline);
    printf("Origin       : %s\n", flights[index].origin);
    printf("Destination  : %s\n", flights[index].destination);
    printf("Date         : %s\n", flights[index].date);
    printf("Time         : %s\n", flights[index].time);
    printf("Total Seats  : %d\n", flights[index].totalSeats);
    printf("Ticket Price : %.2f\n", flights[index].ticketPrice);
}

/* Update flight details */
void updateFlight(void)
{
    int id;
    int index;
    Flight updated;

    printf("\nEnter Flight ID to update: ");

    if (scanf("%d", &id) != 1) {
        printf("Invalid Flight ID.\n");
        clearInput();
        return;
    }
    clearInput();

    index = findFlightIndex(id);

    if (index == -1) {
        printf("Flight not found.\n");
        return;
    }

    updated = flights[index];

    printf("Enter New Airline Name: ");
    if (scanf(" %49[^\n]", updated.airline) != 1) {
        printf("Invalid airline name.\n");
        clearInput();
        return;
    }
    clearInput();

    printf("Enter New Origin: ");
    if (scanf(" %49[^\n]", updated.origin) != 1) {
        printf("Invalid origin.\n");
        clearInput();
        return;
    }
    clearInput();

    printf("Enter New Destination: ");
    if (scanf(" %49[^\n]", updated.destination) != 1) {
        printf("Invalid destination.\n");
        clearInput();
        return;
    }
    clearInput();

    printf("Enter New Date (YYYY-MM-DD): ");
    if (scanf("%19s", updated.date) != 1) {
        printf("Invalid date.\n");
        clearInput();
        return;
    }
    clearInput();

    printf("Enter New Time (HH:MM): ");
    if (scanf("%9s", updated.time) != 1) {
        printf("Invalid time.\n");
        clearInput();
        return;
    }
    clearInput();

    printf("Enter New Total Seats: ");
    if (scanf("%d", &updated.totalSeats) != 1 ||
        updated.totalSeats <= 0) {
        printf("Seats must be greater than zero.\n");
        clearInput();
        return;
    }
    clearInput();

    printf("Enter New Ticket Price: ");
    if (scanf("%f", &updated.ticketPrice) != 1 ||
        updated.ticketPrice < 0) {
        printf("Invalid ticket price.\n");
        clearInput();
        return;
    }
    clearInput();

    flights[index] = updated;
    saveFlights();

    printf("Flight updated successfully!\n");
}

/* Delete a flight */
void deleteFlight(void)
{
    int id;
    int index;
    int i;

    printf("\nEnter Flight ID to delete: ");

    if (scanf("%d", &id) != 1) {
        printf("Invalid Flight ID.\n");
        clearInput();
        return;
    }
    clearInput();

    index = findFlightIndex(id);

    if (index == -1) {
        printf("Flight not found.\n");
        return;
    }

    for (i = index; i < flightCount - 1; i++) {
        flights[i] = flights[i + 1];
    }

    flightCount--;

    saveFlights();

    printf("Flight deleted successfully!\n");
}
