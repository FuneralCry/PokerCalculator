#include "PokerCalculator.h"

pkr::Card PokerCalculator::pickCard(std::unordered_set<pkr::Card> black_list) const
{
    std::cout << "Pick a card" << '\n';
    std::cout << "Current card:" << '\n';
    SDL_Surface* display;
    if((display = SDL_SetVideoMode(CARDS_MATRIX_CELL_SIZE*13,CARDS_MATRIX_CELL_SIZE*4,32,SDL_HWSURFACE | SDL_DOUBLEBUF)) == NULL)
        throw std::runtime_error("Can't render hand matrix");
    auto suit_colors(this->getSuitColors());
    SDL_Rect cell;
    cell.w = CARDS_MATRIX_CELL_SIZE;
    cell.h = CARDS_MATRIX_CELL_SIZE;
    for(int v(0); v < 13; ++v)
    {
        for(char s(0); s < 4; ++s)
        {
            cell.x = v * cell.w;
            cell.y = s * cell.h;
            pkr::Card c((char)pkr::CardValue::two+v,(char)pkr::CardSuit::club+s);
            Uint32 color;
            if(black_list.find(c) == black_list.end())
                color = SDL_MapRGB(display->format,suit_colors[s][0][0],suit_colors[s][0][1],suit_colors[s][0][2]);
            else
                color = SDL_MapRGB(display->format,BLACKED_RGB,BLACKED_RGB,BLACKED_RGB);
            SDL_FillRect(display,&cell,color);
        }
    }
    SDL_Flip(display);
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