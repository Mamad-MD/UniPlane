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
		while (getchar() != '\n'); // پاک کردن بافر ورودی
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
	for (int i = 0; i < course->PrerequisiteCount; i++) {
		fwrite(course->Prerequisites[i], sizeof(char), 10, file);
	}

	fclose(file);
}

void LoadCoursesFromFile(Course** head) {
	//  printf("Debug: Entering LoadCoursesFromFile\n");

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
		// printf("Debug: Reading a course from file\n");

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
	// printf("Debug: Exiting LoadCoursesFromFile\n");
}

void signup(Student** head, Course* courseHead) {
	Student* newStudent = (Student*)malloc(sizeof(Student));
	if (newStudent == NULL) {
		printf("Memory allocation failed!\n");
		return;
	}
	int valid = 0;

	printf("---- Student Registration ----\n\n");
	printf("Enter 0 at any time to go back.\n");

	printf("Please Enter Name: ");
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
		valid = 0;
		printf("Enter Email (must end with @gmail.com): ");
		if (scanf("%s", newStudent->Email) != 1 || strcmp(newStudent->Email, "0") == 0) {
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

	printf("Select Field of study: \n");

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
	//printf("\n---- List of Existing Courses ----\n");
	//Course* tempCourse = courseHead;
	//while (tempCourse != NULL) {
	//    printf("Course ID: %s, Course Name: %s\n", tempCourse->CourseID, tempCourse->CourseName);
	//    tempCourse = tempCourse->next;
	//}
	printf("\n---- List of Existing Courses ----\n");
	Course* tempCourse = courseHead;
	while (tempCourse != NULL) {

		char courseIDPrefix[4];
		strncpy(courseIDPrefix, tempCourse->CourseID, 3);
		courseIDPrefix[3] = '\0';


		printf("Course ID: %s, Course Name: %s\n", courseIDPrefix, tempCourse->CourseName);
		tempCourse = tempCourse->next;
	}
	//printf("Enter Number Of Passed Courses (Max %d): ", MAX_PASSED_COURSES);
	//int PassedCount;
	//if (scanf("%d", &PassedCount) != 1 || PassedCount < 0 || PassedCount > MAX_PASSED_COURSES) {
	//    PassedCount = MAX_PASSED_COURSES;
	//}

	//for (int i = 0; i < PassedCount; i++) {
	//    printf("Enter code for course %d: ", i + 1);
	//    if (scanf("%s", newStudent->CoursesPassed[i]) != 1) {
	//        free(newStudent);
	//        return;
	//    }
	//}

	printf("Enter Number Of Passed Courses (Max %d): ", MAX_PASSED_COURSES);
	int PassedCount;
	if (scanf("%d", &PassedCount) != 1 || PassedCount < 0 || PassedCount > MAX_PASSED_COURSES) {
		PassedCount = MAX_PASSED_COURSES;
	}

	for (int i = 0; i < PassedCount; i++) {
		char courseIDPrefix[4];
		printf("Enter code for course %d (3 digits): ", i + 1);
		if (scanf("%3s", courseIDPrefix) != 1) {
			printf("Invalid input! Please enter 3 digits.\n");
			i--;
			continue;
		}


		Course* tempCourse = courseHead;
		int found = 0;
		while (tempCourse != NULL) {
			char tempCourseIDPrefix[4];
			strncpy(tempCourseIDPrefix, tempCourse->CourseID, 3);
			tempCourseIDPrefix[3] = '\0';

			if (strcmp(tempCourseIDPrefix, courseIDPrefix) == 0) {
				printf("Found Course: %s (%s)\n", tempCourse->CourseName, tempCourse->CourseID);
				strcpy(newStudent->CoursesPassed[i], tempCourse->CourseID);
				found = 1;
				break;
			}
			tempCourse = tempCourse->next;
		}

		if (!found) {
			printf("No course found with ID prefix %s! Please try again.\n", courseIDPrefix);
			i--;
		}
	}

	char password[20], confirmPassword[20];
	do {
		valid = 0;
		printf("Enter Password (minimum 6 characters): ");
		if (scanf("%s", password) != 1) {
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
		if (scanf("%s", confirmPassword) != 1) {
			printf("Invalid input! Please try again.\n");
			while (getchar() != '\n'); // پاک کردن بافر ورودی
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
	printf("\nWelcome Back! Please log in.\n\n");
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

void GenerateCourseID(char* courseID, const char* baseID) {
	int index = atoi(baseID);
	if (index < 100 || index >= MAX_COURSES) {
		printf("Invalid Course ID! Base ID must be between 100 and %d.\n", MAX_COURSES - 1);
		return;
	}

	CourseIDCounter[index]++;

	if (CourseIDCounter[index] == 1) {
		sprintf(courseID, "%s-01", baseID);
	}
	else {
		sprintf(courseID, "%s-%02d", baseID, CourseIDCounter[index]);
	}
}

void AddCourse(Course** head) {
	Course* newCourse = (Course*)malloc(sizeof(Course));
	if (newCourse == NULL) {
		printf("Memory allocation failed!\n");
		return;
	}
	printf("\n--- Add New Course ---\n");

	printf("Enter Course Name: ");
	getchar();
	fgets(newCourse->CourseName, sizeof(newCourse->CourseName), stdin);
	newCourse->CourseName[strcspn(newCourse->CourseName, "\n")] = '\0';

	memset(newCourse->CourseID, 0, sizeof(newCourse->CourseID));
	char baseID[10];
	printf("Enter Base Course ID (e.g., 101): ");
	scanf("%9s", baseID);
	for (int i = 0; i < strlen(baseID); i++) {
		if (!isdigit(baseID[i])) {
			printf("Invalid Base ID! Only digits are allowed.\n");
			return;
		}
	}
	int baseIDValue = atoi(baseID);
	if (baseIDValue < 100) {
		printf("Invalid Base ID! Base ID must be 100 or greater.\n");
		return;
	}

	GenerateCourseID(newCourse->CourseID, baseID);

	newCourse->CourseID[sizeof(newCourse->CourseID) - 1] = '\0';
	printf("Auto-generated Course ID: %s\n", newCourse->CourseID);

	printf("Enter Number of Units for this Course: ");
	if (scanf("%d", &newCourse->Units) != 1 || newCourse->Units < 1) {
		printf("Invalid number of units! Please enter a positive integer.\n");
		free(newCourse);
		return;
	}

	printf("Enter Time (e.g., 8:00 10:00 Saturday-Monday): ");
	char timeInput[100];
	getchar();  // برای حذف کاراکتر جدید خط قبلی
	fgets(timeInput, sizeof(timeInput), stdin);
	timeInput[strcspn(timeInput, "\n")] = '\0';


	sscanf(timeInput, "%s %s %[^\n]", newCourse->StartTime, newCourse->EndTime, newCourse->Days);

	printf("Enter Professor: ");
	fgets(newCourse->Professor, sizeof(newCourse->Professor), stdin);
	newCourse->Professor[strcspn(newCourse->Professor, "\n")] = '\0';

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

	printf("Enter Minimum Units Required to Register for this Course: ");
	if (scanf("%d", &newCourse->MinUnitsRequired) != 1 || newCourse->MinUnitsRequired < 0) {
		printf("Invalid minimum units! Please enter a non-negative integer.\n");
		free(newCourse);
		return;
	}

	printf("\n---- List of Existing Courses ----\n");
	Course* temp = *head;
	while (temp != NULL) {
		char courseIDPrefix[4];
		strncpy(courseIDPrefix, temp->CourseID, 3);
		courseIDPrefix[3] = '\0';
		printf("Course ID: %s, Course Name: %s\n", courseIDPrefix, temp->CourseName);
		temp = temp->next;
	}


	char prerequisiteInput[100];
	printf("\nEnter Prerequisite Course IDs (3 digits each, separated by spaces, or 0 if none): ");
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
				char tempCourseIDPrefix[14];
				strncpy(tempCourseIDPrefix, temp->CourseID, 3);
				tempCourseIDPrefix[3] = '\0';

				if (strcmp(tempCourseIDPrefix, token) == 0) {
					strcpy(newCourse->Prerequisites[count], temp->CourseName);
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


	char corequisiteID[14];
	printf("Enter Corequisite Course ID (3 digits, or 0 if none): ");
	scanf("%3s", corequisiteID);

	if (strcmp(corequisiteID, "0") == 0) {
		newCourse->Corequisite[0] = '\0';
	}
	else {

		Course* temp = *head;
		int found = 0;
		while (temp != NULL) {

			char tempCourseIDPrefix[14];
			strncpy(tempCourseIDPrefix, temp->CourseID, 3);
			tempCourseIDPrefix[3] = '\0';

			printf("Debug: Comparing %s with %s\n", tempCourseIDPrefix, corequisiteID);
			if (strcmp(tempCourseIDPrefix, corequisiteID) == 0) {
				strcpy(newCourse->Corequisite, temp->CourseName);
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
			printf("\n---- List of Existing Courses ----\n");
			Course* temp = courseHead;
			while (temp != NULL) {
				char courseIDPrefix[4];
				strncpy(courseIDPrefix, temp->CourseID, 3);
				courseIDPrefix[3] = '\0';

				printf("Course ID: %s, Course Name: %s\n", courseIDPrefix, temp->CourseName);
				temp = temp->next;
			}
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
	if (course->PrerequisiteCount >= MAX_PREREQUISITES) {
		printf("Maximum number of prerequisites reached!\n");
		return;
	}

	char prerequisiteID[14];
	printf("Enter Prerequisite Course ID (3 digits): ");
	scanf("%3s", prerequisiteID);


	Course* temp = course;
	int found = 0;
	while (temp != NULL) {

		char tempCourseIDPrefix[14];
		strncpy(tempCourseIDPrefix, temp->CourseID, 3);
		tempCourseIDPrefix[3] = '\0';

		if (strcmp(tempCourseIDPrefix, prerequisiteID) == 0) {
			strcpy(course->Prerequisites[course->PrerequisiteCount], temp->CourseName);
			course->PrerequisiteCount++;
			found = 1;
			break;
		}
		temp = temp->next;
	}
	if (!found) {
		printf("Invalid Course ID! Prerequisite not set.\n");
	}
}

void ChangePrerequisite(Course* course) {
	if (course->PrerequisiteCount == 0) {
		printf("This course has no prerequisites to change!\n");
		return;
	}

	char oldPrerequisiteID[14], newPrerequisiteID[14];
	printf("Enter Old Prerequisite Course ID (3 digits): ");
	scanf("%3s", oldPrerequisiteID);


	int index = -1;
	for (int i = 0; i < course->PrerequisiteCount; i++) {
		char tempCourseIDPrefix[4];
		strncpy(tempCourseIDPrefix, course->Prerequisites[i], 3);
		tempCourseIDPrefix[3] = '\0';

		if (strcmp(tempCourseIDPrefix, oldPrerequisiteID) == 0) {
			index = i;
			break;
		}
	}

	if (index == -1) {
		printf("Old Prerequisite Course ID not found!\n");
		return;
	}

	printf("Enter New Prerequisite Course ID (3 digits): ");
	scanf("%3s", newPrerequisiteID);

	Course* temp = course;
	int found = 0;
	while (temp != NULL) {

		char tempCourseIDPrefix[4];
		strncpy(tempCourseIDPrefix, temp->CourseID, 3);
		tempCourseIDPrefix[3] = '\0';

		if (strcmp(tempCourseIDPrefix, newPrerequisiteID) == 0) {
			strcpy(course->Prerequisites[index], temp->CourseName);
			found = 1;
			break;
		}
		temp = temp->next;
	}
	if (!found) {
		printf("Invalid Course ID! Prerequisite not changed.\n");
	}
}

void RemovePrerequisite(Course* course) {
	if (course->PrerequisiteCount == 0) {
		printf("This course has no prerequisites to remove!\n");
		return;
	}

	char prerequisiteID[14];
	printf("Enter Prerequisite Course ID to remove (3 digits): ");
	scanf("%3s", prerequisiteID);


	int index = -1;
	for (int i = 0; i < course->PrerequisiteCount; i++) {
		char tempCourseIDPrefix[4];
		strncpy(tempCourseIDPrefix, course->Prerequisites[i], 3);
		tempCourseIDPrefix[3] = '\0';

		if (strcmp(tempCourseIDPrefix, prerequisiteID) == 0) {
			index = i;
			break;
		}
	}

	if (index == -1) {
		printf("Prerequisite Course ID not found!\n");
		return;
	}

	// حذف پیش‌نیاز
	for (int i = index; i < course->PrerequisiteCount - 1; i++) {
		strcpy(course->Prerequisites[i], course->Prerequisites[i + 1]);
	}
	course->PrerequisiteCount--;
	printf("Prerequisite removed successfully!\n");
}

void ViewPrerequisites(Course* course) {
	printf("\nPrerequisites for Course %s:\n", course->CourseName);
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
		printf("No students found!\n");
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
		for (int i = 0; i < MAX_PASSED_COURSES; i++) {
			if (tempStudent->CoursesPassed[i][0] != '\0') {

				Course* tempCourse = courseHead;
				while (tempCourse != NULL) {
					if (strcmp(tempCourse->CourseID, tempStudent->CoursesPassed[i]) == 0) {
						printf("%s ", tempCourse->CourseName);
						break;
					}
					tempCourse = tempCourse->next;
				}
			}
		}
		printf("\n--------------------------\n");
		tempStudent = tempStudent->next;
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
			ListStudents(studentHead, courseHead);
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

void StudentMenu() {
	printf("\n---- Student Menu ----\n");
	printf("This page has not been created yet.\n");
	printf("Press any key to return to the main menu...\n");
	getchar(); // منتظر فشار دادن یک کلید توسط کاربر
	getchar(); // برای جلوگیری از مشکل بافر ورودی
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

