#include <stdio.h>
#include <cstdlib>
#include <string.h>

int main()
{
        char fileOutput[64];

        int operationNumber = 0;

        // While we don't encounter a failure
        while (scanf("%s", fileOutput) != -1)
        {
            // Increment the operation number
            if (strcmp(fileOutput, "successful.") == 0 || strcmp(fileOutput, "failed.") == 0)
            {
                operationNumber++;
            }
        }

        printf("Program failed on operation %d\n", operationNumber);

        return 0;
}