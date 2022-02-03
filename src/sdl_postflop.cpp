#include "PokerCalculator.h"

int PokerCalculator::Cell_postflop::cards_counter = 0;
std::unordered_set<pkr::Hand> PokerCalculator::Cell_postflop::black_list;
std::unordered_map<char,std::array<std::array<int,3>,2>> PokerCalculator::Cell_postflop::suit_colors;

PokerCalculator::Cell_postflop::Cell_postflop(char s1, char s2) : s1(s1), s2(s2)
{
    for(auto& a : this->hand_matrix_choice)
        for(auto& b : a)
            b = false;
}

void PokerCalculator::Cell_postflop::PutCell(int x, int y, bool val)
{
    if(x < FRAME_WIDTH_POSTFLOP or y < FRAME_WIDTH_POSTFLOP)
        return;
    x -= FRAME_WIDTH_POSTFLOP;
    y -= FRAME_WIDTH_POSTFLOP;
    x /= HANDS_MATRIX_CELL_SIZE_POSTFLOP;
    y /= HANDS_MATRIX_CELL_SIZE_POSTFLOP;
    pkr::Hand h(this->hand_from_xy_postflop(x,y,this->s1,this->s2));
    if(this->black_list.find(h) != this->black_list.end() or h.first == h.second)
        return;
    if(val)
        cards_counter++;
    else
        cards_counter--;
    this->hand_matrix_choice[y][x] = val;
    if(s1 == s2)
        this->hand_matrix_choice[x][y] = val;
}

void PokerCalculator::Cell_postflop::PrintCell(SDL_Renderer* renderer, int init_x, int init_y)
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
    SDL_SetRenderDrawColor(renderer,suit_colors[s2][1][0],suit_colors[s2][1][1],suit_colors[s2][1][2],1);
    SDL_RenderFillRect(renderer,&first_frame);
    SDL_SetRenderDrawColor(renderer,suit_colors[s1][1][0],suit_colors[s1][1][1],suit_colors[s1][1][2],1);
    SDL_RenderFillRect(renderer,&second_frame);
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
                SDL_SetRenderDrawColor(renderer,BLACKED_RGB,BLACKED_RGB,BLACKED_RGB,1);
            else
            {
                char s_color = i > j ? s1 : s2;
                int active = hand_matrix_choice[i][j] ? 1 : 0;
                SDL_SetRenderDrawColor(renderer,suit_colors[s_color][active][0],suit_colors[s_color][active][1],suit_colors[s_color][active][2],1);
            }
            cell.x = j * cell.w + init_x;
            cell.y = i * cell.h+ + init_y;
            SDL_RenderFillRect(renderer,&cell);
        }
    }
}

std::unordered_set<pkr::Hand> PokerCalculator::Cell_postflop::getHands() const
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

std::string PokerCalculator::Cell_postflop::getHandName(int x, int y) const
{
    if(x < FRAME_WIDTH_POSTFLOP or y < FRAME_WIDTH_POSTFLOP)
        return "None";
    x -= FRAME_WIDTH_POSTFLOP;
    y -= FRAME_WIDTH_POSTFLOP;
    x /= HANDS_MATRIX_CELL_SIZE_POSTFLOP;
    y /= HANDS_MATRIX_CELL_SIZE_POSTFLOP;

    return (std::string)pkr::Card((char)pkr::CardValue::ace-x,s2) + (std::string)pkr::Card((char)pkr::CardValue::ace-y,s1);
}

int PokerCalculator::Cell_postflop::getCardsNum() const { return cards_counter; }

std::unordered_set<pkr::Hand> PokerCalculator::mergeSets(std::unordered_set<pkr::Hand> a, std::unordered_set<pkr::Hand>& dest)
{
    for(const pkr::Hand& h : a)
        dest.insert(h);
    
    return dest;
}

pkr::Hand PokerCalculator::Cell_postflop::hand_from_xy_postflop(int x, int y, int s1, int s2) const
{
    char v1((char)pkr::CardValue::ace-y),v2((char)pkr::CardValue::ace-x);

    return std::make_pair(pkr::Card(v1,s1),pkr::Card(v2,s2));
}

std::unordered_set<pkr::Hand> PokerCalculator::render_matrix_and_get_hands_postflop(std::unordered_set<pkr::Hand> black_list, int max_cards)
{
    if(max_cards > 0)
        std::cout << "Comfort mode unavailable" << '\n';
    else if(max_cards == 0)
        return std::unordered_set<pkr::Hand>();
    std::cout << "Current hand:" << '\n';
    int one_cell_size(HANDS_MATRIX_CELL_SIZE_POSTFLOP*13+FRAME_WIDTH_POSTFLOP);
    SDL_Window* window;
    SDL_Renderer* renderer;
    if((window = SDL_CreateWindow("Preflop hands",SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,one_cell_size*4,one_cell_size*4,SDL_WINDOW_SHOWN)) == nullptr)
        throw std::runtime_error("Can't render hand matrix");
    if((renderer = SDL_CreateRenderer(window,-1,SDL_RENDERER_ACCELERATED)) == nullptr)
        throw std::runtime_error("Can't render hand matrix");
    std::array<std::array<Cell_postflop*,4>,4> hand_matrix;
    PokerCalculator::Cell_postflop::suit_colors = getSuitColors();
    PokerCalculator::Cell_postflop::black_list = black_list;
    int cards_num(0);
    for(char s1((char)pkr::CardSuit::club); s1 <= (char)pkr::CardSuit::spade; ++s1)
        for(char s2((char)pkr::CardSuit::club); s2 <= (char)pkr::CardSuit::spade; ++s2)
            hand_matrix[s1][s2] = new Cell_postflop(s2,s1);

    bool checking(true);
    bool comfort_mode(false);
    SDL_Event event;
    while(SDL_PollEvent(&event)) { }
    while(checking)
    {
        if(PokerCalculator::Cell_postflop::cards_counter >= max_cards and max_cards > 0)
            break;
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
                                    {
                                        hand_matrix[y_cell][x_cell]->PutCell(x,y,true);
                                        hand_matrix[y_cell][x_cell]->PutCell(y,x,true);
                                    }
                                    if(event.motion.state == SDL_BUTTON_RMASK)
                                    {
                                        hand_matrix[y_cell][x_cell]->PutCell(x,y,false);
                                        hand_matrix[y_cell][x_cell]->PutCell(y,x,false);
                                    }
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
                                    {
                                        hand_matrix[y_cell][x_cell]->PutCell(x,y,true);
                                        hand_matrix[y_cell][x_cell]->PutCell(y,x,true);
                                    }
                                    if(event.button.button == SDL_BUTTON_RIGHT)
                                    {
                                        hand_matrix[y_cell][x_cell]->PutCell(x,y,false);
                                        hand_matrix[y_cell][x_cell]->PutCell(y,x,false);
                                    }
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
                hand_matrix[i][j]->PrintCell(renderer,j*one_cell_size,i*one_cell_size);

        SDL_RenderPresent(renderer);
        SDL_Delay(10);
    }
    std::unordered_set<pkr::Hand> res;
    for(auto & a : hand_matrix)
        for(auto& cell : a)
            mergeSets(cell->getHands(),res);
    SDL_Quit();
    std::cout << '\n';
    PokerCalculator::Cell_postflop::cards_counter = 0;
    for(char s1((char)pkr::CardSuit::club); s1 <= (char)pkr::CardSuit::spade; ++s1)
        for(char s2((char)pkr::CardSuit::club); s2 <= (char)pkr::CardSuit::spade; ++s2)
            delete hand_matrix[s1][s2];

    return res;
}