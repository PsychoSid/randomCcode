// Use some stuff
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#include <rpm/rpmlib.h>

// Here we've defined the program's storage. Note in particular the HeaderIterator, Header, and int_32 datatypes. 
void main(int argc, char ** argv)
{
    HeaderIterator iter;
        Header h, sig;
        int_32 itertag, type, count;
        void **p = NULL;
        char *blather;
        char * name;

        int fd, stat;

        if (argc == 1) {
            fd = 0;
        } else {
            fd = open(argv[1], O_RDONLY, 0644);
        }

        if (fd < 0) {
            perror("open");
            exit(1);
        }

        stat = rpmReadPackageInfo(fd, &sig, &h);
        if (stat) {
            fprintf(stderr,
                "rpmReadPackageInfo error status: %d\n%s\n",
                stat, strerror(errno));
            exit(stat);
        }

        headerGetEntry(h, RPMTAG_NAME, &type, (void **) &name, &count);

        if (headerIsEntry(h, RPMTAG_PREIN)) {
            printf("There is a preinstall script for %s\n", name);
        }

        if (headerIsEntry(h, RPMTAG_POSTIN)) {
            printf("There is a postinstall script for %s\n", name);
        }

        printf("Dumping signatures...\n");
        headerDump(sig, stdout, 1);

        rpmFreeSignature(sig);

        printf("Iterating through the header...\n");

        iter = headerInitIterator(h);

        while (headerNextIterator(iter, &itertag, &type, p, &count)) {
            switch (itertag) {
            case RPMTAG_SUMMARY:
                blather = *p;
                printf("The Summary: %s\n", blather);
                break;
            case RPMTAG_FILENAMES:
                printf("There are %d files in this package\n", count);
                break;
            }
        }

        headerFreeIterator(iter);

        headerFree(h);
}
