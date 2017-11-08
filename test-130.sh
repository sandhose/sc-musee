#!/bin/sh

#
# Tests de verbosité
#

. ./ftest.sh

CAPA=10			# capacité du musée
QSZ=5			# file d'attente
DURVIS=100		# durée d'une visite

LISTESUFF="dc c do v df ds"	# suffixes utilisés dans lancer_test_complet

#
# Une petite fonction pour lancer un test complet
# avec le niveau de debug $1 (0, 1, etc.)
#
# - créer le musée
# - démarrer le contrôleur
# - vérifier qu'il a bien démarré
# - ouvrir le musée
# - lancer un visiteur et attendre la fin de la visite
# - fermer le musée
# - vérifier que le contrôleur est bien arrêté
# - supprimer le musée
#

lancer_test_complet ()
{
    local N=$1

    DEBUG_MUSEE=$N
    export DEBUG_MUSEE

    $V ./directeur supprimer 2>/dev/null

    $V ./directeur creer $CAPA $QSZ > $TMP.dc$N 2>&1 || fail "création ($N)"

    $V ./controleur > $TMP.c$N 2>&1 &
    PIDCONTROLEUR=$!

    # vérifier qu'il a bien démarré
    sleep 0.1			# délai pour laisser le contrôleur démarrer
    ps_existe $PIDCONTROLEUR || fail "controleur pas démarré ($N)"

    $V ./directeur ouvrir    > $TMP.do$N 2>&1 || fail "ouvrir ($N)"
    $V ./visiteur $DURVIS    > $TMP.v$N  2>&1 || fail "visiteur ($N)"
    $V ./directeur fermer    > $TMP.df$N 2>&1 || fail "fermer ($N)"

    sleep 0.1			# délai pour laisser le contrôleur s'arrêter
    ps_existe $PIDCONTROLEUR && fail "contrôleur pas arrêté ($N)"

    $V ./directeur supprimer > $TMP.ds$N 2>&1 || fail "supprimer ($N)"
}

# $1 : niveau 1
# $2 : niveau 2
comparer ()
{
    local s f1 f2

    for s in $LISTESUFF
    do
	f1=$TMP.$s$1
	f2=$TMP.$s$2
	cmp -s $f1 $f2 && fail "Fichiers $f1 et $f2 identiques"
    done
}


##############################################################################
# Premier test
# - normalement, comme il n'y a pas d'erreur, tous les fichiers sont vides

lancer_test_complet 0

for s in $LISTESUFF
do
    f1=$TMP.${s}0
    [ $(wc -l < $f1) -ne 0 ] && fail "Fichier $f1 non vide"
done

##############################################################################
# Deuxième test
# - au niveau 1, les fichiers ne devraient pas être vides

lancer_test_complet 1

comparer 0 1

##############################################################################
# Troisième test
# - niveau maximum, les fichiers devraient être dfférents

lancer_test_complet 9999

comparer 0 9999

# rm -f $TMP.*

exit 0
