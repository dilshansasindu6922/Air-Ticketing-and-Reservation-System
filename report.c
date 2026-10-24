#include <stdio.h>

void reportMenu(void)
{
    int choice;

    do {
        printf("\n========== REPORTS ==========\n");
        printf("1. Booking Report\n");
        printf("2. Flight Statistics\n");
        printf("3. Revenue Report\n");
        printf("0. Back\n");
        printf("Enter choice: ");

        if(scanf("%d", &choice)!=1)
        {   int ch;
        
            while((ch=getchar())!='\n'&&ch!=EOF){};
            continue;
        }

        if(choice==1){

            FILE *file=fopen("reservations.txt","r"); 
            int bid,flight,seat,ticket,count=0; 
            char pid[20],className[15],status[20]; 
            double fare;

            if(!file)
            {   printf("No reservations found.\n");
                continue;
            }

            printf("\n--- BOOKING REPORT ---\n");

            while(fscanf(file,"%d|%19[^|]|%d|%d|%14[^|]|%lf|%19[^|]|%d\n",&bid,pid,&flight,&seat,className,&fare,status,&ticket)==8)
            {printf("Booking:%d Passenger:%s Flight:%d Status:%s Fare:%.2f\n",bid,pid,flight,status,fare);
                count++;
            }
            printf("Total reservations: %d\n",count); 
            
            fclose(file);

        } else if(choice==2){

            FILE *fp=fopen("flights.txt","r"); 

            int id,seats,count=0; 
            char airline[50],origin[50],dest[50],date[20],time[10];
            double price;

            if(!fp)
            {
                printf("No flights found.\n");
                continue;
            }

            while(fscanf(fp,"%d|%49[^|]|%49[^|]|%49[^|]|%19[^|]|%9[^|]|%d|%lf\n",&id,airline,origin,dest,date,time,&seats,&price)==8)
            {printf("Flight:%d %s -> %s | Seats:%d | Price:%.2f\n",id,origin,dest,seats,price);
                count++;
            }
            printf("Total flights: %d\n",count); 

            fclose(fp);

        } else if(choice==3){

            FILE *fp=fopen("payments.txt","r"); 
            int payID,bid,count=0; 
            double amount,total=0; 
            char method[20],status[20];

            if(!fp)
            {printf("No payment records found.\n");
                continue;
            }

            while(fscanf(fp,"%d|%d|%lf|%19[^|]|%19[^\n]\n",&payID,&bid,&amount,method,status)==5)
            {  if(status[0]=='S')
                {   total+=amount;
                    count++;
                }
            }

            printf("Successful payments: %d\nTotal recorded revenue: Rs. %.2f\n",count,total); 
            fclose(fp);

        } else if(choice!=0) printf("Invalid choice.\n");

    } while(choice!=0);
}
