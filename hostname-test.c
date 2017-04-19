#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

char oldname[65];
char name[256];

int main() {
  memset(name, 'x', 256);
  if(!gethostname(oldname, 65))
    printf("Saving your current hostname (%s)...\n",oldname);
  if(!sethostname(name, 64))
    printf("64 bytes works :)\n");
  if(sethostname(name, 65))
    perror("65 bytes is too long");
  if(sethostname(name, 255))
    perror("255 bytes is too long");
  if(!gethostname(name, 255))
    printf("Hostname successfully set to %s (%d bytes)\n", name, strlen(name));
  if(!sethostname(oldname, strlen(oldname)))
    printf("Restored your hostname to %s\n", oldname);
  return 0;
}
