#include <sys/types.h>
#include <attr/xattr.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[])
{
  char *path, *list, *value;
  ssize_t res;

  path = argv[1];
  /* ... add validation */
  list = malloc(BUFSIZ);
  /* ... add validation */
  /* ssize_t listxattr (const char *path, char *list, size_t size); */
  res = listxattr(path /* /bin/ls */, list, BUFSIZ /* of list */);

  value = malloc(BUFSIZ);
  res = getxattr(path, list /* security.selinux */, value, 0);
  memset(value, 0, sizeof(value));
  getxattr(path, list, value, res);
  printf("%s: %s/%s (size: %d)\\n", path, list, value, res);

  /* ... */

  return EXIT_SUCCESS;
}
