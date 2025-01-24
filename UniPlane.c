#pragma warning(disable : 4996)
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <Windows.h>
#include "UniPlaneHeader.h"



const char welcome_icon[] = "\xE2\x97\x88";

int welcome() {
	SetConsoleOutputCP(CP_UTF8);
	printf("\n %s Welcome to the University Course Selection System! %s\n\n", welcome_icon, welcome_icon);
	printf(" Determine your status:\n");
	printf(" 1. I was here before (previous user:))\n");
	printf(" 2. I will be here from now on(new user).\n");
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
	"Mathematical",
	"Physics",
	"Chemical Engineering",
	"Useless Engineering(EE)",
};
const int FieldOfStudyCount = 7;

void SaveToFile(Student* student) {
	FILE* file = fopen("Students.dat", "ab");
	if (file == NULL) {
		printf("Error opening file!\n");
		return;
	}
	fwrite(student, sizeof(Student), 1, file);
	fclose(file);
}

void LoadFromFile(Student** head) {
	FILE* file = fopen("Students.dat", "rb");
	if (file == NULL) {
		printf("--- No existing data file found. Starting with an empty list.---\n");
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

void SaveCourseToFile(Course* course) {
	FILE* file = fopen("Courses.dat", "ab");
	if (file == NULL) {
		printf("Error opening file!\n");
		return;
	}
	fwrite(course, sizeof(Course), 1, file);
	fclose(file);
}

void LoadCoursesFromFile(Course** head) {
	FILE* file = fopen("Courses.dat", "rb");
	if (file == NULL) {
		printf("--- No existing course data file found. A new file will be created.---\n");
		return;
	}
	Course temp;
	while (fread(&temp, sizeof(Course), 1, file)) {
		Course* newCourse = (Course*)malloc(sizeof(Course));
		if (newCourse == NULL) {
			printf("Memory allocation failed!\n");
			fclose(file);
			return;
		}
		*newCourse = temp;
		newCourse->next = *head;
		*head = newCourse;
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
	printf("Enter LastName: ");
	if (scanf("%s", newStudent->LastName) != 1 || strcmp(newStudent->LastName, "0") == 0) {
		free(newStudent);
		return;
	}
	do {
		valid = 1;
		printf("Enter Username: ");
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
		printf("Enter Email (must end with @gmail.com): ");
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
		printf("Enter Student ID (8 digits starting with 400, 401, 402, or 403): ");
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


	printf("Select Field of study: \n");
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

	char password[20], confirmPassword[20];
	do {
		printf("Enter Password (minimum 6 characters): ");
		if (scanf("%s", password) != 1 || strcmp(password, "0") == 0) {
			free(newStudent);
			return;
		}
		if (strlen(password) < 6) {
			printf("Password too short! Please enter at least 6 characters.\n");
			continue;
		}

		printf("Confirm Password: ");
		if (scanf("%s", confirmPassword) != 1 || strcmp(confirmPassword, "0") == 0) {
			free(newStudent);
			return;
		}

		if (strcmp(password, confirmPassword) != 0) {
			printf("Passwords do not match! Please try again.\n");
		}
		else {
			strcpy(newStudent->password, password);
			valid = 1;
		}
	} while (!valid);


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
		return 1;
	}

	printf("Username or Password is incorrect. Try again later.\n");
	return 0;
}

void AddCourse(Course** head) {
	Course* newCourse = (Course*)malloc(sizeof(Course));
	if (newCourse == NULL) {
		printf("Memory allocation failed!\n");
		return;
	}
	printf("--- Add New Course ---\n");

	printf("Enter Course Name: ");
	getchar(); // برای حذف کاراکتر جدیده که از خط قبلی اینجا مونده
	fgets(newCourse->CourseName, sizeof(newCourse->CourseName), stdin);
	newCourse->CourseName[strcspn(newCourse->CourseName, "\n")] = '\0'; // حذف کاراکتر اون خط جدیده

	printf("Enter Course ID: ");
	fgets(newCourse->CourseID, sizeof(newCourse->CourseID), stdin);
	newCourse->CourseID[strcspn(newCourse->CourseID, "\n")] = '\0';

	printf("Enter Professor: ");
	fgets(newCourse->Professor, sizeof(newCourse->Professor), stdin);
	newCourse->Professor[strcspn(newCourse->Professor, "\n")] = '\0';

	printf("Enter Time: ");
	fgets(newCourse->Time, sizeof(newCourse->Time), stdin);
	newCourse->Time[strcspn(newCourse->Time, "\n")] = '\0';

	printf("Select Department from the list:\n");
	for (int i = 0; i < FieldOfStudyCount; i++) {
		printf("%d. %s\n", i + 1, FieldOfStudy[i]);
	}
	int departmentChoice;
	do {
		printf("Enter Choice (1-%d): ", FieldOfStudyCount);
		scanf("%d", &departmentChoice);
		if (departmentChoice < 1 || departmentChoice > FieldOfStudyCount) {
			printf("Invalid choice! Please try again.\n");
		}
		else {
			strcpy(newCourse->Department, FieldOfStudy[departmentChoice - 1]);
			break;
		}
	} while (1);

	printf("Enter Minimum Units Required: ");
	scanf("%d", &newCourse->MinUnitsRequired);

	char hasPrerequisites;
	printf("Does this course have prerequisites? (y/n): ");
	scanf(" %c", &hasPrerequisites);

	if (hasPrerequisites == 'y' || hasPrerequisites == 'Y') {
		printf("Enter Prerequisites (up to %d, enter 0 to stop):\n", MAX_PREREQUISITES);
		for (int i = 0; i < MAX_PREREQUISITES; i++) {
			printf("Prerequisite %d: ", i + 1);
			scanf("%s", newCourse->Prerequisites[i]);
			if (strcmp(newCourse->Prerequisites[i], "0") == 0) {
				newCourse->Prerequisites[i][0] = '\0';
				break;
			}
		}
	}
	else {
		for (int i = 0; i < MAX_PREREQUISITES; i++) {
			newCourse->Prerequisites[i][0] = '\0';
		}
	}

	newCourse->next = *head;
	*head = newCourse;
	SaveCourseToFile(newCourse);
	printf("Course added successfully!\n");
}

void ManagePrerequisites(Course* courseHead) {
	char courseID[10];
	printf("Enter Course ID to manage prerequisites: ");
	scanf("%s", courseID);

	Course* course = courseHead;
	while (course != NULL) {
		if (strcmp(course->CourseID, courseID) == 0) {
			printf("\nManaging Prerequisites for Course: %s\n", course->CourseName);
			int choice;
			do {
				printf("\n1. Add Prerequisite");
				printf("\n2. Change Prerequisite");
				printf("\n3. Remove Prerequisite");
				printf("\n4. View Prerequisites");
				printf("\n5. Back to Manage Menu");
				printf("\nEnter your choice: ");
				if (scanf("%d", &choice) != 1) {
					printf("Invalid input! Please enter a number.\n");
					while (getchar() != '\n'); // پاک کردن بافر ورودی
					continue;
				}

				switch (choice) {
				case 1:
					AddPrerequisite(course);
					break;
				case 2:
					ChangePrerequisite(course);
					break;
				case 3:
					RemovePrerequisite(course);
					break;
				case 4:
					ViewPrerequisites(course);
					break;
				case 5:
					printf("\nReturning to Manage menu...\n");
					return;
				default:
					printf("\nInvalid choice! Please try again.");
				}
			} while (1);
		}
		course = course->next;
	}
	printf("Course with ID %s not found!\n", courseID);
}

void AddPrerequisite(Course* course) {
	if (course->Prerequisites[MAX_PREREQUISITES - 1][0] != '\0') {
		printf("Maximum number of prerequisites reached!\n");
		return;
	}

	char prerequisiteID[10];
	printf("Enter Prerequisite Course ID: ");
	scanf("%s", prerequisiteID);

	for (int i = 0; i < MAX_PREREQUISITES; i++) {
		if (course->Prerequisites[i][0] == '\0') {
			strcpy(course->Prerequisites[i], prerequisiteID);
			printf("Prerequisite added successfully!\n");
			SaveCourseToFile(course);
			return;
		}
	}
}

void ChangePrerequisite(Course* course) {
	char oldPrerequisiteID[10], newPrerequisiteID[10];
	printf("Enter Old Prerequisite Course ID: ");
	scanf("%s", oldPrerequisiteID);
	printf("Enter New Prerequisite Course ID: ");
	scanf("%s", newPrerequisiteID);

	for (int i = 0; i < MAX_PREREQUISITES; i++) {
		if (strcmp(course->Prerequisites[i], oldPrerequisiteID) == 0) {
			strcpy(course->Prerequisites[i], newPrerequisiteID);
			printf("Prerequisite changed successfully!\n");
			SaveCourseToFile(course);
			return;
		}
	}
	printf("Old prerequisite not found!\n");
}

void RemovePrerequisite(Course* course) {
	char prerequisiteID[10];
	printf("Enter Prerequisite Course ID to remove: ");
	scanf("%s", prerequisiteID);

	for (int i = 0; i < MAX_PREREQUISITES; i++) {
		if (strcmp(course->Prerequisites[i], prerequisiteID) == 0) {
			course->Prerequisites[i][0] = '\0';
			printf("Prerequisite removed successfully!\n");
			SaveCourseToFile(course);
			return;
		}
	}
	printf("Prerequisite not found!\n");
}

void ListStudents(Student* head) {
	if (head == NULL) {
		printf("No students found!\n");
		return;
	}

	printf("\n---- List of Students ----\n");
	Student* temp = head;
	while (temp != NULL) {
		printf("Name: %s %s\n", temp->Name, temp->LastName);
		printf("Username: %s\n", temp->UserName);
		printf("Email: %s\n", temp->Email);
		printf("Student ID: %s\n", temp->ID);
		printf("Field of Study: %s\n", temp->FieldOfStudy);
		printf("Number of Units Passed: %d\n", temp->NumberOfUnitsPassed);
		printf("Courses Passed: ");
		for (int i = 0; i < MAX_PASSED_COURSES; i++) {
			if (temp->CoursesPassed[i][0] != '\0') {
				printf("%s ", temp->CoursesPassed[i]);
			}
		}
		printf("\n--------------------------\n");
		temp = temp->next;
	}
}

void ListCourses(Course* head) {
	if (head == NULL) {
		printf("No courses found!\n");
		return;
	}

	printf("\n---- List of Courses ----\n");
	Course* temp = head;
	while (temp != NULL) {
		printf("Course Name: %s\n", temp->CourseName);
		printf("Course ID: %s\n", temp->CourseID);
		printf("Professor: %s\n", temp->Professor);
		printf("Time: %s\n", temp->Time);
		printf("Department: %s\n", temp->Department);
		printf("Minimum Units Required: %d\n", temp->MinUnitsRequired);
		printf("Prerequisites: ");
		for (int i = 0; i < MAX_PREREQUISITES; i++) {
			if (temp->Prerequisites[i][0] != '\0') {
				printf("%s ", temp->Prerequisites[i]);
			}
		}
		printf("\n--------------------------\n");
		temp = temp->next;
	}
}

void ViewPrerequisites(Course* course) {
	printf("\nPrerequisites for Course %s:\n", course->CourseName);
	for (int i = 0; i < MAX_PREREQUISITES; i++) {
		if (course->Prerequisites[i][0] != '\0') {
			printf("%d. %s\n", i + 1, course->Prerequisites[i]);
		}
	}
}

void ViewMenu(Student* studentHead, Course* courseHead) {
	int choice;
	do {
		printf("\n---- View Menu ----");
		printf("\n1. List of Courses");
		printf("\n2. List of Students");
		printf("\n3. Curricula (Not Implemented)");
		printf("\n4. Back to Golestan Menu");
		printf("\nEnter your choice: ");
		if (scanf("%d", &choice) != 1) {
			printf("Invalid input! Please enter a number.\n");
			while (getchar() != '\n'); // پاک کردن بافر ورودی
			continue;
		}

		switch (choice) {
		case 1:
			ListCourses(courseHead);
			break;
		case 2:
			ListStudents(studentHead);
			break;
		case 3:
			printf("\nCurricula feature is not implemented yet.\n");
			break;
		case 4:
			printf("\nReturning to Golestan menu...\n");
			return;
		default:
			printf("\nInvalid choice! Please try again.");
		}
	} while (1);
}

void ManageMenu(Course** courseHead) {
	int choice;
	do {
		printf("\n---- Manage Menu ----");
		printf("\n1. Manage Prerequisites (Add/Change/Remove)");
		printf("\n2. Add a New Course");
		printf("\n3. Back to Golestan Menu");
		printf("\nEnter your choice: ");
		if (scanf("%d", &choice) != 1) {
			printf("Invalid input! Please enter a number.\n");
			while (getchar() != '\n'); // پاک کردن بافر ورودی
			continue;
		}

		switch (choice) {
		case 1:
			ManagePrerequisites(*courseHead);
			break;
		case 2:
			AddCourse(courseHead);
			break;
		case 3:
			printf("\nReturning to Golestan menu...\n");
			return;
		default:
			printf("\nInvalid choice! Please try again.");
		}
	} while (1);
}

void Golestan(Student* studentHead, Course** courseHead) {

	int choice;
	do {
		printf("\n---- Golestan Menu ----");
		printf("\n1. View");
		printf("\n2. Manage");
		printf("\n3. Exit");
		printf("\nEnter your choice: ");
		if (scanf("%d", &choice) != 1) {
			printf("Invalid input! Please enter a number.\n");
			while (getchar() != '\n'); // پاک کردن بافر ورودی
			continue;
		}

		switch (choice) {
		case 1:
			ViewMenu(studentHead, *courseHead);
			break;
		case 2:
			ManageMenu(courseHead);
			break;
		case 3:
			printf("\nExiting Golestan menu...\n");
			return;
		default:
			printf("\nInvalid choice! Please try again.");
		}
	} while (1);
}



void FreeStudentList(Student* head) {
	Student* temp;
	while (head != NULL) {
		temp = head;
		head = head->next;
		free(temp);
	}
}

void FreeCourseList(Course* head) {
	Course* temp;
	while (head != NULL) {
		temp = head;
		head = head->next;
		free(temp);
	}
}