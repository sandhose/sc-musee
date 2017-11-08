#!/bin/sh

#
# Tests d'attente du visiteur
#

. ./ftest.sh

CAPA=10			# capacité du musée
QSZ=5			# file d'attente
DURVIS=4000		# durée d'une visite


# avant toute chose, partir d'une place nette
$V ./directeur supprimer 2> /dev/null

###############################################################################
# Tests d'attente des visiteurs
# - lancer le contrôleur en arrière-plan
# - lancer le nombre maximum de visiteurs pour la capacité du musée (CAPA)
# - lancer le nombre maximum de visiteurs pour la file d'attente (QSZ)
# - lancer un nouveau visiteur : il doit renoncer immédiatement
# - attendre la durée d'une visite
# - vérifier que tous les visiteurs du premier jet (CAPA) ont terminé
# - vérifier qu'aucun des visiteurs qui étaient en file d'attente n'a terminé
# - attendre à nouveau la durée d'une visite
# - vérifier que tous les visiteurs qui étaient en file d'attente ont terminé
# - fermer le musée
# - vérifier que le contrôleur s'est bien terminé tout seul

# créer le musée
$V ./directeur creer $CAPA $QSZ >&2

# lancer le contrôleur et conserver son pid
./controleur >&2 &
PIDCONTROLEUR=$!
sleep 0.1			# délai pour laisser le contrôleur démarrer

ps_existe $PIDCONTROLEUR || fail "controleur pas démarré correctement"

# ouvrir le musée
$V ./directeur ouvrir >&2

echo "$(date) : le musée est ouvert" >&2

# lancer $CAPA visiteurs : devraient durer $DURVIS exactement
PIDVISITEURS=""
for v in $(seq 1 $CAPA)
do
    ./visiteur $DURVIS >&2 &
    PIDVISITEURS="$PIDVISITEURS $!"
done
sleep 0.1			# délai pour laisser les visiteurs démarrer

# lancer $QSZ visiteurs qui doivent attendre
PIDQ=""
for v in $(seq 1 $QSZ)
do
    ./visiteur $DURVIS >&2 &
    PIDQ="$PIDQ $!"
done
sleep 0.1			# délai pour laisser les visiteurs démarrer

echo "$(date) : les $CAPA et $QSZ visiteurs sont lancés" >&2

# À présent, tout nouveau visiteur doit se faire jeter
c=$(chrono_start)
./visiteur $DURVIS
DUREE=$(chrono_end $c)
[ $DUREE -gt 1 ] && fail "le $((CAPA+QSZ+1))e visiteur a attendu"

# attendre la fin du premier ensemble de visiteurs
sleep $((DURVIS/1000))

echo "$(date) : les $CAPA visiteurs doivent être arrêtés" >&2

# vérifier qu'ils sont tous bien terminés
for p in $PIDVISITEURS
do
    ps_existe $p && fail "un des $CAPA premiers visiteurs est toujours là"
done

# vérifier que les $QSZ autres ne sont pas encore terminés
for p in $PIDQ
do
    ps_existe $p || fail "un des $QSZ visiteurs est déjà arrêté"
done

# attendre la fin du deuxième ensemble de visiteurs
sleep $((DURVIS/1000))

# vérifier que les $QSZ sont bien terminés
for p in $PIDQ
do
    ps_existe $p && fail "un des $QSZ visiteurs ne s'est pas arrêté"
done

# fermer le musée
$V ./directeur fermer >&2 || fail "fermeture"

sleep 0.1			# délai pour laisser le contrôleur s'arrêter
ps_existe $PIDCONTROLEUR && fail "Le contrôleur ne s'est pas terminé tout seul"

$V ./directeur supprimer >&2 || fail "suppression"

exit 0
