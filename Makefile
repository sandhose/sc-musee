#
# Ce Makefile contient les cibles suivantes :
#
# all		: compile les programmes
# clean		: supprime les fichiers générés automatiquement
# coverage	: compile les programmes pour mesurer la couverture de code
# test		: lance tous les tests (scripts shell test-xxx.sh)
#		  (sans appeler valgrind)
# gcov		: génère les rapports de couverture (à lancer après avoir
#		  lancé les cibles 'coverage' et 'test').
#		  Résultats dans *.gcov
# ctags		: génère un fichier tags pour la navigation avec vim.
#		  (voir http://usevim.com/2013/01/18/tags/)
#
# De plus, les cibles supplémentaires suivantes sont fournies pour
# simplifier les tâches répétitives :
#
# test-avec-valgrind	: lance les tests avec valgrind (conseillé)
# couverture-et-tests	: automatise les tests avec rapport de couverture
#

COV = -coverage

CFLAGS = -g -Wall -Wextra -pedantic -std=gnu99 -Werror $(COVERAGE)

PROGS = directeur controleur visiteur dump
TESTS = test-100.sh test-110.sh test-120.sh test-130.sh test-140.sh test-150.sh test-160.sh test-170.sh
SOURCES = $(PROGS:%=%.c) musee.c musee.h

DISTNAME = gliech
DISTFILES = $(SOURCES) rapport.md rapport.pdf Makefile ftest.sh $(TESTS)

all: $(PROGS)

#
# Il vous est suggéré pour vous simplifier la vie de créer un
# fichier musee.c contenant des focntions communes à au moins
# deux programmes et un fichier musee.h pour vos définitions.
#

$(PROGS): musee.o

musee.o: musee.h

coverage: clean
	$(MAKE) COVERAGE=$(COV)

gcov:
	gcov *.c

%.pdf: %.md
	pandoc --pdf-engine=xelatex -o $@ $<


# Par défaut, "test" lance les tests sans valgrind.
# Si on souhaite utiliser valgrind (conseillé), positionner la
# variable VALGRIND ou utiliser la cible "test-avec-valgrind"

test: test-sans-valgrind

test-sans-valgrind: all
	@for i in $(TESTS) ; do echo $$i ; sh $$i || exit 1 ; done

test-avec-valgrind: all
	VALGRIND="valgrind -q" ; export VALGRIND ; \
	    for i in $(TESTS) ; do echo $$i ; sh $$i || exit 1 ; done

couverture-et-tests: clean coverage test gcov

ctags:
	ctags *.[ch]

clean:
	rm -f *.o $(PROGS)
	rm -f *.gc*
	rm -f *.log
	rm -f tags core
	rm -f *.pdf

dist: $(DISTNAME).tar.gz

$(DISTNAME).tar.gz: $(DISTFILES)
	ln -sf . $(DISTNAME)
	tar -czvf $@ $(^:%=$(DISTNAME)/%)
	rm $(DISTNAME)
