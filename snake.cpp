#include <SFML/Graphics.hpp>
#include <cmath> 

const int PART_SIZE = 10;

enum Directions {DIR_UP, DIR_DOWN, DIR_LEFT, DIR_RIGHT};

class Part {
    sf::RectangleShape rectangle;
    Part *back;
    Part *front;
    int x;
    int y;
    bool appended;
public:
    Part(int x, int y) {
        setCoord(x, y);
        this->rectangle.setSize(sf::Vector2f(PART_SIZE, PART_SIZE));
        this->rectangle.setOrigin(PART_SIZE/2, PART_SIZE/2);
        this->rectangle.setPosition(this->x, this->y);
        this->back = NULL;
        this->front = NULL;
    }
    void setCoord(int x, int y) {
        this->x = x;
        this->y = y;
    }
    void setBack(Part *back) {
        this->back = back;
    }
    void setFront(Part *front) {
        this->front = front;
    }
    void changePosition() {
        if(this->x > 790) {
            this->x = 10;
        }
        if(this->x < 10) {
            this->x = 790;
        }
        if(this->y > 590) {
            this->y = 10;
        }
        if(this->y < 10) {
            this->y = 590;
        }
        rectangle.setPosition(this->x, this->y);
    }
    void drawPart(sf::RenderWindow *window) {
        window->draw(this->rectangle);
    }
    Part *getBack() {
        return this->back;
    }
    Part *getFront() {
        return this->front;
    }
    int getX() { return this->x; }
    int getY() { return this->y; }
    void setAppended(bool appended) { this->appended = appended; }
    bool getAppended() { return this->appended; }
};

class Snake {
    sf::RenderWindow *window;
    Directions direction;
    Part *head;
    Part *tail;
    int dHead;
    double speed;
public:
    Snake(sf::RenderWindow *win) {
        direction = DIR_RIGHT;
        tail = new Part(480, 300);
        head = new Part(500, 300);
        head->setBack(tail);
        tail->setFront(head);
        tail->setAppended(true);
        head->setAppended(true);
        speed = 0.4;
        this->window = win;
    }
    void drawParts() {
        Part *currentPart = head;
        while(NULL != currentPart) {
            currentPart->drawPart(window);
            currentPart = currentPart->getBack();
        }
    }
    void update(float time, Part *food) {
        eat(food);
        dHead += time * speed;
        Part *beforeHead = head->getBack();
        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
            if(beforeHead->getY() >= head->getY())
                this->direction = DIR_UP;
        } else if(sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
            if(beforeHead->getY() <= head->getY())
                this->direction = DIR_DOWN;
        } else if(sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
            if(beforeHead->getX() <= head->getX())
                this->direction = DIR_RIGHT;
        } else if(sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
            if(beforeHead->getX() >= head->getX())
                this->direction = DIR_LEFT;
        }

        if(fabs(dHead) >= 10) {
            int headX = head->getX();
            int headY = head->getY();
            Part *newTail = this->tail->getFront();
            newTail->setBack(NULL);
            this->tail->setBack(head);
            this->head->setFront(tail);
            head = tail;
            head->setFront(NULL);
            tail = newTail;
            switch (direction) {
                case DIR_LEFT:
                    head->setCoord(headX - PART_SIZE, headY);
                    break;
                case DIR_RIGHT:
                    head->setCoord(headX + PART_SIZE, headY);
                    break;
                case DIR_UP:
                    head->setCoord(headX, headY - PART_SIZE);
                    break;
                case DIR_DOWN:
                    head->setCoord(headX, headY + PART_SIZE);
                    break;
            }
            dHead = 0;
            head->changePosition();
        }
    }

    void eat(Part *food) {
        int headX = head->getX();
        int headY = head->getY();

        Part *currentPart = head->getBack();
        while(currentPart) {
            if ((headX == currentPart->getX()) &&
                headY == currentPart->getY()) {
                speed = 0;
            }
            currentPart = currentPart->getBack();
        }

        if((headX == food->getX()) && headY == food->getY()) {
            food->setAppended(true);
            food->setCoord(tail->getX(), tail->getY());
            tail->setBack(food);
            food->setFront(tail);
            tail = food;
        }
    }
};

Part *createFood() {
    int foodX = rand() % 79 + 1;
    int foodY = rand() % 59 + 1;
    Part *food = new Part(foodX*10, foodY*10);
    food->setAppended(false);
    return food;
}


int main()
{
    sf::RenderWindow window(sf::VideoMode(800, 600), "Snake");
    window.setMouseCursorVisible(false);

    sf::Clock clock;
    float time;

    window.setVerticalSyncEnabled(true);
    Snake snake(&window);
    Part *food = createFood();

    while(window.isOpen()) {
        sf::Event event;
        while(window.pollEvent(event))
        {
            if(event.type == sf::Event::Closed ||
               event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)
                window.close();
        }

        window.clear();
        time = clock.getElapsedTime().asMilliseconds();

        snake.update(time, food);
        if(food->getAppended()) {
            food = createFood();
        }
        snake.drawParts();
        food->drawPart(&window);
        clock.restart();
        window.display();
    }
}

