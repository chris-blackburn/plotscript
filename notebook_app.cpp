#include "notebook_app.hpp"

#include <QLayout>

NotebookApp::NotebookApp(QWidget* parent): QWidget(parent) {
	input = new InputWidget(this);
	input->setObjectName("input");

	output = new OutputWidget(this);
	output->setObjectName("output");

	// Whenever the user sends an input, we need to process that input
	connect(input, &InputWidget::publish,
		this, &NotebookApp::process);

	// Setup the layout
	auto layout = new QVBoxLayout;
	layout->addWidget(input);
	layout->addWidget(output);

	setLayout(layout);
	setWindowTitle(tr("Plotscript Notebook"));
}

void NotebookApp::process(const QString& str) {

	// Clear the output widget then process the user's input
	output->clear();
	std::istringstream expression(str.toStdString());

	// Parse and process the user's expression
	if (!interp.parseStream(expression)) {
		output->error("Invalid Expression. Could not parse.");
	} else {
		try {
			Expression result = interp.evaluate();
			output->processExpression(result);
		} catch(const SemanticError& ex) {
			output->error(ex.what());
		}
	}
}
