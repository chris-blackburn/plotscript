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

void OutputWidget::handlePointGraphic(const Expression& exp) {

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
					double size = sizeExp.head().asNumber();

					// We want the point to be centered at the entered coordinates
					double x = xExp->head().asNumber() - (size / 2);
					double y = yExp->head().asNumber() - (size / 2);

					// Create the graphic and add it to the scene
					scene->addEllipse(x, y, size, size, QPen(), QBrush(Qt::black));
					return;
				}

				error("Error: invalid size of point object");
				return;
		}

		error("Error: invalid coordinates of point object");
		return;
	}

	error("Error: invalid point object");
}

void OutputWidget::handleObject(const Expression& exp, const std::string& objectName) {
	if (objectName == "point") {
		handlePointGraphic(exp);
	} else {
		error("Error: unknown object name");
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
				handleObject(exp, objectName.head().asSymbol(true));
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
