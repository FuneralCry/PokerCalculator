#ifndef PC_MAIN
#define PC_MAIN

#include <SDL/SDL.h>
#include <iostream>
#include <stdexcept>
#include <array>
#include <unistd.h>
#include <iomanip>
#include "poker.h"
#include "ui_vars.h"

class PokerCalculator
{
private:
    bool running;
    std::unordered_map<std::string,char> CardValuesInOut;
    std::string Hand2String(pkr::HandEq::Hand hand);
    pkr::HandEq::Hand hand_from_xy(int x, int y);
// SDL stuff
    void init_sdl();
    std::unordered_set<pkr::HandEq::Hand,pkr::HandEq::hsh> render_matrix_and_get_hands(const std::unordered_set<pkr::HandEq::Hand,pkr::HandEq::hsh>& black_list);
    void render_ev_matrix(const pkr::Game_equity& game);
public:
    PokerCalculator();
// UI
    void start();
    void calcEquity();
    void processAction(pkr::Game_equity& game, int player_num);
    void calcOdds();
};

#endif