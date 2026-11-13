/*
 * Exam Result and Grade Management System
 * ---------------------------------------
 * A simple menu-driven C program to store student records,
 * calculate total marks, percentage and grade, and keep
 * everything saved in a file (students.dat).
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_STUDENTS 100          /* maximum students we can store     */
#define NUM_SUBJECTS 6            /* every student has 6 subjects      */
#define NAME_LENGTH  50           /* maximum characters in a name      */
#define FILE_NAME    "students.dat"

/* ---------- Structure that holds one student's data ---------- */
struct Student {
    char  name[NAME_LENGTH];
    int   roll;
    int   marks[NUM_SUBJECTS];    /* array: one mark per subject       */
    int   total;
    float percentage;
    char  grade[3];               /* "A+", "A", "B", ... plus '\0'     */
};

/* ---------- Global array of students and a counter ---------- */
struct Student students[MAX_STUDENTS];
int studentCount = 0;

/* ---------- Names of the 6 subjects (same order as marks[]) ---------- */
const char *subjectNames[NUM_SUBJECTS] = {
    "CALCULAS", "PROGRAMMING", "ITFA", "PHYSICS", "ENGLISH", "PST"
};

/* ---------- Function declarations ---------- */
void showMenu(void);
void readLine(const char *prompt, char *dest, int size);
int  readInt(const char *prompt, int min, int max);
int  findStudent(int roll);
void calculateResult(struct Student *s);
void assignGrade(struct Student *s);
void printResult(const struct Student *s);
void saveToFile(void);
void loadFromFile(void);
void addStudent(void);
void viewAllStudents(void);
void searchStudent(void);
void calculateAll(void);
void updateStudent(void);
void deleteStudent(void);
void refreshAllResults(void);
void showStatistics(void);
void showRankings(void);
void saveRecords(void);
void loadRecords(void);
void saveLoadMenu(void);

/* =========================================================
 *  INPUT HELPER FUNCTIONS
 * ========================================================= */

/* Reads a line of text safely (so names with spaces work). */
void readLine(const char *prompt, char *dest, int size)
{
    char buffer[200];
    int len;

    while (1) {
        printf("%s", prompt);

        /* If input ends (Ctrl+D / Ctrl+Z), save and quit nicely */
        if (fgets(buffer, sizeof(buffer), stdin) == NULL) {
            saveToFile();
            printf("\nInput closed. Data saved. Goodbye!\n");
            exit(0);
        }

        len = strlen(buffer);

        /* If there is no newline, the line was too long:
           throw away the rest of it so it doesn't affect next input */
        if (len > 0 && buffer[len - 1] != '\n') {
            int ch;
            while ((ch = getchar()) != '\n' && ch != EOF) {
            }
        } else if (len > 0) {
            buffer[len - 1] = '\0';   /* remove the newline */
        }

        if (strlen(buffer) == 0) {
            printf("Input cannot be empty. Try again.\n");
        } else if ((int)strlen(buffer) >= size) {
            printf("Input too long (max %d characters). Try again.\n", size - 1);
        } else {
            strcpy(dest, buffer);
            return;
        }
    }
}

/* Reads an integer between min and max. Keeps asking until valid. */
int readInt(const char *prompt, int min, int max)
{
    char buffer[100];
    char *end;
    long value;

    while (1) {
        readLine(prompt, buffer, sizeof(buffer));

        value = strtol(buffer, &end, 10);

        /* skip spaces after the number */
        while (*end == ' ' || *end == '\t') {
            end++;
        }

        if (end == buffer || *end != '\0') {
            printf("Invalid input! Please enter a whole number.\n");
        } else if (value < min || value > max) {
            printf("Invalid value! Enter a number between %d and %d.\n", min, max);
        } else {
            return (int)value;
        }
    }
}

/* =========================================================
 *  FILE HANDLING
 * ========================================================= */

/* Writes all student records to the file. */
void saveToFile(void)
{
    FILE *fp = fopen(FILE_NAME, "wb");   /* wb = write binary */

    if (fp == NULL) {
        printf("Error: could not save data to file!\n");
        return;
    }

    fwrite(&studentCount, sizeof(int), 1, fp);                 /* how many */
    fwrite(students, sizeof(struct Student), studentCount, fp); /* the data */
    fclose(fp);
}

/* Reads all student records from the file when the program starts. */
void loadFromFile(void)
{
    FILE *fp = fopen(FILE_NAME, "rb");   /* rb = read binary */
    int count = 0;

    if (fp == NULL) {
        return;   /* no file yet: first time running, nothing to load */
    }

    if (fread(&count, sizeof(int), 1, fp) == 1 &&
        count >= 0 && count <= MAX_STUDENTS) {
        studentCount = (int)fread(students, sizeof(struct Student), count, fp);
    }

    fclose(fp);
}

/* =========================================================
 *  CALCULATION FUNCTIONS
 * ========================================================= */

/* Returns the position of a student in the array, or -1 if not found. */
int findStudent(int roll)
{
    int i;
    for (i = 0; i < studentCount; i++) {
        if (students[i].roll == roll) {
            return i;
        }
    }
    return -1;
}

/* Calculates total marks and percentage for one student. */
void calculateResult(struct Student *s)
{
    int i;
    s->total = 0;

    for (i = 0; i < NUM_SUBJECTS; i++) {
        s->total += s->marks[i];
    }

    /* each subject is out of 100, so full marks = 6 x 100 = 600 */
    s->percentage = (float)s->total / (NUM_SUBJECTS * 100) * 100;
}

/* Decides the grade from the percentage. */
void assignGrade(struct Student *s)
{
    if (s->percentage >= 90) {
        strcpy(s->grade, "A+");
    } else if (s->percentage >= 80) {
        strcpy(s->grade, "A");
    } else if (s->percentage >= 70) {
        strcpy(s->grade, "B");
    } else if (s->percentage >= 60) {
        strcpy(s->grade, "C");
    } else if (s->percentage >= 50) {
        strcpy(s->grade, "D");
    } else {
        strcpy(s->grade, "F");
    }
}

/* =========================================================
 *  DISPLAY FUNCTION
 * ========================================================= */

/* Prints the full result card of one student. */
void printResult(const struct Student *s)
{
    int i;

    printf("\n========== STUDENT RESULT ==========\n");
    printf("Name        : %s\n", s->name);
    printf("Roll Number : %d\n", s->roll);
    printf("------------------------------------\n");

    for (i = 0; i < NUM_SUBJECTS; i++) {
        printf("%-12s: %d\n", subjectNames[i], s->marks[i]);
    }

    printf("------------------------------------\n");
    printf("Total Marks : %d / %d\n", s->total, NUM_SUBJECTS * 100);
    printf("Percentage  : %.2f%%\n", s->percentage);
    printf("Grade       : %s\n", s->grade);
    printf("Result      : %s\n", (s->percentage >= 50) ? "PASS" : "FAIL");
    printf("====================================\n");
}

/* =========================================================
 *  MENU OPTION FUNCTIONS
 * ========================================================= */

/* Option 1: add a new student */
void addStudent(void)
{
    struct Student s;
    int i;

    if (studentCount >= MAX_STUDENTS) {
        printf("\nStorage is full! Cannot add more students.\n");
        return;
    }

    printf("\n--- Add Student Record ---\n");

    readLine("Enter student name: ", s.name, NAME_LENGTH);

    /* roll number must be positive and must not already exist */
    while (1) {
        s.roll = readInt("Enter roll number: ", 1, 999999);
        if (findStudent(s.roll) != -1) {
            printf("This roll number already exists. Enter a different one.\n");
        } else {
            break;
        }
    }

    for (i = 0; i < NUM_SUBJECTS; i++) {
        char prompt[50];
        sprintf(prompt, "Enter marks for %s (0-100): ", subjectNames[i]);
        s.marks[i] = readInt(prompt, 0, 100);
    }

    calculateResult(&s);
    assignGrade(&s);

    students[studentCount] = s;
    studentCount++;
    saveToFile();

    printf("\nStudent added successfully!\n");
}

/* Option 2: show all students in a table */
void viewAllStudents(void)
{
    int i;

    if (studentCount == 0) {
        printf("\nNo records found.\n");
        return;
    }

    printf("\n--- All Student Records ---\n");
    printf("%-8s %-22s %-8s %-12s %-6s\n",
           "Roll", "Name", "Total", "Percentage", "Grade");
    printf("-----------------------------------------------------------\n");

    for (i = 0; i < studentCount; i++) {
        printf("%-8d %-22s %-8d %-12.2f %-6s\n",
               students[i].roll, students[i].name, students[i].total,
               students[i].percentage, students[i].grade);
    }
}

/* Option 3: search by roll number */
void searchStudent(void)
{
    int roll, index;

    if (studentCount == 0) {
        printf("\nNo records found.\n");
        return;
    }

    printf("\n--- Search Student ---\n");
    roll = readInt("Enter roll number to search: ", 1, 999999);
    index = findStudent(roll);

    if (index == -1) {
        printf("Student with roll number %d not found.\n", roll);
    } else {
        printf("Student found!\n");
        printResult(&students[index]);
    }
}

/* Option 4: recalculate total and percentage for every student */
void calculateAll(void)
{
    int i;

    if (studentCount == 0) {
        printf("\nNo records found.\n");
        return;
    }

    for (i = 0; i < studentCount; i++) {
        calculateResult(&students[i]);
    }
    saveToFile();

    printf("\nTotal marks and percentage calculated for %d student(s).\n",
           studentCount);
}

/* Option 5: update name and marks of a student */
void updateStudent(void)
{
    int roll, index, i;

    if (studentCount == 0) {
        printf("\nNo records found.\n");
        return;
    }

    printf("\n--- Update Student Record ---\n");
    roll = readInt("Enter roll number of student to update: ", 1, 999999);
    index = findStudent(roll);

    if (index == -1) {
        printf("Student with roll number %d not found.\n", roll);
        return;
    }

    printf("Current name: %s\n", students[index].name);
    readLine("Enter new name: ", students[index].name, NAME_LENGTH);

    for (i = 0; i < NUM_SUBJECTS; i++) {
        char prompt[70];
        sprintf(prompt, "Enter new marks for %s (current %d): ",
                subjectNames[i], students[index].marks[i]);
        students[index].marks[i] = readInt(prompt, 0, 100);
    }

    calculateResult(&students[index]);
    assignGrade(&students[index]);
    saveToFile();

    printf("\nRecord updated successfully!\n");
}

/* Option 6: delete a student */
void deleteStudent(void)
{
    int roll, index, i;
    char answer[10];

    if (studentCount == 0) {
        printf("\nNo records found.\n");
        return;
    }

    printf("\n--- Delete Student Record ---\n");
    roll = readInt("Enter roll number of student to delete: ", 1, 999999);
    index = findStudent(roll);

    if (index == -1) {
        printf("Student with roll number %d not found.\n", roll);
        return;
    }

    printf("Student: %s\n", students[index].name);
    readLine("Are you sure you want to delete? (y/n): ", answer, sizeof(answer));

    if (answer[0] == 'y' || answer[0] == 'Y') {
        /* move every later student one place to the left */
        for (i = index; i < studentCount - 1; i++) {
            students[i] = students[i + 1];
        }
        studentCount--;
        saveToFile();
        printf("Record deleted successfully!\n");
    } else {
        printf("Deletion cancelled.\n");
    }
}

/* =========================================================
 *  CLASS STATISTICS AND RANKING (new features)
 * ========================================================= */

/* Recalculates total, percentage and grade of every student.
   Statistics and rankings call this first, so they always use
   the latest marks. (It does not change the marks themselves.) */
void refreshAllResults(void)
{
    int i;

    for (i = 0; i < studentCount; i++) {
        calculateResult(&students[i]);
        assignGrade(&students[i]);
    }
}

/* Option 7: show class statistics */
void showStatistics(void)
{
    int i;
    int passed = 0;
    int failed = 0;
    float sumPercentage = 0;
    int highestTotal, lowestTotal;
    float highestPercentage, lowestPercentage;

    if (studentCount == 0) {
        printf("\nNo records found.\n");
        return;
    }

    refreshAllResults();   /* make sure we use the latest data */

    /* start by assuming the first student is both highest and lowest */
    highestTotal      = students[0].total;
    lowestTotal       = students[0].total;
    highestPercentage = students[0].percentage;
    lowestPercentage  = students[0].percentage;

    for (i = 0; i < studentCount; i++) {
        /* pass mark is 50% (same rule as the F grade) */
        if (students[i].percentage >= 50) {
            passed++;
        } else {
            failed++;
        }

        sumPercentage += students[i].percentage;

        if (students[i].total > highestTotal) {
            highestTotal      = students[i].total;
            highestPercentage = students[i].percentage;
        }
        if (students[i].total < lowestTotal) {
            lowestTotal      = students[i].total;
            lowestPercentage = students[i].percentage;
        }
    }

    printf("\n========== CLASS STATISTICS ==========\n");
    printf("Total students      : %d\n", studentCount);
    printf("Students passed     : %d\n", passed);
    printf("Students failed     : %d\n", failed);
    printf("Class average       : %.2f%%\n", sumPercentage / studentCount);
    printf("Highest percentage  : %.2f%%\n", highestPercentage);
    printf("Lowest percentage   : %.2f%%\n", lowestPercentage);

    /* print every student who has the highest total (in case of a tie) */
    printf("\nTopper(s):\n");
    for (i = 0; i < studentCount; i++) {
        if (students[i].total == highestTotal) {
            printf("  %s (Roll %d) - %d marks, %.2f%%, Grade %s\n",
                   students[i].name, students[i].roll, students[i].total,
                   students[i].percentage, students[i].grade);
        }
    }

    /* print every student who has the lowest total (in case of a tie) */
    printf("\nLowest-performing student(s):\n");
    for (i = 0; i < studentCount; i++) {
        if (students[i].total == lowestTotal) {
            printf("  %s (Roll %d) - %d marks, %.2f%%, Grade %s\n",
                   students[i].name, students[i].roll, students[i].total,
                   students[i].percentage, students[i].grade);
        }
    }
    printf("======================================\n");
}

/* Option 8: display student rankings (highest marks first) */
void showRankings(void)
{
    int order[MAX_STUDENTS];   /* stores positions of students, to be sorted */
    int i, j, temp;
    int rank = 1;

    if (studentCount == 0) {
        printf("\nNo records found.\n");
        return;
    }

    refreshAllResults();   /* make sure we use the latest data */

    /* start with positions 0, 1, 2, ... in the same order as the records */
    for (i = 0; i < studentCount; i++) {
        order[i] = i;
    }

    /* Bubble sort: arrange the positions so that the student with the
       highest total comes first. Only the order[] list is sorted, the
       real student records stay in their original places. */
    for (i = 0; i < studentCount - 1; i++) {
        for (j = 0; j < studentCount - 1 - i; j++) {
            if (students[order[j]].total < students[order[j + 1]].total) {
                temp         = order[j];
                order[j]     = order[j + 1];
                order[j + 1] = temp;
            }
        }
    }

    printf("\n--- Student Rankings ---\n");
    printf("%-6s %-22s %-8s %-8s %-12s %-6s\n",
           "Rank", "Name", "Roll", "Total", "Percentage", "Grade");
    printf("---------------------------------------------------------------\n");

    for (i = 0; i < studentCount; i++) {
        /* a new rank is given only when the total is lower than the
           previous student's total; equal totals share the same rank */
        if (i > 0 && students[order[i]].total != students[order[i - 1]].total) {
            rank = i + 1;
        }

        printf("%-6d %-22s %-8d %-8d %-12.2f %-6s\n",
               rank,
               students[order[i]].name,
               students[order[i]].roll,
               students[order[i]].total,
               students[order[i]].percentage,
               students[order[i]].grade);
    }
}

/* =========================================================
 *  SAVE AND LOAD OPTION (new feature)
 * ========================================================= */

/* Saves all records to the file and tells the user. */
void saveRecords(void)
{
    FILE *fp = fopen(FILE_NAME, "wb");

    if (fp == NULL) {
        printf("Error: could not save data to file!\n");
        return;
    }

    fwrite(&studentCount, sizeof(int), 1, fp);
    fwrite(students, sizeof(struct Student), studentCount, fp);
    fclose(fp);

    printf("%d record(s) saved to %s successfully!\n", studentCount, FILE_NAME);
}

/* Loads all records from the file and tells the user.
   The records currently in memory are replaced by the saved ones. */
void loadRecords(void)
{
    FILE *fp = fopen(FILE_NAME, "rb");
    int count = 0;
    char answer[10];

    if (fp == NULL) {
        printf("No saved file (%s) found. Nothing to load.\n", FILE_NAME);
        return;
    }

    if (fread(&count, sizeof(int), 1, fp) != 1 ||
        count < 0 || count > MAX_STUDENTS) {
        printf("Error: the file is empty or damaged.\n");
        fclose(fp);
        return;
    }

    readLine("Loading will replace current records. Continue? (y/n): ",
             answer, sizeof(answer));

    if (answer[0] != 'y' && answer[0] != 'Y') {
        printf("Load cancelled.\n");
        fclose(fp);
        return;
    }

    studentCount = (int)fread(students, sizeof(struct Student), count, fp);
    fclose(fp);

    printf("%d record(s) loaded from %s successfully!\n", studentCount, FILE_NAME);
}

/* Option 9: Save and Load sub-menu */
void saveLoadMenu(void)
{
    int choice;

    printf("\n--- Save and Load Records ---\n");
    printf("1. Save records to file\n");
    printf("2. Load records from file\n");
    printf("3. Back to main menu\n");

    choice = readInt("Enter your choice (1-3): ", 1, 3);

    switch (choice) {
        case 1: saveRecords(); break;
        case 2: loadRecords(); break;
        case 3: break;
    }
}

/* =========================================================
 *  MENU AND MAIN
 * ========================================================= */

void showMenu(void)
{
    printf("\n=====================================\n");
    printf("  EXAM RESULT & GRADE MANAGEMENT\n");
    printf("=====================================\n");
    printf("1. Add Student Record\n");
    printf("2. View All Student Records\n");
    printf("3. Search Student by Roll Number\n");
    printf("4. Calculate Total Marks and Percentage\n");
    printf("5. Update Student Record\n");
    printf("6. Delete Student Record\n");
    printf("7. Show Class Statistics\n");
    printf("8. Display Student Rankings\n");
    printf("9. Save and Load Records\n");
    printf("10. Exit\n");
    printf("-------------------------------------\n");
}

int main(void)
{
    int choice;

    loadFromFile();   /* bring back saved records */

    do {
        showMenu();
        choice = readInt("Enter your choice (1-10): ", 1, 10);

        switch (choice) {
            case 1: addStudent();       break;
            case 2: viewAllStudents();  break;
            case 3: searchStudent();    break;
            case 4: calculateAll();     break;
            case 5: updateStudent();    break;
            case 6: deleteStudent();    break;
            case 7: showStatistics();   break;
            case 8: showRankings();     break;
            case 9: saveLoadMenu();     break;
            case 10:
                saveToFile();
                printf("\nData saved. Goodbye!\n");
                break;
        }
    } while (choice != 10);

    return 0;
}
