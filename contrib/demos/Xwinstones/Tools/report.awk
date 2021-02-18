BEGIN {
	for (i = 0; i < 1000; i++)
	{
		out[i] = "";
	}
	i = 0;
}
{
	if (i == 0)
	{
		out[i] = sprintf("%s%s",out[i],head0);
		i++;
	}
	if (i == 1)
	{
		out[i] = sprintf("%s%s",out[i],head1);
		i++;
	}
	if (i == 2)
	{
		out[i] = sprintf("%s%s",out[i],head2);
		i++;
	}
}
/casename:/ {
	if (i > 1)
	{
		out[i] = sprintf("%s%-25s",out[i],$2);
	}
}
/record:/ {
	if (i > 1)
	{
		out[i] = sprintf("%-30s %-10s",out[i],$2);
		i++;
	}
}
END {
	max = i;
	for (i = 0; i < max; i++)
	{
		print out[i];
	}
}
