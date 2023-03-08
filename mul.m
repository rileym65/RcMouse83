1 Y:                  ~ Start on row 1
(                     ~ Start of row loop
  1 X:                ~ New row starts at column 1
  (                   ~ Start of column loop
    X. Y. * A:        ~ Multply X by Y giving A
    A. 100 < [ " " ]  ~ Need leading space if less than 100
    A. 10 < [ " " ]   ~ Need another if less than 10
    A. ! " "          ~ Print multiplied value
    X. 1 + X:         ~ Increment X
    X. 11 < ^         ~ Exit from loop when X equals 11
  )                   ~ End of column loop
  "!"                 ~ Move to next line
  Y. 1 + Y:           ~ Increment Y
  Y. 11 < ^           ~ Exit from loop when Y equals 11
)                     ~ End of row loop
$                     ~ Exit program
