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
    std::unordered_map<char,char> CardSuitsInOut;
    std::string Hand2String_preflop(pkr::HandEq::Hand hand);
    pkr::HandEq::Hand hand_from_xy_preflop(int x, int y);

    std::unordered_set<pkr::Hand> inverseSet_postflop(const std::unordered_set<pkr::Hand>& hands);
// SDL stuff
    class TwoSuitsHandsCell
    {
    private:
        char s1,s2;
        const std::unordered_set<pkr::Hand> black_list;
        std::unordered_map<char,std::array<std::array<int,3>,2>> suit_colors;
        std::array<std::array<bool,13>,13> hand_matrix_choice;

        pkr::Hand hand_from_xy_postflop(int x, int y, int s1, int s2) const;
    public:
        TwoSuitsHandsCell() = delete;
        TwoSuitsHandsCell(char s1, char s2, const std::unordered_set<pkr::Hand>& black_list, const std::unordered_map<char,std::array<std::array<int,3>,2>>& suit_colors);
        void PutCell(int x, int y, bool val);
        void PrintCell(SDL_Surface* screen, int init_x, int init_y);
        std::unordered_set<pkr::Hand> getHands() const;
        std::string getHandName(int x, int y) const;
    };

    void init_sdl();
    std::unordered_set<pkr::HandEq::Hand,pkr::HandEq::hsh> render_matrix_and_get_hands_preflop(std::unordered_set<pkr::HandEq::Hand,pkr::HandEq::hsh> black_list);
    std::unordered_set<pkr::Hand> render_matrix_and_get_hands_postflop(std::unordered_set<pkr::Hand> black_list);
    std::unordered_map<char,std::array<std::array<int,3>,2>> getSuitColors() const;
    void render_ev_matrix(pkr::Game_equity_postflop* game);
public:
    PokerCalculator();
// UI
    void start();
    void calcEquity();
    void processAction(pkr::Game_equity_postflop* game, int player_num);
    std::vector<pkr::Card> fillBoard();
    void calcOdds();
};

#endif