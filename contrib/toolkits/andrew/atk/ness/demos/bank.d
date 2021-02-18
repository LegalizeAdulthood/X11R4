\begindata{arbiter,956244}
\V 2
\begindata{text,964784}
\textdsversion{12}
\template{default}
\define{plus
}
\plus{Dear Valued Customer:



\leftindent{Your loan application has been approved in the amount of 
\begindata{cel,949556}
\V 2
\begindata{value,949420}
>20
\enddata{value,949420}
10 949420 1 0 0 0 
>OBJ< value
>VIEW< sliderV
>REF< Amount
\begindata{text,949308}
\textdsversion{12}
[long] <min_value> (20)

[long] <max_value> (300)

[long] <bodyfont-size> (22)

[string] <bodyfont> (andysans)

[string] <label> (Amount)

\enddata{text,949308}
\enddata{cel,949556}
\view{celview,949556,0,124,182}000 Dollars

at 
\begindata{cel,1002640}
\V 2
\begindata{value,948788}
>5
\enddata{value,948788}
10 948788 1 0 0 0 
>OBJ< value
>VIEW< thumbstrV
>REF< Interest
\begindata{text,1002444}
\textdsversion{12}
[long] <increment> (1)

[long] <min_value> (5)

[long] <max_value> (20)

[long] <bodyfont-size> (22)

[string] <bodyfont> (andysans)

[string] <label> (Interest)

\enddata{text,1002444}
\enddata{cel,1002640}
\view{celview,1002640,1,109,107} percent interest for 
\begindata{cel,948192}
\V 2
\begindata{value,934508}
>6
\enddata{value,934508}
10 934508 1 0 0 0 
>OBJ< value
>VIEW< thumbstrV
>REF< Months
>LINK< t
\begindata{text,948056}
\textdsversion{12}
[long] <increment> (6)

[long] <min_value> (6)

[long] <max_value> (300)

[long] <bodyfont-size> (22)

[string] <bodyfont> (andysans)

[string] <label> (Months)

\enddata{text,948056}
\enddata{cel,948192}
\view{celview,948192,2,104,102} months.   Your monthly  payment will be 
\begindata{cel,947756}
\V 2
\begindata{value,1001380}
>0
\enddata{value,1001380}
10 1001380 1 0 0 0 
>OBJ< value
>VIEW< stringV
>REF< Payment
>LINK< 
\begindata{text,1001268}
\textdsversion{12}
[long] <bodyfont-size> (22)

[string] <bodyfont> (andysans)

[string] <label> (Payment)

\enddata{text,1001268}
\enddata{cel,947756}
\view{celview,947756,3,153,0} and the total paid to the bank over the lifetime 
of the loan 

will be 
\begindata{cel,1000868}
\V 2
\begindata{value,1000792}
>0
\enddata{value,1000792}
10 1000792 1 0 0 0 
>OBJ< value
>VIEW< stringV
>REF< Total
>LINK< t
\begindata{text,1000680}
\textdsversion{12}
[long] <bodyfont-size> (22)

[string] <bodyfont> (andysans)

[string] <label> (Total Paid)

\enddata{text,1000680}
\enddata{cel,1000868}
\view{celview,1000868,4,147,0}.

}

With best wishes,


Friendly National Bunk}

\begindata{ness,1020140}
\origin{00\4 Nov 1989 at 11:56:04 GMT\guy:  Guy Harris,,MSUB\00}
\template{default}
\define{sans
menu:[Font,Sans]
attr:[FontFamily AndySans Int 0]}
function edit_dollars(real v)

	marker a, c;

	a := textimage(round(v * 100.0));

	c := extent(previous(last(a)), last(a));

	return extent(a, start(c)) ~ "." ~ c;

end function;


real function ipow(real i, integer n)

	integer j;

	real v;

	j := 0;

	v := 1.0;

	while j < n do

		j := j + 1;

		v := v * i;

	end while;

	return v;

end function;


function UpdateResults()

	integer prin;

	real int;

	integer months;

	real payment;

	real totalpaid;

	real pow;

	if mouseaction /= mouseleftup then 

		return;

	end if;

	prin := value_GetValue(inset("Amount"));

--Compute "i/q" - divide by 100 to convert from a percentage,

--and then divide by "q", which is 12.

	int := float(value_GetValue(inset("Interest"))) / 100.0 / 12.0;

	months := value_GetValue(inset("Months"));

--Compute (1 + i/q)^(nq), which is the factor by which the principal is

--multiplied to get the total amount to be paid.

--"n" is the number of years of the loan, and "q" is the number of  times

-- per year the interest is compounded, i.e. 12 for once per month.  Thus,

--"nq" is the number of months of the loan.

	pow := ipow(1.0 + int, months);

--printline (textimage(pow/(pow - 1.0)));

	payment := float(prin * 1000) * int * pow / (pow - 1.0); 

	totalpaid := payment * float(months);

	value_SetString(inset("Payment"), edit_dollars(payment));

	value_SetString(inset("Total"), edit_dollars(totalpaid));


end function;



extend "Payment"

	on mouse "any"

		UpdateResults();

	end mouse

end extend


extend "Months"

	on mouse "any"

		dohit (currentinset, mouseaction, mousex, mousey);

		UpdateResults();

	end mouse

end extend\
\enddata{ness,1020140}
\view{nessview,1020140,13,0,486\
}\enddata{text,964784}
1 964784 0 0 0 0 
>OBJ< text
>VIEW< textview
>REF< 
\enddata{arbiter,956244}
