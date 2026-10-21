#include <stdio.h>

void reportMenu(void)
{
    int choice;

    printf("\n========== REPORTS ==========\n");
    printf("1. Booking Report\n");
    printf("2. Flight Statistics\n");
    printf("3. Revenue Report\n");
    printf("0. Back to Main Menu\n");
    printf("Enter choice: ");

    if (scanf("%d", &choice) != 1)
    {
        printf("Invalid input!\n");
        return;
    }
//hello
    switch (choice)
    {
        case 1:
            printf("\n");
            break;

        case 2:
            printf("\n");
            break;

        case 3:
            printf("\n");
            break;

        case 0:
            printf("Returning to main menu...\n");
            break;

        default:
            printf("Invalid choice!\n");
    }
}

