#include "PokerCalculator.h"

std::unordered_set<pkr::HandEq::Hand,pkr::HandEq::hsh> PokerCalculator::Cell_preflop::black_list;

void PokerCalculator::init_sdl()
{
    if(SDL_Init(SDL_INIT_EVERYTHING) != 0)
        throw std::runtime_error("SDL can't be initialized");
    if(TTF_Init() != 0)
        throw std::runtime_error("SDL_ttf can't be initialized.");
}

pkr::HandEq::Hand PokerCalculator::hand_from_xy_preflop(int x, int y)
{
    char v1((char)pkr::CardValue::ace-y),v2((char)pkr::CardValue::ace-x);
    pkr::HandEq::Hand h(std::make_pair(std::make_pair(v1,v2),v1>v2));

    return h;
}

PokerCalculator::Cell_preflop::Cell_preflop(const pkr::HandEq::Hand& hand, int size) : hand(hand), size(size) {}

void PokerCalculator::Cell_preflop::PrintCell(SDL_Renderer* renderer, int x, int y, int r, int g, int b)
{
    SDL_Rect cell;
    cell.x = x;
    cell.y = y;
    cell.w = cell.h = this->size;
    SDL_SetRenderDrawColor(renderer,CELL_FRAME_R,CELL_FRAME_G,CELL_FRAME_B,1);
    SDL_RenderFillRect(renderer,&cell);
    cell.x += FRAME_WIDTH_PREFLOP;
    cell.y += FRAME_WIDTH_PREFLOP;
    cell.w -= FRAME_WIDTH_PREFLOP;
    cell.h -= FRAME_WIDTH_PREFLOP;
    if(r < 0 and (g > 0 or b > 0))
        throw std::invalid_argument("Bad RGB");
    if(r < 0)
    {
        if(black_list.find(hand) != black_list.end())
            SDL_SetRenderDrawColor(renderer,BLACKED_RGB,BLACKED_RGB,BLACKED_RGB,1);
        else
        {
            if(this->active)
            {
                if(hand.value.second)
                    SDL_SetRenderDrawColor(renderer,SUIT_TRUE_R,SUIT_TRUE_G,SUIT_TRUE_B,1);
                else
                    SDL_SetRenderDrawColor(renderer,NSUIT_TRUE_R,NSUIT_TRUE_G,NSUIT_TRUE_B,1);
            }
            else
            {
                if(hand.value.second)
                    SDL_SetRenderDrawColor(renderer,SUIT_FALSE_R,SUIT_FALSE_G,SUIT_FALSE_B,1);
                else
                    SDL_SetRenderDrawColor(renderer,NSUIT_FALSE_R,NSUIT_FALSE_G,NSUIT_FALSE_B,1);
            }
        }
    }
    else
        SDL_SetRenderDrawColor(renderer,r,g,b,1);
    SDL_RenderFillRect(renderer,&cell);
    std::string handText(pkr::CardValuesOut[this->hand.value.first.first] + pkr::CardValuesOut[this->hand.value.first.second]);
    handText += this->hand.value.second ? 's' : 'n';
    putText(renderer,handText,cell.x,cell.y,this->size);
}

void PokerCalculator::render_ev_matrix(pkr::Game_equity_postflop* game)
{
    SDL_Window* window;
    SDL_Renderer* renderer;
    if((window = SDL_CreateWindow("Preflop hands",SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,HANDS_MATRIX_CELL_SIZE_PREFLOP*13,HANDS_MATRIX_CELL_SIZE_PREFLOP*13,SDL_WINDOW_SHOWN)) == nullptr)
        throw std::runtime_error("Can't render hand matrix");
    if((renderer = SDL_CreateRenderer(window,-1,SDL_RENDERER_ACCELERATED)) == nullptr)
        throw std::runtime_error("Can't render hand matrix");
    std::array<std::array<double,13>,13> ev_matrix;
    std::array<std::array<Cell_preflop*,13>,13> hand_matrix;
    for(int i(0); i < 13; ++i)
        for(int j(0); j < 13; ++j)
            hand_matrix[i][j] = new Cell_preflop(hand_from_xy_preflop(j,i),HANDS_MATRIX_CELL_SIZE_PREFLOP);
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
            int r,g,b;
            if(ev < 0)
            {
                r = 255;
                g = 255-std::abs(ev/min_ev)*255;
                b = g;
            }
            else
            {
                g = 255;
                r = 255-std::abs(ev/max_ev)*255;
                b = r;
            }
            hand_matrix[i][j]->PrintCell(renderer,j * HANDS_MATRIX_CELL_SIZE_PREFLOP,i * HANDS_MATRIX_CELL_SIZE_PREFLOP,r,g,b);
        }
    }
    SDL_RenderPresent(renderer);
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
    SDL_Window* window;
    SDL_Renderer* renderer;
    if((window = SDL_CreateWindow("Preflop hands",SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,HANDS_MATRIX_CELL_SIZE_PREFLOP*13,HANDS_MATRIX_CELL_SIZE_PREFLOP*13,SDL_WINDOW_SHOWN)) == nullptr)
        throw std::runtime_error("Can't render hand matrix");
    if((renderer = SDL_CreateRenderer(window,-1,SDL_RENDERER_ACCELERATED)) == nullptr)
        throw std::runtime_error("Can't render hand matrix");
    std::array<std::array<Cell_preflop*,13>,13> hand_matrix_choice;
    PokerCalculator::Cell_preflop::black_list = black_list;
    for(int i(0); i < 13; ++i)
        for(int j(0); j < 13; ++j)
            hand_matrix_choice[i][j] = new Cell_preflop(hand_from_xy_preflop(j,i),HANDS_MATRIX_CELL_SIZE_PREFLOP);
    bool checking(true);
    SDL_Event event;
    while(SDL_PollEvent(&event)) { }
    while(checking)
    {
        while(SDL_PollEvent(&event))
        {
            switch(event.type)
            {
                case SDL_MOUSEMOTION:
                {
                    int x(event.motion.x / HANDS_MATRIX_CELL_SIZE_PREFLOP),y(event.motion.y / HANDS_MATRIX_CELL_SIZE_PREFLOP);
                    auto h(hand_from_xy_preflop(x,y));
                    if(black_list.find(h) == black_list.end())
                    {
                        if(event.motion.state == SDL_BUTTON_LMASK)
                            hand_matrix_choice[y][x]->active = true;
                        else if(event.motion.state == SDL_BUTTON_RMASK)
                            hand_matrix_choice[y][x]->active = false;
                    }
                    break;
                }
                case SDL_MOUSEBUTTONDOWN:
                {
                    int x(event.button.x / HANDS_MATRIX_CELL_SIZE_PREFLOP),y(event.button.y / HANDS_MATRIX_CELL_SIZE_PREFLOP);
                    auto h(hand_from_xy_preflop(x,y));
                    if(black_list.find(h) == black_list.end())
                    {
                        if(event.button.button == SDL_BUTTON_LEFT)
                            hand_matrix_choice[y][x]->active = true;
                        else if(event.button.button == SDL_BUTTON_RIGHT)
                            hand_matrix_choice[y][x]->active = false;
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
                hand_matrix_choice[i][j]->PrintCell(renderer,j * HANDS_MATRIX_CELL_SIZE_PREFLOP,i * HANDS_MATRIX_CELL_SIZE_PREFLOP);
            }
        }
        SDL_RenderPresent(renderer);
        SDL_Delay(10);
    }
    std::unordered_set<pkr::HandEq::Hand,pkr::HandEq::hsh> res;
    for(int i(0); i < 13; ++i)
        for(int j(0); j < 13; ++j)
        {
            pkr::HandEq::Hand h(this->hand_from_xy_preflop(j,i));
            if(black_list.find(h) == black_list.end() and hand_matrix_choice[i][j]->active)
                res.insert(h);
        }
    SDL_Quit();
    std::cout << '\n';
    
    return res;
}