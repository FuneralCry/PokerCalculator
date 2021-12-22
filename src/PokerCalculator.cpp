#include "PokerCalculator.h"

PokerCalculator::PokerCalculator()
{
    this->init_sdl();
    CardValuesInOut.insert({std::string("2"),'a'});
    CardValuesInOut.insert({std::string("3"),'b'});
    CardValuesInOut.insert({std::string("4"),'c'});
    CardValuesInOut.insert({std::string("5"),'d'});
    CardValuesInOut.insert({std::string("6"),'e'});
    CardValuesInOut.insert({std::string("7"),'f'});
    CardValuesInOut.insert({std::string("8"),'g'});
    CardValuesInOut.insert({std::string("9"),'h'});
    CardValuesInOut.insert({std::string("10"),'i'});
    CardValuesInOut.insert({std::string("J"),'j'});
    CardValuesInOut.insert({std::string("Q"),'k'});
    CardValuesInOut.insert({std::string("K"),'l'});
    CardValuesInOut.insert({std::string("A"),'m'});
    CardValuesInOut.insert({std::string("j"),'j'});
    CardValuesInOut.insert({std::string("q"),'k'});
    CardValuesInOut.insert({std::string("k"),'l'});
    CardValuesInOut.insert({std::string("a"),'m'});
    pkr::HandEq::init();
    running = true;
}

std::string PokerCalculator::Hand2String(pkr::HandEq::Hand hand) { return pkr::CardValuesOut[hand.value.first.first] + pkr::CardValuesOut[hand.value.first.second] + (hand.value.second ? 's' : 'n'); }

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
        calcEquity();
}

void PokerCalculator::processAction(pkr::Game_equity& game, int player_num)
{
    std::cout << "What action: check (c), fold (f), bet (b) or all-in (a)?" << '\n';
    char answ;
    std::cin >> answ;
    if(answ != 'c' and answ != 'f' and answ != 'b' and answ != 'a')
        throw std::invalid_argument("Invalid answer.");
    if(answ == 'b')
    {
        std::cout << "Enter stake:" << '\n';
        long long stake;
        std::cin >> stake;
        game.playerAction(player_num,pkr::Actions::bet,stake);
    }
    else if(answ == 'c')
        game.playerAction(player_num,pkr::Actions::check);
    else if(answ == 'f')
        game.playerAction(player_num,pkr::Actions::fold);
    else
        game.playerAction(player_num,pkr::Actions::allin);
}

void PokerCalculator::calcEquity()
{
    std::cout << "How many players we have at the table? (2-9)" << '\n';
    int players_num;
    std::cin >> players_num;
    if(players_num < 2 or players_num > 9)
        throw std::invalid_argument("Invalid number of players");
    std::cout << "What is your position? (0 - SB ... " << players_num << " - BB)" << '\n';
    int host_pos;
    std::cin >> host_pos;
    if(host_pos < 0 or host_pos >= players_num)
        throw std::invalid_argument("Invalid host position");
    pkr::Hand host_hand;
    std::vector<pkr::Player_s::StratSet> strategies;
    std::vector<long long> stacks;
    for(int i(0); i < players_num; ++i)
    {
        if(i == host_pos)
        {
            std::cout << "What hand do you have? (Enter in format like: 'A A s' or 'j 10 n')" << '\n';
            std::string first,second;
            char suited;
            std::cin >> first >> second >> suited;
            char f,s;
            f = this->CardValuesInOut[first];
            s = this->CardValuesInOut[second];
            if(suited == 's')
                host_hand = std::make_pair(pkr::Card(f,(char)pkr::CardSuit::club),pkr::Card(s,(char)pkr::CardSuit::club));
            else
                host_hand = std::make_pair(pkr::Card(f,(char)pkr::CardSuit::club),pkr::Card(s,(char)pkr::CardSuit::diamond));
            strategies.push_back(std::make_pair(pkr::HandEq::convertRange2Strat(1.0),pkr::HandEq::convertRange2Strat(1.0)));
            std::cout << "What stack do you have? (Enter numer like: 10000)" << '\n';
            long long stack;
            std::cin >> stack;
            stacks.push_back(stack);
        }
        else
        {
            std::cout << "What push range does player " << std::to_string(i) << " has?" << '\n';
            auto strat_push(this->render_matrix_and_get_hands({}));
            std::cout << "What current range does player " << std::to_string(i) << " has?" << '\n';
            auto strat_curr(this->render_matrix_and_get_hands(strat_push));
            for(auto strat : strat_push)
                strat_curr.insert(strat);
            strategies.push_back(std::make_pair(strat_push,strat_curr));
            std::cout << "What stack does player " << std::to_string(i) << " has? (Enter numer like: 10000)" << '\n';
            long long stack;
            std::cin >> stack;
            stacks.push_back(stack);
        }
    }
    std::cout << "What are current blinds? (Enter even number like: 200)" << '\n';
    long long BB;
    std::cin >> BB;
    pkr::Game_equity game(host_hand,std::vector<pkr::Card>(),players_num,strategies,stacks,host_pos,BB);
    game.setBlinds();
    std::cout << "Now you need to enter player's actions." << '\n';
    while(true)
    {
        int curr(*game.getOrder().begin());
        if(curr == host_pos)
        {
            std::cout << "It's your turn. Do you wish to calculate equity (1) or proceed acting (2)?" << '\n';
            int answ;
            std::cin >> answ;
            if(answ < 1 or answ > 2)
                throw std::invalid_argument("Invalid answer.");
            if(answ == 1)
                break;
            else
                this->processAction(game,curr);
        }
        else
        {
            std::cout << "It is turn of " << std::to_string(curr) << '\n';
            this->processAction(game,curr);
        }
    }
    std::cout << "Your chip EV is: " << std::to_string(game.getEquity()) << '\n';
    this->render_ev_matrix(game);
}