#ifndef UNIPLANE_H_
#define UNIPLANE_H_


#define MAX_STUDY_LENGTH 30   //حداکثر طول رشته تحصیلی
#define MAX_PASSED_COURSES 10 // حداکثر تعداد دروس پاس شده
#define MAX_PREREQUISITES 5   //حداکثر تعداد پیش نیازها
#define MAX_COURSES 100       // حداکثر تعداد درس ها در این برنامه

typedef struct Student {
	char Name[20];
	char LastName[50];
	char ID[9];
	char UserName[20];
	char Email[100];
	char FieldOfStudy[MAX_STUDY_LENGTH];     //رشته تحصیلی
	int NumberOfUnitsPassed;
	char CoursesPassed[MAX_PASSED_COURSES][10];  //کد دروس پاس شده
	char password[20];
	struct Student* next;
}Student;

typedef struct Course {
	char CourseName[100];
	char CourseID[10];
	char Days[50];     
	char StartTime[10];
	char EndTime[10];
	char Department[50];
	char Professor[50];
	int MinUnitsRequired;                         //حداقل واحد 
	char Prerequisites[MAX_PREREQUISITES][10];   //پیش نیاز و اینا
	struct Course* next;
}Course;
extern const char* FieldOfStudy[];
extern const int FieldOfStudyCount;

int welcome();
void signup(Student** head);
int login(Student* head);
void Golestan(Student* studentHead, Course** courseHead);
void SaveToFile(Student* student);
void LoadFromFile(Student** head);
void AddCourse(Course** head);
void SaveCourseToFile(Course* course);
void LoadCoursesFromFile(Course** head);
void ManagePrerequisites(Course* courseHead); 
void AddPrerequisite(Course* course);
void ChangePrerequisite(Course* course);
void RemovePrerequisite(Course* course);
void ListCourses(Course* head);
void ListStudents(Student* head);
void ViewPrerequisites(Course* course);
void FreeStudentList(Student* head);
void FreeCourseList(Course* head);



#endif //UNIPLANE_H_