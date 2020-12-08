#pragma once

#include "buffer.h"
#include <stdarg.h>

Result buffer_printf(Buffer buff, string fmt,...);
Result buffer_vprintf(Buffer buff, string fmt, va_list args);
