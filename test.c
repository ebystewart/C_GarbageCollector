#include <stdio.h>
#include "gcol.h"

typedef struct emp_{
    char emp_name[30];
    unsigned int emp_id;
    unsigned int age;
    struct emp_ *mgr;
    float salary;
    int *rating;
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

    gcol_init_primitive_data_types_support(struct_db);

    /* create structure record for structure emp_t */
    static field_info_t emp_fields[] = {
        FIELD_INFO(emp_t, emp_name, CHAR,    0),
        FIELD_INFO(emp_t, emp_id,   UINT32,  0),
        FIELD_INFO(emp_t, age,      UINT32,  0),
        FIELD_INFO(emp_t, mgr,      OBJ_PTR, emp_t),
        FIELD_INFO(emp_t, salary,   FLOAT,   0),
        FIELD_INFO(emp_t, rating,   OBJ_PTR, 0)
    };

    /*Step 3 : Register the structure in structure database*/
    REG_STRUCT(struct_db, emp_t, emp_fields);

    static field_info_t stud_fields[] = {
        FIELD_INFO(student_t, student_name,   CHAR,    0),
        FIELD_INFO(student_t, rollno,         UINT32,  0),
        FIELD_INFO(student_t, age,            UINT32,  0),
        FIELD_INFO(student_t, aggregate,      FLOAT,   0),
        FIELD_INFO(student_t, best_colleague, OBJ_PTR, student_t)
    };
    REG_STRUCT(struct_db, student_t, stud_fields);

    /*Step 4 : Verify the correctness of structure database*/
    print_structure_db(struct_db);

    /*Working with object database*/
    /*Step 1 : Initialize a new Object database */
    object_db_t *object_db = calloc(1, sizeof(object_db_t));
    object_db->struct_db = struct_db;
    
    /*Step 2 : Create some sample objects, equivalent to standard 
     * calloc(1, sizeof(student_t))*/
    student_t *asha = xcalloc(object_db, "student_t", 1);
    gcol_set_dynamic_object_as_root(object_db, asha);

    /* to check if this is detected as leaked object, lets not set this as root object */
    student_t *dinakar = xcalloc(object_db, "student_t", 1);
    strncpy(dinakar->student_name, "dinakar", strlen("dinakar"));

    emp_t *warren = xcalloc(object_db, "emp_t", 2);
    gcol_set_dynamic_object_as_root(object_db, warren);
    warren->rating = xcalloc(object_db, "int", 1);

    print_object_db(object_db);

    run_gcol_algorithm(object_db);
    printf("Leaked Objects : \n");
    report_leaked_objects(object_db);

    return 0;
}
