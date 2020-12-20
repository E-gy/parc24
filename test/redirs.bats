#!/usr/bin/env bats

load common.sh

function setup() {
	isolatefs
	echo "i am carr.txt" > carr.txt
	echo "i am rakk.txt" > rakk.txt
	echo "i am kett.txt" > kett.txt
	echo "i am ttuc.txt" > ttuc.txt
}

@test "[in](2) redir <" {
	testagainstbash <<<'cat <carr.txt'
	testagainstbash <<<'cat /dev/fd/5 5<carr.txt'
}

@test "[in](2) redir >" {
	testagainstbash_cmpfs <<<'ls >carr.txt'
	testagainstbash_cmpfs <<<'ls 1>carr.txt'
	testagainstbash_cmpfs <<<'ls >notexist.txt'
	testagainstbash_cmpfs <<<'ls 1>notexist.txt'
}

@test "[in](2) redir >|" {
	testagainstbash_cmpfs <<<'ls >|carr.txt'
	testagainstbash_cmpfs <<<'ls 1>|carr.txt'
	testagainstbash_cmpfs <<<'ls >|notexist.txt'
	testagainstbash_cmpfs <<<'ls 1>|notexist.txt'
}

@test "[in](2) redir >>" {
	testagainstbash_cmpfs <<<'ls >>rakk.txt'
	testagainstbash_cmpfs <<<'ls 1>>rakk.txt'
	testagainstbash_cmpfs <<<'ls >>notexist.txt'
	testagainstbash_cmpfs <<<'ls 1>>notexist.txt'
}

@test "[in](2) redir <&" {
	testagainstbash <<<'cat 7<carr.txt <&7'
	testagainstbash <<<'cat /dev/fd/5 7<carr.txt 5<&7'
}

@test "[in](2) redir >&" {
	testagainstbash_cmpfs <<<'ls 7>carr.txt >&7'
	testagainstbash_cmpfs <<<'ls 7>carr.txt 1>&7'
}

@test "[in](2) redir <>" {
	testagainstbash_cmpfs <<<'cat rakk.txt - rakk.txt <>ttuc.txt'
	testagainstbash_cmpfs <<<'cat rakk.txt - rakk.txt 1<>ttuc.txt'
	testagainstbash_cmpfs <<<'cat rakk.txt - rakk.txt <>notexist.txt'
	testagainstbash_cmpfs <<<'cat rakk.txt - rakk.txt 1<>notexist.txt'
}

@test "[in](2) redir heredocs <<,<<-" {
	testagainstbash <<'input'
cat <<inp
hi
		there
	little
	$one
inp
input
	testagainstbash <<'input'
cat <<-inp
	hi
			there
		little
		$one
	inp
input
	testagainstbash <<'input'
cat <<'inp'
hi
		there
	little
	$one
inp
input
	testagainstbash <<'input'
cat <<-'inp'
	hi
			there
		little
		$one
	inp
input
	testagainstbash <<'input'
cat <<"inp"
hi
		there
	little
	$one
inp
input
	testagainstbash <<'input'
cat <<-"inp"
	hi
			there
		little
		$one
	inp
input
}

@test "[in](3) redir heredocs <<,<<-; and variables" {
	testagainstbash <<'input'
one=ibonek
cat <<inp
hi
		there
	little
	$one
inp
input
	testagainstbash <<'input'
one=ibonek
cat <<-inp
	hi
			there
		little
		$one
	inp
input
	testagainstbash <<'input'
one=ibonek
cat <<'inp'
hi
		there
	little
	$one
inp
input
	testagainstbash <<'input'
one=ibonek
cat <<-'inp'
	hi
			there
		little
		$one
	inp
input
	testagainstbash <<'input'
one=ibonek
cat <<"inp"
hi
		there
	little
	$one
inp
input
	testagainstbash <<'input'
one=ibonek
cat <<-"inp"
	hi
			there
		little
		$one
	inp
input
}

@test "[in](2) redirections and pipes" {
	testagainstbash_cmpfs <<'input'
grep f >>rakk.txt <<-lul
	hair
	tomato
	hare
	efewf
	frwsred
	gt
	dfhtfth
	frsvdegfeg
	dvgdbvgdrfbd
	bdfbggdrfbre
	gh
	sdv
	bgfhrfszdfv
	svdvrjn
lul
grep f >>ttuc.txt <<-lul
	edgdfv
	sfvdzsgv
	rttgegs\\
	fsrgdethbdf
	faesregdvdbgdfr
	adqewfefvioiyt
	kyikjswcbors
lul
grep h <rakk.txt | cat - ttuc.txt 5>reeh.txt 5>&1 | grep s > quack
input
}
