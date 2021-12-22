#include "PokerCalculator.h"


void PokerCalculator::init_sdl()
{
    if(SDL_Init(SDL_INIT_EVERYTHING) < 0)
        throw std::runtime_error("SDL can't be initialized");
}

pkr::HandEq::Hand PokerCalculator::hand_from_xy(int x, int y)
{
    char c1((char)pkr::CardValue::ace-y),c2((char)pkr::CardValue::ace-x);
    pkr::HandEq::Hand h(std::make_pair(std::make_pair(c1,c2),c1>c2));

    return h;
}

void PokerCalculator::render_ev_matrix(const pkr::Game_equity& game)
{
    SDL_Surface* display;
    if((display = SDL_SetVideoMode(HANDS_MATRIX_WIDTH,HANDS_MATRIX_HEIGHT,32,SDL_HWSURFACE | SDL_DOUBLEBUF)) == NULL)
        throw std::runtime_error("Can't render hand matrix");
    std::array<std::array<double,13>,13> ev_matrix;
    double max_ev(-99999999),min_ev(99999999);
    for(int i(0); i < 13; ++i)
    {
        for(int j(0); j < 13; ++j)
        {
            auto hh(this->hand_from_xy(j,i));
            char s1((char)pkr::CardSuit::club);
            char s2;
            if(hh.value.second)
                s2 = (char)pkr::CardSuit::club;
            else
                s2 = (char)pkr::CardSuit::diamond;
            pkr::Hand h(std::make_pair(pkr::Card(hh.value.first.first,s1),pkr::Card(hh.value.first.second,s2)));
            ev_matrix[i][j] = game.getEquity(&h);
            max_ev = std::max(ev_matrix[i][j],max_ev);
            min_ev = std::min(ev_matrix[i][j],min_ev);
        }
    }
    int cell_width(HANDS_MATRIX_WIDTH/13),cell_height(HANDS_MATRIX_HEIGHT/13);
    for(int i(0); i < 13; ++i)
    {
        for(int j(0); j < 13; ++j)
        {
            double ev(ev_matrix[i][j]);
            Uint32 color;
            SDL_Rect cell;
            cell.h = cell_height;
            cell.w = cell_width;
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
                int x(event.button.x / cell_width),y(event.button.y / cell_height);
                pkr::HandEq::Hand h(this->hand_from_xy(x,y));
                std::cout << '\r' << this->Hand2String(h) << " -> " << ev_matrix[y][x] << "       ";
                std::flush(std::cout);
                break;
        }
    }
    std::cout << '\n';
}

std::unordered_set<pkr::HandEq::Hand,pkr::HandEq::hsh> PokerCalculator::render_matrix_and_get_hands(const std::unordered_set<pkr::HandEq::Hand,pkr::HandEq::hsh>& black_list)
{
    std::cout << "Enter range." << '\n';
    std::cout << "Current hand:" << '\n';
    SDL_Surface* display;
    if((display = SDL_SetVideoMode(HANDS_MATRIX_WIDTH,HANDS_MATRIX_HEIGHT,32,SDL_HWSURFACE | SDL_DOUBLEBUF)) == NULL)
        throw std::runtime_error("Can't render hand matrix");
    std::array<std::array<bool,13>,13> hand_matrix_choice;
    for(int i(0); i < 13; ++i)
        for(int j(0); j < 13; ++j)
            hand_matrix_choice[i][j] = false;
    Uint32 suited_true = SDL_MapRGB(display->format,255,66,66);
    Uint32 suited_false = SDL_MapRGB(display->format,255,145,145);
    Uint32 notsuit_true = SDL_MapRGB(display->format,66,66,255);
    Uint32 notsuit_false = SDL_MapRGB(display->format,145,145,255);
    Uint32 blacked = SDL_MapRGB(display->format,178,178,178);
    int cell_width(HANDS_MATRIX_WIDTH/13),cell_height(HANDS_MATRIX_HEIGHT/13);
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
                    int x(event.motion.x / cell_width),y(event.motion.y / cell_height);
                    pkr::HandEq::Hand h(this->hand_from_xy(x,y));
                    std::cout << '\r' << this->Hand2String(h) << "  ";
                    std::flush(std::cout);
                    if(black_list.find(h) == black_list.end())
                    {
                        if(event.motion.state == SDL_BUTTON_LMASK)
                        {
                            hand_matrix_choice[y][x] = true;
                        }
                        else if(event.motion.state == SDL_BUTTON_RMASK)
                        {
                            hand_matrix_choice[y][x] = false;
                        }
                    }
                    break;
                }
                case SDL_MOUSEBUTTONDOWN:
                {
                    int x(event.button.x / cell_width),y(event.button.y / cell_height);
                    pkr::HandEq::Hand h(this->hand_from_xy(x,y));
                    std::cout << '\r' << this->Hand2String(h) << "  ";
                    std::flush(std::cout);
                    if(black_list.find(h) == black_list.end())
                    {
                        if(event.button.button == SDL_BUTTON_LEFT)
                        {
                            hand_matrix_choice[y][x] = true;
                        }
                        else if(event.motion.state == SDL_BUTTON_RIGHT)
                        {
                            hand_matrix_choice[y][x] = false;
                        }
                    }
                    break;
                }
                case SDL_KEYDOWN:
                    if(event.key.keysym.sym == SDLK_RETURN)
                        checking = false;
                    break;
            }
        }
        SDL_Rect cell;
        cell.h = cell_height;
        cell.w = cell_width;
        for(int i(0); i < 13; ++i)
        {
            for(int j(0); j < 13; ++j)
            {
                pkr::HandEq::Hand h(this->hand_from_xy(j,i));
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
        usleep(10);
    }
    std::unordered_set<pkr::HandEq::Hand,pkr::HandEq::hsh> res;
    for(int i(0); i < 13; ++i)
        for(int j(0); j < 13; ++j)
        {
            pkr::HandEq::Hand h(this->hand_from_xy(j,i));
            if(black_list.find(h) == black_list.end() and hand_matrix_choice[i][j])
                res.insert(h);
        }
    SDL_Quit();
    std::cout << '\n';
    
    return res;
}