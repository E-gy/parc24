#pragma once

#define null NULL

#define new(_type, _var) _type _var = malloc(sizeof(*_var)); if(!_var) return null;
