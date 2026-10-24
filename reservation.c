#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define RES_FILE "reservations.txt"
#define PASS_FILE "passengers.txt"
#define FLIGHT_FILE "flights.txt"

typedef struct {
    int bookingID;
    char passengerID[20];
    int flightID;
    int seat;
    char className[15];
    double price;
    char status[20];
    int ticketID;
} Reservation;

typedef struct {
    int id; 
    char airline[50]; 
    char origin[50]; 
    char destination[50];
    char date[20]; 
    char time[10]; 
    int seats; 
    double price;
} FlightInfo;

static int passengerExistsLocal(const char *id)
{
    FILE *fp = fopen(PASS_FILE, "r"); 
    
    char pid[20], name[50], contact[20]; 
    int age;

    if (!fp) 
      return 0;
    while (fscanf(fp, "%19s %49s %d %19s", pid, name, &age, contact) == 4)
        if (strcmp(pid, id) == 0) { fclose(fp); return 1; }
    
    fclose(fp); 
    return 0;
}

static int readFlight(int id, FlightInfo *f)
{
    FILE *fp = fopen(FLIGHT_FILE, "r"); 
    
    int ok = 0;

    if (!fp) 
      return 0;
    
    while (fscanf(fp, "%d|%49[^|]|%49[^|]|%49[^|]|%19[^|]|%9[^|]|%d|%lf\n",
        &f->id, f->airline, f->origin, f->destination, f->date, f->time, &f->seats, &f->price) == 8)
    {
        if (f->id == id) 
            { ok = 1; break; }
    }

    fclose(fp); 
    
    return ok;
}

static int readReservations(Reservation items[], int max)
{
    FILE *fp = fopen(RES_FILE, "r"); 
    
    int n = 0;

    if (!fp) 
      return 0;
    
    while (n < max && fscanf(fp, "%d|%19[^|]|%d|%d|%14[^|]|%lf|%19[^|]|%d\n",
        &items[n].bookingID, items[n].passengerID, &items[n].flightID, &items[n].seat,
        items[n].className, &items[n].price, items[n].status, &items[n].ticketID) == 8) n++;
    
    fclose(fp); 
    
    return n;
}

static int saveReservations(Reservation items[], int n)
{
    FILE *fp = fopen(RES_FILE, "w"); 
    
    int i;

    if (!fp)
    {
       printf("Cannot save reservations.txt.\n"); 
       return 0; 
    }

    for (i = 0; i < n; i++) fprintf(fp, "%d|%s|%d|%d|%s|%.2f|%s|%d\n",
        items[i].bookingID, items[i].passengerID, items[i].flightID, items[i].seat,
        items[i].className, items[i].price, items[i].status, items[i].ticketID);
    
    fclose(fp); 
    
    return 1;
}

static int nextBookingID(Reservation items[], int n)
{
    int i, max = 1000;

    for (i = 0; i < n; i++) 
      if (items[i].bookingID > max) max = items[i].bookingID;
      return max + 1;
}

static void createReservation(const char *loggedPassenger)
{
    Reservation items[500], r; 
    FlightInfo f; 
    int n, i, booked = 0;

    n = readReservations(items, 500);

    if (n >= 500) 
    {   
      printf("Reservation storage is full.\n"); 
      return; 
    }
    if (loggedPassenger && loggedPassenger[0]) 
        strcpy(r.passengerID, loggedPassenger);
    else {
        printf("Passenger ID: "); if (scanf("%19s", r.passengerID) != 1) return;
    }
    if (!passengerExistsLocal(r.passengerID)) 
    { 
        printf("Passenger not found. Register first.\n"); 
        return; 
    }

    printf("Flight ID: "); 
    
    if (scanf("%d", &r.flightID) != 1) 
        return;

    if (!readFlight(r.flightID, &f)) 
    { 
        printf("Flight not found.\n"); 
        return; 
    }

    for (i = 0; i < n; i++) 
      if (items[i].flightID == r.flightID && strcmp(items[i].status, "CANCELLED") != 0) 
        booked++;
      if (booked >= f.seats) { 
          printf("This flight is fully booked.\n"); 
          return;
      }
    
    printf("Seat number (1-%d): ", f.seats); 
    
    if (scanf("%d", &r.seat) != 1 || r.seat < 1 || r.seat > f.seats) 
    { 
        printf("Invalid seat.\n"); 
        return;
    }

    for (i = 0; i < n; i++) 
    if (items[i].flightID == r.flightID && items[i].seat == r.seat && strcmp(items[i].status, "CANCELLED") != 0) 
    {
       printf("That seat is already booked.\n"); 
       return; 
    }

    printf("Class (1 Economy / 2 Business): "); 
    { int c; if (scanf("%d", &c) != 1 || (c != 1 && c != 2)) { printf("Invalid class.\n"); return; } 
    strcpy(r.className, c == 1 ? "ECONOMY" : "BUSINESS"); r.price = c == 2 ? f.price * 2.0 : f.price; 
    }

    r.bookingID = nextBookingID(items, n); strcpy(r.status, "PENDING"); r.ticketID = 0; items[n++] = r;

    if (saveReservations(items, n)) 
      printf("Reservation created. Booking ID: %d | Fare: %.2f | Status: PENDING (payment required)\n", r.bookingID, r.price);
}

void reservationMenu(int isAdmin, const char *passengerID)
{
    int choice;
    do {
        Reservation items[500]; 
        int n, i;

        printf("\n========== RESERVATION MENU ==========\n");
        printf("1. Create Reservation\n2. View Reservations\n3. Issue / View Ticket\n0. Back\nChoice: ");

        if (scanf("%d", &choice) != 1) 
        { 
            int ch; while ((ch=getchar())!='\n' && ch!=EOF){}; 
            continue; 
        }

        if (choice == 1) createReservation(isAdmin ? "" : passengerID);
        else if (choice == 2) {
            n = readReservations(items, 500);

            if (!n) printf("No reservations found.\n");
            for (i = 0; i < n; i++) if (isAdmin || strcmp(items[i].passengerID, passengerID) == 0)
                printf("Booking:%d Passenger:%s Flight:%d Seat:%d Class:%s Fare:%.2f Status:%s Ticket:%d\n", items[i].bookingID, items[i].passengerID, items[i].flightID, items[i].seat, items[i].className, items[i].price, items[i].status, items[i].ticketID);
        
              } else if (choice == 3) {
            
                int id, found = 0; n = readReservations(items, 500);

            printf("Booking ID: "); if (scanf("%d", &id) != 1) 
            continue;

            for (i = 0; i < n; i++) if (items[i].bookingID == id && (isAdmin || strcmp(items[i].passengerID, passengerID) == 0)) {
                
              if (strcmp(items[i].status, "CONFIRMED") != 0) printf("Ticket cannot be issued until payment is confirmed. Status: %s\n", items[i].status);
                else 
                  { if (!items[i].ticketID) { items[i].ticketID = items[i].bookingID; saveReservations(items, n); }
                    printf("\n========== E-TICKET ==========\nTicket ID: TK%d\nBooking ID: %d\nPassenger ID: %s\nFlight ID: %d\nSeat: %d\nClass: %s\nFare: %.2f\nStatus: %s\n", items[i].ticketID, items[i].bookingID, items[i].passengerID, items[i].flightID, items[i].seat, items[i].className, items[i].price, items[i].status); }
                found = 1; 
                break;
            }
            if (!found) 
              printf("Reservation not found or access denied.\n");
              
        } else if (choice != 0) 
          printf("Invalid choice.\n");

    } while (choice != 0);
}
