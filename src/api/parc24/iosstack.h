#pragma once

#include <ptypes.h>

struct iosstack;
typedef struct iosstack* IOsStack;

/**
 * @return @produces stack 
 */
IOsStack iosstack_new(void);

/**
 * Closes all streams and destroys the stack
 *  
 * @param stack @consumes 
 */
void iosstack_destroy(IOsStack stack);

/**
 * Creates a [duplicate of the] snapshot of the current state of the stack
 * 
 * @param stack @ref 
 * @return @produces stack 
 */
IOsStack iosstack_snapdup(IOsStack stack);

/**
 * Pushes state onto the stack
 *  
 * @param stack @refmut
 */
Result iosstack_push(IOsStack stack);

/**
 * Reverts all operations after previously pushed state
 *  
 * @param stack @refmut
 */
Result iosstack_pop(IOsStack stack);

/**
 * Opens a stream.
 * If there's already a stream there, it is closed.
 * 
 * @param stack @refmut 
 * @param io [virtual] io id
 * @param stream @consumes [real] data stream
 * @return Result 
 */
Result iostack_io_open(IOsStack stack, fd_t io, fd_t stream);

/**
 * Closes a stream.
 * 
 * @param stack @refmut 
 * @param io [virtual] io id
 * @return Result 
 */
Result iostack_io_close(IOsStack stack, fd_t io);

/**
 * Copies a stream.
 * The [real] data stream is copied ["by reference"] - meaning closing one will close the other.
 * 
 * @param stack @refmut
 * @param iodst [virtual] io destination id
 * @param iosrc [virtual] io source id
 * @return Result 
 */
Result iosstack_io_copy(IOsStack stack, fd_t iodst, fd_t iosrc);

/**
 * Duplicates a stream
 * The [real] data stream is duplicated ["by value"] - meaning closing one will _not_ close the other.
 * 
 * @param stack @refmut
 * @param iodst [virtual] io destination id
 * @param iosrc [virtual] io source id
 * @return Result 
 */
Result iosstack_io_dup(IOsStack stack, fd_t iodst, fd_t iosrc);
