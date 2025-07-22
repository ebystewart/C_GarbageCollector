#ifndef __GCOL_H__
#define __GCOL_H__

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
    char filed_name[MAX_FIELD_NAME_SIZE]; /* name of the element */
    data_type_t dtype;      /* data type of the filed */
    unsigned int size;      /* size of the field */
    unsigned int offset;    /* offset for the field */
    /* This field is meaningful only when dtype is OBJ_PTR or OBJ_STRUCT */
    char nested_str_name[MAX_STRUCTURE_NAME_SIZE];
}field_info_t;

/* Structure to store the information of one C struct, which has n- elelments */
struct struct_db_rec_{
    struct_db_rec_t *next;                      /* Pointer to next structure in the list */
    char struct_name[MAX_STRUCTURE_NAME_SIZE];  /* name of the structure */
    unsigned int ds_size;                       /* size of the structure */
    unsigned int n_fields;                      /* number of fields in the structure */
    field_info_t *fileds;                       /* pointer to the @D array of fields */
};

#endif