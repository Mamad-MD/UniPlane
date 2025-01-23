#pragma warning(disable : 4996)
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <Windows.h>
#include "UniPlaneHeader.h"



const char welomce_icon[] = "\xE2\x97\x88";

int welcome() {
	SetConsoleOutputCP(CP_UTF8);
	printf(" %s Welcome to the University Course Selection System! %s\n\n", welomce_icon, welomce_icon);
	printf(" Determine your status:\n");
	printf(" 1. I was here before (previous:))\n");
	printf(" 2. I will be here from now on.\n");

	return 0;
}

const char* FieldOfStudy[] = {
	"Computer Engineering",
	"Industrial Engineering",
	"Mechanical Engineering",
	"Physics",
	"Chemical Engineering",
	"Useless Engineering(EE)",
};
const int FieldOfStudyCount = 6;


void signup(Student* Students, int* StudentCount) {
	Student newStudent;
	int valid = 0;

	printf("---- Student Registration ----\n");

	printf("please Enter Name: ");
	fgets(newStudent.Name, sizeof(newStudent.Name), stdin);
	newStudent.Name[strcspn(newStudent.Name, "\n")] = 0;
	
	printf("\nEnter LastNmae: ");
	scanf("%s", newStudent.LastName);

	do {
		valid = 1;
		printf("\nEnter Username: ");
		scanf("%s", newStudent.UserName);

		for (int i = 0; i < *StudentCount; ++i) {
			if (strcmp(Students[i].UserName, newStudent.UserName) == 0) {
				printf("\nUsername already exists! Please choose another.");
				valid = 0;
				break;
			}
		}
	} while (!valid);

	do {
		printf("\nEnter Email (must end with @gmail.com): ");
		scanf("%s", newStudent.Email);
		if (strstr(newStudent.Email, "@gmail.com") == NULL || strcmp(strchr(newStudent.Email, '@'), "@gmail.com") != 0) {
			printf("\nInvalid email format! Please try again.");
		}
		else {
			valid = 1;
		}
	} while (!valid);

	do {
		printf("\nEnter StudentNumber: ");
		scanf("%s", newStudent.Id);
		if(strlen(newStudent.Id) !=8|| strspn(newStudent.Id, "0123456789") != 8) {
			printf("\nInvalid Student ID! Please enter exactly 8 digits.");
			valid = 0;
		}
		else {
			valid = 1;
		}
	} while (!valid);




	printf("\nSelect Field of study: ");
	for (int i = 0; i < FieldOfStudyCount; i++) {
		printf("\n %d. %s", i + 1, FieldOfStudy[i]);
	}
	int FieldChoice;
	do {
		printf("\nEnter Choice (1-%d): ", FieldOfStudyCount);
		scanf("%d",&FieldChoice);
		if (FieldChoice < 1 || FieldChoice > FieldOfStudyCount) {
			printf("\nInvalid choice! Please try again.");
		}
		else {
			strcpy(newStudent.FieldOfStudy, FieldOfStudy[FieldChoice - 1]);
			valid = 1;
		}


	} while (!valid);

	printf("\nNumber of units passed: ");
	scanf("%d", &newStudent.NumberOfUnitsPassed);
	printf("\nEnter Number Of Passed Courses (Max %d): ", MAX_PASSED_COURSES);
	int PassedCount;
	scanf("%d", &PassedCount);
	if (PassedCount > MAX_PASSED_COURSES) {
		PassedCount = MAX_PASSED_COURSES;
	}

	for (int i = 0; i < PassedCount; i++) {
		printf("\nEnter code for course %d: ", i + 1);
		scanf("%s", newStudent.CoursesPassed[i]);
	}
	printf("\nEnter Password (minimum 6 characters): ");
	do {
		scanf("%s", newStudent.password);
		if (strlen(newStudent.password) < 6) {
			printf("Password too short! Please enter at least 6 characters: ");
		}
	} while (strlen(newStudent.password) < 6);

	Students[*StudentCount] = newStudent;
	(*StudentCount)++;
	printf("Registration successful!\n");
}


int login() {
	char username[20], password[20];
	printf("Welcome Back! Please log in.\n");
	printf("Username: ");
	scanf("%s", username);
	printf("Password: ");
	scanf("%s", password);

	if (strcmp(username, "admin") == 0 && strcmp(password, "1234") == 0) {
		printf("Login successful!\n");
		return 1;
	}
	else {
		printf("Invalid username or password. Try again.\n");
		return 0;
	}
}


void Golestan() {
	int choice;
	printf("\n---- Golestan Menu ----");
	printf("\n1. View list of courses, students, and curricula");
	printf("\n2. Manage prerequisites");
	printf("\n3. Exit");
	printf("\nEnter your choice: ");
	scanf("%d", &choice);

	switch (choice) {
	case 1:
		printf("\n1. List of courses");
		printf("\n2. List of students");
		printf("\n3. List of curricula");
		break;
	case 2:
		printf("\n1. Add prerequisites");
		printf("\n2. Remove prerequisites");
		printf("\n3. Change prerequisites");
		break;
	case 3:
		printf("\nExiting Golestan menu...");
		break;
	default:
		printf("\nInvalid choice! Please try again.");
	}
}


