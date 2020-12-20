#!/usr/bin/env bats

load common.sh

@test '[in](3) exit' {
	testagainstbash <<<'exit'
	testagainstbash <<<'exit 0'
	testagainstbash <<<'exit 3'
	testagainstbash <<<'echo aurora; exit 3; echo borealis'
	testNeGagainstbash <<<'cd doesnotexist; exit'
}

@test '[in](3) exit for' {
	testagainstbash <<<'for i in a b c; do exit; done'
}

@test '[in](3) exit neg' {
	testNeGagainstbash <<<'exit fesdgttg'
	testNeGagainstbash <<<'exit -5'
	testNeGagainstbash <<<'exit 484944'
}

@test "[in](3) continue" {
	testagainstbash <<'input'
for item in vegetables fruits tomatoes; do
	echo kkkkk
	continue
	echo zzzzz
done
input
}

@test "[in](3) continue deep" {
	testagainstbash <<'input'
for item in vegetables fruits tomatoes; do
	echo uuuuu
	for t in a b c d e; do
		echo kkkkk
		continue 1
		echo zzzzz
	done
	echo vvvvv
done
input
	testagainstbash <<'input'
for item in vegetables fruits tomatoes; do
	echo uuuuu
	for t in a b c d e; do
		echo kkkkk
		continue 2
		echo zzzzz
	done
	echo vvvvv
done
input
}

@test "[in](3) continue neg" {
	testNeGagainstbash <<'input'
for item in vegetables fruits tomatoes; do
	echo kkkkk
	continue adcscfdscf
	echo zzzzz
done
input
	testNeGagainstbash <<'input'
for item in vegetables fruits tomatoes; do
	echo kkkkk
	continue -50
	echo zzzzz
done
input
}

@test "[in](3) break" {
	testagainstbash <<'input'
for item in vegetables fruits tomatoes; do
	echo kkkkk
	break
	echo zzzzz
done
input
	testagainstbash <<'input'
until [ 2 -eq 36 ]; do
	echo kkkkk
	break
	echo zzzzz
done
input
	testagainstbash <<'input'
while [ 2 -eq 2 ]; do
	echo kkkkk
	break
	echo zzzzz
done
input
}

@test "[in](3) break deep" {
	testagainstbash <<'input'
for item in vegetables fruits tomatoes; do
	echo uuuuu
	for t in a b c d e; do
		echo kkkkk
		break 1
		echo zzzzz
	done
	echo vvvvv
done
input
	testagainstbash <<'input'
for item in vegetables fruits tomatoes; do
	echo uuuuu
	for t in a b c d e; do
		echo kkkkk
		break 2
		echo zzzzz
	done
	echo vvvvv
done
input
}

@test "[in](3) break neg" {
	testNeGagainstbash <<'input'
for item in vegetables fruits tomatoes; do
	echo kkkkk
	break adcscfdscf
	echo zzzzz
done
input
	testNeGagainstbash <<'input'
for item in vegetables fruits tomatoes; do
	echo kkkkk
	break -50
	echo zzzzz
done
input
}

@test "[in](3) continue deep subshelly" {
	testagainstbash <<'input'
for item in vegetables fruits tomatoes; do
	echo uuuuu
	(
		for t in a b c d e; do
			echo kkkkk
			continue 1
			echo zzzzz
		done
	)
	echo vvvvv
done
input
	testagainstbash <<'input'
for item in vegetables fruits tomatoes; do
	echo uuuuu
	(
		for t in a b c d e; do
			echo kkkkk
			continue 2
			echo zzzzz
		done
	)
	echo vvvvv
done
input
}

@test "[in](3) break deep subshelly" {
	testagainstbash <<'input'
for item in vegetables fruits tomatoes; do
	echo uuuuu
	(
		for t in a b c d e; do
			echo kkkkk
			break 1
			echo zzzzz
		done
	)
	echo vvvvv
done
input
	testagainstbash <<'input'
for item in vegetables fruits tomatoes; do
	echo uuuuu
	(
		for t in a b c d e; do
			echo kkkkk
			break 2
			echo zzzzz
		done
	)
	echo vvvvv
done
input
}

@test "[in](3) exit deep subshelly" {
	testagainstbash <<'input'
for item in vegetables fruits tomatoes; do
	echo uuuuu
	(
		for t in a b c d e; do
			echo kkkkk
			exit 0
			echo zzzzz
		done
	)
	echo vvvvv
done
input
	testagainstbash <<'input'
for item in vegetables fruits tomatoes; do
	echo uuuuu
	(
		for t in a b c d e; do
			echo kkkkk
			exit 1
			echo zzzzz
		done
	)
	echo vvvvv
done
input
}
