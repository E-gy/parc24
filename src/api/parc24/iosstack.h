#pragma once

#include <ptypes.h>

struct iosstack;
#ifndef _IOSSTACK_TDEF
#define _IOSSTACK_TDEF
typedef struct iosstack* IOsStack;
#endif

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

/**
 * @param stack @ref 
 * @param io [virtual]
 * @return 
 */
bool iosstack_raw_has(IOsStack stack, fd_t io);

/**
 * @unsafe
 * 
 * @param stack @ref 
 * @param io [virtual]
 * @return [real] 
 */
fd_t iosstack_raw_get(IOsStack stack, fd_t io);

/**
 * @unsafe
 * 
 * @safe iff used before any managed operation _and_ `io` is unused _and_ ownership of `stream` is transferred to the stack
 * 
 * @param stack @refmut
 * @param io [virtual]
 * @param stream [real]
 */
void iosstack_raw_set(IOsStack stack, fd_t io, fd_t stream);

/**
 * @param stack @ref 
 * @param consumer 
 * @param _
 * @return Result 
 */
Result iosstack_foreach(IOsStack stack, Result (*consumer)(/** [virtual] */ fd_t io, /** [real] */ fd_t stream, void* _), void* _);
