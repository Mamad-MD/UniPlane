#ifndef UNIPLANE_H_
#define UNIPLANE_H_

#define MAX_STUDY_LENGTH 30   //حداکثر طول رشته تحصیلی
#define MAX_PASSED_COURSES 10 // حداکثر تعداد دروس پاس شده


typedef struct {
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


extern const char* FieldOfStudy[];
extern const int FieldOfStudyCount;


int welcome();
void signup(Student** head);
int login(Student* head);
void Golestan();
void SaveToFile(Student* student);
void LoadFromFile(Student** head);

#endif //UNIPLANE_H_