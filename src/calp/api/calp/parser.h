#pragma once

/**
 * @file parser.h
 * Parser type definition.
 * 
 */

struct parser;
typedef struct parser* Parser;

/**
 * @param parser @consumes 
 */
void parser_destroy(Parser parser);
