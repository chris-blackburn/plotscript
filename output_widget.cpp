#include "output_widget.hpp"

OutputWidget::OutputWidget(QWidget* parent): QWidget(parent) {
	scene = new QGraphicsScene(this);
	view = new QGraphicsView(scene);

	// This widget also needs a layout to put the view inside of
	auto layout = new QVBoxLayout;
	layout->addWidget(view);

	setLayout(layout);
}

void OutputWidget::addText(const std::string& str) {
	scene->addText(QString::fromStdString(str));
}

void OutputWidget::addText(const QString& str) {
	scene->addText(str);
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
						return scene->addEllipse(x, y, size, size, QPen(), QBrush(Qt::black))->rect();
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

std::string OutputWidget::getObjectName(const Expression& exp) const {
	return exp.getProperty("object-name").head().asSymbol(true);
}

void OutputWidget::handleObject(const Expression& exp, const std::string& objectName) {
	if (objectName == "point") {
		handlePointGraphic(exp);
	} else if (objectName == "line") {
		handleLineGraphic(exp);
	} else {
		throw SemanticError("Error: unknown object name");
	}
}

void OutputWidget::processExpression(const Expression& exp) {

	// lambda functions don't get printed out
	if (!exp.isHeadLambdaRoot()) {
		if (exp.head().isNone()) {
			addText((QString)"NONE");
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
					addText(out.str());
				}
			}
		}
	}
}

void OutputWidget::error(const QString& str) {
	addText(str);
}

void OutputWidget::clear() {
	scene->clear();
}
