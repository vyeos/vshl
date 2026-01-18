#include <stdbool.h>
#include <stdio.h>
#include <string.h>

int main() {
  bool end = false;
  char input[1024];

  while (!end) {
    printf("myshell> ");
    scanf("%s", input);
    if (strcmp(input, "exit") == 0) {
      end = true;
    }
  }
  return 0;
}
