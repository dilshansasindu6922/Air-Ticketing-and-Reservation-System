#include <stdio.h>

int adminLogin(void);
void reportMenu(void);
void loadFlights(void);
void addFlight(void);
void viewFlights(void);
void searchFlight(void);
void updateFlight(void);
void deleteFlight(void);
int adminLogin(void);
void reportMenu(void);
void flightMenu(void);
void displayWelcomeMessage(void);

void passengerMenu(void)
{
    printf("\n\n");
}

void reservationMenu(void)
{
    printf("\n\n");
}

void paymentMenu(void)
{
    printf("\n\n");
}

void adminMenu(void)
{
    int choice;

    do
    {
        printf("\n========== ADMIN MENU ==========\n");
        printf("1. Passenger Management\n");
        printf("2. Flight Management\n");
        printf("3. View Reservations\n");
        printf("4. Payment Records\n");
        printf("5. Reports\n");
        printf("0. Logout\n");
        printf("Enter choice: ");

        if (scanf("%d", &choice) != 1)
        {
            printf("Invalid input!\n");
            return;
        }

        switch (choice)
        {
            case 1:
                passengerMenu();
                break;

            case 2:
                flightMenu();
                break;

            case 3:
                reservationMenu();
                break;

            case 4:
                paymentMenu();
                break;

            case 5:
                reportMenu();
                break;

            case 0:
                printf("Logged out.\n");
                break;

            default:
                printf("Invalid choice!\n");
        }

    } while (choice != 0);
}

int main(void)
{
    int choice;

    do
    {
        printf("\n========================================\n");
        printf(" AIR TICKETING AND RESERVATION SYSTEM\n");
        printf("========================================\n");
        printf("1. Admin Login\n");
        printf("2. Passenger Login\n");
        printf("3. Register Passenger\n");
        printf("0. Exit\n");
        printf("Enter choice: ");

        if (scanf("%d", &choice) != 1)
        {
            printf("Invalid input!\n");
            return 1;
        }

        switch (choice)
        {
            case 1:
                if (adminLogin())
                {
                    adminMenu();
                }
                break;

            case 2:
                displayWelcomeMessage();
                reservationMenu();
                break;

            case 3:
                printf("\n");
                break;

            case 0:
                printf("Thank you for using our system!\n");
                break;

            default:
                printf("Invalid choice!\n");
        }

    } while (choice != 0);

    return 0;
}

void displayWelcomeMessage(void){
    printf("=========================================\n");
    printf("      Hello Air Ticketing System\n");
    printf("=========================================\n");
    printf(" Welcome to the Flight Management System\n");
    printf("=========================================\n");
}


void flightMenu(void)
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

}