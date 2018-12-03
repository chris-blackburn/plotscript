#include "output_widget.hpp"

#include <cmath>
const double PI = std::atan2(0, -1);

OutputWidget::OutputWidget(QWidget* parent): QWidget(parent) {
	scene = new QGraphicsScene(this);
	view = new QGraphicsView(scene);

	// This widget also needs a layout to put the view inside of
	auto layout = new QVBoxLayout;
	layout->addWidget(view);

	// disable scroll bars on the view
	view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	view->setCacheMode(QGraphicsView::CacheNone);

	setLayout(layout);
}

QGraphicsTextItem* OutputWidget::addText(const QString& str) {
	return scene->addText(str, QFont("Courier", 1));
}

QRectF OutputWidget::handlePointGraphic(const Expression& exp, bool addToScene) {

	// The expression should be a list of coordinates
	if (exp.isHeadListRoot()) {

		// There should be two numbers acting as the coordinates
		auto xExp = exp.tailConstBegin();
		auto yExp = exp.tailConstEnd() - 1;

		// if x != y, then there are only two expression in the list. Also check if both expressions
		// are numbers
		if (xExp != yExp && (xExp->isHeadNumber() && yExp->isHeadNumber())) {

				// verify the size parameter of the point object
				Expression sizeExp = exp.getProperty("size");
				if (sizeExp.isHeadNumber() && sizeExp.head().asNumber() >= 0) {
					qreal size = sizeExp.head().asNumber();

					// We want the point to be centered at the entered coordinates
					qreal x = xExp->head().asNumber() - (size / 2);
					qreal y = yExp->head().asNumber() - (size / 2);

					// Create the graphic and add it to the scene if addToScene is true. This function
					// defaults to adding it, but is also used by line graphics - in that case we don't
					// want to add it to the scene
					if (addToScene) {

						// Make sure to set the pen width to zero to not interfere with the size
						return scene->addEllipse(x, y, size, size, QPen(QBrush(Qt::black), size),
							QBrush(Qt::black))->rect();
					}

					// If addToScene is false, just return an rect item with the correct proportions
					return QRectF(x, y, size, size);
				}

				throw SemanticError("Error: invalid size of point object");
		}

		throw SemanticError("Error: invalid coordinates of point object");
	}

	throw SemanticError("Error: invalid point object");
}

void OutputWidget::handleLineGraphic(const Expression& exp) {
	if (exp.isHeadListRoot()) {

		// Get the coordinate data from the two points in the list
		auto aExp = exp.tailConstBegin();
		auto bExp = exp.tailConstEnd() - 1;

		// verify that both items in the list are point objects
		if (aExp != bExp && (getObjectName(*aExp) == "point" && getObjectName(*bExp) == "point")) {

			// verify the thickness parameter of the line object
			Expression thicknessExp = exp.getProperty("thickness");
			if (thicknessExp.isHeadNumber() && thicknessExp.head().asNumber() >= 0) {
				qreal thickness = thicknessExp.head().asNumber();

				// get the point object rectangular parameters
				QRectF aRect = handlePointGraphic(*aExp, false);
				QRectF bRect = handlePointGraphic(*bExp, false);

				scene->addLine(aRect.left(), aRect.top(), bRect.left(), bRect.top(),
					QPen(QBrush(Qt::black), thickness));
				return;
			}

			throw SemanticError("Error: invalid thickness of line object");
		}

		throw SemanticError("Error: invalid coordinates of line object");
	}

	throw SemanticError("Error: invalid line object");
}

void OutputWidget::handleTextGraphic(const Expression& exp) {
	if (exp.isHeadStringLiteral()) {

		// Grab the text expression (no quotes)
		std::string str = exp.head().asSymbol(true);

		// verify the position parameter of the text object
		Expression posExp = exp.getProperty("position");
		if (getObjectName(posExp) == "point") {
			QRectF posRect = handlePointGraphic(posExp, false);

			// Get the optional scale property
			Expression scaleExp = exp.getProperty("text-scale");
			double scale = scaleExp.head().asNumber();
			if (scale < 1) {
				scale = 1;
			}

			// Get the optional rotation property
			Expression rotExp = exp.getProperty("text-rotation");
			double rot = rotExp.isHeadNumber() ? rotExp.head().asNumber() : 0;

			// Create the text and update its position
			auto text = addText(QString::fromStdString(str));

			// Create the formatted text
			qreal x = posRect.left() - (text->boundingRect().width() / 2);
			qreal y = posRect.top() - (text->boundingRect().height() / 2);
			text->setPos(x, y);
			text->setTransformOriginPoint(text->boundingRect().center());
			text->setRotation(rot * (180 / PI));
			text->setScale(scale);
			return;
		}

		throw SemanticError("Error: invalid position of text object");
	}

	throw SemanticError("Error: invalid text object");
}

std::string OutputWidget::getObjectName(const Expression& exp) const {
	return exp.getProperty("object-name").head().asSymbol(true);
}

void OutputWidget::handleObject(const Expression& exp, const std::string& objectName) {
	if (objectName == "point") {
		handlePointGraphic(exp);
	} else if (objectName == "line") {
		handleLineGraphic(exp);
	} else if (objectName == "text") {
		handleTextGraphic(exp);
	} else {
		throw SemanticError("Error: unknown object name");
	}
}

void OutputWidget::processExpression(const Expression& exp) {

	// lambda functions don't get printed out
	if (!exp.isHeadLambdaRoot()) {
		if (exp.head().isNone()) {
			addText("NONE");
		} else {

			// First, check if the expression has an object-name property that matches one of
			// the graphic primitive types
			Expression objectName = exp.getProperty("object-name");
			if (!objectName.head().isNone()) {
				try {
					handleObject(exp, getObjectName(exp));
				} catch (const SemanticError& ex) {
					error(ex.what());
				}
			} else {
				if (exp.isHeadListRoot()) {

					// As of now, items in lists just get printed out on top of each other. We just recurse
					// through and add everything to the scene
					for (auto e = exp.tailConstBegin(); e != exp.tailConstEnd(); e++) {
						processExpression(*e);
					}
				} else {

					// Everything else prints the same as the REPL, the head is just wrapped in
					// a set of parentheses
					std::stringstream out;
					out << "(" << exp.head() << ")";
					addText(QString::fromStdString(out.str()));
				}
			}
		}
	}
}

void OutputWidget::error(const QString& str) {
	addText("Error: " + str);
}

void OutputWidget::clear() {
	scene->clear();
}

void OutputWidget::scale() {
	scene->setSceneRect(scene->itemsBoundingRect());
	view->fitInView(scene->sceneRect(), Qt::KeepAspectRatio);
}

void OutputWidget::resizeEvent(QResizeEvent* event) {
	scale();
	QWidget::resizeEvent(event);
}
