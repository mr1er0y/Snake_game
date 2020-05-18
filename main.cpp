#include <SFML/Graphics.hpp>
#include <list>
#include <ctime>
#include <stdlib.h>
#include <iostream>
#include <string>

const int table_size = 600; // Размер игрового поля
const int table_width = 5; // Ширина линий поля
const int table_record_size = 25; //Размер таблички для счета
const int food_size = 5; // Размер еды для змеки
const int step = 2; // Шаг змейки по полю
const int size_rise = 10; // Размер роста змейки


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
    // Функция проверяет находится ли одна точка в окружности другой точки
    return (a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y) <= r * r;
}

class Food : public sf::Drawable, public sf::Transformable {
public:
    Point food_point;

    void CreateFood() {
        // Создает точку в пределах игрового поля
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
        // Рисует точку
        states.transform *= getTransform();
        sf::CircleShape circle(5.f);
        circle.move(food_point.x, food_point.y);
        circle.setFillColor(sf::Color(255, 0, 0));
        target.draw(circle, states);
    }
};

class Snake : public sf::Drawable, public sf::Transformable {
private:
    std::list<Point> snake_list;
    int food_flag = size_rise;

public:
    Snake(int x = table_size / 2, int y = table_size / 2, int length = 40) {
        // Создает змейку
        for (int i = x - length; i < x; i += step) {
            Point p((i), y);
            snake_list.push_back(p);

        }
    }

    virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const {
        // Отрисовывает змейку
        states.transform *= getTransform();
        for (auto i = snake_list.begin(); i != snake_list.end(); ++i) {
            sf::CircleShape circle(5.f);
            circle.move(i->x, i->y);
            circle.setFillColor(sf::Color(49, 127, 67));
            target.draw(circle, states);
        }

    }

    Point GetNextPoint(Direction direction) {
        // Задает направлене движение змейки
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
            // Останавлиает удаление хвоста змейки
            snake_list.pop_back();
        } else {
            ++food_flag;
        }
    }

    void Eat() {
        // Запускает рост змеки
        food_flag = 0;
    }

    Point get_head() {
        // Возвращает голову змейку
        return snake_list.front();
    }

    const std::list<Point> &get_snake() {
        // Возвращает список  координат  змейки
        return snake_list;
    }
};

class Game : public sf::Drawable, public sf::Transformable {
    Snake tvar;
    Food food;
    Direction direction_snake;
    int score = 0;

public:
    Game() {
        direction_snake = Direction::Up;
    }

    void change_direction(Direction direction_in) {
        // Меняет направление движение змейки
        if (direction_in == Direction::Left && direction_snake != Direction::Right) direction_snake = direction_in;
        if (direction_in == Direction::Right && direction_snake != Direction::Left) direction_snake = direction_in;
        if (direction_in == Direction::Up && direction_snake != Direction::Down) direction_snake = direction_in;
        if (direction_in == Direction::Down && direction_snake != Direction::Up) direction_snake = direction_in;
    }

    void check_food() {
        // Проверяет появление еды на змейке
        std::list<Point> snake_list = tvar.get_snake();
        for (auto i = snake_list.begin(); i != snake_list.end(); ++i) {
            if (*i == food.food_point) {
                food.CreateFood();
                i = snake_list.begin();
            }
        }
    }

    bool checker() {
        // Функция проверяет змейку на столкновение с собой же и стенами
        Point head = tvar.get_head();
        if (head.x < table_width || head.x >= table_size - table_width * 2)
            return false;
        if (head.y < table_record_size + table_width || head.y > table_size - table_width * 2 + table_record_size)
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
        // Функция отвечает за перемещение змейки. Проверяет съела ли змека еду
        if (in_radius(food.food_point, tvar.get_head(), food_size + 1)) {
//            std::cout << "Point";
            tvar.Eat();
            food.CreateFood();
            check_food();
            score += 10;
        }
        tvar.Move(direction_snake);
    }

    virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const {
        // Отрисовка игры
        states.transform *= getTransform();
        tvar.draw(target, states);
        food.draw(target, states);
        // Отрисовка поля
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

    int get_score() {
        return score;
    }
};

int main() {
    sf::RenderWindow window(sf::VideoMode(table_size, table_size + table_record_size), "SNAKE");
    window.setFramerateLimit(60);
    Game game;
    sf::Event event;

    sf::Font font;
    if (!font.loadFromFile("Arial.ttf")) {
        return 1;
    }
    sf::Text txt;
    txt.setPosition(0, 0);
    txt.setFont(font);
    txt.setCharacterSize(20);
    txt.setColor(sf::Color::White);
    txt.setStyle(sf::Text::Bold);
    while (window.isOpen()) {
        game.goo();
        if (!game.checker()) window.close();
        txt.setString("SCORE  "  + std::to_string(game.get_score()));
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
        window.draw(txt);
        window.display();
    }
    return 0;
}
