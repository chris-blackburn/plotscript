#include "notebook_app.hpp"

#include <QLayout>

NotebookApp::NotebookApp(QWidget* parent): QWidget(parent) {
	input = new InputWidget(this);
	input->setObjectName("input");

	output = new OutputWidget(this);
	output->setObjectName("output");

	// Connect the input widget to the output widget
	connect(input, &InputWidget::publish,
		output, &OutputWidget::update);

	// Setup the layout
	auto layout = new QVBoxLayout;
	layout->addWidget(input);

	setLayout(layout);
	setWindowTitle(tr("Plotscript Notebook"));
}
