#include "PokerCalculator.h"
#include "sdl_general.h"

void putText(SDL_Renderer* renderer, std::string text, int x, int y, int size)
{
    static TTF_Font* font(TTF_OpenFont(FONT_PATH,size));
    if(font == nullptr)
        std::cout << TTF_GetError() << '\n';
    SDL_Color color = {TEXT_R, TEXT_G, TEXT_B};
    SDL_Surface* surfaceText(TTF_RenderText_Solid(font,text.c_str(),color));
    SDL_Texture* textureText(SDL_CreateTextureFromSurface(renderer,surfaceText));
    SDL_Rect textRect;
    textRect.h = textRect.w = size;
    textRect.x = x;
    textRect.y = y;
    SDL_RenderCopy(renderer,textureText,nullptr,&textRect);
    SDL_FreeSurface(surfaceText);
    SDL_DestroyTexture(textureText);
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
                SDL_SetRenderDrawColor(renderer,suit_colors[s][0][0],suit_colors[s][0][1],suit_colors[s][0][2],1);
            else
                SDL_SetRenderDrawColor(renderer,BLACKED_RGB,BLACKED_RGB,BLACKED_RGB,1);
            SDL_RenderFillRect(renderer,&cell);
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