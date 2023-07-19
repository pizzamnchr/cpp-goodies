#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <algorithm>

#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char **argv)
{
	// Creating the process id for each child
	pid_t child_process_id_1;

    pid_t child_process_id_2;

    // Creating the statuses for each child
	int child_status_1;
    
    int child_status_2;

	// Creating the four pipes
	int parent_to_child_pipe_1[2], child_to_parent_pipe_1[2], child_to_parent_pipe_2[2], child_1_to_child_2_pipe[2];

    // Creating lists to store the arguments
	int child_list_1[5];

    int child_list_2[5];

    // If any of the pipes contain a -1, exit the program
	if (pipe(parent_to_child_pipe_1) == -1 || pipe(child_to_parent_pipe_1) == -1 || pipe(child_to_parent_pipe_2) == -1 || pipe(child_1_to_child_2_pipe) == -1) 
	{
		fprintf(stderr, "Pipes failed.\n");

        return EXIT_FAILURE;
	}

	// Forking child 1's process
	child_process_id_1 = fork();

    // If child 1's process failed, exit the program
	if (child_process_id_1 < 0)
	{
		fprintf(stderr, "Child 1 failed.\n");

		return EXIT_FAILURE;
	} 
	else if (child_process_id_1 == 0)
	{
		// Closing all the pipes
		close(parent_to_child_pipe_1[1]);

		close(child_to_parent_pipe_1[0]);

		close(child_1_to_child_2_pipe[0]);

		close(child_to_parent_pipe_2[0]);

		close(child_to_parent_pipe_2[1]);

        // Taking in the five arguments + storing them in a list
		read(parent_to_child_pipe_1[0], child_list_1, sizeof(child_list_1));

		// Getting the length of the list
		int child_list_1_length = sizeof(child_list_1) / sizeof(child_list_1[0]);

        // Sorting the list
		std::sort(child_list_1, child_list_1 + child_list_1_length);

		// Writing the sorted list to the parent
		write(child_to_parent_pipe_1[1], child_list_1, sizeof(child_list_1));

        // Writing the sorted list to child 2
		write(child_1_to_child_2_pipe[1], child_list_1, sizeof(child_list_1));

		// Closing all the pipes
		close(parent_to_child_pipe_1[0]);

		close(child_to_parent_pipe_1[1]);

		close(child_1_to_child_2_pipe[1]);

		exit(EXIT_SUCCESS);
	}
	else if (child_process_id_1 > 0)
	{
		// Closing all the pipes
		close(parent_to_child_pipe_1[0]);

		close(child_to_parent_pipe_1[1]);

		close(child_1_to_child_2_pipe[1]);

		// Retrieving the five arguments + storing them in a list
		int argument_list[] = {atoi(argv[1]), atoi(argv[2]), atoi(argv[3]), atoi(argv[4]), atoi(argv[5])};

        // Writing the argument list to the child
		write(parent_to_child_pipe_1[1], argument_list, sizeof(argument_list));

		// Waiting until child 1 is done
		waitpid(child_process_id_1, &child_status_1, 0);

        // Creating a sorted list
		int sorted_list[5];

        // Reading the list from the child
		read(child_to_parent_pipe_1[0], sorted_list, sizeof(sorted_list));

		// Displaying the list
		printf("The sorted list is: ");

		for (int i = 0; i < 5; i++)
        {
			if (i == 4)
            {
                printf("%d\n", sorted_list[i]);
			}
            else
            {
                printf("%d ", sorted_list[i]);
			}
		}

		// Forking child 2's process
		child_process_id_2 = fork();

        // If child 2's process failed, exit the program
		if (child_process_id_2 < 0)
		{
			fprintf(stderr, "Child 2 failed.\n");

			return EXIT_FAILURE;
		}
		else if (child_process_id_2 == 0)
		{
			// Closing all the pipes
			close(parent_to_child_pipe_1[1]);

			close(parent_to_child_pipe_1[0]);

			close(child_to_parent_pipe_1[1]);

			close(child_to_parent_pipe_1[0]);

			close(child_to_parent_pipe_2[0]);

			close(child_1_to_child_2_pipe[1]);

            // Reading the sorted list from child 1
			read(child_1_to_child_2_pipe[0], child_list_2, sizeof(child_list_2));

			// Retrieving the median
			int child2_median = child_list_2[2];

            // Writing the median to the parent
			write(child_to_parent_pipe_2[1], &child2_median, sizeof(child2_median));

			// Closing all the pipes
			close(child_to_parent_pipe_2[1]);

			close(child_1_to_child_2_pipe[0]);

			exit(EXIT_SUCCESS);
		}
	
		// Waiting until child 2 is done
		waitpid(child_process_id_2, &child_status_2, 0);

		// Storing the median
		int parent_median;

        // Reading the median from child 2
		read(child_to_parent_pipe_2[0], &parent_median, sizeof(parent_median));
		
        // Displaying the median
		printf("The median is: %d\n", parent_median);

		// Closing all the pipes
		close(child_to_parent_pipe_2[1]);

		close(parent_to_child_pipe_1[1]);

		close(child_to_parent_pipe_1[0]);

		close(child_1_to_child_2_pipe[0]);

		close(child_to_parent_pipe_2[0]);
	}

    // Exiting the program
	return EXIT_SUCCESS;
}