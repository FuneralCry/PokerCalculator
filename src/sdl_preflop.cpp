#include "PokerCalculator.h"


void PokerCalculator::init_sdl()
{
    if(SDL_Init(SDL_INIT_EVERYTHING) < 0)
        throw std::runtime_error("SDL can't be initialized");
}

pkr::HandEq::Hand PokerCalculator::hand_from_xy_preflop(int x, int y)
{
    char v1((char)pkr::CardValue::ace-y),v2((char)pkr::CardValue::ace-x);
    pkr::HandEq::Hand h(std::make_pair(std::make_pair(v1,v2),v1>v2));

    return h;
}

pkr::Hand PokerCalculator::TwoSuitsHandsCell::hand_from_xy_postflop(int x, int y, int s1, int s2) const
{
    char v1((char)pkr::CardValue::ace-y),v2((char)pkr::CardValue::ace-x);

    return std::make_pair(pkr::Card(v1,s1),pkr::Card(v2,s2));
}

void PokerCalculator::render_ev_matrix(pkr::Game_equity_postflop* game)
{
    SDL_Surface* display;
    if((display = SDL_SetVideoMode(HANDS_MATRIX_CELL_SIZE_PREFLOP*13,HANDS_MATRIX_CELL_SIZE_PREFLOP*13,32,SDL_HWSURFACE | SDL_DOUBLEBUF)) == NULL)
        throw std::runtime_error("Can't render hand matrix");
    std::array<std::array<double,13>,13> ev_matrix;
    double max_ev(-99999999),min_ev(99999999);
    for(int i(0); i < 13; ++i)
    {
        for(int j(0); j < 13; ++j)
        {
            auto hh(this->hand_from_xy_preflop(j,i));
            char s1((char)pkr::CardSuit::club);
            char s2;
            if(hh.value.second)
                s2 = (char)pkr::CardSuit::club;
            else
                s2 = (char)pkr::CardSuit::diamond;
            pkr::Hand h(std::make_pair(pkr::Card(hh.value.first.first,s1),pkr::Card(hh.value.first.second,s2)));
            ev_matrix[i][j] = game->getEquity(&h);
            max_ev = std::max(ev_matrix[i][j],max_ev);
            min_ev = std::min(ev_matrix[i][j],min_ev);
        }
    }
    for(int i(0); i < 13; ++i)
    {
        for(int j(0); j < 13; ++j)
        {
            double ev(ev_matrix[i][j]);
            Uint32 color;
            SDL_Rect cell;
            cell.h = cell.w = HANDS_MATRIX_CELL_SIZE_PREFLOP;
            cell.x = j * cell.w;
            cell.y = i * cell.h;
            if(ev < 0)
            {
                int r(255);
                int g(255-std::abs(ev/min_ev)*255);
                int b(g);
                color = SDL_MapRGB(display->format,r,g,b);
            }
            else
            {
                int g(255);
                int r(255-std::abs(ev/max_ev)*255);
                int b(r);
                color = SDL_MapRGB(display->format,r,g,b);
            }
            SDL_FillRect(display,&cell,color);
        }
    }
    SDL_Flip(display);
    SDL_Event event;
    bool observing(true);
    while(observing)
    {
        SDL_PollEvent(&event);
        switch(event.type)
        {
            case SDL_KEYDOWN:
                if(event.key.keysym.sym == SDLK_RETURN)
                    observing = false;
                break;
            case SDL_MOUSEMOTION:
                int x(event.button.x / HANDS_MATRIX_CELL_SIZE_PREFLOP),y(event.button.y / HANDS_MATRIX_CELL_SIZE_PREFLOP);
                pkr::HandEq::Hand h(this->hand_from_xy_preflop(x,y));
                std::cout << '\r' << this->Hand2String_preflop(h) << " -> " << ev_matrix[y][x] << "       ";
                std::flush(std::cout);
                break;
        }
    }
    std::cout << '\n';
}

std::unordered_set<pkr::HandEq::Hand,pkr::HandEq::hsh> PokerCalculator::render_matrix_and_get_hands_preflop(std::unordered_set<pkr::HandEq::Hand,pkr::HandEq::hsh> black_list)
{
    std::cout << "Enter range." << '\n';
    std::cout << "Current hand:" << '\n';
    SDL_Surface* display;
    if((display = SDL_SetVideoMode(HANDS_MATRIX_CELL_SIZE_PREFLOP*13,HANDS_MATRIX_CELL_SIZE_PREFLOP*13,32,SDL_HWSURFACE | SDL_DOUBLEBUF)) == NULL)
        throw std::runtime_error("Can't render hand matrix");
    std::array<std::array<bool,13>,13> hand_matrix_choice;
    for(int i(0); i < 13; ++i)
        for(int j(0); j < 13; ++j)
            hand_matrix_choice[i][j] = false;
    Uint32 suited_true = SDL_MapRGB(display->format,SUIT_TRUE_R,SUIT_TRUE_G,SUIT_TRUE_B);
    Uint32 suited_false = SDL_MapRGB(display->format,SUIT_FALSE_R,SUIT_FALSE_G,SUIT_FALSE_B);
    Uint32 notsuit_true = SDL_MapRGB(display->format,NSUIT_TRUE_R,NSUIT_TRUE_G,NSUIT_TRUE_B);
    Uint32 notsuit_false = SDL_MapRGB(display->format,NSUIT_FALSE_R,NSUIT_FALSE_G,NSUIT_FALSE_B);
    Uint32 blacked = SDL_MapRGB(display->format,BLACKED_RGB,BLACKED_RGB,BLACKED_RGB);
    bool checking(true);
    while(checking)
    {
        SDL_Event event;
        while(SDL_PollEvent(&event))
        {
            switch(event.type)
            {
                case SDL_MOUSEMOTION:
                {
                    int x(event.motion.x / HANDS_MATRIX_CELL_SIZE_PREFLOP),y(event.motion.y / HANDS_MATRIX_CELL_SIZE_PREFLOP);
                    pkr::HandEq::Hand h(this->hand_from_xy_preflop(x,y));
                    std::cout << '\r' << this->Hand2String_preflop(h) << "  ";
                    if(black_list.find(h) == black_list.end())
                    {
                        if(event.motion.state == SDL_BUTTON_LMASK)
                            hand_matrix_choice[y][x] = true;
                        else if(event.motion.state == SDL_BUTTON_RMASK)
                            hand_matrix_choice[y][x] = false;
                    }
                    break;
                }
                case SDL_MOUSEBUTTONDOWN:
                {
                    int x(event.button.x / HANDS_MATRIX_CELL_SIZE_PREFLOP),y(event.button.y / HANDS_MATRIX_CELL_SIZE_PREFLOP);
                    pkr::HandEq::Hand h(this->hand_from_xy_preflop(x,y));
                    std::cout << '\r' << this->Hand2String_preflop(h) << "  ";
                    if(black_list.find(h) == black_list.end())
                    {
                        if(event.button.button == SDL_BUTTON_LEFT)
                            hand_matrix_choice[y][x] = true;
                        else if(event.motion.state == SDL_BUTTON_RIGHT)
                            hand_matrix_choice[y][x] = false;
                    }
                    break;
                }
                case SDL_KEYDOWN:
                    if(event.key.keysym.sym == SDLK_RETURN)
                        checking = false;
                    break;
            }
            std::flush(std::cout);
        }
        SDL_Rect cell;
        cell.h = cell.w = HANDS_MATRIX_CELL_SIZE_PREFLOP;
        for(int i(0); i < 13; ++i)
        {
            for(int j(0); j < 13; ++j)
            {
                pkr::HandEq::Hand h(this->hand_from_xy_preflop(j,i));
                Uint32 color;
                if(black_list.find(h) != black_list.end())
                    color = blacked;
                else
                {
                    if(hand_matrix_choice[i][j])
                    {
                        if(h.value.second)
                            color = suited_true;
                        else
                            color = notsuit_true;
                    }
                    else
                    {
                        if(h.value.second)
                            color = suited_false;
                        else
                            color = notsuit_false;
                    }
                }
                
                cell.x = j * cell.w;
                cell.y = i * cell.h;
                SDL_FillRect(display,&cell,color);
            }
        }
        SDL_Flip(display);
        usleep(100);
    }
    std::unordered_set<pkr::HandEq::Hand,pkr::HandEq::hsh> res;
    for(int i(0); i < 13; ++i)
        for(int j(0); j < 13; ++j)
        {
            pkr::HandEq::Hand h(this->hand_from_xy_preflop(j,i));
            if(black_list.find(h) == black_list.end() and hand_matrix_choice[i][j])
                res.insert(h);
        }
    SDL_Quit();
    std::cout << '\n';
    
    return res;
}