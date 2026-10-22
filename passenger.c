#include <stdio.h>
#include <string.h>


struct Passenger{
	char passengerID[20];
	char name[50];
	int age;
	char contactNumber[15];
};




void registerPassenger() {
	struct Passenger p;
	
	FILE *file = fopen("passengers.txt","a");
	
	if (file == NULL) {
		printf("Error opening file!\n");
		return;
	}

	printf("\n-----Register New Passenger-----\n\n");
	
	printf("Enter Passenger ID :  ");
	scanf("%s", p.passengerID);
	
	printf("Enter Name :  ");
	scanf(" %[^\n]s", p.name);

	printf("Enter Age:  ");
	scanf("%d", &p.age);

	printf("Enter Contact Number :  ");
	scanf("%s", p.contactNumber);
	
	
	fprintf(file, "%s %s %d %s\n", p.passengerID, p.name, p.age, p.contactNumber);
	
	fclose(file);
	
	printf("\n\nPassenger registered successfully!\n\n");
		
}





void displayPassengers(){
    struct Passenger p;
    
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
      struct Passenger p;
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
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 
 int main(){
    registerPassenger();
    return 0;
 }







