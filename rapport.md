---
title: Rendu du TP 3 – Systèmes concurents
author: Quentin [Gliech]{.smallcaps}
lang: fr
---

# Introduction

La gestion des sources s'est faite sur `git`, et vous pouvez le trouver sur GitHub[^1]

Tout ce qui est demandé dans le sujet a été implémenté. J'ai testé le projet sur macOS, Linux (turing), FreeBSD et OpenBSD.
Sur ces deux dernières plateformes, il faut cependant utiliser `GNU Make`.

J'ai d'ailleurs remarqué un problème sur OpenBSD: un appel à `semctl(id, n, GETZCNT)` semble toujours renvoyer zéro. 
Je n'ai pas encore investigué si c'est un problème de permission ou autre chose.

Ce rapport est écrit en markdown et généré avec pandoc (`>=2.0`), et l'archive est généré avec `make dist`.

[^1]: <https://github.com/sandhose/sc-musee/>

# Sémaphores et mémoire partagée

Les seules informations stockées dans l'espace de mémoire partagé sont la capacité du musée et la capacité de sa file d'attente, le tout englobé dans un `struct musee`.

4 sémaphores sont utilisés, et leurs identifiants sont déclarés dans des constantes de `musee.h`

 - `SEM_CLOSED` est à zéro si le musée est ouvert, 1 sinon.
 - `SEM_SLEEP` est à zéro quand le contrôleur est en train de travailleur, 1 s'il est prêt à accepter du travail.
 - `SEM_CAPACITY` compte d'entrées possibles dans l'immédiat.
   - il est mis à `musee->capacite` à l'ouverture du musée par le contrôleur
   - les visiteurs le décrémentent quand ils rentrent
   - le contrôleur le ré-incrémente lorsqu'un visiteur sort
   - il est mis à zéro par le contrôleur à la fermeture
 - `SEM_INSIDE` compte le nombre de visiteurs à l'intérieur
   - il est incrémenté par le visiteur quand il rentre
   - décrémenté quand il sort
   - vérifié à chaque boucle par le contrôleur pour éventuellement ré-incrémenter la capacité


# Macros et logs

Les logs sont au format `[niveau] [fichier]:[ligne] [message]` (et en couleur), avec `niveau` pouvant être `FATAL`, `ERROR`, `WARN`, `INFO` ou `DEBUG`.

Chaque niveau de message a une macro avec (ex: `DEBUGF(message, ...)`) et sans format (ex: `DEBUG(message)`).

Pour afficher correctement le numéro de ligne et le fichier concerné, ces messages sont fait dans des macros, en utilisant les variables `__FILE__` et `__LINE__`.

Les macros sont construits de sorte à ce qu'ils ne fassent qu'un seul appel à `fprintf` avec une chaine concaténée à la compilation.

`DEBUGF("semid = %d", semid)` appellera donc `fprintf(stderr, "DEBUG fichier.c:12 semid = %d\n", semid)` (modulo la couleur, les tabulations et la vérification de la variable d'environnement).

---

La macro `TRY(condition, message)` va vérifier que la `condition` est vraie, et si ce n'est pas le cas, `exit` le programme en appelant `perror` avant.

`TRY_SYS(valeur, msg)` appelle `TRY` avec comme condition `valeur != -1`, utile pour tous les appels aux primitives système et fonctions de bibliothèque.

La macro `TRY_USAGE(condition, message)` fonctionne de la même manière que `TRY`, mais affiche l'`usage` plutôt qu'un message de `perror`.

---

`SEMOPS` est une macro pour simplifier les appels à `semop`, en créant à la volée la structure `sembuf` et en remplissant automatiquement le troisième argument de `semop`.

La seule limitation de cette macro est qu'on ne peut récupérer la valeur de retour.
Ce n'est pas un problème, sauf une fois à la fin du `visiteur`.


# Tests ajoutés

Tous les tests fonctionnent fournis fonctionnent.

J'ai ajouté un test qui teste la sortie de `dump`, et vérifie (entre autre) que le contrôleur ne s'arrête pas tant qu'il y a encore des visiteurs à la fermeture.

Les tests couvrent plus de 95% du code.
