#include <stdio.h>
#include <string.h>


typedef struct{
	char passengerID[20];
	char name[50];
	int age;
	char contactNumber[15];
} Passenger ;

static int findPassenger(const char *id, Passenger *found)
{
    FILE *file = fopen("passengers.txt", "r");
    Passenger p;
    
    if (file == NULL){
        printf("Error opening file!\n");
        return 0;
    }
    while (fscanf(file, "%19s %49s %d %14s", p.passengerID, p.name, &p.age, p.contactNumber) == 4) {
        if (strcmp(p.passengerID, id) == 0) 
        { if (found) *found = p; 
            fclose(file); 
            return 1; 
        }
    }

    fclose(file); 
    return 0;
}


void registerPassenger() {
	Passenger p;

    FILE *file = fopen("passengers.txt","a");
	
	if (file == NULL) {
		printf("Error opening file!\n");
		return;
	}

	printf("\n-----Register New Passenger-----\n\n");
	
	printf("Enter Passenger ID :  ");
	if (scanf("%19s", p.passengerID) != 1) 
        return;
    
    if (findPassenger(p.passengerID, NULL)) 
    { 
        printf("That Passenger ID already exists.\n"); 
        return; 
    }
	
	printf("Enter Name :  ");
	if (scanf(" %[^\n]s", p.name) != 1) {
		printf("Error reading name.\n");
		return;
	}

	printf("Enter Age:  ");
	if(scanf("%d", &p.age)!= 1 || p.age < 1 || p.age > 120) {
        printf("Invalid age input. Age must be between 1 and 120.\n");
        return;
    }
	printf("Enter Contact Number (07XXXXXXXX):  ");
	scanf("%s", p.contactNumber);
	if (strlen(p.contactNumber) != 10)
    {
        printf("Invalid Contact Number! Enter exactly 10 digits.\n");
        return;
    }
	
	fprintf(file, "%s %s %d %s\n", p.passengerID, p.name, p.age, p.contactNumber);
	
	fclose(file);
	
	printf("\n\nPassenger registered successfully!\n\n");
		
}


void displayPassengers(){
    Passenger p;
    
    FILE *file = fopen("passengers.txt","r");
    
    if (file == NULL){
        printf("\nNo passenger records found!  (file could not be opened)\n");
        return;
    }
    
    
    printf("\n-------------Registered Passengers List--------------\n");
    printf("%-15s %-20s %-5s %-15s\n", "ID", "Name", "Age", "Contact");
    printf("-----------------------------------------------------\n");
    
    while (fscanf(file, "%s %s %d %s",p.passengerID, p.name, &p.age, p.contactNumber) == 4){
        printf("%-15s %-20s %-5d %-15s\n", p.passengerID, p.name, p.age, p.contactNumber);
    }


fclose(file);
printf("------------------------------------------------------\n");
}



 void searchPassenger(){
      Passenger p;
      char searchID[20];
      int found = 0;
      
      FILE *file = fopen("passengers.txt","r");
      
      if (file == NULL){
          printf("\nNo passenger records found to search!\n");
          return;
      }
 
      printf("--------Search Passenger---------\n");
      printf("Enter passengerID to search : ");
      scanf("%s",searchID);
      
      while (fscanf(file, "%s %s %d %s", p.passengerID, p.name, &p.age, p.contactNumber) == 4){
      
          if (strcmp(p.passengerID, searchID) == 0){
              printf("\nPassenger Found Successfully!\n");
              printf("-------------------------------\n");
              printf("Passenger ID   : %s\n", p.passengerID);
              printf("Name           : %s\n", p.name);
              printf("Age            : %d\n", p.age);
              printf("Contact Number : %s\n", p.contactNumber);
              printf("-------------------------------\n");
              found = 1;
              break;
          }
      }
 
 
 if (!found){
    printf("\nPassenger with ID '%s' not found!\n",searchID);
 }
 
 fclose(file);
 }
