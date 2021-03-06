#include <stdlib.h>
#include <stdio.h>
#include <libxml/parser.h>
#include <libxml/tree.h>

static void print_elements(xmlNode *);

static void
print_elements(xmlNode *node)
{
    xmlNode *n;
    xmlAttrPtr a;
    xmlChar *v, *p, *q;

    for (n = node; n; n = n->next) {
        if (n->type == XML_ELEMENT_NODE) {
            if ((p = xmlGetNodePath(n)) == NULL)
                return;

            for (q = p; *q; q++)
                if (*q == '/')
                    *q = '.';

            for (a = n->properties; a; a = a->next) {
                if ((v = xmlGetProp(n, a->name)) == NULL)
                    return;

                printf("%s.%s=%s\n", p + 1, a->name, v);
                xmlFree(v);
            }

            xmlFree(p);
        }

        print_elements(n->children);
    }
}

int
main(int argc, char **argv)
{
    xmlDoc *doc = NULL;
    xmlNode *root = NULL;

    LIBXML_TEST_VERSION

    if (argc != 2) {
        fprintf(stderr, "usage: %s <file>\n", *argv);
        return EXIT_FAILURE;
    }

    if ((doc = xmlReadFile(argv[1], NULL, 0)) == NULL)
        return EXIT_FAILURE;

    if ((root = xmlDocGetRootElement(doc)) == NULL)
        return EXIT_FAILURE;

    print_elements(root);

    xmlFreeDoc(doc);
    xmlCleanupParser();

    return EXIT_SUCCESS;
}
