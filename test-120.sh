#!/bin/sh

#
# Suites des tests de vérification de syntaxe
#

. ./ftest.sh

# avant toute chose, partir d'une place nette
$V ./directeur supprimer 2> /dev/null

# créer le musée et l'ouvrir pour faire les tests
$V ./directeur creer 2 2 >&2

$V ./controleur xxx 2> $TMP >&2 || tu && fail "controleur: 1 arg"
$V ./dump xxx       2> $TMP >&2 || tu && fail "dump: 1 arg"
$V ./visiteur       2> $TMP >&2 || tu && fail "visiteur: 0 arg"
$V ./visiteur xx xx 2> $TMP >&2 || tu && fail "visiteur: 2 arg"
$V ./visiteur -1    2> $TMP >&2 || tu && fail "visiteur: arg < 0"

$V ./directeur supprimer >&2    || fail "suppression IPC"

rm -f $TMP

exit 0
