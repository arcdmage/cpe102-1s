#include <iostream>
#include <string>
#include <cstdlib>
#include <cctype>

#ifdef _WIN32
    #include <conio.h>
    #include <windows.h>
#else
    #include <cstdio>
    #include <cstring>
    #include <unistd.h>
    #include <termios.h>

    // Reads one keypress without echo on Unix-like systems
    inline int _getch() {
        termios oldt, newt;
        if (tcgetattr(STDIN_FILENO, &oldt) != 0) return -1;
        newt = oldt;
        newt.c_lflag &= ~(ICANON | ECHO);
        newt.c_cc[VMIN] = 1;
        newt.c_cc[VTIME] = 0;
        tcsetattr(STDIN_FILENO, TCSANOW, &newt);
        unsigned char c;
        ssize_t n = read(STDIN_FILENO, &c, 1);
        tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
        if (n <= 0) return -1;
        if (c == '\n') return '\r';
        return c;
    }

    // Clears the screen (or runs a command) on Unix-like systems
    inline int mac_system(const char* cmd) {
        if (std::strcmp(cmd, "cls") == 0) {
            std::printf("\033[3J\033[H\033[2J");
            std::fflush(stdout);
            return 0;
        }
        return std::system(cmd);
    }
    #define system(cmd) mac_system(cmd)
#endif

using namespace std;

// ANSI color codes used throughout the UI
#define C_RESET    "\033[0m"
#define C_BOLD     "\033[1m"
#define C_GRAY     "\033[90m"
#define C_RED      "\033[31m"
#define C_BRED     "\033[91m"
#define C_GREEN    "\033[32m"
#define C_DGREEN   "\033[2;32m"
#define C_BGREEN   "\033[92m"
#define C_BYELLOW  "\033[93m"
#define C_BBLUE    "\033[94m"
#define C_BMAGENTA "\033[95m"
#define C_BCYAN    "\033[96m"
#define C_BROWN    "\033[38;5;130m"
#define C_PLAYER   "\033[1;30;107m"
#define C_SAFE     "\033[1;30;48;5;172m"
#define C_DESK     "\033[1;30;48;5;45m"
#define C_TRASH    "\033[1;30;48;5;208m"
#define C_SOIL     "\033[1;30;60;5;99m"
#define C_SEL      "\033[1;30;102m"

// Width in characters of each map cell
const int CELL_W = 1;

// Foreground colors for each suspect
const char* SUSPECT_COLOR[5] = { C_BRED, C_BBLUE, C_BMAGENTA, C_BGREEN, C_BYELLOW };
// Background colors for each suspect tile
const char* SUSPECT_BG[5]    = { "\033[1;30;101m", "\033[1;30;104m",
                                  "\033[1;30;105m", "\033[1;30;102m",
                                  "\033[1;30;103m" };

// Repeats a string n times
string rep(const string& s, int n) {
    string r;
    for (int i = 0; i < n; i++) r += s;
    return r;
}

// Enables UTF-8 output and ANSI colors on Windows
void setupConsole() {
#ifdef _WIN32
    SetConsoleOutputCP(65001);
    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD mode = 0;
    if (GetConsoleMode(h, &mode)) SetConsoleMode(h, mode | 0x0004);
#endif
}

// Flushes output then reads a single keypress
int  readKey()     { cout.flush(); return _getch(); }

// Clears the terminal screen
void clearScreen() { system("cls"); }

// Reads a menu key, supporting arrows and mouse wheel
int readMenuKey() {
#ifdef _WIN32
    HANDLE hIn = GetStdHandle(STD_INPUT_HANDLE);
    DWORD oldMode;
    GetConsoleMode(hIn, &oldMode);
    SetConsoleMode(hIn, ENABLE_EXTENDED_FLAGS | ENABLE_MOUSE_INPUT | ENABLE_WINDOW_INPUT);

    INPUT_RECORD rec;
    DWORD count;
    int result = -1;
    while (result == -1) {
        ReadConsoleInput(hIn, &rec, 1, &count);
        if (rec.EventType == KEY_EVENT && rec.Event.KeyEvent.bKeyDown) {
            char c = rec.Event.KeyEvent.uChar.AsciiChar;
            if (c != 0) result = c;
            else if (rec.Event.KeyEvent.wVirtualKeyCode == VK_UP)   result = 'w';
            else if (rec.Event.KeyEvent.wVirtualKeyCode == VK_DOWN) result = 's';
        }
        else if (rec.EventType == MOUSE_EVENT &&
                 rec.Event.MouseEvent.dwEventFlags == MOUSE_WHEELED) {
            short direction = (short)HIWORD(rec.Event.MouseEvent.dwButtonState);
            result = (direction > 0) ? 'w' : 's';
        }
    }
    SetConsoleMode(hIn, oldMode);
    return result;
#else
    cout << "\033[?1000h" << flush;
    int c = _getch();
    if (c == 27) {
        int c2 = _getch();
        if (c2 == '[') {
            int c3 = _getch();
            if (c3 == 'M') {
                int button = _getch();
                _getch();
                _getch();
                if (button == 96)      c = 'w';
                else if (button == 97) c = 's';
                else                   c = 0;
            }
            else if (c3 == 'A') c = 'w';
            else if (c3 == 'B') c = 's';
            else                c = 0;
        }
        else c = 0;
    }
    cout << "\033[?1000l" << flush;
    return c;
#endif
}

// Displays the instructions screen
void showInstructions() {
    int consoleWidth  = 100;
    int consoleHeight = 30;
    int frameWidth    = 48;
    int contentHeight = 27;
    int leftPad = (consoleWidth - frameWidth) / 2;
    string pad(leftPad, ' ');
    int topPad    = (consoleHeight - contentHeight) / 2;
    int bottomPad = consoleHeight - contentHeight - topPad - 1;

    clearScreen();
    for (int i = 0; i < topPad; i++) cout << "\n";

    cout << pad << "+======================================================================+\n";
    cout << pad << "|                           INSTRUCTIONS                               |\n";
    cout << pad << "+----------------------------------------------------------------------+\n";
    cout << "\n";
    cout << pad << "   OBJECTIVE\n";
    cout << pad << "   Examine the case file, search the mansion for clues, question\n";
    cout << pad << "   the suspects, then unmask the thief with your final deduction.\n\n";
    cout << pad << "   CONTROLS\n";
    cout << pad << "   [W][A][S][D] Move your detective (@)   [E] Interact / Examine\n";
    cout << pad << "   [C] Case File   [N] Notes   [X] Deduction (any time)   [Q] Menu\n\n";
    cout << pad << "   QUESTIONING\n";
    cout << pad << "   Pick a topic, then the best question. Correct = +50 points.\n";
    cout << pad << "   Wrong = -20 points, but you can try again.\n\n";
    cout << pad << "   SEARCHING\n";
    cout << pad << "   Stand next to $ safe, & desk, % trash can or ^ soil, press E.\n\n";
    cout << pad << "   DEDUCTION\n";
    cout << pad << "   Choose the suspect, reasoning, motive and evidence. ALL FOUR must\n";
    cout << pad << "   be right to win (up to 1000 points). Any mistake = GAME OVER.\n";
    cout << pad << "   Interview everyone with few mistakes to earn an efficiency bonus!\n";

    cout << "\n";
    cout << pad << "+-----------------------------------------------------------------------+\n";
    cout << pad << "|                      PRESS ANY KEY TO CONTINUE                        |\n";
    cout << pad << "+-----------------------------------------------------------------------+\n";

    for (int i = 0; i < bottomPad; i++) cout << "\n";
    _getch();
}

// Displays the credits screen
void showCredits() {
    int consoleWidth  = 100;
    int consoleHeight = 30;
    int frameWidth    = 48;
    int contentHeight = 13;
    int leftPad = (consoleWidth - frameWidth) / 2;
    string pad(leftPad, ' ');
    int topPad    = (consoleHeight - contentHeight) / 2;
    int bottomPad = consoleHeight - contentHeight - topPad - 1;

    clearScreen();
    for (int i = 0; i < topPad; i++) cout << "\n";

    cout << pad << "+======================================================================+\n";
    cout << pad << "|                             Credits                                  |\n";
    cout << pad << "+----------------------------------------------------------------------+\n";
    cout << "\n";
    cout << pad << "   \t\tSantos\n";
    cout << pad << "   \t\tCpE11S1\n";
    cout << pad << "   \t\tLeader and Programmer\n\n";
    cout << pad << "   \t\tSales\n";
    cout << pad << "   \t\tCpE11S1\n";
    cout << pad << "   \t\tDesign & Programmer\n\n";
    cout << pad << "   \t\tFabela\n";
    cout << pad << "   \t\tCpE11S1\n";
    cout << pad << "   \t\tProgrammer & Concept\n\n";
    cout << pad << "   \t\tNucum\n";
    cout << pad << "   \t\tCpE11S1\n";
    cout << pad << "   \t\tStory and Concept\n\n";
    cout << "\n";
    cout << pad << "+-----------------------------------------------------------------------+\n";
    cout << pad << "|                      PRESS ANY KEY TO CONTINUE                        |\n";
    cout << pad << "+-----------------------------------------------------------------------+\n";

    for (int i = 0; i < bottomPad; i++) cout << "\n";
    _getch();
}

// Draws the main menu with the selected option highlighted
void drawMenu(int selected) {
    string options[4] = { "INSTRUCTION", "START GAME", "QUIT", "CREDITS" };

    int consoleWidth  = 100;
    int consoleHeight = 30;
    int frameWidth    = 48;
    int menuHeight    = 21;

    int leftPad = (consoleWidth - frameWidth) / 2;
    string pad(leftPad, ' ');
    int topPad    = (consoleHeight - menuHeight) / 2;
    int bottomPad = consoleHeight - menuHeight - topPad - 1;

    for (int i = 0; i < topPad; i++) cout << "\n";

    cout << C_BYELLOW;
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
    cout << C_RESET;
    cout << "\n";

    for (int i = 0; i < 4; i++) {
        int indent = 6 + (i * 4);
        cout << pad;
        for (int s = 0; s < indent; s++) cout << " ";

        if (i == selected) {
            string text = "  >> " + to_string(i + 1) + ". " + options[i] + " <<";
            while (text.size() < 25) text += " ";
            string blank(25, ' ');

            cout << C_SEL << blank << C_RESET << "\n";
            cout << pad;
            for (int s = 0; s < indent; s++) cout << " ";
            cout << C_SEL << text << C_RESET << "\n";
            cout << pad;
            for (int s = 0; s < indent; s++) cout << " ";
            cout << C_SEL << blank << C_RESET << "\n";
        } else {
            cout << C_GRAY << "     " << (i + 1) << ". " << options[i] << C_RESET << "\n";
        }
    }

    for (int i = 0; i < bottomPad; i++) cout << "\n";
}

// Map and text layout constants
const int MAP_W = 60;
const int MAP_H = 21;
const int NUM_SUSPECTS = 5;
const int NUM_EVIDENCE = 8;
const int MAX_NOTES = 40;
const int MAX_TOPICS = 4;
const int TEXT_WIDTH = 86;

// Suspect and evidence index constants
const int S_JULIAN = 0, S_ARTHUR = 1, S_EVELYN = 2, S_GIDEON = 3, S_CLARA = 4;
const int EV_CUFFLINK = 4, EV_INK = 5, EV_CATALOG = 6, EV_SOIL = 7;

// Speaker prefixes used in dialogue strings
#define DET "DETECTIVE: "
#define JUL "JULIAN: "
#define ART "ARTHUR: "
#define EVE "EVELYN: "
#define GID "GIDEON: "
#define CLA "CLARA: "

// Static map layout (rooms, doors, and examinable symbols)
const char* gameMap[MAP_H] = {
    "############################################################",
    "#.......###.....................................###........#",
    "#../\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\/\\..#",
    "#..|             | [#][#] [##] | [#][#][#]  | o  o  o  o|..#",
    "#..| [=====] [t] |             |            |           |..#",
    "#..|             |             |   [__&__]  |       [^] |..#",
    "#..| (O)         |     [$]     |            |           |..#",
    "#..|             |             |            |           |..#",
    "#..+----[  ]-----+----[  ]-----+---[  ]-----+---[  ]----+..#",
    "#..|                     [========]                     |..#",
    "#..                                                     |..#",
    "#..|                                                    |..#",
    "#..+----[  ]-----+----[  ]-----+---[  ]-----+---[  ]----+..#",
    "#..|       [o][o]|          {*}| (O)    (O) |        {*}|..#",
    "#..| [###]       |  [~~~~~~]   | [==]  [==] |           |..#",
    "#..|             |             |            | [=======] |..#",
    "#..|    [===]    |      [=]    |         [%]|           |..#",
    "#..|             |             |            |           |..#",
    "#..+-------------+-------------+------------+-----------+..#",
    "#.......###.....................................###........#",
    "############################################################",
};

// A single answer choice in a topic
struct Choice {
    string label;
    string reply;
    string feedback;
};

// A question topic containing three choices and metadata
struct Topic {
    string label;
    string intro;
    Choice ch[3];
    int correct;
    string review;
    int evidence;
    int needEvidence;
    bool done;
};

// A suspect with profile, dialogue, and interview state
struct Suspect {
    string name, role, profile, statement, opening, closing, farewell;
    Topic topics[MAX_TOPICS];
    int numTopics;
    int state;
    int x, y;
    char symbol;
};

// A piece of evidence the player can discover
struct Evidence {
    string name, desc;
    bool found;
};

// Global game state
Suspect  suspects[NUM_SUSPECTS];
Evidence evidenceList[NUM_EVIDENCE];
string   notes[MAX_NOTES];
int      noteCount = 0;
int      correctCount = 0;
int      wrongCount = 0;

// Deduction option texts for reasoning, motive, and evidence
const string reasonOptions[5] = {
    "Arthur holds the keys and hates Julian, so he planted the cufflink to frame him. Julian is a victim.",
    "A frame-up only works if the evidence stays at the scene, but Arthur took the cufflink away. Julian planted it himself, used the spare key from the Study Room desk, and blamed Arthur.",
    "Evelyn was alone in the corridor on her phone, and her tea left Grace asleep, so she had the chance to rob the safe.",
    "Gideon knows every entrance and saw the Study Room light, so he climbed the balcony and used the spare key.",
    "Clara was the last to touch the safe and wiped it clean, so she hid her own traces."
};
const string motiveOptions[5] = {
    "He wanted to buy a collection of luxury designer handbags.",
    "He needed money to pay a blackmailer who threatened him.",
    "He wanted someone he despises to take the blame.",
    "He needed to sell the earrings to pay off his heavy debts.",
    "He wanted to stop someone from exposing a secret."
};
const string evidenceOptions[5] = {
    "The gold cufflink found near the safe",
    "The sanitizer wipe residue in the Walk-in Closet",
    "The fountain pen ink on the spare safe key in the Study Room desk",
    "The half-empty chamomile teacup",
    "The torn designer handbag catalog pages"
};
const int CORRECT_SUSPECT = S_JULIAN, CORRECT_REASON = 1,
          CORRECT_MOTIVE = 3, CORRECT_EVIDENCE = 2;

// Waits for any key press before continuing
void waitKey() {
    cout << "\n  [Press any key to continue]";
    if (readKey() == -1) exit(0);
}

// Reads a digit choice within range, with optional zero
int askChoice(int maxChoice, bool allowZero) {
    while (true) {
        int k = readKey();
        if (k == -1) exit(0);
        int n = k - '0';
        if (k >= '0' && k <= '9' && n <= maxChoice &&
            (n >= 1 || (allowZero && n == 0)))
            return n;
        cout << "\r  Invalid input! Choose " << (allowZero ? 0 : 1) << "-"
             << maxChoice << ": " << flush;
    }
}

// Prints a colored title header with horizontal bars
void printHeader(const string& title) {
    const char* col = C_BCYAN;
    if (title.rfind("CASE SOLVED", 0) == 0) col = C_BGREEN;
    else if (title.rfind("GAME OVER", 0) == 0) col = C_BRED;
    cout << col << rep("═", 60) << C_RESET << "\n";
    cout << col << C_BOLD << "  ◆ " << title << C_RESET << "\n";
    cout << col << rep("═", 60) << C_RESET << "\n";
}

// Word-wraps and prints text with indent and hanging indent
void printWrapped(const string& text, int indent, int hang, int width) {
    size_t start = 0;
    while (start <= text.size()) {
        size_t nl = text.find('\n', start);
        string line = (nl == string::npos) ? text.substr(start) : text.substr(start, nl - start);

        string out = string(indent, ' ');
        int len = indent;
        bool first = true;
        size_t i = 0;
        while (i < line.size()) {
            while (i < line.size() && line[i] == ' ') i++;
            size_t j = i;
            while (j < line.size() && line[j] != ' ') j++;
            if (j == i) break;
            string word = line.substr(i, j - i);
            if (!first && len + 1 + (int)word.size() > width) {
                cout << out << "\n";
                out = string(hang, ' ');
                len = hang;
                first = true;
            }
            if (!first) { out += " "; len++; }
            out += word;
            len += (int)word.size();
            first = false;
            i = j;
        }
        cout << out << "\n";
        if (nl == string::npos) break;
        start = nl + 1;
    }
}

// Prints dialogue, coloring each line by the speaker
void printDialogue(const string& text) {
    size_t start = 0;
    while (start <= text.size()) {
        size_t nl = text.find('\n', start);
        string line = (nl == string::npos) ? text.substr(start) : text.substr(start, nl - start);

        const char* col = "";
        if (line.rfind("DETECTIVE:", 0) == 0)      col = C_BCYAN;
        else if (line.rfind("JULIAN:", 0) == 0)    col = SUSPECT_COLOR[1];
        else if (line.rfind("ARTHUR:", 0) == 0)    col = SUSPECT_COLOR[0];
        else if (line.rfind("EVELYN:", 0) == 0)    col = SUSPECT_COLOR[2];
        else if (line.rfind("GIDEON:", 0) == 0)    col = SUSPECT_COLOR[3];
        else if (line.rfind("CLARA:", 0) == 0)     col = SUSPECT_COLOR[4];

        cout << col;
        printWrapped(line, 2, 4, TEXT_WIDTH);
        cout << C_RESET;

        if (nl == string::npos) break;
        start = nl + 1;
    }
}

// Pops a width-limited line from a string buffer
string takeLine(string& rest, int width) {
    if ((int)rest.size() <= width) {
        string l = rest;
        rest = "";
        return l;
    }
    size_t cut = rest.rfind(' ', width);
    if (cut == string::npos || cut == 0) cut = width;
    string l = rest.substr(0, cut);
    rest = rest.substr(cut);
    while (!rest.empty() && rest[0] == ' ') rest.erase(0, 1);
    return l;
}

// Returns how many evidence items have been found
int countEvidence() {
    int n = 0;
    for (int i = 0; i < NUM_EVIDENCE; i++) if (evidenceList[i].found) n++;
    return n;
}

// Appends a note to the investigation notes
void addNote(const string& text) {
    if (noteCount < MAX_NOTES) notes[noteCount++] = text;
}

// Returns a human-readable interview status
string stateText(int s) {
    if (s == 0) return "Not yet interviewed";
    if (s == 1) return "Currently interviewing";
    return "Previously interviewed";
}

// Returns true if every suspect has been fully interviewed
bool allInterviewed() {
    for (int i = 0; i < NUM_SUSPECTS; i++)
        if (suspects[i].state != 2) return false;
    return true;
}

// Marks evidence as found and optionally adds a note
void foundEvidence(int idx, const string& note) {
    if (evidenceList[idx].found) return;
    evidenceList[idx].found = true;
    cout << "\n  " << C_BYELLOW << C_BOLD << "★ NEW EVIDENCE: "
         << evidenceList[idx].name << C_RESET << "\n";
    printWrapped(evidenceList[idx].desc, 4, 4, TEXT_WIDTH);
    if (note != "") addNote(note);
}

// Computes the current investigation score (clamped 0..400)
int investigationScore() {
    int s = correctCount * 50 - wrongCount * 20;
    if (s < 0) s = 0;
    if (s > 400) s = 400;
    return s;
}

// Computes the efficiency bonus for interviewing everyone
int efficiencyBonus() {
    if (!allInterviewed()) return 0;
    int b = 100 - wrongCount * 10;
    if (b < 0) b = 0;
    return b;
}

// Builds a Topic object from the supplied data
Topic makeTopic(const string& label, const string& intro, int correct,
                int evidence, int needEvidence, const string& review,
                const Choice& a, const Choice& b, const Choice& c) {
    Topic t;
    t.label = label; t.intro = intro;
    t.ch[0] = a; t.ch[1] = b; t.ch[2] = c;
    t.correct = correct; t.review = review;
    t.evidence = evidence; t.needEvidence = needEvidence; t.done = false;
    return t;
}

// Fills in a suspect's base info and starting state
void setupSuspect(int i, const string& name, const string& role, char symbol,
                  int x, int y, const string& profile, const string& statement,
                  const string& opening, const string& closing,
                  const string& farewell) {
    Suspect& s = suspects[i];
    s.name = name; s.role = role; s.symbol = symbol;
    s.x = x; s.y = y;
    s.profile = profile; s.statement = statement;
    s.opening = opening; s.closing = closing; s.farewell = farewell;
    s.state = 0; s.numTopics = 0;
}

// Sets up Julian's profile and four interview topics
void setupJulian() {
    setupSuspect(
        S_JULIAN, "Julian Vance", "Owner's Partner", 'J', 28, 15,
        "Age: 28\nOccupation: Stock Market Investor\nRelationship to Owner: Husband (Partner)\n"
        "Background: An ambitious investor struggling with heavy financial debts who has become increasingly secretive about his late-night activities.",
        "I spent the evening in the 1st-floor Living Room reviewing stock trends on my laptop and making private overseas calls until midnight. I never went anywhere near the Walk-in Closet. If you want a real suspect, look at Arthur. He's been following me around the house and would love nothing more than to frame me.",
        JUL "Detective. I assume you're here about Grace's missing earrings. A terrible situation, of course, but I doubt I can be of much help.\n"
        JUL "I spent the evening in the first-floor Living Room reviewing stock trends on my laptop and making private overseas calls until midnight. I never went anywhere near the Walk-in Closet.\n"
        JUL "If you're looking for someone suspicious, I suggest you speak to Arthur. He has access to the household's security arrangements, and he has never made an effort to hide how much he dislikes me.\n"
        DET "I'll decide who is suspicious after I've heard everyone's account. Let's start with your movements last night.",
        DET "That covers everything I wanted to ask for now. I'll compare your statements with the evidence.\n"
        JUL "By all means, Detective. Just make sure you have evidence before accusing anyone.",
        DET "I'll continue the investigation. Thank you for your cooperation.\n"
        JUL "Make sure you have evidence before accusing anyone, Detective.");
    Suspect& s = suspects[S_JULIAN];

    s.topics[0] = makeTopic("Ask about his evening.",
        DET "You said you spent the evening in the Living Room reviewing stock trends and making overseas calls. Tell me more about that.\n"
        JUL "There's not much to tell. I was following the markets and speaking with business contacts. I had work to finish, and I stayed occupied with it.\n"
        DET "Can anyone verify that you remained in the Living Room throughout the evening?",
        0, -1, -1,
        DET "You claimed to have spent the evening working in the Living Room and making private calls. I still need independent evidence to verify that account.\n"
        JUL "Then I suggest you look for it instead of asking me the same questions.",
        Choice{"Ask whether his calls can be verified.",
            DET "You mentioned private calls. Who did you speak with, and can those calls be verified?\n"
            JUL "Business contacts. I'd rather not disclose their names, and I don't see why my private dealings should concern this investigation.\n"
            JUL "My laptop should show that I was working. Surely that is enough to establish where I spent my evening.\n"
            DET "Your work may establish what you were doing, but it doesn't necessarily establish where you were every minute of the evening.\n"
            JUL "If you insist. But I won't have my business affairs treated as evidence of a crime.",
            "Julian claims to have made private overseas calls, but his alibi has not yet been independently verified."},
        Choice{"Ask what he was working on.",
            DET "Which stocks were you reviewing that evening?\n"
            JUL "Several overseas markets. The details are rather technical, Detective, and I doubt they have anything to do with Grace's missing earrings.\n"
            DET "The details of your investments aren't what I'm investigating. I need to establish your movements that night.",
            "His investment activity does not establish whether his whereabouts can be verified."},
        Choice{"Ask whether he noticed anything unusual.",
            DET "Did you notice anyone moving around the mansion that evening?\n"
            JUL "Not that I can recall. I was focused on my work. If something happened elsewhere in the house, I wasn't aware of it.\n"
            DET "I'll ask the others what they observed. For now, let's return to your own movements.",
            "Julian's answer does not help verify his own alibi."});

    s.topics[1] = makeTopic("Ask about the household.",
        DET "You mentioned that Arthur has access to the household's security arrangements. How familiar are you with the mansion's keys and security?\n"
        JUL "I live here, Detective. Of course I know something about how the house is run. But Arthur has worked for this family for years. He knows far more about its daily routines than I do.\n"
        DET "I see. Let me ask you something about the household.",
        0, -1, -1,
        DET "You said you are generally familiar with the household's key arrangements, but you don't know who holds each key.\n"
        JUL "That's right. I live here, but I don't keep track of every key.",
        Choice{"Ask whether he knows the household's key arrangements.",
            DET "Are you familiar with how the household manages its keys?\n"
            JUL "Generally, yes. Different keys serve different purposes, and certain areas are restricted. But I don't keep track of every key or who has it at any given moment.\n"
            DET "Understood. I'll compare that statement with the key registry and the accounts of the other household members.\n"
            JUL "You're welcome to. I have nothing to add.",
            "Julian is generally familiar with household key arrangements but claims not to know who holds each key."},
        Choice{"Ask why Arthur dislikes him.",
            DET "Why do you think Arthur dislikes you so much?\n"
            JUL "You would have to ask him. We don't particularly get along, but that doesn't mean I know what goes on in his head.\n"
            DET "I'll hear Arthur's side of the story separately. I need something more useful to establish your own account.",
            "Julian's personal conflict with Arthur does not establish either man's involvement in the theft."},
        Choice{"Ask whether he knows the staff's schedules.",
            DET "Do you know where the household staff were throughout the evening?\n"
            JUL "Not precisely. They have their duties, and I have mine. I don't monitor everyone in the house.\n"
            DET "Then I'll need to establish their movements by speaking with them directly.",
            "Julian does not provide a verifiable account of the staff's movements."});

    s.topics[2] = makeTopic("Ask about the spare safe key.",
        DET "The safe was found locked and undamaged. According to the Mansion Key Registry, the primary safe key was held by Grace, while the secondary override key was kept in a locked desk in the Study Room.\n"
        JUL "I see. So someone may have used the spare key. But that doesn't tell you who opened the safe.\n"
        DET "I want to ask you about that spare key.",
        1, -1, -1,
        DET "That answers my questions about the spare key. I'll compare your statement with the evidence as the investigation continues.\n"
        JUL "By all means, Detective. Just make sure you have evidence before accusing anyone.",
        Choice{"Ask whether the safe could have been opened without a key.",
            DET "Do you think someone could have opened the safe without using either key?\n"
            JUL "The safe was found without visible damage, wasn't it? I'd assume a key was used. But I'm no safe specialist.\n"
            DET "The evidence already establishes that the safe was undamaged. I need to find out who could have accessed it.",
            "The safe's condition is already recorded in the Case File. This question does not help verify Julian's alibi."},
        Choice{"Ask whether he entered the Study Room that night.",
            DET "Did you enter the Study Room at any point last night?\n"
            JUL "No. I had no reason to go in there. I spent the evening in the Living Room, as I already told you.\n"
            DET "And you didn't retrieve or handle the secondary safe key?\n"
            JUL "No. I didn't touch it. If someone took the key, you'll need evidence that places them in the Study Room.\n"
            DET "I'll compare your statement with the other evidence before drawing a conclusion.\n"
            JUL "That's all I can ask, Detective.",
            "Julian denies entering the Study Room or handling the secondary safe key. His claim must be checked against the timeline and physical evidence."},
        Choice{"Ask why someone would use the spare key.",
            DET "Why would someone use the secondary key instead of the primary one?\n"
            JUL "I couldn't tell you. Perhaps the primary key wasn't available, or perhaps someone simply knew where the spare was kept.\n"
            DET "That explains a possibility, not who accessed the safe.",
            "Julian's answer offers possible explanations but does not establish his own movements."});

    s.topics[3] = makeTopic("Ask about the gold cufflink.",
        DET "Julian, I have something else I'd like to ask you about. A gold cufflink belonging to you was found in the Walk-in Closet.\n"
        JUL "Mine? I see. I'm not denying that it belongs to me, but I didn't leave it in Grace's closet. Arthur has every reason to make me look guilty. If he found it there, perhaps he put it there himself.\n"
        DET "Arthur says he found the cufflink after hearing a noise near the Study Room corridor. He admits taking it from the Walk-in Closet instead of leaving it where he found it.\n"
        JUL "How convenient. He takes the cufflink, and now he gets to explain why it was there in the first place. You can't expect me to believe that was an innocent mistake.",
        0, -1, EV_CUFFLINK,
        DET "You say Arthur may have taken the cufflink to frame you, but that remains only a theory.\n"
        JUL "Then find something more convincing than a missing accessory.",
        Choice{"Challenge Julian's theory about Arthur.",
            DET "If Arthur intended to frame you, why would he take the cufflink away from the scene instead of leaving it where it could implicate you?\n"
            JUL "Perhaps he panicked. Perhaps he wanted to hide it and later claim he had discovered it. You're assuming he acted rationally.\n"
            JUL "Arthur has disliked me for years. I wouldn't put it past him to do something foolish just to cause trouble for me.\n"
            DET "That's a possibility, but it's only a theory. Arthur's actions are suspicious, but they don't establish who stole the earrings.\n"
            JUL "Then I suggest you find something more convincing than a missing accessory.",
            "Julian claims Arthur may have removed the cufflink to frame him, but this explanation remains unproven."},
        Choice{"Ask whether he owns other gold accessories.",
            DET "Do you own other gold accessories that could be mistaken for this cufflink?\n"
            JUL "I own several accessories. This particular cufflink is mine, though. I've already told you that I didn't leave it in the closet.\n"
            DET "I already know it belongs to you. I need to examine the circumstances in which it was found.",
            "The cufflink's ownership is established. The unresolved issue is how it came to be at the scene and why Arthur removed it."},
        Choice{"Ask whether the cufflink proves the safe was opened with a key.",
            DET "Does the cufflink tell us whether the safe was opened with a key?\n"
            JUL "No. A cufflink can't tell you that. The safe was found undamaged, so perhaps someone used a key. But you can't determine who did it from my cufflink alone.\n"
            DET "Correct. I need to establish how the cufflink got there and compare that with the other evidence.",
            "The cufflink does not establish how the safe was opened."});
    s.numTopics = 4;
}

// Sets up Arthur's profile and three interview topics
void setupArthur() {
    setupSuspect(
        S_ARTHUR, "Arthur Pendelton", "Head Butler", 'A', 34, 16,
        "Age: 45\nOccupation: Head Butler\nRelationship to Owner: Head of Household Staff\n"
        "Background: A strictly punctual servant who has worked for the family since the Owner was a child and openly harbors deep resentment toward Julian.",
        "At 10:00 PM, I was inside the Staff Quarters on the 1st floor winding the clocks. At precisely 11:15 PM, I heard a metallic thud near the 2nd-floor Study Room corridor. I went upstairs to conduct a security patrol, checked the doors, and returned to my room. I suggest investigating the person buried in stock market debts.",
        ART "Detective. I expected you would want to speak with me. I have served this household for many years, and I understand how serious the disappearance of Miss Grace's earrings is.\n"
        ART "At 10:00 PM, I was in the Staff Quarters on the first floor, winding the clocks. At approximately 11:15 PM, I heard a metallic thud near the second-floor Study Room corridor.\n"
        ART "I went upstairs to investigate, checked the nearby rooms, and returned to my quarters afterward. I suggest you look closely at anyone who might have a reason to steal something valuable.\n"
        DET "I'll consider everyone's account. For now, I'd like to understand your movements and what you observed last night.",
        DET "That answers my questions about your evening, your duties, and your relationship with Julian. I'll continue comparing everyone's statements with the evidence.\n"
        ART "Of course, Detective. I hope the truth comes to light.",
        DET "I'll continue the investigation. Thank you for your cooperation.\n"
        ART "I hope you find the truth, Detective.");
    Suspect& s = suspects[S_ARTHUR];

    s.topics[0] = makeTopic("Ask about his evening and the noise he heard.",
        DET "You mentioned hearing a metallic thud near the Study Room corridor at around 11:15 PM. Tell me what happened after that.\n"
        ART "I went upstairs to investigate. I found a brass bookend knocked out of place near the Study Room corridor. I checked the surrounding rooms to make sure nothing else was disturbed.\n"
        DET "Which rooms did you check?\n"
        ART "The nearby rooms, including the Walk-in Closet. I wanted to make sure everything was in order before returning to my quarters.",
        0, EV_CUFFLINK, -1,
        DET "Arthur says he heard a metallic thud at approximately 11:15 PM, found a brass bookend out of place, and checked the nearby rooms. He admits finding and pocketing Julian's cufflink.\n"
        ART "I have nothing to add to that account.",
        Choice{"Ask why he went into the Walk-in Closet.",
            DET "Why did you enter the Walk-in Closet when the noise came from the Study Room corridor?\n"
            ART "I was checking the nearby rooms. The closet is close enough to warrant a quick inspection. I found nothing visibly disturbed, so I was about to leave.\n"
            DET "Did you notice anything unusual while you were inside?\n"
            ART "There was something on the floor near the safe. A gold cufflink. I recognized it as belonging to Mr. Julian.\n"
            DET "What did you do with it?\n"
            ART "I picked it up and put it in my pocket.\n"
            DET "Why didn't you leave it where you found it?\n"
            ART "I thought it might be used to accuse him. I admit I have little affection for Mr. Julian, but I didn't want a misplaced object to become the basis of an accusation against him. I should have left it untouched.\n"
            DET "By removing it, you made it impossible to examine the scene as you found it.\n"
            ART "I understand, Detective. I made a poor decision. But I did not steal the earrings.",
            "Arthur admits finding Julian's gold cufflink near the safe at approximately 11:15 PM and pocketing it instead of leaving it at the scene."},
        Choice{"Ask whether he saw anyone in the corridor.",
            DET "Did you see anyone in the corridor when you went upstairs?\n"
            ART "No one I could identify. I heard the noise, investigated it, and checked the nearby rooms.\n"
            DET "So you cannot confirm who was in the area at the time?\n"
            ART "No, Detective. I can only tell you what I personally observed.",
            "Arthur's answer does not establish what he found during his search or what happened afterward."},
        Choice{"Ask about the brass bookend.",
            DET "Was the brass bookend damaged when you found it?\n"
            ART "I don't recall seeing any damage. It had simply been knocked out of place.\n"
            DET "Did you move it?\n"
            ART "I may have put it back in order, but I was more concerned with checking the nearby rooms.\n"
            DET "I need to know what else you encountered when you went upstairs.",
            "The bookend explains the noise Arthur heard, but it does not clarify what he discovered during his search."});

    s.topics[1] = makeTopic("Ask about his duties and knowledge of the mansion.",
        DET "You've served this household for years. I imagine you're familiar with its security arrangements.\n"
        ART "I am familiar with the household's routines, the staff's duties, and the general security arrangements. It is part of my responsibility to ensure the house runs properly.\n"
        DET "Then I have a few questions about your access to the mansion.",
        2, -1, -1,
        DET "Arthur says he knows the household's security arrangements and the location of the secondary safe key. He denies retrieving it or using it on the night of the theft.\n"
        ART "Correct. That is all I can tell you about the key.",
        Choice{"Ask whether his duties give him access to every room.",
            DET "Does your position give you access to every room in the mansion?\n"
            ART "I am responsible for the household, but that does not mean I enter every room whenever I please. The residents have their private spaces, and I respect that.\n"
            DET "I need to establish the specific access you had, not simply your job title.",
            "Arthur's general access does not establish his movements during the theft."},
        Choice{"Ask whether he holds the primary safe key.",
            DET "Do you hold the primary key to Grace's floor safe?\n"
            ART "No. The primary safe key is kept by Miss Grace. I do not hold it.\n"
            DET "The key registry already records who holds the primary key. I need to understand your knowledge of the household's other keys.",
            "The Case File already identifies Grace as the holder of the primary safe key. This answer adds little to the investigation."},
        Choice{"Ask whether he knows where spare keys are kept.",
            DET "Are you familiar with where the household keeps its spare keys?\n"
            ART "I know the general arrangements. Different keys serve different purposes, and staff members have keys for the rooms they need to access.\n"
            DET "You knew about the secondary safe key in the Study Room desk, then?\n"
            ART "I knew the household had a secondary safe key, yes. The key registry records where it is kept. But knowing where it is stored does not mean I took it or used it.\n"
            DET "Did you retrieve it on the night of the theft?\n"
            ART "No. I had no reason to retrieve that key.\n"
            DET "I'll compare your statement with the key registry and the timeline.\n"
            ART "That is the proper course, Detective.",
            "Arthur knew about the household's spare-key arrangements but denies retrieving or using the secondary safe key."});

    s.topics[2] = makeTopic("Ask about his relationship with Julian.",
        DET "You mentioned that you and Julian have never been particularly close. Has your relationship with him affected how you carry out your duties?\n"
        ART "No, Detective. Whatever differences we may have, I have always performed my duties to this household. Personal feelings should not interfere with my responsibilities.\n"
        DET "Have you and Julian had any disagreements recently?\n"
        ART "We have disagreed on occasion, particularly about household matters. But I have no intention of turning a personal disagreement into an accusation without evidence.",
        0, -1, EV_CUFFLINK,
        DET "You admitted that you and Julian have a strained relationship, but you deny having any intention of framing him. You claim you pocketed the cufflink because you thought it might be used to accuse him.\n"
        ART "That's correct, Detective. I know I should have left it at the scene.",
        Choice{"Ask whether his dislike of Julian influenced his decision to pocket the cufflink.",
            DET "You admitted finding Julian's cufflink near the safe and putting it in your pocket. Did your personal feelings toward him influence that decision?\n"
            ART "I won't deny that I dislike Mr. Julian. But I did not take the cufflink to frame him. I thought someone might use it to accuse him, and I made the foolish decision to remove it instead of leaving it where I found it.\n"
            DET "Don't you think removing potential evidence could make you look suspicious instead?\n"
            ART "Yes. Looking back, I should have left it untouched. I understand that my decision only made the situation more complicated.\n"
            DET "Your intentions remain unverified, Arthur. I'll have to compare your account with the other evidence.\n"
            ART "I understand, Detective. I can only tell you what happened.",
            "Arthur admits disliking Julian but claims he pocketed the cufflink to prevent it from being used against him. He acknowledges that removing it was a mistake."},
        Choice{"Ask whether he has a reason to accuse Julian of stealing the earrings.",
            DET "Do you have any personal reason to accuse Julian of stealing the earrings?\n"
            ART "No, Detective. Disliking someone is not proof that they committed a crime. I won't accuse Mr. Julian without evidence.\n"
            DET "Then I'll need to look beyond personal disagreements to establish what happened.",
            "No new evidence or notes are added."},
        Choice{"Ask whether he has argued with Julian about the mansion's security.",
            DET "Have you and Julian ever argued about the mansion's security arrangements?\n"
            ART "We've had disagreements about household matters, but I cannot recall a specific argument about security that would explain the disappearance of the earrings.\n"
            DET "I see. That doesn't establish a connection to the theft.",
            "No new evidence or notes are added."});
    s.numTopics = 3;
}

// Sets up Evelyn's profile and three interview topics
void setupEvelyn() {
    setupSuspect(
        S_EVELYN, "Evelyn Cruz", "Head Chef", 'E', 14, 15,
        "Age: 32\nOccupation: Head Chef\nRelationship to Owner: Estate Chef\n"
        "Background: A perfectionist culinary artist who frequently loses track of time while baking and secretly obsessing over luxury designer handbag auctions.",
        "I was in the Kitchen prepping pastries for tomorrow's birthday celebration. I only went up to the 2nd floor at 10:00 PM to deliver hot chamomile tea to Bedroom 1. I returned straight to the Kitchen and stayed there past midnight. I didn't enter the Walk-in Closet.",
        EVE "Detective. I heard about Miss Grace's missing earrings this morning. It's a terrible thing to happen in this household.\n"
        EVE "I was in the kitchen for most of the evening. At around 10:00 PM, I brought Miss Grace a cup of chamomile tea in her bedroom. After that, I returned to my duties.\n"
        DET "You were one of the last people known to have interacted with Grace before the theft was discovered. I'd like to ask you a few questions about that evening.\n"
        EVE "Of course, Detective. I'll answer what I can.",
        DET "Thank you, Evelyn. I've recorded your account of the tea delivery, your duties, and your activities later that evening. I'll compare your statements with the other evidence before drawing any conclusions.\n"
        EVE "I understand, Detective. I hope you find whoever took the earrings.",
        DET "I'll continue the investigation. Thank you for your cooperation.\n"
        EVE "Of course, Detective. I hope you find the truth.");
    Suspect& s = suspects[S_EVELYN];

    s.topics[0] = makeTopic("Ask about her evening and the tea she delivered.",
        DET "You mentioned bringing Grace chamomile tea at around 10:00 PM. Tell me what happened when you delivered it.\n"
        EVE "I prepared the tea in the kitchen and brought it to her bedroom myself. She seemed tired, so I left it on her bedside table and returned downstairs.\n"
        DET "Did you stay in the bedroom for long?\n"
        EVE "No. I only stayed long enough to deliver the tea and make sure she had everything she needed.",
        0, -1, -1,
        DET "You delivered chamomile tea to Grace at approximately 10:00 PM. You said she appeared tired, but you didn't recall seeing her finish the tea.\n"
        EVE "That's correct. I delivered it and returned to my duties.",
        Choice{"Ask whether Grace drank the tea before Evelyn left.",
            DET "Did Grace drink the tea while you were still in the bedroom?\n"
            EVE "I don't remember seeing her finish it. I placed the cup on her bedside table, and she thanked me before I left.\n"
            DET "Did she seem unwell or unusually drowsy at the time?\n"
            EVE "She seemed tired, but that wasn't unusual. I assumed she wanted to rest. I didn't think much of it.\n"
            DET "Did you put anything in the tea besides the usual ingredients?\n"
            EVE "No, Detective. Just chamomile tea, prepared as I normally would. I had no reason to give her anything else.\n"
            DET "I'll have to compare your account with the condition of the teacup and the other evidence.\n"
            EVE "That's understandable. I prepared the tea as part of my usual duties.",
            "Evelyn confirms delivering chamomile tea to Grace at approximately 10:00 PM. She does not recall seeing Grace finish it and says Grace appeared tired."},
        Choice{"Ask whether Evelyn noticed anything unusual in the bedroom.",
            DET "Did you notice anything unusual in Grace's bedroom when you delivered the tea?\n"
            EVE "Nothing in particular, Detective. I was there briefly, and I was focused on delivering the tea.\n"
            DET "A brief visit may not tell me much. Let's focus on what you remember clearly.",
            "No new evidence or notes are added."},
        Choice{"Ask whether anyone saw her prepare the tea.",
            DET "Was anyone in the kitchen who could confirm how you prepared the tea?\n"
            EVE "The kitchen wasn't particularly busy at that hour, Detective. I don't recall anyone watching me prepare that specific cup.\n"
            DET "Then I'll have to rely on the available evidence to verify your account.",
            "No new evidence or notes are added."});

    s.topics[1] = makeTopic("Ask about her duties and access to the household.",
        DET "As head chef, what areas of the mansion do you usually have access to?\n"
        EVE "Primarily the kitchen, pantry, and dining areas. I sometimes visit other rooms when delivering food or beverages, but I have no reason to enter private rooms without being asked.\n"
        DET "So you were familiar with the layout of the mansion?\n"
        EVE "Yes, of course. I've worked here long enough to know where most rooms are.",
        1, -1, -1,
        DET "You said your work mainly involved the kitchen, pantry, and dining areas. You denied having access to the safe's secondary key and said you didn't personally handle Grace's earrings.\n"
        EVE "That's right, Detective. I had no reason to handle her jewelry.",
        Choice{"Ask whether her duties gave her access to the Walk-in Closet.",
            DET "Did your duties ever require you to enter Grace's Walk-in Closet?\n"
            EVE "No, Detective. Delivering meals or beverages doesn't require me to enter her closet. I have no ordinary reason to go in there.\n"
            DET "Understood. Knowing the layout doesn't establish that you entered the closet.",
            "No new evidence or notes are added."},
        Choice{"Ask whether she knew where Grace kept the earrings.",
            DET "Did you know that Grace kept the Moonlit Pearl Earrings in the safe inside her Walk-in Closet?\n"
            EVE "I knew the earrings were valuable, Detective, but I wasn't involved in storing them. I don't normally enter the Walk-in Closet, and I had no reason to handle them.\n"
            DET "Did you know exactly where the safe was located?\n"
            EVE "Not from personal experience. I knew Miss Grace kept valuable belongings secured, but I never needed to open or inspect her safe.\n"
            DET "I'll keep that in mind. Familiarity with the household and knowledge of the safe are two different things.\n"
            EVE "Certainly, Detective. I understand why you're asking.",
            "Evelyn says she knew the earrings were valuable but denies knowing the safe's exact location through personal experience. Her claim has not been independently verified."},
        Choice{"Ask whether she had access to the safe's spare key.",
            DET "Did you have access to the secondary key for Grace's safe?\n"
            EVE "No, Detective. I have keys related to my work areas when necessary, but I don't have a safe key. The household's key arrangements are handled separately.\n"
            DET "I'll verify that against the mansion's key registry.",
            "No new evidence or notes are added."});

    s.topics[2] = makeTopic("Ask about her activities after leaving Grace's bedroom.",
        DET "You said you returned to your duties after delivering the tea. Where did you go afterward?\n"
        EVE "I went back downstairs toward the kitchen. I had some things to attend to before the night was over.\n"
        DET "Did you remain in the kitchen for the rest of the evening?\n"
        EVE "Not continuously. I moved around the household as needed, but I spent most of my time near the kitchen.",
        0, -1, -1,
        DET "You confirmed that you were using your phone in the corridor around 10:10 PM. You initially described it as a personal matter and said it was unrelated to the theft.\n"
        EVE "I was checking an online listing. I didn't think it was relevant to the case.",
        Choice{"Ask whether she was using her phone in the corridor that evening.",
            DET "Arthur reported seeing you using your phone in the corridor at around 10:10 PM. What were you doing there?\n"
            EVE "I was checking something on my phone, Detective. I had finished delivering the tea and was on my way back to the kitchen.\n"
            DET "What were you checking?\n"
            EVE "Just a personal matter. It had nothing to do with Miss Grace or her earrings.\n"
            DET "Why didn't you mention this when you first described your evening?\n"
            EVE "I didn't think checking my phone briefly was relevant. I wasn't trying to hide anything about the theft.\n"
            DET "Can you tell me what you were looking at?\n"
            EVE "I was checking an online listing. I didn't think it was important enough to mention.\n"
            DET "I'll note that you were in the corridor at that time. Your explanation will need to be considered alongside the timeline.\n"
            EVE "That's fair, Detective. But checking my phone doesn't mean I had anything to do with the missing earrings.",
            "Evelyn confirms using her phone in the corridor at approximately 10:10 PM. She initially describes it as a personal matter and does not volunteer the specific activity until questioned. Arthur's account supports the timing."},
        Choice{"Ask whether she noticed Arthur going upstairs.",
            DET "Did you see Arthur go upstairs that evening?\n"
            EVE "I can't say that I did. I was focused on my own tasks, and I wasn't keeping track of where everyone else was going.\n"
            DET "All right. Not seeing him doesn't establish where either of you were.",
            "No new evidence or notes are added."},
        Choice{"Ask whether she saw anyone carrying the earrings.",
            DET "Did you see anyone carrying the Moonlit Pearl Earrings that night?\n"
            EVE "No, Detective. I didn't see anyone carrying them. If I had, I would have remembered something like that.\n"
            DET "I see. I'll have to establish what happened through the timeline and physical evidence.",
            "No new evidence or notes are added."});
    s.numTopics = 3;
}

// Sets up Gideon's profile and three interview topics
void setupGideon() {
    setupSuspect(
        S_GIDEON, "Gideon Ocampo", "Gardener", 'G', 2, 13,
        "Age: 38\nOccupation: Head Groundskeeper / Gardener\nRelationship to Owner: Long-time Estate Staff\n"
        "Background: A quiet, observant groundskeeper who knows every secret passage and balcony route around the mansion after working there for a decade.",
        "I was outside checking the plants near the terrace and Entrance before the rain started. I finished my rounds by 9:30 PM and retired to the groundskeeper cottage. But around 10:15 PM, I saw the light inside the 2nd-floor Study Room flick on and off from the garden.",
        GID "Detective. I heard about Miss Grace's missing earrings this morning.\n"
        GID "I was outside checking the plants near the terrace and entrance before the rain started. I finished my rounds by 9:30 PM and retired to the groundskeeper cottage. But around 10:15 PM, I saw the light inside the second-floor Study Room flick on and off from the garden.\n"
        DET "You noticed activity in the Study Room after you had finished your rounds. I'd like to ask you a few questions about that evening.\n"
        GID "All right, Detective. I'll tell you what I remember.",
        DET "Thank you, Gideon. I've recorded your account of your evening, your duties, and your observation of the Study Room light. I'll compare your statements with the evidence before reaching a conclusion.\n"
        GID "I understand, Detective. I was doing my job, and I told you what I saw.",
        DET "I'll continue the investigation. Thank you for your cooperation.\n"
        GID "I hope you find whoever took the earrings, Detective.");
    Suspect& s = suspects[S_GIDEON];

    s.topics[0] = makeTopic("Ask about his evening and his whereabouts.",
        DET "You said you were checking the plants near the terrace and entrance before the rain started. Tell me more about your rounds that evening.\n"
        GID "I was making sure the plants were secure and that nothing would be damaged by the weather. I finished by 9:30 PM, then returned to the groundskeeper cottage.\n"
        DET "Did you leave the cottage again after that?\n"
        GID "Not that I recall. I was preparing to settle in for the night when I noticed the Study Room light from the garden.",
        0, -1, -1,
        DET "You said you checked the plants near the terrace and entrance, finished your rounds by 9:30 PM, and returned to the groundskeeper cottage. You cannot identify anyone who can independently confirm your movements throughout the evening.\n"
        GID "That's correct, Detective. I can only give you my own account.",
        Choice{"Ask whether anyone saw him during his rounds.",
            DET "Did anyone see you while you were checking the plants near the terrace and entrance?\n"
            GID "I can't say who was watching me the whole time. The staff may have seen me outside while going about their own duties, but I wasn't keeping track of them.\n"
            DET "So you can't name anyone who can confirm your movements?\n"
            GID "No, Detective. I can tell you what I was doing, but I can't guarantee someone was watching me every minute.\n"
            DET "And you maintain that you returned to the cottage by 9:30 PM?\n"
            GID "Yes. That's when I finished my rounds. I had no reason to stay outside working after that.\n"
            DET "I'll record your statement, but your account still needs to be checked against the other evidence.",
            "Gideon claims he finished his gardening rounds by 9:30 PM and returned to the groundskeeper cottage. He cannot identify anyone who can independently confirm his movements throughout the evening."},
        Choice{"Ask why he finished his rounds before 9:30 PM.",
            DET "Why did you finish your rounds before 9:30 PM?\n"
            GID "Because I had finished the work that needed doing before the rain started. There's nothing unusual about completing my duties early when the work is done.\n"
            DET "That explains why you finished early, but it doesn't help establish what happened afterward.",
            "No new evidence or notes are added."},
        Choice{"Ask whether he took anything from the mansion that evening.",
            DET "Did you take anything from the mansion when you finished your rounds?\n"
            GID "No, Detective. I was checking the plants outside. I had no reason to take anything from the mansion.\n"
            DET "I'll keep that in mind. I need to establish your movements before drawing any conclusions.",
            "No new evidence or notes are added."});

    s.topics[1] = makeTopic("Ask about his duties and access to the mansion.",
        DET "Explain your usual responsibilities around the estate. Do they ever require you to enter the mansion?\n"
        GID "Most of my work is outdoors. I maintain the garden and terrace, take care of the plants, and secure them when the weather turns bad. I only enter the mansion if there's a specific reason.\n"
        DET "Are you familiar with the second-floor rooms?\n"
        GID "I know where they are, of course. I've worked here long enough to understand the layout, but I don't have any regular duties upstairs.",
        1, -1, -1,
        DET "You stated that your duties were mainly outdoors and denied entering the Study Room that evening. You also denied having access to the safe's secondary key.\n"
        GID "Correct. The Study Room and its contents aren't part of my work.",
        Choice{"Ask whether his work required him to enter Grace's Walk-in Closet.",
            DET "Did your gardening duties ever require you to enter Grace's Walk-in Closet?\n"
            GID "No, Detective. I have no reason to enter her closet. My responsibilities are outside, not in the private rooms.\n"
            DET "Understood. Knowing the layout doesn't establish that you entered the closet.",
            "No new evidence or notes are added."},
        Choice{"Ask whether he had a reason to enter the second-floor Study Room that night.",
            DET "Did you have any work that required you to enter the second-floor Study Room that evening?\n"
            GID "No. The Study Room has nothing to do with my gardening duties. I had no reason to go there.\n"
            DET "Did you enter it for any other reason?\n"
            GID "No, Detective. I finished my rounds and returned to the cottage. I didn't go into the Study Room.\n"
            DET "You understand that the safe's secondary key is kept in a locked desk there?\n"
            GID "I understand what you're asking, but I don't have access to that key. I'm responsible for the grounds, not the household's valuables.\n"
            DET "I'll compare your statement with the key registry and the other evidence.\n"
            GID "That's all I can tell you, Detective. I had no reason to go inside.",
            "Gideon denies entering the second-floor Study Room that evening and says he had no work-related reason to go there. He also denies having access to the safe's secondary key. His account has not been independently verified."},
        Choice{"Ask whether he knew where the Moonlit Pearl Earrings were kept.",
            DET "Did you know the Moonlit Pearl Earrings were kept in a safe inside Grace's Walk-in Closet?\n"
            GID "I knew Miss Grace owned valuable jewelry, but I didn't know where she kept those particular earrings. That isn't information I need for my work.\n"
            DET "All right. I'll compare what you knew with the information available to the other suspects.",
            "No new evidence or notes are added."});

    s.topics[2] = makeTopic("Ask about the light in the second-floor Study Room.",
        DET "You mentioned seeing the light inside the second-floor Study Room flick on and off around 10:15 PM. Tell me exactly what you saw.\n"
        GID "I had already finished my rounds and returned to the groundskeeper cottage. From the garden, I noticed the Study Room light flick on and off. I couldn't see what was happening inside.\n"
        DET "Could you tell whether anyone was in the room?\n"
        GID "No. I was too far away to see anyone clearly. I only noticed the light.",
        0, -1, -1,
        DET "You claimed you saw the Study Room light flick on and off around 10:15 PM from the garden. You couldn't see who was inside or whether anyone entered or left.\n"
        GID "That's right. I saw the light, nothing more.",
        Choice{"Ask whether he saw anyone enter or leave the Study Room.",
            DET "Did you see anyone enter or leave the Study Room around that time?\n"
            GID "No. I only noticed the light flicking on and off. I didn't see anyone entering or leaving the room.\n"
            DET "You were watching from the garden. How certain are you that the light came from the Study Room?\n"
            GID "I'm certain it was that room. I know where it is from the garden. But I can't tell you who was inside.\n"
            DET "Did you report what you saw to anyone?\n"
            GID "No, Detective. At the time, I didn't think a light being switched on and off was suspicious. I had already finished my rounds and was preparing to settle in for the night.\n"
            DET "So your observation establishes that the Study Room was illuminated around 10:15 PM, but not who was there?\n"
            GID "That's right. I won't pretend I saw more than I did.",
            "Gideon claims he saw the second-floor Study Room light flick on and off at approximately 10:15 PM from the garden. He did not see anyone enter or leave and cannot identify who was inside."},
        Choice{"Ask whether he went inside the mansion to investigate the light.",
            DET "Did you go inside the mansion to investigate when you saw the Study Room light flicker?\n"
            GID "No, Detective. I was already back at the groundskeeper cottage. I didn't go inside. I didn't see a reason to investigate a light.\n"
            DET "All right. I'll verify your movements against the other statements.",
            "No new evidence or notes are added."},
        Choice{"Ask whether he could identify who turned the light on and off.",
            DET "Could you identify who turned the Study Room light on and off?\n"
            GID "No, Detective. I was outside, looking toward the mansion. I couldn't see anyone clearly enough to identify them.\n"
            DET "Then the light alone can't tell me who was in the room.",
            "No new evidence or notes are added."});
    s.numTopics = 3;
}

// Sets up Clara's profile and three interview topics
void setupClara() {
    setupSuspect(
        S_CLARA, "Clara Reyes", "Maid", 'C', 39, 15,
        "Age: 28\nOccupation: Personal Housemaid\nRelationship to Owner: Personal Maid\n"
        "Background: A quiet, germaphobic maid who meticulously sanitizes every surface in the mansion to cope with her private anxiety.",
        "I completed my turn-down service and disinfected the Master Bedroom and Walk-in Closet at 09:30 PM. The floor safe was shut tight, wiped clean, and locked. After finishing, I went straight down to the Staff Quarters for the night.",
        CLA "Detective. I understand you're investigating Miss Grace's missing earrings.\n"
        CLA "I completed my turn-down service and disinfected the Master Bedroom and Walk-in Closet at 9:30 PM. The floor safe was shut tight, wiped clean, and locked. After finishing, I went straight down to the Staff Quarters for the night.\n"
        DET "You were one of the last people to clean the room where the earrings were kept. I'd like to ask you a few questions about what you saw and did that evening.\n"
        CLA "Of course, Detective. I did my usual work, but I'll tell you everything I can remember.",
        DET "Thank you, Clara. I've recorded your account of the cleaning, your movements afterward, and what you observed outside. I'll compare your statements with the physical evidence and the other suspects' accounts.\n"
        CLA "I understand, Detective. I cleaned the room as part of my duties, and I went downstairs when I was finished. I hope that helps you find out what happened.",
        DET "I'll continue the investigation. Thank you for your cooperation.\n"
        CLA "Of course, Detective. I hope you find the truth.");
    Suspect& s = suspects[S_CLARA];

    s.topics[0] = makeTopic("Ask about her cleaning routine and the condition of the safe.",
        DET "You said you disinfected the Master Bedroom and Walk-in Closet at 9:30 PM. Tell me exactly what you did.\n"
        CLA "I completed the usual turn-down service. I tidied the room, disinfected the surfaces, and made sure everything was in order before leaving.\n"
        DET "What did you notice about the floor safe?\n"
        CLA "It was shut tight and locked. The outside looked clean, and I didn't notice any obvious damage or signs that someone had forced it open.",
        0, -1, -1,
        DET "You said you disinfected the Master Bedroom and Walk-in Closet at 9:30 PM. You wiped the outside of the safe, which was locked and showed no obvious damage.\n"
        CLA "That's right, Detective. I didn't open the safe or touch anything inside.",
        Choice{"Ask whether she touched or wiped the safe itself.",
            DET "You mentioned that the safe was wiped clean. Did you wipe the safe yourself?\n"
            CLA "Yes, Detective. I was disinfecting the surfaces around the room, including the outside of the safe. I didn't open it or handle anything inside.\n"
            DET "Could your cleaning have removed fingerprints or other traces from the safe?\n"
            CLA "It's possible. I was cleaning the exterior, so I may have removed marks that were already there. But I wasn't trying to hide anything. It was part of my normal duties.\n"
            DET "Did you notice any scratches, damage, or signs of tampering before you cleaned it?\n"
            CLA "No. The safe appeared intact, and it was locked when I finished. I didn't see anything that made me think someone had forced it open.\n"
            DET "Your cleaning may explain why the safe's exterior was clean, but I'll need to establish whether anyone accessed it afterward.\n"
            CLA "I understand, Detective. I left it exactly as I found it, except for cleaning the outside.",
            "Clara confirms wiping and disinfecting the exterior of the floor safe during her 9:30 PM cleaning routine. She says the safe was locked and showed no obvious damage. Her cleaning may have removed traces from the exterior."},
        Choice{"Ask whether she opened the safe to check the earrings.",
            DET "Did you open the safe to make sure the earrings were still inside?\n"
            CLA "No, Detective. I have no reason to open Miss Grace's safe. I only cleaned the outside of it.\n"
            DET "Understood. I'll need to establish who had access to the safe and its keys.",
            "No new evidence or notes are added."},
        Choice{"Ask whether she noticed anything unusual in the closet.",
            DET "Did you notice anything unusual in the Walk-in Closet while cleaning?\n"
            CLA "Nothing unusual, Detective. Everything appeared to be in order, and I didn't notice any obvious disturbance.\n"
            DET "All right. I'll compare your account with the physical evidence from the room.",
            "No new evidence or notes are added."});

    s.topics[1] = makeTopic("Ask about her movements after finishing her duties.",
        DET "You said you went straight to the Staff Quarters after finishing your cleaning at 9:30 PM. Is that correct?\n"
        CLA "Yes, Detective. I finished my work and went downstairs. I had no further duties in the Master Bedroom that night.\n"
        DET "Did you leave the Staff Quarters again later that evening?\n"
        CLA "I don't recall going back upstairs after I finished. I was done with my work for the night.",
        0, -1, -1,
        DET "You claimed you went straight to the Staff Quarters after finishing your duties and did not return to the second floor. You cannot identify anyone who can confirm your whereabouts for the entire night.\n"
        CLA "Correct. I finished my work and went downstairs.",
        Choice{"Ask whether anyone can confirm she went to the Staff Quarters.",
            DET "Did anyone see you arrive at the Staff Quarters after finishing your work?\n"
            CLA "I can't say whether anyone noticed me arrive, Detective. Other staff members may have been around, but I wasn't paying attention to who was there.\n"
            DET "So you don't have anyone who can confirm your whereabouts for the entire night?\n"
            CLA "No, Detective. I can tell you that I went downstairs after finishing my duties, but I can't say someone watched me the whole time.\n"
            DET "Did you have any reason to return to the Master Bedroom or Walk-in Closet after 9:30 PM?\n"
            CLA "No. My cleaning was finished, and I had no reason to enter those rooms again.\n"
            DET "I'll compare your statement with the other accounts and the evidence from the second floor.\n"
            CLA "That's all I can tell you, Detective. I didn't return to the room after cleaning it.",
            "Clara claims she went straight to the Staff Quarters after completing her duties at 9:30 PM. She cannot identify anyone who can independently confirm her whereabouts throughout the night."},
        Choice{"Ask whether she returned to the second floor later that night.",
            DET "Did you return to the second floor at any point after 9:30 PM?\n"
            CLA "No, Detective. I had finished my duties and went downstairs to the Staff Quarters. I didn't have a reason to go back.\n"
            DET "I'll need to compare that statement with the rest of the timeline.",
            "No new evidence or notes are added."},
        Choice{"Ask whether she heard anything unusual after going downstairs.",
            DET "Did you hear anything unusual after you went downstairs?\n"
            CLA "I don't recall hearing anything specific, Detective. I was no longer working upstairs, and I wasn't paying attention to what was happening in the rest of the mansion.\n"
            DET "I see. Your account doesn't establish what happened upstairs after you left.",
            "No new evidence or notes are added."});

    s.topics[2] = makeTopic("Ask about Gideon and what she noticed that evening.",
        DET "Did you notice anyone near the garden or balcony while you were finishing your duties?\n"
        CLA "I remember seeing Gideon outside near the garden. It was late, and I thought it was unusual to see him there at that hour.\n"
        DET "Did you see him enter the mansion?\n"
        CLA "I don't remember seeing him enter. I only noticed him outside.",
        1, -1, -1,
        DET "You saw Gideon outside near the garden, but you didn't see him enter the mansion or approach the Study Room or Walk-in Closet. Your suspicion was based on seeing him outside late at night.\n"
        CLA "That's right. I can't say he did anything wrong because I didn't see him do anything.",
        Choice{"Ask why she found Gideon's presence suspicious.",
            DET "Why did seeing Gideon outside make you suspicious?\n"
            CLA "I suppose because it was late, Detective. But he is the groundskeeper, so he has reasons to be outside. I didn't see him doing anything wrong.\n"
            DET "Being outside isn't proof of wrongdoing. I'll need something more specific than suspicion.",
            "No new evidence or notes are added."},
        Choice{"Ask whether she actually saw Gideon enter the mansion.",
            DET "You said you saw Gideon near the garden. Did you actually see him enter the mansion?\n"
            CLA "No, Detective. I only saw him outside. I can't say that he went inside because I didn't see him do so.\n"
            DET "Did you see him near the second-floor Study Room or the Walk-in Closet?\n"
            CLA "No. I didn't see him near either of those rooms. I only noticed him from the outside.\n"
            DET "Then you can't confirm that he entered the mansion or approached the room where the earrings were kept?\n"
            CLA "That's correct. I don't want to accuse him of something I didn't actually see.\n"
            DET "That's an important distinction. I'll treat what you actually saw separately from what you suspected.\n"
            CLA "Thank you, Detective. I only want to tell you what I know.",
            "Clara confirms seeing Gideon outside near the garden but did not see him enter the mansion or approach the Study Room or Walk-in Closet. Her suspicion was based on the time and location, not direct evidence of wrongdoing."},
        Choice{"Ask whether she confronted Gideon about being outside.",
            DET "Did you confront Gideon when you saw him outside?\n"
            CLA "No, Detective. I didn't approach him or ask what he was doing. I was finishing my own duties.\n"
            DET "All right. Then you don't have any further information about his activities that night.",
            "No new evidence or notes are added."});
    s.numTopics = 3;
}

// Resets evidence, notes, scores, and all suspect data for a new game
void resetGame() {
    noteCount = 0;
    correctCount = 0;
    wrongCount = 0;
    for (int i = 0; i < MAX_NOTES; i++) notes[i] = "";

    evidenceList[0] = {"Intact Floor Safe", "Location: Walk-in Closet. The floor safe where the Moonlit Pearl Earrings were kept. It shows zero physical damage, scratches, or signs of lockpicking. Can only be unlocked using the primary safe key or the secondary override key.", true};
    evidenceList[1] = {"Mansion Key Registry", "Location: Study Room (2nd Floor Desk). Official log of estate keys. The Primary Safe Key is kept by the Owner. A Secondary Override Key is kept in the locked 2nd-Floor Study Room desk. Staff members carry sub-keys for bedroom access only.", true};
    evidenceList[2] = {"Half-Empty Teacup", "Location: Bedroom 1. Contains residual chamomile tea delivered by Chef Evelyn at 10:00 PM. The Owner fell asleep shortly after drinking it.", true};
    evidenceList[3] = {"Sanitizer Wipe Residue", "Location: Walk-in Closet Threshold. Fresh disinfectant traces near the closet door, confirming Maid Clara's routine cleaning at 09:30 PM.", true};
    evidenceList[EV_CUFFLINK] = {"Gold Cufflink", "Location: found near the safe, now with Arthur. Julian's gold cufflink, picked up by the Butler at about 11:15 PM. It was no longer at the scene when you arrived.", false};
    evidenceList[EV_INK] = {"Ink-Stained Spare Key", "Location: Study Room desk. The secondary safe key has faint traces of fountain pen ink on its lock mechanism, the same ink Julian uses with his stock-trading pen.", false};
    evidenceList[EV_CATALOG] = {"Handbag Catalog Pages", "Location: Staff Quarters trash can. Torn pages from luxury designer handbag catalogs. They show Evelyn's hobby, not that she ever entered the closet.", false};
    evidenceList[EV_SOIL] = {"Garden Soil Traces", "Location: Balcony door, 2nd floor. Small traces of garden soil. Plants are tied to the balcony pillars, so this does not place anyone in the Walk-in Closet.", false};

    setupJulian();
    setupArthur();
    setupEvelyn();
    setupGideon();
    setupClara();
}

// Returns the room name based on map coordinates
string getRoomName(int x, int y) {
    if (x < 3 || x > 56 || y < 2 || y > 18) return "Manor Garden";
    if (y == 8 || y == 12) return "Doorway";
    if (y <= 7) {
        if (x < 17) return "Bedroom 1 (2F)";
        if (x < 31) return "Walk-in Closet (2F)";
        if (x < 44) return "Study Room (2F)";
        return "Balcony (2F)";
    }
    if (y <= 11) return "Grand Hall (1F)";
    if (x < 17) return "Kitchen (1F)";
    if (x < 31) return "Living Room (1F)";
    if (x < 44) return "Staff Quarters (1F)";
    return "Dining Room (1F)";
}

// Returns true if the tile is walkable and not occupied by a suspect
bool canMove(int x, int y) {
    if (x < 0 || x >= MAP_W || y < 0 || y >= MAP_H) return false;
    char c = gameMap[y][x];
    if (c != ' ' && c != '.') return false;
    for (int i = 0; i < NUM_SUSPECTS; i++)
        if (suspects[i].x == x && suspects[i].y == y) return false;
    return true;
}

// Returns true if the player is within 1 tile of (x, y)
bool isNear(int px, int py, int x, int y) {
    return abs(px - x) <= 1 && abs(py - y) <= 1;
}

// Returns true if a given map symbol is within 1 tile of the player
bool nearSymbol(int px, int py, char symbol) {
    for (int dy = -1; dy <= 1; dy++)
        for (int dx = -1; dx <= 1; dx++) {
            int x = px + dx, y = py + dy;
            if (x >= 0 && x < MAP_W && y >= 0 && y < MAP_H &&
                gameMap[y][x] == symbol) return true;
        }
    return false;
}

// Returns the ANSI color used to draw a map character
const char* mapColor(char ch) {
    switch (ch) {
        case ' ': return "\033[48;5;236m";
        case '#': return "\033[48;5;28m";
        case '.': return "\033[48;5;22m";
        case '/': case '\\': return "\033[48;5;124m";
        case '|': case '-': case '+': return "\033[48;5;244m";
        case '$': return C_SAFE;
        case '&': return C_DESK;
        case '%': return C_TRASH;
        case '^': return C_SOIL;
        default:
            if (ch >= 'A' && ch <= 'Z') return "\033[1;30;48;5;252m";
            return "\033[48;5;94m";
    }
}

// Returns the glyph used to draw a map character
char cellGlyph(char ch) {
    if (ch == '$' || ch == '&' || ch == '%' || ch == '^') return ch;
    if (ch >= 'A' && ch <= 'Z') return ch;
    return ' ';
}

// Wraps a character in a color and reset code
string tag(const char* col, char s) { return string(col) + s + C_RESET; }

// Draws the map, player, suspects, and side-panel UI
void drawScreen(int px, int py, const string& msg) {
    clearScreen();

    string uiLines[MAP_H];
    for (int i = 0; i < MAP_H; i++) uiLines[i] = "";

    uiLines[0] = string(C_BCYAN) + C_BOLD + " ◆ LOCATION" + C_RESET;
    uiLines[1] = string(C_BYELLOW) + " " + getRoomName(px, py) + C_RESET;
    uiLines[2] = " Evidence: " + string(C_BGREEN) + to_string(countEvidence()) + "/" + to_string(NUM_EVIDENCE) + C_RESET;
    uiLines[3] = " Score: " + string(C_BYELLOW) + to_string(investigationScore()) + C_RESET;
    uiLines[5] = string(C_BCYAN) + C_BOLD + " ◆ LEGEND" + C_RESET;
    uiLines[6] = " " + tag(C_PLAYER, '@') + " You   " + tag(C_SAFE, '$') + " Safe   " + tag(C_DESK, '&') + " Desk";
    uiLines[7] = " " + tag(C_TRASH, '%') + " Trash can   " + tag(C_SOIL, '^') + " Soil";
    uiLines[8] = " " + tag(SUSPECT_BG[0], 'J') + " Julian   " + tag(SUSPECT_BG[1], 'A') + " Arthur";
    uiLines[9] = " " + tag(SUSPECT_BG[2], 'E') + " Evelyn   " + tag(SUSPECT_BG[3], 'G') + " Gideon";
    uiLines[10] = " " + tag(SUSPECT_BG[4], 'C') + " Clara";
    uiLines[12] = string(C_BCYAN) + C_BOLD + " ◆ CONTROLS" + C_RESET;
    uiLines[13] = " [WASD] Move  [E] Interact";
    uiLines[14] = " [C] Case File  [N] Notes";
    uiLines[15] = " [X] Deduction  [Q] Menu";
    uiLines[17] = string(C_BCYAN) + C_BOLD + " ◆ STATUS" + C_RESET;
    string rest = msg;
    for (int i = 18; i < 21; i++)
        uiLines[i] = string(C_BYELLOW) + " " + takeLine(rest, 31) + C_RESET;

    for (int r = 0; r < MAP_H; r++) {
        string line = "";
        for (int c = 0; c < MAP_W; c++) {
            char ch = gameMap[r][c];
            string col = mapColor(ch);
            char glyph = cellGlyph(ch);

            for (int i = 0; i < NUM_SUSPECTS; i++) {
                if (suspects[i].x == c && suspects[i].y == r) {
                    glyph = suspects[i].symbol;
                    col = SUSPECT_BG[i];
                }
            }
            if (r == py && c == px) { glyph = '@'; col = C_PLAYER; }
            line += col + glyph + string(CELL_W - 1, ' ') + C_RESET;
        }
        cout << line << C_GRAY << "   │   " << C_RESET << uiLines[r] << "\n";
    }
}

// Shows a suspect's profile, statement, and status
void showPerson(int i) {
    Suspect& p = suspects[i];
    clearScreen();
    printHeader("PERSON OF INTEREST: " + p.name);
    cout << "\n";
    printWrapped(p.profile, 2, 4, TEXT_WIDTH);
    cout << "\n  Initial Statement:\n";
    printWrapped("\"" + p.statement + "\"", 4, 4, TEXT_WIDTH);
    cout << "\n  Status: " << stateText(p.state) << "\n";
    waitKey();
}

// Shows the case overview and crime scene summary
void showCaseOverview() {
    clearScreen();
    printHeader("CASE OVERVIEW AND CRIME SCENE");
    cout << "\n CASE OVERVIEW\n";
    printWrapped("On the morning of September 19, a high-value internal theft occurred at the estate. The Moonlit Pearl Earrings, a priceless heirloom gifted to Grace Ivory by her grandmother, were reported missing from her private dressing room.", 2, 2, TEXT_WIDTH);
    cout << "\n CRIME SCENE\n";
    printWrapped("Location: Walk-in Closet, Second Floor", 2, 2, TEXT_WIDTH);
    printWrapped("* No signs of forced entry were found on the doors of the Master Bedroom or Walk-in Closet.", 2, 4, TEXT_WIDTH);
    printWrapped("* The safe was found closed and locked, with no visible scratches, tampering, or damage around the keyhole.", 2, 4, TEXT_WIDTH);
    printWrapped("* The room appeared clean and orderly, with no obvious signs of disturbance.", 2, 4, TEXT_WIDTH);
    printWrapped("* A faint smell of sanitizer lingered in the Walk-in Closet.", 2, 4, TEXT_WIDTH);
    waitKey();
}

// Displays the case file menu with overview, timeline, evidence, and persons
void showCaseFile() {
    while (true) {
        clearScreen();
        printHeader("CASE FILE: THE MOONLIT HEIST");
        cout << "\n  Case Name: Moonlit Heist\n"
             << "  Case File Ref: #MH-2026-1019\n"
             << "  Victim: Grace Ivory\n"
             << "  Stolen Property: The Moonlit Pearl Earrings\n\n"
             << "   [1] Case Overview and Crime Scene\n"
             << "   [2] Timeline of Events\n"
             << "   [3] Evidence Log (" << countEvidence() << "/" << NUM_EVIDENCE << " found)\n"
             << "   [4] Persons of Interest\n"
             << "   [0] Close the Case File\n\n  Your choice: ";
        int c = askChoice(4, true);
        if (c == 0) return;

        if (c == 1) { showCaseOverview(); continue; }

        clearScreen();
        if (c == 2) {
            printHeader("TIMELINE OF EVENTS");
            cout << "\n";
            printWrapped("09:00 PM: The Owner verifies that the Moonlit Pearl Earrings are secured inside the floor safe within the Walk-in Closet.", 2, 12, TEXT_WIDTH);
            printWrapped("09:30 PM: The Maid enters the Master Bedroom suite and Walk-in Closet for her scheduled evening cleaning and sanitation routine.", 2, 12, TEXT_WIDTH);
            printWrapped("10:00 PM: The Chef delivers bedtime tea and light pastries to the Owner in Bedroom 1.", 2, 12, TEXT_WIDTH);
            printWrapped("10:10 PM - 10:30 PM: Multiple household members reported movement on the 2nd floor, including sightings near the Study Room and outer Balcony.", 2, 4, TEXT_WIDTH);
            printWrapped("10:30 PM - 12:00 AM: (ESTIMATED WINDOW OF THEFT) Exact time of breach remains unknown. The Owner falls asleep shortly after reading in Bedroom 1.", 2, 4, TEXT_WIDTH);
            printWrapped("11:15 PM: The Butler conducts a patrol of the 2nd-floor corridor after reporting an unusual noise near the Study Room.", 2, 12, TEXT_WIDTH);
            printWrapped("07:00 AM: The Owner opens the Walk-in Closet safe to prepare for her birthday celebration and discovers the earrings are missing.", 2, 12, TEXT_WIDTH);
        } else if (c == 3) {
            printHeader("EVIDENCE LOG");
            for (int i = 0; i < NUM_EVIDENCE; i++)
                if (evidenceList[i].found) {
                    cout << "\n  * " << evidenceList[i].name << "\n";
                    printWrapped(evidenceList[i].desc, 4, 4, TEXT_WIDTH);
                }
        } else {
            printHeader("PERSONS OF INTEREST");
            cout << "\n";
            for (int i = 0; i < NUM_SUSPECTS; i++)
                cout << "   [" << (i + 1) << "] " << suspects[i].name << " (" << suspects[i].role
                     << ") - " << stateText(suspects[i].state) << "\n";
            cout << "   [0] Back\n\n  Open whose file? ";
            int s = askChoice(NUM_SUSPECTS, true);
            if (s != 0) showPerson(s - 1);
            continue;
        }
        waitKey();
    }
}

// Shows investigation notes in pages of five
void showNotes() {
    const int PER_PAGE = 5;
    int pages = (noteCount + PER_PAGE - 1) / PER_PAGE;
    if (pages < 1) pages = 1;
    for (int pg = 0; pg < pages; pg++) {
        clearScreen();
        printHeader("INVESTIGATION NOTES  (page " + to_string(pg + 1) + " of " + to_string(pages) + ")");
        if (noteCount == 0)
            cout << "\n  No notes yet. Question the suspects and search the mansion to fill this in.\n";
        for (int i = pg * PER_PAGE; i < noteCount && i < (pg + 1) * PER_PAGE; i++) {
            cout << "\n";
            printWrapped(to_string(i + 1) + ". " + notes[i], 2, 5, TEXT_WIDTH);
        }
        waitKey();
    }
}

// Shows the description of the floor safe
void examineSafe() {
    clearScreen();
    printHeader("THE FLOOR SAFE: WALK-IN CLOSET");
    cout << "\n";
    printWrapped("The floor safe is shut and locked. There are no scratches, no tampering and no damage around the keyhole. It can only be opened with the primary safe key or the secondary override key. The closet is clean and orderly, and a faint smell of sanitizer lingers in the air. Whoever opened it used a key.", 2, 2, TEXT_WIDTH);
    waitKey();
}

// Examines the Study Room desk and may reveal the ink evidence
void examineDesk() {
    clearScreen();
    printHeader("THE STUDY ROOM DESK");
    cout << "\n";
    if (evidenceList[EV_INK].found) {
        printWrapped("You have already examined the spare safe key. Faint fountain pen ink is still visible on its lock mechanism.", 2, 2, TEXT_WIDTH);
    } else if (suspects[S_GIDEON].topics[2].done || evidenceList[EV_CUFFLINK].found) {
        printWrapped("The Key Registry says the secondary safe key is kept in this locked desk. You open the drawer and examine the key under the lamp. The key is there, but its lock mechanism has faint traces of fountain pen ink, the same ink Julian uses with his stock-trading pen.", 2, 2, TEXT_WIDTH);
        foundEvidence(EV_INK, "Study Room desk: faint fountain pen ink on the secondary safe key matches Julian's stock-trading pen. Someone handled the spare key.");
    } else {
        printWrapped("A locked mahogany desk. The Key Registry says the secondary safe key is kept inside, but nothing points to it yet. (Hint: someone may have noticed activity in this room, or something may turn up as you question people.)", 2, 2, TEXT_WIDTH);
    }
    waitKey();
}

// Examines the Staff Quarters trash can and reveals the catalog evidence
void examineTrash() {
    clearScreen();
    printHeader("STAFF QUARTERS TRASH CAN");
    cout << "\n";
    if (evidenceList[EV_CATALOG].found) {
        printWrapped("Only the torn designer handbag catalog pages you already found.", 2, 2, TEXT_WIDTH);
    } else {
        printWrapped("Torn pages from luxury designer handbag catalogs fill the bin. It could look suspicious at first glance.", 2, 2, TEXT_WIDTH);
        foundEvidence(EV_CATALOG, "Staff Quarters trash can: torn designer handbag catalog pages. They show a hobby, not that anyone entered the closet.");
    }
    waitKey();
}

// Examines the balcony door and reveals the soil evidence
void examineSoil() {
    clearScreen();
    printHeader("THE BALCONY DOOR");
    cout << "\n";
    if (evidenceList[EV_SOIL].found) {
        printWrapped("Only the garden soil traces you already found.", 2, 2, TEXT_WIDTH);
    } else {
        printWrapped("There are small traces of garden soil near the Balcony door. Plants are tied to the pillars out here, so it is easy to track in soil.", 2, 2, TEXT_WIDTH);
        foundEvidence(EV_SOIL, "Balcony door: small traces of garden soil. It does not place anyone in the Walk-in Closet.");
    }
    waitKey();
}

// Returns true if a topic is unlocked by required evidence
bool topicVisible(Suspect& p, int t) {
    int need = p.topics[t].needEvidence;
    return need < 0 || evidenceList[need].found;
}

// Returns true if all topics for a suspect are completed
bool allTopicsDone(Suspect& p) {
    for (int t = 0; t < p.numTopics; t++)
        if (!p.topics[t].done) return false;
    return true;
}

// Runs a single topic's question loop until answered or reviewed
void runTopic(int s, int t) {
    Suspect& p = suspects[s];
    Topic& tp = p.topics[t];
    string title = p.name + " (" + p.role + ")";

    if (tp.done) {
        clearScreen();
        printHeader("REVIEW: " + title);
        cout << "\n";
        printDialogue(tp.review);
        waitKey();
        return;
    }

    while (true) {
        clearScreen();
        printHeader("INTERVIEW: " + title + "   Score: " + to_string(investigationScore()));
        cout << "\n";
        printDialogue(tp.intro);
        cout << "\n  How do you respond?\n";
        for (int i = 0; i < 3; i++)
            printWrapped("[" + to_string(i + 1) + "] " + tp.ch[i].label, 3, 7, TEXT_WIDTH);
        cout << "   [0] Back to the topic menu\n\n  Your choice: ";

        int c = askChoice(3, true);
        if (c == 0) return;

        Choice& ch = tp.ch[c - 1];
        clearScreen();
        printHeader("INTERVIEW: " + title);
        cout << "\n";
        printDialogue(ch.reply);

        if (c - 1 == tp.correct) {
            correctCount++;
            cout << "\n  " << C_BGREEN << C_BOLD << "✔ CORRECT! +50 points" << C_RESET << "\n";
            addNote(p.name + ": " + ch.feedback);
            printWrapped("Added to Notes: " + ch.feedback, 2, 4, TEXT_WIDTH);
            if (tp.evidence >= 0) foundEvidence(tp.evidence, "");
            tp.done = true;
            waitKey();
            return;
        }
        wrongCount++;
        cout << "\n  " << C_BRED << C_BOLD << "✘ WRONG! -20 points" << C_RESET << "\n";
        printWrapped("Display: " + ch.feedback, 2, 4, TEXT_WIDTH);
        cout << "  Try again.\n";
        waitKey();
    }
}

// Main interview loop for a suspect (opening, topics, closing)
void interview(int s) {
    Suspect& p = suspects[s];
    string title = p.name + " (" + p.role + ")";

    if (p.state == 0) {
        clearScreen();
        printHeader("INTERVIEW: " + title);
        cout << "\n";
        printDialogue(p.opening);
        p.state = 1;
        waitKey();
    }

    while (true) {
        clearScreen();
        printHeader("INTERVIEW: " + title + "   Score: " + to_string(investigationScore()));
        cout << "\n  " << stateText(p.state) << ". What would you like to ask?\n\n";
        int shown = 0;
        int pick[MAX_TOPICS];
        for (int t = 0; t < p.numTopics; t++) {
            if (!topicVisible(p, t)) continue;
            pick[shown] = t;
            shown++;
            printWrapped("[" + to_string(shown) + "] " +
                         (p.topics[t].done ? "Review: " : "") + p.topics[t].label,
                         3, 7, TEXT_WIDTH);
        }
        cout << "   [0] End conversation\n\n  Your choice: ";

        int c = askChoice(shown, true);
        if (c == 0) {
            clearScreen();
            printHeader("INTERVIEW: " + title);
            cout << "\n";
            printDialogue(p.farewell);
            waitKey();
            return;
        }
        runTopic(s, pick[c - 1]);

        if (p.state != 2 && allTopicsDone(p)) {
            p.state = 2;
            clearScreen();
            printHeader("INTERVIEW COMPLETE: " + p.name);
            cout << "\n";
            printDialogue(p.closing);
            cout << "\n  You can review this interview any time from the topic menu.\n";
            waitKey();
        }
    }
}

// Prints a numbered list of deduction options
void showOptions(const string options[], int count) {
    for (int i = 0; i < count; i++)
        printWrapped("[" + to_string(i + 1) + "] " + options[i], 3, 7, 90);
}

// Runs the final deduction and returns true if the case ends
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
    showOptions(reasonOptions, 5);
    cout << "\n  Your choice: ";
    int r = askChoice(5, false) - 1;

    clearScreen();
    printHeader("DEDUCTION: MOTIVE");
    cout << "\n  Why did " << suspects[s].name << " do it?\n\n";
    showOptions(motiveOptions, 5);
    cout << "\n  Your choice: ";
    int m = askChoice(5, false) - 1;

    clearScreen();
    printHeader("DEDUCTION: SUPPORTING EVIDENCE");
    cout << "\n  Which evidence proves it?\n\n";
    showOptions(evidenceOptions, 5);
    cout << "\n  Your choice: ";
    int e = askChoice(5, false) - 1;

    bool okS = (s == CORRECT_SUSPECT), okR = (r == CORRECT_REASON);
    bool okM = (m == CORRECT_MOTIVE),  okE = (e == CORRECT_EVIDENCE);

    int deductionScore = 0;
    if (okS) deductionScore += 200;
    if (okR) deductionScore += 100;
    if (okM) deductionScore += 100;
    if (okE) deductionScore += 100;

    clearScreen();
    if (okS && okR && okM && okE) {
        int total = investigationScore() + efficiencyBonus() + deductionScore;
        printHeader("CASE SOLVED!");
        cout << "\n";
        printWrapped("You confront Julian Vance with the ink-stained spare key. His frame-up collapses: he took the spare key from the Study Room desk at 10:15 PM, emptied the safe at 10:40 PM and dropped his own cufflink so Arthur would take the blame. A frame-up needs the evidence to stay at the scene, and the cufflink never did. The Moonlit Pearl Earrings are found in his luggage, and Julian admits he needed the money to pay off his debts.", 2, 2, TEXT_WIDTH);
        cout << "\n  Investigation score : " << investigationScore() << "\n";
        cout << "  Efficiency bonus    : " << efficiencyBonus() << "\n";
        cout << "  Deduction score     : " << deductionScore << "\n";
        cout << "  ----------------------------\n";
        cout << "  FINAL SCORE         : " << total << " / 1000\n";
        string rank = "Trainee Detective";
        if (total >= 900)      rank = "Master Detective";
        else if (total >= 700) rank = "Senior Detective";
        else if (total >= 500) rank = "Rookie Detective";
        cout << "  Rank                : " << rank << "\n";
        return true;
    }

    printHeader("GAME OVER - THE THIEF ESCAPES");
    cout << "\n";
    int wrongTotal = (!okS) + (!okR) + (!okM) + (!okE);
    if (!okS) printWrapped("* WRONG SUSPECT: The detective identifies the wrong person, resulting in a failed investigation.", 2, 4, TEXT_WIDTH);
    if (!okR) printWrapped("* WRONG REASONING: The accusation lacks sufficient justification.", 2, 4, TEXT_WIDTH);
    if (!okM) printWrapped("* WRONG MOTIVE: The detective fails to establish why the suspect committed the theft.", 2, 4, TEXT_WIDTH);
    if (!okE) printWrapped("* WRONG EVIDENCE: The suspect escapes due to insufficient evidence.", 2, 4, TEXT_WIDTH);
    if (wrongTotal > 1)
        cout << "\n  Multiple mistakes: the failures above combine. The case collapses.\n";
    cout << "\n  Final Score: 0 (Game Over)\n";
    return true;
}

// Main exploration loop: movement, interaction, case file, notes, deduction
int exploreLoop() {
    int px = 30, py = 10;
    string msg = "Welcome, Detective. Walk up to someone and press E to question them.";

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
        else if (key == 'n') { showNotes();   msg = "Notes closed.";     continue; }
        else if (key == 'e') {
            bool done = false;
            for (int i = 0; i < NUM_SUSPECTS && !done; i++) {
                if (isNear(px, py, suspects[i].x, suspects[i].y)) {
                    interview(i);
                    msg = "You finished talking to " + suspects[i].name + ".";
                    done = true;
                }
            }
            if (!done && nearSymbol(px, py, '$')) { examineSafe();  msg = "You examined the floor safe.";          done = true; }
            if (!done && nearSymbol(px, py, '&')) { examineDesk();  msg = "You examined the Study Room desk.";      done = true; }
            if (!done && nearSymbol(px, py, '%')) { examineTrash(); msg = "You searched the trash can.";            done = true; }
            if (!done && nearSymbol(px, py, '^')) { examineSoil();  msg = "You examined the Balcony door.";         done = true; }
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

// Runs a full game session, allowing replay or return to menu
void playGame() {
    while (true) {
        resetGame();
        showCaseOverview();
        int result = exploreLoop();
        if (result == 0) return;

        cout << "\n  [1] Play Again   [2] Main Menu\n\n  Your choice: ";
        int c = askChoice(2, false);
        if (c == 2) return;
    }
}

// Entry point: setup, instructions, and main menu loop
int main() {
    setupConsole();

    int  selected = 0;
    bool running  = true;
    char input;

    while (running) {
        clearScreen();
        drawMenu(selected);

        input = (char)readMenuKey();

        if (input == 'w' || input == 'W') {
            selected--;
            if (selected < 0) selected = 3;
        }
        else if (input == 's' || input == 'S') {
            selected++;
            if (selected > 3) selected = 0;
        }
        else if (input >= '1' && input <= '4') {
            selected = input - '1';
            input = 'e';
        }

        if (input == 'e' || input == 'E' || input == '\r') {
            clearScreen();
            cout << "\n  ============================================\n\n";

            switch (selected) {
                case 0: showInstructions(); break;
                case 1: playGame();         break;
                case 2: cout << "      Goodbye. See you again!\n";
                        running = false;    break;
                case 3: showCredits();      break;
            }
        }
    }
    return 0;
}