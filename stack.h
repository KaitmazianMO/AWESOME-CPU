#define _CRT_SECURE_NO_WARNINGS

#include <limits.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>


#ifndef NO_PROTECTION
    //! The macro code will disappear from the program in the mode NO_PROTECTION.
    #define ON_PROTECTION_MODE(...)  __VA_ARGS__      
#else
    #define ON_PROTECTION_MODE(...)      
#endif

#ifndef NO_LOG
    //! The macro code will disappear from the program in the mode NO_LOG.
    #define ON_LOG_MODE(...)    __VA_ARGS__
#else
    #define ON_LOG_MODE(...)      
#endif 

#ifndef ANOTHER_STACK
    //! The macro code will disappear from the program if there is more then only one type stack.
    #define ON_FIRST_RUN(...)   __VA_ARGS__ 
#else
    #define ON_FIRST_RUN(...)       
#endif



//*****************************************************************************
//*****************************************************************************
//
//  Declaring constants and structures.
//
//*****************************************************************************
//*****************************************************************************



#ifndef LOG_FILE_NAME
    #define LOG_FILE_NAME  "LogStack.txt"      
#endif

ON_FIRST_RUN (   
               typedef long long canary_t;     

               static FILE*      LOG_FILE_PTR = fopen(LOG_FILE_NAME, "w");
               static int        stk_err      = 0;    
               static const char POISON       = "POISON";    //!< Variable value for creating stack poison.
             )

#ifndef stack_t 
#define stack_t double
#endif 

//! Stack struct declaration.
#define cat( struct_, separator, type )  struct_##separator##type   
#define declare( struct_, type )         cat (struct_, _, type)     
#define stack                            declare (stack, stack_t)   


struct stack
    {
    ON_PROTECTION_MODE ( canary_t frontCanary; )       

    ON_PROTECTION_MODE ( int stack_hash;       )     
    ON_PROTECTION_MODE ( int stack_data_hash ; )   

    stack_t *data;                                   
    stack_t  poison;
    int      size;                                  
    int      capacity;                                

    ON_PROTECTION_MODE ( canary_t backCanary;  )      
    };


ON_FIRST_RUN (

//{----------------------------------------------------------------------------
//!  Errors, that can be detected when you working with the stack.
//!  Errors with num from 0 to 47 can happend by stack fucntions
//!  errors and stk_err takes their value.
//!  Other errors  danote some unusual situations, like random stack changes.
//}----------------------------------------------------------------------------
               enum errors                               
                   {                                     
                   NULL_STACK_PTR          = 60,         
                   NULL_STACK_DATA_PTR     = 61,         
                   CAPACITY_LESS_THAN_SIZE = 62,         
                   NEGATIVE_CAPACITY       = 63,         
                   NEGATIVE_SIZE           = 64,         
                                                         
                   FRONT_STACK_CANARY_ERROR  = 71,
                   BACK_STACK_CANARY_ERROR   = 72,
                   FRONT_DATA_CANARY_ERROR   = 73,
                   BACK_DATA_CANARY_ERROR    = 74,
                   STACK_HASH_ERROR          = 75,
                   STACK_DATA_HASH_ERROR     = 76,

                   NO_ERROR            = 0,
                   REALLOCATION_ERROR  = 41,
                   CONSTRUCTING_ERROR  = 42,
                   POPPING_EMPTY_STACK = 43,
                   PEEKING_EMPTY_STACK = 44,
                   WRONG_PUSHUNG_VALUE = 45,
                   NEGATIVE_VALUE_SIZE = 46,
                   POISON_ERROR        = 47,
                   };

               struct info
                   {
                   const char *file;
                   const char *func;
                   int         line;
                   const char *param_name;
                   };
             )



//*****************************************************************************
//*****************************************************************************
//
//  Minor revision of the standard functions to simplify the work with logs.
//
//*****************************************************************************
//*****************************************************************************


//!  Macro for stacndart log info.
#define INFO__(stack)  { __FILE__, __func__ , __LINE__, #stack } 

//{----------------------------------------------------------------------------
//!  Sets the params of a previously allocated stack.
//!
//!  @param [in] stack_ptr    - a pointer to the stack;
//!  @param [in] capacity     - new number of elements of the stack;
//1
//!  @note In case of an error, replaces stk_err with the error number.
//!
//!  @warning Dont contruct a already constructed
//!           stack to avoid memory leak.
//!
//!  @return In case of an error - number of error, otherwise NO_ERROR.
//}----------------------------------------------------------------------------
#define stack_ctor(      stack_ptr, capacity )   stack_ctor_      (stack_ptr, capacity, INFO__ (stack_ptr))

//{----------------------------------------------------------------------------
//!  Add an item to the top of the stack.
//!
//!  @param [in] stack_ptr - a pointer to the stack;
//!  @param [in] value     - pushing value;
//!
//!  @note In case of an error, replaces stk_err with the error number.
//!
//!  @return In case of an error - number of error, otherwise NO_ERROR.
//}----------------------------------------------------------------------------
#define stack_push(      stack_ptr, value    )   stack_push_      (stack_ptr, value,    INFO__ (stack_ptr))

//{----------------------------------------------------------------------------
//!  Returns value of the top element of the stack.
//!
//!  @param [in] stack_ptr - a pointer to the stack; 
//!
//!  @note In case of an error, replaces stk_err with the error number.
//!
//!  @return Value of the top element of the stack or POISON in case of an error.
//}----------------------------------------------------------------------------
#define stack_peek(      stack_ptr           )   stack_peek_      (stack_ptr,           INFO__ (stack_ptr))

//{----------------------------------------------------------------------------
//!  Returns value of the top element of the stack and deletes it.
//!
//!  @param [in] stack_ptr - a pointer to the stack; 
//!
//!  @note In case of an error, replaces stk_err with the error number.
//!
//!  @return Value of the top element of the stack or POISON in case of an error.
//}----------------------------------------------------------------------------
#define stack_pop(       stack_ptr           )   stack_pop_       (stack_ptr,           INFO__ (stack_ptr))

//{----------------------------------------------------------------------------
//!  Clears the data of stack.
//!
//!  @param [in] stack_ptr - a pointer to the stack; 
//}----------------------------------------------------------------------------
#define stack_free_data( stack_ptr           )   stack_free_data_ (stack_ptr,           INFO__ (stack_ptr))

//{----------------------------------------------------------------------------
//!  Returnes current size of the stack.
//!                                               
//!  @param [in] stack_ptr - a pointer to the stack;
//}----------------------------------------------------------------------------
#define stack_size(      stack_ptr           )   stack_size_      (stack_ptr,           INFO__ (stack_ptr)) 

//{----------------------------------------------------------------------------
//!  Returnes current capacity of the stack.
//!                                               
//!  @param [in] stack_ptr - a pointer to the stack;
//}----------------------------------------------------------------------------
#define stack_capacity(  stack_ptr           )   stack_capacity_  (stack_ptr,           INFO__ (stack_ptr))

//{----------------------------------------------------------------------------
//!  Delletes all spaces and the data of the stack. 
//!                                               
//!  @param [in] stack_ptr - a pointer to the stack;
//!
//!  @return NULL.
//}----------------------------------------------------------------------------
#define stack_dtor(      stack_ptr           )   stack_dtor_      (stack_ptr,           INFO__ (stack_ptr))

//{----------------------------------------------------------------------------
//!  Changes capacity of stack.
//!
//!  @param [in] stack_ptr    - a pointer to the stack;
//!  @param [in] new_capacity - new number of elements of the stack;
//!  @param [in] size_value   - size of stacks value;
//!
//!  @note The current capacity is not equal to the capacity
//!        in fact it is more than it by 2 * sizeof (canary_t) + 1.
//!
//!  @return In case of an error - number of error, otherwise NO_ERROR.
//}----------------------------------------------------------------------------
#define stack_resize(    stack_ptr, new_size, value_size )    \
    stack_resize_  (stack_ptr, new_size, value_size, INFO (stack_ptr))




//*****************************************************************************
//*****************************************************************************
//
//  Function declaration.
//
//*****************************************************************************
//*****************************************************************************




//-----------------------------------------------------------------------------------------------------------
                                                                                                             //
static int      stack_ctor_          (stack *stack_ptr, int new_capacity, info func_info);                   //
                                                                                                             //
static int      stack_push_          (stack *stack_ptr, stack_t value, info func_info);                      //
                                                                                                             //      
static stack_t  stack_peek_          (stack *stack_ptr, info func_info);                                     //
                                                                                                             //      
static stack_t  stack_pop_           (stack *stack_ptr, info func_info);                                     //      LOGGED
                                                                                                             //
static int      stack_free_data_     (stack *stack_ptr, info func_info);                                     //      FUNCTIONS
                                                                                                             //      
static size_t   stack_size_          (stack *stack_ptr, info func_info);                                     //
                                                                                                             //
static size_t   stack_capacity_      (stack *stack_ptr, info func_info);                                     //
                                                                                                             //
static stack   *stack_dtor_          (stack *stack_ptr, info func_info);                                     //
                                                                                                             //
static int      stack_resize_        (stack *stack_ptr, int new_capacity, int size_value, info func_info);   //
                                                                                                             //
//------------------------------------------------------------------------------------------------------------

//{----------------------------------------------------------------------------
//!  Closes log file =/.
//}----------------------------------------------------------------------------
static int      close_log_file();
                                 
static void     setPoisonValue       (stack_t *poison, size_t psize);
static void     setCanaryValue       (canary_t *canary);
static void     setCanaries          (stack *stack_ptr);

//{----------------------------------------------------------------------------
//!  Sets poison for items coming after size.
//!
//!  @param [in] stack_ptr - a pointer to the stack;
//}----------------------------------------------------------------------------
static void     add_poison           (stack *stack_ptr);

//{----------------------------------------------------------------------------
//!  Returnes hash of stack from stack_ptr.
//}----------------------------------------------------------------------------
static int      get_stack_hash       (stack *stack_ptr);

//{----------------------------------------------------------------------------
//!  Returnes hash of stacks data from stack_ptr.
//}----------------------------------------------------------------------------
static int      get_stack_data_hash  (stack *stack_ptr);

//{----------------------------------------------------------------------------
//! Checks is all elements after size are POISON.
//}----------------------------------------------------------------------------
static int      poison_error         (stack *stack_ptr);

//{----------------------------------------------------------------------------
//!  Returnes errors of stacks spaces is case of an error, otherwise NO_ERROR.
//}----------------------------------------------------------------------------
static int      stack_error          (stack *stack_ptr);

//{----------------------------------------------------------------------------
//!  Returnes canary errors is case of an error, otherwise NO_ERROR.
//}----------------------------------------------------------------------------
static int      canary_error         (stack *stack_ptr);

//{----------------------------------------------------------------------------
//!  Returnes hash errors is case of an error, otherwise NO_ERROR.
//}----------------------------------------------------------------------------
static int      hash_error           (stack *stack_ptr);

//{----------------------------------------------------------------------------
//!  Returnes all erros that can happend with the stack
//!  is case of error, otherwise NO_ERROR.
//}----------------------------------------------------------------------------
static int      stack_verify         (stack *stack_ptr);


//{----------------------------------------------------------------------------
//!  Returns a string with the input number of the error.
//!
//!  @param [in] error - number of error;
//!
//!  @return A pointer to a string wiht number of the error.
//}----------------------------------------------------------------------------
static const char *str_error         (int error);


//{----------------------------------------------------------------------------
//!  Prints a line in the file.
//!
//!  @param [in] file - a pointer to a file to print;
//}----------------------------------------------------------------------------
static void        print_line        (FILE * file);

//{----------------------------------------------------------------------------
//!  Verifies input canary.
//!
//!  @param [in] canary - a canary for verifying;
//!
//!  @return true if canary is OK, otherwise false.
//}----------------------------------------------------------------------------

static bool        canary_value_error (canary_t *canary);

static int         stack_data_hash_error (stack *stack_ptr);

static int         stack_hash_error (stack *stack_ptr);



//*****************************************************************************
//*****************************************************************************
//
//  Macros for debugging and logging.
//
//*****************************************************************************
//*****************************************************************************



//{----------------------------------------------------------------------------
//!  Handles errors not in NO_DBG mode.
//!
//!  @param [in] error - returned value from function,
//!                      that checks stack errors;
//!
//!  In case of non-zero error, aborts the program
//!  and dumps some info about error at log file (LOG_FILE_NAME). 
//}----------------------------------------------------------------------------
#define Verify(error)                                                                               \
                if ((error))                                                                        \
                    {                                                                               \
                    if (!LOG_FILE_PTR) LOG_FILE_PTR = fopen (LOG_FILE_NAME, "+a");                  \
                    const char *str_err = str_error (error);                                        \
                    print_line (LOG_FILE_PTR);                                                      \
                    print_line (LOG_FILE_PTR);                                                      \
                                                                                                    \
                    fprintf (LOG_FILE_PTR, "Stack <%s> %s (ERROR: %d (%s)) [%p] \n"                 \
                                           "Function: %s\n"                                         \
                                           "Called from file: %s\n"                                 \
                                           "Called from function: %s\n"                             \
                                           "Line: %d\n",                                            \
                                           type_str (stack_t), func_info.param_name, error,         \
                                           str_error (error), stack_ptr, __func__,                  \
                                           func_info.file, func_info.func, func_info.line);         \
                                                                                                    \
                    printf ("\n\n***  FATAL ERROR!"                                                 \
                            "\n\n***  You can find log information in file: %s\n\n", LOG_FILE_NAME);\
                                                                                                    \
                    print_line (LOG_FILE_PTR);                                                      \
                    fflush     (LOG_FILE_PTR);                                                      \
                                                                                                    \
                    abort ();                                                                       \
                    }                                                                               \

//-----------------------------------------------------------------------------

//!  Sets stack hashes. 
#define SetHashes(stack_ptr)  ON_PROTECTION_MODE ( stack_ptr->stack_hash      = get_stack_hash      (stack_ptr); )  \
                              ON_PROTECTION_MODE ( stack_ptr->stack_data_hash = get_stack_data_hash (stack_ptr); )  

//!  Verifies stack at start. 
#define StartVerify           ON_PROTECTION_MODE ( Verify (stack_error  (stack_ptr)) )\
                              ON_PROTECTION_MODE ( Verify (canary_error (stack_ptr)) )\
                              ON_PROTECTION_MODE ( Verify (hash_error   (stack_ptr)) )

//!  Verifies stack at end and sets new hashes.
#define EndVerify             ON_PROTECTION_MODE ( Verify (stack_error  (stack_ptr)) )\
                              ON_PROTECTION_MODE ( Verify (canary_error (stack_ptr)) )\
                              ON_PROTECTION_MODE ( SetHashes            (stack_ptr)  )
   
//-----------------------------------------------------------------------------


#define IS_FLOAT_TYPE(type) (((type)(1 + 0.1)) ==  1.1)
#define CANARY(i)  *(canary_t *)((char *)stack_ptr->data + i*sizeof (stack_t) -        \
                       ((i == 0) ? sizeof(canary_t) : sizeof (stack_t))) 

//!  Prints arbitrary type element.
ON_LOG_MODE (
static void PrintElem (FILE *file, stack *stack_ptr, int i)
    {
    if (i <= stack_ptr->size && i != 0) 
        fprintf (LOG_FILE_PTR, "\r\t*");        
    else
        fprintf (LOG_FILE_PTR, "\r\t ");           

    if (IS_FLOAT_TYPE(stack_t))
        if (i == 0 || i == stack_ptr->capacity + 1)
            {
            ON_PROTECTION_MODE ( 
            fprintf (LOG_FILE_PTR, "[%d] = %lld (CANARY)", i - 1, CANARY (i)); 
                               )
            }

        else if (i > stack_ptr->size)                                                        
            fprintf (LOG_FILE_PTR, "[%d] = %lg  (POISON)", i - 1, (double)*(stack_ptr->data + i - 1));  
            
        else                                                                                 
            fprintf (LOG_FILE_PTR, "[%d] = %lg",           i - 1, (double)*(stack_ptr->data + i - 1));

    else
        if (i == 0 || i == stack_ptr->capacity + 1)
            {
            ON_PROTECTION_MODE (
            fprintf (LOG_FILE_PTR, "[%d] = %lld (CANARY)", i - 1, CANARY (i)); 
                               )
            }
        else if (i > stack_ptr->size)                                                        
            fprintf (LOG_FILE_PTR, "[%d] = %lld (POISON)", i - 1, (long long)*(stack_ptr->data + i - 1));  
            
        else                                                                                 
            fprintf (LOG_FILE_PTR, "[%d] = %lld",          i - 1, (long long)*(stack_ptr->data + i - 1));
    }
          )

#undef CANARY
#undef IS_FLOAT_TYPE

#define str(lexem)      #lexem
#define type_str(type)  str (type)

//! Dumps stack info not in NO_LOG mode.
#define StackDump                                                                                       \
ON_LOG_MODE (                                                                                           \
              {                                                                                         \
              if (LOG_FILE_PTR)                                                                         \
                  {                                                                                     \
                  fprintf (LOG_FILE_PTR, "Stack <%s> %s (OK) [%p] \n"                                   \
                                         "Function: %s\n"                                               \
                                         "Called from file: %s\n"                                       \
                                         "Called from function: %s\n"                                   \
                                         "Line: %d\n",                                                  \
                                          type_str (stack_t), func_info.param_name, stack_ptr,          \
                                          __func__, func_info.file, func_info.func, func_info.line);    \
                                                                                                        \
                  fprintf (LOG_FILE_PTR,                                                                \
                                    "{\n"                                                               \
                                    "\tsize     = %d\n"                                                 \
                                    "\tcapasity = %d\n"                                                 \
                                    "\tdata[%p]\n\t{",                                                  \
                                     stack_ptr->size,                                                   \
                                     stack_ptr->capacity,                                               \
                                     stack_ptr->data);                                                  \
                                                                                                        \
                  for (int i = 0; i <= stack_ptr->capacity + 1; ++i)                                    \
                      PrintElem (LOG_FILE_PTR, stack_ptr, i);                                           \
                                                                                                        \
                  fprintf (LOG_FILE_PTR, "\r\t}\n}\n");                                                 \
                                                                                                        \
                  print_line (LOG_FILE_PTR);                                                            \
                  fflush     (LOG_FILE_PTR);                                                            \
                  }                                                                                     \
                }                                                                                       \
            )                                                                                           \




//*****************************************************************************
//*****************************************************************************
//
//  Function definition.
//
//*****************************************************************************
//*****************************************************************************



static int stack_ctor_ (stack *stack_ptr, int capacity, info func_info)
    {
    if (!stack_ptr)   return stk_err = NULL_STACK_PTR;
    if (capacity < 0) return stk_err = NEGATIVE_CAPACITY;


    char *new_stack_ptr_data  = (char *)calloc (capacity * sizeof (stack_t)  
                                                ON_PROTECTION_MODE ( + 2 * sizeof (canary_t) ),
                                                sizeof (char));

    if (new_stack_ptr_data == NULL)
        return stk_err = CONSTRUCTING_ERROR;
    else
        stack_ptr->data = (stack_t *)(new_stack_ptr_data 
                                      ON_PROTECTION_MODE ( + sizeof (canary_t) ));

    stack_ptr->capacity = capacity;
    stack_ptr->size     = 0;

    ON_PROTECTION_MODE ( setCanaries (stack_ptr); )

    setPoisonValue (&stack_ptr->poison, sizeof (stack_t));
    add_poison (stack_ptr);

    SetHashes  (stack_ptr)
    EndVerify
    StackDump

    return NO_ERROR;
    }

//-----------------------------------------------------------------------------

static int stack_push_ (stack *stack_ptr, stack_t value, info func_info)
    {
    StartVerify
    StackDump

    if (value != value) return stk_err = WRONG_PUSHUNG_VALUE;

    if (stack_ptr->size == stack_ptr->capacity)
        stk_err = stack_resize_ (stack_ptr, stack_ptr->capacity * 2 + 1,
                                            sizeof (*stack_ptr->data),
                                            func_info);
    if ( !stk_err )
        *(stack_ptr->data + stack_ptr->size++) = value;
                                  
    EndVerify
    StackDump

    return NO_ERROR;
    }

//-----------------------------------------------------------------------------

static stack_t stack_pop_ (stack *stack_ptr, info func_info)
    {
    StartVerify
    StackDump

    if (stack_ptr->size == 0) { stk_err = POPPING_EMPTY_STACK; return stack_ptr->poison; }

    if (stack_ptr->capacity > 4 * stack_ptr->size)
        stk_err = stack_resize_ (stack_ptr, stack_ptr->size * 2 + 1, sizeof(stack_t), func_info);

    if (stk_err) return stack_ptr->poison;

    stack_t value = *(stack_ptr->data + --stack_ptr->size);

    *(stack_ptr->data + stack_ptr->size) = stack_ptr->poison;
                                  
    EndVerify
    StackDump

    return value;
    }

//-----------------------------------------------------------------------------

static stack_t stack_peek_ (stack *stack_ptr, info func_info)
    {
    StartVerify
    StackDump

    if (stack_ptr->size == 0) { stk_err = PEEKING_EMPTY_STACK; return stack_ptr->poison; }


    EndVerify
    StackDump

    return *(stack_ptr->data + stack_ptr->size - 1);
    }

//-----------------------------------------------------------------------------

static int stack_resize_ (stack *stack_ptr, int new_capacity, int size_value, info func_info)
    {
    StartVerify

    if (new_capacity < 0) 
        return NEGATIVE_CAPACITY;

    if (size_value < 0)
        return NEGATIVE_VALUE_SIZE;

    char *new_data = (char *)realloc ((char *)stack_ptr->data ON_PROTECTION_MODE ( -   sizeof (canary_t) ),
                                     new_capacity*size_value  ON_PROTECTION_MODE ( + 2*sizeof (canary_t) ));

    if (new_data == NULL)
        return stk_err = REALLOCATION_ERROR;

    else
        {
        stack_ptr->data     = (stack_t *)(new_data ON_PROTECTION_MODE ( + sizeof (canary_t) ));
        stack_ptr->capacity = new_capacity;

        ON_PROTECTION_MODE ( setCanaries (stack_ptr); )
        add_poison (stack_ptr);
        }

    StackDump
    EndVerify
    return NO_ERROR;
    }

//-----------------------------------------------------------------------------

static int stack_free_data_ (stack *stack_ptr, info func_info)
    {
    StartVerify
    StackDump

    stack_resize_ (stack_ptr, 0, sizeof (stack_t), func_info);

    stack_ptr->size = 0;

    EndVerify
    StackDump

    return NO_ERROR;
    }

//-----------------------------------------------------------------------------

static stack *stack_dtor_ (stack *stack_ptr, info func_info)
    {
    StartVerify
    
    free (((char *)stack_ptr->data ON_PROTECTION_MODE ( - sizeof (canary_t) )));
    stack_ptr->data = NULL;
        stack_ptr->size = -1;
    stack_ptr->capacity = -1;

    return NULL;
    }

//-----------------------------------------------------------------------------

ON_FIRST_RUN (
               static int close_log_file () 
                   {
                   return LOG_FILE_PTR && fclose (LOG_FILE_PTR);
                   }            

//-----------------------------------------------------------------------------

               static void setCanaryValue (canary_t *canary) 
                   {
                   for (size_t i = 0; 4*i < sizeof (canary_t); ++i) 
                       *((int *)canary + i) = (int)canary;
                   }
             )

//-----------------------------------------------------------------------------

static void setPoisonValue (stack_t* poison, size_t psize) 
    {
    for (size_t i = 0; i < psize; ++i)
        *((char *)poison + i) = POISON;
    }

static void setCanaries (stack* stack_ptr) {
ON_PROTECTION_MODE (
                     canary_t *frontStackCanary     = &stack_ptr->frontCanary;
                     canary_t *backStackCanary      = &stack_ptr->backCanary;
                     canary_t *frontDataStackCanary = ((canary_t *)stack_ptr->data - 1);
                     canary_t *backDataStackCanary  = (canary_t *)(stack_ptr->data + stack_ptr->capacity);

                     setCanaryValue (frontStackCanary);    
                     setCanaryValue (backStackCanary);      
                     setCanaryValue (frontDataStackCanary); 
                     setCanaryValue (backDataStackCanary); 
                    )
}

//-----------------------------------------------------------------------------

static void add_poison (stack *stack_ptr)
    {
    for (int i = stack_ptr->size; i < stack_ptr->capacity; ++i)
        *(stack_ptr->data + i) = stack_ptr->poison;
    }

//-----------------------------------------------------------------------------

static size_t stack_size_ (stack *stack_ptr, info func_info)
    {
    StartVerify

    return stack_ptr->size;
    }

//-----------------------------------------------------------------------------

static size_t stack_capacity_ (stack *stack_ptr, info func_info)
    {
    StartVerify

    return stack_ptr->capacity;
    }

//-----------------------------------------------------------------------------

static int get_stack_hash (stack *stack_ptr)
    {
    ON_PROTECTION_MODE (
                         int old_stack_hash      = stack_ptr->stack_hash;
                         int old_stack_data_hash = stack_ptr->stack_data_hash;

                         stack_ptr->stack_hash      = 0;
                         stack_ptr->stack_data_hash = 0;
                         int *hash_ptr = (int *)stack_ptr;

                         int new_stack_hash = 0;
    
                         int p = 1, p_pow = INT_MAX;
                         for (size_t i = 0; i < sizeof (*stack_ptr) / sizeof (int); ++i, p *= p_pow)
                             new_stack_hash += *(hash_ptr + i) * p;

                         stack_ptr->stack_data_hash = old_stack_data_hash;
                         stack_ptr->stack_hash      = old_stack_hash;

                         return new_stack_hash + (int)stack_ptr * p;
                       ) 
        return 0;
    }

//-----------------------------------------------------------------------------

static int get_stack_data_hash (stack *stack_ptr)
    {
    ON_PROTECTION_MODE ( 
                         int new_stack_data_hash = 0;
                         stack_t *hash_ptr = stack_ptr->data;

                         int p = 1, p_pow = INT_MAX;
                         for (int i = 0; i < stack_ptr->capacity; ++i, p *= p_pow)
                             new_stack_data_hash += (int)*(hash_ptr + i) * p;

                         return new_stack_data_hash + (int)stack_ptr * p;
                       )
    return 0;
    }

//-----------------------------------------------------------------------------

static int stack_hash_error (stack *stack_ptr)
    {
    ON_PROTECTION_MODE ( 
                         if (stack_ptr->stack_hash != get_stack_hash (stack_ptr))
                             return STACK_HASH_ERROR;
                       )
    return 0;
    }

static int stack_data_hash_error (stack *stack_ptr)
    {
    ON_PROTECTION_MODE ( 
                         if (stack_ptr->stack_data_hash != get_stack_data_hash (stack_ptr))
                             return STACK_DATA_HASH_ERROR;
                       )
    return 0;
    }

//-----------------------------------------------------------------------------

#define catch(condition, err_num) if (condition) return err_num;


static int stack_verify (stack *stack_ptr)
    {  
    int err_num = 0;

    if (err_num = stack_error  (stack_ptr)) return err_num;

    if (err_num = canary_error (stack_ptr)) return err_num;
                                                           
    if (err_num = hash_error   (stack_ptr)) return err_num;

    return NO_ERROR;
    }

static int stack_error (stack *stack_ptr)
    {
    catch (stack_ptr           == NULL,  NULL_STACK_PTR         ); 
    catch (stack_ptr->data     == NULL,  NULL_STACK_DATA_PTR    ); 
    catch (stack_ptr->capacity <  0,     NEGATIVE_CAPACITY      ); 
    catch (stack_ptr->size     <  0,     NEGATIVE_SIZE          ); 
    catch (stack_ptr->capacity <                                               
           stack_ptr->size,              CAPACITY_LESS_THAN_SIZE);
    catch (poison_error (stack_ptr)    , POISON_ERROR           );
    return NO_ERROR;
    }

static int canary_error (stack *stack_ptr)
    {
ON_PROTECTION_MODE (
                     canary_t *frontDataStackCanary = ((canary_t *)stack_ptr->data - 1);
                     canary_t *backDataStackCanary  = (canary_t *)(stack_ptr->data +  stack_ptr->capacity);
                     canary_t *frontStackCanary     = &stack_ptr->frontCanary;
                     canary_t *backStackCanary      = &stack_ptr->backCanary;
         
                     ON_PROTECTION_MODE ( catch (canary_value_error (frontStackCanary),     FRONT_STACK_CANARY_ERROR); )           
                     ON_PROTECTION_MODE ( catch (canary_value_error (backStackCanary),      BACK_STACK_CANARY_ERROR);  )
                     ON_PROTECTION_MODE ( catch (canary_value_error (frontDataStackCanary), FRONT_DATA_CANARY_ERROR);  )
                     ON_PROTECTION_MODE ( catch (canary_value_error (backDataStackCanary),  BACK_DATA_CANARY_ERROR);   )
                   )
    return NO_ERROR;
    }


ON_FIRST_RUN (
               static bool canary_value_error (canary_t *canary)
                   {
                   for (size_t i = 0; 4*i < sizeof (canary_t); ++i)
                       if (*((int *)canary + i) != (int)canary)  return 1;

                   return 0;
                   }
             )

static int hash_error (stack *stack_ptr)
    {
    catch (stack_hash_error      (stack_ptr), STACK_HASH_ERROR     );
    catch (stack_data_hash_error (stack_ptr), STACK_DATA_HASH_ERROR);
    return NO_ERROR;
    }

static int poison_error (stack *stack_ptr)
    {
    for (int i = stack_ptr->size; i < stack_ptr->capacity; ++i)
        if (*(stack_ptr->data + i) != stack_ptr->poison)
            return stk_err = POISON_ERROR;

    return NO_ERROR;
    }

    
#define CASE(err_num)  if(error == err_num) return #err_num;
#define DEFAULT(err_num)  return #err_num;

ON_FIRST_RUN (
               const char *str_error (int error)                              
                   {
                   CASE (NULL_STACK_PTR)
                   CASE (NULL_STACK_DATA_PTR)
                   CASE (NEGATIVE_CAPACITY)
                   CASE (NEGATIVE_SIZE)
                   CASE (CAPACITY_LESS_THAN_SIZE)
                   CASE (FRONT_STACK_CANARY_ERROR)
                   CASE (BACK_STACK_CANARY_ERROR)
                   CASE (FRONT_DATA_CANARY_ERROR)
                   CASE (BACK_DATA_CANARY_ERROR)
                   CASE (STACK_HASH_ERROR)
                   CASE (STACK_DATA_HASH_ERROR)
                   CASE (POISON_ERROR)
                   CASE (REALLOCATION_ERROR)
                   CASE (CONSTRUCTING_ERROR)
                   CASE (POPPING_EMPTY_STACK)
                   CASE (PEEKING_EMPTY_STACK)
                   CASE (WRONG_PUSHUNG_VALUE)
                   CASE (NEGATIVE_VALUE_SIZE)
                   CASE (POISON_ERROR)       
                   CASE (NO_ERROR)

                   DEFAULT (UNKNOWN_ERROR)
                   }

               void print_line(FILE * file)
                   {
                   if (file == NULL) return;

                   fputs ("\n", file);

                   for (int i = 0; i < 100; ++i)
                       fputs ("-", file);

                   fputs ("\n", file);
                   }
             )


#undef CASE
#undef DEFAULT
#undef POISON
#undef catch
#undef cat
#undef declare
#undef cat
#undef declare
#undef stack
#undef LOG_FILE_NAME
#undef stack_t
#undef canary_t
#undef ON_PROTECTION_MODE
#undef NO_LOG
#undef NO_PROTECTION
#undef ONLOG_MODE
#define ANOTHER_STACK    //!< Used by other stack types to avoid double-inclusion errors.