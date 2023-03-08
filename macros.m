7 A: 9 B:           ~ Set a couple variables
#C;                 ~ Call macro C
#a,5,3; ! "!"       ~ Call macro a with 5 and 3 as parameters
#b,5,3; ! "!"       ~ Call macro b with 5 and 3 as parameters
#a,A.,B.; ! "!"     ~ Call macro a with variables A and B as parameters
$

$a 1% 2% + @        ~ Add both parameters leaving result on stack
$b 1% 2% * @        ~ Multiply both parameters leaving result on stack
$A 1% 2% - @        ~ Subtract both parameters leaving result on stack
$B 1% 2% / @        ~ Divide both parameters leaving result on stack
$C "test macro!" @  ~ Print some text

