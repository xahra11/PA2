#include <stdio.h>
#include <stdlib.h>

typedef struct {
    const char *name; //name of the test
    const char *cmd; //command to run the spell program
    const char *expected_output;   //expected output file or ""
} Test;

int main(void) {

    Test tests[] = {
        //good words, no errors
        { "Test 1: Words correctly found in dictionary",
          "./spell dictionary1.txt testfiles/good.txt > output.txt",
          ""},

        //capitalization rules
        { "Test 2: Capitalization rules",
          "./spell dictionary1.txt testfiles/evil.txt > output.txt",
          "expected/evil.out"},

        //clean words of punctuation 
        { "Test 3: Punctuation rules",
          "./spell dictionary1.txt testfiles/special.txt > output.txt",
          "expected/special.out"
        },
        //directory traversal
        { "Test 4: Recursive directory traversal",
          "./spell dictionary1.txt testfiles/testDir > output.txt",
          "expected/testDir.out"
        },
        //-s suffix
        { "Test 5: Suffix (-s) option",
          "./spell -s .data dictionary1.txt testfiles/testSuffix > output.txt",
          "expected/suffix.out"
        }

        { "Test 6: Standard input reading",
          "cat testfiles/stdin.txt | ./spell dictionary1.txt > output.txt",
          "expected/stdin.out"
        }
    };

    int total = sizeof(tests) / sizeof(Test);
    int passed = 0;

    for (int i = 0; i < total; i++) {
        int ret = system(tests[i].cmd);
        if (ret == -1) {
            perror("system");
            continue;
        }

        if (tests[i].expected_output[0] == '\0') {
            //expect no output
            FILE *f = fopen("output.txt", "r");
            if (!f) { 
                perror("fopen"); 
                continue; 
            }
            fseek(f, 0, SEEK_END);
            long size = ftell(f);
            fclose(f);

            if (size == 0) {
                printf("Passed\n\n");
                passed++;
            } else {
                printf("Failed — expected no output\n\n");
            }
        }
        else {
            //compare output to expected
            char diffCmd[256];
            snprintf(diffCmd, sizeof(diffCmd), "diff -q output.txt %s > /dev/null", tests[i].expected_output);
            int diffResult = system(diffCmd);

            if (diffResult == 0) {
                printf("Passed\n\n");
                passed++;
            } else {
                printf("Failed — output differs from expected\n\n");
            }
        }
    }

    printf("RESULTS: %d / %d tests passed.\n", passed, total);
    return (passed == total) ? EXIT_SUCCESS : EXIT_FAILURE;
}
