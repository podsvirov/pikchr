#include <pikchr.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>

int main()
{
  const char *piktxt = "box \"pikchr\"";
  char *piksvg = pikchr(piktxt, NULL, 0u, NULL, NULL);
  if (piksvg)
  {
    printf("%s\n", piksvg);
    free(piksvg);
    piksvg = NULL;
    return 0;
  }
  return 1;
}
