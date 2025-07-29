#include <stdint.h>
#include <stdio.h>
#include "gcol.h"
#include "css.h"
#include "memory.h"

char *DATA_TYPE[] = {"UINT8", "UINT32", "INT32",
    "CHAR", "OBJ_PTR", "VOID_PTR", "FLOAT",
    "DOUBLE", "OBJ_STRUCT"};

void print_structure_rec(struct_db_rec_t *struct_rec)
{
    if(!struct_rec)
        return;
    int j = 0;
    field_info_t *field = NULL;
    printf(ANSI_COLOR_CYAN "|------------------------------------------------------|\n" ANSI_COLOR_RESET);
    printf(ANSI_COLOR_YELLOW "| %-20s | size = %-8d | #flds = %-3d |\n" ANSI_COLOR_RESET, struct_rec->struct_name, struct_rec->ds_size, struct_rec->n_fields);
    printf(ANSI_COLOR_CYAN "|------------------------------------------------------|------------------------------------------------------------------------------------------|\n" ANSI_COLOR_RESET);
    for(j = 0; j < struct_rec->n_fields; j++){
        field = &struct_rec->fields[j];
        printf("  %-20s |", "");
        printf("%-3d %-20s | dtype = %-15s | size = %-5d | offset = %-6d|  nstructname = %-20s  |\n",
                j, field->field_name, DATA_TYPE[field->dtype], field->size, field->offset, field->nested_struct_name);
        printf("  %-20s |", "");
        printf(ANSI_COLOR_CYAN "--------------------------------------------------------------------------------------------------------------------------|\n" ANSI_COLOR_RESET);
    }  
}

void print_structure_db(struct_db_t *struct_db){

    if(!struct_db)
        return;
    printf("Printing struct database\n");

    struct_db_rec_t *struct_rec = struct_db->head;
    printf("No. of registered structures is %d\n",struct_db->count);
    int i = 0U;
    while(struct_rec){
        printf("Structure No.: %d (%p)\n", i++, struct_rec);
        print_structure_rec(struct_rec);
        struct_rec = struct_rec->next;
    }
}

int add_structure_to_struct_db(struct_db_t *struct_db, struct_db_rec_t *struct_rec)
{
    struct_db_rec_t *head = struct_db->head;
    struct_db_rec_t *curr = NULL;
    if(!head){
        struct_db->head = struct_rec;
        struct_rec->next = NULL;
        struct_db->count++;
        return 0;
    }
 #if 0   
    curr = head;
    for(; curr; curr = curr->next){
        continue;
    }
    curr = struct_rec;
#endif
    struct_rec->next = head;
    struct_db->head = struct_rec;
    struct_db->count++;
    return 0;
}

static struct_db_rec_t *struct_db_lookup(struct_db_t *struct_db, char *struct_name)
{
    struct_db_rec_t *curr_rec = NULL;
    struct_db_rec_t *head = struct_db->head;
    curr_rec = head;
    for(; curr_rec; curr_rec = curr_rec->next){
        if(strncmp(struct_name, curr_rec->struct_name, MAX_STRUCTURE_NAME_SIZE) == 0U){
            return curr_rec;
        }
    }
    return NULL;
}


void print_object_rec(object_db_rec_t *obj_rec, int i)
{
    if(!obj_rec) return;
    printf(ANSI_COLOR_MAGENTA "-----------------------------------------------------------------------------------------------------|\n"ANSI_COLOR_RESET);
    printf(ANSI_COLOR_YELLOW "%-3d ptr = %-10p | next = %-10p | units = %-4d | struct_name = %-10s | is_root = %s |\n"ANSI_COLOR_RESET, 
        i, obj_rec->ptr, obj_rec->next, obj_rec->units, obj_rec->struct_rec->struct_name, obj_rec->is_root ? "TRUE " : "FALSE"); 
    printf(ANSI_COLOR_MAGENTA "-----------------------------------------------------------------------------------------------------|\n"ANSI_COLOR_RESET);
}

void print_object_db(object_db_t *object_db)
{
    object_db_rec_t *head = object_db->head;
    unsigned int i = 0;
    printf(ANSI_COLOR_CYAN "Printing OBJECT DATABASE\n");
    for(; head; head = head->next){
        print_object_rec(head, i++);
    }
}

static object_db_rec_t *object_db_lookup(object_db_t *object_db, void *ptr){

    if(!object_db)
        return NULL;
    
    object_db_rec_t *object_rec = object_db->head;
    for(; object_rec; object_rec = object_rec->next){
        if(object_rec->ptr == ptr)
            return object_rec;
    }
    return NULL;
}

static void add_object_to_object_db(object_db_t *object_db, void *ptr, int units, 
    struct_db_rec_t *struct_rec, gcol_bool_t is_root){

    object_db_rec_t *obj_rec = object_db_lookup(object_db, ptr);
    assert(!obj_rec);

    obj_rec = calloc(1, sizeof(object_db_rec_t));
    obj_rec->next = NULL;
    obj_rec->ptr = ptr;
    obj_rec->struct_rec = struct_rec;
    obj_rec->units = units;
    obj_rec->is_visited = GCOL_FALSE;
    obj_rec->is_root = is_root;

    object_db_rec_t *head = object_db->head;
    if(!head){
        object_db->head = obj_rec;
        obj_rec->next = NULL;
        object_db->count++;
        return;
    }
    object_db->head = obj_rec;
    obj_rec->next = head;
    object_db->count++;
}

static void
delete_object_record_from_object_db(object_db_t *object_db, 
                                    object_db_rec_t *object_rec){
    
    assert(object_rec);

    object_db_rec_t *head = object_db->head;
    if(head == object_rec){
        object_db->head = object_rec->next;
        free(object_rec);
        return;
    }

    /* find the previous node of object_rec */
    object_db_rec_t *prev = head;
    head = head->next; /* exclude head */
    while(head){
        if(head != object_rec){
            prev = head;
            head = head->next;
            continue;
        }

        prev->next = head->next;
        head->next = NULL;
        free(head);
        return;
    }
}

void *xcalloc(object_db_t *object_db, char *struct_name, int units)
{
    struct_db_rec_t *struct_rec = struct_db_lookup(object_db->struct_db, struct_name);
    assert(struct_rec);
    void *ptr = calloc(units, struct_rec->ds_size);
    add_object_to_object_db(object_db, ptr, units, struct_rec, GCOL_FALSE);  /*xcalloc by default set the object as non-root*/
    return ptr;
}

void xfree(object_db_t *object_db, void *ptr){

    if(!ptr) return;
    object_db_rec_t *object_rec = 
        object_db_lookup(object_db, ptr);
        
    assert(object_rec);
    assert(object_rec->ptr);
    free(object_rec->ptr);
    object_rec->ptr = NULL;
    /*Delete object record from object db*/
    delete_object_record_from_object_db(object_db, object_rec);
}


/*APIs to register root objects*/
void gcol_register_global_object_as_root(object_db_t *object_db, 
    void *objptr, 
    char *struct_name, 
    unsigned int units)
{
    struct_db_rec_t *struct_rec = struct_db_lookup(object_db->struct_db, struct_name);
    assert(struct_rec);

    /* create a new object record and add to obejct database */
    add_object_to_object_db(object_db, objptr, units, struct_rec, GCOL_TRUE);
}

void gcol_set_dynamic_object_as_root(object_db_t *object_db, void *obj_ptr)
{
    object_db_rec_t *obj_rec = object_db_lookup(object_db, obj_ptr);
    assert(obj_rec);

    obj_rec->is_root = GCOL_TRUE;
}

void gcol_set_object_as_global_root(object_db_t *object_db, void *obj_ptr)
{

}

static void init_gcol_algorithm(object_db_t *object_db){

    /* set the "is_visited" flag of all the obj records to false */
    object_db_rec_t *obj_rec = object_db->head;
    for(; obj_rec; obj_rec = obj_rec->next){
        obj_rec->is_visited = GCOL_FALSE;
    }
}

static object_db_rec_t *get_next_root_object(object_db_t *object_db, object_db_rec_t *start_obj_rec)
{
    object_db_rec_t *start = start_obj_rec ? start_obj_rec : object_db->head;
    object_db_rec_t *curr = start;
    for(; curr; curr = curr->next){
        if(curr->is_root == GCOL_TRUE)
            return curr;
    }
    return NULL;
}

static void gcol_explore_objects_recursively(object_db_t *object_db, object_db_rec_t *parent_obj_rec){

    unsigned int i = 0;
    unsigned int n_fields;
    char *parent_obj_ptr = NULL;
    char *child_obj_offset = NULL;
    void *child_object_address = NULL;
    field_info_t *field_info = NULL;
    struct_db_rec_t *parent_struct_rec = NULL;
    object_db_rec_t *child_object_rec = NULL;

    parent_struct_rec = parent_obj_rec->struct_rec;
    assert(parent_obj_rec->is_visited);

    if(parent_struct_rec->n_fields == 0)
        return;
    
    for(i = 0; i < parent_obj_rec->units; i++){

        parent_obj_ptr = (char *)parent_obj_rec->ptr + (i * parent_struct_rec->ds_size);

        for(n_fields = 0; n_fields < parent_struct_rec->n_fields; n_fields++){

            field_info = &parent_struct_rec->fields[n_fields];

            /* we consider only pointers to other objects */
            switch(field_info->dtype){

                case UINT8:
                case UINT32:
                case INT32:
                case CHAR:
                case FLOAT:
                case DOUBLE:
                case OBJ_STRUCT:
                    break;
                case VOID_PTR:
                case OBJ_PTR:
                default:
                    ;          
                
                /*child_obj_offset is the memory location inside parent object
                 * where address of next level object is stored*/
                child_obj_offset = parent_obj_ptr + field_info->offset;
                memcpy(&child_object_address, child_obj_offset, sizeof(void *));

                /*child_object_address now stores the address of the next object in the
                 * graph. It could be NULL, Handle that as well*/
                if(!child_object_address) continue;

                child_object_rec = object_db_lookup(object_db, child_object_address);

                assert(child_object_rec);
                /* Since we are able to reach this child object "child_object_rec" 
                 * from parent object "parent_obj_ptr", mark this
                 * child object as visited and explore its children recirsively. 
                 * If this child object is already visited, then do nothing - avoid infinite loops*/
                if(!child_object_rec->is_visited){
                    child_object_rec->is_visited = GCOL_TRUE;
                    if(field_info->dtype != VOID_PTR) /*Explore next object only when it is not a VOID_PTR*/
                        gcol_explore_objects_recursively(object_db, child_object_rec);
                }
                else{
                    continue; /*Do nothing, explore next child object*/
                }
            }
        }
    }
}

/*APIs for Garbage Collector Algorithm*/
void run_gcol_algorithm(object_db_t *object_db)
{
    /* initialize by clearing all the "is_visited" flags */
    init_gcol_algorithm(object_db);

    /* get the first root object from the object database */
    object_db_rec_t *root_obj = get_next_root_object(object_db, NULL);
    while(root_obj){

        if(root_obj->is_visited){
            /* This means all the reachable from this root node are already expolred */
            root_obj = get_next_root_object(object_db, root_obj->next);
            continue;
        }
        /* root objects are always reachable as object rec holds direct reference to it */
        root_obj->is_visited = GCOL_TRUE;

        /* explore all reachable objects from this root_obj recursively */
        gcol_explore_objects_recursively(object_db, root_obj);

        root_obj = get_next_root_object(object_db, root_obj->next);
    }
}

void report_leaked_objects(object_db_t *object_db)
{

}

void  gcol_init_primitive_data_types_support(struct_db_t *struct_db)
{
    
}