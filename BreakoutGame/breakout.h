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
	static const int M_WIDTH = 300; // ������ �������
	static const int M_HEIGHT = 400; // ������ �������
	static const int M_PADDLE_STEP = 10; // ��� �������� �������
	static const int M_BRICKS_IN_WIDTH = 6; // ���������� �������� �� �����������
	static const int M_BRICKS_IN_HEIGHT = 5; // ���������� �������� �� ���������
	static const int M_PADDLE_Y_FROM_BOTTOM_BORDER = 50; // ���������� �� ������ ������� ������� �� �������
	static const int M_BRICK_Y_FROM_TOP_BORDER = 50; // ����������� �� ������� ������� ������� �� �������
	static const int M_DELAY = 10; // �������� ��� �������
private:
	void newGame(); // ������ ����� ����
	void startGame(); // ��������� ������
	void pauseGame(); // ���������� ����
	void ballMove(); // �������� ����
	void checkBallTouch(); // �������� �� ������������ ���� � ��������� � �������
	void paintGameField(QPainter* painter); // ������ ������� ����
	void paintText(QPainter* painter, qreal x, qreal y, QString text, QFont font, QColor pen, QColor brush); // ������ �����
private:
	void paintEvent(QPaintEvent* event); // ���������� ������� �����������
	void timerEvent(QTimerEvent* event); // ���������� ������� ���� �������
	void keyPressEvent(QKeyEvent* event); // ���������� ������� ������� �� ������
private:
	Item* m_paddle; // �������
	QList<Item*> m_bricks; // ������ � ��������� :)
	Item* m_ball; // �����
	int m_xdir; // ����������� �������� ������ �� ��� x
	int m_ydir; // ����������� �������� ������ �� ��� y
	int m_timerID; // ID �������
	int m_score; // ���� ������
	int m_score_mult; // �������������� ��� ���������� �����
	bool m_game_over; // ���� ��������
	bool m_paused; // ���� �� �����
	bool m_new_game; // ����� ����
};
