#ifndef __GCOL_H__
#define __GCOL_H__

#include <assert.h>
#include <string.h>
#include <stdlib.h>

#define MAX_STRUCTURE_NAME_SIZE 128U
#define MAX_FIELD_NAME_SIZE 128U

/* enumeration for data types */
typedef enum{
    UINT8,
    UINT16,
    UINT32,
    INT32,
    CHAR,
    OBJ_PTR,
    VOID_PTR,
    FLOAT,
    DOUBLE,
    OBJ_STRUCT
}data_type_t;

typedef enum{
    GCOL_FALSE,
    GCOL_TRUE
}gcol_bool_t;

typedef struct struct_db_rec_ struct_db_rec_t;

typedef struct field_info_t{
    char field_name[MAX_FIELD_NAME_SIZE]; /* name of the element */
    data_type_t dtype;      /* data type of the field */
    unsigned int size;      /* size of the field */
    unsigned int offset;    /* offset for the field */
    /* This field is meaningful only when dtype is OBJ_PTR or OBJ_STRUCT */
    char nested_struct_name[MAX_STRUCTURE_NAME_SIZE];
}field_info_t;

/* Structure to store the information of one C struct, which has n- elelments */
struct struct_db_rec_{
    struct_db_rec_t *next;                      /* Pointer to next structure in the list */
    char struct_name[MAX_STRUCTURE_NAME_SIZE];  /* name of the structure */
    unsigned int ds_size;                       /* size of the structure */
    unsigned int n_fields;                      /* number of fields in the structure */
    field_info_t *fields;                       /* pointer to the 2D array of fields */
};

#define OFFSET_OF(struct_name, field_name) \
    (unsigned long)&(((struct_name *)0)->field_name)

#define FIELD_SIZE(struct_name, field_name) \
    sizeof(((struct_name *)0)->field_name)

typedef struct _struct_db{
    struct_db_rec_t *head;
    unsigned int count;
}struct_db_t;

/* Function declarations */

void print_structure_rec(struct_db_rec_t *struct_rec);

void print_structure_db(struct_db_t *struct_db);

int add_structure_to_struct_db(struct_db_t *struct_db, struct_db_rec_t *struct_rec);

#define FIELD_INFO(struct_name, field_name, dtype, nested_struct_name) \
    {#field_name, dtype, FIELD_SIZE(struct_name, field_name), \
    OFFSET_OF(struct_name, field_name), #nested_struct_name}

#define REG_STRUCT(struct_db, structure_name, fields_array)             \
{                                                                       \
    struct_db_rec_t *rec = calloc(1, sizeof(struct_db_rec_t));          \
    strncpy(rec->struct_name, #structure_name, MAX_STRUCTURE_NAME_SIZE);\
    rec->ds_size = sizeof(structure_name);                              \
    rec->n_fields = sizeof(fields_array)/ sizeof(field_info_t);         \
    rec->fields = fields_array;                                         \
    if(add_structure_to_struct_db(struct_db, rec)){                     \
        assert(0);                                                      \
    }                                                                   \
}

#endif