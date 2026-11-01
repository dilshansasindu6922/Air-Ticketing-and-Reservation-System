#include <stdio.h>
#include <string.h>

#define RESERVATION_FILE "reservations.txt"
#define FLIGHT_FILE "flights.txt"
#define PAYMENT_FILE "payments.txt"
#define CANCELLATION_FILE "cancellations.txt"

static void bookingReport(void)
{
    FILE *fp = fopen(RESERVATION_FILE, "r");
    int bookingID, flightID, seat, ticketID;
    int total = 0, cancelled = 0, active = 0;
    char passengerID[20], className[15], status[20];
    double fare;

    if (fp == NULL) {
        printf("\nNo reservation records found.\n");
        return;
    }

    printf("\n========== BOOKING REPORT ==========\n");
    printf("%-10s %-15s %-9s %-6s %-10s %-12s %-12s %-8s\n",
           "Booking", "Passenger", "Flight", "Seat",
           "Class", "Fare", "Status", "Ticket");

    while (fscanf(fp, "%d|%19[^|]|%d|%d|%14[^|]|%lf|%19[^|]|%d",
                  &bookingID, passengerID, &flightID, &seat,
                  className, &fare, status, &ticketID) == 8) {

        printf("%-10d %-15s %-9d %-6d %-10s %-12.2f %-12s %-8d\n",
               bookingID, passengerID, flightID, seat,
               className, fare, status, ticketID);

        total++;
        if (strcmp(status, "CANCELLED") == 0)
            cancelled++;
        else
            active++;
    }

    printf("------------------------------------\n");
    printf("Total reservations : %d\n", total);
    printf("Active bookings    : %d\n", active);
    printf("Cancelled bookings : %d\n", cancelled);

    fclose(fp);
}

static void flightStatistics(void)
{
    FILE *flightFile = fopen(FLIGHT_FILE, "r");

    int flightID, totalSeats, totalFlights = 0;
    int airlineBookings, availableSeats;
    int bookingID, seat, ticketID;
    char airline[50], origin[50], destination[50];
    char date[20], time[10];
    char passengerID[20], className[15], bookingStatus[20];
    double flightPrice, fare;

    FILE *reservationFile;

    if (flightFile == NULL) {
        printf("\nNo flight records found.\n");
        return;
    }

    printf("\n========== FLIGHT STATISTICS ==========\n");

    while (fscanf(flightFile,
                  "%d|%49[^|]|%49[^|]|%49[^|]|%19[^|]|%9[^|]|%d|%lf",
                  &flightID, airline, origin, destination,
                  date, time, &totalSeats, &flightPrice) == 8) {

        airlineBookings = 0;

        reservationFile = fopen(RESERVATION_FILE, "r");
        if (reservationFile != NULL) {
            int reservationFlightID;
            while (fscanf(reservationFile,
                          "%d|%19[^|]|%d|%d|%14[^|]|%lf|%19[^|]|%d",
                          &bookingID, passengerID, &reservationFlightID, &seat,
                          className, &fare, bookingStatus, &ticketID) == 8) {
                if (reservationFlightID == flightID &&
                    strcmp(bookingStatus, "CANCELLED") != 0) {
                    airlineBookings++;
                }
            }
            fclose(reservationFile);
        }

        availableSeats = totalSeats - airlineBookings;
        if (availableSeats < 0)
            availableSeats = 0;

        printf("\nFlight ID       : %d\n", flightID);
        printf("Airline         : %s\n", airline);
        printf("Route           : %s -> %s\n", origin, destination);
        printf("Date / Time     : %s / %s\n", date, time);
        printf("Ticket Price    : Rs. %.2f\n", flightPrice);
        printf("Total Seats     : %d\n", totalSeats);
        printf("Booked Seats    : %d\n", airlineBookings);
        printf("Available Seats : %d\n", availableSeats);

        totalFlights++;
    }

    printf("\nTotal flights: %d\n", totalFlights);
    fclose(flightFile);
}

static void revenueReport(void)
{
    FILE *fp;
    int paymentID, bookingID;
    int successfulPayments = 0;
    double amount, grossRevenue = 0.0;

    int cancellationID;
    int approvedCancellations = 0;
    double refundAmount, approvedRefunds = 0.0;
    char method[20], paymentStatus[20], cancellationStatus[20];
    double netRevenue;

    fp = fopen(PAYMENT_FILE, "r");
    if (fp != NULL) {
        while (fscanf(fp, "%d|%d|%lf|%19[^|]|%19[^\n]",
                      &paymentID, &bookingID, &amount,
                      method, paymentStatus) == 5) {
            if (strcmp(paymentStatus, "SUCCESS") == 0) {
                grossRevenue += amount;
                successfulPayments++;
            }
        }
        fclose(fp);
    } else {
        printf("\nPayment file not found. Gross revenue is treated as Rs. 0.00.\n");
    }

    fp = fopen(CANCELLATION_FILE, "r");
    if (fp != NULL) {
        while (fscanf(fp, "%d|%d|%lf|%19[^\n]",
                      &cancellationID, &bookingID,
                      &refundAmount, cancellationStatus) == 4) {
            if (strcmp(cancellationStatus, "APPROVED") == 0) {
                approvedRefunds += refundAmount;
                approvedCancellations++;
            }
        }
        fclose(fp);
    }

    netRevenue = grossRevenue - approvedRefunds;

    printf("\n========== REVENUE REPORT ==========\n");
    printf("Successful payments       : %d\n", successfulPayments);
    printf("Gross revenue              : Rs. %.2f\n", grossRevenue);
    printf("Approved cancellation count: %d\n", approvedCancellations);
    printf("Approved refunds           : Rs. %.2f\n", approvedRefunds);
    printf("------------------------------------\n");
    printf("NET REVENUE                : Rs. %.2f\n", netRevenue);
    printf("\nOnly SUCCESS payments are included in gross revenue.\n");
    printf("Only APPROVED refunds are deducted from revenue.\n");
}

static void cancellationReport(void)
{
    FILE *fp = fopen(CANCELLATION_FILE, "r");
    int cancellationID, bookingID;
    int total = 0, pending = 0, approved = 0, declined = 0;
    double refundAmount, approvedRefundTotal = 0.0;
    char status[20];

    if (fp == NULL) {
        printf("\nNo cancellation records found.\n");
        return;
    }

    printf("\n========== CANCELLATION REPORT ==========\n");
    printf("%-15s %-12s %-16s %-12s\n",
           "Cancellation", "Booking", "Refund (Rs.)", "Status");

    while (fscanf(fp, "%d|%d|%lf|%19[^\n]",
                  &cancellationID, &bookingID,
                  &refundAmount, status) == 4) {

        printf("%-15d %-12d %-16.2f %-12s\n",
               cancellationID, bookingID, refundAmount, status);

        total++;

        if (strcmp(status, "PENDING") == 0)
            pending++;
        else if (strcmp(status, "APPROVED") == 0) {
            approved++;
            approvedRefundTotal += refundAmount;
        }
        else if (strcmp(status, "DECLINED") == 0)
            declined++;
    }

    printf("-----------------------------------------\n");
    printf("Total cancellation requests : %d\n", total);
    printf("Pending requests            : %d\n", pending);
    printf("Approved requests           : %d\n", approved);
    printf("Declined requests           : %d\n", declined);
    printf("Approved refund total       : Rs. %.2f\n",
           approvedRefundTotal);

    fclose(fp);
}

void reportMenu(void)
{
    int choice;

    do {
        printf("\n========== REPORTS ==========\n");
        printf("1. Booking Report\n");
        printf("2. Flight Statistics\n");
        printf("3. Revenue Report (After Refunds)\n");
        printf("4. Cancellation Report\n");
        printf("0. Back\n");
        printf("Enter choice: ");

        if (scanf("%d", &choice) != 1) {
            int ch;
            while ((ch = getchar()) != '\n' && ch != EOF) { }
            printf("Invalid input. Please enter a number.\n");
            continue;
        }

        switch (choice) {
            case 1:
                bookingReport();
                break;
            case 2:
                flightStatistics();
                break;
            case 3:
                revenueReport();
                break;
            case 4:
                cancellationReport();
                break;
            case 0:
                printf("Returning to previous menu...\n");
                break;
            default:
                printf("Invalid choice. Please select 0-4.\n");
        }

    } while (choice != 0);
}
