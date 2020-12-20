#!/usr/bin/env bats

load common.sh

function setup() {
	isolatefs
	mkdir funny
	mkdir punny
	mkdir dummy
	touch carr.txt
	touch rakk.txt
	touch kett.txt
	touch ttuc.txt
	touch empty.sh
	echo 'echo i am a big potato' > bigpotato.sh
	echo '
ABC=ninini
' > ni.sh
	echo '
echo $#
echo $@
' > nu.sh
	echo "
fewfrer4 &&4;;; |43tege fi
" > invalid.sh
}

@test '[in](3) source' {
	testagainstbash <<<'source empty.sh'
	testagainstbash <<<'source bigpotato.sh'
}

@test '[in](3) source neg' {
	testNeGagainstbash <<<'source'
	testNeGagainstbash <<<'source degdfrefe.grr'
	testNeGagainstbash <<<'source invalid.sh'
}

@test '[in](3) source, variables' {
	testNeGagainstbash <<<'source nu.sh'
	testNeGagainstbash <<<'source nu.sh wuppity wap wap'
	testagainstbash <<'input'
source ni.sh
echo $ABC
input
}

@test '[in](3) cd, pwd' {
	testagainstbash <<<'pwd'
	testagainstbash <<<'cd ; pwd'
	testagainstbash <<<'cd dummy; pwd; cd -; pwd'
	testagainstbash <<<'cd dummy; pwd; cd ..; pwd; cd -'
}

@test '[in](3) cd neg' {
	testNeGagainstbash <<<'cd doesnotexistlul'
	testNeGagainstbash <<<'cd dummy; cd doesnotexistlul'
	testNeGagainstbash <<<'cd dummy; pwd; cd ..; pwd; rm -rf dummy; cd -; pwd; mkdir dummy'
}

@test '[in](3) echo' {
	testagainstbash <<<'echo'
	testagainstbash <<<'echo 1 2 3'
	testagainstbash <<<'echo one two three'
	testagainstbash <<<'echo -n'
	testagainstbash <<<'echo -n 1 2 3'
	testagainstbash <<<'echo -n one two three'
	testagainstbash <<<'echo -e'
	testagainstbash <<<'echo -n -e 1 2 3'
	testagainstbash <<<'echo -e -n one two three'
	testagainstbash <<<'echo -E'
	testagainstbash <<<'echo -E -n 1 2 3'
	testagainstbash <<<'echo -n -E one two three'
}

@test '[in](3) echo -e [relies on expansions]' {
	testagainstbash <<'input'
echo -e '\x39\x36\0136'
input
		testagainstbash <<'input'
echo -e '\a \b \e \f \n \r \t \v \\'
input
		testagainstbash <<'input'
echo 'how dy \c oh no' anyway
input
}

@test '[in](3) echo xpg, shopt [relies on expansions]' {
	testagainstbash <<'input'
shopt -s xpg_echo
echo '\x39\x36\0136'
input
	testagainstbash <<'input'
shopt -s xpg_echo
echo '\a \b \e \f \n \r \t \v \\'
input
	testagainstbash <<'input'
shopt -s xpg_echo
echo 'how dy \c oh no' anyway
input
}

@test '[in](3) echo -e neg [relies on expansions]' {
	testNeGagainstbash <<'input'
echo -e '\P'
input
}
