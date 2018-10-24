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

	void processExpression(const Expression& exp);

	void error(const QString& str);

	void clear();
private:
	QGraphicsScene* scene;
	QGraphicsView* view;

	// Helper functions to add text to the scene with default settings
	void addText(const std::string& str);
	void addText(const QString& str);

	// helper function to determine the time of object and delegate the object to other functions
	void handleObject(const Expression& exp, const std::string& objectName);
};

#endif
