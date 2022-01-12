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

PokerCalculator::TwoSuitsHandsCell::TwoSuitsHandsCell(char s1, char s2, const std::unordered_set<pkr::Hand>& black_list, const std::unordered_map<char,std::array<std::array<int,3>,2>>& suit_colors) : s1(s1), s2(s2), black_list(black_list), suit_colors(suit_colors)
{
    for(auto& a : this->hand_matrix_choice)
        for(auto& b : a)
            b = false;
}

void PokerCalculator::TwoSuitsHandsCell::PutCell(int x, int y, bool val)
{
    if(x < FRAME_WIDTH_POSTFLOP or y < FRAME_WIDTH_POSTFLOP)
        return;
    x -= FRAME_WIDTH_POSTFLOP;
    y -= FRAME_WIDTH_POSTFLOP;
    x /= HANDS_MATRIX_CELL_SIZE_POSTFLOP;
    y /= HANDS_MATRIX_CELL_SIZE_POSTFLOP;
    this->hand_matrix_choice[y][x] = val;
    if(s1 == s2)
        this->hand_matrix_choice[x][y] = val;
}

void PokerCalculator::TwoSuitsHandsCell::PrintCell(SDL_Surface* screen, int init_x, int init_y)
{
    SDL_Rect first_frame,second_frame;
    first_frame.x = FRAME_WIDTH_POSTFLOP + init_x;
    first_frame.y = init_y+FRAME_WIDTH_POSTFLOP/3;
    first_frame.h = FRAME_WIDTH_POSTFLOP/3;
    first_frame.w = HANDS_MATRIX_CELL_SIZE_POSTFLOP*13;
    second_frame.x = init_x+FRAME_WIDTH_POSTFLOP/3;
    second_frame.y = FRAME_WIDTH_POSTFLOP + init_y;
    second_frame.w = FRAME_WIDTH_POSTFLOP/3;
    second_frame.h = HANDS_MATRIX_CELL_SIZE_POSTFLOP*13;
    SDL_FillRect(screen,&first_frame,SDL_MapRGB(screen->format,suit_colors[s1][1][0],suit_colors[s1][1][1],suit_colors[s1][1][2]));
    SDL_FillRect(screen,&second_frame,SDL_MapRGB(screen->format,suit_colors[s2][1][0],suit_colors[s2][1][1],suit_colors[s2][1][2]));
    init_x += FRAME_WIDTH_POSTFLOP;
    init_y += FRAME_WIDTH_POSTFLOP;
    SDL_Rect cell;
    cell.w = cell.h = HANDS_MATRIX_CELL_SIZE_POSTFLOP;
    for(int i(0); i < 13; ++i)
    {
        for(int j(0); j < 13; ++j)
        {
            pkr::Hand h(hand_from_xy_postflop(j,i,s1,s2));
            Uint32 color;
            if(black_list.find(h) != black_list.end() or (s1 == s2 and i == j))
                color = SDL_MapRGB(screen->format,BLACKED_RGB,BLACKED_RGB,BLACKED_RGB);
            else
            {
                char s_color = i > j ? s1 : s2;
                int active = hand_matrix_choice[i][j] ? 1 : 0;
                color = SDL_MapRGB(screen->format,suit_colors[s_color][active][0],suit_colors[s_color][active][1],suit_colors[s_color][active][2]);
            }
            cell.x = j * cell.w + init_x;
            cell.y = i * cell.h+ + init_y;
            SDL_FillRect(screen,&cell,color);
        }
    }
}

std::unordered_set<pkr::Hand> PokerCalculator::TwoSuitsHandsCell::getHands() const
{
    std::unordered_set<pkr::Hand> res;
    for(int i(0); i < 13; ++i)
    {
        for(int j(0); j < 13; ++j)
        {
            pkr::Hand h(hand_from_xy_postflop(j,i,s1,s2));
            if(i == j and s1 == s2)
                continue;
            if(hand_matrix_choice[i][j])
                res.insert(h);
        }
    }

    return res;
}

std::string PokerCalculator::TwoSuitsHandsCell::getHandName(int x, int y) const
{
    if(x < FRAME_WIDTH_POSTFLOP or y < FRAME_WIDTH_POSTFLOP)
        return "None";
    x -= FRAME_WIDTH_POSTFLOP;
    y -= FRAME_WIDTH_POSTFLOP;
    x /= HANDS_MATRIX_CELL_SIZE_POSTFLOP;
    y /= HANDS_MATRIX_CELL_SIZE_POSTFLOP;

    return (std::string)pkr::Card((char)pkr::CardValue::ace-x,s1) + (std::string)pkr::Card((char)pkr::CardValue::ace-y,s2);
}

std::unordered_map<char,std::array<std::array<int,3>,2>> PokerCalculator::getSuitColors() const
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

std::unordered_set<pkr::Hand> mergeSets(std::unordered_set<pkr::Hand> a, std::unordered_set<pkr::Hand>& dest)
{
    for(const pkr::Hand& h : a)
        dest.insert(h);
    
    return dest;
}

std::unordered_set<pkr::Hand> PokerCalculator::render_matrix_and_get_hands_postflop(std::unordered_set<pkr::Hand> black_list)
{
    std::cout << "Enter range." << '\n';
    std::cout << "Current hand:" << '\n';
    SDL_Surface* display;
    int one_cell_size(HANDS_MATRIX_CELL_SIZE_POSTFLOP*13+FRAME_WIDTH_POSTFLOP);
    if((display = SDL_SetVideoMode(one_cell_size*4,one_cell_size*4,32,SDL_HWSURFACE | SDL_DOUBLEBUF)) == NULL)
        throw std::runtime_error("Can't render hand matrix");
    std::array<std::array<TwoSuitsHandsCell*,4>,4> hand_matrix;
    std::unordered_map<char,std::array<std::array<int,3>,2>> suit_colors(getSuitColors());
    for(char s1((char)pkr::CardSuit::club); s1 <= (char)pkr::CardSuit::spade; ++s1)
    {
        for(char s2((char)pkr::CardSuit::club); s2 <= (char)pkr::CardSuit::spade; ++s2)
            hand_matrix[s1][s2] = new TwoSuitsHandsCell(s2,s1,black_list,suit_colors);
    }

    bool checking(true);
    bool comfort_mode(false);
    while(checking)
    {
        SDL_Event event;
        while(SDL_PollEvent(&event))
        {
            switch(event.type)
            {
                case SDL_MOUSEMOTION:
                {
                    int x(event.motion.x % one_cell_size),y(event.motion.y % one_cell_size);
                    int x_cell(event.motion.x / one_cell_size),y_cell(event.motion.y / one_cell_size);
                    bool suited(x_cell == y_cell);
                    std::cout << '\r' << hand_matrix[y_cell][x_cell]->getHandName(x,y) << "   ";
                    if(comfort_mode)
                    {
                        for(int x_cell(0); x_cell < 4; ++x_cell)
                            for(int y_cell(0); y_cell < 4; ++y_cell)
                            {
                                bool equal_coord(x_cell == y_cell);
                                if(equal_coord == suited)
                                {
                                    if(event.motion.state == SDL_BUTTON_LMASK)
                                        hand_matrix[y_cell][x_cell]->PutCell(x,y,true);
                                    if(event.motion.state == SDL_BUTTON_RMASK)
                                        hand_matrix[y_cell][x_cell]->PutCell(x,y,false);
                                }
                            }
                    }
                    else
                    {
                        if(event.motion.state == SDL_BUTTON_LMASK)
                        {
                            hand_matrix[y_cell][x_cell]->PutCell(x,y,true);
                            hand_matrix[x_cell][y_cell]->PutCell(y,x,true);
                        }
                        if(event.motion.state == SDL_BUTTON_RMASK)
                        {
                            hand_matrix[y_cell][x_cell]->PutCell(x,y,false);
                            hand_matrix[x_cell][y_cell]->PutCell(y,x,false);
                        }
                    }
                    break;
                }
                case SDL_MOUSEBUTTONDOWN:
                {
                    int x(event.button.x % one_cell_size),y(event.button.y % one_cell_size);
                    int x_cell(event.button.x / one_cell_size),y_cell(event.button.y / one_cell_size);
                    bool suited(x_cell == y_cell);
                    std::cout << '\r' << hand_matrix[y_cell][x_cell]->getHandName(x,y) << "   ";
                    if(comfort_mode)
                    {
                        for(int x_cell(0); x_cell < 4; ++x_cell)
                            for(int y_cell(0); y_cell < 4; ++y_cell)
                            {
                                bool equal_coord(x_cell == y_cell);
                                if(equal_coord == suited)
                                {
                                    if(event.button.button == SDL_BUTTON_LEFT)
                                        hand_matrix[y_cell][x_cell]->PutCell(x,y,true);
                                    if(event.button.button == SDL_BUTTON_RIGHT)
                                        hand_matrix[y_cell][x_cell]->PutCell(x,y,false);
                                }
                            }
                    }
                    else
                    {
                        if(event.button.button == SDL_BUTTON_LEFT)
                        {
                            hand_matrix[y_cell][x_cell]->PutCell(x,y,true);
                            hand_matrix[x_cell][y_cell]->PutCell(y,x,true);
                        }
                        if(event.button.button == SDL_BUTTON_RIGHT)
                        {
                            hand_matrix[y_cell][x_cell]->PutCell(x,y,false);
                            hand_matrix[x_cell][y_cell]->PutCell(y,x,false);
                        }
                    }
                    break;
                }
                case SDL_KEYDOWN:
                    if(event.key.keysym.sym == SDLK_RETURN)
                        checking = false;
                    else if(event.key.keysym.sym == SDLK_SPACE)
                        comfort_mode = not comfort_mode;
                    break;
            }
            std::flush(std::cout);
        }
        for(int i(0); i < hand_matrix.size(); ++i)
            for(int j(0); j < hand_matrix.size(); ++j)
                hand_matrix[i][j]->PrintCell(display,j*one_cell_size,i*one_cell_size);

        SDL_Flip(display);
        usleep(10);
    }
    std::unordered_set<pkr::Hand> res;
    for(auto & a : hand_matrix)
        for(auto& cell : a)
            mergeSets(cell->getHands(),res);
    SDL_Quit();
    std::cout << '\n';

    return res;
}