#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define RESERVATION_FILE "reservations.txt"
#define PAYMENT_FILE "payments.txt"
#define CANCELLATION_FILE "cancellations.txt"
#define REFUND_PERCENT 90.0

typedef struct {
    int bookingID;
    int passengerID;
    int flightID;
    double ticketPrice;
    char status[20];
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


void paymentcustomerMenu(void);
void paymentprocessPayment(void);
void viewPaymentDetails(void);
void requestCancellation(void);
void viewMyCancellationStatus(void);


void adminMenu(void);
int getpendingcancellationcount(void);
void manageCancellationRequests(void);
void viewAllPayments(void);
void viewAllCancellations(void);



int findReservation(int bookingID, Reservation *r) {
    FILE *fp = fopen(RESERVATION_FILE, "r");
    if (fp == NULL) {
        return 0;
    }

    while (fscanf(fp, "%d|%d|%d|%lf|%19[^\n]", 
                  &r->bookingID, &r->passengerID, 
                  &r->flightID, &r->ticketPrice, 
                  r->status) == 5) {
        if (r->bookingID == bookingID) {
            fclose(fp);
            return 1; 
        }
    }

    fclose(fp);
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

// --- Customer Feature Implementations ---

void processPayment(void) {
    Reservation r;
    Payment p;
    int bookingID, choice;

    printf("\n--- Process Payment ---\n");
    printf("Enter Booking ID: ");
    if (scanf("%d", &bookingID) != 1) {
        printf("Invalid input!\n");
        while (getchar() != '\n');
        return;
    }

    if (!findReservation(bookingID, &r)) {
        printf("Booking ID %d not found!\n", bookingID);
        return;
    }

    if (strcmp(r.status, "CONFIRMED") != 0) {
        printf("This booking is not available for payment (Status: %s).\n", r.status);
        return;
    }

    if (hasSuccessfulPayment(bookingID)) {
        printf("This booking has already been paid successfully.\n");
        return;
    }

    printf("Ticket Price: Rs. %.2f\n", r.ticketPrice);
    printf("1. Cash\n");
    printf("2. Card (Simulation)\n");
    printf("3. Bank Transfer (Simulation)\n");
    printf("Choose payment method (1-3): ");
    
    if (scanf("%d", &choice) != 1) {
        printf("Invalid choice!\n");
        while (getchar() != '\n');
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

    p.paymentID = 5001;
    FILE *fpCheck = fopen(PAYMENT_FILE, "r");
    if (fpCheck != NULL) {
        Payment tempP;
        while (fscanf(fpCheck, "%d|%d|%lf|%19[^|]|%19[^\n]", 
                      &tempP.paymentID, &tempP.bookingID, &tempP.amount, 
                      tempP.method, tempP.status) == 5) {
            p.paymentID = tempP.paymentID + 1;
        }
        fclose(fpCheck);
    }

    p.bookingID = bookingID;
    p.amount = r.ticketPrice;
    strcpy(p.status, "SUCCESS");

    FILE *fp = fopen(PAYMENT_FILE, "a");
    if (fp == NULL) {
        printf("Error opening payment file!\n");
        return;
    }

    fprintf(fp, "%d|%d|%.2f|%s|%s\n", 
            p.paymentID, p.bookingID, p.amount, p.method, p.status);
    fclose(fp);

    printf("\n[SUCCESS] Payment recorded successfully!\n");
    printf("Payment ID  : %d\n", p.paymentID);
    printf("Amount Paid : Rs. %.2f\n", p.amount);
}

void viewPaymentDetails(void) {
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

    if (!found) printf("No records available in payment file.\n");
    fclose(fp);
}

void requestCancellation(void) {
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

    if (strcmp(r.status, "CONFIRMED") != 0) {
        printf("Booking is not confirmed or already processed (Status: %s).\n", r.status);
        return;
    }

    if (!hasSuccessfulPayment(bookingID)) {
        printf("Cannot request cancellation: No successful payment found for this booking.\n");
        return;
    }

    // Check if a pending cancellation request already exists
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

void viewMyCancellationStatus(void) {
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

    if (actionChoice == 1) {
        strcpy(requests[foundIdx].status, "APPROVED");

        // Update reservation status to CANCELLED
        FILE *resFp = fopen(RESERVATION_FILE, "r");
        FILE *tempRes = fopen("reservations_temp.txt", "w");
        if (resFp != NULL && tempRes != NULL) {
            Reservation current;
            while (fscanf(resFp, "%d|%d|%d|%lf|%19[^\n]", 
                          &current.bookingID, &current.passengerID, 
                          &current.flightID, &current.ticketPrice, 
                          current.status) == 5) {
                if (current.bookingID == requests[foundIdx].bookingID) {
                    strcpy(current.status, "CANCELLED");
                }
                fprintf(tempRes, "%d|%d|%d|%.2f|%s\n", 
                        current.bookingID, current.passengerID, 
                        current.flightID, current.ticketPrice, 
                        current.status);
            }
            fclose(resFp);
            fclose(tempRes);
            remove(RESERVATION_FILE);
            rename("reservations_temp.txt", RESERVATION_FILE);
        }

        printf("\n[SUCCESS] Cancellation APPROVED and reservation cancelled successfully!\n");
    } else if (actionChoice == 2) {
        strcpy(requests[foundIdx].status, "DECLINED");
        printf("\n[SUCCESS] Cancellation request DECLINED.\n");
    } else {
        printf("Invalid action choice.\n");
        return;
    }

    // Save updated cancellations back to file
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
    viewPaymentDetails();
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

void paymentcustomerMenu(void) {

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
            case 1: processPayment(); break;
            case 2: viewPaymentDetails(); break;
            case 3: requestCancellation(); break;
            case 4: viewMyCancellationStatus(); break;
            case 0: printf("Returning to Main Menu...\n"); break;
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
