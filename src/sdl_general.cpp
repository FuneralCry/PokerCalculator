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

std::unordered_map<char,std::array<std::array<int,3>,2>> getSuitColors()
{
    std::unordered_map<char,std::array<std::array<int,3>,2>> res;
    res[(char)pkr::CardSuit::club][0][0] = CLUB_FALSE_R;
    res[(char)pkr::CardSuit::club][0][1] = CLUB_FALSE_G;
    res[(char)pkr::CardSuit::club][0][2] = CLUB_FALSE_B;
    res[(char)pkr::CardSuit::club][1][0] = CLUB_TRUE_R;
    res[(char)pkr::CardSuit::club][1][1] = CLUB_TRUE_G;
    res[(char)pkr::CardSuit::club][1][2] = CLUB_TRUE_B;

    res[(char)pkr::CardSuit::spade][0][0] = SPADE_FALSE_R;
    res[(char)pkr::CardSuit::spade][0][1] = SPADE_FALSE_G;
    res[(char)pkr::CardSuit::spade][0][2] = SPADE_FALSE_B;
    res[(char)pkr::CardSuit::spade][1][0] = SPADE_TRUE_R;
    res[(char)pkr::CardSuit::spade][1][1] = SPADE_TRUE_G;
    res[(char)pkr::CardSuit::spade][1][2] = SPADE_TRUE_B;

    res[(char)pkr::CardSuit::diamond][0][0] = DIAMOND_FALSE_R;
    res[(char)pkr::CardSuit::diamond][0][1] = DIAMOND_FALSE_G;
    res[(char)pkr::CardSuit::diamond][0][2] = DIAMOND_FALSE_B;
    res[(char)pkr::CardSuit::diamond][1][0] = DIAMOND_TRUE_R;
    res[(char)pkr::CardSuit::diamond][1][1] = DIAMOND_TRUE_G;
    res[(char)pkr::CardSuit::diamond][1][2] = DIAMOND_TRUE_B;

    res[(char)pkr::CardSuit::heart][0][0] = HEART_FALSE_R;
    res[(char)pkr::CardSuit::heart][0][1] = HEART_FALSE_G;
    res[(char)pkr::CardSuit::heart][0][2] = HEART_FALSE_B;
    res[(char)pkr::CardSuit::heart][1][0] = HEART_TRUE_R;
    res[(char)pkr::CardSuit::heart][1][1] = HEART_TRUE_G;
    res[(char)pkr::CardSuit::heart][1][2] = HEART_TRUE_B;

    return res;
}
