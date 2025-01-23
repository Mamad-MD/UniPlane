#pragma warning(suppress : 4996)
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <Windows.h>
#include "UniPlaneHeader.h"

int main() {
    Student* head = NULL;
    LoadFromFile(&head); 

    int choice;
    do {
        choice = welcome();
        switch (choice) {
        case 1:
            while (!login(head)) {
                printf("Returning to main menu...\n");
                break;
            }
            break;
        case 2:
            signup(&head);
            break;
        case 3:
            if (login(head)) {
                Golestan();
            }
            break;
        case 0:
            printf("Exiting...\n");
            break;
        default:
            printf("Invalid choice. Please try again.\n");
            break;
        }
    } while (choice != 0);

    return 0;
}