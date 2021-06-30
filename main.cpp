#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <ctime>
#include <cstdlib>
#include <fstream>
#include <regex>

using namespace std;

struct Config {
    int r;
    int g;
    int b;
    int health;
    double speed;
    string level;
};
Config config;

class Entity: public sf::RectangleShape
{
public:
    int lane;
    vector <sf::Texture *> Tex;
    virtual void animate(const sf::Time &elapsed)=0;
};

class Moving: public Entity
{
public:
    int lane,hit,score,frame=0;
    double animationTime=0;
    Moving(int L)
    {
        double y;
        if(L) y=640;
        else y=800;
        setPosition(1820,y);
        sf::Vector2f S(100.0, 100.0);
        setSize(S);
        lane =L;
        //cout<<
    }
    void animate(const sf::Time &elapsed)
    {
        double time=float((elapsed.asMicroseconds())/1000000.0);
        move(time*-config.speed,0.0);
        if(Tex.size())
        {
            animationTime+=time;
            if(animationTime>=0.15)
            {
                animationTime-=0.15;
                frame++;
                if(frame==Tex.size()) frame=0;
                setTexture(Tex[frame]);
            }
        }
    }
};

class Enemy: public Moving
{
public:
    Enemy(int L): Moving(L)
    {
        hit=10;
        score=50;
        //setFillColor(sf::Color(100, 256, 100));
    }
};

class Heart: public Moving
{
public:
    Heart(int l): Moving(l)
    {
        hit=-50;
        score=20;
        //setFillColor(sf::Color(50, 168, 86));
    }
};

class Note: public Moving
{
public:
    Note(int l): Moving(l)
    {
        hit=0;
        score=100;
        //setFillColor(sf::Color(255, 0, 255));
    }
};

struct LvlElement
{
    double time;
    char type;
    int lane;
};

class Player: public Entity
{
public:
    Player()
    {
        setPosition(480,800);
        sf::Vector2f S(192.0, 192.0);
        setSize(S);
        setFillColor(sf::Color(config.r, config.g, config.b));
        lane=0;
    }
    int health=config.health,points=0,combo=0;
    double HitTimer=0;
    void up()
    {
        setPosition(480,640);
        lane=1;
        setTexture(Tex[1]);
        HitTimer=0.15;
    }
    void down()
    {
        setPosition(480,800);
        lane=0;
        setTexture(Tex[1]);
        HitTimer=0.15;
    }
    void animate(const sf::Time &elapsed)
    {
        sf::Vector2f V=getPosition();
        double time=float((elapsed.asMicroseconds())/1000000.0);
        if(HitTimer>0.0)
        {
            HitTimer-=time;
        }
        else
        {
            setTexture(Tex[0]);
        }
        if(V.y<680)
        {
            move(0.0,time*111.1);
        }
        else if(V.y<800)
        {
            move(0.0,time*444.4);
            //cout<<time*100.0<<endl;
            if(V.y<700) lane=0;
        }
        if(V.y>800) setPosition(480,800);
    }
    bool hit(Moving &M)
    {
        sf::FloatRect player_bounds = getGlobalBounds();
        sf::FloatRect enemy_bounds = M.getGlobalBounds();
        double l=enemy_bounds.left-player_bounds.left-player_bounds.width;
        //cout<<l<<endl;
        //sf::Vector2f V=M.getPosition();
        if(lane==M.lane)
        {
            if(l>-80.0 and l<00.0)
            {
                M.setFillColor(sf::Color(100, 100, 100));
                if(M.hit<0) health-=M.hit;
                points+=M.score;
                if(combo>15)points+=M.score/5;
                if(combo>30)points+=M.score/5;
                combo++;
                cout<<combo<<endl;
                if(health>config.health)
                {
                    health=config.health;
                    points+=100;
                }
                return 1;
            }
        }
        return 0;
    }
    bool collide(Moving &M)
    {
        sf::FloatRect player_bounds = getGlobalBounds();
        sf::FloatRect enemy_bounds = M.getGlobalBounds();
        double l=enemy_bounds.left-player_bounds.left-player_bounds.width;
        //cout<<l<<endl;
        //sf::Vector2f V=M.getPosition();
        if(lane==M.lane)
        {
            if(l<-80.0 and l>-1*player_bounds.width)
            {
                health-=M.hit;
                if(health>config.health)
                {
                    health=config.health;
                    points+=100;
                }
                if(M.hit<=0)
                {
                    points+=M.score;
                    if(combo>10)points+=M.score/5;
                    if(combo>20)points+=M.score/5;
                }
                else combo=0;
                return 1;
            }
            if(l<-1*player_bounds.width)
            {
                cout<<combo<<endl;
                combo=0;
                cout<<combo<<endl<<endl;
                return 1;
            }
        }
        return 0;
    }
};


void loadConfig(Config& config) {
    ifstream fin("config.txt");
    string line;
    while (getline(fin, line)) {
        istringstream sin(line.substr(line.find("=") + 1));
        if (line.find("r") != -1)
            sin >> config.r;
        else if (line.find("g") != -1)
            sin >> config.g;
        else if (line.find("b") != -1)
            sin >> config.b;
        else if (line.find("speed") != -1)
            sin >> config.speed;
        else if (line.find("level") != -1)
            sin >> config.level;
        else if (line.find("health") != -1)
            sin >> config.health;
    }
}

class Background
{
public:
    double speed;
    sf::RectangleShape back[2];
    Background(sf::Texture* Tex, double spd)
    {
        speed=spd;
        back[0].setTexture(Tex);
        back[1].setTexture(Tex);
        back[1].setPosition(1920.0,0.0);
        sf::Vector2f S(1920.0, 1080.0);
        back[0].setSize(S);
        back[1].setSize(S);
    }
    void animate(const sf::Time &elapsed)
    {
        double time=float((elapsed.asMicroseconds())/1000000.0);
        back[0].move(time*-speed,0.0);
        back[1].move(time*-speed,0.0);
        if(back[0].getGlobalBounds().left<-1920) back[0].move(2*1920,0);
        if(back[1].getGlobalBounds().left<-1920) back[1].move(2*1920,0);
    }
};

int main()
{
    loadConfig(config);

    sf::RenderWindow window(sf::VideoMode(1920 , 1080), "My window");
    sf::Event event;

    vector <Moving*> V;
    vector <LvlElement> E;
    LvlElement ele;
    int l;
    char t;
    double ti;
    int ECounter=0;
    Player P;

    ifstream level;

    level.open(config.level+".txt");
    while(level>>ti>>t>>l)
    {
        ele.lane=l;
        ele.type=t;
        //ele.time=ti-1.647;
        ele.time=ti-(350.0/config.speed);
        E.push_back(ele);
    }
    level.close();

    double T=0;
    sf::Texture slash0,slash1,enemy1,enemy2,enemy3,enemy4,heart1,heart2,heart3,heart4,back1,back2,back3,back4,back5,coin1,coin2,coin3,coin4,coin5;
    if (!slash0.loadFromFile("./animation experiment x3_slash_0.png")) {
        std::cerr << "Could not load texture" << std::endl;
        return 1;
    }
    if (!slash1.loadFromFile("./animation experiment x3_slash_1.png")) {
        std::cerr << "Could not load texture" << std::endl;
        return 1;
    }

    if (!enemy1.loadFromFile("./row-1-col-1.png")) {
        std::cerr << "Could not load texture" << std::endl;
        return 1;
    }

    if (!enemy2.loadFromFile("./row-1-col-2.png")) {
        std::cerr << "Could not load texture" << std::endl;
        return 1;
    }

    if (!enemy3.loadFromFile("./row-1-col-3.png")) {
        std::cerr << "Could not load texture" << std::endl;
        return 1;
    }

    if (!enemy4.loadFromFile("./row-1-col-4.png")) {
        std::cerr << "Could not load texture" << std::endl;
        return 1;
    }

    if (!heart1.loadFromFile("./heart (1).png")) {
        std::cerr << "Could not load texture" << std::endl;
        return 1;
    }

    if (!heart2.loadFromFile("./heart (2).png")) {
        std::cerr << "Could not load texture" << std::endl;
        return 1;
    }

    if (!heart3.loadFromFile("./heart (3).png")) {
        std::cerr << "Could not load texture" << std::endl;
        return 1;
    }

    if (!heart4.loadFromFile("./heart (4).png")) {
        std::cerr << "Could not load texture" << std::endl;
        return 1;
    }

    if (!back5.loadFromFile("./parallax-forest-back-trees.png")) {
        std::cerr << "Could not load texture" << std::endl;
        return 1;
    }

    if (!back3.loadFromFile("./parallax-forest-middle-trees.png")) {
        std::cerr << "Could not load texture" << std::endl;
        return 1;
    }

    if (!back2.loadFromFile("./parallax-forest-front-trees.png")) {
        std::cerr << "Could not load texture" << std::endl;
        return 1;
    }

    if (!back4.loadFromFile("./parallax-forest-lights.png")) {
        std::cerr << "Could not load texture" << std::endl;
        return 1;
    }

    if (!back1.loadFromFile("./country-platform-tiles-example.png")) {
        std::cerr << "Could not load texture" << std::endl;
        return 1;
    }

    if (!coin1.loadFromFile("./coin (1).png")) {
        std::cerr << "Could not load texture" << std::endl;
        return 1;
    }

    if (!coin2.loadFromFile("./coin (2).png")) {
        std::cerr << "Could not load texture" << std::endl;
        return 1;
    }

    if (!coin3.loadFromFile("./coin (3).png")) {
        std::cerr << "Could not load texture" << std::endl;
        return 1;
    }

    if (!coin4.loadFromFile("./coin (4).png")) {
        std::cerr << "Could not load texture" << std::endl;
        return 1;
    }

    if (!coin5.loadFromFile("./coin (5).png")) {
        std::cerr << "Could not load texture" << std::endl;
        return 1;
    }

    sf::SoundBuffer buffer;
        if (!buffer.loadFromFile(config.level+".ogg")){
            std::cerr << "Could not load music file" << std::endl;
            return 1;
        }


    sf::Font font;
    if (!font.loadFromFile("./PressStart2P.ttf")){
        std::cerr << "Could not load font" << std::endl;
        return 1;
    }

    sf::Text text,health,combo;


    text.setFont(font);
    health.setFont(font);
    combo.setFont(font);

    text.setCharacterSize(24);
    health.setCharacterSize(24);
    combo.setCharacterSize(24);

    text.setFillColor(sf::Color::White);
    health.setFillColor(sf::Color::Red);
    combo.setFillColor(sf::Color::White);

    text.setCharacterSize(33);
    health.setCharacterSize(33);
    combo.setCharacterSize(33);



    sf::Sound sound;
    sound.setBuffer(buffer);

    P.Tex.push_back(&slash0);
    P.Tex.push_back(&slash1);
    P.setTexture(&slash0);
    sound.play();
    sf::Clock clock;
/*
    sf::RectangleShape b1,b2,b3,b4,b5;

    sf::Vector2f S(1920 , 1080);
    b1.setSize(S);b2.setSize(S);b3.setSize(S);b4.setSize(S);b5.setSize(S);
    b1.setTexture(&back1);
    b2.setTexture(&back2);
    b3.setTexture(&back3);
    b4.setTexture(&back4);
    b5.setTexture(&back5);
*/

    Background b1(&back1,config.speed),b2(&back2,config.speed*0.8),b3(&back3,config.speed*0.7),b4(&back4,config.speed*0.6),b5(&back5,config.speed*0.4);
    while (window.isOpen()) {

        while (window.pollEvent(event)) {

                    // "close requested" event: we close the window
                    if (event.type == sf::Event::Closed)
                        window.close();
                    if (event.type == sf::Event::KeyPressed)
                    {
                        if (event.key.code == sf::Keyboard::Q or event.key.code == sf::Keyboard::W)
                        {
                            P.up();
                            int i=0;
                            for(auto &O : V)
                            {
                                if(P.hit(*O))
                                {
                                    delete O;
                                    V.erase(V.begin()+i);

                                }
                                i++;

                            }

                        }

                        if (event.key.code == sf::Keyboard::O or event.key.code == sf::Keyboard::P)
                        {
                            P.down();
                            int i=0;
                            for(auto &O : V)
                            {
                                if(P.hit(*O))
                                {
                                    delete O;
                                    V.erase(V.begin()+i);

                                }
                                i++;
                            }

                        }
                    }

    }
        //sf::Time elapsed = clock.restart();
        int i=0;
        for(auto &O : V)
        {
            if(P.collide(*O))
            {
                delete O;
                V.erase(V.begin()+i);

            }
            i++;
        }
    sf::Time elapsed = clock.restart();
    T=T+elapsed.asMicroseconds()/1000000.0;
    //cout<<T*1000000.0<<endl;
    //cout<<clock.getElapsedTime().asMicroseconds()<<endl;
    Moving K(1),L(0);

    while(E[ECounter].time<=T and ECounter<E.size())
    {
        switch(E[ECounter].type)
        {
        case('E'):
            //cout<< E[ECounter].time<<endl<<T<<endl<<endl;
                V.push_back(new Enemy(E[ECounter].lane));
                V[V.size()-1]->setTexture(&enemy1);
                V[V.size()-1]->Tex.push_back(&enemy1);
                V[V.size()-1]->Tex.push_back(&enemy2);
                V[V.size()-1]->Tex.push_back(&enemy3);
                V[V.size()-1]->Tex.push_back(&enemy4);
                break;
        case('H'):
                V.push_back(new Heart(E[ECounter].lane));
                V[V.size()-1]->setTexture(&heart1);
                V[V.size()-1]->Tex.push_back(&heart1);
                V[V.size()-1]->Tex.push_back(&heart2);
                V[V.size()-1]->Tex.push_back(&heart3);
                V[V.size()-1]->Tex.push_back(&heart4);
                break;

        case('N'):
                V.push_back(new Note(E[ECounter].lane));
                V[V.size()-1]->setTexture(&coin1);
                V[V.size()-1]->Tex.push_back(&coin1);
                V[V.size()-1]->Tex.push_back(&coin2);
                V[V.size()-1]->Tex.push_back(&coin3);
                V[V.size()-1]->Tex.push_back(&coin4);
                V[V.size()-1]->Tex.push_back(&coin5);
                break;
        }
        ECounter++;
    }

    if(P.health<=0)
    {
        window.close();
    }
    /*if(T>2.0)
    {
         V.push_back(new Enemy(1));
         V.push_back(new Heart(0));
         T=T-2.0;
         cout<<"K"<<endl;
    }*/


    text.setString(to_string(P.points)); 
    health.setString(to_string(P.health));
    health.setPosition(1920 - health.getLocalBounds().width, 0);
    combo.setString(to_string(P.combo));
    combo.setPosition((1920.0 - combo.getLocalBounds().width)/2,1080.0-combo.getLocalBounds().height);

    window.clear(sf::Color(23, 178, 255));
    //window.draw(b1);window.draw(b2);window.draw(b3);window.draw(b4);window.draw(b5);
    b1.animate(elapsed);
    b2.animate(elapsed);
    b3.animate(elapsed);
    b4.animate(elapsed);
    b5.animate(elapsed);
    window.draw(b5.back[0]);
    window.draw(b5.back[1]);
    window.draw(b4.back[0]);
    window.draw(b4.back[1]);
    window.draw(b3.back[0]);
    window.draw(b3.back[1]);
    window.draw(b2.back[0]);
    window.draw(b2.back[1]);
    window.draw(b1.back[0]);
    window.draw(b1.back[1]);
    window.draw(text);
    window.draw(health);
    window.draw(combo);
    P.animate(elapsed);
    window.draw(P);

    for(auto &rec : V) {
        rec->animate(elapsed);
        window.draw(*rec);
    }
    window.display();

    }
    return 0;
}
