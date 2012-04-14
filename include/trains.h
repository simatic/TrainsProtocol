#ifndef _TRAINS_H
#define _TRAINS_H

/**
 * @brief Enumeration used to define booleans.
 */
typedef enum {false, true} bool;

/**
 * @brief Address of a process in the trains protocol
 */
typedef short address;

/**
 * @brief Max length of string which can be filled up by add2str
 */
#define MAX_LEN_ADDRESS_AS_STR 5 /* Address is a short. If we represent this short as an hexadecimal number, it has a maximum of 2 digits. With "0x" prior those 2 d!igits and '\0' after them, we get 5 bytes. */

/**
 * @brief Address of the current process in the train protocol
 */
extern address my_address;

/**
 * @brief Macro to test whether address @a ad is null (i.e. it does not correspond to any existing process) or not
 */
#define addr_isnull(ad) ((ad) == 0)

/**
 * @brief Macro to test whether address @a a1 is lower or equal to address @a a2
 */
#define addr_cmp(a1,a2) ((a1) <= (a2))

/**
 * @brief Macro to test whether address @a a1 is equal to address @a a2
 */
#define addr_isequal(a1,a2) ((a1) == (a2))

/**
 * @brief Macro to test whether address @a a1 is equal to address @a a2
 */
#define addr_ismine(ad) (addr_isequal((ad),my_address))

/**
 * @brief Maximum number of members in the protocol
 */
#define MAX_MEMB 16

/** 
 * @brief Messages carried by trains protocol
 *
 * The @a payload contains the information carried by the protocol for the application.
 */
typedef struct{
  int            len;                /**< Length of whole message */
  char           payload[];          /**< Payload (i.e. contents) of the message */
} message;

/**
 * @brief Macro to compute the payload of message @a mp
 */
#define payload_size(mp) ((mp)->len - sizeof(mp->len))

/** 
 * @brief View (members, who joined, who departed) of the circuit.
 *
 * Notice that if @a addr_isnull(cv_joined) (respectively @a addr_isnull(cv_departed)) is false, a new 
 * process has joined (respectively left) the list of processes members of the circuit (thus participating 
 * to the trains protocol). In this case, the address found in @a cv_joined (respectively @a cv_departed) 
 * can (respectively cannot) be found in @a cv_members[]
 */
typedef struct {
  short       cv_nmemb;              /**< Number of members */
  address     cv_members[MAX_MEMB];  /**< List of members */
  address     cv_joined;             /**< New member, if any */
  address     cv_departed;           /**< Departed member, if any */
} circuitview;

extern int tr_error;

/** 
 * @brief Type of function called by trains middleware when there is a change in circuit members
 */
typedef  void (*CallbackCircuitChange)(circuitview*);

/** 
 * @brief Type of function called by trains middleware when it is ready to uto-deliver a message to the application layer
 */
typedef  void (*CallbackUtoDeliver)(address,message*);

/**
 * @brief Strore the error number specific to errors in trains middleware
 */
extern int tr_errno;

/**
 * @brief Stores in @a s the (null-terminated) string representation of @a a
 * @param[in,out] s Array of chars of at least MAX_LEN_ADDRESS_AS_STR bytes 
 * @param[in] ad Address to convert
 * @return @a s
 */
char *addr_2_str(char *s, address ad);

/**
 * @brief Request for a pointer on a new message with a payload of size @a payloadSize
 * @param[in] payloadSize Size requested for the @a payload field of the returned message
 * @return pointer on a message upon successful completion, or NULL if an error occurred (in which case, @a tr_errno is set appropriately)
 */
message *newmsg(int payloadSize);

/**
 * @brief Prints an error message

 * Same as gnu @a error_at_line function (see man error_at_line) except that 1) @a errnum is searched in trains middleware specific errors and 2) parameters behind @a format are not taken into account
 * @param[in] status same role as in @a error_at_line
 * @param[in] errnum same role as in @a error_at_line except that @a tr_error_at_line uses the string given by @a tr_perror(errnum)
 * @param[in] filename same role as in @a error_at_line
 * @param[in] linenum same role as in @a error_at_line
 * @param[in] format same role as in @a error_at_line
 */
void tr_error_at_line(int status, int errnum, const char *filename, unsigned int linenum, const char *format);

/**
 * @brief Initialization of trains protocol middleware
 * @param[in] callbackCircuitChange Function to be called when there is a circuit changed (Arrival or departure of a process)
 * @param[in] callbackUtoDeliver    Function to be called when a message can be uto-delivered by trains protocol
 * @return 0 upon successful completion, or -1 if an error occurred (in which case, @a tr_errno is set appropriately)
 */
int tr_init(CallbackCircuitChange callbackCircuitChange, CallbackUtoDeliver callbackUtoDeliver);

/**
 * @brief Prints (trains middleware specific) error message
 * @param[in] errnum Error number of the message to be printed
 */
void tr_perror(int errnum);

/**
 * @brief uto-broadcast of message @a mp
 * @param[in] mp Message to be uto-broadcasted
 * @return 0 upon successful completion, or -1 if an error occurred (in which case, @a tr_errno is set appropriately)
 */
int uto_broadcast(message *mp);

/**
 * @brief Termination of trains protocol middleware
 * @return 0 upon successful completion, or -1 if an error occurred (in which case, @a tr_errno is set appropriately)
 */
int tr_terminate();

#endif
