#pragma warning(disable : 4996)
#define DEBUG
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <Windows.h>
#include <stdbool.h>
#include "UniPlaneHeader.h"

const char welcome_icon[] = "\xE2\x97\x88";
int scheduleCount = 0;

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
		while (getchar() != '\n');
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
		printf("Error opening file! Students.dat\n");
		return;
	}
	if (fwrite(student, sizeof(Student), 1, file) != 1) {
		printf("⚠ Error writing to file Students.dat!\n");
	}
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
		if (strlen(temp.ID) == 0 || strlen(temp.Name) == 0 || strlen(temp.Email) == 0) {
			printf("⚠ Invalid student data found! Skipping...\n");
			continue; 
		}

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
		printf("Error opening file! Courses.dat\n");
		return;
	}
	fwrite(course, sizeof(Course), 1, file);
	for (int i = 0; i < course->PrerequisiteCount; i++) {
		if (fwrite(course->Prerequisites[i], sizeof(char), 10, file) != 10) {
			printf("⚠ Error writing prerequisites to file!\n");
			fclose(file);
			return;
		}
	}
	fclose(file);
}


void LoadCoursesFromFile(Course** head) {
	FILE* file = fopen("Courses.dat", "rb");
	if (file == NULL) {
		printf("--- No existing course data file found. A new file will be created.---\n");
		return;
	}

	fseek(file, 0, SEEK_END);
	if (ftell(file) == 0) {
		printf("--- The course data file is empty. ---\n");
		fclose(file);
		return;
	}
	fseek(file, 0, SEEK_SET);

	Course temp;
	while (fread(&temp, sizeof(Course), 1, file)) {
		Course* newCourse = (Course*)malloc(sizeof(Course));
		if (newCourse == NULL) {
			printf("Memory allocation failed!\n");
			fclose(file);
			return;
		}
		*newCourse = temp;

		if (newCourse->PrerequisiteCount > 0) {
			for (int i = 0; i < newCourse->PrerequisiteCount; i++) {
				if (fread(newCourse->Prerequisites[i], sizeof(char), 10, file) != 10) {
					printf("Error reading prerequisites from file!\n");
					free(newCourse);
					fclose(file);
					return;
				}
			}
		}

		newCourse->next = *head;
		*head = newCourse;
	}

	fclose(file);
}

void SaveSchedulesToFile() {
	FILE* file = fopen("schedules.dat", "wb");
	if (file == NULL) {
		printf("Error opening file! schedules.dat\n");
		return;
	}

	fwrite(&scheduleCount, sizeof(int), 1, file);
	fwrite(allSchedules, sizeof(StudentSchedule), scheduleCount, file);

	fclose(file);
	printf("Schedules saved to file successfully!\n");
}

void LoadSchedulesFromFile() {
	FILE* file = fopen("schedules.dat", "rb");
	if (file == NULL) {
		printf("--- No existing schedules found. Starting with an empty list.---\n");
		return;
	}

	fread(&scheduleCount, sizeof(int), 1, file);
	fread(allSchedules, sizeof(StudentSchedule), scheduleCount, file);

	fclose(file);
	printf("Schedules loaded from file successfully!\n");
}

void simpleHash(char* input, char* output) {
	unsigned long hash = 5381;
	int c;

	while ((c = *input++)) {
		hash = ((hash << 5) + hash) + c;
	}

	sprintf(output, "%lx", hash);
}

void signup(Student** head, Course* courseHead) {
	Student* newStudent = (Student*)malloc(sizeof(Student));
	if (newStudent == NULL) {
		printf("Memory allocation failed!\n");
		return;
	}
	memset(newStudent, 0, sizeof(Student));
	int valid = 0;

	printf("---- Student Registration ----\n\n");
	printf("Enter 0 at any time to go back.\n");

	printf("Please Enter Name: ");
	if (scanf("%49s", newStudent->Name) != 1 || strcmp(newStudent->Name, "0") == 0) {
		free(newStudent);
		return;
	}
	printf("Enter LastName: ");
	if (scanf("%49s", newStudent->LastName) != 1 || strcmp(newStudent->LastName, "0") == 0) {
		free(newStudent);
		return;
	}
	do {
		valid = 1;
		printf("Enter Username: ");
		if (scanf("%19s", newStudent->UserName) != 1 || strcmp(newStudent->UserName, "0") == 0) {
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
		valid = 0;
		printf("Enter Email (must end with @gmail.com): ");
		if (scanf("%99s", newStudent->Email) != 1 || strcmp(newStudent->Email, "0") == 0) {
			free(newStudent);
			return;
		}
		char* atSign = strchr(newStudent->Email, '@');
		if (atSign == NULL) {
			printf("Invalid email format! Email must contain '@'.\n");
		}
		else if (strcmp(atSign, "@gmail.com") != 0) {
			printf("Invalid email format! Email must end with '@gmail.com'.\n");
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

	printf("\nSelect Field of study: \n");

	for (int i = 0; i < FieldOfStudyCount; i++) {
		printf("%d. %s\n", i + 1, FieldOfStudy[i]);
	}
	int FieldChoice;
	char input[100];
	do {
		printf("Enter Choice (1-%d): ", FieldOfStudyCount);
		if (scanf("%s", input) != 1) {
			printf("Invalid input! Please enter a number.\n");
			while (getchar() != '\n');
			continue;
		}
		FieldChoice = atoi(input);


		if (FieldChoice < 1 || FieldChoice > FieldOfStudyCount) {
			printf("Invalid choice! Please enter a number between 1 and %d.\n", FieldOfStudyCount);
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
	printf("\n---- List of Existing Courses ----\n");
	Course* tempCourse = courseHead;
	while (tempCourse != NULL) {
		printf("Course ID: %s, Course Name: %s\n", tempCourse->CourseID, tempCourse->CourseName);
		tempCourse = tempCourse->next;
	}

	printf("Enter Number Of Passed Courses (Max %d): ", MAX_PASSED_COURSES);
	int PassedCount;
	if (scanf("%d", &PassedCount) != 1 || PassedCount < 0 || PassedCount > MAX_PASSED_COURSES) {
		PassedCount = MAX_PASSED_COURSES;
	}

	for (int i = 0; i < PassedCount; i++) {
		char courseID[20];
		printf("Enter code for course %d (e.g., 101-01): ", i + 1);
		if (scanf("%19s", courseID) != 1) {
			printf("Invalid input! Please enter a valid course ID.\n");
			i--;
			continue;
		}

		Course* tempCourse = courseHead;
		int found = 0;
		while (tempCourse != NULL) {
			if (strcmp(tempCourse->CourseID, courseID) == 0) {
				printf("Found Course: %s (%s)\n", tempCourse->CourseName, tempCourse->CourseID);
				strcpy(newStudent->CoursesPassed[i], tempCourse->CourseID);
				found = 1;
				break;
			}
			tempCourse = tempCourse->next;
		}

		if (!found) {
			printf("No course found with ID %s! Please try again.\n", courseID);
			i--;
		}
	}
	char password[20], confirmPassword[20];
	do {
		valid = 0;
		printf("Enter Password (minimum 6 characters): ");
		if (scanf("%19s", password) != 1) {
			printf("Invalid input! Please try again.\n");
			while (getchar() != '\n');
			continue;
		}
		if (strcmp(password, "0") == 0) {
			free(newStudent);
			return;
		}
		if (strlen(password) < 6) {
			printf("Password too short! Please enter at least 6 characters.\n");
			while (getchar() != '\n');
			continue;
		}

		printf("Confirm Password: ");
		if (scanf("%19s", confirmPassword) != 1) {
			printf("Invalid input! Please try again.\n");
			while (getchar() != '\n');
			continue;
		}
		if (strcmp(confirmPassword, "0") == 0) {
			free(newStudent);
			return;
		}

		if (strcmp(password, confirmPassword) != 0) {
			printf("Passwords do not match! Please try again.\n");
			while (getchar() != '\n');
		}
		else {
			char hashedPassword[32];
			simpleHash(password, hashedPassword);  
			strcpy(newStudent->password, hashedPassword);  
			break;
		}
	} while (!valid);

	newStudent->next = *head;
	*head = newStudent;
	SaveToFile(newStudent);
	printf("Registration successful!\n");
}

int login(Student* head) {
	char username[20], password[20];

	printf("\nWelcome Back! Please log in.\n\n");
	printf("Enter 0 at any time to go back.\n");

	printf("Username: ");
	if (scanf("%19s", username) != 1 || strcmp(username, "0") == 0) {
		return 0;
	}

	printf("Password: ");
	if (scanf("%19s", password) != 1 || strcmp(password, "0") == 0) {
		return 0;
	}

	char hashedPassword[32];
	simpleHash(password, hashedPassword);


	Student* temp = head;
	while (temp != NULL) {
		if (strcmp(temp->UserName, username) == 0 && strcmp(temp->password, hashedPassword) == 0) {
			printf("\nLogin successful! Welcome %s %s.\n", temp->Name, temp->LastName);
			return 1;  
		}
		temp = temp->next;
	}

	if (strcmp(username, "Golestan") == 0 && strcmp(password, "12345") == 0) {
		printf("\nGolestan login successful!\n");
		return 2; 
	}


	printf("Username or Password is incorrect. Try again later.\n");
	return 0;
}

void GenerateCourseID(char* courseID, const char* baseID, Course* head) {
	int counter = 1;  // شمارنده را از 1 شروع می‌کنیم

	// بررسی لیست دروس برای یافتن بزرگ‌ترین شماره‌ی موجود برای این پیشوند
	Course* temp = head;
	while (temp != NULL) {
		char existingPrefix[10];
		int existingNumber;

		// استخراج بخش شماره از CourseID موجود
		if (sscanf(temp->CourseID, "%3s-%02d", existingPrefix, &existingNumber) == 2) {
			if (strcmp(existingPrefix, baseID) == 0 && existingNumber >= counter) {
				counter = existingNumber + 1; // شماره جدید باید از بزرگ‌ترین مقدار قبلی بیشتر باشد
			}
		}
		temp = temp->next;
	}

	// تولید شناسه جدید بر اساس شماره‌ای که پیدا کردیم
	sprintf(courseID, "%s-%02d", baseID, counter);
}

void AddCourse(Course** head) {
	if (head == NULL) {
		printf("Error: Course list is not initialized.\n");
		return;
	}
	Course* newCourse = (Course*)malloc(sizeof(Course));
	if (newCourse == NULL) {
		printf("Memory allocation failed!\n");
		return;
	}
	memset(newCourse, 0, sizeof(Course));

	printf("\n--- Add New Course ---\n");

	printf("Enter Course Name: ");
	getchar();
	fgets(newCourse->CourseName, sizeof(newCourse->CourseName), stdin);
	newCourse->CourseName[strcspn(newCourse->CourseName, "\n")] = '\0';

	char baseID[10];
	printf("Enter Base Course ID (e.g., 101): ");
	scanf("%9s", baseID);

	// اصلاح شده: پاس دادن لیست `head` به `GenerateCourseID`
	GenerateCourseID(newCourse->CourseID, baseID, *head);

	printf("Auto-generated Course ID: %s\n", newCourse->CourseID);

	printf("Enter Number of Units for this Course: ");
	if (scanf("%d", &newCourse->Units) != 1 || newCourse->Units < 1) {
		printf("Invalid number of units! Please enter a positive integer.\n");
		free(newCourse);
		return;
	}


	printf("Enter Time (for example, 8:00 10:00 Saturday-Monday): ");
	char timeInput[100];
	getchar();  // واسه حذف کاراکتر جدید خط قبلی
	fgets(timeInput, sizeof(timeInput), stdin);
	timeInput[strcspn(timeInput, "\n")] = '\0';
	sscanf(timeInput, "%s %s %[^\n]", newCourse->StartTime, newCourse->EndTime, newCourse->Days);

	printf("Enter Professor: ");
	fgets(newCourse->Professor, sizeof(newCourse->Professor), stdin);
	newCourse->Professor[strcspn(newCourse->Professor, "\n")] = '\0';

	printf("\nSelect Department from the list:\n");
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

	printf("\nEnter Minimum Units Required to Register for this Course: ");
	if (scanf("%d", &newCourse->MinUnitsRequired) != 1 || newCourse->MinUnitsRequired < 0) {
		printf("Invalid minimum units! Please enter a non-negative integer.\n");
		free(newCourse);
		return;
	}

	printf("\n---- List of Existing Courses ----\n");
	Course* temp = *head;
	while (temp != NULL) {
		char courseIDPrefix[14];
		strncpy(courseIDPrefix, temp->CourseID, 6);
		courseIDPrefix[6] = '\0';
		printf("Course ID: %s, Course Name: %s\n", courseIDPrefix, temp->CourseName);
		temp = temp->next;
	}
	char prerequisiteInput[100];
	printf("\nEnter Prerequisite Course IDs (e.g., 101-01, 102-01, or 0 if none): ");
	getchar();
	fgets(prerequisiteInput, sizeof(prerequisiteInput), stdin);
	prerequisiteInput[strcspn(prerequisiteInput, "\n")] = '\0';

	if (strcmp(prerequisiteInput, "0") == 0) {
		newCourse->PrerequisiteCount = 0;
	}
	else {
		char* token = strtok(prerequisiteInput, " ");
		int count = 0;

		while (token != NULL && count < MAX_PREREQUISITES) {
			Course* temp = *head;
			int found = 0;
			while (temp != NULL) {
				if (strcmp(temp->CourseID, token) == 0) {
					strcpy(newCourse->Prerequisites[count], temp->CourseID);
					found = 1;
					break;
				}
				temp = temp->next;
			}

			if (!found) {
				printf("Invalid Course ID: %s! Prerequisite not set.\n", token);
			}
			else {
				count++;
			}

			token = strtok(NULL, " ");
		}

		newCourse->PrerequisiteCount = count;
	}

	int confirmPrerequisites;
	do {
		printf("\nCurrent Prerequisites:\n");
		if (newCourse->PrerequisiteCount == 0) {
			printf("No prerequisites set.\n");
		}
		else {
			for (int i = 0; i < newCourse->PrerequisiteCount; i++) {
				printf("%d. %s\n", i + 1, newCourse->Prerequisites[i]);
			}
		}

		printf("\nAre you okay with these prerequisites?\n");
		printf("1. Yes, continue.\n");
		printf("2. No, I want to change them.\n");
		printf("Enter your choice: ");
		scanf("%d", &confirmPrerequisites);

		if (confirmPrerequisites == 2) {
			ManagePrerequisites(newCourse);
		}
	} while (confirmPrerequisites != 1);

	char corequisiteID[14];
	printf("Enter Corequisite Course ID (e.g., 101-01, or 0 if none): ");
	scanf("%s", corequisiteID);

	if (strcmp(corequisiteID, "0") == 0) {
		newCourse->Corequisite[0] = '\0';
	}
	else {
		Course* temp = *head;
		int found = 0;
		while (temp != NULL) {
			if (strcmp(temp->CourseID, corequisiteID) == 0) {
				strcpy(newCourse->Corequisite, temp->CourseID);
				found = 1;
				break;
			}
			temp = temp->next;
		}
		if (!found) {
			printf("Invalid Course ID! Corequisite not set.\n");
			newCourse->Corequisite[0] = '\0';
		}
	}
	newCourse->next = *head;
	*head = newCourse;
	SaveCourseToFile(newCourse);
	printf("\nCourse added successfully!\n");
}

void ManagePrerequisites(Course* courseHead) {
	char courseID[20];
	printf("Enter Course ID to manage prerequisites: ");
	if (scanf("%s", courseID) != 1) {
		printf("Invalid input! Please enter a valid Course ID.\n");
		while (getchar() != '\n'); // clear buffer
		return;
	}


	Course* tempCourse = courseHead;
	while (tempCourse != NULL) {
		if (strcmp(tempCourse->CourseID, courseID) == 0) {
			printf("\nManaging Prerequisites for Course: %s (%s)\n", tempCourse->CourseName, tempCourse->CourseID);
			int choice;
			do {
				printf("\n1. Add Prerequisite");
				printf("\n2. Change Prerequisite");
				printf("\n3. Remove Prerequisite");
				printf("\n4. View Prerequisites");
				printf("\n5. Back to Manage Menu");
				printf("\nEnter your choice: ");
				scanf("%d", &choice);

				switch (choice) {
				case 1:
					AddPrerequisite(tempCourse);
					break;
				case 2:
					ChangePrerequisite(tempCourse);
					break;
				case 3:
					RemovePrerequisite(tempCourse);
					break;
				case 4:
					ViewPrerequisites(tempCourse);
					break;
				case 5:
					printf("\nReturning to Manage menu...\n");
					return;
				default:
					printf("\nInvalid choice! Please try again.");
				}
			} while (1);
		}
		tempCourse = tempCourse->next;
	}
	printf("Course with ID %s not found!\n", courseID);
}

void AddPrerequisite(Course* course) {
	if (course->PrerequisiteCount >= MAX_PREREQUISITES) {
		printf("Maximum number of prerequisites reached!\n");
		return;
	}

	char prerequisiteID[20];
	printf("Enter Prerequisite Course ID: ");
	if (scanf("%s", prerequisiteID) != 1) {
		printf("Invalid input! Please enter a valid Course ID.\n");
		return;
	}

	for (int i = 0; i < course->PrerequisiteCount; i++) {
		if (strcmp(course->Prerequisites[i], prerequisiteID) == 0) {
			printf("Prerequisite already exists!\n");
			return;
		}
	}

	strcpy(course->Prerequisites[course->PrerequisiteCount], prerequisiteID);
	course->PrerequisiteCount++;
	printf("Prerequisite added successfully!\n");
}

void ChangePrerequisite(Course* course) {
	if (course->PrerequisiteCount == 0) {
		printf("This course has no prerequisites to change!\n");
		return;
	}

	char oldPrerequisiteID[14], newPrerequisiteID[14];
	printf("Enter Old Prerequisite Course ID: ");
	if (scanf("%s", oldPrerequisiteID) != 1) {
		printf("Invalid input! Please enter a valid Course ID.\n");
		return;
	}

	int index = -1;
	for (int i = 0; i < course->PrerequisiteCount; i++) {
		if (strcmp(course->Prerequisites[i], oldPrerequisiteID) == 0) {
			index = i;
			break;
		}
	}

	if (index == -1) {
		printf("Old Prerequisite Course ID not found!\n");
		return;
	}

	printf("Enter New Prerequisite Course ID: ");
	if (scanf("%s", newPrerequisiteID) != 1) {
		printf("Invalid input! Please enter a valid Course ID.\n");
		return;
	}
	
	int found = 0;
	Course* temp = course;
	while (temp != NULL) {
		if (strcmp(temp->CourseID, newPrerequisiteID) == 0) {
			strcpy(course->Prerequisites[index], temp->CourseID);
			found = 1;
			break;
		}
		temp = temp->next;
	}

	if (!found) {
		printf("Invalid Course ID! Prerequisite not changed.\n");
	}
	else {
		printf("Prerequisite changed successfully!\n");
	}
}

void RemovePrerequisite(Course* course) {
	if (course->PrerequisiteCount == 0) {
		printf("This course has no prerequisites to remove!\n");
		return;
	}

	char prerequisiteID[14];
	printf("Enter Prerequisite Course ID to remove (e.g., 101-01): ");
	if (scanf("%s", prerequisiteID) != 1) {
		printf("Invalid input! Please enter a valid Course ID.\n");
		return;
	}


	int index = -1;
	for (int i = 0; i < course->PrerequisiteCount; i++) {
		if (strcmp(course->Prerequisites[i], prerequisiteID) == 0) {
			index = i;
			break;
		}
	}

	if (index == -1) {
		printf("Prerequisite Course ID not found!\n");
		return;
	}

	for (int i = index; i < course->PrerequisiteCount - 1; i++) {
		strcpy(course->Prerequisites[i], course->Prerequisites[i + 1]);
	}
	course->PrerequisiteCount--;
	printf("Prerequisite removed successfully!\n");
}

void ViewPrerequisites(Course* course) {
	printf("\nPrerequisites for Course %s (%s):\n", course->CourseName, course->CourseID);
	if (course->PrerequisiteCount == 0) {
		printf("No prerequisites found.\n");
	}
	else {
		for (int i = 0; i < course->PrerequisiteCount; i++) {
			printf("%d. %s\n", i + 1, course->Prerequisites[i]);
		}
	}
}

void ListStudents(Student* head, Course* courseHead) {
	if (head == NULL) {
		printf("\nNo students found!\n");
		return;
	}

	printf("\n---- List of Students ----\n");
	Student* tempStudent = head;
	while (tempStudent != NULL) {
		printf("Name: %s %s\n", tempStudent->Name, tempStudent->LastName);
		printf("Username: %s\n", tempStudent->UserName);
		printf("Email: %s\n", tempStudent->Email);
		printf("Student ID: %s\n", tempStudent->ID);
		printf("Field of Study: %s\n", tempStudent->FieldOfStudy);
		printf("Number of Units Passed: %d\n", tempStudent->NumberOfUnitsPassed);
		printf("Courses Passed: ");

		int hasPassedCourses = 0; 
		for (int i = 0; i < MAX_PASSED_COURSES; i++) {
			if (tempStudent->CoursesPassed[i][0] != '\0') {
				hasPassedCourses = 1; 
				Course* tempCourse = courseHead;
				int found = 0;
				while (tempCourse != NULL) {
					if (strcmp(tempCourse->CourseID, tempStudent->CoursesPassed[i]) == 0) {
						printf("%s ", tempCourse->CourseName);
						found = 1;
						break;
					}
					tempCourse = tempCourse->next;
				}
				if (!found) {
					printf("[Course not found] ");
				}
			}
		}
		if (!hasPassedCourses) {
			printf("None");
		}

		printf("\n--------------------------\n");

		tempStudent = tempStudent->next; 
	}
}

void ListCourses(Course* head) {
	if (head == NULL) {
		printf("\nNo courses found!\n");
		return;
	}

	printf("\n---- List of Courses ----\n");
	Course* temp = head;
	while (temp != NULL) {
		printf("Course ID: %s\n", temp->CourseID);
		printf("Course Name: %s\n", temp->CourseName);
		printf("Units: %d\n", temp->Units);
		printf("Professor: %s\n", temp->Professor);
		printf("Days: %s\n", temp->Days);
		printf("Start Time: %s\n", temp->StartTime);
		printf("End Time: %s\n", temp->EndTime);
		printf("Department: %s\n", temp->Department);
		printf("Minimum Units Required: %d\n", temp->MinUnitsRequired);
		printf("Prerequisites: ");
		if (temp->PrerequisiteCount == 0) {
			printf("None\n");
		}
		else {
			for (int i = 0; i < temp->PrerequisiteCount; i++) {
				printf("%s ", temp->Prerequisites[i]);
			}
			printf("\n");
		}
		printf("Corequisite: %s\n", temp->Corequisite[0] != '\0' ? temp->Corequisite : "None");
		printf("--------------------------\n");

		temp = temp->next;
	}
}

int getValidChoice(int min, int max) {
	int choice;
	while (1) {

		if (scanf("%d", &choice) != 1 || choice < min || choice > max) {
			printf("Invalid input! Please enter a number between %d and %d.\n", min, max);
			while (getchar() != '\n'); // clear the buffer
		}
		else {
			return choice;
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

		choice = getValidChoice(1, 4);

		switch (choice) {
		case 1:
			ListCourses(courseHead);
			break;
		case 2:
			ListStudents(studentHead, courseHead);
			break;
		case 3:
			ViewAllSchedules();
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
		printf("\n3. Back to Golestan Menu\n");
		printf("Enter choice: ");
		choice = getValidChoice(1, 3);

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
			while (getchar() != '\n'); 
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




void ShowEnrolledCourses(Student* student) {
	printf("\n---- Enrolled Courses ----\n");
	int courseCount = 0;
	for (int i = 0; i < MAX_ENROLLED_COURSES; i++) {
		if (student->EnrolledCourses[i][0] != '\0') {
			printf("📌 %s\n", student->EnrolledCourses[i]);
			courseCount++;
		}
	}
	if (courseCount == 0) {
		printf("❌ No courses enrolled.\n");
	}
}

void printCourseData(Course* head) {
	Course* tempCourse = head;

	while (tempCourse != NULL) {
		printf("\n--- Course Data ---\n");
		printf("CourseID: %s\n", tempCourse->CourseID);
		printf("CourseName: %s\n", tempCourse->CourseName);
		printf("StartTime: %s\n", tempCourse->StartTime);
		printf("EndTime: %s\n", tempCourse->EndTime);
		printf("Days: %s\n", tempCourse->Days);

		if (tempCourse->PrerequisiteCount > 0) {
			printf("Prerequisites: ");
			for (int i = 0; i < tempCourse->PrerequisiteCount; i++) {
				printf("%s ", tempCourse->Prerequisites[i]);
			}
			printf("\n");
		}
		else {
			printf("No prerequisites for this course.\n");
		}

		tempCourse = tempCourse->next;
	}
}

void EnrollInCourses(Student* student, Course* courseHead) {
	if (student == NULL || courseHead == NULL) {
		printf("❌ Error: Student or Course list is not initialized!\n");
		return;
	}

	// چاپ داده‌های دوره‌ها
	printCourseData(courseHead);

	char courseID[20];
	int continueEnroll = 1;

	while (continueEnroll) {
		Course* tempCourse = courseHead;
		printf("\n---- List of Available Courses ----\n");

		int foundCourse = 0;
		while (tempCourse != NULL) {
			// بررسی اینکه داده‌ها به درستی مقداردهی شده‌اند
			if (isValidCourseData(tempCourse)) {
				printf("📌 Course ID: %s, Name: %s, Time: %s - %s, Days: %s\n",
					tempCourse->CourseID, tempCourse->CourseName, tempCourse->StartTime, tempCourse->EndTime, tempCourse->Days);
				foundCourse = 1;
			}
			else {
				printf("⚠ Warning: Invalid course data! Skipping...\n");
			}
			tempCourse = tempCourse->next;
		}

		if (!foundCourse) {
			printf("❌ No courses available.\n");
			return;
		}

		// ورودی شناسه دوره
		printf("\nEnter Course ID to enroll: ");
		fgets(courseID, sizeof(courseID), stdin);
		courseID[strcspn(courseID, "\n")] = '\0'; // حذف '\n' اضافی

		tempCourse = courseHead;
		int courseFound = 0;
		while (tempCourse != NULL) {
			if (strcmp(tempCourse->CourseID, courseID) == 0) {
				courseFound = 1;

				// بررسی تعداد دروس ثبت‌نام شده
				int enrolledCount = 0;
				for (int i = 0; i < MAX_ENROLLED_COURSES; i++) {
					if (student->EnrolledCourses[i][0] != '\0') {
						enrolledCount++;
					}
				}
				if (enrolledCount >= MAX_ENROLLED_COURSES) {
					printf("❌ Error: Maximum number of enrolled courses (%d) reached.\n", MAX_ENROLLED_COURSES);
					return;
				}

				// بررسی پیش‌نیازهای















void SaveToFileAfterChange(Student* student) {
	if (student == NULL) {
		printf("Error: Invalid student data!\n");
		return;
	}
	FILE* file = fopen("Students.dat", "rb+");
	if (file == NULL) {
		printf("Error opening file!\n");
		return;
	}

	Student temp;
	int found = 0;

	while (fread(&temp, sizeof(Student), 1, file)) {
		if (strcmp(temp.ID, student->ID) == 0) {  
			found = 1;
			break;
		}
	}

	if (!found) {
		printf("Student not found in file!\n");
		fclose(file);
		return;
	}

	if (fseek(file, -(long)sizeof(Student), SEEK_CUR) != 0) {
		printf("Error seeking file position!\n");
		fclose(file);
		return;
	}

	if (fwrite(student, sizeof(Student), 1, file) != 1) {
		printf("Error writing data to file!\n");
	}
	else {
		fflush(file); 
		printf("Changes saved successfully!\n");
	}

	fclose(file);
}

void ChangePassword(Student* student) {
	char newPassword[20], confirmPassword[20];
	char hashedPassword[32];

	printf("\n---- Change Password ----\n");
	printf("Enter new password: ");
	scanf("%s", newPassword);
	printf("Confirm new password: ");
	scanf("%s", confirmPassword);

	if (strcmp(newPassword, confirmPassword) == 0) {
		simpleHash(newPassword, hashedPassword);
		strcpy(student->password, hashedPassword); 

		printf("Password changed successfully!\n");

		SaveToFileAfterChange(student); 
	}
	else {
		printf("Passwords do not match!\n");
	}
}

void ChangeUsername(Student* student) {
	char newUsername[20];
	printf("\n---- Change Username ----\n");
	printf("Enter new username: ");
	scanf("%s", newUsername);
	strcpy(student->UserName, newUsername);
	printf("Username changed successfully!\n");
	SaveToFileAfterChange(student);
}

void ViewCourses(Student* student, Course* courseHead) {
	if (student == NULL || courseHead == NULL) {
		printf("Error: Student or Course list is not initialized!\n");
		return;
	}

	printf("\n---- Passed Courses ----\n");
	int passedCoursesFound = 0; 
	for (int i = 0; i < MAX_PASSED_COURSES; i++) {
		if (student->CoursesPassed[i][0] != '\0') {
			Course* tempCourse = courseHead;
			while (tempCourse != NULL) {
				if (strcmp(tempCourse->CourseID, student->CoursesPassed[i]) == 0) {
					printf("Course ID: %s, Course Name: %s\n", tempCourse->CourseID, tempCourse->CourseName);
					passedCoursesFound = 1;
					break; 
				}
				tempCourse = tempCourse->next;
			}
		}
	}
	if (!passedCoursesFound) {
		printf("No passed courses found.\n");
	}

	printf("\n---- Enrolled Courses ----\n");
	int enrolledCoursesFound = 0; 
	for (int i = 0; i < MAX_ENROLLED_COURSES; i++) {
		if (student->EnrolledCourses[i][0] != '\0') {
			Course* tempCourse = courseHead;
			while (tempCourse != NULL) {
				if (strcmp(tempCourse->CourseID, student->EnrolledCourses[i]) == 0) {
					printf("Course ID: %s, Course Name: %s\n", tempCourse->CourseID, tempCourse->CourseName);
					enrolledCoursesFound = 1;
					break; 
				}
				tempCourse = tempCourse->next;
			}
		}
	}
	if (!enrolledCoursesFound) {
		printf("No enrolled courses found.\n");
	}
}

void RequestNewProgram(Student* student, Course* courseHead) {
	printf("\n---- Request New Program ----\n");

	int firstThreeDigits = atoi(student->ID) / 100000;  
	int year = 1000 + firstThreeDigits;  

	printf("Your admission year is: %d\n", year);

	char* term = NULL;
	if (firstThreeDigits == 403) {
		term = "Term 2";
	}
	else if (firstThreeDigits == 402) {
		term = "Term 4";
	}
	else if (firstThreeDigits == 401) {
		term = "Term 6";
	}
	else if (firstThreeDigits == 400) {
		term = "Term 8";
	}
	printf("Your current term: %s\n", term);

	printf("Requested Program: \n");
	Course* current = courseHead;
	while (current != NULL) {
		printf("Course: %s\n", current->CourseName);
		current = current->next;
	}
}

void PrioritizeCourses(Student* student, Course* courseHead) {
	printf("\n---- Prioritize Your Courses ----\n");

	int enrolledCount = 0;
	for (int i = 0; i < MAX_ENROLLED_COURSES; i++) {
		if (student->EnrolledCourses[i][0] != '\0') {
			enrolledCount++;
		}
	}

	int priority[MAX_ENROLLED_COURSES] = { 0 };
	bool usedPriority[MAX_ENROLLED_COURSES + 1] = { false };

	printf("Enter the priority for each course (1 to %d):\n", enrolledCount);
	for (int i = 0; i < enrolledCount; i++) {
		int input;
		printf("Priority for course %d: ", i + 1);
		if (scanf("%d", &input) != 1 || input < 1 || input > enrolledCount || usedPriority[input]) {
			printf("Error: Invalid or duplicate priority value.\n");
			while (getchar() != '\n');  // Clear input buffer
			i--;
			continue;
		}
		priority[i] = input;
		usedPriority[input] = true;
	}

	for (int i = 0; i < enrolledCount; i++) {
		int courseIndex = priority[i] - 1;
		printf("Course %s is prioritized as %d\n", student->EnrolledCourses[courseIndex], priority[i]);
	}

	printf("Courses prioritized successfully!\n");
}

void GenerateSchedules(Student* student, Course* courseHead, char schedules[MAX_SCHEDULES][MAX_ENROLLED_COURSES][20], char scheduleNames[MAX_SCHEDULES][50], int maxSchedules) {
	printf("\n---- Generate Weekly Schedules ----\n");

	int uniqueCourseCount = 0;
	char uniqueCourses[MAX_ENROLLED_COURSES][20] = { 0 };

	for (int i = 0; i < MAX_ENROLLED_COURSES; i++) {
		if (student->EnrolledCourses[i][0] != '\0') {
			int isUnique = 1;
			char currentID[4];
			strncpy(currentID, student->EnrolledCourses[i], 3);
			currentID[3] = '\0';

			for (int j = 0; j < uniqueCourseCount; j++) {
				if (strncmp(uniqueCourses[j], currentID, 3) == 0) {
					isUnique = 0;
					break;
				}
			}

			if (isUnique) {
				strcpy(uniqueCourses[uniqueCourseCount], student->EnrolledCourses[i]);
				uniqueCourseCount++;
			}
		}
	}

	if (maxSchedules > uniqueCourseCount) {
		printf("Warning: Cannot generate %d schedules! Only %d unique courses available.\n", maxSchedules, uniqueCourseCount);
		maxSchedules = uniqueCourseCount;
	}

	for (int i = 0; i < maxSchedules; i++) {
		printf("Enter name for schedule %d (or press Enter for default name): ", i + 1);
		fgets(scheduleNames[i], sizeof(scheduleNames[i]), stdin);
		if (scheduleNames[i][0] == '\n') {
			snprintf(scheduleNames[i], sizeof(scheduleNames[i]), "Schedule_%d", i + 1);
		}

		for (int j = 0; j < MAX_ENROLLED_COURSES; j++) {
			if (uniqueCourses[j][0] != '\0') {
				strcpy(schedules[i][j], uniqueCourses[j]);
			}
		}

		if (scheduleCount < MAX_SCHEDULES) {
			strcpy(allSchedules[scheduleCount].studentName, student->UserName);
			strcpy(allSchedules[scheduleCount].scheduleName, scheduleNames[i]);

			for (int j = 0; j < MAX_ENROLLED_COURSES; j++) {
				strcpy(allSchedules[scheduleCount].schedule[j], schedules[i][j]);
			}

			scheduleCount++;
		}
		else {
			printf("Error: Maximum number of schedules reached!\n");
		}
	}

	printf("Generated %d schedules successfully!\n", maxSchedules);
}

void PrintSchedules(Student* student, Course* courseHead, char schedules[MAX_SCHEDULES][MAX_ENROLLED_COURSES][20], char scheduleNames[MAX_SCHEDULES][50], int maxSchedules) {
	if (student == NULL || courseHead == NULL) {
		printf("Error: Student or Course list is not initialized!\n");
		return;
	}

	printf("debug: %d", maxSchedules);
	if (maxSchedules > 5) {
		maxSchedules = 5;
	}
	printf("\n---- Weekly Schedules ----\n");

	const char* daysOfWeek[6] = { "Saturday", "Sunday", "Monday", "Tuesday", "Wednesday", "Thursday" };

	// Iterate over each schedule
	for (int i = 0; i < maxSchedules; i++) {
		printf("\nSchedule: %s\n", scheduleNames[i]);
		printf("----------------------------------------------------------------------------------------------------\n");
		printf("Time         | Saturday     | Sunday       | Monday       | Tuesday      | Wednesday    | Thursday     \n");
		printf("----------------------------------------------------------------------------------------------------\n");

		// Check each hour in the schedule
		for (int hour = 8; hour <= 17; hour++) {
			printf("%02d:00-%02d:00 | ", hour, hour + 1);

			// Check each day for the schedule
			for (int day = 0; day < 6; day++) {
				int found = 0;

				// Go through the enrolled courses for this schedule
				for (int j = 0; j < MAX_ENROLLED_COURSES; j++) {
					if (schedules[i][j][0] != '\0') {
						Course* tempCourse = courseHead;

						// Check if the course matches and its time/day corresponds to the current slot
						while (tempCourse != NULL) {
							if (strcmp(tempCourse->CourseID, schedules[i][j]) == 0) {
								if (strstr(tempCourse->Days, daysOfWeek[day]) != NULL && atoi(tempCourse->StartTime) == hour) {
									printf("%-12s | ", tempCourse->CourseName);
									found = 1;
									break;
								}
							}
							tempCourse = tempCourse->next;
						}
					}
				}

				// If no course was found for this time slot, print a blank space
				if (!found) {
					printf("               | ");
				}
			}

			printf("\n");
		}

		printf("-------------------------------------------------------------------------------------------------------\n");
	}
}

void CheckDuplicateCourses(Student* student) {
	printf("\n---- Check Duplicate Courses ----\n");
	bool found = false;

	for (int i = 0; i < MAX_ENROLLED_COURSES - 1; i++) {
		for (int j = i + 1; j < MAX_ENROLLED_COURSES; j++) {
			if (student->EnrolledCourses[i][0] != '\0' &&
				strcmp(student->EnrolledCourses[i], student->EnrolledCourses[j]) == 0) {
				printf("Duplicate course found: %s\n", student->EnrolledCourses[i]);
				found = true;
			}
		}
	}

	if (!found) {
		printf("No duplicate courses found.\n");
	}
}

void ViewAllSchedules() {
	printf("\n---- All Saved Schedules ----\n");

	if (scheduleCount == 0) {
		printf("No schedules found!\n");
		return;
	}

	for (int i = 0; i < scheduleCount; i++) {
		printf("Student: %s, Schedule Name: %s\n", allSchedules[i].studentName, allSchedules[i].scheduleName);
		printf("Courses:\n");
		for (int j = 0; j < MAX_ENROLLED_COURSES; j++) {
			if (allSchedules[i].schedule[j][0] != '\0') {
				printf("- %s\n", allSchedules[i].schedule[j]);
			}
		}
		printf("----------------------------\n");
	}
}

void StudentMenu(Student* student, Course* courseHead) {
	int choice;
	int maxSchedules;
	char input[10];

	printf("\nEnter the maximum number of schedules you want to generate: ");
	fgets(input, sizeof(input), stdin);
	if (sscanf(input, "%d", &maxSchedules) != 1 || maxSchedules <= 0 || maxSchedules > MAX_SCHEDULES) {
		printf("Invalid number! Defaulting to %d schedules.\n", MAX_SCHEDULES);
		maxSchedules = MAX_SCHEDULES;
	}
	char (*schedules)[MAX_ENROLLED_COURSES][20] = malloc(maxSchedules * sizeof(*schedules));
	char (*scheduleNames)[50] = malloc(maxSchedules * sizeof(*scheduleNames));

	if (schedules == NULL || scheduleNames == NULL) {
		printf("Error: Memory allocation failed!\n");
		return;
	}

	memset(schedules, 0, maxSchedules * sizeof(*schedules));
	memset(scheduleNames, 0, maxSchedules * sizeof(*scheduleNames));

	do {
		printf("\n---- Student Menu ----\n");
		printf("1. Enroll in Courses\n");
		printf("2. Change Password\n");
		printf("3. Change Username\n");
		printf("4. View Courses\n");
		printf("5. Request New Program\n");
		printf("6. Prioritize Courses\n");
		printf("7. Generate Schedules\n");
		printf("8. Print Schedules\n");
		printf("9. Check Duplicate Courses\n");
		printf("0. Exit\n");
		printf("Enter your choice: ");

		fgets(input, sizeof(input), stdin);
		if (sscanf(input, "%d", &choice) != 1) {
			printf("Invalid input! Please enter a number.\n");
			continue;
		}

		switch (choice) {
		case 1:
			EnrollInCourses(student, courseHead);
			break;
		case 2:
			ChangePassword(student);
			break;
		case 3:
			ChangeUsername(student);
			break;
		case 4:
			ViewCourses(student, courseHead);
			break;
		case 5:
			RequestNewProgram(student, courseHead);
			break;
		case 6:
			PrioritizeCourses(student, courseHead);
			break;
		case 7:
			GenerateSchedules(student, courseHead, schedules, scheduleNames, maxSchedules);
			break;
		case 8:
			PrintSchedules(student, courseHead, schedules, scheduleNames, maxSchedules);
			break;
		case 9:
			CheckDuplicateCourses(student);
			break;
		case 0:
			printf("Exiting Student Menu...\n");
			break;
		default:
			printf("Invalid choice! Please try again.\n");
		}
	} while (choice != 0);

	free(schedules);
	free(scheduleNames);
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


/*
void ShowCoursesForPriority(Course* head, Student* student) {
	printf("\n--- Your Selected Courses ---\n");
	int i = 1;
	for (int j = 0; j < MAX_ENROLLED_COURSES; j++) {
		if (strlen(student->EnrolledCourses[j]) > 0) {
			Course* temp = head;
			while (temp != NULL) {
				if (strcmp(temp->CourseID, student->EnrolledCourses[j]) == 0) {
					printf("%d. Course ID: %s, Name: %s\n", i, temp->CourseID, temp->CourseName);
					break;
				}
				temp = temp->next;
			}
			i++;
		}
	}
}


void PrioritizeCourses(Course* head, Student* student) {
	int priorities[MAX_ENROLLED_COURSES] = { 0 };
	int numCourses = 0;

	// نمایش دروس انتخاب‌شده و دریافت اولویت
	ShowCoursesForPriority(head, student);

	printf("\n--- Assign Priorities to Courses ---\n");
	for (int j = 0; j < MAX_ENROLLED_COURSES; j++) {
		if (strlen(student->EnrolledCourses[j]) > 0) {
			printf("Enter priority for course %s (1-%d): ", student->EnrolledCourses[j], MAX_ENROLLED_COURSES);
			int priority;
			scanf("%d", &priority);
			priorities[numCourses] = priority;
			numCourses++;
		}
	}

	// تایید یا تغییر اولویت‌ها
	int confirm;
	do {
		printf("\n--- Confirm Priorities ---\n");
		for (int i = 0; i < numCourses; i++) {
			printf("Course: %s, Priority: %d\n", student->EnrolledCourses[i], priorities[i]);
		}

		printf("\nDo you want to change the priorities?\n");
		printf("1. Yes\n");
		printf("2. No, confirm\n");
		printf("Enter your choice: ");
		scanf("%d", &confirm);

		if (confirm == 1) {
			// تغییر اولویت‌ها
			printf("\nRe-enter priorities:\n");
			for (int i = 0; i < numCourses; i++) {
				printf("Enter new priority for course %s: ", student->EnrolledCourses[i]);
				scanf("%d", &priorities[i]);
			}
		}
	} while (confirm != 2);

	// ذخیره اولویت‌ها در لیست
	for (int i = 0; i < numCourses; i++) {
		// جابجایی بر اساس اولویت
		// اینجا شما می‌توانید بر اساس اولویت‌ها دروس را جابجا کنید
		// به‌طور مثال، تغییر ترتیب دروس در student->EnrolledCourses با استفاده از اولویت‌ها
	}
}


void RearrangeCoursesByPriority(Student* student, int* priorities) {
	char tempCourse[20];
	int numCourses = 0;

	// تعداد دروس انتخاب‌شده
	for (int i = 0; i < MAX_ENROLLED_COURSES; i++) {
		if (strlen(student->EnrolledCourses[i]) > 0) {
			numCourses++;
		}
	}

	// جابجایی دروس بر اساس اولویت‌ها
	for (int i = 0; i < numCourses; i++) {
		for (int j = i + 1; j < numCourses; j++) {
			if (priorities[i] > priorities[j]) {
				// جابجایی اولویت‌ها
				int tempPriority = priorities[i];
				priorities[i] = priorities[j];
				priorities[j] = tempPriority;

				// جابجایی دروس در آرایه EnrolledCourses
				strcpy(tempCourse, student->EnrolledCourses[i]);
				strcpy(student->EnrolledCourses[i], student->EnrolledCourses[j]);
				strcpy(student->EnrolledCourses[j], tempCourse);
			}
		}
	}
}



void FinalizePrioritiesAndPlan(Student* student, Course* head) {
	int priorities[MAX_ENROLLED_COURSES];

	// اولویت‌بندی دروس
	PrioritizeCourses(head, student);

	// جابجایی دروس بر اساس اولویت‌ها
	RearrangeCoursesByPriority(student, priorities);

	// تایید نهایی
	printf("\n--- Final Confirmation ---\n");
	printf("Your courses have been prioritized and reordered.\n");

	// نمایش برنامه‌ی نهایی
	printf("\n--- Your Final Schedule ---\n");
	for (int i = 0; i < MAX_ENROLLED_COURSES; i++) {
		if (strlen(student->EnrolledCourses[i]) > 0) {
			printf("%d. %s\n", i + 1, student->EnrolledCourses[i]);
		}
	}
}
*/