#!/bin/sh

#
# Tests basiques de visites
#

. ./ftest.sh

CAPA=10			# capacité du musée
QSZ=5			# file d'attente
DURVIS=500		# durée d'une visite


# avant toute chose, partir d'une place nette
$V ./directeur supprimer 2> /dev/null

###############################################################################
# Tests de visiteurs et d'attentes
# - lancer le contrôleur en arrière plan
# - vérifier qu'il a bien démarré
# - ouvrir le musée
# - lancer un visiteur : la durée de la visite ne doit pas excéder 2 sec

# créer le musée
$V ./directeur creer $CAPA $QSZ >&2

# lancer le contrôleur et conserver son pid
./controleur >&2 &
PIDCONTROLEUR=$!

# vérifier qu'il a bien démarré
sleep 0.1
ps_existe $PIDCONTROLEUR || fail "controleur pas démarré correctement"

# ouvrir le musée
$V ./directeur ouvrir >&2

c=$(chrono_start)

# lancer un visiteur
./visiteur $DURVIS >&2 || fail "visiteur1"

DUREE=$(chrono_end $c)
# vérifier qu'on n'a pas attendu trop longtemps
[ $DUREE -gt 2 ] && fail "visite trop longue ($DUREE)"

###############################################################################
# Deuxième test de visiteurs
# - (le contrôleur est toujours lancé et le musée toujours ouvert)
# - lancer le nombre maximum de visiteurs en arrière plan
# - attendre une seconde, le temps qu'ils aient tous terminé
# - il ne devrait plus en rester un seul
# - supprimer le musée
# - le contrôleur devrait le détecter automatiquement et sortir en erreur

# lancer $CAPA visiteurs de 0,5 sec
PIDVISITEURS=
for v in $(seq 1 $CAPA)
do
    ./visiteur $DURVIS >&2 &
    PIDVISITEURS="$PIDVISITEURS $!"
done

# attendre une seconde, ça devrait suffire pour qu'ils soient tous terminés
sleep 1

# s'il reste un visiteur : pb
for P in $PIDVISITEURS
do
    ps_existe $P && fail "Il reste un visiteur (pid = $P)"
done

# si on supprime le musée, le contrôleur devrait détecter une erreur
# car le sémaphore sur lequel il attend est supprimé par devers lui
$V ./directeur supprimer >&2 || fail "suppression du musée"

sleep 1

ps_existe $PIDCONTROLEUR && fail "Le contrôleur ne s'est pas terminé tout seul"

exit 0
