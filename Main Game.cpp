// Include the libraries we need.
// <iostream> lets us print text (cout) and read keyboard input (cin).
// <string> lets us use the "string" type to store words.
// <conio.h> (Windows only) gives us _getch(), which reads a key instantly, without waiting for the player to press ENTER.
// <cstdlib> provides a collection of general-purpose functions
// <cctype> provides character classification and conversion functions
#include <iostream>
#include <string>
#include <conio.h>
#include <cstdlib>
#include <cctype> 

using namespace std;

// A function that "clears" the screen.
  int readKey() { cout.flush(); return _getch(); }
  void clearScreen() { system("cls"); }

// A function that shows the instruction screen.
// It waits for ANY key press, then returns to whoever called it.
void showInstructions() {

    // CENTERING SETTINGS (same idea as drawMenu)
    int consoleWidth  = 100;   // How many characters wide your console is.
    int consoleHeight = 30;    // How many lines tall your console is.
    int frameWidth    = 48;    // How wide our frame is (count of characters).
    int contentHeight = 13;    // How many lines this screen takes up.
                               // Add 1 for every cout line you add!

    // Horizontal centering: split the leftover space in half.
    int leftPad = (consoleWidth - frameWidth) / 2;
    string pad(leftPad, ' ');   // A string made of "leftPad" spaces.

    // Vertical centering: split the leftover lines in half.
    int topPad = (consoleHeight - contentHeight) / 2;
    int bottomPad = consoleHeight - contentHeight - topPad - 1;   // -1 keeps us inside the window.

    clearScreen();

    // Empty lines above, to push the screen to the middle.
    for (int i = 0; i < topPad; i++) {
        cout << "\n";
    }

    cout << pad << "+======================================================================+\n";
    cout << pad << "|                           INSTRUCTIONS                               |\n";
    cout << pad << "+----------------------------------------------------------------------+\n";
    cout << "\n";

    // PASTE / EDIT YOUR INSTRUCTION LINES HERE
    cout << pad << "   1. Find out who stole the Moonlit Pearl Earrings by reading the case\n";
    cout << pad << "   2. file and questioning the four suspects.\n\n";
    cout << pad << "                             Controls\n";
    cout << pad << "   WASD  Move your detective (@)\n";
    cout << pad << "   E Talk to a suspect/examine the dresser ($) when next to it\n";
    cout << pad << "   C Open the Case File\n";
    cout << pad << "   N  Open Investigation Notes\n";
    cout << pad << "   X  Make your final Deduction\n"; 
	cout << pad << "   (available any time)\n";
    cout << pad << "   Q  Return to the main menu\n\n";   
    cout << pad << "                          QUESTIONING\n";
    cout << pad << "   1. Pick the best response to each statement (number keys).\n";
    cout << pad << "   2. correct = +50 points.  Wrong = -20 points, but you can try again.\n\n";
    cout << pad << "                           DEDUCTION\n";
    cout << pad << "   1. Choose the suspect, reasoning, motive and evidence. ALL FOUR must be\n";
    cout << pad << "   2. right to win (up to 1000 points). Any mistake = GAME OVER.\n";
    cout << pad << "   3. Interview everyone with few mistakes to earn an efficiency bonus!\n";
    // Kakapagod mag type tigil ko na dito XD dagdagan nyu nalang pag kaylangan

    cout << "\n";
    cout << pad << "+-----------------------------------------------------------------------+\n";
    cout << pad << "|                      PRESS ANY KEY TO CONTINUE                        |\n";
    cout << pad << "+-----------------------------------------------------------------------+\n";

    // Empty lines below, so the screen stays in the middle.
    for (int i = 0; i < bottomPad; i++) {
        cout << "\n";
    }

    _getch();   // Wait for any key (no ENTER needed).
}

//  A function that draws the whole menu.
// "selected" tells us which option is currently highlighted (0 to 3).
void drawMenu(int selected) {

    // An array holding the names of our menu options.
    string options[3] = { "INSTRUCTION", "START GAME", "QUIT" };
    
    // CENTERING SETTINGS
    int consoleWidth  = 100;   // How many characters wide your console is.
    int consoleHeight = 30;    // How many lines tall your console is.
    int frameWidth    = 48;    // How wide our menu frame is (count of characters).
    int menuHeight    = 16;    // How many lines our menu takes up.

    // Horizontal centering: split the leftover space in half.
    int leftPad = (consoleWidth - frameWidth) / 2;
    string pad(leftPad, ' ');   // A string made of "leftPad" spaces.

    // Vertical centering: split the leftover lines in half.
    int topPad = (consoleHeight - menuHeight) / 2;
    int bottomPad = consoleHeight - menuHeight - topPad - 1;   // -1 for the input line.

    // Print empty lines above the menu to push it down to the middle.
    for (int i = 0; i < topPad; i++) {
        cout << "\n";
    }
    
    // The top border and title.
    cout << pad << " _____ _            __  __                 _ _ _\n";
    cout << pad << "|_   _| |__   ___  |  \\/  | ___   ___  _ __| (_) |_\n";
    cout << pad << "  | | | '_ \\ / _ \\ | |\\/| |/ _ \\ / _ \\| '_ \\ | | __|\n";
    cout << pad << "  | | | | | |  __/ | |  | | (_) | (_) | | | | | | |_\n";
    cout << pad << "  |_| |_| |_|\\___| |_|  |_|\\___/ \\___/|_| |_|_|_|\\__|\n";
    cout << pad << "           H E I S T   -   a detective game\n\n";
    cout << pad << "                   /\\/\\/\\/\\/\\/\\/\\\n";
    cout << pad << "                   | [  ]  [  ] |\n";
    cout << pad << "                   |____[__]____|\n\n";
    cout << pad << "  |        Santos - Sales - Fabela - Nucum       |\n";
    cout << pad << "  +==============================================+\n";
    cout << "\n";

    
	
    //cout << pad << "////////// CRIME SCENE - DO NOT CROSS //////////\n";
    //cout << pad << "+==============================================+\n";
    //cout << pad << "| CASE FILE No. 00XX          [ CONFIDENTIAL ] |\n";
    //cout << pad << "+----------------------------------------------+\n";
    //cout << pad << "|                                              |\n";
    //cout << pad << "|               THE MOONLIT HEIST              |\n";
    //cout << pad << "|                                              |\n";
    //cout << pad << "+----------------------------------------------+\n";
    //cout << pad << "|        Santos - Sales - Fabela - Nucum       |\n";
    //cout << pad << "+==============================================+\n";
    //cout << "\n";

    
    // Loop through every option and print it.
    // "i" counts 0, 1, 2, 3 - one number per option.
    for (int i = 0; i < 3; i++) {

        // Each option is pushed further to the right than the one above it.
        // This creates the slanted "staircase" look.
        int indent = 6 + (i * 4);

        // Print the centering spaces, then the staircase spaces.
        cout << pad;
        for (int s = 0; s < indent; s++) {
            cout << " ";
        }

        // Is this the option the player is currently on?
        if (i == selected) {
            // YES: draw it big and boxed so it stands out.
            cout << "/////////////////////////\n";
            cout << pad;
            for (int s = 0; s < indent; s++) cout << " ";
            cout << "//>>  " << (i + 1) << ". " << options[i] << "  <<\n";
            cout << pad; 
            for (int s = 0; s < indent; s++) cout << " ";
            cout << "/////////////////////////\n";
        } else {
            // NO: draw it plain and quiet.
            cout << "     " << (i + 1) << ". " << options[i] << "\n";
        }
    }

    // Print empty lines below the menu so it stays in the middle.
    for (int i = 0; i < bottomPad; i++) {
        cout << "\n";
    }
}

// ---------- Constants ----------
const int MAP_W = 60;
const int MAP_H = 21;
const int NUM_SUSPECTS = 4;
const int NUM_EVIDENCE = 7;
const int MAX_NOTES = 30;

// ---------- The ASCII map (ARRAY) ----------
// '#' hedges/border   '/\' roof   '|' '-' '+' walls   '[ ]' furniture/doors
// '.' garden path     ' ' floor   '$' = dresser (crime scene)
const char* gameMap[MAP_H] = {
    "############################################################",
    "#.......###.....................................###........#",
    "#../\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\..#",
    "#..|                |          [#][#] |                 |..#",
    "#..| [=====]   [$]  |     [_____]     |              {*}|..#",
    "#..|                |                 |    [~~~~~~]     |..#",
    "#..|                |                 |                 |..#",
    "#..|                |                 |                 |..#",
    "#..+-----[  ]-------+-----[  ]--------+--------[  ]-----+..#",
    "#..|                          [|]                       |..#",
    "#..                                                     |..#",
    "#..|                                                    |..#",
    "#..+-----[  ]-------+-----[  ]--------+--------[  ]-----+..#",
    "#..|        [o][o]  |          [#][#] |                 |..#",
    "#..| [###]          | (O)         (O) |  [O] [O]        |..#",
    "#..|          [===] |                 |                 |..#",
    "#..|                |                 |            [=o] |..#",
    "#..|                |                 |                 |..#",
    "#..+----------------+-----------------+-----------------+..#",
    "#.......###.....................................###........#",
    "############################################################",
};

// ---------- Data structures ----------
struct Statement {
    string testimony;
    string options[3];   // fixed questioning options
    int correct;         // index (0-2) of the correct option
    string response;     // what the suspect says after the correct choice
    int evidence;        // evidence unlocked (-1 = none)
    string note;         // detective note added
};

struct Suspect {
    string name;
    string role;
    Statement st[2];
    int state;           // 0 = not interviewed, 1 = currently, 2 = previously
    int progress;        // statements completed
    int x, y;            // position on the map
    char symbol;
};

struct Evidence {
    string name;
    string desc;
    bool found;
};

// ---------- Game data (ARRAYS) ----------
Suspect suspects[NUM_SUSPECTS];
Evidence evidenceList[NUM_EVIDENCE];
string notes[MAX_NOTES];
int noteCount = 0;
int correctCount = 0;   // correct questioning choices
int wrongCount = 0;     // incorrect questioning choices

// Deduction answer choices (ARRAYS)
const string reasonOptions[4] = {
    "The window was latched, so the partner must have hidden the earrings himself.",
    "The butler holds a master key, so he alone could have entered the room.",
    "The squeaky cart (9:10) and staff key use (9:15) point to the maid, and her alibi has no witness.",
    "The chef was alone the longest and nobody could watch her."
};
const string motiveOptions[4] = {
    "She needed money to pay her mother's overdue hospital bills.",
    "She wanted revenge on the owner for cutting her pay.",
    "She was jealous of the owner's lavish lifestyle.",
    "She lost her savings gambling."
};
const string evidenceOptions[4] = {
    "The empty velvet jewelry box",
    "The pawnshop slip and hospital bill found in her apron",
    "Damian's phone call log",
    "The latched window"
};
const int CORRECT_SUSPECT = 3, CORRECT_REASON = 2, CORRECT_MOTIVE = 0, CORRECT_EVIDENCE = 1;

// ============================================================
//  Utility functions
// ============================================================
void waitKey() {
    cout << "\n  [Press any key to continue]";
    if (readKey() == -1) exit(0);
}

// Reads one key until it is a valid number. INPUT VALIDATION.
int askChoice(int maxChoice, bool allowZero) {
    while (true) {
        int k = readKey();
        if (k == -1) exit(0);                       // input closed
        int n = k - '0';
        if (k >= '0' && k <= '9' && n <= maxChoice && (n >= 1 || (allowZero && n == 0)))
            return n;
        cout << "\r  Invalid input! Choose " << (allowZero ? 0 : 1) << "-" << maxChoice << ": " << flush;
    }
}

void printHeader(const string& title) {
    cout << "============================================================\n";
    cout << "  " << title << "\n";
    cout << "============================================================\n";
}

int countEvidence() {
    int n = 0;
    for (int i = 0; i < NUM_EVIDENCE; i++)
        if (evidenceList[i].found) n++;
    return n;
}

void addNote(const string& text) {
    if (noteCount < MAX_NOTES) notes[noteCount++] = text;
}

string stateText(int s) {
    if (s == 0) return "Not yet interviewed";
    if (s == 1) return "Currently interviewing";
    return "Previously interviewed";
}

bool allInterviewed() {
    for (int i = 0; i < NUM_SUSPECTS; i++)
        if (suspects[i].state != 2) return false;
    return true;
}

// ---------- Scoring ----------
int investigationScore() {
    int s = correctCount * 50 - wrongCount * 20;   // +50 correct, -20 wrong
    if (s < 0) s = 0;
    return s;
}

int efficiencyBonus() {            // up to +100, only if everyone was interviewed
    if (!allInterviewed()) return 0;
    int b = 100 - wrongCount * 10;
    if (b < 0) b = 0;
    return b;
}

// ============================================================
//  Game setup
// ============================================================
void resetGame() {
    noteCount = 0;
    correctCount = 0;
    wrongCount = 0;
    for (int i = 0; i < MAX_NOTES; i++) notes[i] = "";

    evidenceList[0] = {"Empty Velvet Box", "The jewelry box on the master dresser is empty.", true};
    evidenceList[1] = {"Latched Window", "The bedroom window was latched from inside. The thief used the door.", true};
    evidenceList[2] = {"Door Log", "Master bedroom door was opened with a STAFF KEY at 9:15 PM. It is locked otherwise.", true};
    evidenceList[3] = {"Phone Call Log", "Study phone log shows Damian on a business call from 8:50 to 10:00 PM.", false};
    evidenceList[4] = {"Key Register", "Only two staff keys exist: Edmund's, and one on the maid's cleaning ring.", false};
    evidenceList[5] = {"Squeaky Cart", "Rosa heard Lily's cleaning cart squeak down the back corridor at 9:10 PM.", false};
    evidenceList[6] = {"Pawnshop Slip", "A pawnshop appointment slip for 'pearl earrings' and an overdue hospital bill, found in Lily's apron.", false};

    suspects[0] = {"Damian Cross", "Owner's Partner", {
        {"I last saw the earrings at 7:45 PM, safe in the box. After that I was in the study on a long business call until about 10 PM.",
         {"Then you had the perfect time to sneak upstairs.",
          "Can anything prove you were on that call?",
          "Skip it. Let's talk about something else."},
         1, "Check the study phone log, detective. I was connected from 8:50 to 10:00 PM without a break.", 3,
         "Damian was on a recorded phone call 8:50-10:00 PM. His alibi holds."},
        {"Vivienne is in Paris until next week. Only the staff get near her room. I certainly don't need her jewelry.",
         {"Who is allowed into the master bedroom, then?",
          "Then you must have hired someone to take it.",
          "Maybe you are the one hiding something."},
         0, "The door stays locked. Only the staff keys open it. Ask Edmund who holds them.", -1,
         "The master bedroom is always locked. Only staff keys open it."}
    }, 0, 0, 29, 6, 'D'};

    suspects[1] = {"Edmund Price", "Butler", {
        {"I found the box empty at 10:30 PM and called you at once. The window was latched from inside, so no one climbed in.",
         {"A latched window means nobody was ever inside.",
          "Why did you go in and touch the box?",
          "Was the door locked before 10:30, and who holds keys?"},
         2, "Locked, sir. Only a staff key opens it. I carry one. The other hangs on the maid's cleaning ring. No one else.", 4,
         "Only two staff keys exist: Edmund's and the one on Lily's cleaning ring."},
        {"From 8:55 to 9:40 PM I was in the wine cellar counting bottles for tomorrow's dinner. My key never left my belt.",
         {"Hand over your keys right now.",
          "Is there anything that confirms you were in the cellar?",
          "Butlers always do it. Case closed."},
         1, "The cellar inventory sheet is stamped 8:55 and 9:40. I was nowhere near the master bedroom.", -1,
         "Edmund was in the wine cellar 8:55-9:40 PM (stamped sheet). Cleared for 9:15 PM."}
    }, 0, 0, 29, 16, 'E'};

    suspects[2] = {"Rosa Mendez", "Chef", {
        {"I was in the kitchen all evening, finishing tomorrow's dessert prep. I never go upstairs.",
         {"Did you hear anything unusual in the back corridor tonight?",
          "What kind of dessert was it?",
          "You have no witnesses. You're our thief."},
         0, "Now that you ask... around 9:10 PM I heard a cart squeaking past the kitchen door, heading toward the bedrooms.", 5,
         "Rosa heard a squeaky cleaning cart at about 9:10 PM, heading toward the bedrooms."},
        {"I didn't see who pushed it. I was busy at the oven, and that cart is kept by the laundry room.",
         {"You're lying about the oven.",
          "Perhaps the cart rolled by itself.",
          "Who is assigned to that cart?"},
         2, "Only the maid, Lily. She complains every day that the wheel needs oil.", -1,
         "The squeaky cart is Lily's. Nobody else uses it."}
    }, 0, 0, 11, 16, 'R'};

    suspects[3] = {"Lily Santos", "Maid", {
        {"I cleaned the master bedroom at 6 PM and locked up. After that I was folding laundry, alone, until 10 PM.",
         {"Fine, the laundry alibi sounds solid.",
          "Your cart squeaked by at 9:10 and a staff key opened the bedroom at 9:15. Explain.",
          "How do you like working here?"},
         1, "I... I left the cart in the corridor! Someone borrowed it! My key ring was in the laundry room! You can't prove anything!", -1,
         "Lily's alibi has no witness, and her cart and key match the 9:10-9:15 PM timing."},
        {"Yes, we have debts. My mother's hospital bills are heavy. But I would never steal from Madam Vivienne!",
         {"Then you're free to go. Thank you.",
          "Please empty your apron pockets, Lily.",
          "Hospitals are expensive everywhere, aren't they?"},
         1, "(Her hands shake. She pulls out an overdue hospital bill and a pawnshop slip for pearl earrings.) I only meant to sell them to save my mother...", 6,
         "Lily had a pawnshop appointment slip for pearl earrings and an overdue hospital bill in her apron."}
    }, 0, 0, 47, 16, 'L'};
}

// ============================================================
//  Map helpers
// ============================================================
string getRoomName(int x, int y) {
    if (x < 3 || x > 56 || y < 3 || y > 18) return "Manor Garden";
    if (y == 8 || y == 12) return "Doorway";
    if (y <= 7) {
        if (x < 20) return "Master Bedroom (Crime Scene)";
        if (x < 38) return "Study";
        return "Parlor";
    }
    if (y <= 11) return "Grand Hall";
    if (x < 20) return "Kitchen";
    if (x < 38) return "Wine Cellar";
    return "Laundry Room";
}

bool canMove(int x, int y) {
    if (x < 0 || x >= MAP_W || y < 0 || y >= MAP_H) return false;
    char c = gameMap[y][x];
    if (c != ' ' && c != '.') return false;
    for (int i = 0; i < NUM_SUSPECTS; i++)
        if (suspects[i].x == x && suspects[i].y == y) return false;
    return true;
}

bool isNear(int px, int py, int x, int y) {
    return abs(px - x) <= 1 && abs(py - y) <= 1;
}

bool nearDresser(int px, int py) {
    for (int dy = -1; dy <= 1; dy++)
        for (int dx = -1; dx <= 1; dx++) {
            int x = px + dx, y = py + dy;
            if (x >= 0 && x < MAP_W && y >= 0 && y < MAP_H && gameMap[y][x] == '$') return true;
        }
    return false;
}

// Draws the map in real time with the player and suspects on top
// Draws the map side-by-side with the UI panel on the right
void drawScreen(int px, int py, const string& msg) {
    clearScreen();
    
    // UI lines to display on the right side of the map
    string uiLines[MAP_H];
    for(int i = 0; i < MAP_H; i++) uiLines[i] = "";

    // Populate UI lines based on the current game state
    uiLines[0] = " Location: " + getRoomName(px, py);
    uiLines[1] = " Evidence: " + to_string(countEvidence()) + "/" + to_string(NUM_EVIDENCE) + "   Score: " + to_string(investigationScore());
    uiLines[2] = "";
    uiLines[3] = " LEGEND:";
    uiLines[4] = " @ You";
    uiLines[5] = " D Damian";
    uiLines[6] = " E Edmund";
    uiLines[7] = " R Rosa";
    uiLines[8] = " L Lily";
    uiLines[9] = " $ Dresser";
    uiLines[10] = "";
    uiLines[11] = " CONTROLS:";
    uiLines[12] = " [WASD] Move";
    uiLines[13] = " [E]    Interact";
    uiLines[14] = " [C]    Case File";
    uiLines[15] = " [N]    Notes";
    uiLines[16] = " [X]    Deduction";
    uiLines[17] = " [Q]    Menu";
    uiLines[18] = "";
    uiLines[19] = " STATUS:";
    uiLines[20] = " " + msg;

    for (int r = 0; r < MAP_H; r++) {
        string line = "";
        for (int c = 0; c < MAP_W; c++) {
            char ch = gameMap[r][c];
            for (int i = 0; i < NUM_SUSPECTS; i++)
                if (suspects[i].x == c && suspects[i].y == r) ch = suspects[i].symbol;
            if (r == py && c == px) ch = '@';
            line += ch;
        }
        // Print the map row followed by a separator and the corresponding UI line
        cout << line << "   |   " << uiLines[r] << "\n";
    }
}
// ============================================================
//  Case file, notes, crime scene
// ============================================================
void showCaseFile() {
    clearScreen();
    printHeader("CASE FILE: THE MOONLIT PEARL EARRINGS");
    cout << "\n CASE OVERVIEW\n"
         << "  The Moonlit Pearl Earrings were stolen from the master bedroom of\n"
         << "  Hale Manor. The owner, Lady Vivienne Hale, is abroad in Paris and\n"
         << "  is ruled out. Four people were in the house that night.\n";
    cout << "\n TIME OF INCIDENT\n"
         << "  Last seen: 7:45 PM on the master dresser.\n"
         << "  Discovered missing: 10:30 PM (by the butler).\n";
    cout << "\n CRIME SCENE\n"
         << "  Master bedroom (dresser). Door kept locked, window latched inside.\n";
    cout << "\n EVIDENCES\n";
    for (int i = 0; i < NUM_EVIDENCE; i++)
        if (evidenceList[i].found)
            cout << "  * " << evidenceList[i].name << ": " << evidenceList[i].desc << "\n";
    cout << "\n PERSONS OF INTEREST\n";
    for (int i = 0; i < NUM_SUSPECTS; i++)
        cout << "  " << suspects[i].symbol << " " << suspects[i].name << " (" << suspects[i].role
             << ") - " << stateText(suspects[i].state) << "\n";
    waitKey();
}

void showNotes() {
    clearScreen();
    printHeader("INVESTIGATION NOTES");
    if (noteCount == 0) cout << "\n  No notes yet. Question the suspects to fill this in.\n";
    for (int i = 0; i < noteCount; i++)
        cout << "\n  " << (i + 1) << ". " << notes[i];
    cout << "\n";
    waitKey();
}

void showCrimeScene() {
    clearScreen();
    printHeader("CRIME SCENE: MASTER BEDROOM");
    cout << "\n  The velvet box on the dresser lies open and empty. The window is\n"
         << "  latched from the inside, so the thief came through the door. The\n"
         << "  door lock records a STAFF KEY being used at 9:15 PM.\n"
         << "  Someone with a key knew exactly where the earrings were.\n";
    waitKey();
}

// ============================================================
//  Questioning (Ace-Attorney style fixed options)
// ============================================================
void reviewInterview(int s) {
    Suspect& p = suspects[s];
    clearScreen();
    printHeader("INTERVIEW REVIEW: " + p.name + " (" + p.role + ")");
    for (int i = 0; i < 2; i++) {
        cout << "\n  Statement " << (i + 1) << ": \"" << p.st[i].testimony << "\"\n";
        cout << "  You asked: " << p.st[i].options[p.st[i].correct] << "\n";
        cout << "  " << p.name << ": \"" << p.st[i].response << "\"\n";
    }
    waitKey();
}

void interview(int s) {
    Suspect& p = suspects[s];
    if (p.state == 2) { reviewInterview(s); return; }

    // Lily refuses until the player has the key register and cart evidence
    if (s == 3 && p.progress == 0 && (!evidenceList[4].found || !evidenceList[5].found)) {
        clearScreen();
        printHeader("INTERVIEW: " + p.name + " (" + p.role + ")");
        cout << "\n  Lily crosses her arms. \"I have nothing to say. You have no proof.\"\n";
        cout << "\n  (Hint: gather more evidence from the other suspects first.)\n";
        waitKey();
        return;
    }

    p.state = 1;
    while (p.progress < 2) {
        Statement& st = p.st[p.progress];
        clearScreen();
        printHeader("INTERVIEW: " + p.name + " (" + p.role + ")   Score: " + to_string(investigationScore()));
        cout << "\n  Statement " << (p.progress + 1) << " of 2:\n";
        cout << "  \"" << st.testimony << "\"\n\n  How do you respond?\n\n";
        for (int i = 0; i < 3; i++)
            cout << "   [" << (i + 1) << "] " << st.options[i] << "\n";
        cout << "   [0] Step away (return to the map)\n\n  Your choice: ";

        int c = askChoice(3, true);
        if (c == 0) return;            // progress is kept (state stays "currently")

        if (c - 1 == st.correct) {
            correctCount++;
            cout << "\n\n  CORRECT! +50 points\n";
            cout << "  " << p.name << ": \"" << st.response << "\"\n";
            if (st.evidence >= 0 && !evidenceList[st.evidence].found) {
                evidenceList[st.evidence].found = true;
                cout << "\n  NEW EVIDENCE: " << evidenceList[st.evidence].name << "\n";
            }
            addNote(p.name + ": " + st.note);
            p.progress++;
        } else {
            wrongCount++;
            cout << "\n\n  WRONG! -20 points\n";
            cout << "  " << p.name << ": \"I don't see what you're getting at, detective.\"\n";
            cout << "  Try again.\n";
        }
        waitKey();
    }
    p.state = 2;
    clearScreen();
    printHeader("INTERVIEW COMPLETE: " + p.name);
    cout << "\n  Check your Case File and Notes for what you learned.\n";
    waitKey();
}

// ============================================================
//  Deduction
// ============================================================
// Returns true if the player solved the case (all four correct)
bool deduction() {
    clearScreen();
    printHeader("DEDUCTION: WHO STOLE THE EARRINGS?");
    cout << "\n  Who is the thief?\n\n";
    for (int i = 0; i < NUM_SUSPECTS; i++)
        cout << "   [" << (i + 1) << "] " << suspects[i].name << " (" << suspects[i].role << ")\n";
    cout << "   [0] Not ready yet (go back)\n\n  Your choice: ";
    int s = askChoice(NUM_SUSPECTS, true);
    if (s == 0) return false;
    s--;

    clearScreen();
    printHeader("DEDUCTION: REASONING");
    cout << "\n  Accusing " << suspects[s].name << ". What is your reasoning?\n\n";
    for (int i = 0; i < 4; i++) cout << "   [" << (i + 1) << "] " << reasonOptions[i] << "\n";
    cout << "\n  Your choice: ";
    int r = askChoice(4, false) - 1;

    clearScreen();
    printHeader("DEDUCTION: MOTIVE");
    cout << "\n  Why did " << suspects[s].name << " do it?\n\n";
    for (int i = 0; i < 4; i++) cout << "   [" << (i + 1) << "] " << motiveOptions[i] << "\n";
    cout << "\n  Your choice: ";
    int m = askChoice(4, false) - 1;

    clearScreen();
    printHeader("DEDUCTION: SUPPORTING EVIDENCE");
    cout << "\n  Which evidence proves it?\n\n";
    for (int i = 0; i < 4; i++) cout << "   [" << (i + 1) << "] " << evidenceOptions[i] << "\n";
    cout << "\n  Your choice: ";
    int e = askChoice(4, false) - 1;

    bool okS = (s == CORRECT_SUSPECT), okR = (r == CORRECT_REASON);
    bool okM = (m == CORRECT_MOTIVE),  okE = (e == CORRECT_EVIDENCE);

    int deductionScore = 0;
    if (okS) deductionScore += 200;
    if (okR) deductionScore += 100;
    if (okM) deductionScore += 100;
    if (okE) deductionScore += 100;

    clearScreen();
    if (okS && okR && okM && okE) {
        int inv = investigationScore() + efficiencyBonus();
        int total = inv + deductionScore;
        printHeader("CASE SOLVED!");
        cout << "\n  Lily Santos confessed. She took the Moonlit Pearl Earrings to pay\n"
             << "  her mother's hospital bills. The earrings are returned to Hale Manor.\n";
        cout << "\n  Investigation score : " << investigationScore() << "\n";
        cout << "  Efficiency bonus    : " << efficiencyBonus() << "\n";
        cout << "  Deduction score     : " << deductionScore << "\n";
        cout << "  ----------------------------\n";
        cout << "  FINAL SCORE         : " << total << " / 1000\n";
        string rank = "Trainee Detective";
        if (total >= 900) rank = "Master Detective";
        else if (total >= 700) rank = "Senior Detective";
        else if (total >= 500) rank = "Rookie Detective";
        cout << "  Rank                : " << rank << "\n";
        return true;
    }

    printHeader("GAME OVER - THE THIEF ESCAPES");
    cout << "\n";
    int wrongTotal = (!okS) + (!okR) + (!okM) + (!okE);
    if (!okS) cout << "  * WRONG SUSPECT: You accused an innocent person. The real thief slips away.\n";
    if (!okR) cout << "  * WRONG REASONING: Your accusation lacks sufficient justification.\n";
    if (!okM) cout << "  * WRONG MOTIVE: You fail to establish why the thief did it.\n";
    if (!okE) cout << "  * WRONG EVIDENCE: The suspect escapes due to insufficient evidence.\n";
    if (wrongTotal > 1) cout << "\n  Multiple mistakes: the failures above combine. The case collapses.\n";
    cout << "\n  Final Score: 0 (Game Over)\n";
    return true;
}

// ============================================================
//  Exploration loop (real-time WASD movement)
//  Returns 0 = back to menu, 1 = case ended (win or lose)
// ============================================================
int exploreLoop() {
    int px = 30, py = 10;
    string msg = "Welcome, Detective. Walk to a suspect and press E to question them.";

    while (true) {
        drawScreen(px, py, msg);
        int k = readKey();
        if (k == -1) exit(0);
        char key = (char)tolower(k);
        int nx = px, ny = py;

        if (key == 'w') ny--;
        else if (key == 's') ny++;
        else if (key == 'a') nx--;
        else if (key == 'd') nx++;
        else if (key == 'c') { showCaseFile(); msg = "Case file closed."; continue; }
        else if (key == 'n') { showNotes(); msg = "Notes closed."; continue; }
        else if (key == 'e') {
            bool done = false;
            for (int i = 0; i < NUM_SUSPECTS && !done; i++) {
                if (isNear(px, py, suspects[i].x, suspects[i].y)) {
                    interview(i);
                    msg = "You finished talking to " + suspects[i].name + ".";
                    done = true;
                }
            }
            if (!done && nearDresser(px, py)) {
                showCrimeScene();
                msg = "You examined the crime scene.";
                done = true;
            }
            if (!done) msg = "Nothing nearby to interact with.";
            continue;
        }
        else if (key == 'x') {
            if (deduction()) return 1;
            msg = "You are not ready to accuse anyone yet.";
            continue;
        }
        else if (key == 'q') return 0;
        else { msg = "Invalid key! Use W A S D, E, C, N, X or Q."; continue; }

        if (canMove(nx, ny)) { px = nx; py = ny; msg = "Exploring..."; }
        else msg = "Something is in the way.";
    }
}

void playGame() {
    while (true) {
        resetGame();
        int result = exploreLoop();
        if (result == 0) return;            // back to main menu

        cout << "\n  [1] Play Again   [2] Main Menu\n\n  Your choice: ";
        int c = askChoice(2, false);
        if (c == 2) return;                 // else loop restarts the game
    }
}


// The main function - where the program starts running.
int main() {

    int selected = 0;      // Which option is highlighted. 0 = NEW GAME.
    bool running = true;   // The menu loop keeps going while this is true.
    char input;            // Stores the key the player types.

    showInstructions();    // Show the instructions first, then go to the menu.
    
    // The main loop. It redraws the menu again and again
    // until the player chooses QUIT (which sets running to false).
    while (running) {

        clearScreen();         // Wipe the old menu.
        drawMenu(selected);    // Draw the menu with the current highlight.

        input = _getch();      // Wait for the player to type a key + ENTER.

        //  Decide what to do with the key.
        // 'W' and 'S' move the highlight. We accept both
        // lowercase and uppercase letters.
        if (input == 'w' || input == 'W') {
            selected--;                 // Move up one option.
            if (selected < 0) {
                selected = 2;           // Wrap around to the bottom.
            }
        }
        else if (input == 's' || input == 'S') {
            selected++;                 // Move down one option.
            if (selected > 2) {
                selected = 0;           // Wrap around to the top.
            }
        }
        else if (input >= '1' && input <= '3') {
            // Characters are stored as numbers, so '1' - '1' = 0,
            // '2' - '1' = 1, and so on. This turns the key into 0-3.
            selected = input - '1';
            input = 'e';                // Pretend they also pressed E.
        }

        // I replaced it so you just need to press ENTER (NEAT RIGHT@!)
        if (input == 'e' || input == 'E' || input == '\r') {

            clearScreen();
            cout << "\n  ============================================\n\n";

            // A switch statement picks ONE block of code to run,
            // depending on the value of "selected".
            switch (selected) {
                case 0:
                    showInstructions();
                    break; // "break" stops us from falling into the next case.
                case 1:
                    playGame();
                    break;
                case 2:
                    cout << "      Goodbye. See you again!\n";
                    running = false;    // This ends the while loop.
                    break;
            }
        }
    }
    
    return 0;
}
