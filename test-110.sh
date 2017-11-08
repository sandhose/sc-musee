#!/bin/sh

#
# Tests de création et suppression des objets IPC System V
#

. ./ftest.sh

###############################################################################
# Test de création et suppression des IPC
# - prendre une photo 1 du résultat de ipcs -m / ipcs -s
# - créer le musée
# - prendre une photo 2 du résultat de ipcs et comparer photo 2 / photo 1
# - supprimer le musée
# - prendre une photo 3 du résultat de ipcs et comparer photo 3 / photo 1


# avant toute chose, partir d'une place nette
$V ./directeur supprimer 2> /dev/null > /dev/null

# conserver l'état initial des IPC
ipcs -m > $TMP.m1
ipcs -s > $TMP.s1
(echo "État initial des IPC" ; cat $TMP.m1 $TMP.s1) >&2

# créer les IPC
$V ./directeur creer 2 2 >&2 || fail "directeur: création"

# normalement, on ne devrait pas retrouver la même chose
ipcs -m > $TMP.m2
ipcs -s > $TMP.s2
(echo "État des IPC après création" ; cat $TMP.m2 $TMP.s2) >&2
cmp -s $TMP.m1 $TMP.m2       && fail "création : ipcs -m identique"
cmp -s $TMP.s1 $TMP.s2       && fail "création : ipcs -s identique"

# supprimer : on devrait retrouver les IPC du début"
$V ./directeur supprimer >&2 || fail "directeur: suppression"

ipcs -m > $TMP.m2
ipcs -s > $TMP.s2
(echo "État des IPC après suppression" ; cat $TMP.m2 $TMP.s2) >&2
cmp -s $TMP.m1 $TMP.m2       || fail "suppression : ipcs -m pas identique"
cmp -s $TMP.s1 $TMP.s2       || fail "suppression : ipcs -s pas identique"

rm -f $TMP.[ms][12]

###############################################################################
# Test pendant qu'on est là
# - tenter de démarrer les programmes qui supposent le musée déjà créé

$V ./directeur supprimer >&2 && fail "suppression avec shm non créé"
$V ./directeur ouvrir    >&2 && fail "ouverture avec shm non créé"
$V ./directeur fermer    >&2 && fail "fermeture avec shm non créé"
$V ./controleur          >&2 && fail "controleur avec shm non créé"
$V ./visiteur 1000       >&2 && fail "visiteur avec shm non créé"

exit 0
