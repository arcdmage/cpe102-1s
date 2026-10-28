#include <iostream>
#include <conio.h>   // for _getch() (reads a key without pressing Enter)
#include <string>
#include <cstdlib>   // for rand(), srand(), system()
#include <ctime>     // for time()

using namespace std;

// ============================================================
//  PART 1: MAP AND PLAYER POSITION
// ============================================================
const int MAP_WIDTH = 5;
const int MAP_HEIGHT = 5;

// 0 = Walkable Corridor, 1 = Solid Wall Block
int mapData[MAP_HEIGHT][MAP_WIDTH] = {
    {0, 0, 0, 0, 0},
    {0, 1, 0, 1, 0},
    {0, 0, 0, 0, 0},
    {0, 1, 0, 1, 0},
    {0, 0, 0, 0, 0}
};

int posX = 0;
int posY = 0;

// 0 = North, 1 = East, 2 = South, 3 = West
int dir = 1;
int dx[] = {0, 1, 0, -1};
int dy[] = {-1, 0, 1, 0};

bool isWall(int x, int y) {
    if (x < 0 || x >= MAP_WIDTH || y < 0 || y >= MAP_HEIGHT) return true;
    return mapData[y][x] == 1;
}

// ============================================================
//  PART 2: PLAYER AND ENEMY DATA
// ============================================================

// --- Player stats ---
int playerHp = 100;
int playerMaxHp = 100;
int playerMp = 30;
int playerMaxMp = 30;
int potions = 3;

// --- Enemy stats ---
// Each enemy has a number (0, 1, 2). Enemy number 0 uses slot [0]
// in every array below, enemy number 1 uses slot [1], and so on.
const int NUM_ENEMIES = 3;

string enemyName[NUM_ENEMIES]  = {"Pixie", "Jack Frost", "Black Ooze"};
int enemyX[NUM_ENEMIES]        = {3, 2, 4};     // where they stand on the map
int enemyY[NUM_ENEMIES]        = {0, 2, 4};
int enemyHp[NUM_ENEMIES]       = {30, 45, 60};
int enemyMaxHp[NUM_ENEMIES]    = {30, 45, 60};
int enemyAtk[NUM_ENEMIES]      = {8, 11, 14};   // how hard they hit
bool enemyWeakToFire[NUM_ENEMIES] = {true, true, true};
bool enemyAlive[NUM_ENEMIES]   = {true, true, true};

// Gives a random number from a to b (including both)
int randomNumber(int a, int b) {
    return a + rand() % (b - a + 1);
}

// Checks if a living enemy is standing at (x, y).
// Returns the enemy number, or -1 if nobody is there.
int enemyAt(int x, int y) {
    for (int i = 0; i < NUM_ENEMIES; i++) {
        if (enemyAlive[i] == true && enemyX[i] == x && enemyY[i] == y) {
            return i;
        }
    }
    return -1;
}

// Looks down the corridor you are facing (up to 3 tiles).
// The results are saved in these two variables:
int visibleEnemy = -1;     // which enemy you can see (-1 = none)
int visibleDistance = 0;   // how many tiles away it is

void lookForEnemy() {
    visibleEnemy = -1;
    visibleDistance = 0;

    for (int d = 1; d <= 3; d++) {
        int x = posX + dx[dir] * d;
        int y = posY + dy[dir] * d;

        if (isWall(x, y)) return;   // a wall blocks your view

        int found = enemyAt(x, y);
        if (found != -1) {
            visibleEnemy = found;
            visibleDistance = d;
            return;
        }
    }
}

// ============================================================
//  PART 3: DRAWING HELPERS
// ============================================================

// NOTE: In C++ text, a backslash must be written twice ("\\")
// to show a single "\" on the screen.

// Prints one row INSIDE the small inner box of the corridor.
// The text is centered automatically. (inner width = 58)
void printInnerRow(string text) {
    int width = 58;
    int len = text.length();
    int leftSpaces = (width - len) / 2;
    int rightSpaces = width - len - leftSpaces;

    cout << "|    |";
    for (int i = 0; i < leftSpaces; i++) cout << " ";
    cout << text;
    for (int i = 0; i < rightSpaces; i++) cout << " ";
    cout << "|    |" << endl;
}

// Prints one row across the WHOLE frame, text centered. (width = 68)
void printFullRow(string text) {
    int width = 68;
    int len = text.length();
    int leftSpaces = (width - len) / 2;
    int rightSpaces = width - len - leftSpaces;

    cout << "|";
    for (int i = 0; i < leftSpaces; i++) cout << " ";
    cout << text;
    for (int i = 0; i < rightSpaces; i++) cout << " ";
    cout << "|" << endl;
}

void printBorder() {
    cout << "+--------------------------------------------------------------------+" << endl;
}

// Top half of the corridor drawing
void printCorridorTop() {
    printBorder();
    cout << "| \\                                                                / |" << endl;
    cout << "|  \\                                                              /  |" << endl;
    cout << "|   \\                                                            /   |" << endl;
    cout << "|    +----------------------------------------------------------+    |" << endl;
}

// Bottom half of the corridor drawing
void printCorridorBottom() {
    cout << "|    +----------------------------------------------------------+    |" << endl;
    cout << "|   /                                                            \\   |" << endl;
    cout << "|  /                                                              \\  |" << endl;
    cout << "| /                                                                \\ |" << endl;
    printBorder();
    cout << endl;
}

// Draws the BIG enemy filling the screen.
// Used when an enemy is right in front of you, and in battle.
void printBigEnemyScreen() {
    printBorder();
    printFullRow("");
    printFullRow("");
    printFullRow(" /\\       /\\ ");
    printFullRow("/  \\_____/  \\");
    printFullRow("|  O     O  |");
    printFullRow("|     v     |");
    printFullRow(" \\  \\___/  / ");
    printFullRow("  \\_|||||_/  ");
    printFullRow("  / ||||| \\  ");
    printFullRow(" /__|||||__\\ ");
    printFullRow("");
    printFullRow("");
    printFullRow("");
    printBorder();
    cout << endl;
}

// Draws what you see in front of you while exploring
void renderPerspective(bool wallAhead) {
    // Wall right in front of you: just an empty box
    if (wallAhead) {
        printBorder();
        for (int i = 0; i < 13; i++) printFullRow("");
        printBorder();
        cout << endl;
        return;
    }

    lookForEnemy();

    // Enemy 1 tile away: show the big enemy
    if (visibleDistance == 1) {
        printBigEnemyScreen();
        return;
    }

    // Otherwise draw the corridor, with a medium/small enemy inside if visible
    printCorridorTop();

    if (visibleDistance == 2) {
        // medium enemy (5 rows tall)
        printInnerRow("/\\   /\\");
        printInnerRow("|O   O|");
        printInnerRow("| \\_/ |");
        printInnerRow(" ||||| ");
        printInnerRow(" /   \\ ");
    }
    else if (visibleDistance == 3) {
        // small enemy (3 rows tall)
        printInnerRow("");
        printInnerRow("/\\_/\\");
        printInnerRow("(o o)");
        printInnerRow(" /|\\ ");
        printInnerRow("");
    }
    else {
        // nobody there: empty corridor
        for (int i = 0; i < 5; i++) printInnerRow("");
    }

    printCorridorBottom();
}

void renderMap() {
    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            if (x == posX && y == posY) {
                char pChar = '^';
                if (dir == 1) pChar = '>';
                else if (dir == 2) pChar = 'v';
                else if (dir == 3) pChar = '<';
                cout << pChar << " ";
            }
            else if (enemyAt(x, y) != -1) {
                cout << "E ";
            }
            else if (mapData[y][x] == 1) {
                cout << "# ";
            }
            else {
                cout << ". ";
            }
        }
        cout << endl;
    }
    cout << endl;
}

// Draws a health bar like [#####.....] with 20 slots
void printBar(int current, int maximum) {
    if (current < 0) current = 0;

    int filled = current * 20 / maximum;
    if (current > 0 && filled == 0) filled = 1;   // show at least 1 if alive

    cout << "[";
    for (int i = 0; i < 20; i++) {
        if (i < filled) cout << "#";
        else cout << ".";
    }
    cout << "]";
}

// ============================================================
//  PART 4: TURN-BASED COMBAT
// ============================================================

// The message box in battle holds 4 lines of text
string logLines[4];

// Adds a new message at the bottom and pushes the old ones up
void addLog(string text) {
    for (int i = 0; i < 3; i++) {
        logLines[i] = logLines[i + 1];
    }
    logLines[3] = text;
}

// Draws the whole battle screen for enemy number e
void drawBattle(int e) {
    system("cls");

    printBigEnemyScreen();

    // Enemy name and HP
    int shownHp = enemyHp[e];
    if (shownHp < 0) shownHp = 0;
    cout << "  " << enemyName[e] << "  HP ";
    printBar(enemyHp[e], enemyMaxHp[e]);
    cout << " " << shownHp << "/" << enemyMaxHp[e] << endl;
    cout << endl;

    // Message box
    cout << "  ----------------------------------------------------------------" << endl;
    for (int i = 0; i < 4; i++) {
        cout << "  " << logLines[i] << endl;
    }
    cout << "  ----------------------------------------------------------------" << endl;

    // Player HP and MP
    int shownPlayerHp = playerHp;
    if (shownPlayerHp < 0) shownPlayerHp = 0;
    cout << "  YOU  HP ";
    printBar(playerHp, playerMaxHp);
    cout << " " << shownPlayerHp << "/" << playerMaxHp << endl;
    cout << "       MP ";
    printBar(playerMp, playerMaxMp);
    cout << " " << playerMp << "/" << playerMaxMp << endl;
    cout << endl;

    cout << "  [1] Attack   [2] Agi (8 MP)   [3] Guard   [4] Potion x"
         << potions << "   [5] Run" << endl;
}

// Fights enemy number e.
// Returns:  1 = you won,  2 = you ran away,  3 = you died
int fight(int e) {
    // clear the message box
    for (int i = 0; i < 4; i++) logLines[i] = "";
    addLog("A " + enemyName[e] + " blocks your path!");

    while (true) {
        drawBattle(e);

        char key = _getch();
        bool usedTurn = false;    // did the player actually do something?
        bool guarding = false;    // is the player guarding this turn?

        // ---------- YOUR TURN ----------
        if (key == '1') {
            // ATTACK
            usedTurn = true;
            if (randomNumber(1, 100) <= 10) {
                addLog("You swing and miss!");
            }
            else {
                int damage = randomNumber(8, 14);
                string extra = "";
                if (randomNumber(1, 100) <= 15) {       // 15% critical hit
                    damage = damage * 3 / 2;
                    extra = " Critical hit!";
                }
                enemyHp[e] = enemyHp[e] - damage;
                addLog("You strike " + enemyName[e] + " for " + to_string(damage) + "." + extra);
            }
        }
        else if (key == '2') {
            // AGI (fire spell)
            if (playerMp < 8) {
                addLog("Not enough MP!");
            }
            else {
                usedTurn = true;
                playerMp = playerMp - 8;
                int damage = randomNumber(18, 26);
                string extra = "";
                if (enemyWeakToFire[e] == true) {
                    damage = damage * 3 / 2;
                    extra = " Weak point!";
                }
                enemyHp[e] = enemyHp[e] - damage;
                addLog("You cast Agi! " + to_string(damage) + " fire damage." + extra);
            }
        }
        else if (key == '3') {
            // GUARD
            usedTurn = true;
            guarding = true;
            addLog("You brace yourself.");
        }
        else if (key == '4') {
            // POTION
            if (potions <= 0) {
                addLog("No potions left!");
            }
            else {
                usedTurn = true;
                potions = potions - 1;
                playerHp = playerHp + 40;
                if (playerHp > playerMaxHp) playerHp = playerMaxHp;
                addLog("You drink a potion and recover 40 HP.");
            }
        }
        else if (key == '5') {
            // RUN (50% chance)
            if (randomNumber(1, 100) <= 50) {
                addLog("You escaped!");
                drawBattle(e);
                cout << "  (press any key)" << endl;
                _getch();
                return 2;
            }
            else {
                usedTurn = true;
                addLog("You couldn't get away!");
            }
        }

        // If the player pressed a wrong key, let them choose again
        if (usedTurn == false) {
            continue;
        }

        // ---------- DID THE ENEMY DIE? ----------
        if (enemyHp[e] <= 0) {
            addLog(enemyName[e] + " is defeated!");
            drawBattle(e);
            cout << "  (press any key)" << endl;
            _getch();
            return 1;
        }

        // ---------- ENEMY'S TURN ----------
        int enemyDamage = randomNumber(enemyAtk[e] - 2, enemyAtk[e] + 3);

        if (randomNumber(1, 100) <= 70) {
            // normal attack
            if (guarding == true) enemyDamage = enemyDamage / 2;
            addLog(enemyName[e] + " attacks! You take " + to_string(enemyDamage) + ".");
        }
        else {
            // heavy attack (60% stronger)
            enemyDamage = enemyDamage * 8 / 5;
            if (guarding == true) enemyDamage = enemyDamage / 2;
            addLog(enemyName[e] + " lands a heavy blow! You take " + to_string(enemyDamage) + ".");
        }
        playerHp = playerHp - enemyDamage;

        // ---------- DID THE PLAYER DIE? ----------
        if (playerHp <= 0) {
            addLog("You have fallen...");
            drawBattle(e);
            cout << "  (press any key)" << endl;
            _getch();
            return 3;
        }
    }
}

// ============================================================
//  PART 5: MAIN GAME LOOP
// ============================================================
int main() {
    srand(time(0));   // makes the random numbers different every run

    while (true) {
        system("cls");

        int nextX = posX + dx[dir];
        int nextY = posY + dy[dir];

        bool wallAhead = isWall(nextX, nextY);

        renderPerspective(wallAhead);
        renderMap();

        cout << "HP: " << playerHp << "/" << playerMaxHp
             << "  MP: " << playerMp << "/" << playerMaxMp
             << "  Potions: " << potions << endl;
        cout << "Controls: [W] Forward | [S] Backward | [A] Turn Left | [D] Turn Right | [Q] Quit" << endl;

        char input = _getch();

        // remember where we were, in case we run away from a fight
        int oldX = posX;
        int oldY = posY;
        bool moved = false;

        if (input == 'w' || input == 'W') {
            if (!wallAhead) {
                posX = nextX;
                posY = nextY;
                moved = true;
            }
        }
        else if (input == 's' || input == 'S') {
            int backX = posX - dx[dir];
            int backY = posY - dy[dir];
            if (!isWall(backX, backY)) {
                posX = backX;
                posY = backY;
                moved = true;
            }
        }
        else if (input == 'a' || input == 'A') {
            dir = (dir + 3) % 4;
        }
        else if (input == 'd' || input == 'D') {
            dir = (dir + 1) % 4;
        }
        else if (input == 'q' || input == 'Q') {
            break;
        }

        // Did we just walk onto an enemy? Then start a fight!
        if (moved == true) {
            int e = enemyAt(posX, posY);

            if (e != -1) {
                int result = fight(e);

                if (result == 1) {
                    // WON: remove the enemy
                    enemyAlive[e] = false;

                    // check if any enemies are left
                    bool anyLeft = false;
                    for (int i = 0; i < NUM_ENEMIES; i++) {
                        if (enemyAlive[i] == true) anyLeft = true;
                    }

                    if (anyLeft == false) {
                        system("cls");
                        cout << "All enemies defeated. You cleared the corridors!" << endl;
                        _getch();
                        break;
                    }
                }
                else if (result == 2) {
                    // RAN AWAY: go back to the previous tile
                    posX = oldX;
                    posY = oldY;
                }
                else {
                    // DIED
                    system("cls");
                    cout << "GAME OVER" << endl;
                    _getch();
                    break;
                }
            }
        }
    }

    return 0;
}
