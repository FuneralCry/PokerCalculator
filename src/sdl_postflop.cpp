#include "PokerCalculator.h"

int PokerCalculator::TwoSuitsHandsCell::cards_counter = 0;

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
    pkr::Hand h(this->hand_from_xy_postflop(x,y,this->s1,this->s2));
    if(this->black_list.find(h) != this->black_list.end())
        return;
    if(val)
        cards_counter++;
    else
        cards_counter--;
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
    SDL_FillRect(screen,&first_frame,SDL_MapRGB(screen->format,suit_colors[s2][1][0],suit_colors[s2][1][1],suit_colors[s2][1][2]));
    SDL_FillRect(screen,&second_frame,SDL_MapRGB(screen->format,suit_colors[s1][1][0],suit_colors[s1][1][1],suit_colors[s1][1][2]));
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

    return (std::string)pkr::Card((char)pkr::CardValue::ace-x,s2) + (std::string)pkr::Card((char)pkr::CardValue::ace-y,s1);
}

int PokerCalculator::TwoSuitsHandsCell::getCardsNum() const { return cards_counter; }

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

std::unordered_set<pkr::Hand> PokerCalculator::mergeSets(std::unordered_set<pkr::Hand> a, std::unordered_set<pkr::Hand>& dest)
{
    for(const pkr::Hand& h : a)
        dest.insert(h);
    
    return dest;
}

std::unordered_set<pkr::Hand> PokerCalculator::render_matrix_and_get_hands_postflop(std::unordered_set<pkr::Hand> black_list, int max_cards)
{
    if(max_cards > 0)
        std::cout << "Comfort mode unavailable" << '\n';
    else if(max_cards == 0)
        return std::unordered_set<pkr::Hand>();
    std::cout << "Current hand:" << '\n';
    SDL_Surface* display;
    int one_cell_size(HANDS_MATRIX_CELL_SIZE_POSTFLOP*13+FRAME_WIDTH_POSTFLOP);
    if((display = SDL_SetVideoMode(one_cell_size*4,one_cell_size*4,32,SDL_HWSURFACE | SDL_DOUBLEBUF)) == NULL)
        throw std::runtime_error("Can't render hand matrix");
    std::array<std::array<TwoSuitsHandsCell*,4>,4> hand_matrix;
    std::unordered_map<char,std::array<std::array<int,3>,2>> suit_colors(getSuitColors());
    int cards_num(0);
    for(char s1((char)pkr::CardSuit::club); s1 <= (char)pkr::CardSuit::spade; ++s1)
        for(char s2((char)pkr::CardSuit::club); s2 <= (char)pkr::CardSuit::spade; ++s2)
            hand_matrix[s1][s2] = new TwoSuitsHandsCell(s2,s1,black_list,suit_colors);

    bool checking(true);
    bool comfort_mode(false);
    while(checking)
    {
        if(PokerCalculator::TwoSuitsHandsCell::cards_counter >= max_cards and max_cards > 0)
            break;
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
                        comfort_mode = not (comfort_mode and max_cards > 0);
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
    PokerCalculator::TwoSuitsHandsCell::cards_counter = 0;
    for(char s1((char)pkr::CardSuit::club); s1 <= (char)pkr::CardSuit::spade; ++s1)
        for(char s2((char)pkr::CardSuit::club); s2 <= (char)pkr::CardSuit::spade; ++s2)
            delete hand_matrix[s1][s2];

    return res;
}