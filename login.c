#include <stdio.h>
#include <string.h>

int adminLogin(void)
{
    char username[30];
    char password[30];

    printf("\n===== ADMIN LOGIN =====\n");

    printf("Username: ");
    scanf("%29s", username);

    printf("Password: ");
    scanf("%29s", password);

    if (strcmp(username, "admin") == 0 &&
        strcmp(password, "admin123") == 0)
    {
        printf("Admin login successful!\n");
        return 1;
    }

    printf("Invalid username or password!\n");
    return 0;
}
