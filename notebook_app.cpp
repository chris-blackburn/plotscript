#include "notebook_app.hpp"

#include <QLayout>

NotebookApp::NotebookApp(QWidget* parent): QWidget(parent) {
	input = new InputWidget(this);
	input->setObjectName("input");

	output = new OutputWidget(this);
	output->setObjectName("output");

	// Connect the input widget to the process slot of this class
	connect(input, &InputWidget::publish,
		this, &NotebookApp::process);

	// Setup the layout
	auto layout = new QVBoxLayout;
	layout->addWidget(input);
	layout->addWidget(output);

	setLayout(layout);
	setWindowTitle(tr("Plotscript Notebook"));
}

#include <QDebug>
#include <istream>
void NotebookApp::process(const QString& str) {
	std::istringstream expression(str.toStdString());

	if (!interp.parseStream(expression)) {
		output->error("Invalid Expression. Could not parse.");
	} else {
		try {

			// TODO: Should this get handled in output or should a simple string get handed to output?
			interp.evaluate();
		} catch(const SemanticError& ex) {
			output->error(ex.what());
		}
	}
}
