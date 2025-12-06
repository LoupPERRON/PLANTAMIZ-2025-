Plantamitz - Jeu console en C

Description:
- Implémentation minimaliste en C du jeu inspiré de FarmHeroes/CandyCrush.
- Plateau 10x10, items: S,F,P,O,M.
- Contrats: 3 niveaux (codés en dur) de difficulté croissante.

Build & Run (Windows, MinGW / gcc):

Compile:
    gcc -o plantamitz.exe plantamitz/*.c -Wall -O2

Run:
    ./plantamitz.exe

Notes:
- Le projet utilise `conio.h` (kbhit/getch) et l'API Windows (`windows.h`) pour couleur
  et position du curseur; compiler avec MinGW ou Code::Blocks sur Windows.
- Vous pouvez pousser ces fichiers sur votre dépôt GitHub.
