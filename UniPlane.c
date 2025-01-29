#pragma warning(disable : 4996)
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <Windows.h>
//#include <openssl/sha.h>
#include "UniPlaneHeader.h"


/*void hashPassword(const char* password, char* output) {
	unsigned char hash[SHA256_DIGEST_LENGTH];
	SHA256_CTX sha256;
	SHA256_Init(&sha256);
	SHA256_Update(&sha256, password, strlen(password));
	SHA256_Final(hash, &sha256);

	for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
		sprintf(output + (i * 2), "%02x", hash[i]);
	}
	output[SHA256_DIGEST_LENGTH * 2] = '\0';
}*/

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
	if (fwrite(student, sizeof(Student), 1, file) != 1) {
		printf("Error writing to file!\n");
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

/*//////   void hashPassword(const char* password, char* output) {
	unsigned char hash[SHA256_DIGEST_LENGTH];
	SHA256_CTX sha256;
	SHA256_Init(&sha256);
	SHA256_Update(&sha256, password, strlen(password));
	SHA256_Final(hash, &sha256);

	for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
		sprintf(output + (i * 2), "%02x", hash[i]);
	}
	output[SHA256_DIGEST_LENGTH * 2] = '\0';
} /////////*/

// تابع هش پیچیده‌تر
void simpleHash(char* input, char* output) {
	unsigned long hash = 5381;
	int c;

	while ((c = *input++)) {
		hash = ((hash << 5) + hash) + c;  // hash * 33 + c
	}

	sprintf(output, "%lx", hash);  // تبدیل هش به رشته
}

// تبدیل هش به رشته
void hashToString(char* hashed, char* output) {
	sprintf(output, "%s", hashed);
}

// مقایسه هش‌ها
int compareHashes(char* original, char* entered) {
	return strcmp(original, entered) == 0;
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
			// هش کردن پسورد
			char hashedPassword[20];
			simpleHash(password, hashedPassword);  // هش کردن پسورد
			strcpy(newStudent->password, hashedPassword);  // ذخیره هش در سیستم
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
	if (scanf("%s", username) != 1 || strcmp(username, "0") == 0) {
		return 0;
	}

	printf("Password: ");
	if (scanf("%s", password) != 1 || strcmp(password, "0") == 0) {
		return 0;
	}

	Student* temp = head;
	char hashedPassword[20];
	simpleHash(password, hashedPassword);  // هش کردن پسورد وارد شده
	char finalHash[20];
	hashToString(hashedPassword, finalHash); // تبدیل به رشته قابل نمایش

	while (temp != NULL) {
		if (strcmp(temp->UserName, username) == 0 && compareHashes(temp->password, finalHash)) {
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
	static int counter = 1; 
	sprintf(courseID, "%s-%02d", baseID, counter);
	counter++;
}

void AddCourse(Course** head) {
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

	/*memset(newCourse->CourseID, 0, sizeof(newCourse->CourseID));
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
	printf("Auto-generated Course ID: %s\n", newCourse->CourseID);*/

	char baseID[10];
	printf("Enter Base Course ID (e.g., 101): ");
	scanf("%9s", baseID);
	GenerateCourseID(newCourse->CourseID, baseID);
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
		char courseIDPrefix[14];
		strncpy(courseIDPrefix, temp->CourseID, 3);
		courseIDPrefix[3] = '\0';
		printf("Course ID: %s, Course Name: %s\n", courseIDPrefix, temp->CourseName);
		temp = temp->next;
	}

		// بررسی پیش‌نیازها
		//char prerequisiteInput[100];
		//printf("\nEnter Prerequisite Course IDs (3 digits each, separated by spaces, or 0 if none): ");
		//getchar();
		//fgets(prerequisiteInput, sizeof(prerequisiteInput), stdin);
		//prerequisiteInput[strcspn(prerequisiteInput, "\n")] = '\0';

		//if (strcmp(prerequisiteInput, "0") != 0) {
		//	char* token = strtok(prerequisiteInput, " ");
		//	int count = 0;

		//	while (token != NULL && count < MAX_PREREQUISITES) {
		//		if (IsCourseExists(*head, token)) {
		//			strcpy(newCourse->Prerequisites[count], token);
		//			count++;
		//		}
		//		else {
		//			printf("Invalid Course ID: %s! Prerequisite not set.\n", token);
		//		}
		//		token = strtok(NULL, " ");
		//	}

		//	newCourse->PrerequisiteCount = count;
		//}

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
	printf("Course added successfully!\n");
}

void ManagePrerequisites(Course* courseHead) {
	char courseID[20];
	printf("Enter Course ID to manage prerequisites: ");
	scanf("%s", courseID);

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
	scanf("%s", prerequisiteID);

	// Check if the prerequisite already exists
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
	printf("Enter Old Prerequisite Course ID (e.g., 101-01): ");
	scanf("%s", oldPrerequisiteID);

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

	printf("Enter New Prerequisite Course ID (e.g., 102-01): ");
	scanf("%s", newPrerequisiteID);

	Course* temp = course;
	int found = 0;
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
	scanf("%s", prerequisiteID);

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

void EnrollInCourses(Student* student, Course* courseHead) {
	printf("\n---- Enroll in Courses ----\n");
	printf("Select Department:\n");
	for (int i = 0; i < FieldOfStudyCount; i++) {
		printf("%d. %s\n", i + 1, FieldOfStudy[i]);
	}
	int departmentChoice;
	if (scanf("%d", &departmentChoice) != 1 || departmentChoice < 1 || departmentChoice > FieldOfStudyCount) {
		printf("Invalid input! Please enter a number between 1 and %d.\n", FieldOfStudyCount);
		while (getchar() != '\n'); // پاک‌سازی ورودی
		return;
	}

	Course* tempCourse = courseHead;
	while (tempCourse != NULL) {
		if (strcmp(tempCourse->Department, FieldOfStudy[departmentChoice - 1]) == 0) {
			printf("Course ID: %s, Course Name: %s\n", tempCourse->CourseID, tempCourse->CourseName);
		}
		tempCourse = tempCourse->next;
	}

	char courseID[20];
	printf("Enter Course ID to enroll: ");
	scanf("%s", courseID);

	tempCourse = courseHead;
	while (tempCourse != NULL) {
		if (strcmp(tempCourse->CourseID, courseID) == 0) {
			// بررسی تعداد دوره‌های ثبت‌شده
			int enrolledCount = 0;
			for (int i = 0; i < MAX_ENROLLED_COURSES; i++) {
				if (student->EnrolledCourses[i][0] != '\0') {
					enrolledCount++;
				}
			}
			if (enrolledCount >= MAX_ENROLLED_COURSES) {
				printf("Error: Maximum number of enrolled courses (%d) reached.\n", MAX_ENROLLED_COURSES);
				return;
			}

			// بررسی پیش‌نیازها
			int prerequisitesMet = 1;
			for (int i = 0; i < tempCourse->PrerequisiteCount; i++) {
				int found = 0;
				for (int j = 0; j < MAX_PASSED_COURSES; j++) {
					if (strcmp(student->CoursesPassed[j], tempCourse->Prerequisites[i]) == 0) {
						found = 1;
						break;
					}
				}
				if (!found) {
					prerequisitesMet = 0;
					break;
				}
			}
			if (!prerequisitesMet) {
				printf("You have not met the prerequisites for this course!\n");
				return;
			}

			// بررسی تداخل زمانی
			for (int i = 0; i < MAX_ENROLLED_COURSES; i++) {
				if (student->EnrolledCourses[i][0] != '\0') {
					Course* enrolledCourse = courseHead;
					while (enrolledCourse != NULL) {
						if (strcmp(enrolledCourse->CourseID, student->EnrolledCourses[i]) == 0) {
							if (strcmp(enrolledCourse->Days, tempCourse->Days) == 0 &&
								strcmp(enrolledCourse->StartTime, tempCourse->StartTime) == 0) {
								printf("Error: Time conflict detected between \"%s\" and \"%s\".\n", tempCourse->CourseName, enrolledCourse->CourseName);
								return;
							}
							break;
						}
						enrolledCourse = enrolledCourse->next;
					}
				}
			}

			// ثبت‌نام در دوره
			for (int i = 0; i < MAX_ENROLLED_COURSES; i++) {
				if (student->EnrolledCourses[i][0] == '\0') {
					if (strlen(tempCourse->CourseID) < 20) {
						strcpy(student->EnrolledCourses[i], tempCourse->CourseID);
					}
					else {
						printf("Error: Course ID is too long!\n");
						return;
					}

					printf("Enrolled in course %s successfully!\n", tempCourse->CourseName);
					return;
				}
			}
		}
		tempCourse = tempCourse->next;
	}
	printf("Course with ID %s not found!\n", courseID);
}

void ChangePassword(Student* student) {
	char newPassword[20], confirmPassword[20];
	printf("\n---- Change Password ----\n");
	printf("Enter new password: ");
	scanf("%s", newPassword);
	printf("Confirm new password: ");
	scanf("%s", confirmPassword);

	if (strcmp(newPassword, confirmPassword) == 0) {
		strcpy(student->password, newPassword);
		printf("Password changed successfully!\n");
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
}

void ViewCourses(Student* student, Course* courseHead) {
	printf("\n---- Passed Courses ----\n");
	for (int i = 0; i < MAX_PASSED_COURSES; i++) {
		if (student->CoursesPassed[i][0] != '\0') {
			Course* tempCourse = courseHead;
			while (tempCourse != NULL) {
				if (strcmp(tempCourse->CourseID, student->CoursesPassed[i]) == 0) {
					printf("Course ID: %s, Course Name: %s\n", tempCourse->CourseID, tempCourse->CourseName);
					break;
				}
				tempCourse = tempCourse->next;
			}
		}
	}

	printf("\n---- Enrolled Courses ----\n");
	for (int i = 0; i < MAX_ENROLLED_COURSES; i++) {
		if (student->EnrolledCourses[i][0] != '\0') {
			Course* tempCourse = courseHead;
			while (tempCourse != NULL) {
				if (strcmp(tempCourse->CourseID, student->EnrolledCourses[i]) == 0) {
					printf("Course ID: %s, Course Name: %s\n", tempCourse->CourseID, tempCourse->CourseName);
					break;
				}
				tempCourse = tempCourse->next;
			}
		}
	}
}

void RequestNewProgram(Student* student, Course* courseHead) {
	printf("\n---- Request New Program ----\n");

	// استخراج سه رقم اول شماره دانشجویی
	int firstThreeDigits = atoi(student->ID) / 100000;  // سه رقم اول شماره دانشجویی
	int year = 1300 + firstThreeDigits;  // سال شمسی با افزودن 1300

	printf("Your admission year is: %d\n", year);

	// استخراج ترم بر اساس شماره دانشجویی
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

	// نمایش رشته پیشنهادی برای برنامه
	printf("Suggested program based on your admission year: %s\n", FieldOfStudy[0]);
}

void PrioritizeCourses(Student* student, Course* courseHead) {
	printf("\n---- Prioritize Courses ----\n");
	for (int i = 0; i < MAX_ENROLLED_COURSES; i++) {
		if (student->EnrolledCourses[i][0] != '\0') {
			Course* tempCourse = courseHead;
			while (tempCourse != NULL) {
				if (strcmp(tempCourse->CourseID, student->EnrolledCourses[i]) == 0) {
					printf("%d. %s\n", i + 1, tempCourse->CourseName);
					break;
				}
				tempCourse = tempCourse->next;
			}
		}
	}

	int priority[MAX_ENROLLED_COURSES];
	printf("Enter the priority for each course (1 to %d):\n", MAX_ENROLLED_COURSES);
	for (int i = 0; i < MAX_ENROLLED_COURSES; i++) {
		if (student->EnrolledCourses[i][0] != '\0') {
			printf("Priority for course %d: ", i + 1);
			scanf("%d", &priority[i]);
			if (priority[i] < 1 || priority[i] > MAX_ENROLLED_COURSES) {
				printf("Error: Priority must be between 1 and %d.\n", MAX_ENROLLED_COURSES);
				i--; // بازگشت به ورودی قبلی
				continue;
			}
			for (int j = 0; j < i; j++) {
				if (priority[i] == priority[j]) {
					printf("Error: Duplicate priority value. Please choose a unique priority.\n");
					i--; // بازگشت به ورودی قبلی
					break;
				}
			}

		}
	}
	printf("Courses prioritized successfully!\n");
}

void GenerateSchedules(Student* student, Course* courseHead, char schedules[3][MAX_ENROLLED_COURSES][20], char scheduleNames[3][50]) {
	printf("\n---- Generate Weekly Schedules ----\n");
	int scheduleCount = 0;

	// Generate 3 schedules
	for (int i = 0; i < 3; i++) {
		printf("Enter name for schedule %d: ", i + 1);
		scanf("%s", scheduleNames[i]);

		// Copy enrolled courses to schedule based on priority
		for (int j = 0; j < MAX_ENROLLED_COURSES; j++) {
			if (student->EnrolledCourses[j][0] != '\0') {
				strcpy(schedules[i][j], student->EnrolledCourses[j]);
			}
		}
		scheduleCount++;
	}

	printf("Generated %d schedules successfully!\n", scheduleCount);
}	

// void PrintSchedules(Student* student, Course* courseHead, char schedules[3][MAX_ENROLLED_COURSES][20], char scheduleNames[3][50]) {
	/*printf("\n---- Weekly Schedules ----\n");
	for (int i = 0; i < 3; i++) {
		printf("\nSchedule: %s\n", scheduleNames[i]);
		printf("Time\t\tMonday\t\tTuesday\t\tWednesday\tThursday\tFriday\n");

		for (int hour = 8; hour <= 17; hour++) {
			printf("%02d:00 - %02d:00\t", hour, hour + 1);
			for (int day = 0; day < 5; day++) {
				char* dayName;
				switch (day) {
				case 0: dayName = "Monday"; break;
				case 1: dayName = "Tuesday"; break;
				case 2: dayName = "Wednesday"; break;
				case 3: dayName = "Thursday"; break;
				case 4: dayName = "Friday"; break;
				}

				int found = 0;
				for (int j = 0; j < MAX_ENROLLED_COURSES; j++) {
					if (schedules[i][j][0] != '\0') {
						Course* tempCourse = courseHead;
						while (tempCourse != NULL) {
							if (strcmp(tempCourse->CourseID, schedules[i][j]) == 0) {
								if (strstr(tempCourse->Days, dayName) != NULL && atoi(tempCourse->StartTime) == hour) {
									printf("%s\t", tempCourse->CourseName);
									found = 1;
									break;
								}
							}
							tempCourse = tempCourse->next;
						}
					}
				}
				if (!found) {
					printf("-\t\t");
				}
//			}*/
//			printf("\n");
//		}
//	}
//}
void PrintSchedules(Student* student, Course* courseHead, char schedules[3][MAX_ENROLLED_COURSES][20], char scheduleNames[3][50]) {
	printf("\n---- Weekly Schedules ----\n");

	// روزهای هفته از شنبه تا چهارشنبه
	const char* daysOfWeek[5] = { "Saturday", "Sunday", "Monday", "Tuesday", "Wednesday" };

	// برای هر برنامه هفتگی
	for (int i = 0; i < 3; i++) {
		printf("\nSchedule: %s\n", scheduleNames[i]);
		printf("---------------------------------------------------------------\n");
		printf("Time      | Saturday     | Sunday       | Monday       | Tuesday      | Wednesday    \n");
		printf("---------------------------------------------------------------\n");

		// برای هر ساعت از 8 تا 17
		for (int hour = 8; hour <= 17; hour++) {
			printf("%02d:00-%02d:00 | ", hour, hour + 1);

			// برای هر روز از هفته
			for (int day = 0; day < 5; day++) {
				int found = 0;

				// بررسی دروس ثبت‌نام‌شده در این زمان و روز
				for (int j = 0; j < MAX_ENROLLED_COURSES; j++) {
					if (schedules[i][j][0] != '\0') {
						Course* tempCourse = courseHead;

						// جستجو در لیست دروس
						while (tempCourse != NULL) {
							if (strcmp(tempCourse->CourseID, schedules[i][j]) == 0) {
								if (strstr(tempCourse->Days, daysOfWeek[day]) != NULL &&
									atoi(tempCourse->StartTime) == hour) {
									printf("%-12s | ", tempCourse->CourseName);
									found = 1;
									break;
								}
							}
							tempCourse = tempCourse->next;
						}
					}
				}

				// اگر هیچ درسی پیدا نشد، خالی نمایش بده
				if (!found) {
					printf("            | ");
				}
			}

			printf("\n");
		}

		printf("---------------------------------------------------------------\n");
	}
}

void CheckDuplicateCourses(Student* student) {
	printf("\n---- Check Duplicate Courses ----\n");
	for (int i = 0; i < MAX_ENROLLED_COURSES; i++) {
		if (student->EnrolledCourses[i][0] != '\0') {
			for (int j = i + 1; j < MAX_ENROLLED_COURSES; j++) {
				if (strcmp(student->EnrolledCourses[i], student->EnrolledCourses[j]) == 0) {
					printf("Duplicate course found: %s\n", student->EnrolledCourses[i]);
					return;
				}
			}
		}
	}
	printf("No duplicate courses found.\n");
}

void StudentMenu(Student* student, Course* courseHead) {
	int choice;
	char schedules[3][MAX_ENROLLED_COURSES][20] = { 0 };
	char scheduleNames[3][50] = { 0 };

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
		scanf("%d", &choice);

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
			GenerateSchedules(student, courseHead, schedules, scheduleNames);
			break;
		case 8:
			PrintSchedules(student, courseHead, schedules, scheduleNames);
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

