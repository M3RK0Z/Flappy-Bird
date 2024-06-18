#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <fstream>

const int SCREEN_WIDTH = 800; ///< Szerokosc ekranu
const int SCREEN_HEIGHT = 750; ///< Wysokosc ekranu
const float GRAVITY = 0.25; ///< Grawitacja ptaka
const float JUMP_STRENGTH = -4.5; ///< Sila skoku ptaka
const int PIPE_WIDTH = 80; ///< Szerokosc rury
const int PIPE_HEIGHT = 400; ///< Wysokosc rury
const int PIPE_GAP = 200; ///< Wielkosc otworu miedzy rurami
const int BIRD_WIDTH = 54; ///< Szerokosc ptaka
const int BIRD_HEIGHT = 44; ///< Wysokosc ptaka
const int PIPE_SPACING = 380; ///< Odleglosc pomiedzy kolejnymi rurami
const int BUTTON_WIDTH = 200; ///< Szerokosc przycisku
const int BUTTON_HEIGHT = 50; ///< Wysokosc przycisku

/**
 * @class Bird
 * @brief Klasa reprezentujaca ptaka.
 */
class Bird {
public:
    float x, y, velY; ///< Pozycja i predkosc ptaka.
    ALLEGRO_BITMAP* image; ///< Obrazek ptaka.

    /**
     * @brief Konstruktor klasy Bird.
     * @param startX Poczatkowa pozycja X.
     * @param startY Poczatkowa pozycja Y.
     * @param img Obrazek ptaka.
     */
    Bird(float startX, float startY, ALLEGRO_BITMAP* img) : x(startX), y(startY), velY(0), image(img) {}

    /**
     * @brief Aktualizuje stan ptaka.
     */
    void update() {
        velY += GRAVITY; // Zwieksza predkosc ptaka o wartosc grawitacji
        y += velY; // Aktualizuje pozycje ptaka na podstawie predkosci
    }

    /**
     * @brief Sprawia, ze ptak skacze.
     */
    void jump() {
        velY = JUMP_STRENGTH; // Ustawia predkosc pionowa ptaka na wartosc skoku
    }

    /**
     * @brief Rysuje ptaka na ekranie.
     */
    void draw() {
        al_draw_scaled_bitmap(image, 0, 0, al_get_bitmap_width(image), al_get_bitmap_height(image), x, y, BIRD_WIDTH, BIRD_HEIGHT, 0);
    }

    /**
     * @brief Sprawdza kolizje ptaka z rura.
     * @param pipeX Pozycja X rury.
     * @param pipeY Pozycja Y rury.
     * @return true jesli wystepuje kolizja, false w przeciwnym razie.
     */
    bool is_colliding(float pipeX, float pipeY) {
        return (x + BIRD_WIDTH > pipeX && x < pipeX + PIPE_WIDTH) && (y < pipeY || y + BIRD_HEIGHT > pipeY + PIPE_GAP);
    }
};

/**
 * @class Pipe
 * @brief Klasa reprezentujaca rure.
 */
class Pipe {
public:
    float x, gapY; ///< Pozycja rury i wysokosc przerwy.
    bool passed; ///< Flaga oznaczajaca, czy rura zostala juz minieta przez ptaka.
    ALLEGRO_BITMAP* top_image; ///< Obrazek gornej rury.
    ALLEGRO_BITMAP* bottom_image; ///< Obrazek dolnej rury.

    /**
     * @brief Konstruktor klasy Pipe.
     * @param startX Poczatkowa pozycja X rury.
     * @param startY Poczatkowa wysokosc przerwy.
     * @param top_img Obrazek gornej rury.
     * @param bottom_img Obrazek dolnej rury.
     */
    Pipe(float startX, float startY, ALLEGRO_BITMAP* top_img, ALLEGRO_BITMAP* bottom_img)
        : x(startX), gapY(startY), passed(false), top_image(top_img), bottom_image(bottom_img) {}

    /**
     * @brief Aktualizuje pozycje rury.
     */
    void update() {
        x -= 3;
    }

    /**
     * @brief Rysuje rure na ekranie.
     */
    void draw() {
        al_draw_scaled_bitmap(top_image, 0, 0, al_get_bitmap_width(top_image), al_get_bitmap_height(top_image), x, gapY - al_get_bitmap_height(top_image), PIPE_WIDTH, al_get_bitmap_height(top_image), 0);
        al_draw_scaled_bitmap(bottom_image, 0, 0, al_get_bitmap_width(bottom_image), al_get_bitmap_height(bottom_image), x, gapY + PIPE_GAP, PIPE_WIDTH, al_get_bitmap_height(bottom_image), 0);
    }
};

/**
 * @brief Sprawdza, czy kursor myszy znajduje sie nad przyciskiem.
 * @param mouseX Pozycja X kursora myszy.
 * @param mouseY Pozycja Y kursora myszy.
 * @param buttonX Pozycja X przycisku.
 * @param buttonY Pozycja Y przycisku.
 * @param buttonWidth Szerokosc przycisku.
 * @param buttonHeight Wysokosc przycisku.
 * @return true jesli kursor jest nad przyciskiem, false w przeciwnym razie.
 */
bool is_mouse_over_button(int mouseX, int mouseY, int buttonX, int buttonY, int buttonWidth, int buttonHeight) {
    return mouseX >= buttonX && mouseX <= buttonX + buttonWidth && mouseY >= buttonY && mouseY <= buttonY + buttonHeight;
}

/**
 * @brief Glowna funkcja programu.
 * @return Funkcja zwraca 0 w przypadku poprawnego zakonczenia programu.
 * @note W funkcji inicjalizowane sa biblioteki Allegro.
 * @note Pobierane sa wszystkie wymagane obrazy, zrodla zdarzen oraz inne dane.
 * @note Na koncu programu nastepuje zwolnienie pamieci.
 * @warning Najlepiej nie zmieniac w tej funkcji zadnych wartosci, poniewaz moze to prowadzic do nieprawidlowego dzialania programu.
 */
int main() {
    if (!al_init()) {
        std::cerr << "Nie udalo sie zainicjalizowac Allegro!" << std::endl;
        return -1;
    }
    if (!al_init_image_addon()) {
        std::cerr << "Nie udalo sie zainicjalizowac dodatku obrazu Allegro!" << std::endl;
        return -1;
    }
    if (!al_install_keyboard()) {
        std::cerr << "Nie udalo sie zainicjalizowac wejscia klawiatury!" << std::endl;
        return -1;
    }
    if (!al_install_mouse()) {
        std::cerr << "Nie udalo sie zainicjalizowac wejscia myszy!" << std::endl;
        return -1;
    }
    if (!al_init_primitives_addon()) {
        std::cerr << "Nie udalo sie zainicjalizowac dodatku prymitywow Allegro!" << std::endl;
        return -1;
    }
    if (!al_init_font_addon()) {
        std::cerr << "Nie udalo sie zainicjalizowac dodatku czcionek Allegro!" << std::endl;
        return -1;
    }
    if (!al_init_ttf_addon()) {
        std::cerr << "Nie udalo sie zainicjalizowac dodatku TTF Allegro!" << std::endl;
        return -1;
    }

    ALLEGRO_DISPLAY* display = al_create_display(SCREEN_WIDTH, SCREEN_HEIGHT);
    if (!display) {
        std::cerr << "Nie udalo sie stworzyc wyswietlacza!" << std::endl;
        return -1;
    }

    ALLEGRO_TIMER* timer = al_create_timer(1.0 / 60.0);
    if (!timer) {
        std::cerr << "Nie udalo sie stworzyc timera!" << std::endl;
        al_destroy_display(display);
        return -1;
    }

    ALLEGRO_EVENT_QUEUE* event_queue = al_create_event_queue();
    if (!event_queue) {
        std::cerr << "Nie udalo sie stworzyc kolejki zdarzen!" << std::endl;
        al_destroy_timer(timer);
        al_destroy_display(display);
        return -1;
    }

    ALLEGRO_BITMAP* image[4] = { NULL };

    image[0] = al_load_bitmap("bird.png");
    if (!image[0]) {
        std::cerr << "Nie udalo sie zaladowac obrazka ptaka!" << std::endl;
        al_destroy_event_queue(event_queue);
        al_destroy_timer(timer);
        al_destroy_display(display);
        return -1;
    }

    image[1] = al_load_bitmap("background.png");
    if (!image[1]) {
        std::cerr << "Nie udalo sie zaladowac obrazka tla!" << std::endl;
        al_destroy_event_queue(event_queue);
        al_destroy_timer(timer);
        al_destroy_display(display);
        return -1;
    }

    image[2] = al_load_bitmap("pipe_top.png");
    if (!image[2]) {
        std::cerr << "Nie udalo sie zaladowac obrazka gornej rury!" << std::endl;
        al_destroy_event_queue(event_queue);
        al_destroy_timer(timer);
        al_destroy_display(display);
        return -1;
    }

    image[3] = al_load_bitmap("pipe_bottom.png");
    if (!image[3]) {
        std::cerr << "Nie udalo sie zaladowac obrazka dolnej rury!" << std::endl;
        al_destroy_event_queue(event_queue);
        al_destroy_timer(timer);
        al_destroy_display(display);
        return -1;
    }

    ALLEGRO_FONT* font = al_load_font("arial.ttf", 36, 0);
    if (!font) {
        std::cerr << "Nie udalo sie zaladowac czcionki!" << std::endl;
        al_destroy_bitmap(image[0]);
        al_destroy_bitmap(image[1]);
        al_destroy_bitmap(image[2]);
        al_destroy_bitmap(image[3]);
        al_destroy_event_queue(event_queue);
        al_destroy_timer(timer);
        al_destroy_display(display);
        return -1;
    }

    srand(static_cast<unsigned int>(time(0)));

    Bird bird(SCREEN_WIDTH / 4, SCREEN_HEIGHT / 2, image[0]); // Tworzy obiekt ptaka
    std::vector<Pipe> pipes; // Wektor przechowujacy rury

    int initial_pipe_y = rand() % (SCREEN_HEIGHT - PIPE_GAP - 50) + 25;
    pipes.emplace_back(SCREEN_WIDTH, initial_pipe_y, image[2], image[3]);

    al_register_event_source(event_queue, al_get_display_event_source(display));
    al_register_event_source(event_queue, al_get_timer_event_source(timer));
    al_register_event_source(event_queue, al_get_keyboard_event_source());
    al_register_event_source(event_queue, al_get_mouse_event_source());

    bool redraw = true; // Flaga sygnalizujaca, czy ekran powinien zostac odswiezony
    bool doexit = false; // Flaga sygnalizujaca zakonczenie gry
    bool game_over = false; // Flaga oznaczajaca, czy gra sie skonczyla
    bool game_started = false; // Flaga oznaczajaca, czy gra jest w toku
    int score = 0; // Wynik gracza
    int best_score = 0; // Najwyzszy wynik

    std::ifstream score_file("best_score.txt"); // Otwiera plik z najwyzszym wynikiem
    if (score_file.is_open()) {
        score_file >> best_score; // Odczytuje najwyzszy wynik z pliku
        score_file.close();
    }

    al_start_timer(timer); // Uruchamia timer

    while (!doexit) {
        ALLEGRO_EVENT ev;
        al_wait_for_event(event_queue, &ev);

        if (ev.type == ALLEGRO_EVENT_TIMER) {
            if (!game_started) {
                // Nie aktualizuj stanu gry, gdy gra nie jest rozpoczeta
            }
            else if (!game_over) { // Aktualizacja stanu gry, gdy gra jest w toku
                bird.update();

                if (bird.y > SCREEN_HEIGHT || bird.y < 0) game_over = true; // Sprawdzanie, czy ptak nie wylecial poza ekran

                for (auto& pipe : pipes) { // Aktualizacja stanu rur
                    pipe.update();
                    if (pipe.x + PIPE_WIDTH < 0) {
                        pipes.erase(pipes.begin()); // Usuwanie rur, ktore wyszly poza ekran
                    }
                    if (bird.is_colliding(pipe.x, pipe.gapY)) game_over = true; // Sprawdzanie kolizji ptaka z rurami
                }

                for (auto& pipe : pipes) { // Aktualizacja wyniku
                    if (!game_over && pipe.x + PIPE_WIDTH < bird.x && !pipe.passed) {
                        score++;
                        pipe.passed = true; // Oznaczanie rur, ktore ptak minal
                    }
                }

                if (pipes.empty() || pipes.back().x < SCREEN_WIDTH - PIPE_SPACING) { // Dodawanie nowych rur
                    int new_pipe_y = rand() % (SCREEN_HEIGHT - PIPE_GAP - 50) + 25;
                    pipes.emplace_back(SCREEN_WIDTH, new_pipe_y, image[2], image[3]);
                }
            }

            redraw = true; // Ustawianie flagi odswiezania ekranu
        }
        else if (ev.type == ALLEGRO_EVENT_DISPLAY_CLOSE) { // Zdarzenie zamkniecia wyswietlacza
            doexit = true; // Ustawianie flagi zakonczenia gry
        }
        else if (ev.type == ALLEGRO_EVENT_KEY_DOWN) { // Zdarzenie nacisniecia klawisza
            if (ev.keyboard.keycode == ALLEGRO_KEY_SPACE) { // Jesli nacisnieto klawisz spacji
                if (!game_started) {
                    game_started = true; // Rozpoczecie gry
                }
                if (!game_over) {
                    bird.jump(); // Sprawianie, ze ptak podskakuje
                }
                else {
                    game_started = false; // Resetuj stan gry
                    game_over = false; // Resetuj stan zakonczenia gry
                    bird = Bird(SCREEN_WIDTH / 4, SCREEN_HEIGHT / 2, image[0]); // Resetuj pozycje ptaka
                    pipes.clear();
                    int initial_pipe_y = rand() % (SCREEN_HEIGHT - PIPE_GAP - 50) + 25; // Wygeneruj poczatkowa pozycje rury
                    pipes.emplace_back(SCREEN_WIDTH, initial_pipe_y, image[2], image[3]); // Dodaj nowa rure
                    if (score > best_score) { // Sprawdz, czy obecny wynik jest najlepszym wynikiem
                        best_score = score; // Zaktualizuj najlepszy wynik
                        std::ofstream score_file("best_score.txt");
                        if (score_file.is_open()) {
                            score_file << best_score; // Zapisz najlepszy wynik do pliku
                            score_file.close();
                        }
                    }
                    score = 0; // Zresetuj wynik
                }
            }
        }
        else if (ev.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) { // Obsluga zdarzenia klikniecia przycisku myszy
            int mouseX = ev.mouse.x; // Pobierz wspolrzedna X klikniecia myszy
            int mouseY = ev.mouse.y; // Pobierz wspolrzedna Y klikniecia myszy

            if (!game_started) { // Jesli gra nie jest rozpoczeta
                // Sprawdz klikniecia w menu
                int buttonX = SCREEN_WIDTH / 2 - BUTTON_WIDTH / 2; // Oblicz pozycje X przyciskow menu
                int startY = SCREEN_HEIGHT / 2 - 100; // Pozycja Y przycisku Start
                int exitY = startY + BUTTON_HEIGHT + 20; // Pozycja Y przycisku Wyjscie

                if (is_mouse_over_button(mouseX, mouseY, buttonX, startY, BUTTON_WIDTH, BUTTON_HEIGHT)) {
                    game_started = true; // Rozpocznij gre po kliknieciu przycisku Start
                }
                else if (is_mouse_over_button(mouseX, mouseY, buttonX, exitY, BUTTON_WIDTH, BUTTON_HEIGHT)) {
                    doexit = true; // Zakoncz gre po kliknieciu przycisku Wyjscie
                }
            }
            else {
                if (!game_over) { // Jesli gra jest w toku
                    bird.jump(); // Sprawianie, ze ptak podskakuje
                }
                else {
                    // Sprawdz klikniecia w przycisk powrotu do menu
                    int buttonX = SCREEN_WIDTH / 2 - BUTTON_WIDTH / 2;
                    int menuY = SCREEN_HEIGHT / 2 + 100;

                    if (is_mouse_over_button(mouseX, mouseY, buttonX, menuY, BUTTON_WIDTH, BUTTON_HEIGHT)) {
                        game_started = false; // Resetuj stan gry
                        game_over = false;
                        bird = Bird(SCREEN_WIDTH / 4, SCREEN_HEIGHT / 2, image[0]); // Resetuj pozycje ptaka
                        pipes.clear();
                        int initial_pipe_y = rand() % (SCREEN_HEIGHT - PIPE_GAP - 50) + 25;  // Wygeneruj poczatkowa pozycje rury
                        pipes.emplace_back(SCREEN_WIDTH, initial_pipe_y, image[2], image[3]); // Dodaj nowa rure
                        if (score > best_score) { // Sprawdz, czy obecny wynik jest najlepszym wynikiem
                            best_score = score; // Zaktualizuj najlepszy wynik
                            std::ofstream score_file("best_score.txt"); // Otworz plik z najlepszym wynikiem
                            if (score_file.is_open()) {
                                score_file << best_score; // Zapisz najlepszy wynik do pliku
                                score_file.close();
                            }
                        }
                        score = 0; // Zresetuj wynik
                    }
                }
            }
        }

        if (redraw && al_is_event_queue_empty(event_queue)) { // Jesli ekran wymaga odswiezenia i kolejka zdarzen jest pusta
            redraw = false; // Zresetuj flage odswiezenia

            al_clear_to_color(al_map_rgb(0, 0, 0)); // Wyczysc ekran do koloru czarnego

            if (!game_started) { // Jesli gra nie jest rozpoczeta
                al_draw_scaled_bitmap(image[1], 0, 0, al_get_bitmap_width(image[1]), al_get_bitmap_height(image[1]), 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0); // Rysuj tlo
                al_draw_text(font, al_map_rgb(255, 255, 255), SCREEN_WIDTH / 2, SCREEN_HEIGHT / 4 - 100, ALLEGRO_ALIGN_CENTRE, "Flappy Bird"); // Rysuj tytul gry

                int buttonX = SCREEN_WIDTH / 2 - BUTTON_WIDTH / 2; // Oblicz pozycje X przyciskow menu
                int startY = SCREEN_HEIGHT / 2 - 100; // Pozycja Y przycisku Start
                int exitY = startY + BUTTON_HEIGHT + 20; // Pozycja Y przycisku Wyjscie

                al_draw_filled_rectangle(buttonX, startY, buttonX + BUTTON_WIDTH, startY + BUTTON_HEIGHT, al_map_rgb(255, 0, 0)); // Rysuj przycisk Start
                al_draw_text(font, al_map_rgb(255, 255, 255), buttonX + BUTTON_WIDTH / 2, startY + BUTTON_HEIGHT / 4, ALLEGRO_ALIGN_CENTRE, "Start"); // Rysuj tekst na przycisku Start

                al_draw_filled_rectangle(buttonX, exitY, buttonX + BUTTON_WIDTH, exitY + BUTTON_HEIGHT, al_map_rgb(255, 0, 0)); // Rysuj przycisk Wyjscie
                al_draw_text(font, al_map_rgb(255, 255, 255), buttonX + BUTTON_WIDTH / 2, exitY + BUTTON_HEIGHT / 4, ALLEGRO_ALIGN_CENTRE, "Wyjscie"); // Rysuj tekst na przycisku Wyjscie
            }
            else if (game_over) { // Jesli gra jest zakonczona
                al_draw_scaled_bitmap(image[1], 0, 0, al_get_bitmap_width(image[1]), al_get_bitmap_height(image[1]), 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0); // Rysuj tlo
                for (auto& pipe : pipes) {
                    pipe.draw(); // Rysuj rury
                }
                bird.draw(); // Rysuj ptaka
                al_draw_textf(font, al_map_rgb(0, 0, 0), SCREEN_WIDTH / 2, SCREEN_HEIGHT / 4, ALLEGRO_ALIGN_CENTRE, "Koniec gry! Wynik: %d", score); // Rysuj wynik koncowy
                al_draw_textf(font, al_map_rgb(0, 0, 0), SCREEN_WIDTH / 2, SCREEN_HEIGHT / 4 + 50, ALLEGRO_ALIGN_CENTRE, "Najlepszy wynik: %d", best_score); // Rysuj najlepszy wynik

                int buttonX = SCREEN_WIDTH / 2 - BUTTON_WIDTH / 2; // Oblicz pozycje X przycisku Menu
                int menuY = SCREEN_HEIGHT / 2 + 100; // Pozycja Y przycisku Menu

                al_draw_filled_rectangle(buttonX, menuY, buttonX + BUTTON_WIDTH, menuY + BUTTON_HEIGHT, al_map_rgb(255, 0, 0)); // Rysuj przycisk Menu
                al_draw_text(font, al_map_rgb(255, 255, 255), buttonX + BUTTON_WIDTH / 2, menuY + BUTTON_HEIGHT / 4, ALLEGRO_ALIGN_CENTRE, "Menu"); // Rysuj tekst na przycisku Menu
            }
            else { // Jesli gra jest w toku
                al_draw_scaled_bitmap(image[1], 0, 0, al_get_bitmap_width(image[1]), al_get_bitmap_height(image[1]), 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0); // Rysuj tlo
                for (auto& pipe : pipes) {
                    pipe.draw(); // Rysuj rury
                }
                bird.draw(); // Rysuj ptaka
                al_draw_textf(font, al_map_rgb(255, 255, 255), SCREEN_WIDTH / 2, 50, ALLEGRO_ALIGN_CENTRE, "Wynik: %d", score); // Rysuj biezacy wynik
            }

            al_flip_display(); // Przelacz bufory wyswietlania, aby zaktualizowac ekran
        }
    }

    al_destroy_bitmap(image[0]);
    al_destroy_bitmap(image[1]);
    al_destroy_bitmap(image[2]);
    al_destroy_bitmap(image[3]);
    al_destroy_font(font);
    al_destroy_timer(timer);
    al_destroy_event_queue(event_queue);
    al_destroy_display(display);

    return 0;
}

