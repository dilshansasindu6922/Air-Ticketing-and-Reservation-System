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
void processPayment(void);
void viewPaymentDetails(void);
void cancelTicket(void);
void viewCancellationDetails(void);


int findReservation(int bookingID, Reservation *r) {
    FILE *fp = fopen(RESERVATION_FILE, "r");
    if (fp == NULL) {
        printf("Error: Reservation file not found!\n");
        return 0;
    }

    
    while (fscanf(fp, "%d|%d|%d|%lf|%19[^\n]", 
                  &r->bookingID, &r->passengerID, 
                  &r->flightID, &r->ticketPrice, 
                  r->status) == 5) {
        if (r->bookingID == bookingID) {
            fclose(fp);
            return 1; // Found
        }
    }

    fclose(fp);
    return 0; // Not found
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


void cancelTicket(void) {
    Reservation r;
    int bookingID;

    printf("\n--- Cancel Ticket ---\n");
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
        printf("Booking is not confirmed or already cancelled.\n");
        return;
    }

    if (!hasSuccessfulPayment(bookingID)) {
        printf("Cannot cancel: No successful payment found for this booking.\n");
        return;
    }

    double refundAmount = (r.ticketPrice * REFUND_PERCENT) / 100.0;
    printf("Ticket Price   : Rs. %.2f\n", r.ticketPrice);
    printf("Refund Policy  : %.1f%%\n", REFUND_PERCENT);
    printf("Refund Amount  : Rs. %.2f\n", refundAmount);

    char confirm;
    printf("Confirm cancellation? (Y/N): ");
    scanf(" %c", &confirm);

    if (confirm != 'Y' && confirm != 'y') {
        printf("Cancellation aborted by user.\n");
        return;
    }

    
    FILE *fp = fopen(RESERVATION_FILE, "r");
    FILE *temp = fopen("reservations_temp.txt", "w");
    if (fp == NULL || temp == NULL) {
        printf("Error handling reservation data files.\n");
        if (fp) fclose(fp);
        if (temp) fclose(temp);
        return;
    }

    Reservation current;
    while (fscanf(fp, "%d|%d|%d|%lf|%19[^\n]", 
                  &current.bookingID, &current.passengerID, 
                  &current.flightID, &current.ticketPrice, 
                  current.status) == 5) {
        if (current.bookingID == bookingID) {
            strcpy(current.status, "CANCELLED");
        }
        fprintf(temp, "%d|%d|%d|%.2f|%s\n", 
                current.bookingID, current.passengerID, 
                current.flightID, current.ticketPrice, 
                current.status);
    }

    fclose(fp);
    fclose(temp);

    remove(RESERVATION_FILE);
    rename("reservations_temp.txt", RESERVATION_FILE);

    
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

    fprintf(fpC, "%d|%d|%.2f|SUCCESS\n", cancellationID, bookingID, refundAmount);
    fclose(fpC);

    printf("\n[SUCCESS] Ticket cancelled successfully.\n");
    printf("Cancellation ID : %d\n", cancellationID);
    printf("Refund Issued   : Rs. %.2f\n", refundAmount);
}


void viewCancellationDetails(void) {
    FILE *fp = fopen(CANCELLATION_FILE, "r");
    if (fp == NULL) {
        printf("No cancellation records found.\n");
        return;
    }

    Cancellation c;
    int found = 0;
    printf("\n--- Cancellation Details Records ---\n");
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


int main(void) {
    int choice;

    do {
        printf("\n=========================================\n");
        printf("          PAYMENT AND CANCELLATION         \n");
        printf("=========================================\n");
        printf("1. Process Payment\n");
        printf("2. View Payment Details\n");
        printf("3. Cancel Ticket & Refund\n");
        printf("4. View Cancellation Details\n");
        printf("0. Exit Program\n");
        printf("-----------------------------------------\n");
        printf("Enter your choice: ");

        if (scanf("%d", &choice) != 1) {
            printf("Invalid input! Please enter a valid number.\n");
            while (getchar() != '\n');
            continue;
        }

        switch (choice) {
            case 1:
                processPayment();
                break;
            case 2:
                viewPaymentDetails();
                break;
            case 3:
                cancelTicket();
                break;
            case 4:
                viewCancellationDetails();
                break;
            case 0:
                printf("Exiting program. Goodbye!\n");
                break;
            default:
                printf("Invalid choice! Please select between 0 and 4.\n");
        }

    } while (choice != 0);

    return 0;
}