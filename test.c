#include <stdio.h>
#include "gcol.h"

typedef struct emp_{
    char emp_name[30];
    unsigned int emp_id;
    unsigned int age;
    struct emp_ *mgr;
    float salary;
}emp_t;

typedef struct student_{
    char student_name[30];
    unsigned int rollno;
    unsigned int age;
    float aggregate;
    struct student_ *best_colleague;
}student_t;

int main(int argc, char **argv)
{
    /* initialize a new structure database */
    struct_db_t *struct_db = calloc(1, sizeof(struct_db_t));

    /* create structure record for structure emp_t */
    static field_info_t emp_fields[] = {
        FIELD_INFO(emp_t, emp_name, CHAR,    0),
        FIELD_INFO(emp_t, emp_id,   UINT32,  0),
        FIELD_INFO(emp_t, age,      UINT32,  0),
        FIELD_INFO(emp_t, mgr,      OBJ_PTR, emp_t),
        FIELD_INFO(emp_t, salary,   FLOAT,   0)
    };

    /*Step 3 : Register the structure in structure database*/
    REG_STRUCT(struct_db, emp_t, emp_fields);

    static field_info_t stud_fields[] = {
        FIELD_INFO(student_t, student_name,  CHAR,    0),
        FIELD_INFO(student_t, rollno,        UINT32,  0),
        FIELD_INFO(student_t, age,           UINT32,  0),
        FIELD_INFO(student_t, aggregate,     FLOAT,   0),
        FIELD_INFO(student_t, best_colleague, OBJ_PTR, student_t)
    };
    REG_STRUCT(struct_db, student_t, stud_fields);

    /*Step 4 : Verify the correctness of structure database*/
    print_structure_db(struct_db);

    return 0;
}
