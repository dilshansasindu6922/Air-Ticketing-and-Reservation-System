#include <stdio.h>
void displayWelcomeMessage(void){
    printf("=========================================\n");
    printf("      Hello Air Ticketing System\n");
    printf("=========================================\n");
    printf(" Welcome to the Flight Management System\n");
    printf("=========================================\n");
}

/* functions implemented in flight.c */
void loadFlights(void);
void addFlight(void);
void viewFlights(void);
void searchFlight(void);
void updateFlight(void);
void deleteFlight(void);

int main(void)
{
    displayWelcomeMessage();
    
    int choice;

    loadFlights();

    do {
        printf("\n===============================\n");
        printf(" AIR TICKETING SYSTEM\n");
        printf(" FLIGHT MANAGEMENT MODULE\n");
        printf("===============================\n");
        printf("1. Add Flight\n");
        printf("2. View All Flights\n");
        printf("3. Search Flight\n");
        printf("4. Update Flight\n");
        printf("5. Delete Flight\n");
        printf("0. Exit\n");
        printf("Enter your choice: ");

        if (scanf("%d", &choice) != 1) {
            printf("Invalid input. Enter a number.\n");

            int ch;
            while ((ch = getchar()) != '\n' && ch != EOF) {
            }

            continue;
        }
        int ch;

        while ((ch = getchar()) != '\n' && ch != EOF) {
        }

        switch (choice) {
            case 1:
                addFlight();
                break;

            case 2:
                viewFlights();
                break;

            case 3:
                searchFlight();
                break;

            case 4:
                updateFlight();
                break;

            case 5:
                deleteFlight();
                break;

            case 0:
                printf("Thank you! Goodbye.\n");
                break;

            default:
                printf("Invalid choice. Try again.\n");
        }

    } while (choice != 0);

    return 0;
}