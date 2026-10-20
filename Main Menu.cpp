// Include the libraries we need.
// <iostream> lets us print text (cout) and read keyboard input (cin).
// <string> lets us use the "string" type to store words.
#include <iostream>
#include <string>

using namespace std;

// A function that "clears" the screen.
// Instead of using system("cls") (which only works on some computers),
// we simply print many blank lines to push the old menu out of view.
void clearScreen() {
    for (int i = 0; i < 30; i++) {
        cout << "\n";
    }
}

//  A function that draws the whole menu.
// "selected" tells us which option is currently highlighted (0 to 3).
void drawMenu(int selected) {

    // An array holding the names of our menu options.
    string options[4] = { "NEW GAME", "LOAD GAME", "INSTRUCTION", "QUIT" };

    // The top border and title.
    cout << "\n";
    cout << "  ============================================\n";
    cout << "  ||                                        ||\n";
    cout << "  ||            The Moonlit Heist           ||\n";
    cout << "  ||                                        ||\n";
    cout << "  ============================================\n\n";

    // Loop through every option and print it.
    // "i" counts 0, 1, 2, 3 - one number per option.
    for (int i = 0; i < 4; i++) {

        // Each option is pushed further to the right than the one above it.
        // This creates the slanted "staircase" look.
        int indent = 6 + (i * 4);

        // Print empty spaces to move the text to the right.
        for (int s = 0; s < indent; s++) {
            cout << " ";
        }

        // Is this the option the player is currently on?
        if (i == selected) {
            // YES: draw it big and boxed so it stands out.
            cout << "/////////////////////////\n";
            for (int s = 0; s < indent; s++) cout << " ";
            cout << "//>>  " << (i + 1) << ". " << options[i] << "  <<\n";
            for (int s = 0; s < indent; s++) cout << " ";
            cout << "/////////////////////////\n";
        } else {
            // NO: draw it plain and quiet.
            cout << "     " << (i + 1) << ". " << options[i] << "\n";
        }
        cout << "\n";
    }

    // Footer with the controls.
    cout << "  --------------------------------------------\n";
    cout << "   [W] Up    [S] Down    [E] Select    [1-4]\n";
    cout << "  --------------------------------------------\n";
    cout << "   > ";
}

// The main function - where the program starts running.
int main() {

    int selected = 0;      // Which option is highlighted. 0 = NEW GAME.
    bool running = true;   // The menu loop keeps going while this is true.
    char input;            // Stores the key the player types.

    // The main loop. It redraws the menu again and again
    // until the player chooses QUIT (which sets running to false).
    while (running) {

        clearScreen();         // Wipe the old menu.
        drawMenu(selected);    // Draw the menu with the current highlight.

        cin >> input;          // Wait for the player to type a key + ENTER.

        //  Decide what to do with the key.
        // 'W' and 'S' move the highlight. We accept both
        // lowercase and uppercase letters.
        if (input == 'w' || input == 'W') {
            selected--;                 // Move up one option.
            if (selected < 0) {
                selected = 3;           // Wrap around to the bottom.
            }
        }
        else if (input == 's' || input == 'S') {
            selected++;                 // Move down one option.
            if (selected > 3) {
                selected = 0;           // Wrap around to the top.
            }
        }
        else if (input >= '1' && input <= '4') {
            // Characters are stored as numbers, so '1' - '1' = 0,
            // '2' - '1' = 1, and so on. This turns the key into 0-3.
            selected = input - '1';
            input = 'e';                // Pretend they also pressed E.
        }

        //  If the player pressed E (or a number), run the option.
        if (input == 'e' || input == 'E') {

            clearScreen();
            cout << "\n  ============================================\n\n";

            // A switch statement picks ONE block of code to run,
            // depending on the value of "selected".
            switch (selected) {
                case 0:
                    cout << "      Starting New Game...\n";
                    break;      // "break" stops us from falling into the next case.
                case 1:
                    cout << "      Loading Game...\n";
                    break;
                case 2:
                    cout << "      Opening INSTRUCTION...\n";
                    break;
                case 3:
                    cout << "      Goodbye. See you again!\n";
                    running = false;    // This ends the while loop.
                    break;
            }
        }
    }
    
    return 0;
}
