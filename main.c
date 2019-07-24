int main(void)
{
	pipe_t pipe;
	long value;
	long result;
	char line[128];
	pipe_create(&pipe, 10);
	printf("Enter integer values, or [=] for next result\n");
	while (1)
	{
		printf("Data> ");
		if (!fgets(line, sizeof(line), stdin))
		{
			return 0;
		}
		if (strlen(line) <= 1)
		{
			continue;
		}
		if (strlen(line) <= 2 && line[0] == '=')
		{
			if (pipe_result(&pipe, &result))
			{
				printf("Result is %ld\n", result);
			}
			else
			{
				printf("Result is empty\n");			
			}
			return 0;
		}
		if (sscanf(line, "%ld", &value) < 1)
		{
			fprintf(stderr, "Enter an integer value\n");
			return 0;
		}
		pipe_start(&pipe, value);
	}
	return 0;
}