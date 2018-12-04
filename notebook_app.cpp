#include "notebook_app.hpp"

#include <QLayout>

#include "interrupt_flag.hpp"

void NotebookApp::initButtons() {
	kernelControlButtons = new QHBoxLayout;

	// Create the interpreter kernel control buttons
	startBtn = new QPushButton("Start Kernel", this);
	stopBtn = new QPushButton("Stop Kernel", this);
	resetBtn = new QPushButton("Reset Kernel", this);
	interruptBtn = new QPushButton("Interrupt", this);

	// place them in the layout
	kernelControlButtons->addWidget(startBtn);
	kernelControlButtons->addWidget(stopBtn);
	kernelControlButtons->addWidget(resetBtn);
	kernelControlButtons->addWidget(interruptBtn);

	// set the button names
	startBtn->setObjectName("start");
	stopBtn->setObjectName("stop");
	resetBtn->setObjectName("reset");
	interruptBtn->setObjectName("interrupt");

	// connect the button to control functions of the interpreter
	connect(startBtn, SIGNAL(clicked()), this, SLOT(startKernel()));
	connect(stopBtn, SIGNAL(clicked()), this, SLOT(stopKernel()));
	connect(resetBtn, SIGNAL(clicked()), this, SLOT(resetKernel()));
	connect(interruptBtn, SIGNAL(clicked()), this, SLOT(interruptKernel()));
}

NotebookApp::NotebookApp(QWidget* parent): QWidget(parent), interp(&iq, &oq) {
	interrupt_flag.store(false);

	input = new InputWidget(this);
	input->setObjectName("input");

	output = new OutputWidget(this);
	output->setObjectName("output");

	initButtons();

	// Setup the layout
	auto layout = new QVBoxLayout;
	layout->addLayout(kernelControlButtons);
	layout->addWidget(input);
	layout->addWidget(output);

	setLayout(layout);
	setWindowTitle(tr("Plotscript Notebook"));

	// Whenever the user sends an input, we need to process that input
	connect(input, &InputWidget::publish,
		this, &NotebookApp::process);
}

void NotebookApp::process(const QString& str) {
	input->setDisabled(true);

	// Clear the output widget then process the user's input
	output->clear();
	if (interp.isActive()) {
		iq.push(str.toStdString());

		// TODO: use a timer to check the input and clear the interrupt flag
		OutputMessage msg;
		oq.wait_pop(msg);

		// Output the message
		if (msg.type == ErrorType) {
			output->error(QString::fromStdString(msg.err));
		} else if (msg.type == ExpressionType) {
			output->processExpression(msg.exp);
		}
	} else {
		output->error("interpreter kernel not running");
	}

	// re-scale the output after the user's input gets processed
	output->scale();
	input->setDisabled(false);
}

void NotebookApp::startKernel() {
	interp.start();
}

void NotebookApp::stopKernel() {
	interp.stop();
}

void NotebookApp::resetKernel() {
	interp.reset();
}

void NotebookApp::interruptKernel() {
	interrupt_flag.store(true);
}
