#include "PokerCalculator.h"
#include "sdl_general.h"

std::unordered_set<pkr::Card> PokerCalculator::Cell_card::black_list{};

PokerCalculator::Cell_card::Cell_card(pkr::Card c, int size) : card(c), size(size) {}

void PokerCalculator::Cell_card::PrintCell(SDL_Renderer* renderer)
{
    auto suit_colors(getSuitColors());
    SDL_Rect cell;
    cell.w = CARDS_MATRIX_CELL_SIZE;
    cell.h = CARDS_MATRIX_CELL_SIZE;
    cell.x = (this->card.getValue() - (char)pkr::CardValue::two) * cell.w;
    cell.y = (this->card.getSuit()) * cell.h;
    SDL_SetRenderDrawColor(renderer,CELL_FRAME_R,CELL_FRAME_G,CELL_FRAME_B,1);
    SDL_RenderFillRect(renderer,&cell);
    cell.x += FRAME_WIDTH_PREFLOP;
    cell.y += FRAME_WIDTH_PREFLOP;
    cell.w -= FRAME_WIDTH_PREFLOP;
    cell.h -= FRAME_WIDTH_PREFLOP;
    char s(this->card.getSuit());
    if(black_list.find(this->card) == black_list.end())
        SDL_SetRenderDrawColor(renderer,suit_colors[s][0][0],suit_colors[s][0][1],suit_colors[s][0][2],1);
    else
        SDL_SetRenderDrawColor(renderer,BLACKED_RGB,BLACKED_RGB,BLACKED_RGB,1);
    SDL_RenderFillRect(renderer,&cell);
    putText(renderer,(std::string)this->card,cell.x,cell.y,this->size);
}

pkr::Card PokerCalculator::pickCard(std::unordered_set<pkr::Card> black_list) const
{
    std::cout << "Pick a card" << '\n';
    std::cout << "Current card:" << '\n';
    SDL_Window* window;
    SDL_Renderer* renderer;
    if((window = SDL_CreateWindow("Preflop hands",SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,HANDS_MATRIX_CELL_SIZE_PREFLOP*13,HANDS_MATRIX_CELL_SIZE_PREFLOP*4,SDL_WINDOW_SHOWN)) == nullptr)
        throw std::runtime_error("Can't render hand matrix");
    if((renderer = SDL_CreateRenderer(window,-1,SDL_RENDERER_ACCELERATED)) == nullptr)
        throw std::runtime_error("Can't render hand matrix");
    Cell_card::black_list = black_list;
    for(int v(0); v < 13; ++v)
    {
        for(char s(0); s < 4; ++s)
        {
            pkr::Card c((char)pkr::CardValue::two+v,(char)pkr::CardSuit::club+s);
            Cell_card cell(c,CARDS_MATRIX_CELL_SIZE);
            cell.PrintCell(renderer);
        }
    }
    SDL_RenderPresent(renderer);
    while(true)
    {
        SDL_Event event;
        while(SDL_PollEvent(&event))
        {
            switch(event.type)
            {
                case SDL_MOUSEBUTTONDOWN:
                {
                    int x(event.button.x / CARDS_MATRIX_CELL_SIZE),y(event.button.y / CARDS_MATRIX_CELL_SIZE);
                    pkr::Card c((char)pkr::CardValue::two+x,(char)pkr::CardSuit::club+y);
                    if(black_list.find(c) == black_list.end())
                    {
                        std::cout << '\n';
                        SDL_Quit();
                        return c;
                    }
                }
                case SDL_MOUSEMOTION:
                {
                    int x(event.button.x / CARDS_MATRIX_CELL_SIZE),y(event.button.y / CARDS_MATRIX_CELL_SIZE);
                    pkr::Card c((char)pkr::CardValue::two+x,(char)pkr::CardSuit::club+y);
                    std::cout << '\r' << (std::string)c  << "  ";
                    std::flush(std::cout);
                }
            }
        }
    }
}