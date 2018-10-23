#ifndef OUTPUT_WIDGET_HPP
#define OUTPUT_WIDGET_HPP

#include <QWidget>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QLayout>

#include "expression.hpp"

class OutputWidget: public QWidget {
Q_OBJECT
public:
	OutputWidget(QWidget* parent = nullptr);

	void addText(const QString& str);

	void processExpression(const Expression& exp);

	void error(const QString& str);

	void clear();
private:
	QGraphicsScene* scene;
	QGraphicsView* view;
};

#endif
