#pragma once

#include <calp/ptypes.h>

struct buffer {
	size_t cap;
	size_t size;
	char* data;
};
/**
 * Self-managing null terminated binary/char buffer with length information.
 */
typedef struct buffer* Buffer;

/**
 * @produces buffer
 */
Buffer buffer_new(size_t inicap);
#define buffer_empty() buffer_new(0)

/**
 * Destroys the buffer entirely
 * @consumes buffer
 */
void buffer_destroy(Buffer buffer);

/**
 * Destroys the buffer, but not the data, returning contents of the buffer.
 * @consumes buffer
 * @produces string
 * @returns contents of the destroyed buffer
 */
string_mut buffer_destr(Buffer buffer);

/**
 * @ref str
 * @nullable str
 * @produces buffer
 */
Buffer buffer_new_from(string str, ssize_t len);

/**
 * @refmut buffer
 */
Result buffer_resize(Buffer buffer, size_t newcap);

/**
 * @param from first byte of the deleted range in buffer
 * @param toe first byte after the deleted range in buffer
 * @refmut buffer
 * @ref data
 * @nullable data
 */
Result buffer_splice(Buffer buffer, size_t from_i, size_t to_e, string ins, ssize_t len);
#define buffer_splice_str(buffer, from_i, to_e, ins) buffer_splice(buffer, from_i, to_e, ins, -1)
#define buffer_splice_p(buffer, from_p_i, to_p_e, ins, len) buffer_splice(buffer, from_p_i-buffer->data, to_p_e-buffer->data, ins, len)
#define buffer_splice_p_str(buffer, from_p_i, to_p_e, ins) buffer_splice_p(buffer, from_p_i, to_p_e, ins, -1)
#define buffer_splice_inc(buffer, from_i, to_i, ins, len) buffer_splice(buffer, from_i, to_i+1, ins, len)

#define buffer_append(buffer, app, len) buffer_splice(buffer, buffer->size, buffer->size, app, len)
#define buffer_append_str(buffer, app) buffer_append(buffer, app, -1)

#define buffer_delete(buffer, from_i, to_e) buffer_splice(buffer, from_i, to_e, NULL, 0)
#define buffer_delete_p(buffer, from_p_i, to_p_e) buffer_delete(buffer, from_p_i-buffer->data, to_p_e-buffer->data)
