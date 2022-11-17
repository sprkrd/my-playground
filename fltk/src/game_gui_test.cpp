#include <game_gui.hpp>

int main(int argc, char* argv[]) {
    GameGUI gui(true);
    //gui.ResetBoardStatus({"1", "2", "3", "4", "5", "6", "7", "8", "9", "10", "", "", "", "", "", "", "", "", "", ""});
    gui.ResetTokensOutOfBoard({"282", "Hey", "Yai"});
    gui.Run(argc, argv);
}
