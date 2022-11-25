#include <stdio.h>

/* count lines, words, and characters in input                         */

// NOTE:  In C and C++, if two C strings appear adjacent, they are
//        automatically concatenated together.
const char *text = "This is a sample text.  The Khoury office is 201 WVH,\n"
     "and the university address is 360 Huntington Ave., Boston, MA 02115.\n";

// Global variables:
int ndigit[10];
int nwhite;
int nother;

main()
{
    int c, i;
    nwhite = nother = 0;
    for (i = 0; i < 10; ++i) {
        ndigit[i] = 0;
    }
    i = 0;
    while ((c = text[i++]) != '\0') {
        if (c >= '0' && c <= '9')
            ++ndigit[c-'0'];
        else if (c == ' ' || c == '\n' || c == '\t')
            ++nwhite;
        else
            ++nother;
    }
    printf("This program counts the number of occurences of each digit,\n"
           "and of each white space character, and each other character.\n\n");
    printf("TEXT:\n%s\n", text);
    printf("digits =");
    for (i = 0; i < 10; ++i)
        printf(" %d", ndigit[i]);
    printf(", white space = %d, other = %d\n",
        nwhite, nother);
    return 0;
}
