1 N:            ~ Starting count
(               ~ Start loop
  N. ! " "      ~ Print current loop count
  N. 1 + N:     ~ Increment count
  N. 11 < ^     ~ Exit loop when N gets to 11
)               ~ End of loop
"!"             ~ Print carriage return
$

