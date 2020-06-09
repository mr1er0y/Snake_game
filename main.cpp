#include <SFML/Graphics.hpp>
#include <list>
#include <ctime>
#include <iostream>
#include <string>

const int table_size = 600; ///< Размер игрового поля
const int table_width = 5; ///< Ширина линий поля
const int table_record_size = 25; ///< Размер таблички для счета
const int food_size = 5; ///< Размер еды для змеки
const int step = 2; ///< Шаг змейки по полю
const int size_rise = 10; ///< Размер роста змейки
const int shake_width = 5; ///< Ширина змейки

/*!
	\brief Структура Точка

	Данная структура содержит координаты x и y
*/
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

/// Набор всех возможных наравлений
enum class Direction {
    Left = 0, ///<  Лево
    Right = 1, ///<  Право
    Up = 2, ///< Верх
    Down = 3 ///< Низ
};

/*!
Функция проверяет находится ли одна точка в окружности другой точки
\param[in] a Координаты первой точки
\param[in] b Координаты второй точки
\param[in] r Радиус точки
\return возвращает true если одна из точек находится в окружности другой точки, false в ином случае
*/
bool in_radius(const Point &a, const Point &b, const int &r) {
    return (a.x - b.x) * (a.x - b.x) + (a.y - b.y) * (a.y - b.y) <= r * r;
}

/*!
	\brief Класс еды

	Данный класс предназначен для работы с едой для змейки.
    Наследует sf::Drawable, sf::Transformable для рисования еды
*/
class Food : public sf::Drawable, public sf::Transformable {
public:
    Point food_point;///< Координаты еды

    /*!
    Создает точку в пределах игрового поля
    */
    void CreateFood() {
        std::srand(time(nullptr));
        int rnd = random();
        food_point.x = rnd % (table_size - table_width - food_size + 1) + table_width + food_size;
        food_point.y =
                rnd % (table_size - table_width - table_record_size - food_size + 1) + table_width + table_record_size +
                food_size;
    }

    Food() {
        CreateFood();
    }

    /*!
        Рисует еду
    */
    virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const {
        states.transform *= getTransform();
        sf::CircleShape circle(5.f);
        circle.move(food_point.x, food_point.y);
        circle.setFillColor(sf::Color(255, 0, 0));
        target.draw(circle, states);
    }
};

/*!
	\brief Класс перемещаемого объекта

    Данный класс предназначен для работы с перемещаемым объектом - змейкой.
    Наследует sf::Drawable, sf::Transformable для рисования еды
*/
class Snake : public sf::Drawable, public sf::Transformable {
private:
    std::list<Point> snake_list; ///< лист координат змейки
    int food_flag = size_rise;

public:
    /*!
      Создает змейку
    */
    Snake(int x = table_size / 2, int y = table_size / 2, int length = 40) {
        for (int i = x - length; i < x; i += step) {
            Point p((i), y);
            snake_list.push_back(p);

        }
    }

    /*!
    Отрисовывает змейку
    */
    virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const {
        states.transform *= getTransform();
        for (auto i : snake_list) {
            sf::CircleShape circle(5.f);
            circle.move(i.x, i.y);
            circle.setFillColor(sf::Color(49, 127, 67));
            target.draw(circle, states);
        }

    }

    /*!
    Функция задает направлене движения объекта
    \param[in] direction направление объекта
    \return координату следующей точки
    */
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

    /*!
    Данная функция отвечает за перемещения объекта
    \param[in] direction направление объекта
    */
    void Move(const Direction &direction) {
        Point head = GetNextPoint(direction);
        snake_list.push_front(head);
        if (food_flag == size_rise) {
            /// Останавлиает удаление хвоста змейки
            snake_list.pop_back();
        } else {
            ++food_flag;
        }
    }

    /*!
    Запускает рост змеки
    */
    void Eat() {
        food_flag = 0;
    }

    /*!
    Возвращает голову змейку
    \return координату начала объекта
    */
    Point get_head() {
        return snake_list.front();
    }

    /*!
    Возвращает список  координат  змейки
    \return список  координат  объекта
    */
    const std::list<Point> &get_snake() {
        return snake_list;
    }
};

/*!
	\brief


*/
class Game : public sf::Drawable, public sf::Transformable {
    Snake tvar; ///< сам объект - змейка
    Food food; ///< еда для змейки
    Direction direction_snake; ///< направление змейки
    int score = 0; ///< счет игры

public:
    Game() {
        direction_snake = Direction::Up;
    }

    /*!
     Меняет направление движение змейки
    */
    void change_direction(Direction direction_in) {
        if (direction_in == Direction::Left && direction_snake != Direction::Right) direction_snake = direction_in;
        if (direction_in == Direction::Right && direction_snake != Direction::Left) direction_snake = direction_in;
        if (direction_in == Direction::Up && direction_snake != Direction::Down) direction_snake = direction_in;
        if (direction_in == Direction::Down && direction_snake != Direction::Up) direction_snake = direction_in;
    }

    /*!
       Проверяет появление еды на змейке
    */
    void check_food() {
        std::list<Point> snake_list = tvar.get_snake();
        for (auto i = snake_list.begin(); i != snake_list.end(); ++i) {
            if (in_radius(*i, food.food_point, food_size + shake_width)) {
                food.CreateFood();
                i = snake_list.begin();
            }
        }
    }

    /*!
     Функция проверяет змейку на столкновение с собой же и стенами
    \return false если объект врезается в стены или сам в себя, true если все хорошо
    */
    bool checker() {
        Point head = tvar.get_head();
        if (head.x + shake_width < table_width || head.x + shake_width >= table_size - table_width)
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

    /*!
    Функция отвечает за перемещение змейки. Проверяет съела ли змека еду
    */
    void goo() {
        if (in_radius(food.food_point, tvar.get_head(), food_size + shake_width)) {
            tvar.Eat();
            food.CreateFood();
            check_food();
            score += 10;
        }
        tvar.Move(direction_snake);
    }

    /*!
    Функция возвращает текущий счет
    \return текущий счет
    */
    int get_score() {
        return score;
    }

    /*!
    Рисование всех игровых объектов
    */
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
        txt.setString("SCORE  " + std::to_string(game.get_score()));
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

