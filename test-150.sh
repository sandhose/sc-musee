#!/bin/sh

#
# Tests basiques de démarrage/arrêt du contrôleur
#

. ./ftest.sh

CAPA=10			# capacité du musée
QSZ=5			# file d'attente
DURVIS=1000		# durée d'une visite


# avant toute chose, partir d'une place nette
$V ./directeur supprimer 2> /dev/null

##############################################################################
# Premier test :
# - lancement du contrôleur
# - ouverture et fermeture du musée
# - le contrôleur doit s'être terminé automatiquement

# créer le musée
$V ./directeur creer $CAPA $QSZ >&2

# lancer le contrôleur et conserver son pid
./controleur >&2 &
PIDCONTROLEUR=$!

# vérifier qu'il a bien démarré
sleep 0.1			# délai pour laisser le contrôleur démarrer
ps_existe $PIDCONTROLEUR     || fail "controleur pas démarré correctement"

$V ./directeur ouvrir >&2    || fail "erreur d'ouverture"
$V ./directeur fermer >&2    || fail "erreur de fermeture"

sleep 0.1			# délai pour laisser le contrôleur s'arrêter
ps_existe $PIDCONTROLEUR     && fail "controleur pas arrêté correctement"


##############################################################################
# Deuxième test :
# - lancement du contrôleur
# - lancement d'un visiteur
# - le visiteur doit attendre
# - ouverture du musée
# - le visiteur doit entrer
# - au bout de la visite, le visiteur doit s'être arrêté automatiquement
# - ainsi que le contrôleur

# On recommence, mais cette fois-ci avec un visiteur

# on repart au turbin...
./controleur >&2 &
PIDCONTROLEUR=$!
sleep 0.1			# délai pour laisser le contrôleur démarrer

# on lance un visiteur. Normalement, celui-ci doit attendre que le
# musée ouvre
./visiteur $DURVIS >&2 &
PIDVISITEUR=$!
sleep $((DURVIS/1000))		# le visiteur attend pendant ce temps
sleep 0.1			# délai supplémentaire

# à cette étape, le visiteur est sensé être toujours là
ps_existe $PIDVISITEUR       || fail "le visiteur a disparu"

$V ./directeur ouvrir >&2    || fail "erreur d'ouverture"
sleep 0.1			# délai pour laisser le contrôleur ouvrir

# ok, on attend la durée de la visite
sleep $((DURVIS/1000))		# le visiteur visite
sleep 0.1			# délai suppl pour laisser le visiteur sortir

ps_existe $PIDVISITEUR && fail "le visiteur n'a pas terminé sa visite"

# fermer le musée
$V ./directeur fermer >&2    || fail "erreur de fermeture"

sleep 0.1			# délai pour laisser le contrôleur terminer
ps_existe $PIDCONTROLEUR     && fail "contrôleur 2 pas arrêté correctement"

$V ./directeur supprimer >&2 || fail "erreur de suppression"

exit 0
