#ifndef OUTPUT_WIDGET_HPP
#define OUTPUT_WIDGET_HPP

#include <QWidget>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsItem>
#include <QLayout>

#include "expression.hpp"
#include "semantic_error.hpp"

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
	QGraphicsTextItem* addText(const std::string& str);
	QGraphicsTextItem* addText(const QString& str);

	// helper function to determine the time of object and delegate the object to other functions
	std::string getObjectName(const Expression& exp) const;
	void handleObject(const Expression& exp, const std::string& objectName);

	// helper functions to help with graphics
	QRectF handlePointGraphic(const Expression& exp, bool addToScene = true);
	void handleLineGraphic(const Expression& exp);
	void handleTextGraphic(const Expression& exp);
};

#endif
