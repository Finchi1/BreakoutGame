#pragma once
#include <qpoint.h>
#include <qimage.h>
class Item
{
public:
	Item()
		: m_image(QImage()), m_cords(QPoint(0, 0))
	{}
public:
	// �������/�������
	QPoint getCords() { return m_cords; }
	void setCords(QPoint cords) { m_cords = cords; }
	QImage& getImage() { return m_image; }
	void setImage(QImage image) { m_image = image; }
private:
	QPoint m_cords; // ���������� ��� �������� ��������� ��������
	QImage m_image; // ���������� ��� �������� ����������� ��������
};

