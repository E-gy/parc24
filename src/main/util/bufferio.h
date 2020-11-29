#include "buffer.h"

#include <ptypes.h>

Result_T(buffer_from_result, Buffer, string_v);
#define BufferResult struct buffer_from_result

BufferResult buffer_from_file(string file);

/**
 * @param file @consumes
 * @return BufferResult 
 */
BufferResult buffer_from_fd(fd_t file);
