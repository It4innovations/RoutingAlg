#!/usr/bin/awk -f
BEGIN {
		EDGE_ID = ""
		FROM = ""
		FILE = ""
}

{
	if($1 == "From:")
	{
		FROM = substr($3,0,length($3) - 1)
		next
	}

	if($1 == "To:")
	{
		FILE = substr($3,0,length($3) - 1) "_" FROM ".csv"
		printf("edge\tinterval\tprofile\tvelocity\tprobs\n") > FILE
		next
	}

	if($1 == "Generating")
	{
		EDGE_ID = $5
		next
	}

	if($1 >= 0 && $1 <= 671 && EDGE_ID != "")
	{
		printf("%d\t%s\n",EDGE_ID, $0) >> FILE
	}

	if($1 == 671)
	{
		EDGE_ID = ""

	}
}
