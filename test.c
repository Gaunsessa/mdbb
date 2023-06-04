#include <stdio.h>
#include <locale.h>

#include "mdbb.h"

#include "hs.h"
#include "print.h"

bool _mdbb_is_inline_ident(wchar_t *inp);

wchar_t *test_str = L"<***ham**bu`rge`r";
// wchar_t *test_str = L"!";

int main(int argc, char **argv) {
   setlocale(LC_ALL, "");

   mdbb_init();

   mdbb_inline_stack_t *stack = calloc(1, sizeof(mdbb_inline_stack_t));
   stack->next = mdbb_gen_inline_stack(test_str, stack);

   // while (stack != NULL) {
   //    printf("%ls\n", stack->str);

   //    stack = stack->next;
   // }

   // printf("%d\n", _mdbb_is_inline_ident(test_str));

   mdbb_parse_inline_stack_escapes(stack);
   mdbb_parse_inline_stack(stack);

   mdbb_print_inline_stack(stack, true);

   return 0;
}