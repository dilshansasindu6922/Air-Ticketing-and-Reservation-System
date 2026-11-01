#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define RESERVATION_FILE "reservations.txt"
#define PAYMENT_FILE "payments.txt"
#define CANCELLATION_FILE "cancellations.txt"
#define REFUND_PERCENT 90.0
#define MAX_RESERVATIONS 500
#define MAX_CANCELLATIONS 500
typedef struct {
    int bookingID;
    char passengerID[20];
    int flightID;
    int seat;
    char className[15];
    double ticketPrice;
    char status[20];
    int ticketID;
} Reservation;

typedef struct {
    int paymentID;
    int bookingID;
    double amount;
    char method[20];
    char status[20];
} Payment;

typedef struct {
    int cancellationID;
    int bookingID;
    double refundAmount;
    char status[20];
} Cancellation;


int findReservation(int bookingID, Reservation *r);
int hasSuccessfulPayment(int bookingID);


void paymentcustomerMenu(const char *passengerID);
void processPayment(const char *passengerID);
void viewPaymentDetails(const char *passengerID);
void requestCancellation(const char *passengerID);
void viewMyCancellationStatus(const char *passengerID);


void adminMenu(void);
int getpendingcancellationcount(void);
void manageCancellationRequests(void);
void viewAllPayments(void);
void viewAllCancellations(void);


static int readAllReservations(Reservation items[], int maxItems)
{
    FILE *fp = fopen(RESERVATION_FILE, "r");
    int count = 0;

    if (fp == NULL) {
        return 0;
    }

    while (count < maxItems &&
           fscanf(fp, "%d|%19[^|]|%d|%d|%14[^|]|%lf|%19[^|]|%d",
                  &items[count].bookingID,
                  items[count].passengerID,
                  &items[count].flightID,
                  &items[count].seat,
                  items[count].className,
                  &items[count].ticketPrice,
                  items[count].status,
                  &items[count].ticketID) == 8) {
        count++;
    }

    fclose(fp);
    return count;
}


static int saveAllReservations(Reservation items[], int count)
{
    FILE *fp = fopen(RESERVATION_FILE, "w");
    int i;

    if (fp == NULL) {
        printf("Error: Cannot update %s.\n", RESERVATION_FILE);
        return 0;
    }

    for (i = 0; i < count; i++) {
        if (fprintf(fp, "%d|%s|%d|%d|%s|%.2f|%s|%d\n",
                    items[i].bookingID,
                    items[i].passengerID,
                    items[i].flightID,
                    items[i].seat,
                    items[i].className,
                    items[i].ticketPrice,
                    items[i].status,
                    items[i].ticketID) < 0) {
            fclose(fp);
            return 0;
        }
    }

    if (fclose(fp) != 0) {
        return 0;
    }
    return 1;
}



int findReservation(int bookingID, Reservation *r) {
    Reservation items[MAX_RESERVATIONS];
    int count = readAllReservations(items, MAX_RESERVATIONS);
    int i;

    for (i = 0; i < count; i++) {
        if (items[i].bookingID == bookingID) {
            *r = items[i];
            return 1;
        }
    }
    return 0;
}

static int updateReservationStatus(int bookingID, const char *newStatus)
{
    Reservation items[MAX_RESERVATIONS];
    int count = readAllReservations(items, MAX_RESERVATIONS);
    int i;

    for (i = 0; i < count; i++) {
        if (items[i].bookingID == bookingID) {
            if (strlen(newStatus) >= sizeof(items[i].status)) {
                return 0;
            }
            strcpy(items[i].status, newStatus);
            return saveAllReservations(items, count);
        }
    }

    return 0;
}

int hasSuccessfulPayment(int bookingID) {
    FILE *fp = fopen(PAYMENT_FILE, "r");
    if (fp == NULL) return 0;

    Payment p;
    while (fscanf(fp, "%d|%d|%lf|%19[^|]|%19[^\n]", 
                  &p.paymentID, &p.bookingID, &p.amount, 
                  p.method, p.status) == 5) {
        if (p.bookingID == bookingID && strcmp(p.status, "SUCCESS") == 0) {
            fclose(fp);
            return 1;
        }
    }

    fclose(fp);
    return 0;
}

static int nextPaymentID(void)
{
    FILE *fp = fopen(PAYMENT_FILE, "r");
    Payment p;
    int maxID = 5000;

    if (fp != NULL) {
        while (fscanf(fp, "%d|%d|%lf|%19[^|]|%19[^\n]",
                      &p.paymentID, &p.bookingID, &p.amount,
                      p.method, p.status) == 5) {
            if (p.paymentID > maxID) {
                maxID = p.paymentID;
            }
        }
        fclose(fp);
    }

    return maxID + 1;
}

void processPayment(const char *passengerID) {
    Reservation r;
    Payment p;
    int bookingID, choice;
    FILE *fp;
    int writeOK;

    printf("\n--- Process Payment ---\n");
    printf("Enter Booking ID: ");
    if (scanf("%d", &bookingID) != 1) {
        printf("Invalid input! Please enter a number.\n");
        while (getchar() != '\n') { }
        return;
    }

    if (!findReservation(bookingID, &r)) {
        printf("Booking ID %d not found.\n", bookingID);
        return;
    }

    if (strcmp(r.passengerID, passengerID) != 0) {
        printf("Access Denied! This booking does not belong to you.\n");
        return;
    }

    if (hasSuccessfulPayment(bookingID)) {
        printf("This booking has already been paid successfully.\n");
        return;
    }


    if (strcmp(r.status, "PENDING") != 0) {
        printf("Payment is allowed only for PENDING reservations. Current status: %s\n", r.status);
        return;
    }

    printf("Ticket Price: Rs. %.2f\n", r.ticketPrice);
    printf("1. Cash (simulation)\n");
    printf("2. Card (simulation)\n");
    printf("3. Bank Transfer (simulation)\n");
    printf("Choose payment method (1-3): ");

    if (scanf("%d", &choice) != 1) {
        printf("Invalid choice!\n");
        while (getchar() != '\n') { }
        return;
    }

    switch (choice) {
        case 1: strcpy(p.method, "CASH"); break;
        case 2: strcpy(p.method, "CARD"); break;
        case 3: strcpy(p.method, "BANK"); break;
        default:
            printf("Invalid payment method selected!\n");
            return;
    }

    p.paymentID = nextPaymentID();
    p.bookingID = bookingID;
    p.amount = r.ticketPrice;
    strcpy(p.status, "SUCCESS");


    if (!updateReservationStatus(bookingID, "CONFIRMED")) {
        printf("Payment stopped: could not confirm the reservation in reservations.txt.\n");
        return;
    }

    fp = fopen(PAYMENT_FILE, "a");
    if (fp == NULL) {
        updateReservationStatus(bookingID, "PENDING");
        printf("Payment stopped: could not open payments.txt. Reservation returned to PENDING.\n");
        return;
    }

    writeOK = (fprintf(fp, "%d|%d|%.2f|%s|%s\n",
                       p.paymentID, p.bookingID, p.amount,
                       p.method, p.status) >= 0);
    if (fclose(fp) != 0) {
        writeOK = 0;
    }

    if (!writeOK) {
        updateReservationStatus(bookingID, "PENDING");
        printf("Payment record could not be saved. Reservation returned to PENDING. Please check payments.txt.\n");
        return;
    }

    printf("\n[SUCCESS] Payment recorded successfully!\n");
    printf("Payment ID  : %d\n", p.paymentID);
    printf("Amount Paid : Rs. %.2f\n", p.amount);
    printf("Booking Status: CONFIRMED\n");
}

void viewPaymentDetails(const char *passengerID) {
    FILE *fp = fopen(PAYMENT_FILE, "r");
    if (fp == NULL) {
        printf("No payment records found.\n");
        return;
    }

    Payment p;
    int found = 0;
    printf("\n--- Payment Details Records ---\n");
    while (fscanf(fp, "%d|%d|%lf|%19[^|]|%19[^\n]", 
                  &p.paymentID, &p.bookingID, &p.amount, 
                  p.method, p.status) == 5) {
        printf("Payment ID: %d | Booking ID: %d | Amount: Rs. %.2f | Method: %s | Status: %s\n",
               p.paymentID, p.bookingID, p.amount, p.method, p.status);
        found = 1;
    }

    if (!found) {
        printf("No records available in payment file.\n");
    }
    fclose(fp);
}

static int nextCancellationID(void)
{
    FILE *fp = fopen(CANCELLATION_FILE, "r");
    Cancellation c;
    int maxID = 7000;

    if (fp != NULL) {
        while (fscanf(fp, "%d|%d|%lf|%19[^\n]",
                      &c.cancellationID, &c.bookingID,
                      &c.refundAmount, c.status) == 4) {
            if (c.cancellationID > maxID) {
                maxID = c.cancellationID;
            }
        }
        fclose(fp);
    }

    return maxID + 1;
}

static int cancellationAlreadyPending(int bookingID)
{
    FILE *fp = fopen(CANCELLATION_FILE, "r");
    Cancellation c;

    if (fp == NULL) {
        return 0;
    }

    while (fscanf(fp, "%d|%d|%lf|%19[^\n]",
                  &c.cancellationID, &c.bookingID,
                  &c.refundAmount, c.status) == 4) {
        if (c.bookingID == bookingID && strcmp(c.status, "PENDING") == 0) {
            fclose(fp);
            return 1;
        }
    }

    fclose(fp);
    return 0;
}


void requestCancellation(const char *passengerID) {
    Reservation r;
    int bookingID;

    printf("\n--- Request Ticket Cancellation ---\n");
    printf("Enter Booking ID to cancel: ");
    if (scanf("%d", &bookingID) != 1) {
        printf("Invalid input!\n");
        while (getchar() != '\n');
        return;
    }

    if (!findReservation(bookingID, &r)) {
        printf("Booking ID not found!\n");
        return;
    }

    if (strcmp(r.passengerID, passengerID) != 0) {
        printf("Access Denied! This booking does not belong to you.\n");
        return;
    }

    if (strcmp(r.status, "CONFIRMED") != 0) {
        printf("Booking is not confirmed or already processed (Status: %s).\n", r.status);
        return;
    }

    if (!hasSuccessfulPayment(bookingID)) {
        printf("Cannot request cancellation: No successful payment found for this booking.\n");
        return;
    }

    FILE *fpCheck = fopen(CANCELLATION_FILE, "r");
    if (fpCheck != NULL) {
        Cancellation tempC;
        while (fscanf(fpCheck, "%d|%d|%lf|%19[^\n]", 
                      &tempC.cancellationID, &tempC.bookingID, 
                      &tempC.refundAmount, tempC.status) == 4) {
            if (tempC.bookingID == bookingID && strcmp(tempC.status, "PENDING") == 0) {
                printf("A cancellation request for this booking is already PENDING admin approval.\n");
                fclose(fpCheck);
                return;
            }
        }
        fclose(fpCheck);
    }

    double refundAmount = (r.ticketPrice * REFUND_PERCENT) / 100.0;
    printf("Ticket Price   : Rs. %.2f\n", r.ticketPrice);
    printf("Refund Policy  : %.1f%%\n", REFUND_PERCENT);
    printf("Expected Refund: Rs. %.2f\n", refundAmount);

    char confirm;
    printf("Submit cancellation request to admin? (Y/N): ");
    scanf(" %c", &confirm);

    if (confirm != 'Y' && confirm != 'y') {
        printf("Cancellation request aborted.\n");
        return;
    }

    int cancellationID = 7001;
    FILE *fpC = fopen(CANCELLATION_FILE, "r");
    if (fpC != NULL) {
        Cancellation tempC;
        while (fscanf(fpC, "%d|%d|%lf|%19[^\n]", 
                      &tempC.cancellationID, &tempC.bookingID, 
                      &tempC.refundAmount, tempC.status) == 4) {
            cancellationID = tempC.cancellationID + 1;
        }
        fclose(fpC);
    }

    fpC = fopen(CANCELLATION_FILE, "a");
    if (fpC == NULL) {
        printf("Error opening cancellation file!\n");
        return;
    }

    fprintf(fpC, "%d|%d|%.2f|PENDING\n", cancellationID, bookingID, refundAmount);
    fclose(fpC);

    printf("\n[SUCCESS] Cancellation request submitted successfully!\n");
    printf("Cancellation ID : %d\n", cancellationID);
    printf("Status          : PENDING (Waiting for Admin approval)\n");
}

void viewMyCancellationStatus(const char *passengerID) {
    int bookingID;
    printf("\n--- Check Cancellation Status ---\n");
    printf("Enter Booking ID: ");
    if (scanf("%d", &bookingID) != 1) {
        printf("Invalid input!\n");
        while (getchar() != '\n');
        return;
    }

    FILE *fp = fopen(CANCELLATION_FILE, "r");
    if (fp == NULL) {
        printf("No cancellation records found.\n");
        return;
    }

    Cancellation c;
    int found = 0;
    while (fscanf(fp, "%d|%d|%lf|%19[^\n]", 
                  &c.cancellationID, &c.bookingID, 
                  &c.refundAmount, c.status) == 4) {
        if (c.bookingID == bookingID) {
            printf("\nCancellation ID : %d\n", c.cancellationID);
            printf("Booking ID      : %d\n", c.bookingID);
            printf("Refund Amount   : Rs. %.2f\n", c.refundAmount);
            printf("Status          : %s\n", c.status);
            found = 1;
        }
    }

    if (!found) {
        printf("No cancellation request found for Booking ID %d.\n", bookingID);
    }
    fclose(fp);
}

int saveCancellations(Cancellation requests[], int count)
{
    FILE *fp = fopen(CANCELLATION_FILE, "w");
    int i;

    if (fp == NULL) {
        printf("Error updating cancellations.txt.\n");
        return 0;
    }

    for (i = 0; i < count; i++) {
        if (fprintf(fp, "%d|%d|%.2f|%s\n",
                    requests[i].cancellationID,
                    requests[i].bookingID,
                    requests[i].refundAmount,
                    requests[i].status) < 0) {
            fclose(fp);
            return 0;
        }
    }

    if (fclose(fp) != 0) {
        return 0;
    }
    return 1;
}


void manageCancellationRequests(void) {
    FILE *fp = fopen(CANCELLATION_FILE, "r");
    if (fp == NULL) {
        printf("No cancellation requests found.\n");
        return;
    }

    Cancellation requests[100];
    int count = 0;

    while (fscanf(fp, "%d|%d|%lf|%19[^\n]", 
                  &requests[count].cancellationID, &requests[count].bookingID, 
                  &requests[count].refundAmount, requests[count].status) == 4) {
        count++;
    }
    fclose(fp);

    printf("\n--- Pending Cancellation Requests ---\n");
    int pendingFound = 0;
    for (int i = 0; i < count; i++) {
        if (strcmp(requests[i].status, "PENDING") == 0) {
            printf("Index: %d | Cancellation ID: %d | Booking ID: %d | Refund Amount: Rs. %.2f\n",
                   i, requests[i].cancellationID, requests[i].bookingID, requests[i].refundAmount);
            pendingFound = 1;
        }
    }

    if (!pendingFound) {
        printf("No pending cancellation requests to review.\n");
        return;
    }

    int targetID, actionChoice;
    printf("\nEnter Cancellation ID to process: ");
    if (scanf("%d", &targetID) != 1) {
        printf("Invalid input!\n");
        while (getchar() != '\n');
        return;
    }

    int foundIdx = -1;
    for (int i = 0; i < count; i++) {
        if (requests[i].cancellationID == targetID && strcmp(requests[i].status, "PENDING") == 0) {
            foundIdx = i;
            break;
        }
    }

    if (foundIdx == -1) {
        printf("Invalid Cancellation ID or request is not pending.\n");
        return;
    }

    printf("1. Approve (Accept)\n");
    printf("2. Decline (Reject)\n");
    printf("Choose action (1-2): ");
    if (scanf("%d", &actionChoice) != 1) {
        printf("Invalid choice!\n");
        while (getchar() != '\n');
        return;
    }

    
    if (actionChoice == 1){
        if (updateReservationStatus(
            requests[foundIdx].bookingID,
            "CANCELLED"))
        {
            strcpy(requests[foundIdx].status, "APPROVED");

            printf("\n[SUCCESS] Cancellation APPROVED!\n");
            printf("Reservation status changed to CANCELLED.\n");
        }
        else{
            printf("\nError: Unable to update reservation.\n");
            return;
        }
    }
    else if (actionChoice == 2){
        strcpy(requests[foundIdx].status, "DECLINED");

        printf("\n[SUCCESS] Cancellation request DECLINED.\n");
    }
    else{
        printf("Invalid action choice.\n");
        return;
    }


    FILE *fpWrite = fopen(CANCELLATION_FILE, "w");
    if (fpWrite == NULL) {
        printf("Error updating cancellation file.\n");
        return;
    }
    for (int i = 0; i < count; i++) {
        fprintf(fpWrite, "%d|%d|%.2f|%s\n", 
                requests[i].cancellationID, requests[i].bookingID, 
                requests[i].refundAmount, requests[i].status);
    }
    fclose(fpWrite);
}

void viewAllPayments(void) {
    FILE *fp = fopen(PAYMENT_FILE, "r");

    if (fp == NULL) {
        printf("No payment records found.\n");
        return;
    }

    Payment p;
    int found = 0;

    printf("\n--- All Payment Records ---\n");

    while (fscanf(fp, "%d|%d|%lf|%19[^|]|%19[^\n]",
                  &p.paymentID,
                  &p.bookingID,
                  &p.amount,
                  p.method,
                  p.status) == 5)
    {
        printf("\nPayment ID : %d\n", p.paymentID);
        printf("Booking ID : %d\n", p.bookingID);
        printf("Amount     : Rs. %.2f\n", p.amount);
        printf("Method     : %s\n", p.method);
        printf("Status     : %s\n", p.status);

        found = 1;
    }

    if (!found) {
        printf("No payment records available.\n");
    }

    fclose(fp);
}

void viewAllCancellations(void) {
    FILE *fp = fopen(CANCELLATION_FILE, "r");
    if (fp == NULL) {
        printf("No cancellation records found.\n");
        return;
    }

    Cancellation c;
    int found = 0;
    printf("\n--- All Cancellation Records ---\n");
    while (fscanf(fp, "%d|%d|%lf|%19[^\n]", 
                  &c.cancellationID, &c.bookingID, 
                  &c.refundAmount, c.status) == 4) {
        printf("Cancellation ID: %d | Booking ID: %d | Refund: Rs. %.2f | Status: %s\n",
               c.cancellationID, c.bookingID, c.refundAmount, c.status);
        found = 1;
    }

    if (!found) printf("No cancellation records available.\n");
    fclose(fp);
}

int getpendingcancellationcount(void){
    FILE*fp=fopen(CANCELLATION_FILE,"r");
    if(fp == NULL)
    return 0;
    Cancellation c;
    int count = 0;
    while (fscanf(fp, "%d|%d|%lf|%19[^\n]",&c.cancellationID,&c.bookingID,&c.refundAmount,c.status)==4){
        if(strcmp(c.status,"PENDING")==0){
            count++;

        }
    }
    fclose(fp);
    return count;
}

void paymentcustomerMenu(const char *passengerID) {

    int choice;

    do {
        printf("\n=========================================\n");
        printf("             CUSTOMER PORTAL             \n");
        printf("=========================================\n");
        printf("1. Process Payment\n");
        printf("2. View Payment Details\n");
        printf("3. Request Ticket Cancellation\n");
        printf("4. Check Cancellation Request Status\n");
        printf("0. Back to Main Menu\n");
        printf("-----------------------------------------\n");
        printf("Enter your choice: ");

        if (scanf("%d", &choice) != 1) {
            printf("Invalid input!\n");
            while (getchar() != '\n');
            continue;
        }

        switch (choice) {
            case 1: processPayment(passengerID); break;
            case 2: viewPaymentDetails(passengerID); break;
            case 3: requestCancellation(passengerID); break;
            case 4: viewMyCancellationStatus(passengerID); break;
            case 0: printf("Returning to Passenger Menu...\n"); break;
            default: printf("Invalid choice! Choose between 0 and 4.\n");
        }
    } while (choice != 0);
}

void paymentadminMenu(void) {
    int choice;
    
    do {
        int pencount = getpendingcancellationcount();
        printf("\n=========================================\n");
        printf("              ADMIN PORTAL               \n");
        printf("=========================================\n");
        printf("1. Manage Cancellation Requests (Approve/Decline)(PENDING REQUEStS... %d)\n",pencount);
        printf("2. View All Payment Records\n");
        printf("3. View All Cancellation Records\n");
        printf("0. Back to Main Menu\n");
        printf("-----------------------------------------\n");
        printf("Enter your choice: ");

        if (scanf("%d", &choice) != 1) {
            printf("Invalid input!\n");
            while (getchar() != '\n');
            continue;
        }

        switch (choice) {
            case 1: manageCancellationRequests(); break;
            case 2: viewAllPayments(); break;
            case 3: viewAllCancellations(); break;
            case 0: printf("Returning to Main Menu...\n"); break;
            default: printf("Invalid choice! Choose between 0 and 3.\n");
        }

    } while (choice != 0);

}
