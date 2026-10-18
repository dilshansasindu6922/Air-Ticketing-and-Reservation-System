#include <stdio.h>

int adminLogin(void);
void reportMenu(void);

void passengerMenu(void)
{
    printf("\n\n");
}

void flightMenu(void)
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
                printf("\n");
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

