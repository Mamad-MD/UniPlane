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
	printf(" 3. Enter Golestan System.\n");
	printf(" 0. Exit.\n");

	int choice;
	if (scanf("%d", &choice) != 1) {
		printf("Invalid input! Please enter a number.\n");
		while (getchar() != '\n'); // واسه اینکه بافر ورودی را پاک کنیم
		return -1; 
	}
	return choice;
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

void SaveToFile(Student* student) {
	FILE* file = fopen("Student.dat", "ab");
	if (file == NULL) {
		printf("Error opening file!\n");
		return;
	}
	fwrite(student, sizeof(Student), 1, file);
	fclose(file);
}

void LoadFromFile(Student** head) {
	FILE* file = fopen("Student.dat", "rb");
	if (file == NULL) {
		printf("---No existing data file found. Starting with an empty list.---\n\n");
		return;
	}
	Student temp;
	while (fread(&temp, sizeof(Student), 1, file)) {
		Student* newStudent = (Student*)malloc(sizeof(Student));
		if (newStudent == NULL) {
			printf("Memory allocation failed!\n");
			fclose(file);
			return;
		}
		*newStudent = temp;
		newStudent->next = *head;
		*head = newStudent;
	}
	fclose(file);
}

void signup(Student** head) {
	Student* newStudent = (Student*)malloc(sizeof(Student));
	if (newStudent == NULL) {
		printf("Memory allocation failed!\n");
		return;
	}
	int valid = 0;

	printf("---- Student Registration ----\n\n");
	printf("Enter 0 at any time to go back.\n");

	printf("please Enter Name: ");
	if (scanf("%s", newStudent->Name) != 1 || strcmp(newStudent->Name, "0") == 0) {
		free(newStudent);
		return;
	}
	printf("\nEnter LastNmae: ");
	if (scanf("%s", newStudent->LastName) != 1 || strcmp(newStudent->LastName, "0") == 0) {
		free(newStudent);
		return;
	}
	do {
		valid = 1;
		printf("\nEnter Username: ");
		if (scanf("%s", newStudent->UserName) != 1 || strcmp(newStudent->UserName, "0") == 0) {
			free(newStudent);
			return;
		}

		Student* temp = *head;
		while (temp != NULL) {
			if (strcmp(temp->UserName, newStudent->UserName) == 0) {
				printf("Username already exists! Please choose another.\n");
				valid = 0;
				break;
			}
			temp = temp->next;
		}
	} while (!valid);

	do {
		printf("\nEnter Email (must end with @gmail.com): ");
		if (scanf("%s", newStudent->Email) != 1 || strcmp(newStudent->Email, "0") == 0) {
			free(newStudent);
			return;
		}
		char* atSign = strchr(newStudent->Email, '@');
		if (atSign == NULL || strcmp(atSign, "@gmail.com") != 0) {
			printf("Invalid email format! Please try again.\n");
		}
		else {
			valid = 1;
		}
	} while (!valid);

	do {
		valid = 1;
		printf("\nEnter Student ID (8 digits starting with 400, 401, 402, or 403): ");
		if (scanf("%s", newStudent->ID) != 1 || strcmp(newStudent->ID, "0") == 0) {
			free(newStudent);
			return;
		}

		if (strlen(newStudent->ID) != 8 || strspn(newStudent->ID, "0123456789") != 8) {
			printf("Invalid Student ID! Please enter exactly 8 numeric digits.\n");
			valid = 0;
		}
		else if (strncmp(newStudent->ID, "400", 3) != 0 &&
			strncmp(newStudent->ID, "401", 3) != 0 &&
			strncmp(newStudent->ID, "402", 3) != 0 &&
			strncmp(newStudent->ID, "403", 3) != 0) {
			printf("Student ID must start with 400, 401, 402, or 403.\n");
			valid = 0;
		}
	} while (!valid);


	printf("\nSelect Field of study: \n");
	for (int i = 0; i < FieldOfStudyCount; i++) {
		printf("%d. %s\n", i + 1, FieldOfStudy[i]);
	}
	int FieldChoice;
	do {
		printf("Enter Choice (1-%d): ", FieldOfStudyCount);
		if (scanf("%d", &FieldChoice) != 1 || FieldChoice < 1 || FieldChoice > FieldOfStudyCount) {
			printf("Invalid choice! Please try again.\n");
		}
		else {
			strcpy(newStudent->FieldOfStudy, FieldOfStudy[FieldChoice - 1]);
			valid = 1;
		}
	} while (!valid);

	printf("Enter Number of units passed: ");
	if (scanf("%d", &newStudent->NumberOfUnitsPassed) != 1) {
		free(newStudent);
		return;
	}

	printf("Enter Number Of Passed Courses (Max %d): ", MAX_PASSED_COURSES);
	int PassedCount;
	if (scanf("%d", &PassedCount) != 1 || PassedCount < 0 || PassedCount > MAX_PASSED_COURSES) {
		PassedCount = MAX_PASSED_COURSES;
	}

	for (int i = 0; i < PassedCount; i++) {
		printf("Enter code for course %d: ", i + 1);
		if (scanf("%s", newStudent->CoursesPassed[i]) != 1) {
			free(newStudent);
			return;
		}
	}

	printf("Enter Password (minimum 6 characters): ");
	do {
		if (scanf("%s", newStudent->password) != 1 || strcmp(newStudent->password, "0") == 0) {
			free(newStudent);
			return;
		}
		if (strlen(newStudent->password) < 6) {
			printf("Password too short! Please enter at least 6 characters: ");
		}
	} while (strlen(newStudent->password) < 6);

	newStudent->next = *head;
	*head = newStudent;
	SaveToFile(newStudent);
	printf("Registration successful!\n");
}

int login(Student* head) {
	char username[20], password[20];
	printf("Welcome Back! Please log in.\n\n");
	printf("Enter 0 at any time to go back.\n");

	printf("Username: ");
	if (scanf("%s", username) != 1 || strcmp(username, "0") == 0) {
		return 0;
	}

	printf("Password: ");
	if (scanf("%s", password) != 1 || strcmp(password, "0") == 0) {
		return 0;
	}

	Student* temp = head;
	while (temp != NULL) {
		if (strcmp(temp->UserName, username) == 0 && strcmp(temp->password, password) == 0) {
			printf("Login successful! Welcome %s %s.\n", temp->Name, temp->LastName);
			return 1;
		}
		temp = temp->next;
	}

	if (strcmp(username, "Golestan") == 0 && strcmp(password, "12345") == 0) {
		printf("Golestan login successful!\n");
		Golestan();
		return 1;
	}

	printf("Invalid username or password. Try again.\n");
	return 0;
}


void Golestan() {
	int choice;
	do {
		printf("\n---- Golestan Menu ----");
		printf("\n1. View list of courses, students, and curricula");
		printf("\n2. Manage prerequisites");
		printf("\n3. Exit");
		printf("\nEnter your choice: ");
		if (scanf("%d", &choice) != 1) {
			printf("Invalid input! Please enter a number.\n");
			while (getchar() != '\n'); // پاک کردن بافر ورودی
			continue;
		}

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
			return;
		default:
			printf("\nInvalid choice! Please try again.");
		}
	} while (choice != 3);
}


