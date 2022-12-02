#include "breakout.h"
#include <qpainter.h>
#include <QKeyEvent>
#include <QPainterPath>
Breakout::Breakout(QWidget* parent)
	: QWidget(parent), m_paddle(), m_bricks(), m_ball(), m_xdir(0), m_ydir(0), m_timerID(0), m_score(0),
	m_score_mult(1), m_game_over(false), m_paused(0), m_new_game(true)
	// инициализируем родительский класс и переменные-члены
{
	this->setFixedSize(M_WIDTH, M_HEIGHT); // устанавливаем фиксированный размер виджета
	this->setStyleSheet("background-color: lightyellow"); // устанавливаем фон игрового поля
	newGame(); // начинаем новую игру
}

Breakout::~Breakout()
{
	// Освобождаем выделенную память
	delete m_paddle;
	for (auto& brick : m_bricks)
		delete brick;
	m_bricks.clear();
	delete m_ball;
}

void Breakout::newGame()
{
	// Освобождаем память перед новым выделением
	if (m_paddle != nullptr)
		delete m_paddle;
	for (auto& brick : m_bricks)
		if (brick != nullptr)
		{
			delete brick;
			brick = nullptr;
		}
	m_bricks.clear();
	if (m_ball != nullptr)
		delete m_ball;
	// Если был запущен таймер, то остановим его
	if (m_timerID != 0)
		killTimer(m_timerID);
	// Сбрасываем значения всех переменных-членов
	m_timerID = 0;
	m_score = 0;
	m_score_mult = 1;
	m_game_over = false;
	m_paused = false;
	m_new_game = true;
	// Выделяем память под объект ракетки, задаем ему изображение и начальные координаты
	m_paddle = new Item();
	QImage paddle_image("paddle.png");
	m_paddle->setImage(paddle_image);
	m_paddle->setCords(QPoint(M_WIDTH / 2 - paddle_image.width() / 2, M_HEIGHT - M_PADDLE_Y_FROM_BOTTOM_BORDER));
	// Выделяем память под объекты кирпичей, задаем им изображение и начальные координаты
	QImage brick_image("brickie.png");
	qreal widget_width = this->width();
	for (int h = 0; h < M_BRICKS_IN_HEIGHT; h++)
	{
		int y = M_BRICK_Y_FROM_TOP_BORDER + h * brick_image.height();
		for (int w = 0; w < M_BRICKS_IN_WIDTH; w++)
		{
			m_bricks.push_back(new Item());
			m_bricks.at(m_bricks.size() - 1)->setImage(brick_image);
			int x;
			// Рассчитываем расположение кирпичей по горизонтали для корректного отображения их по центру
			if (M_BRICKS_IN_WIDTH % 2 == 0) // если кирпичей в горизонтали четное число
				x = widget_width / 2 - M_BRICKS_IN_WIDTH / 2 * brick_image.width() + w * brick_image.width();
			else // если нечетное
				x = widget_width / 2 - M_BRICKS_IN_WIDTH / 2 * brick_image.width() - brick_image.width() / 2 + w * brick_image.width();
			m_bricks.at(m_bricks.size() - 1)->setCords(QPoint(x, y));
		}
	}
	// Выделяем память пож объект мячика, задаем ему изображение, начальные координаты и направления движения
	m_ball = new Item();
	m_ball->setImage(QImage("ball.png"));
	m_ball->setCords(QPoint(m_paddle->getCords().x() + paddle_image.width() / 2, m_paddle->getCords().y() - paddle_image.height() / 2));
	m_xdir = 1;
	m_ydir = -1;
}

// Запускаем таймер, который начнет движения мячика
void Breakout::startGame()
{
	if (m_timerID == 0)
	{
		m_timerID = startTimer(M_DELAY);
		m_paused = false;
		m_new_game = false;
	}
}

// Останавливаем игру на паузу
void Breakout::pauseGame()
{
	killTimer(m_timerID);
	m_timerID = 0;
	m_paused = true;
}

// Движение мячика по игровому полю
void Breakout::ballMove()
{
	int add_x = 0;
	int add_y = 0;
	if (m_xdir == 1)
		add_x++;
	else if (m_xdir == -1)
		add_x--;
	if (m_ydir == 1)
		add_y++;
	else if (m_ydir == -1)
		add_y--;
	QPoint temp = m_ball->getCords();
	int x = temp.x() + add_x;
	int y = temp.y() + add_y;
	m_ball->setCords(QPoint(x, y));
}

// Обрабатываем столкновения мячика с объектами игрового поля и стенами
void Breakout::checkBallTouch()
{
	// Получиаем информацию о координатах и размере мячика и размерах игрового поля
	QPoint ball_cords = m_ball->getCords();
	QImage ball_image = m_ball->getImage();
	int ball_width = ball_image.width();
	int ball_height = ball_image.height();
	int widget_width = this->width();
	int widget_height = this->height();
	// Проверяем на столкновение мячика со стенами, если такое происходит, то меняем ему направление
	if (ball_cords.x() + ball_width > widget_width)
		m_xdir = -1;
	if (ball_cords.x() < 0)
		m_xdir = 1;
	if (ball_cords.y() < 0)
		m_ydir = 1;
	// Если мячик "упал" за нижнюю стенку виджета, то игра закончилась
	if (ball_cords.y() > widget_height)
	{
		killTimer(m_timerID);
		m_game_over = true;
	}
	// Получаем информацию о координатах и размерах ракетки
	QPoint paddle_cords = m_paddle->getCords();
	QImage paddle_image = m_paddle->getImage();
	int paddle_width = paddle_image.width();
	int paddle_height = paddle_image.height();
	// Проверяем на столкновение мячика с ракеткой
	if ((ball_cords.y() + ball_height > paddle_cords.y()) &&
		(ball_cords.y() + ball_height < paddle_cords.y() + paddle_height) &&
		ball_cords.x() + ball_width / 2 > paddle_cords.x() &&
		ball_cords.x() - ball_width / 2 < paddle_cords.x() + paddle_width)
	{
		m_ydir = -1; // меняем направление движения мячика по оси Y
		// Если мячик попал в центр ракетки
		if (ball_cords.x() + ball_width / 2 > paddle_cords.x() + paddle_width / 2 - ball_width / 2 &&
			ball_cords.x() + ball_width / 2 < paddle_cords.x() + paddle_width / 2 + ball_width / 2)
		{
			m_xdir = 0; // меняем направление движения мячика по оси X
			m_score_mult = 1; // сбрасываем мультипликатор на 1
		}
		// Если мячик попал в правую часть ракетки
		else if (ball_cords.x() + ball_width / 2 > paddle_cords.x() + paddle_width / 2)
		{
			m_xdir = 1; // меняем направление движения мячика по оси X
			m_score_mult = 1; // сбрасываем мультипликатор на 1
		}
		// Если мячик попал в левую часть ракетки
		else
		{
			m_xdir = -1; // меняем направление движения мячика по оси X
			m_score_mult = 1; // сбрасываем мультипликатор на 1
		}
	}
	// Проверяем на столкновение мячика с кирпичами
	for (size_t brick = 0; brick < m_bricks.size(); brick++)
	{
		// Получаем информацию про текущий кирпич
		QPoint brick_cords = m_bricks.at(brick)->getCords();
		QImage brick_image = m_bricks.at(brick)->getImage();
		int brick_width = brick_image.width();
		int brick_height = brick_image.height();
		// Проверяем коснулся ли мячик кирпича снизу-вверх, если коснулся, то начисляем очки, меняем направление движение мячика и удаляем этот кирпич
		if (m_ydir == -1 && ball_cords.y() < brick_cords.y() + brick_height &&
			ball_cords.y() > brick_cords.y() &&
			ball_cords.x() + ball_width > brick_cords.x() &&
			ball_cords.x() < brick_cords.x() + brick_width)
		{
			m_ydir = 1; // меняем направление движения мячика по оси Y
			m_score += 10 * m_score_mult; // добавляем очки
			m_score_mult *= 2; // умножаем мультипликатор
			if (ball_cords.x() + ball_width / 2 < brick_cords.x() + brick_width / 2) // если мячик попал в левую часть кирпича
				m_xdir = -1; // меняем направление движения мячика по оси X
			else // если мячик попал в правую часть кирпича
				m_xdir = 1; // меняем направление движения мячика по оси X
			Item* temp = m_bricks.at(brick); // сохраним указатель во временной переменной для очистки памяти
			m_bricks.remove(brick); // удалим кирпич из списка
			delete temp; // очистим память
		}
		// Проверяем коснулся ли мячик кирпича сверху-вниз, если коснулся, то начисляем очки, меняем направление движение мячика и удаляем этот кирпич
		else if (m_ydir == 1 && ball_cords.y() + ball_height > brick_cords.y() &&
			ball_cords.y() + ball_height < brick_cords.y() + brick_height &&
			ball_cords.x() + ball_width > brick_cords.x() &&
			ball_cords.x() < brick_cords.x() + brick_width)
		{
			m_ydir = -1; // меняем направление движения мячика по оси Y
			m_score += 20 * m_score_mult; // добавляем очки
			m_score_mult *= 2; // умножаем мультипликатор
			if (ball_cords.x() + ball_width / 2 < brick_cords.x() + brick_width / 2) // если мячик попал в левую часть кирпича
				m_xdir = -1; // меняем направление движения мячика по оси X
			else // если мячик попал в правую часть кирпича
				m_xdir = 1; // меняем направление движения мячика по оси X
			Item* temp = m_bricks.at(brick); // сохраним указатель во временной переменной для очистки памяти
			m_bricks.remove(brick); // удалим кирпич из списка
			delete temp; // очистим память
		}
	}
}

// Рисуем основные элементы игрового поля: ракетку, кирпичи, мячик
void Breakout::paintGameField(QPainter* painter)
{
	painter->drawImage(m_paddle->getCords(), m_paddle->getImage());
	for (auto& brick : m_bricks)
		painter->drawImage(brick->getCords(), brick->getImage());
	painter->drawImage(m_ball->getCords(), m_ball->getImage());
}

// Рисуем текст
void Breakout::paintText(QPainter* painter, qreal x, qreal y, QString text, QFont font, QColor pen, QColor brush)
{
	QFontMetrics metrics(font);
	qreal text_width = metrics.horizontalAdvance(text);
	QPainterPath path;
	path.addText(x, y, font, text);
	painter->setPen(QPen(pen));
	painter->setBrush(QBrush(brush));
	painter->drawPath(path);
}

// Обработчик события перерисовки виджета
void Breakout::paintEvent(QPaintEvent* event)
{
	Q_UNUSED(event); // указываем компилятору на неиспользуемую переменную
	qreal widget_width = this->width(); // получаем ширину виджета
	qreal widget_height = this->height(); // получаем высоту виджета
	QPainter* painter = new QPainter(this);
	painter->setRenderHint(QPainter::Antialiasing); // установим сглаживание
	// Если игра не закончилась, то рисуем игровое поле и отображаем количество очков вверху
	if (!m_game_over && m_bricks.size() > 0)
	{
		paintGameField(painter);
		QString text("Score: %1");
		text = text.arg(m_score);
		QFont font("Times New Roman", 14);
		QFontMetrics metrics(font);
		qreal text_width = metrics.horizontalAdvance(text);
		qreal text_height = metrics.height();
		paintText(painter, widget_width / 2 - text_width / 2, M_PADDLE_Y_FROM_BOTTOM_BORDER / 2, text, font, QColor("red"), QColor("red"));
		// Если игра еще не началась, то рисуем ниже ракетки текст с подсказкой как ее начать
		if (m_new_game)
		{
			QString text("To game begin click \"Space\"");
			QFont font("Times New Roman", 10);
			QFontMetrics metrics(font);
			qreal text_width = metrics.horizontalAdvance(text);
			qreal text_height = metrics.height();
			paintText(painter, widget_width / 2 - text_width / 2, widget_height - static_cast<qreal>(M_PADDLE_Y_FROM_BOTTOM_BORDER) / 2 + text_height, text, font, QColor("orange"), QColor("black"));
		}
	}
	// Если игра закончилась, а все кирпичи небыли уничтожены, то выводим надпись "Game Over" и количество заработанных очков
	else if (m_bricks.size() > 0)
	{
		QString text("Game Over");
		QString score("your score: %1");
		score = score.arg(m_score);
		QFont text_font("Times New Roman", 35);
		QFont score_font("Times New Roman", 14);
		QFontMetrics text_metrics(text_font);
		QFontMetrics score_metrics(score_font);
		qreal text_width = text_metrics.horizontalAdvance(text);
		qreal score_width = score_metrics.horizontalAdvance(score);
		qreal score_height = score_metrics.height();
		paintText(painter, widget_width / 2 - text_width / 2, widget_height / 2, text, text_font, QColor("red"), QColor("black"));
		paintText(painter, widget_width / 2 - score_width / 2, widget_height / 2 + score_height, score, score_font, QColor("red"), QColor("red"));
	}
	// Если все кирпичи уничтоженны, то выводим надпись "You win!" и количество заработанных очков
	else
	{
		QString text("You win!");
		QString score("your score: %1");
		score = score.arg(m_score);
		QFont text_font("Times New Roman", 35);
		QFont score_font("Times New Roman", 14);
		QFontMetrics text_metrics(text_font);
		QFontMetrics score_metrics(score_font);
		qreal text_width = text_metrics.horizontalAdvance(text);
		qreal score_width = score_metrics.horizontalAdvance(score);
		qreal score_height = score_metrics.height();
		paintText(painter, widget_width / 2 - text_width / 2, widget_height / 2, text, text_font, QColor("black"), QColor("red"));
		paintText(painter, widget_width / 2 - score_width / 2, widget_height / 2 + score_height, score, score_font, QColor("red"), QColor("red"));
	}
	// Если игра на паузе, то выводим подсказку как ее убрать
	if (m_paused)
	{
		QString text("To game continue click \"Space\", or \"Escape\", or \"P\"");
		QFont font("Times New Roman", 10);
		QFontMetrics metrics(font);
		qreal text_width = metrics.horizontalAdvance(text);
		qreal text_height = metrics.height();
		qreal widget_height = this->height();
		paintText(painter, widget_width / 2 - text_width / 2, widget_height - static_cast<qreal>(M_PADDLE_Y_FROM_BOTTOM_BORDER) / 2 + text_height, text, font, QColor("orange"), QColor("black"));
	}
}

// Обработчик события тиков таймера
void Breakout::timerEvent(QTimerEvent* event)
{
	Q_UNUSED(event); // указываем компилятору на неиспользуемую переменную
	ballMove(); // двигаем мячик
	checkBallTouch(); // проверяем столкновения мячика со стенами и объекатами игрового поля
	this->repaint(); // перерисовываем виджет
}

// Обработчик события нажатия клавиш
void Breakout::keyPressEvent(QKeyEvent* event)
{
	int key = event->key(); // получаем нажатие клавиши
	// Если нажата клавиша "Влево"
	if (key == Qt::Key_Left)
	{
		QPoint paddle_cords = m_paddle->getCords();
		int x_paddle = paddle_cords.x();
		// Перемещаем ракетку вправо
		if (x_paddle > 0)
		{
			x_paddle -= M_PADDLE_STEP;
			paddle_cords.setX(x_paddle);
			m_paddle->setCords(paddle_cords);
			// А если игра еще не началась, то вместе с ракеткой двигаем и мячик
			if (m_new_game)
			{
				QPoint ball_cords = m_ball->getCords();
				int x_ball = ball_cords.x();
				x_ball -= M_PADDLE_STEP;
				ball_cords.setX(x_ball);
				m_ball->setCords(ball_cords);
			}
		}
	}
	// Если нажата клавиша "Вправо"
	else if (key == Qt::Key_Right)
	{
		QPoint paddle_cords = m_paddle->getCords();
		int paddle_x = paddle_cords.x();
		// Перемещаем ракетку вправо
		if (paddle_x < M_WIDTH - m_paddle->getImage().width())
		{
			paddle_x += M_PADDLE_STEP;
			paddle_cords.setX(paddle_x);
			m_paddle->setCords(paddle_cords);
			// А если игра еще не началась, то вместе с ракеткой двигаем и мячик
			if (m_new_game)
			{
				QPoint ball_cords = m_ball->getCords();
				int x_ball = ball_cords.x();
				x_ball += M_PADDLE_STEP;
				ball_cords.setX(x_ball);
				m_ball->setCords(ball_cords);
			}
		}
	}
	// Если нажат "Пробел"
	else if (key == Qt::Key_Space)
	{
		startGame(); // запускаем новую игру, или снимаем игру с паузы
	}
	// Если нажата клавиша "Escape" или "P" во время игры
	else if ((key == Qt::Key_P || key == Qt::Key_Escape) && !m_new_game)
	{
		if (m_timerID != 0) // если игра не на паузе
			pauseGame(); // то ставим на паузу
		else // если игра на паузе
			startGame(); // то снимаем с паузы
	}
	// Если нажата клавиша "N", то начинаем новую игру
	else if (key == Qt::Key_N)
		newGame();
	this->repaint(); // перерисовываем виджет
}