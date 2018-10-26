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
		auto x = exp.tailConstBegin();
		auto y = exp.tailConstEnd() - 1;

		// if x != y, then there are only two expression in the list. Also check if both expressions
		// are numbers that are positive
		if (x != y && (x->isHeadNumber() && y->isHeadNumber()) &&
			(x->head().asNumber() >= 0 && y->head().asNumber() >= 0)) {

				// verify the size parameter of the point object
				Expression size = exp.getProperty("size");
				if (size.isHeadNumber() && size.head().asNumber() >= 0) {

					// Create the graphic and add it to the scene
					scene->addEllipse(x->head().asNumber(), y->head().asNumber(),
						size.head().asNumber(), size.head().asNumber(),
						QPen(), QBrush(Qt::black));
					return;
				}

				error("Error: invalid size property");
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
