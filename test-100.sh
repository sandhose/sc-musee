#!/bin/sh

#
# Tests basiques de vérification de syntaxe
#

. ./ftest.sh

###############################################################################
# Tests d'arguments invalides
# (on attend un message d'erreur du type "usage: ..." pour être sûr
# que le problème de syntaxe est bien détecté)

$V ./directeur               2> $TMP >&2 || tu && fail "dir: pas d'arg"
$V ./directeur blabla        2> $TMP >&2 || tu && fail "dir: arg invalide"
$V ./directeur creer         2> $TMP >&2 || tu && fail "dir: creer 0 arg"
$V ./directeur creer 1       2> $TMP >&2 || tu && fail "dir: creer 1 seul arg"
$V ./directeur creer 1 1 1   2> $TMP >&2 || tu && fail "dir: creer 3 args"
$V ./directeur creer -1 1    2> $TMP >&2 || tu && fail "dir: creer capa -1"
$V ./directeur creer 1 -1    2> $TMP >&2 || tu && fail "dir: creer attente -1"
$V ./directeur creer 0 1     2> $TMP >&2 || tu && fail "dir: creer avec capa 0"
$V ./directeur ouvrir xxx    2> $TMP >&2 || tu && fail "dir: ouvrir 1 arg"
$V ./directeur fermer xxx    2> $TMP >&2 || tu && fail "dir: fermer 1 arg"
$V ./directeur supprimer xxx 2> $TMP >&2 || tu && fail "dir: supprimer 1 arg"

rm -f $TMP

exit 0
