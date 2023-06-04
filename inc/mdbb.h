#ifndef MDBB_H
#define MDBB_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stddef.h>

#include <wchar.h>

#include <dynarr.h>
#include <hs.h>
#include <print.h>

#define MDBB_INLINE_IDENT_MAX_SIZE 2

typedef struct mdbb_inline_stack_t {
   bool ident;
   bool start;

   wchar_t *str;

   struct mdbb_inline_stack_t *prev;
   struct mdbb_inline_stack_t *next;

   // Used for parsing
   bool _parsed;
} mdbb_inline_stack_t;

hs_t *MDBB_INLINE_IDENTS;

void mdbb_init(void);

void mdbb_print_inline_stack(mdbb_inline_stack_t *stack, bool rec);
void mdbb_free_inline_stack(mdbb_inline_stack_t *stack, bool rec);

mdbb_inline_stack_t *mdbb_gen_inline_stack(wchar_t *inp, mdbb_inline_stack_t *prev);

void mdbb_parse_inline_stack_escapes(mdbb_inline_stack_t *stack);
void mdbb_parse_inline_stack(mdbb_inline_stack_t *stack);

#endif