#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <iostream>
#include <ctime>
#include <cstdlib>

using namespace std;

class Entity: public sf::RectangleShape
{
public:
    int lane;
    virtual void animate(const sf::Time &elapsed)=0;
};

class Moving: public Entity
{
public:
    int lane;
    Moving(int L)
    {
        double y;
        if(L) y=540;
        else y=700;
        setPosition(1820,y);
        sf::Vector2f S(100.0, 100.0);
        setSize(S);
        setFillColor(sf::Color(255, 0, 0));
        lane =L;
    }
    void animate(const sf::Time &elapsed)
    {
        double time=float((elapsed.asMicroseconds())/1000000.0);
        move(time*-333.3,0.0);
    }
};

class Player: public Entity
{
public:
    Player()
    {
        setPosition(480,700);
        sf::Vector2f S(100.0, 100.0);
        setSize(S);
        setFillColor(sf::Color(0, 255, 0));
        lane=0;
    }
    int health=100;
    void up()
    {
        setPosition(480,540);
        lane=1;
    }
    void down()
    {
        setPosition(480,700);
        lane=0;
    }
    void animate(const sf::Time &elapsed)
    {
        sf::Vector2f V=getPosition();
        if(V.y<700)
        {
            double time=float((elapsed.asMicroseconds())/1000000.0);
            move(0.0,time*222.2);
            //cout<<time*100.0<<endl;
            if(V.y<600) lane=0;
        }
        if(V.y>700) setPosition(480,700);
    }
    bool hit(Moving &M)
    {
        sf::Vector2f V=M.getPosition();
        if(lane==M.lane)
        {
            if(V.x>420 and V.x<580)
            {
                M.setFillColor(sf::Color(100, 100, 100));
                return 1;
            }
            else return 0;
        }
        return 0;
    }
};

class Enemy: public Moving
{
public:
    int hit=10;
};

class Heart: public Moving
{
public:
};

int main()
{
    sf::RenderWindow window(sf::VideoMode(1920 , 1080), "My window");
    sf::Clock clock;
    sf::Event event;
    Player P;
    Moving M(1);
    vector <Moving> V;
    double T=0;
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
                            P.hit(M);
                            for(auto &O : V)
                            {
                                P.hit(O);
                            }

                        }

                        if (event.key.code == sf::Keyboard::O or event.key.code == sf::Keyboard::P)
                        {
                            P.down();
                            P.hit(M);
                            for(auto &O : V)
                            {
                                P.hit(O);
                            }

                        }
                    }

    }
    sf::Time elapsed = clock.restart();
    T=T+elapsed.asMilliseconds()/100.0;
    //cout<<T<<endl;
    Moving K(1),L(0);
    if(T>0.5)
    {
         V.push_back(K);
         V.push_back(L);
         T=T-0.5;
    }

    window.clear(sf::Color::Black);
    M.animate(elapsed);
    window.draw(M);
    P.animate(elapsed);
    window.draw(P);
    for(auto &rec : V) {
        rec.animate(elapsed);
        window.draw(rec);
    }
    window.display();


    }
}
