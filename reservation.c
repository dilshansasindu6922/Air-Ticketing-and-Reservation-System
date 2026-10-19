#include<stdio.h>
#include<string.h>
#define MAX 250

int rId[MAX];
char rPass[MAX][20];
char rFlights[MAX][20];
int rSeat[MAX];
char rClass[MAX][15];
char rStatus[MAX][15];
int rTicket[MAX];

int rCount = 0;

int isAdmin = 0;
char currentPass[20] = "";

void LoadReservation(){
  FILE *fp;

  rCount=0;
  fp=fopen("reservations.txt","r");

  if(fp==NULL){
    return;
  }

  while(!feof(fp) && rCount<MAX){
    if(fscanf(fp,"%d %s %s %d %s %s %d",&rId[rCount],rPass[rCount],rFlights[rCount],&rSeat[rCount],rClass[rCount],rStatus[rCount],&rTicket[rCount])!=7)
      break;

    rCount++;
  }
  fclose(fp);
}

void SaveReservation(){
  FILE *fp;
  int i;

  fp=fopen("reservations.txt","w");
  if(fp==NULL){
    printf("Can not Open reservations.txt\n");
    return;
  }
  for(i=0;i<rCount;i++){
    fprintf(fp,"%d %s %s %d %s %s %d\n",rId[i],rPass[i],rFlights[i],rSeat[i],rClass[i],rStatus[i],rTicket[i]);
  }
  fclose(fp);
}

int canAccess(int i){
  if(isAdmin)
    return 1;
  if(strcmp(rPass[i],currentPass)==0)
    return 1;
  return 0;
}

/* ===== NEW helper 1: find a reservation's position (-1 = not found) ===== */
int findIndex(int id){
  int i;
  for(i=0;i<rCount;i++){
    if(rId[i]==id)
      return i;
  }
  return -1;
}

/* ===== NEW helper 2: read passengers.txt ===== */
/* >>> CHANGE HERE (Student 1 - Passenger module) <<<
   This function reads Student 1's passenger file.
   1) File name      : change "passengers.txt" if Student 1 uses another name.
   2) Line format    : now each line is   id name ...   (e.g.  P001 Kamal)
                       if Student 1's line looks different (e.g. name first,
                       or more fields before the id) change the fscanf below.
   Returns 1 and copies the name into name[] if the passenger exists.      */
int passengerExists(char id[], char name[]){
  FILE *fp;
  char pid[20], pname[50], rest[200];

  fp=fopen("passengers.txt","r");              /* <<< file name */
  if(fp==NULL)
    return 0;

  while(fscanf(fp,"%s %s",pid,pname)==2){      /* <<< line format: id name */
    fgets(rest,200,fp);                        /* skips the rest of the line */
    if(strcmp(pid,id)==0){
      strcpy(name,pname);
      fclose(fp);
      return 1;
    }
  }
  fclose(fp);
  return 0;
}

/* ===== NEW helper 3: read flights.txt (seats and price are pointers) ===== */
/* >>> CHANGE HERE (Student 2 - Flight module) <<<
   This function reads Student 2's flight file.
   1) File name   : change "flights.txt" if Student 2 uses another name.
   2) Line format : now each line is
                    flightNo origin destination date time seats price ...
                    e.g.  UL225 Colombo Dubai 2026-11-20 08:30 5 85000
      If Student 2's order / fields are different, change the fscanf below.
      (the order of the variables must match the order in the file)
   seats = total seats of the flight, price = ticket price (economy).      */
int getFlight(char fno[], char origin[], char dest[], char date[], char ftime[], int *seats, float *price){
  FILE *fp;
  char f[20], rest[200];

  fp=fopen("flights.txt","r");                 /* <<< file name */
  if(fp==NULL)
    return 0;

  while(fscanf(fp,"%s %s %s %s %s %d %f",f,origin,dest,date,ftime,seats,price)==7){  /* <<< line format */
    fgets(rest,200,fp);
    if(strcmp(f,fno)==0){
      fclose(fp);
      return 1;
    }
  }
  fclose(fp);
  return 0;
}

/* Payment module -> changeStatus(id,"CONFIRMED"); Cancellation -> changeStatus(id,"CANCELLED") */
int changeStatus(int id, char newStatus[]){
  int i;

  LoadReservation();
  i=findIndex(id);
  if(i==-1)
    return 0;
  strcpy(rStatus[i],newStatus);
  SaveReservation();
  return 1;
}

/* ===================== helpers for CreateReservation ===================== */

/* next reservation id = biggest id + 1 (first one is 1001) */
int nextResId(){
  int i, max=1000;
  for(i=0;i<rCount;i++){
    if(rId[i]>max)
      max=rId[i];
  }
  return max+1;
}

/* is this seat already booked on this flight? (cancelled ones do not count) */
int seatTaken(char fno[], int seat){
  int i;
  for(i=0;i<rCount;i++){
    /* >>> CHANGE HERE if Student 4 uses another word than "CANCELLED" for cancelled reservations <<< */
    if(strcmp(rFlights[i],fno)==0 && rSeat[i]==seat && strcmp(rStatus[i],"CANCELLED")!=0)
      return 1;
  }
  return 0;
}

/* how many seats are booked on this flight (cancelled ones do not count) */
int countBooked(char fno[]){
  int i, c=0;
  for(i=0;i<rCount;i++){
    if(strcmp(rFlights[i],fno)==0 && strcmp(rStatus[i],"CANCELLED")!=0)
      c++;
  }
  return c;
}

/* ===================== PASSENGER MENU 1 : New Reservation ===================== */
void CreateReservation(){
  char pid[20], name[50], fno[20];
  char origin[30], dest[30], date[15], ftime[10];
  char classText[15];
  int totalSeats, seat, classChoice, i;
  float price, fare;

  LoadReservation();                         /* get the latest data from the file */

  if(rCount>=MAX){
    printf("Reservation list is full!\n");
    return;
  }

  printf("\n--- New Reservation ---\n");

  /* STEP 1: which passenger? */
  if(isAdmin){
    printf("Passenger ID : ");               /* admin books for any passenger */
    scanf("%s",pid);
  }
  else{
    strcpy(pid,currentPass);                 /* passenger books for himself */
  }

  if(!passengerExists(pid,name)){            /* uses Student 1's passenger file */
    printf("Passenger '%s' is not registered. Register the passenger first.\n",pid);
    return;
  }
  printf("Passenger    : %s (%s)\n",name,pid);

  /* STEP 2: which flight? */
  printf("Flight No    : ");
  scanf("%s",fno);
  if(!getFlight(fno,origin,dest,date,ftime,&totalSeats,&price)){   /* uses Student 2's flight file */
    printf("Flight '%s' not found.\n",fno);
    return;
  }
  printf("Flight       : %s -> %s  on %s at %s\n",origin,dest,date,ftime);

  if(countBooked(fno)>=totalSeats){
    printf("Sorry, this flight is fully booked.\n");
    return;
  }

  /* STEP 3: class */
  /* >>> CHANGE HERE: class names / number of classes. The words saved in the
     file are ECONOMY and BUSINESS - if you change them, change them in
     issueTicket() (fare rule) as well. <<< */
  printf("Class        : 1-Economy  2-Business : ");
  scanf("%d",&classChoice);
  if(classChoice==1)
    strcpy(classText,"ECONOMY");
  else if(classChoice==2)
    strcpy(classText,"BUSINESS");
  else{
    printf("Invalid class choice!\n");
    return;
  }

  /* >>> CHANGE HERE: fare rule (same rule as in issueTicket): Business = price x 2 <<< */
  if(classChoice==2)
    fare=price*2;
  else
    fare=price;
  printf("Fare         : Rs. %.2f\n",fare);

  /* STEP 4: seat - show the booked seats first */
  printf("Booked seats : ");
  for(i=0;i<rCount;i++){
    if(strcmp(rFlights[i],fno)==0 && strcmp(rStatus[i],"CANCELLED")!=0)
      printf("%d ",rSeat[i]);
  }
  printf("\n");

  printf("Seat number (1-%d) : ",totalSeats);
  scanf("%d",&seat);
  if(seat<1 || seat>totalSeats){
    printf("Invalid seat number!\n");
    return;
  }
  if(seatTaken(fno,seat)){
    printf("Seat %d is already taken!\n",seat);
    return;
  }

  /* STEP 5: put the new reservation at the end of the arrays and save */
  rId[rCount]=nextResId();
  strcpy(rPass[rCount],pid);
  strcpy(rFlights[rCount],fno);
  rSeat[rCount]=seat;
  strcpy(rClass[rCount],classText);
  /* >>> CHANGE HERE: first status. "PENDING" = waiting for payment. Student 4 (Payment)
     must change it to "CONFIRMED" using changeStatus(id,"CONFIRMED"). <<< */
  strcpy(rStatus[rCount],"PENDING");
  rTicket[rCount]=0;                         /* 0 = ticket not issued yet */

  printf("\nReservation successful! Your Reservation ID is %d\n",rId[rCount]);
  printf("Status = PENDING (payment needed to confirm)\n");

  rCount++;
  SaveReservation();
}

/* ===================== ADMIN MENU 2 : View Any Reservation ===================== */
void ViewReservation(){
  int id, i;

  printf("\nEnter Reservation ID : ");
  scanf("%d",&id);

  LoadReservation();
  i=findIndex(id);
  if(i==-1){
    printf("Reservation %d not found.\n",id);
    return;
  }
  if(!canAccess(i)){
    printf("This reservation is not yours!\n");
    return;
  }

  printf("\n--- Reservation Details ---\n");
  printf("Reservation ID : %d\n",rId[i]);
  printf("Passenger ID   : %s\n",rPass[i]);
  printf("Flight No      : %s\n",rFlights[i]);
  printf("Seat           : %d\n",rSeat[i]);
  printf("Class          : %s\n",rClass[i]);
  printf("Status         : %s\n",rStatus[i]);
  if(rTicket[i]==0)
    printf("Ticket No      : not issued\n");
  else
    printf("Ticket No      : TK%d\n",rTicket[i]);
}

void ListReservation(){
  int i, shown=0;

  LoadReservation();
  if(rCount==0){
    printf("\nNo reservations found.\n");
    return;
  }

  printf("\n%-8s %-10s %-8s %-6s %-10s %-10s %-8s\n","ResID","Passenger","Flight","Seat","Class","Status","Ticket");
  printf("------------------------------------------------------------------\n");
  for(i=0;i<rCount;i++){
    if(canAccess(i)){
      printf("%-8d %-10s %-8s %-6d %-10s %-10s %-8d\n",rId[i],rPass[i],rFlights[i],rSeat[i],rClass[i],rStatus[i],rTicket[i]);
      shown++;
    }
  }
  if(shown==0)
    printf("You have no reservations.\n");
}

/* ===================== ADMIN MENU 4 : Issue And Print Ticket ===================== */
void issueTicket(){
  int id, i, totalSeats;
  float price, fare;
  char name[50], origin[30], dest[30], date[15], ftime[10];

  printf("\nEnter Reservation ID : ");
  scanf("%d",&id);

  LoadReservation();
  i=findIndex(id);
  if(i==-1){
    printf("Reservation %d not found.\n",id);
    return;
  }
  if(!canAccess(i)){
    printf("This reservation is not yours!\n");
    return;
  }
  if(strcmp(rStatus[i],"CANCELLED")==0){
    printf("This reservation is cancelled. Cannot issue ticket.\n");
    return;
  }
  if(strcmp(rStatus[i],"CONFIRMED")!=0){
    printf("Payment not completed. Ticket can only be issued after payment.\n");
    return;
  }

  if(!passengerExists(rPass[i],name)){
    printf("Passenger details not found in passengers.txt\n");
    return;
  }
  if(!getFlight(rFlights[i],origin,dest,date,ftime,&totalSeats,&price)){
    printf("Flight details not found in flights.txt\n");
    return;
  }

  if(rTicket[i]==0){
    rTicket[i]=rId[i];
    SaveReservation();
  }

  /* >>> CHANGE HERE: fare rule. Business = Economy price x 2.
     Change the 2 (or the class names) to match your project. Use the SAME rule in CreateReservation. <<< */
  if(strcmp(rClass[i],"BUSINESS")==0)
    fare=price*2;
  else
    fare=price;

  printf("\n==========================================\n");
  printf("          E-TICKET / BOARDING PASS\n");
  printf("==========================================\n");
  printf(" Ticket No  : TK%d\n",rTicket[i]);
  printf(" Passenger  : %s (%s)\n",name,rPass[i]);
  printf(" Flight     : %s\n",rFlights[i]);
  printf(" From       : %s\n",origin);
  printf(" To         : %s\n",dest);
  printf(" Date/Time  : %s  %s\n",date,ftime);
  printf(" Seat       : %d\n",rSeat[i]);
  printf(" Class      : %s\n",rClass[i]);
  printf(" Fare       : Rs. %.2f\n",fare);
  printf("==========================================\n");
}

/* ===================== ADMIN MENU 3 : Mark As Paid ===================== */
void markAsPaid(){
  int id, i;

  printf("\nEnter Reservation ID : ");
  scanf("%d",&id);

  LoadReservation();
  i=findIndex(id);
  if(i==-1){
    printf("Reservation %d not found.\n",id);
    return;
  }
  if(strcmp(rStatus[i],"CANCELLED")==0){
    printf("This reservation is cancelled. Cannot mark as paid.\n");
    return;
  }
  if(strcmp(rStatus[i],"CONFIRMED")==0){
    printf("Reservation %d is already paid.\n",id);
    return;
  }

  changeStatus(id,"CONFIRMED");
  printf("Reservation %d marked as CONFIRMED (paid).\n",id);
}

void passengerMenu(){
  int choise;
  do{
    printf("\n=========== PASSENGER MENU (%s) ===========\n",currentPass);
    printf("1-New Reservation\n");
    printf("2-View My Reservation\n");
    printf("3-List My Reservations\n");
    printf("4-Print My Ticket\n");
    printf("0-Logout\n");
    printf("Enter your choise:");
    scanf("%d",&choise);

    switch(choise){
      case 1: CreateReservation(); break;
      case 2: ViewReservation();   break;
      case 3: ListReservation();   break;
      case 4: issueTicket();       break;
      case 0: break;
      default: printf("Invalid Entry!!!\n");
    }
  } while(choise!=0);
}

void adminMenu(){
  int choise;
  do{
    printf("\n=========== ADMIN MENU ===========\n");
    printf("1-List All Reservations\n");
    printf("2-View Any Reservation\n");
    printf("3-Mark As Paid\n");
    printf("4-Issue And Print Ticket\n");
    printf("5-New Reservation (for a passenger)\n");
    printf("0-Logout\n");
    printf("Enter your choise:");
    scanf("%d",&choise);

    switch(choise){
      case 1: ListReservation();   break;
      case 2: ViewReservation();   break;
      case 3: markAsPaid();        break;
      case 4: issueTicket();       break;
      case 5: CreateReservation(); break;
      case 0: break;
      default: printf("Invalid Entry!!!\n");
    }
  } while(choise!=0);
}

void reservationMenu(){
  if(isAdmin)
    adminMenu();
  else
    passengerMenu();
}

int main(){
  int choice;
  char pw[20];

  do{
    printf("\n======= AIRLINE RESERVATION SYSTEM =======\n");
    printf("1-Passenger Login\n");
    printf("2-Admin Login\n");
    printf("0-Exit\n");
    printf("Enter your choise:");
    scanf("%d",&choice);

    if(choice==1){
      printf("Enter your Passenger ID:");
      scanf("%s",currentPass);
      isAdmin=0;
      reservationMenu();
    }
    else if(choice==2){
      printf("Enter admin password:");
      scanf("%s",pw);
      if(strcmp(pw,"admin123")==0){
        isAdmin=1;
        strcpy(currentPass,"ADMIN");
        reservationMenu();
      }
      else
        printf("Wrong password!\n");
    }
    else if(choice!=0)
      printf("Invalid Entry!!!\n");
  } while(choice!=0);

  return 0;
} 

