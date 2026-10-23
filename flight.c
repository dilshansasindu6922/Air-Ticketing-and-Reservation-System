
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

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

static int findFlightIndex(int id);
static int readLine(const char *prompt, char *buffer, size_t size);
static int readInt(const char *prompt, int *value);
static int readPrice(const char *prompt, float *value);
static int readText(const char *prompt, char *buffer, size_t size);
static int validDate(const char *date);
static int validTime(const char *time);
static void showFlight(const Flight *f);

/* Read a complete line safely */
static int readLine(const char *prompt, char *buffer, size_t size)
{
    size_t len;
    int ch;

    printf("%s", prompt);
    fflush(stdout);

    if (fgets(buffer, (int)size, stdin) == NULL)
        return 0;

    len = strlen(buffer);

    if (len > 0 && buffer[len - 1] == '\n') {
        buffer[len - 1] = '\0';
    } else {
        while ((ch = getchar()) != '\n' && ch != EOF) {
        }
        printf("Input is too long. Please try again.\n");
        return 0;
    }

    len = strlen(buffer);

    if (len > 0 && buffer[len - 1] == '\r')
        buffer[len - 1] = '\0';

    return 1;
}

/* Read an integer without accepting extra characters */
static int readInt(const char *prompt, int *value)
{
    char input[100];
    char *end;
    long number;

    if (!readLine(prompt, input, sizeof(input)))
        return 0;

    errno = 0;
    number = strtol(input, &end, 10);

    while (isspace((unsigned char)*end))
        end++;

    if (input[0] == '\0' || end == input ||
        *end != '\0' || errno == ERANGE ||
        number < -2147483647L - 1 ||
        number > 2147483647L) {
        printf("Invalid number. Please enter an integer.\n");
        return 0;
    }

    *value = (int)number;
    return 1;
}

/* Read a non-negative ticket price */
static int readPrice(const char *prompt, float *value)
{
    char input[100];
    char *end;
    float price;

    if (!readLine(prompt, input, sizeof(input)))
        return 0;

    errno = 0;
    price = strtof(input, &end);

    while (isspace((unsigned char)*end))
        end++;

    if (input[0] == '\0' || end == input ||
        *end != '\0' || errno == ERANGE ||
        price < 0.0f || price > 1000000000.0f) {
        printf("Invalid price. Enter a value from 0 to 1000000000.\n");
        return 0;
    }

    *value = price;
    return 1;
}

/* Read text and reject empty values and file separators */
static int readText(const char *prompt, char *buffer, size_t size)
{
    size_t i;
    int nonSpace = 0;

    if (!readLine(prompt, buffer, size))
        return 0;

    for (i = 0; buffer[i] != '\0'; i++) {
        if (buffer[i] == '|') {
            printf("The character '|' is not allowed.\n");
            return 0;
        }

        if (!isspace((unsigned char)buffer[i]))
            nonSpace = 1;
    }

    if (!nonSpace) {
        printf("This field cannot be empty.\n");
        return 0;
    }

    return 1;
}

/* Validate YYYY-MM-DD, including leap years */
static int validDate(const char *date)
{
    int year, month, day;
    int daysInMonth[] = {
        31, 28, 31, 30, 31, 30,
        31, 31, 30, 31, 30, 31
    };

    if (strlen(date) != 10 ||
        date[4] != '-' || date[7] != '-')
        return 0;

    if (!isdigit((unsigned char)date[0]) ||
        !isdigit((unsigned char)date[1]) ||
        !isdigit((unsigned char)date[2]) ||
        !isdigit((unsigned char)date[3]) ||
        !isdigit((unsigned char)date[5]) ||
        !isdigit((unsigned char)date[6]) ||
        !isdigit((unsigned char)date[8]) ||
        !isdigit((unsigned char)date[9]))
        return 0;

    if (sscanf(date, "%4d-%2d-%2d",
               &year, &month, &day) != 3)
        return 0;

    if (year < 1 || month < 1 || month > 12)
        return 0;

    if (year % 400 == 0 ||
        (year % 4 == 0 && year % 100 != 0))
        daysInMonth[1] = 29;

    return day >= 1 && day <= daysInMonth[month - 1];
}

/* Validate 24-hour HH:MM */
static int validTime(const char *time)
{
    int hour, minute;

    if (strlen(time) != 5 || time[2] != ':')
        return 0;

    if (!isdigit((unsigned char)time[0]) ||
        !isdigit((unsigned char)time[1]) ||
        !isdigit((unsigned char)time[3]) ||
        !isdigit((unsigned char)time[4]))
        return 0;

    if (sscanf(time, "%2d:%2d", &hour, &minute) != 2)
        return 0;

    return hour >= 0 && hour <= 23 &&
           minute >= 0 && minute <= 59;
}

static int findFlightIndex(int id)
{
    int i;

    for (i = 0; i < flightCount; i++) {
        if (flights[i].flightID == id)
            return i;
    }

    return -1;
}

/* Load flights from file */
void loadFlights(void)
{
    FILE *file = fopen(FILE_NAME, "r");

    flightCount = 0;

    if (file == NULL)
        return;

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

        if (result != 8)
            break;

        if (f.flightID <= 0 || f.totalSeats <= 0 ||
            f.ticketPrice < 0.0f ||
            !validDate(f.date) || !validTime(f.time) ||
            findFlightIndex(f.flightID) != -1)
            continue;

        flights[flightCount++] = f;
    }

    fclose(file);
}

/* Save flights to file */
void saveFlights(void)
{
    FILE *file = fopen(FILE_NAME, "w");
    int i;

    if (file == NULL) {
        printf("Error: Cannot save flight data.\n");
        return;
    }

    for (i = 0; i < flightCount; i++) {
        fprintf(file, "%d|%s|%s|%s|%s|%s|%d|%.2f\n",
                flights[i].flightID,
                flights[i].airline,
                flights[i].origin,
                flights[i].destination,
                flights[i].date,
                flights[i].time,
                flights[i].totalSeats,
                flights[i].ticketPrice);
    }

    fclose(file);
}

/* Display one flight */
static void showFlight(const Flight *f)
{
    printf("\nFlight ID    : %d\n", f->flightID);
    printf("Airline      : %s\n", f->airline);
    printf("Origin       : %s\n", f->origin);
    printf("Destination  : %s\n", f->destination);
    printf("Date         : %s\n", f->date);
    printf("Time         : %s\n", f->time);
    printf("Total Seats  : %d\n", f->totalSeats);
    printf("Ticket Price : %.2f\n", f->ticketPrice);
}

/* Add a new flight */
void addFlight(void)
{
    Flight f = {0};

    printf("\n--- ADD NEW FLIGHT ---\n");

    if (flightCount >= MAX_FLIGHTS) {
        printf("Flight storage is full.\n");
        return;
    }

    if (!readInt("Enter Flight ID: ", &f.flightID))
        return;

    if (f.flightID <= 0) {
        printf("Flight ID must be positive.\n");
        return;
    }

    if (findFlightIndex(f.flightID) != -1) {
        printf("This Flight ID already exists.\n");
        return;
    }

    if (!readText("Enter Airline Name: ", f.airline,
                  sizeof(f.airline))) return;
    if (!readText("Enter Origin: ", f.origin,
                  sizeof(f.origin))) return;
    if (!readText("Enter Destination: ", f.destination,
                  sizeof(f.destination))) return;

    if (!readText("Enter Date (YYYY-MM-DD): ", f.date,
                  sizeof(f.date))) return;
    if (!validDate(f.date)) {
        printf("Invalid date. Example: 2026-12-15\n");
        return;
    }

    if (!readText("Enter Time (HH:MM): ", f.time,
                  sizeof(f.time))) return;
    if (!validTime(f.time)) {
        printf("Invalid time. Use 00:00 to 23:59.\n");
        return;
    }

    if (!readInt("Enter Total Seats: ", &f.totalSeats))
        return;

    if (f.totalSeats <= 0) {
        printf("Seats must be greater than zero.\n");
        return;
    }

    if (!readPrice("Enter Ticket Price: ", &f.ticketPrice))
        return;

    flights[flightCount++] = f;
    saveFlights();

    printf("Flight added successfully!\n");
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
        showFlight(&flights[i]);
        printf("---------------------------------\n");
    }
}

/* Search for a flight */
void searchFlight(void)
{
    int id, index;

    if (!readInt("\nEnter Flight ID to search: ", &id))
        return;

    if (id <= 0) {
        printf("Flight ID must be positive.\n");
        return;
    }

    index = findFlightIndex(id);

    if (index == -1) {
        printf("Flight not found.\n");
        return;
    }

    printf("\n--- FLIGHT FOUND ---\n");
    showFlight(&flights[index]);
}

/* Update flight details */
void updateFlight(void)
{
    int id, index;
    Flight updated;

    if (!readInt("\nEnter Flight ID to update: ", &id))
        return;

    if (id <= 0) {
        printf("Flight ID must be positive.\n");
        return;
    }

    index = findFlightIndex(id);

    if (index == -1) {
        printf("Flight not found.\n");
        return;
    }

    updated = flights[index];

    printf("Enter New Airline Name: ");
    if (!readText("Enter New Airline Name: ", updated.airline,
                  sizeof(updated.airline))) return;

    if (!readText("Enter New Origin: ", updated.origin,
                  sizeof(updated.origin))) return;
    if (!readText("Enter New Destination: ", updated.destination,
                  sizeof(updated.destination))) return;

    if (!readText("Enter New Date (YYYY-MM-DD): ", updated.date,
                  sizeof(updated.date))) return;
    if (!validDate(updated.date)) {
        printf("Invalid date. Example: 2026-12-15\n");
        return;
    }

    if (!readText("Enter New Time (HH:MM): ", updated.time,
                  sizeof(updated.time))) return;
    if (!validTime(updated.time)) {
        printf("Invalid time. Use 00:00 to 23:59.\n");
        return;
    }

    if (!readInt("Enter New Total Seats: ", &updated.totalSeats))
        return;

    if (updated.totalSeats <= 0) {
        printf("Seats must be greater than zero.\n");
        return;
    }

    if (!readPrice("Enter New Ticket Price: ", &updated.ticketPrice))
        return;

    flights[index] = updated;
    saveFlights();

    printf("Flight updated successfully!\n");
}

/* Delete a flight */
void deleteFlight(void)
{
    int id, index, i;

    if (!readInt("\nEnter Flight ID to delete: ", &id))
        return;

    if (id <= 0) {
        printf("Flight ID must be positive.\n");
        return;
    }

    index = findFlightIndex(id);

    if (index == -1) {
        printf("Flight not found.\n");
        return;
    }

    for (i = index; i < flightCount - 1; i++)
        flights[i] = flights[i + 1];

    flightCount--;
    saveFlights();

    printf("Flight deleted successfully!\n");
}
