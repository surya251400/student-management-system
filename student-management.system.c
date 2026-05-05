#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FILE_NAME "students.dat"
#define MAX_NAME  50

/* ── Structure Definition ───────────────────────────────────────────── */
typedef struct {
    int   roll;
    char  name[MAX_NAME];
    float marks[3];          /* marks[0]=Sub1, marks[1]=Sub2, marks[2]=Sub3 */
} Student;

/* ── Helper: total marks ────────────────────────────────────────────── */
float total(const Student *s) {
    return s->marks[0] + s->marks[1] + s->marks[2];
}

/* ── Feature 1: Add a student record ────────────────────────────────── */
void addStudent(void) {
    Student s;

    printf("\n--- Add Student Record ---\n");
    printf("Enter Roll Number : ");
    scanf("%d", &s.roll);
    getchar();                          /* flush newline */

    printf("Enter Name        : ");
    fgets(s.name, MAX_NAME, stdin);
    s.name[strcspn(s.name, "\n")] = '\0'; /* strip trailing newline */

    printf("Enter Marks (Subject 1 2 3) : ");
    scanf("%f %f %f", &s.marks[0], &s.marks[1], &s.marks[2]);

    /* Append record to binary file */
    FILE *fp = fopen(FILE_NAME, "ab");
    if (!fp) { perror("Cannot open file"); return; }
    fwrite(&s, sizeof(Student), 1, fp);
    fclose(fp);

    printf("Record added successfully.\n");
}

/* ── Feature 2: Display all records ─────────────────────────────────── */
void displayAll(void) {
    FILE *fp = fopen(FILE_NAME, "rb");
    if (!fp) { printf("No records found.\n"); return; }

    Student s;
    int found = 0;

    printf("\n%-6s  %-20s  %8s  %8s  %8s  %8s\n",
           "Roll", "Name", "Sub-1", "Sub-2", "Sub-3", "Total");
    printf("%-6s  %-20s  %8s  %8s  %8s  %8s\n",
           "------", "--------------------",
           "--------", "--------", "--------", "--------");

    while (fread(&s, sizeof(Student), 1, fp) == 1) {
        printf("%-6d  %-20s  %8.2f  %8.2f  %8.2f  %8.2f\n",
               s.roll, s.name,
               s.marks[0], s.marks[1], s.marks[2], total(&s));
        found = 1;
    }
    fclose(fp);

    if (!found) printf("No records found.\n");
}

/* ── Feature 3: Rank list (descending by total marks) ───────────────── */
void rankList(void) {
    FILE *fp = fopen(FILE_NAME, "rb");
    if (!fp) { printf("No records found.\n"); return; }

    /* Count records */
    fseek(fp, 0, SEEK_END);
    long fileSize = ftell(fp);
    rewind(fp);

    int n = (int)(fileSize / sizeof(Student));
    if (n == 0) { printf("No records found.\n"); fclose(fp); return; }

    /* Load all records into a dynamic array */
    Student *arr = (Student *)malloc(n * sizeof(Student));
    if (!arr) { perror("Memory error"); fclose(fp); return; }
    fread(arr, sizeof(Student), n, fp);
    fclose(fp);

    /* Bubble sort — descending order of total marks */
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - 1 - i; j++) {
            if (total(&arr[j]) < total(&arr[j + 1])) {
                Student tmp = arr[j];
                arr[j]     = arr[j + 1];
                arr[j + 1] = tmp;
            }
        }
    }

    /* Display rank list */
    printf("\n%-5s  %-6s  %-20s  %8s  %8s  %8s  %8s\n",
           "Rank", "Roll", "Name", "Sub-1", "Sub-2", "Sub-3", "Total");
    printf("%-5s  %-6s  %-20s  %8s  %8s  %8s  %8s\n",
           "-----", "------", "--------------------",
           "--------", "--------", "--------", "--------");

    for (int i = 0; i < n; i++) {
        printf("%-5d  %-6d  %-20s  %8.2f  %8.2f  %8.2f  %8.2f\n",
               i + 1, arr[i].roll, arr[i].name,
               arr[i].marks[0], arr[i].marks[1], arr[i].marks[2],
               total(&arr[i]));
    }

    free(arr);
}

/* ── Main menu ───────────────────────────────────────────────────────── */
int main(void) {
    int choice;

    do {
        printf("\n========= Student Record Management =========\n");
        printf("  1. Add Student Record\n");
        printf("  2. Display All Records\n");
        printf("  3. Generate Rank List\n");
        printf("  4. Exit\n");
        printf("=============================================\n");
        printf("Enter your choice : ");
        scanf("%d", &choice);

        switch (choice) {
            case 1: addStudent();  break;
            case 2: displayAll();  break;
            case 3: rankList();    break;
            case 4: printf("Exiting program. Goodbye!\n"); break;
            default: printf("Invalid choice. Please try again.\n");
        }
    } while (choice != 4);

    return 0;
}
