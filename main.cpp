#include <SFML/Graphics.hpp>
#include <cstdlib>
#include <list>
#include <ctime>
#include <stdlib.h>
#include <iostream>

const int table_size = 600;
const int table_width = 5;
const int table_record_size = 25;
const int food_size = 5;



struct Point {
    int x, y;

    Point(int a = 0, int b = 0) {
        x = a;
        y = b;
    }

    bool operator==(Point b) const {
        return x == b.x && y == b.y;
    }
};

enum class Direction {
    Left = 0, Right = 1, Up = 2, Down = 3
};

bool in_radius(Point a, Point b, int r) {
    return  (a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y) <= r * r;
}
class Food : public sf::Drawable, public sf::Transformable {
public:
    Point food_point;

    void CreateFood() {
        std::srand(time(nullptr));
        int rnd = random();
        food_point.x = rnd % (table_size - table_width - food_size + 1) + table_width + food_size;
        food_point.y =
                rnd % (table_size - table_width - table_record_size - food_size + 1) + table_width + table_record_size +
                food_size;
        std::cout << food_point.x << " " << food_point.y << std::endl;
    }

    Food() {
        CreateFood();
    }

    virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const {
        states.transform *= getTransform();
        sf::CircleShape circle(5.f);
        circle.move(food_point.x, food_point.y);
        target.draw(circle, states);
    }
};

class Snake : public sf::Drawable, public sf::Transformable {
private:
    std::list<Point> snake_list;
    const int step = 2;
    const int size_rise = 10;
    int food_flag = size_rise;
    bool rotate = true;


public:
    Snake(int x = 300, int y = 300, int length =  40) {
        for (int i = x - length; i < x;i+=step) {
            Point p((i), y);
            snake_list.push_back(p);

        }
    }

    virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const {
        states.transform *= getTransform();
        for (auto i = snake_list.begin(); i != snake_list.end(); ++i) {
            sf::CircleShape circle(5.f);
            circle.move(i->x, i->y);
            target.draw(circle, states);
        }

    }

    Point GetNextPoint(Direction direction) {
        Point p = snake_list.front();
        switch (direction) {
            case Direction::Left:
                p.x -= step;
                break;
            case Direction::Right:
                p.x += step;
                break;
            case Direction::Up:
                p.y -= step;
                break;
            case Direction::Down:
                p.y += step;
                break;
        }
        return p;
    }

    void Move(Direction direction) {
        Point head = GetNextPoint(direction);
        snake_list.push_front(head);
        if (food_flag == size_rise) {
            snake_list.pop_back();
        } else {
            ++food_flag;
        }
        rotate = true;
    }

    void Eat() {
        food_flag = 0;
    }

    Point get_head() {
        return snake_list.front();
    }

    const std::list<Point> &get_snake() {
        return snake_list;
    }
};

class Game : public sf::Drawable, public sf::Transformable {
    Snake tvar;
    Food food;
    Direction direction_snake;

public:
    Game() {
        direction_snake = Direction::Up;
    }

    void change_direction(Direction direction_in) {
        if (direction_in == Direction::Left && direction_snake != Direction::Right) direction_snake = direction_in;
        if (direction_in == Direction::Right && direction_snake != Direction::Left) direction_snake = direction_in;
        if (direction_in == Direction::Up && direction_snake != Direction::Down) direction_snake = direction_in;
        if (direction_in == Direction::Down && direction_snake != Direction::Up) direction_snake = direction_in;
    }

    void check_food() {
        std::list<Point> snake_list = tvar.get_snake();
        for (auto i = snake_list.begin(); i != snake_list.end(); ++i) {
            if (*i == food.food_point) {
                food.CreateFood();
                i = snake_list.begin();
            }
        }
    }

    bool checker() {
        Point head = tvar.get_head();
        if (head.x < table_width || head.x > table_size - table_width)
            return false;
        if (head.y < table_record_size + table_width || head.y > table_size - table_width + table_record_size)
            return false;
        std::list<Point> snake_list = tvar.get_snake();
        int j = 0;
        for (auto i = snake_list.begin(); i != snake_list.end(); ++i, ++j) {
            if (in_radius(*i, head, food_size) && j > 5) {
                return false;
            }
        }
        return true;
    }

    void goo() {
        if (in_radius(food.food_point, tvar.get_head(), food_size + 1)) {
            std::cout << "Point";
            tvar.Eat();
            food.CreateFood();
            check_food();
//            (food.food_point.x + food_size > tvar.get_head().x
//             && food.food_point.x - food_size < tvar.get_head().x)
//            && (food.food_point.y + food_size > tvar.get_head().y
//                && food.food_point.y - food_size < tvar.get_head().y)
        }
        tvar.Move(direction_snake);
    }

    virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const {
        states.transform *= getTransform();
        tvar.draw(target, states);
        food.draw(target, states);
        sf::RectangleShape line_Up(sf::Vector2f(table_size, table_width));
        sf::RectangleShape line_Down(sf::Vector2f(table_size, table_width));
        sf::RectangleShape line_Left(sf::Vector2f(table_width, table_size));
        sf::RectangleShape line_Right(sf::Vector2f(table_width, table_size));
        line_Up.move(0, table_record_size);
        line_Down.move(0, table_size + table_record_size - table_width);
        line_Left.move(0, table_record_size);
        line_Right.move(table_size - table_width, table_record_size);
        target.draw(line_Up, states);
        target.draw(line_Down, states);
        target.draw(line_Left, states);
        target.draw(line_Right, states);
    }
};

int main() {
    sf::RenderWindow window(sf::VideoMode(table_size, table_size + table_record_size), "SNAKE");
    window.setFramerateLimit(60);
    Game game;
    sf::Event event;
    while (window.isOpen()) {
        game.goo();
        if (!game.checker()) window.close();
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) window.close();
            if (event.type == sf::Event::KeyPressed) {
                // Получаем нажатую клавишу - выполняем соответствующее действие
                if (event.key.code == sf::Keyboard::Escape) window.close();
                if (event.key.code == sf::Keyboard::Left) game.change_direction(Direction::Left);
                if (event.key.code == sf::Keyboard::Right) game.change_direction(Direction::Right);
                if (event.key.code == sf::Keyboard::Up) game.change_direction(Direction::Up);
                if (event.key.code == sf::Keyboard::Down) game.change_direction(Direction::Down);
                if (event.key.code == sf::Keyboard::A) game.change_direction(Direction::Left);
                if (event.key.code == sf::Keyboard::D) game.change_direction(Direction::Right);
                if (event.key.code == sf::Keyboard::W) game.change_direction(Direction::Up);
                if (event.key.code == sf::Keyboard::S) game.change_direction(Direction::Down);
                // Новая игра
                if (event.key.code == sf::Keyboard::F2) {
//                    game.Init();
                    std::cout << "LOL";
                }
            }
        }
        // Выполняем необходимые действия по отрисовке
        window.clear();
        window.draw(game);
//        window.draw(text);
        window.display();
    }
    return 0;
}
