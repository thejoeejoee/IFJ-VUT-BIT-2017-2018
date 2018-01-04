# IFJ-VUT-BIT-2017-2018

[![Build Status](https://travis-ci.com/thejoeejoee/IFJ-VUT-BIT-2017-2018.svg?token=gqhWb2e43GFsqhjPPAw1&branch=develop)](https://travis-ci.com/thejoeejoee/IFJ-VUT-BIT-2017-2018)
[![codecov](https://codecov.io/gh/thejoeejoee/IFJ-VUT-BIT-2017-2018/branch/develop/graph/badge.svg?token=H6Cdiv7Mov)](https://codecov.io/gh/thejoeejoee/IFJ-VUT-BIT-2017-2018)
[![Benchmark](https://img.shields.io/badge/benchmark-view-blue.svg)](https://plot.ly/~thejoeejoee/)
[![Join the chat at https://gitter.im/IFJ-VUT-BIT-2017-2018/](https://img.shields.io/badge/GITTER-join%20chat-green.svg)](https://gitter.im/IFJ-VUT-BIT-2017-2018/)

Repository for program interpret of custom language IFJ2017 written in C tested under Google tests.

Authors
------------ 
* Tomáš Pazdiora
* Son Hai Nguyen
* Josef Kolář
* Martin Kobelka

Výsledky
------------ 
* Lexikální analýza (detekce chyb): 94% (193/204)
* Syntaktická analýza (detekce chyb): 100% (213/213)
* Sémantická analýza (detekce chyb): 96% (304/316)
* Běhové chyby (detekce): 100% (20/20)
* Interpretace přeloženého kódu: 99% (811/813)

Výsledky pro rozšíření
------------ 
* CYCLES 0% (0/150)
* UNARY 80% (40/50)
* BASE 100% (50/50)
* FUNEXP 100% (150/150)
* BOOLOP 100% (100/100)
* IFTHEN 100% (50/50)
* GLOBAL 100% (100/100)
* SCOPE 100% (100/100)

Celkem bez rozšíření: 98% (1521/1546)

Rychlostní soutěž
------------ 

Překladač se umístil na prvním místě rychlostní soutěže. Byla testována efektivita vygenerovaného kódu měřená cenou instrukce.

1) xkobel02 - 59 511 913
2) xxxxxxdd - 87 847 120
3) xxxxxxdd - 89 016 483
4) xxxxxxdd - 94 992 663
5) xxxxxxdd - 95 679 957
