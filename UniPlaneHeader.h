#ifndef UNIPLANE_H_
#define UNIPLANE_H_

 
              //   کامنت ها بصورت دستی برای خوانایی کد و رساندن منظور در این قسمت اضافه شده

#define MAX_STUDY_LENGTH 50   //حداکثر طول رشته تحصیلی
#define MAX_PASSED_COURSES 10 // حداکثر تعداد دروس پاس شده
#define MAX_PREREQUISITES 5   //حداکثر تعداد پیش نیازها
#define MAX_COURSES 200       // حداکثر تعداد درس هایی که تو این برنامه ن
#define MAX_ENROLLED_COURSES 10 // حداکثر تعداد دروسی که ثبت‌نام شده
#define MAX_SCHEDULES 100  // حداکثر تعداد برنامه‌های قابل ذخیره شدن

extern int CourseIDCounter[MAX_COURSES];

typedef struct Student {
	char Name[50];
	char LastName[50];
	char ID[9];
	char UserName[20];
	char Email[100];
	char FieldOfStudy[MAX_STUDY_LENGTH];     
    int NumberOfUnitsPassed;
	char CoursesPassed[MAX_PASSED_COURSES][10]; 
    char EnrolledCourses[MAX_ENROLLED_COURSES][20];
    char password[20];
	struct Student* next;
}Student;

typedef struct Course {
    char CourseName[100];
    char CourseID[20];
    int Units;
    char Days[50];
    char StartTime[10];
    char EndTime[10];
    char Department[50];
    char Professor[50];
    int MinUnitsRequired;
    char Prerequisites[MAX_PREREQUISITES][10];
    int PrerequisiteCount;
    char Corequisite[10];
    struct Course* next;
} Course;

typedef struct StudentSchedule {
    char studentName[50]; 
    char scheduleName[50];
    char schedule[MAX_ENROLLED_COURSES][20]; 
    int scheduleUnits[MAX_ENROLLED_COURSES];  // واحدهای درسی برای هر درس
    char scheduleTimes[MAX_ENROLLED_COURSES][20];
} StudentSchedule;
StudentSchedule allSchedules[MAX_SCHEDULES];

extern const char* FieldOfStudy[];
extern const int FieldOfStudyCount;


int welcome();
void signup(Student** head, Course* courseHead);
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
void ListStudents(Student* head, Course* courseHead);
void ViewPrerequisites(Course* course);
void StudentMenu(Student* student, Course* courseHead); 
void EnrollInCourses(Student* student, Course* courseHead);
void ChangePassword(Student* student);
void ChangeUsername(Student* student);
void ViewCourses(Student* student, Course* courseHead);
void RequestNewProgram(Student* student, Course* courseHead);
void PrioritizeCourses(Student* student, Course* courseHead);
void GenerateSchedules(Student* student, Course* courseHead, char schedules[MAX_SCHEDULES][MAX_ENROLLED_COURSES][20], char scheduleNames[MAX_SCHEDULES][50], int maxSchedules);
void PrintSchedules(Student* student, Course* courseHead, char schedules[MAX_SCHEDULES][MAX_ENROLLED_COURSES][20], char scheduleNames[MAX_SCHEDULES][50], int maxSchedules);
void CheckDuplicateCourses(Student* student);
void ViewAllSchedules();
void FreeStudentList(Student* head);
void FreeCourseList(Course* head);

#endif //UNIPLANE_H_