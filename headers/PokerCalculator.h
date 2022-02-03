#ifndef PC_MAIN
#define PC_MAIN

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>
#include <stdexcept>
#include <array>
#include <unistd.h>
#include <iomanip>
#include "poker.h"
#include "ui_vars.h"
#include "sdl_general.h"

class PokerCalculator
{
private:
    bool running;
    std::unordered_map<std::string,char> CardValuesInOut;
    std::unordered_map<char,char> CardSuitsInOut;
    std::string Hand2String_preflop(pkr::HandEq::Hand hand);
    pkr::HandEq::Hand hand_from_xy_preflop(int x, int y);
    std::unordered_set<pkr::Card> getCardsFromHands(std::unordered_set<pkr::Hand> hands);
    std::unordered_set<pkr::Hand> getHandsFromCards(const std::unordered_set<pkr::Card>& cards);
    std::unordered_set<pkr::Hand> mergeSets(std::unordered_set<pkr::Hand> a, std::unordered_set<pkr::Hand>& dest);
    std::unordered_set<pkr::Hand> inverseSet_postflop(const std::unordered_set<pkr::Hand>& hands);
// SDL stuff
    class Cell_preflop
    {
    private:
        pkr::HandEq::Hand hand;
        int size;
        TTF_Font* font;
    public:
        bool active{false};
        static std::unordered_set<pkr::HandEq::Hand,pkr::HandEq::hsh> black_list;

        Cell_preflop() = delete;
        Cell_preflop(const pkr::HandEq::Hand& hand, int size);
        void PrintCell(SDL_Renderer* renderer, int x, int y, int r=-1, int g=-1, int b=-1);
    };

    class Cell_postflop
    {
    private:
        char s1,s2;
        std::array<std::array<bool,13>,13> hand_matrix_choice;

        pkr::Hand hand_from_xy_postflop(int x, int y, int s1, int s2) const;
    public:
        static int cards_counter;
        static std::unordered_set<pkr::Hand> black_list;
        static std::unordered_map<char,std::array<std::array<int,3>,2>> suit_colors;

        Cell_postflop() = delete;
        Cell_postflop(char s1, char s2);
        void PutCell(int x, int y, bool val);
        void PrintCell(SDL_Renderer* renderer, int init_x, int init_y);
        std::unordered_set<pkr::Hand> getHands() const;
        std::string getHandName(int x, int y) const;
        int getCardsNum() const;
    };

    void init_sdl();
    std::unordered_set<pkr::HandEq::Hand,pkr::HandEq::hsh> render_matrix_and_get_hands_preflop(std::unordered_set<pkr::HandEq::Hand,pkr::HandEq::hsh> black_list);
    std::unordered_set<pkr::Hand> render_matrix_and_get_hands_postflop(std::unordered_set<pkr::Hand> black_list, int max_cards=-1);
    std::unordered_map<char,std::array<std::array<int,3>,2>> getSuitColors() const;
    void render_ev_matrix(pkr::Game_equity_postflop* game);
    pkr::Card pickCard(std::unordered_set<pkr::Card> black_list) const;
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