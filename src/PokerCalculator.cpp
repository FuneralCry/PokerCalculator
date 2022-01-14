#include "PokerCalculator.h"

PokerCalculator::PokerCalculator()
{
    this->init_sdl();
    CardValuesInOut.insert({std::string("2"),(char)pkr::CardValue::two });
    CardValuesInOut.insert({std::string("3"),(char)pkr::CardValue::three });
    CardValuesInOut.insert({std::string("4"),(char)pkr::CardValue::four });
    CardValuesInOut.insert({std::string("5"),(char)pkr::CardValue::five });
    CardValuesInOut.insert({std::string("6"),(char)pkr::CardValue::six });
    CardValuesInOut.insert({std::string("7"),(char)pkr::CardValue::seven });
    CardValuesInOut.insert({std::string("8"),(char)pkr::CardValue::eight });
    CardValuesInOut.insert({std::string("9"),(char)pkr::CardValue::nine });
    CardValuesInOut.insert({std::string("10"),(char)pkr::CardValue::ten });
    CardValuesInOut.insert({std::string("J"),(char)pkr::CardValue::jack });
    CardValuesInOut.insert({std::string("Q"),(char)pkr::CardValue::queen });
    CardValuesInOut.insert({std::string("K"),(char)pkr::CardValue::king });
    CardValuesInOut.insert({std::string("A"),(char)pkr::CardValue::ace });
    CardValuesInOut.insert({std::string("j"),(char)pkr::CardValue::jack });
    CardValuesInOut.insert({std::string("q"),(char)pkr::CardValue::queen });
    CardValuesInOut.insert({std::string("k"),(char)pkr::CardValue::king });
    CardValuesInOut.insert({std::string("a"),(char)pkr::CardValue::ace });
    CardSuitsInOut.insert({'c',(char)pkr::CardSuit::club });
    CardSuitsInOut.insert({'d',(char)pkr::CardSuit::diamond });
    CardSuitsInOut.insert({'s',(char)pkr::CardSuit::spade });
    CardSuitsInOut.insert({'h',(char)pkr::CardSuit::heart });
    pkr::HandEq::init();
    running = true;
}

std::string PokerCalculator::Hand2String_preflop(pkr::HandEq::Hand hand) { return pkr::CardValuesOut[hand.value.first.first] + pkr::CardValuesOut[hand.value.first.second] + (hand.value.second ? 's' : 'n'); }

std::unordered_set<pkr::Hand> PokerCalculator::inverseSet_postflop(const std::unordered_set<pkr::Hand>& hands)
{
    std::unordered_set<pkr::Hand> res;
    for(char v1((char)pkr::CardValue::two); v1 <= (char)pkr::CardValue::ace; ++v1)
    {
        for(char s1((char)pkr::CardSuit::club); s1 <= (char)pkr::CardSuit::spade; ++s1)
        {
            for(char v2((char)pkr::CardValue::two); v2 <= (char)pkr::CardValue::ace; ++v2)
            {
                for(char s2((char)pkr::CardSuit::club); s2 <= (char)pkr::CardSuit::spade; ++s2)
                {
                    pkr::Hand h(
                        std::make_pair(
                            pkr::Card(v1,s1),
                            pkr::Card(v2,s2)
                        )
                        );
                    if(hands.find(h) == hands.end())
                        res.insert(h);
                }
            }
        }
    }

    return res;
}

std::unordered_set<pkr::Card> PokerCalculator::getCardsFromHands(std::unordered_set<pkr::Hand> hands)
{
    std::unordered_set<pkr::Card> res;
    for(const pkr::Hand& h : hands)
    {
        res.insert(h.first);
        res.insert(h.second);
    }

    return res;
}

std::unordered_set<pkr::Hand> PokerCalculator::getHandsFromCards(const std::unordered_set<pkr::Card>& cards)
{
    std::unordered_set<pkr::Hand> res;
    for(char v1((char)pkr::CardValue::two); v1 <= (char)pkr::CardValue::ace; ++v1)
    {
        for(char s1((char)pkr::CardSuit::club); s1 <= (char)pkr::CardSuit::spade; ++s1)
        {
            for(char v2((char)pkr::CardValue::two); v2 <= (char)pkr::CardValue::ace; ++v2)
            {
                for(char s2((char)pkr::CardSuit::club); s2 <= (char)pkr::CardSuit::spade; ++s2)
                {
                    pkr::Hand h(
                        std::make_pair(
                            pkr::Card(v1,s1),
                            pkr::Card(v2,s2)
                        )
                        );
                    if(cards.find(h.first) != cards.end() or cards.find(h.second) != cards.end())
                        res.insert(h);
                }
            }
        }
    }

    return res;
}

void PokerCalculator::start()
{
    std::cout << "Welcome to Poker Calculator!" << '\n';
    std::cout << "Here you can calculate equity of hand on preflop (1), calculate your odds against custom opponents (2)." << '\n';
    std::cout << "So, what do you want?" << '\n';
    int answ;
    std::cin >> answ;
    if(answ < 1 or answ > 2)
        throw std::invalid_argument("Invalid answer");
    if(answ == 1)
        this->calcEquity();
    else if(answ == 2)
        this->calcOdds();
}

void PokerCalculator::processAction(pkr::Game_equity_postflop* game, int player_num)
{
    std::cout << "What action: check (c), fold (f), bet (b), call (l) or all-in (a)?" << '\n';
    long long to_call(game->getMinStake() - game->getPlayers()[player_num]->getPaidThisState());
    if(to_call > 0)
        std::cout << to_call << " to call.";
    else
        std::cout << "Call is unavailable.";
    std::cout << '\n';
    char answ;
    std::cin >> answ;
    if(answ != 'c' and answ != 'f' and answ != 'b' and answ != 'a' and answ != 'l')
        throw std::invalid_argument("Invalid answer.");
    if(answ == 'b')
    {
        std::cout << "Enter stake:" << '\n';
        long long stake;
        std::cin >> stake;
        game->playerAction(player_num,pkr::Actions::bet,stake);
    }
    else if(answ == 'c')
        game->playerAction(player_num,pkr::Actions::check);
    else if(answ == 'f')
        game->playerAction(player_num,pkr::Actions::fold);
    else if(answ == 'l')
        game->playerAction(player_num,pkr::Actions::call);
    else if(answ == 'a')
        game->playerAction(player_num,pkr::Actions::allin);
    else
        throw std::invalid_argument("Invalid answer.");
}

std::vector<pkr::Card> PokerCalculator::fillBoard()
{
    std::cout << "What is current game state: Preflop (p), Flop(f), Turn (t), River (r)" << '\n';
    char answ;
    std::cin >> answ;
    int card_num;
    if(answ == 'p')
        card_num = 0;
    else if(answ == 'f')
        card_num = 3;
    else if(answ == 't')
        card_num = 4;
    else if(answ == 'r')
        card_num == 5;
    else
        throw std::invalid_argument("Invalid answer.");
    std::vector<pkr::Card> res;
    std::unordered_set<pkr::Card> black_list;
    for(int i(0); i < card_num; ++i)
    {
        /*
        std::cout << "Enter " << std::to_string(i) << "-th card: (In format like: 2 s or A c)" << '\n' << "c - club, h - heart, s - spade, d - diamond" << '\n';
        std::string vs,ss;
        std::cin >> vs >> ss;
        char v(CardValuesInOut[vs]);
        char s(CardSuitsInOut[ss[0]]);
        */
        pkr::Card c(pickCard(black_list));
        black_list.insert(c);
        res.push_back(c);
    }
    
    return res;
}

void PokerCalculator::calcEquity()
{
    std::cout << "How many players we have at the table? (2-3)" << '\n';
    int players_num;
    std::cin >> players_num;
    if(players_num < 2 or players_num > 3)
        throw std::invalid_argument("Invalid number of players");
    std::cout << "What is your position? (0 - SB ... " << players_num << " - BU)" << '\n';
    int host_pos;
    std::cin >> host_pos;
    if(host_pos < 0 or host_pos >= players_num)
        throw std::invalid_argument("Invalid host position");
    pkr::Hand host_hand;
    std::vector<std::pair<double,double>> strategies;
    std::vector<long long> stacks;
    std::vector<pkr::Card> board(this->fillBoard());
    std::unordered_set<pkr::Card> reserved_cards(board.begin(),board.end());
    std::unordered_set<pkr::Hand> black_list(this->getHandsFromCards(reserved_cards));
    for(int i(0); i < players_num; ++i)
    {
        if(i == host_pos)
        {
            std::cout << "What hand do you have? (Enter in format like: 'A c A d' or 'j h 10 h')" << '\n';
            std::cout << "c - club, h - heart, s - spade, d - diamond" << '\n';
            /*
            std::string first,second;
            char s1,s2;
            std::cin >> first >> s1 >> second >> s2;
            char f,s;
            f = this->CardValuesInOut[first];
            s = this->CardValuesInOut[second];
            s1 = CardSuitsInOut[s1];
            s2 = CardSuitsInOut[s2];
            host_hand = std::make_pair(pkr::Card(f,s1),pkr::Card(s,s2));
            */
            auto hand_set(render_matrix_and_get_hands_postflop(black_list,1));
            if(hand_set.empty())
                throw std::invalid_argument("Pick a card");
            host_hand = *hand_set.begin();
            this->mergeSets(this->getHandsFromCards(this->getCardsFromHands(std::unordered_set<pkr::Hand>{host_hand})),black_list);
            std::cout << "What stack do you have? (Enter number like: 10000)" << '\n';
            long long stack;
            std::cin >> stack;
            stacks.push_back(stack);
        }
        else
        {
            std::cout << "What stack does player " << std::to_string(i) << " has? (Enter number like: 10000)" << '\n';
            long long stack;
            std::cin >> stack;
            stacks.push_back(stack);
        }
        strategies.push_back(std::make_pair(1.0,1.0));
    }
    std::cout << "What are current blinds? (Enter even number like: 200)" << '\n';
    long long BB;
    std::cin >> BB;
    pkr::Game_equity_postflop* game;
    if(board.empty())
        game = new pkr::Game_equity_preflop(host_hand,players_num,strategies,stacks,host_pos,BB);
    else
        game = new pkr::Game_equity_postflop(host_hand,board,players_num,strategies,stacks,host_pos,BB);
    game->setBlinds();
    std::cout << "Now you need to enter player's actions." << '\n';
    while(true)
    {
        int curr(*game->getOrder().begin());
        if(curr == host_pos)
        {
            std::cout << "It's your turn." << '\n';
            if(game->getState() == game->getMaxState())
            {
                std::cout << "Do you wish to calculate equity (1) or proceed acting (2)?" << '\n';
                int answ;
                std::cin >> answ;
                if(answ < 1 or answ > 2)
                    throw std::invalid_argument("Invalid answer.");
                if(answ == 1)
                    break;
            }
            this->processAction(game,curr);
        }
        else
        {
            std::cout << "It is turn of " << std::to_string(curr) << '\n';
            this->processAction(game,curr);
        }
        if(game->canChangeState())
        {
            if(game->getState() == (int)pkr::States::pflop)
            {
                if(board.size() < 3)
                    throw std::runtime_error("Invalid board size");
                game->changeGameState();
            }
            else if(game->getState() == (int)pkr::States::flop)
            {
                if(board.size() < 4)
                    throw std::runtime_error("Invalid board size");
                game->changeGameState();
            }
            else if(game->getState() == (int)pkr::States::flop)
            {
                if(board.size() < 5)
                    throw std::runtime_error("Invalid board size");
                game->changeGameState();
            }
            else
                throw std::runtime_error("Game has been ended");

            std::cout << "Game state has been changed to: " << pkr::SatesOut[game->getState()] << '\n';
        }
    }

    for(int i : game->getOrder())
    {
        if(game->getPlayers()[i]->isHost())
            continue;
        if(board.empty())
        {
            std::cout << "What current range does player " << std::to_string(i) << " has?" << '\n';
            auto strat_curr(this->render_matrix_and_get_hands_preflop({}));
            std::cout << "What push range does player " << std::to_string(i) << " has?" << '\n';
            auto strat_push(this->render_matrix_and_get_hands_preflop(pkr::HandEq::inverseSet(strat_curr)));
            for(auto strat : strat_push)
                strat_curr.insert(strat);
            pkr::Game_equity_preflop* game_prf(dynamic_cast<pkr::Game_equity_preflop*>(game));
            game_prf->changeStrategy(i,std::make_pair(strat_push,strat_curr));
        }
        else
        {
            std::cout << "What current range does player " << std::to_string(i) << " has?" << '\n';
            auto strat_curr(this->render_matrix_and_get_hands_postflop(black_list));
            std::cout << "What push range does player " << std::to_string(i) << " has?" << '\n';
            std::unordered_set<pkr::Hand> curr_black_list(this->inverseSet_postflop(strat_curr));
            this->mergeSets(black_list,curr_black_list);
            auto strat_push(this->render_matrix_and_get_hands_postflop(curr_black_list));
            for(auto strat : strat_push)
                strat_curr.insert(strat);
            game->changeStrategy(i,std::make_pair(strat_push,strat_curr));
        }
    }
    std::cout << "Your chip EV is: ";
    if(board.empty())
    {
        std::cout << std::to_string(game->getEquity()) << '\n';
        this->render_ev_matrix(game);
    }
    else
        std::cout << std::to_string(game->getEquity()) << '\n';
}

std::unordered_set<int> findOutWinner(const pkr::Win& result)
{
    int mx(0);
    std::unordered_set<int> res;
    for(int i(1); i < result.size(); ++i)
    {
        if(*result[i] > *result[mx])
            mx = i;
    }
    for(int i(0); i < result.size(); ++i)
    {
        if(not (*result[i] < *result[mx]))
            res.insert(i);
    }

    return res;
}

void PokerCalculator::calcOdds()
{
    std::cout << "How many players we have at the table? (2-9)" << '\n';
    int players_num;
    std::cin >> players_num;
    if(players_num < 2 or players_num > 9)
        throw std::invalid_argument("Invalid number of players");
    std::unordered_set<pkr::Hand> black_list;
    std::vector<pkr::Card> board(fillBoard());
    this->mergeSets(this->getHandsFromCards(std::unordered_set<pkr::Card>(board.begin(),board.end())),black_list);
    std::vector<pkr::Player> players;
    for(int i(0); i < players_num; ++i)
    {
        std::cout << "Enter hand of player " << std::to_string(i) << '\n';
        auto hand_set(render_matrix_and_get_hands_postflop(black_list,1));
        if(hand_set.empty())
            throw std::invalid_argument("Pick a card");
        pkr::Hand h(*hand_set.begin());
        this->mergeSets(this->getHandsFromCards(this->getCardsFromHands(std::unordered_set<pkr::Hand>({h}))),black_list);
        players.push_back(pkr::Player(h,"",false));
    }
    std::vector<long> players_wins(players_num);
    pkr::Game_binary_sim game(players,board);
    for(int i(0); i < SIMULATIONS_NUMBER; ++i)
    {
        pkr::Game_binary_sim g_sim(game.simulate());
        auto winners(findOutWinner(g_sim.getResults()));
        for(int p : winners)
            ++players_wins[p];
    }
    for(int i(0); i < players_num; ++i)
        std::cout << "Player " << std::to_string(i) << ": " << std::to_string(players_wins[i] * 100 / SIMULATIONS_NUMBER) << "%" << '\n';
}