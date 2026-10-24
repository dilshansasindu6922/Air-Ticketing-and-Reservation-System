#include <stdio.h>

int adminLogin(void);
void reportMenu(void);

void flightMenu(void);
void loadFlights(void);
void addFlight(void);
void viewFlights(void);
void searchFlight(void);
void updateFlight(void);
void deleteFlight(void);

void reservationMenu(int isAdmin, const char *passengerID);

void registerPassenger(void);
void displayPassengers(void);
void searchPassenger(void);
void passengerMenu(void);
void paymentadminMenu(void);
void paymentcustomerMenu(void);

void processPayment(void);
void viewPaymentDetails(void);
void cancelTicket(void);
void viewCancellationDetails(void);


static void clearInput(void)
{
    int ch;
    while ((ch = getchar()) != '\n' && ch != EOF) { }
}

void adminMenu(void)
{
    int choice;

    do
    {
        printf("\n========== ADMIN MENU ==========\n");
        printf("1. Passenger Management\n");
        printf("2. Flight Management\n");
        printf("3. Reservations Management\n");
        printf("4. Payment Management\n");
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
                {
                int pChoice;
                do {
                    printf("\n--- PASSENGER MANAGEMENT ---\n1. Register Passenger\n2. Display Passengers\n3. Search Passenger\n0. Back\nChoice: ");
                    if (scanf("%d", &pChoice) != 1) { clearInput(); pChoice = -1; }
                    switch (pChoice) {
                        case 1: 
                            registerPassenger(); 
                            break;
                        case 2: 
                            displayPassengers(); 
                            break;
                        case 3: 
                            searchPassenger(); 
                            break;
                        case 0: 
                            break;
                        default: 
                            printf("Invalid choice.\n");
                    }
                } while (pChoice != 0);
                break;
            }
                break;

            case 2:
                flightMenu();
                break;

            case 3:
                reservationMenu(1, "Admin");
                break;

            case 4:
                paymentadminMenu();
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
                passengerMenu();
                break;

            case 3:
                registerPassenger();
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

void flightMenu(void)
{
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
                break;

            default:
                printf("Invalid choice. Try again.\n");
        }

    } while (choice != 0);

}

void passengerMenu(void)
{
    char passengerID[20];
    int choice;
    printf("\nEnter your Passenger ID: ");
    if (scanf("%19s", passengerID) != 1) 
    { 
        clearInput(); 
        return; 
    }

    do {
        printf("\n========== PASSENGER MENU (%s) ==========\n", passengerID);
        printf("1. View Flights\n2. Make Reservation\n3. View My Reservations / Ticket\n4. Payment & Cancellation\n0. Logout\nChoice: ");
        if (scanf("%d", &choice) != 1) 
        { 
            clearInput(); 
            continue; 
        }
        switch (choice) {
            case 1: 
                loadFlights(); 
                viewFlights(); 
                break;
            case 2: 
                reservationMenu(0, passengerID);
                break;
            case 3: 
                reservationMenu(0, passengerID);
                break;
            case 4: 
                paymentcustomerMenu(); 
                break;
            case 0: 
                break;
            default: 
                printf("Invalid choice.\n");
        }
    } while (choice != 0);
}
