#pragma once

#include "helpers.hpp"

class TortoiseAndHare
{
public:
    TortoiseAndHare(sf::RenderWindow& w, float dr, size_t nc):
        window(w),
        n_circles(nc),
        gen(1, nc-1),
        number_vector(nc),
        disk_radius(dr),
        circle_radius(2*M_PI*dr/(4*nc)),
        diff_angle(2*M_PI/nc),
        circle_shape(circle_radius)
    {
        circle_shape.setFillColor(sf::Color(20, 120, 20));
        circle_shape.setOrigin(circle_radius, circle_radius);
        circle_shape.setOutlineThickness(2);
        circle_shape.setOutlineColor(sf::Color(20, 70, 20));
        font.loadFromFile("arial.ttf");

        t.setString("0");
        t.setFont(font);
        t.setFillColor(sf::Color::Black);
        auto rect = t.getGlobalBounds();
        t.setOrigin(rect.width/2, rect.height/2);

        set_sprites();

        
    }

    TortoiseAndHare(sf::RenderWindow& w, float dr, std::vector<int> v):
        TortoiseAndHare(w, dr, v.size())
    {
        number_vector=std::move(v);
    }

    size_t get_width() const
    {
        return window.getSize().x;
    }
    size_t get_height() const
    {
        return window.getSize().y;
    }
    float cwidth() const
    {
        return get_width()/2.f;
    }
    float cheight() const
    {
        return get_height()/2.f;
    }
    const auto& get_vector() const
    {
        return number_vector;
    }

    void start_loop()
    {
        while (window.isOpen())
        {
            sf::Event event;
            while (window.pollEvent(event))
            {
                switch(event.type)
                {
                    case sf::Event::Closed:
                        window.close();
                        break;

                    case sf::Event::KeyPressed:
                        if (event.key.code == sf::Keyboard::Space)
                        {
                            moving = true;
                        }
                        break;
                }
            }
            window.clear(sf::Color(140, 136, 140));
            
            window.draw(hare);
            window.draw(tortoise);
            draw_arrows();            
            draw_circles();
            
            if (moving)
            {
                move();
            }
            window.draw(hare);
            window.draw(tortoise);
            window.display();
        }
    }

private:
    sf::Vector2f get_speed(size_t c1, size_t c2, float magnitude)
    {
        auto movement = get_circle_pos(c2)-get_circle_pos(c1);
        change_size_to(movement, magnitude);
        return movement;
    }
    void move()
    {
        static size_t hare_i=0;
        static size_t tortoise_i=0;
        static sf::Vector2f hare_movement = sf::Vector2f(0, 0);
        static sf::Vector2f tortoise_movement = sf::Vector2f(0, 0);
        static bool hare_first_circle = true;
        const static float hare_speed = 0.8f, tortoise_speed = 0.4f;
        //moving = false;
        //size_t hare_i_next = number_vector[number_vector[hare_i]];
        size_t hare_i_next = number_vector[hare_i];
        size_t tortoise_i_next = number_vector[tortoise_i];
        if (hare_movement == sf::Vector2f(0, 0) and tortoise_movement == sf::Vector2f(0, 0))
        {
            if(hare_i != hare_i_next)
            {
                hare_movement = get_speed(hare_i, hare_i_next, hare_speed);
            }
            if(tortoise_i != tortoise_i_next)
            {
                tortoise_movement = get_speed(tortoise_i, tortoise_i_next, tortoise_speed);
            }
            hare.setPosition(get_circle_pos(hare_i));
            tortoise.setPosition(get_circle_pos(tortoise_i));
            hare_i = hare_i_next;
            tortoise_i = tortoise_i_next;
        }
        hare.move(hare_movement);
        tortoise.move(tortoise_movement);
        if(distance(hare.getPosition(), get_circle_pos(hare_i))<hare_speed)
        {

            if (hare_first_circle and hare_i != hare_i_next)
            {
                hare_first_circle = false;
                hare_movement = get_speed(hare_i, hare_i_next, hare_speed);
                hare_i = hare_i_next;
            }
            else
            {
                hare_movement = sf::Vector2f(0, 0);
            }
            
        }
        if(distance(tortoise.getPosition(), get_circle_pos(tortoise_i))<tortoise_speed)
        {
            tortoise_movement = sf::Vector2f(0, 0);
        }

        if (hare_movement == sf::Vector2f(0, 0) and tortoise_movement == sf::Vector2f(0, 0))
        {
            hare_first_circle = true;
            moving = false;
        }
    }
    void set_sprites()
    {
        hareTexture.loadFromFile("hare.png");
        tortoiseTexture.loadFromFile("tortoise.png");
        hare.setTexture(hareTexture);
        hare.setColor(sf::Color(255, 200, 255, 200));
        hare.setScale(0.7, 0.7);
        auto rect = hare.getLocalBounds();
        hare.setOrigin(rect.width/2, rect.height/2);

        tortoise.setTexture(tortoiseTexture);
        tortoise.setColor(sf::Color(255, 200, 255, 200));
        tortoise.setScale(0.07, 0.07);
        rect = tortoise.getLocalBounds();
        tortoise.setOrigin(rect.width/2, rect.height/2);
    }
    sf::Vector2f get_circle_pos(size_t circle_index)
    {
        float angle = circle_index*diff_angle + M_PI_2;
        return {disk_radius*std::cos(angle)+cwidth(), disk_radius*std::sin(angle)+cheight()};
    }

    void draw_arrows()
    {
        int start = 0;
        for (size_t i=0; i<n_circles; i++)
        {
            sf::Vector2f pos1 = get_circle_pos(start);

            int next = number_vector[start];
            sf::Vector2f pos2 = get_circle_pos(next);
            auto v = pos2-pos1;
            if(start != next)
            {
                change_size_to(v, circle_radius+2);
                pos2 -= v;
                draw_arrow(window, pos1, pos2, hsv((360.f/n_circles)*i, 0.7f, 0.7f));
            }
            else
            {
                auto v = pos1 - sf::Vector2f{cwidth(), cheight()};
                change_size_to(v, circle_radius);
                sf::CircleShape curved_arrow(circle_radius);
                auto color = hsv((360.f/n_circles)*i, 0.7f, 0.7f);
                curved_arrow.setOutlineColor(color);
                curved_arrow.setOrigin(circle_radius, circle_radius);
                curved_arrow.setOutlineThickness(2);
                curved_arrow.setFillColor(sf::Color::Transparent);
                auto arrow_center = v+pos1;
                curved_arrow.setPosition(arrow_center);

                float deg15 = M_PI/3;
                sf::Vector2f arrow_tip = turn_vector(v, deg15);
                sf::Vertex arrow_tip_vertex = arrow_tip + pos1;
                arrow_tip_vertex.color = color;
                sf::Vector2f adjust_vector = turn_vector(arrow_tip, -M_PI/7);
                sf::Vertex far_vertex = 3.f*adjust_vector + pos1;
                far_vertex.color = color;
                auto triangle = get_triangle_for_arrow(far_vertex, arrow_tip_vertex);
                window.draw(triangle);
                window.draw(curved_arrow);
            }
            
            start = next;
        }
    }
    void draw_circles()
    {
        for (size_t i=0; i<n_circles; i++)
        {
            auto pos = get_circle_pos(i);
            circle_shape.setPosition(pos);
            t.setString(std::to_string(i));
            t.setPosition(pos);

            window.draw(circle_shape);
            window.draw(t);
        }
    }
    

    sf::RenderWindow& window;
    size_t n_circles;
    RandomGen<int> gen;
    std::vector<int> number_vector;
    float disk_radius;
    float circle_radius;
    float diff_angle;

    sf::CircleShape circle_shape;
    sf::Font font;
    sf::Text t;
    sf::Texture hareTexture, tortoiseTexture;
    sf::Sprite hare, tortoise;
    bool moving = false;
};