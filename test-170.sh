#!/bin/sh

#
# Tests de dump et de fermeture avant la fin des visites
#

. ./ftest.sh

CAPA=5			# capacité du musée
QSZ=5			# file d'attente
DURVIS=2000		# durée d'une visite


# avant toute chose, partir d'une place nette
$V ./directeur supprimer 2> /dev/null

###############################################################################
# Tests de fermeture alors qu'il y a encore des visiteurs et de dump
# - lancer le contrôleur en arrière-plan
# - lancer les visiteurs
# - vérifier que les visiteurs sont en attente
# - ouvrir le musée
# - vérifier que les visiteurs sont dans le musée
# - fermer le musée
# - vérifier que les visiteurs sont toujours dans le musée, et que le
#   controleur attend leur sortie
# - attendre la durée de la visite
# - vérifier que tous les visiteurs et le controleur se sont bien terminés

$V ./dump >&2 && fail "dump a fonctionné sans musée"

# créer le musée (pour tester dump)
$V ./directeur creer 1 2 >&2

$V ./dump 2>$TMP || fail "dump n'a pas foncitonné avec musée"
grep -iq "capa.*\<1\>" $TMP || fail "dump ne donne pas la bonne capacité"
grep -iq "file.*\<2\>"  $TMP || fail "dump ne donne pas la bonne file"
grep -iq "ouvert"          $TMP && fail "dump affiche le musée ouvert"
grep -iq "attente.*\<0\>"  $TMP || fail "dump affiche des personnes en attente"

# re-créer le musée (et constate le changement de paramètres)
$V ./directeur creer $CAPA $QSZ >&2

$V ./dump 2>$TMP || fail "dump n'a pas foncitonné avec musée"
grep -iq "capa.*\<$CAPA\>" $TMP || fail "dump ne donne pas la bonne capacité"
grep -iq "file.*\<$QSZ\>"  $TMP || fail "dump ne donne pas la bonne file"
grep -iq "ouvert"          $TMP && fail "dump affiche le musée ouvert"
grep -iq "attente.*\<0\>"  $TMP || fail "dump affiche des personnes en attente"

# lancer le contrôleur et conserver son pid
./controleur >&2 &
PIDCONTROLEUR=$!
sleep 0.1			# délai pour laisser le contrôleur démarrer

ps_existe $PIDCONTROLEUR || fail "controleur pas démarré correctement"

# lancer $CAPA visiteurs : devraient durer $DURVIS exactement
PIDVISITEURS=""
for v in $(seq 1 $CAPA)
do
    ./visiteur $DURVIS >&2 &
    PIDVISITEURS="$PIDVISITEURS $!"
done
sleep 0.1			# délai pour laisser les visiteurs démarrer

$V ./dump 2>$TMP || fail "dump n'a pas foncitonné avec musée"
grep -iq "attente.*\<$CAPA\>" $TMP || fail "dump: pas de personnes en attente"

# ouvrir le musée
$V ./directeur ouvrir >&2

echo "$(date) : le musée est ouvert" >&2

sleep 1

$V ./dump 2>$TMP || fail "dump n'a pas foncitonné avec musée"
grep -iq "ouvert" $TMP || fail "dump n'affiche pas le musée ouvert"
grep -iq "attente.*\<0\>" $TMP || fail "dump: des visiteurs attendent toujours"
grep -iq "intérieur.*\<$CAPA\>" $TMP || fail "un visiteur n'est pas dedans"

# on refermer le musée
$V ./directeur fermer >&2

echo "$(date) : le musée est fermé" >&2

sleep 0.1

for p in $PIDVISITEURS $PIDCONTROLEUR
do
    ps_existe $p || fail "un des processus n'est plus là"
done

$V ./dump 2>$TMP || fail "dump n'a pas foncitonné avec musée"
grep -iq "fermé" $TMP || fail "dump n'affiche pas le musée fermé"
grep -iq "attente.*\<0\>" $TMP || fail "dump: des visiteurs attendent toujours"
grep -iq "intérieur.*\<$CAPA\>" $TMP || fail "un visiteur n'est pas dedans"

# attendre la fin du premier ensemble de visiteurs
sleep $((DURVIS/1000))

echo "$(date) : les $CAPA visiteurs et le contrôleur doivent être arrêtés" >&2


# vérifier qu'ils sont tous bien terminés
for p in $PIDVISITEURS $PIDCONTROLEUR
do
  ps_existe $p && fail "un des processus est toujours là"
done

$V ./directeur supprimer >&2 || fail "suppression"

exit 0
