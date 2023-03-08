#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

typedef struct {
  int argCount;
  int args[32];
  } MACRO;

char  program[65535];
int   stack[65535];
int   vars[65535];
int   lstack[256];
int   macros[52];
int   pstack[1024];
MACRO mstack[1024];
int   msp;
int   psp;
int   sp;
int   pc;
int   lsp;
int   trace;

void load(char* filename) {
  int   i;
  int   isQt;
  int   isNum;
  char  line[1024];
  FILE *file;
  int pos;
  pos = 0;
  isNum = 0;
  isQt = 0;
  for (i=0; i<52; i++) macros[i] = -1;
  file = fopen(filename,"r");
  if (file == NULL) {
    printf("Error opening %s\n", filename);
    exit(1);
    }
  while (!feof(file)) {
    if (fgets(line, 1023, file) != NULL) {
      for (i=0; i<strlen(line); i++) {
        if (line[i] == ' ') {
          if (isQt) program[pos++] = ' ';
          else if (isNum) {
            program[pos++] = ' ';
            isNum = 0;
            }
          }
        else if (line[i] == '"') {
          if (isNum) isNum = 0;
          program[pos++] = '"';
          isQt ^= 0xff;
          }
        else if (pos > 0 && program[pos-1] == '$' &&
                 line[i] >= 'a' && line[i] <= 'z') {
          program[pos++] = line[i];
          macros[26 + line[i] - 'a'] = pos;
          }
        else if (pos > 0 && program[pos-1] == '$' &&
                 line[i] >= 'A' && line[i] <= 'Z') {
          program[pos++] = line[i];
          macros[line[i] - 'A'] = pos;
          }
        else if (line[i] >= '0' && line[i] <= '9') {
          if (isQt) program[pos++] = line[i];
          else {
            program[pos++] = line[i];
            isNum = 0xff;
            }
          }
        else if (line[i] == '~') {
          i = strlen(line);
          }
        else if (line[i] < ' ') {
          isNum = 0;
          }
        else {
          program[pos++] = line[i];
          isNum = 0;
          }
        }
      }
    }
  fclose(file);
  program[pos++] = '$';
  program[pos] = 0;
// printf("%s\n\n",program);
  }

void run() {
  int  m;
  int  count;
  char cmd;
  char buffer[32];
  int isNum;
  isNum = 0;
  pc = 0;
  sp = 0;
  lsp = 0;
  psp = 0;
  msp = 0;
  trace = 0;
  while (1) {
    cmd = program[pc++];
    if (cmd < '0' || cmd > '9') isNum = 0;
    if (trace && cmd >= ' ' & cmd < 127) { printf("[%c]",cmd); fflush(stdout); }
    if (cmd == 0) return;

    else if (cmd == ' ') {
      isNum = 0;
      }

    else if (cmd == '!') {
      if (sp == 0) {
        printf("Error: Stack empty\n");
        return;
        }
      sp--;
      if (program[pc] == '\'') {
        printf("%c", stack[sp]);
        pc++;
        }
      else {
        printf("%d", stack[sp]);
        }
      }

    else if (cmd == '$') return;

    else if (cmd == '+') {
      if (sp < 2) {
        printf("Error: Stack empty\n");
        return;
        }
      stack[sp-2] += stack[sp-1];
      sp--;
      }

    else if (cmd == '-') {
      if (sp < 2) {
        printf("Error: Stack empty\n");
        return;
        }
      stack[sp-2] -= stack[sp-1];
      sp--;
      }

    else if (cmd == '*') {
      if (sp < 2) {
        printf("Error: Stack empty\n");
        return;
        }
      stack[sp-2] *= stack[sp-1];
      sp--;
      }

    else if (cmd == '/') {
      if (sp < 2) {
        printf("Error: Stack empty\n");
        return;
        }
      stack[sp-2] /= stack[sp-1];
      sp--;
      }

    else if (cmd == '\\') {
      if (sp < 2) {
        printf("Error: Stack empty\n");
        return;
        }
      stack[sp-2] %= stack[sp-1];
      sp--;
      }

    else if (cmd == '<') {
      if (sp < 2) {
        printf("Error: Stack empty\n");
        return;
        }
      stack[sp-2] = (stack[sp-2] < stack[sp-1]) ? 1 : 0;
      sp--;
      }

    else if (cmd == '>') {
      if (sp < 2) {
        printf("Error: Stack empty\n");
        return;
        }
      stack[sp-2] = (stack[sp-2] > stack[sp-1]) ? 1 : 0;
      sp--;
      }

    else if (cmd == '=') {
      if (sp < 2) {
        printf("Error: Stack empty\n");
        return;
        }
      stack[sp-2] = (stack[sp-2] == stack[sp-1]) ? 1 : 0;
      sp--;
      }

    else if (cmd == '.') {
      if (sp < 1) {
        printf("Error: Stack empty\n");
        return;
        }
      stack[sp-1] = vars[stack[sp-1]];
      }

    else if (cmd == ':') {
      if (sp < 2) {
        printf("Error: Stack empty\n");
        return;
        }
      vars[stack[sp-1]] = stack[sp-2];
      sp -= 2;
      }

    else if (cmd == '\'') {
      if (program[pc] == 0) {
        printf("Unexpected end of program\n");
        exit(1);
        }
      stack[sp++] = program[pc++];
      }

    else if (cmd >= 'A' && cmd <= 'Z') {
      stack[sp++] = cmd - 'A';
      }

    else if (cmd >= 'a' && cmd <= 'z') {
      stack[sp++] = 26 + cmd - 'a';
      }

    else if (cmd >= '0' && cmd <= '9') {
      if (isNum) {
        stack[sp-1] = (stack[sp-1] * 10) + (cmd - '0');
        }
      else {
        stack[sp++] = cmd - '0';
        }
      isNum = 0xff;
      }

    else if (cmd == '"') {
      while (program[pc] != 0 && program[pc] != '"') {
        if (program[pc] == '!') {
          printf("\n");
          pc++;
          }
        else printf("%c",program[pc++]);
        }
      if (program[pc] == '"') pc++;
      }

    else if (cmd == '[') {
      if (sp == 0) {
        printf("Stack empty\n");
        exit(1);
        }
      sp--;
      if (stack[sp] <= 0) {
        count = 1;
        while (count > 0 && program[pc] != 0) {
          if (program[pc] == '|' && count == 1) { count--; pc++; }
          else if (program[pc] == '[') { count++; pc++; }
          else if (program[pc] == ']') { count--; pc++; }
          else pc++;
          }
        }
      }

    else if (cmd == ']') ;

    else if (cmd == '|') {
      count = 1;
      while (count > 0 && program[pc] != 0) {
        if (program[pc] == '[') { count++; pc++; }
        else if (program[pc] == ']') { count--; pc++; }
        else pc++;
        }
      }

    else if (cmd == '(') {
      lstack[lsp++] = pc;
      }

    else if (cmd == ')') {
      if (lsp == 0) {
        printf("Lstack empty\n");
        exit(1);
        }
      pc = lstack[lsp-1];
      }

    else if (cmd == '^') {
      if (sp == 0) {
        printf("Stack empty\n");
        exit(1);
        }
      if (lsp == 0) {
        printf("Lstack empty\n");
        exit(1);
        }
      sp--;
      if (stack[sp] == 0) {
        count = 1;
        while (count > 0 && program[pc] != 0) {
          if (program[pc] == '(') { count++; pc++; }
          else if (program[pc] == ')') { count--; pc++; }
          else pc++;
          }
        lsp--;
        }
      }

    else if (cmd == '?') {
      printf("? ");
      fgets(buffer, 31, stdin);
      if (program[pc] == '\'') {
        stack[sp++] = buffer[0];
        pc++;
        }
      else stack[sp++] = atoi(buffer);
      }

    else if (cmd == '#') {
      if (program[pc] >= 'a' && program[pc] <= 'z') m = 26 + program[pc++] - 'a';
      else if (program[pc] >= 'A' && program[pc] <= 'Z') m = program[pc++] - 'A';
      else {
        printf("Invalid character following #\n");
        exit(1);
        }
      if (macros[m] < 0) {
        printf("Call to undefined macro\n");
        exit(1);
        }
      mstack[msp].argCount = 0;
      while (program[pc] != 0 && program[pc] != ';') {
        if (program[pc] == ',') {
          mstack[msp].args[mstack[msp].argCount++] = pc+1;
          }
        pc++;
        }
      if (program[pc] != ';') {
        printf("Unexpected end of program\n");
        exit(1);
        }
      pc++;
      msp++;
      pstack[psp++] = pc;
      pc = macros[m];
      }

    else if (cmd == '%') {
      if (msp == 0) {
        printf("Parameter called outside of macro\n");
        exit(1);
        }
      if (sp == 0) {
        printf("Stack empty\n");
        exit(1);
        }
      m = stack[--sp];
      if (m == 0) {
        printf("Invalid parameter reference\n");
        exit(1);
        }
      if (m > mstack[msp-1].argCount) {
        printf("Nonexistant parameter referenced\n");
        exit(1);
        }
      pstack[psp++] = pc;
      pc = mstack[msp-1].args[m-1];
      }

    else if (cmd == '@') {
      if (msp == 0) {
        printf("Invalid return\n");
        exit(1);
        }
      if (psp == 0) {
        printf("Invalid return\n");
        exit(1);
        }
      msp--;
      pc = pstack[--psp];
      }

    else if (cmd == ',' || cmd == ';') {
      if (msp == 0) {
        printf("Invalid return\n");
        exit(1);
        }
      if (psp == 0) {
        printf("Invalid return\n");
        exit(1);
        }
      pc = pstack[--psp];
      }

    else if (cmd == '{') {
      trace = 0xff;
      }

    else if (cmd == '}') {
      trace = 0;
      }

    else {
      printf("Unknown command encountered: %c\n",cmd);
      return;
      }
    }
  }

int main(int argc, char **argv) {
  int i;
  for (i=1; i<argc; i++) {
    if (strcmp(argv[i], "-v") == 0) {
      printf("Rc/Mouse-83 v1.0\n");
      printf("by Michael H. Riley\n\n");
      }
    else {
      load(argv[i]);
      run();
      }
    }
  exit(0);
  }

