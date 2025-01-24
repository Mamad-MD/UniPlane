#pragma warning(suppress : 4996)
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <Windows.h>
#include "UniPlaneHeader.h"

int main() {
    Student* studentHead = NULL;
    Course* courseHead = NULL;

    LoadFromFile(&studentHead);
    LoadCoursesFromFile(&courseHead);

    int choice;
    do {
        choice = welcome();

        switch (choice) {
        case 1:
            while (!login(studentHead)) {
                printf("Returning to main menu...\n");
                break;
            }
            break;
        case 2:
            signup(&studentHead);
            break;
        case 3:
            if (login(studentHead)) {
                Golestan(studentHead, &courseHead);
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

    FreeStudentList(studentHead);
    FreeCourseList(courseHead);
    return 0;
}