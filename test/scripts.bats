#!/usr/bin/env bats

load common.sh

function setup() {
	isolatefs
}

@test "[file](3) script 1" {
	cat > script.sh <<'input'
#!/bin/bash
echo "Printing text with newline"
echo -n "Printing text without newline"
echo -e "\nRemoving \t backslash \t characters\n"
input
	testagainstbash_cmpfs script.sh
}

@test "[file](4) script 2" {
	cat > script.sh <<'input'
#!/bin/bash
valid=true
count=1
while [ $valid ]
do
echo $count
if [ $count -eq 5 ];
then
break
fi
count=$((count+1))
done
input
	testagainstbash_cmpfs script.sh
}

@test "[file](3) script 3" {
	cat > script.sh <<'input'
#!/bin/bash
echo "Total arguments : $#"
echo "1st Argument = $1"
echo "2nd argument = $2"
input
	testagainstbash_cmpfs script.sh
	testagainstbash_cmpfs script.sh drf vgb svdb
}

@test "[file](4) script 4" { skip
	cat > script.sh <<'input'
#!/bin/bash
for arg
do
index=$(echo $arg | cut -f1 -d=)
val=$(echo $arg | cut -f2 -d=)
case $index in
X) x=$val;;

Y) y=$val;;

*)
esac
done
echo "X+Y=$((x+y))"
input
	testagainstbash_cmpfs script.sh X=45 Y=30
}

@test "[file](4) script 5" {
	cat > script.sh <<'input'
#!/bin/bash

Rectangle_Area() {
area=$(($1 * $2))
echo "Area is : $area"
}

Rectangle_Area 10 20
input
	testagainstbash_cmpfs script.sh
}

@test "[file](3) script 6" {
	cat > script.sh <<'input'
#!/bin/bash
function greeting() {

str="Hello, $name"
echo $str

}

echo "Enter your name"
read name

val=$(greeting)
echo "Return value of the function is $val"
input
	testagainstbash_cmpfs script.sh
}

@test "[file](3) script 7" {
	cat > script.sh <<'input'
#!/bin/bash

echo "Before appending the file"
cat book.txt

echo "Learning Laravel 5">> book.txt
echo "After appending the file"
cat book.txt
input
	testagainstbash_cmpfs script.sh
}
