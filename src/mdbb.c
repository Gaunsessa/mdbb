#include "mdbb.h"

/* 
   TODO: Make everything inbetween quick link non ident
   split the parse function into each ident type
*/

void mdbb_init(void) {
   MDBB_INLINE_IDENTS = hs_new_s(wchar_t, 
      L"\\", // Escape
      L"*",  // Itatlics
      L"**", // Bold
      L"`",  // Code
      L"<",  // Quick Link
      L">",  // ^
      L"[",  // Links & Images
      L"]",  // ^
      L"(",  // ^
      L")",  // ^
      L"!"   // ^
   );
}

void mdbb_print_inline_stack(mdbb_inline_stack_t *stack, bool rec) {
   if (stack == NULL) {
      printf("STACK: Null\n");

      return;
   }

   printf(
      "STACK: str: %ls, ident: %d, start: %d, parsed: %d\n",
      stack->str,
      stack->ident,
      stack->start,
      stack->_parsed
   );

   if (rec) mdbb_print_inline_stack(stack->next, true);
}

void mdbb_free_inline_stack(mdbb_inline_stack_t *stack, bool rec) {
   if (stack == NULL) return;

   if (stack->str != NULL)
      free(stack->str);

   mdbb_inline_stack_t *next = stack->next;

   free(stack);

   if (rec) mdbb_free_inline_stack(next, true);
}

bool _mdbb_is_inline_ident(wchar_t *inp) {
   dynarr_t(wchar_t) iden = dy_init(wchar_t);
   dy_push(iden, L'\0');

   bool res = false;

   for (int i = 0; i < MDBB_INLINE_IDENT_MAX_SIZE; i++) {
      dy_pop(iden);

      dy_push(iden, inp[i]);
      dy_push(iden, L'\0');

      if (hs_exists_s(MDBB_INLINE_IDENTS, dyi(iden))) {
         res = true;
         break;
      } 
   }

   dy_free(iden);

   return res;
}

mdbb_inline_stack_t *mdbb_gen_inline_stack(wchar_t *inp, mdbb_inline_stack_t *prev) {
   if (inp[0] == L'\0') return NULL;

   mdbb_inline_stack_t *stack = calloc(1, sizeof(mdbb_inline_stack_t));

   dynarr_t(wchar_t) str = dy_init(wchar_t);

   dy_push(str, inp[0]);
   dy_push(str, L'\0');

   int i = 1;

   if (hs_exists_s(MDBB_INLINE_IDENTS, dyi(str))) {
      // Note this wouldnt work if idents were: * and *a*

      while (hs_exists_s(MDBB_INLINE_IDENTS, dyi(str)) && inp[i] != '\0') {
         dy_pop(str);

         dy_push(str, inp[i++]);
         dy_push(str, L'\0');
      }

      if (i > 1) {
         print(i);

         dy_pop(str);
         dy_pop(str);

         dy_push(str, L'\0');

         i--;
      }

      stack->ident = true;
   } else {
      while (!_mdbb_is_inline_ident(inp + i) && inp[i] != L'\0') {
         dy_pop(str);

         dy_push(str, inp[i++]);

         dy_push(str, L'\0');
      }
   }

   stack->str = dy_toarr(str);
   stack->prev = prev;
   stack->next = mdbb_gen_inline_stack(inp + i, stack);

   return stack;
}

void mdbb_parse_inline_stack_escapes(mdbb_inline_stack_t *stack) {
   if (stack == NULL) return;

   if (!stack->ident || stack->str[0] != L'\\') 
      return mdbb_parse_inline_stack_escapes(stack->next);

   if (stack->next->ident) {
      stack->next->ident = false;

      if (stack->prev != NULL)
         stack->prev->next = stack->next;

      mdbb_parse_inline_stack_escapes(stack);
   } else stack->ident = false;

   mdbb_parse_inline_stack_escapes(stack->next);
}

bool _mdbb_parse_inline_stack_close(mdbb_inline_stack_t *stack, wchar_t *ident) {
   if (stack == NULL) return false;

   if (!wcscmp(stack->str, ident) && !stack->_parsed) {
      stack->_parsed = true;

      return true;
   } else return _mdbb_parse_inline_stack_close(stack->next, ident);
}

void mdbb_parse_inline_stack(mdbb_inline_stack_t *stack) {
   if (stack == NULL) return;

   if (!stack->ident)
      stack->_parsed = true;

   if (stack->_parsed)
      return mdbb_parse_inline_stack(stack->next);

   stack->_parsed = true;

   wchar_t *close = NULL;

   switch (stack->str[0]) {
   case L'*':
      if (!wcscmp(stack->str, L"*")) close = L"*";
      else close = L"**";
      break;
   case L'`':
      close = L"`";
      break;
   case L'<':
      close = L">";
      break;
   }

   if (!_mdbb_parse_inline_stack_close(stack, close)) 
      stack->ident = false;

   mdbb_parse_inline_stack(stack->next);
}