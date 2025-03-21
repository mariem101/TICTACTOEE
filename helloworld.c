//
// Created by mariem on 20/03/25.
//

#include <stdlib.h>

#include "curses.h"
#include "ncurses_dll.h"
#include <string.h>
#include <time.h>
#include <ncurses.h>


#define NLINE 3
#define NCOL 3

char plateau[NLINE][NCOL];
int centerX, centerY;

// Réinitialise la grille
void resetPlateau() {
    for (int i = 0; i < NLINE; i++)
        for (int j = 0; j < NCOL; j++)
            plateau[i][j] = ' ';
}

// Affiche la grille en plein écran sous forme de "fenêtre"
void afficherPlateau() {
    clear();
    int cellWidth = (COLS - 4) / NCOL;    // Largeur des grandes cases
    int cellHeight = (LINES - 4) / NLINE; // Hauteur des grandes cases

    for (int i = 0; i < NLINE; i++) {
        for (int j = 0; j < NCOL; j++) {
            int startY = i * cellHeight;
            int startX = j * cellWidth;

            // Dessine les bordures des rectangles pour les cellules internes et évite les bordures externes
            for (int y = 0; y < cellHeight; y++) {
                for (int x = 0; x < cellWidth; x++) {
                    // Vérifie si c'est une ligne de bordure interne partagée
                    bool isTopBorder = (y == 0 && i > 0);
                    bool isBottomBorder = (y == cellHeight - 1 && i < NLINE - 1);
                    bool isLeftBorder = (x == 0 && j > 0);
                    bool isRightBorder = (x == cellWidth - 1 && j < NCOL - 1);

                    // Coins internes uniquement
                    if (((isTopBorder || isBottomBorder) && (isLeftBorder || isRightBorder))) {
                        attron(COLOR_PAIR(2));
                        mvprintw(startY + y, startX + x, "*");
                        attroff(COLOR_PAIR(2));
                    }
                    // Lignes horizontales internes
                    else if (isTopBorder || isBottomBorder) {
                        attron(COLOR_PAIR(1));
                        mvprintw(startY + y, startX + x, "-");
                        attroff(COLOR_PAIR(1));
                    }
                    // Lignes verticales internes
                    else if (isLeftBorder || isRightBorder) {
                        attron(COLOR_PAIR(1));
                        mvprintw(startY + y, startX + x, "|");
                        attroff(COLOR_PAIR(1));
                    }
                }
            }

            // Place le symbole centré dans la case
            if (plateau[i][j] != ' ') {
                if (plateau[i][j] == 'X') {
                    attron(COLOR_PAIR(3));
                    mvprintw(startY + cellHeight / 2, startX + cellWidth / 2, "%c", plateau[i][j]);
                    attroff(COLOR_PAIR(3));
                } else {
                    attron(COLOR_PAIR(4));
                    mvprintw(startY + cellHeight / 2, startX + cellWidth / 2, "%c", plateau[i][j]);
                    attroff(COLOR_PAIR(4));
                }
            }
        }
    }
}

// Vérifie les conditions de victoire
char verifierVictoire() {
    for (int i = 0; i < NLINE; i++) {
        if (plateau[i][0] == plateau[i][1] && plateau[i][1] == plateau[i][2] && plateau[i][0] != ' ')
            return plateau[i][0];
        if (plateau[0][i] == plateau[1][i] && plateau[1][i] == plateau[2][i] && plateau[0][i] != ' ')
            return plateau[0][i];
    }
    if (plateau[0][0] == plateau[1][1] && plateau[1][1] == plateau[2][2] && plateau[0][0] != ' ')
        return plateau[0][0];
    if (plateau[0][2] == plateau[1][1] && plateau[1][1] == plateau[2][0] && plateau[0][2] != ' ')
        return plateau[0][2];
    return ' ';
}

// Vérifie si c'est un match nul
int estMatchNul() {
    for (int i = 0; i < NLINE; i++)
        for (int j = 0; j < NCOL; j++)
            if (plateau[i][j] == ' ') return 0;
    return 1;
}

// IA joue un coup aléatoire
void iaJoue(char symbole) {
    int x, y;
    do {
        x = rand() % NLINE;
        y = rand() % NCOL;
    } while (plateau[y][x] != ' ');

    plateau[y][x] = symbole;
}

// Logique principale du jeu
void jouerMorpion(int mode) {
    resetPlateau();
    int x = 0, y = 0;
    char joueur = 'X';

    while (1) {
        int coupJoue = 0;

        afficherPlateau();
        mvprintw(LINES - 3, 2, "Joueur: %c | Utilisez les fleches + Entrée pour jouer", joueur);
        move(centerY + y, centerX + x);
        refresh();

        if ((mode == 1 && joueur == 'X') || mode == 3) { // Joueur humain
            int touche;
            while (1) {
                touche = getch();
                switch (touche) {
                    case KEY_UP: if (y > 0) y--; break;
                    case KEY_DOWN: if (y < NLINE - 1) y++; break;
                    case KEY_LEFT: if (x > 0) x--; break;
                    case KEY_RIGHT: if (x < NCOL - 1) x++; break;
                    case '\n':
                        if (plateau[y][x] == ' ') {
                            plateau[y][x] = joueur;
                            coupJoue = 1;
                        }
                        break;
                }
                if (coupJoue) break;
            }
        } else { // L'IA joue
            napms(500);
            iaJoue(joueur);
            coupJoue = 1;
        }

        // Vérifie s'il y a un gagnant
        if (verifierVictoire() != ' ') {
            afficherPlateau();
            attron(A_BOLD | COLOR_PAIR(6)); // Gras et couleur pour le message de victoire
            mvprintw(LINES - 2, 2, "Le joueur %c a gagné !", joueur);
            attroff(A_BOLD | COLOR_PAIR(6));
            getch();
            return;
        }
        if (estMatchNul()) {
            afficherPlateau();
            attron(A_BOLD | COLOR_PAIR(7)); // Gras et couleur pour le message de match nul
            mvprintw(LINES - 2, 2, "Match nul !");
            attroff(A_BOLD | COLOR_PAIR(7));
            getch();
            return;
        }

        if (coupJoue) {
            joueur = (joueur == 'X') ? 'O' : 'X';
        }
    }
}

// Fonction du menu
int afficherMenu() {
    int choix = 0;
    int menuHeight = 6; // Nombre de lignes du menu
    int menuWidth = 30; // Largeur du menu

    while (1) {
        clear();

        // Dessine la bordure autour du menu avec couleur
        for (int y = LINES / 2 - menuHeight / 2 - 1; y <= LINES / 2 + menuHeight / 2; y++) {
            for (int x = (COLS - menuWidth) / 2 - 1; x <= (COLS + menuWidth) / 2; x++) {
                if (y == LINES / 2 - menuHeight / 2 - 1 || y == LINES / 2 + menuHeight / 2) {
                    attron(COLOR_PAIR(5));
                    mvprintw(y, x, "-");
                    attroff(COLOR_PAIR(5));
                } else if (x == (COLS - menuWidth) / 2 - 1 || x == (COLS + menuWidth) / 2) {
                    attron(COLOR_PAIR(5));
                    mvprintw(y, x, "|");
                    attroff(COLOR_PAIR(5));
                }
            }
        }

        // Affiche le texte du menu à l'intérieur de la bordure
        attron(A_BOLD | COLOR_PAIR(6));
        mvprintw(LINES / 2 - 3, (COLS - menuWidth) / 2, "=== MENU DU MORPION ===");
        attroff(A_BOLD | COLOR_PAIR(6));

        mvprintw(LINES / 2, (COLS - menuWidth) / 2, "1. Joueur vs IA");
        mvprintw(LINES / 2 + 1, (COLS - menuWidth) / 2, "2. IA vs IA");
        mvprintw(LINES / 2 + 2, (COLS - menuWidth) / 2, "3. Joueur vs Joueur");

        attron(COLOR_PAIR(7));
        mvprintw(LINES / 2 + 3, (COLS - menuWidth) / 2, "4. Quitter");
        attroff(COLOR_PAIR(7));

        attron(COLOR_PAIR(8));
        mvprintw(LINES / 2 + 5, (COLS - menuWidth) / 2, "Choisissez une option: ");
        attroff(COLOR_PAIR(8));

        refresh();

        choix = getch() - '0';
        if (choix >= 1 && choix <= 4) return choix;
    }
}

int main() {
    initscr();
    keypad(stdscr, TRUE);
    noecho();
    curs_set(0);
    srand(time(NULL));

    start_color();
    init_pair(1, COLOR_WHITE, COLOR_BLACK);   // Bordure - par défaut
    init_pair(2, COLOR_CYAN, COLOR_BLACK);    // Bordure du menu
    init_pair(3, COLOR_GREEN, COLOR_BLACK);   // Joueur 'X'
    init_pair(4, COLOR_BLUE, COLOR_BLACK);    // Joueur 'O'
    init_pair(5, COLOR_MAGENTA, COLOR_BLACK); // Titre du menu
    init_pair(6, COLOR_YELLOW, COLOR_BLACK);  // Message de victoire
    init_pair(7, COLOR_RED, COLOR_BLACK);     // Message de match nul
    init_pair(8, COLOR_YELLOW, COLOR_BLACK);  // Message de choix d'option

    centerX = 0;  // Plein écran
    centerY = 0;

    while (1) {
        int mode = afficherMenu();
        if (mode == 4) break; // Quitter
        jouerMorpion(mode);
    }

    endwin();
    return 0;
}
