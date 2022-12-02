#pragma once
#include <qwidget.h>
#include "item.h"
class Breakout :
	public QWidget
{
public:
	Breakout(QWidget* parent = nullptr);
	~Breakout();
private:
	static const int M_WIDTH = 300; // ширина виджета
	static const int M_HEIGHT = 400; // высота виджета
	static const int M_PADDLE_STEP = 10; // шаг движения ракетки
	static const int M_BRICKS_IN_WIDTH = 6; // количество кирпичей по горизонтали
	static const int M_BRICKS_IN_HEIGHT = 5; // количество кирпичей по вертикали
	static const int M_PADDLE_Y_FROM_BOTTOM_BORDER = 50; // расстояние от нижней границы виджета до ракетки
	static const int M_BRICK_Y_FROM_TOP_BORDER = 50; // расстоянеие от верхней границы виджета до кирпича
	static const int M_DELAY = 10; // задержка для таймера
private:
	void newGame(); // начать новую игру
	void startGame(); // запустить таймер
	void pauseGame(); // остановить игру
	void ballMove(); // движение мяча
	void checkBallTouch(); // проверка на столкновение мяча с объектами и стенами
	void paintGameField(QPainter* painter); // рисует игровое поле
	void paintText(QPainter* painter, qreal x, qreal y, QString text, QFont font, QColor pen, QColor brush); // рисует текст
private:
	void paintEvent(QPaintEvent* event); // обработчик события перерисовки
	void timerEvent(QTimerEvent* event); // обработчик события тика таймера
	void keyPressEvent(QKeyEvent* event); // обработчик события нажатия на кнопку
private:
	Item* m_paddle; // ракетка
	QList<Item*> m_bricks; // список с кирпичами :)
	Item* m_ball; // мячик
	int m_xdir; // направление движения мячика по оси x
	int m_ydir; // направление движения мячика по оси y
	int m_timerID; // ID таймера
	int m_score; // счет отчков
	int m_score_mult; // мультипликатор для начисления очков
	bool m_game_over; // игра окончена
	bool m_paused; // игра на паузе
	bool m_new_game; // новая игра
};
