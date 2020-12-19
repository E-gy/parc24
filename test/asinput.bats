#!/usr/bin/env bats

load common.sh

@test "[in](1) simply command" {
	testagainstbash <<'input'
ls
input
}

@test "[in](1) simply commands" {
	testagainstbash <<'input'
ls
#i am a comment
ls #i am a comment too!
input
}

@test "[in](1) if/elif/else" {
	testagainstbash <<<'if [ 1 -eq 1 ] ; then echo -n "one"; fi'
	testagainstbash <<<'if [ 2 -eq 1 ] ; then echo -n "one"; fi'
	testagainstbash <<<'if [ 2 -eq 1 ] ; then echo -n "one"; else echo -n "other"; fi'
	testagainstbash <<<'if [ 2 -eq 1 ] ; then echo -n "one"; elif [ 2 -eq 2 ] ; then echo -n "two"; else echo -n "other"; fi'
	testagainstbash <<<'if [ 3 -eq 1 ] ; then echo -n "one"; elif [ 3 -eq 2 ] ; then echo -n "two"; else echo -n "other"; fi'
	testagainstbash <<<'if [ 3 -eq 1 ] ; then echo -n "one"; elif [ 3 -eq 2 ] ; then echo -n "two"; fi'
}

@test "[in](1) if/elif/else multiline" {
	testagainstbash <<'input'
if [ 3 -eq 1 ];
	then
			echo -n "one";
	elif [ 3 -eq 2 ] ;
	then echo -n "two";

	#I AM AN ANNOYING LIL' SHIT

else echo -n "other";

	fi
input
}

@test "[in](3) if/elif/else, vars" {
	testagainstbash <<<'i=1; if [ $i -eq 1 ] ; then echo -n "one"; fi'
	testagainstbash <<<'i=2; if [ $i -eq 1 ] ; then echo -n "one"; fi'
	testagainstbash <<<'i=2; if [ $i -eq 1 ] ; then echo -n "one"; else echo -n "other"; fi'
	testagainstbash <<<'i=2; if [ $i -eq 1 ] ; then echo -n "one"; elif [ $i -eq 2 ] ; then echo -n "two"; else echo -n "other"; fi'
	testagainstbash <<<'i=3; if [ $i -eq 1 ] ; then echo -n "one"; elif [ $i -eq 2 ] ; then echo -n "two"; else echo -n "other"; fi'
	testagainstbash <<<'i=3; if [ $i -eq 1 ] ; then echo -n "one"; elif [ $i -eq 2 ] ; then echo -n "two"; fi'
}

@test "[in](2) for" {
	testagainstbash <<<'for i in d e f; do echo -n $i; done'
	testagainstbash <<<'for i in d; do echo -n $i; done'
	testagainstbash <<<'for i in hello there friend; do echo -n $i; done'
	testagainstbash <<'input'
for i in d e f; do
	echo -n $i;
done
input
}

@test "[in](2) -s, for args" {
	testagainstbash -s <<<'for i do echo $i; done'
	testagainstbash -s <<<'for i; do echo $i; done'
	testagainstbash -s <<<'for i do echo $i; done' hello there
	testagainstbash -s <<<'for i; do echo $i; done' hello there
}

@test "[in](2) case" {
	testagainstbash <<'input'
case Lithuania in
  Lithuania)
    echo -n "Lithuanian"
    ;;
  Romania | Moldova)
    echo -n "Romanian"
    ;;
  Italy | Switzerland )
    echo -n "Italian"
    ;;
  *)
    echo -n "unknown"
    ;;
esac
input
	testagainstbash <<'input'
case China in esac
input
	testagainstbash <<'input'
case China in

esac
input
	testagainstbash <<'input'
case Italy in
  Lithuania)
    echo -n "Lithuanian"
    ;;
  Romania | Moldova)
    echo -n "Romanian"
    ;;
  Italy | Switzerland )
    echo -n "Italian"
    ;;
  *)
    echo -n "unknown"

esac
input
	testagainstbash <<'input'
case Switzerland in
  Lithuania)
    echo -n "Lithuanian"
    ;;
  Romania | Moldova)
    echo -n "Romanian"
    ;;
  Italy | Switzerland )
    echo -n "Italian"
    ;;
  *)
    echo -n "unknown"
    ;;
esac
input
}

@test "[in](3) case, quotes" {
	testagainstbash <<'input'
input
	testagainstbash <<'input'
case "San Marino" in
  Lithuania)
    echo -n "Lithuanian"
    ;;
  Romania | Moldova)
    echo -n "Romanian"
    ;;
  Italy | "San Marino" | Switzerland | "Vatican City")
    echo -n "Italian"
    ;;
  *)
    echo -n "unknown"
    ;;
esac
input
}

@test "[in](4) case, quotes, patterns" {
	testagainstbash <<'input'
case China in

  Lithuania)
    echo -n "Lithuanian"
    ;;

  Romania | Moldova)
    echo -n "Romanian"
    ;;

  Italy | "San Marino" | Switzerland | "Vatican City")
    echo -n "Italian"
    ;;

  *)
    echo -n "unknown"
    ;;
esac
input
}

@test "[in](2) lists &&" {
	testagainstbash <<<'[ 1 -eq 1 ] && echo 1 && [ 2 -eq 2 ] && echo 2'
	testagainstbash <<<'[ 1 -eq 1 ] && echo 1 && [ 3 -eq 2 ] && echo 2'
	testagainstbash <<<'[ 5 -eq 1 ] && echo 1 && [ 2 -eq 2 ] && echo 2'
};

@test "[in](2) lists ||" {
	testagainstbash <<<'[ 1 -eq 1 ] || echo 1 && [ 2 -eq 2 ] || echo 2'
	testagainstbash <<<'[ 1 -eq 1 ] || echo 1 && [ 3 -eq 2 ] || echo 2'
	testagainstbash <<<'[ 5 -eq 1 ] || echo 1 && [ 2 -eq 2 ] || echo 2'
};

@test "[in](2) lists ;" {
	testagainstbash <<<'[ 1 -eq 1 ] ; echo 1 ; [ 2 -eq 2 ] ; echo 2'
	testagainstbash <<<'[ 1 -eq 1 ] ; echo 1 ; [ 3 -eq 2 ] ; echo 2'
	testagainstbash <<<'[ 5 -eq 1 ] ; echo 1 ; [ 2 -eq 2 ] ; echo 2'
	testagainstbash <<<'[ 1 -eq 1 ] ; echo 1 ; [ 2 -eq 2 ]'
	testagainstbash <<<'[ 1 -eq 1 ] ; echo 1 ; [ 3 -eq 2 ]'
	testagainstbash <<<'[ 5 -eq 1 ] ; echo 1 ; [ 2 -eq 2 ]'
	testagainstbash <<<'[ 1 -eq 1 ] ; echo 1 ; [ 2 -eq 2 ] ;'
	testagainstbash <<<'[ 1 -eq 1 ] ; echo 1 ; [ 3 -eq 2 ] ;'
	testagainstbash <<<'[ 5 -eq 1 ] ; echo 1 ; [ 2 -eq 2 ] ;'
};

@test "[in](2) functions" {
	testagainstbash <<'input'
function fun(){
	echo hello
}
input
	testagainstbash <<'input'
function fun(){
	echo hello
}
fun
input
	testagainstbash <<'input'
fun(){
	echo hello
}
fun
input
}

@test "[in](3) functions arguments" {
	testagainstbash <<'input'
function fun(){
	echo $#
	echo $1 $2
	echo $@
}
	fun
	fun potato
	fun eat vegetables every day or santa will take the heater away
input
}

@test "[in](2) pipes" {
	testagainstbash<<<'ls | cat'
	testagainstbash<<<'ls | cat - | cat -'
	testagainstbash<<<'ls | grep s'
	testagainstbash<<<'ls | grep s | grep h'
}

@test "[in](2) pipes, functions" {
	testagainstbash<<'input'
function fun(){
	echo grr
	echo frr
	echo btt
	echo ztt
	echo raa
	echo tee
}
fun | grep t
input
}
