#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>

void ParentProcess(int *);
void ChildProcess(int *);

int main(int argc, char *argv[])
{
    int ShmID;
    int *ShmPTR;
    pid_t pid;
    int status;

    ShmID = shmget(IPC_PRIVATE, 2 * sizeof(int), IPC_CREAT | 0666);

    if (ShmID < 0)
    {
        printf("*** shmget error ***\n");
        exit(1);
    }

    ShmPTR = (int *)shmat(ShmID, NULL, 0);

    if (ShmPTR == (int *)-1)
    {
        printf("*** shmat error ***\n");
        exit(1);
    }

    // BankAccount = 0, Turn = 0
    ShmPTR[0] = 0;
    ShmPTR[1] = 0;

    printf("Shared memory initialized: BankAccount = %d, Turn = %d\n",
           ShmPTR[0], ShmPTR[1]);

    pid = fork();

    if (pid < 0)
    {
        printf("*** fork error ***\n");
        exit(1);
    }

    // Child process
    else if (pid == 0)
    {
        ChildProcess(ShmPTR);
        shmdt((void *)ShmPTR);
        exit(0);
    }

    // Parent process
    else
    {
        ParentProcess(ShmPTR);

        wait(&status);

        shmdt((void *)ShmPTR);
        shmctl(ShmID, IPC_RMID, NULL);

        exit(0);
    }
}

void ParentProcess(int *SharedMem)
{
    int account;
    int balance;
    int i;

    srandom(time(NULL) ^ getpid());

    for (i = 0; i < 25; i++)
    {
        sleep(random() % 6);

        account = SharedMem[0];

        while (SharedMem[1] != 0)
        {
            // Wait for child's turn
        }

        if (account <= 100)
        {
            balance = random() % 100;

            if (balance % 2 == 0)
            {
                account += balance;

                printf("Dear old Dad: Deposits $%d / Balance = $%d\n",
                       balance, account);
            }
            else
            {
                printf("Dear old Dad: Doesn't have any money to give\n");
            }
        }
        else
        {
            printf("Dear old Dad: Thinks Student has enough Cash ($%d)\n",
                   account);
        }

        SharedMem[0] = account;
        SharedMem[1] = 1;
    }
}

void ChildProcess(int *SharedMem)
{
    int account;
    int balance;
    int i;

    srandom(time(NULL) ^ getpid());

    for (i = 0; i < 25; i++)
    {
        sleep(random() % 6);

        account = SharedMem[0];

        while (SharedMem[1] != 1)
        {
            // Wait for Dad's turn
        }

        balance = random() % 50;

        printf("Poor Student needs $%d\n", balance);

        if (balance <= account)
        {
            account -= balance;

            printf("Poor Student: Withdraws $%d / Balance = $%d\n",
                   balance, account);
        }
        else
        {
            printf("Poor Student: Not Enough Cash ($%d)\n", account);
        }

        SharedMem[0] = account;
        SharedMem[1] = 0;
    }
}