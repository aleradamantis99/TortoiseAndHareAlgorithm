#include <SFML/Graphics.hpp>
#include <concepts>
#include <iostream>
#include <cmath>
#include <random>
#include <algorithm>


template <std::integral I>
auto find_duplicates(const std::vector<I>& v)
{
    I tortoise=0, hare=0;

    do
    {
        tortoise = v[tortoise];
        hare = v[v[hare]];
    } while(tortoise != hare);

    I ptr1 = 0;
    I ptr2 = hare;

    while(ptr1 != ptr2)
    {
        ptr1 = v[ptr1];
        ptr2 = v[ptr2];
    }
    return ptr2;
}

// hue: 0-360°; sat: 0.f-1.f; val: 0.f-1.f
sf::Color hsv(int hue, float sat, float val)
{
  hue %= 360;
  while(hue<0) hue += 360;

  if(sat<0.f) sat = 0.f;
  if(sat>1.f) sat = 1.f;

  if(val<0.f) val = 0.f;
  if(val>1.f) val = 1.f;

  int h = hue/60;
  float f = float(hue)/60-h;
  float p = val*(1.f-sat);
  float q = val*(1.f-sat*f);
  float t = val*(1.f-sat*(1-f));

  switch(h)
  {
    default:
    case 0:
    case 6: return sf::Color(val*255, t*255, p*255);
    case 1: return sf::Color(q*255, val*255, p*255);
    case 2: return sf::Color(p*255, val*255, t*255);
    case 3: return sf::Color(p*255, q*255, val*255);
    case 4: return sf::Color(t*255, p*255, val*255);
    case 5: return sf::Color(val*255, p*255, q*255);
  }
}

float vector_mod(sf::Vector2f v)
{
    return std::sqrt(v.x*v.x + v.y*v.y);
}

void change_size_to(sf::Vector2f& v, float new_size)
{
    float size = vector_mod(v);
    if (size==0)
    {
        throw std::invalid_argument("Can't change the size of Null Vector to " + std::to_string(new_size));
    }
    v *= new_size/vector_mod(v);
}
float distance(sf::Vector2f p1, sf::Vector2f p2)
{
    return vector_mod(p2-p1);
}
sf::VertexArray get_triangle_for_arrow(sf::Vertex p1, sf::Vertex p2, float triangle_height = 20.)
{
    sf::Vector2f v = p1.position-p2.position;
    change_size_to(v, triangle_height);
    sf::Vector2f u{v.y, -v.x};
    u *= (triangle_height*0.5f)/triangle_height;
    sf::VertexArray triangle_vertex(sf::Triangles, 3);
    sf::Vector2f mid_vertex = p2.position + v;

    triangle_vertex[0] = p2.position;
    triangle_vertex[0].color = p2.color;
    triangle_vertex[1] = mid_vertex+u;
    triangle_vertex[1].color = p2.color;
    triangle_vertex[2] = mid_vertex-u;
    triangle_vertex[2].color = p2.color;
    return triangle_vertex;
}

void draw_arrow(sf::RenderWindow& window, sf::Vector2f p1, sf::Vector2f p2, sf::Color c, float triangle_height = 20.)
{
    sf::VertexArray line(sf::Lines, 2);
    line[0] = p1;
    line[0].color = c;

    line[1] = p2;
    line[1].color = c;

    auto triangle = get_triangle_for_arrow(line[0], line[1], triangle_height);

    window.draw(line);
    window.draw(triangle);
}
template <std::integral I>
class RandomGen
{
public:
    RandomGen(I min, I max):
        dist(min, max),
        gen(std::random_device{}())
    {}

    I operator()()
    {
        return dist(gen);
    }

private:
    std::uniform_int_distribution<I> dist;
    std::mt19937 gen;
};

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
        circle_shape.setFillColor(sf::Color::Green);
        circle_shape.setOrigin(circle_radius, circle_radius);

        font.loadFromFile("arial.ttf");

        t.setString("0");
        t.setFont(font);
        t.setFillColor(sf::Color::Black);
        auto rect = t.getGlobalBounds();
        t.setOrigin(rect.width/2, rect.height/2);

        set_sprites();

        std::ranges::generate(number_vector, gen);
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
            window.clear();
            
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
        const static float hare_speed = 0.6f, tortoise_speed = 0.3f;
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
            if(v!=sf::Vector2f(0,0))
            {
                change_size_to(v, circle_radius+2);
            }
            
            pos2 -= v;
            draw_arrow(window, pos1, pos2, hsv((360.f/n_circles)*i, 0.7f, 0.7f));

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

int main()
{
    sf::Clock clock;

    size_t width = 800, height = 700;
    float cwidth = width/2.f, cheight = height/2.f;
    sf::RenderWindow window(sf::VideoMode(width, height), "SFML works!");
    
    TortoiseAndHare tah(window, 200, 6);
    auto v = tah.get_vector();
    for (int i: v)
    {
        std::cout << i << ", ";
    }
    std::cout << std::endl;

    for (int i=0; i<v.size(); i++)
    {
        std::cout << i << ", ";
    }
    std::cout << std::endl;

    std::cout << find_duplicates(v) << '\n';

    tah.start_loop();



    return 0;
}