#include <QTest>
#include <QString>
#include <QGraphicsItem>
#include <QList>
#include <QPair>

#include "notebook_app.hpp"
#include "interrupt_flag.hpp"

class NotebookTest : public QObject {
	Q_OBJECT

private slots:
	void initTestCase();

	void findInputWidget();
	void findOutputWidget();
	void findOutputView();
	void findOutputScene();
	void simpleExpressions();

	// Tests for point objects
	void testPoint();
	void testPointLists();

	// Tests for line objects
	void testLine();
	void testLineLists();

	// Tests for line objects
	void testText();
	void testTextLists();

	// Test for plots
	void testDiscretePlotLayout();
	void testContinuousPlotSimple();

	// TODO: implement additional tests here
private:
	NotebookApp app;

	// Helper functions for repeated tests
	void submitInput(QWidget* w);
	QGraphicsScene* getScene(QWidget* output);
	void verifyNumberOfOutputGraphics(QWidget* output, qreal n);
	void testSimpleExpression(const QString& input, const QString& output);
	void testSinglePoint(const QString& input, const QRectF& expected);
	void testSingleLine(const QString& input, const QLineF& expected, const qreal& thickness);
	void testSingleText(const QString& input, const QString& expected, const QPointF& pos,
		qreal rotation = 0);
};

void NotebookTest::initTestCase() {
	interrupt_flag.store(false);
}

// ************ START: Helper functions for repeated tests ************
void NotebookTest::submitInput(QWidget* w) {
	QTest::keyPress(w, Qt::Key_Return, Qt::ShiftModifier);
	QTest::keyRelease(w, Qt::Key_Return, Qt::ShiftModifier);
}

QGraphicsScene* NotebookTest::getScene(QWidget* output) {
	auto view = output->findChild<QGraphicsView*>();
	return view->scene();
}

void NotebookTest::verifyNumberOfOutputGraphics(QWidget* output, qreal n) {
	auto children = getScene(output)->items();
	qreal numItems = children.size();
	QCOMPARE(numItems, n);
}

void NotebookTest::testSimpleExpression(const QString& input, const QString& output) {
	auto ip = app.findChild<QWidget*>("input");
	auto op = app.findChild<QWidget*>("output");

	// Type in a simple expression
	QTest::keyClicks(ip, input);
	submitInput(ip);

	// There should only be one child in the output
	verifyNumberOfOutputGraphics(op, 1);

	// Make sure the one graphic is a text object at the center of the scene
	auto scene = getScene(op);
	auto item = scene->itemAt(0, 0, QTransform());

	QGraphicsTextItem* graphic = qgraphicsitem_cast<QGraphicsTextItem*>(item);
	QVERIFY2(graphic != NULL, "Graphics item is not a text item");

	// I use contains here so I can test for errors (i.e. test if starts with "Error:")
	QString msg = "Testing simple text expression: unexpected output for " + input +
		". got \"" + graphic->toPlainText() + "\" expected \"" + output + "\"";
	QVERIFY2(graphic->toPlainText().contains(output), qPrintable(msg));
}

void NotebookTest::testSinglePoint(const QString& input, const QRectF& expected) {
	auto ip = app.findChild<QWidget*>("input");
	auto op = app.findChild<QWidget*>("output");

	// Type in a simple expression
	QTest::keyClicks(ip, input);
	submitInput(ip);

	// There should only be one child in the output
	verifyNumberOfOutputGraphics(op, 1);

	auto scene = getScene(op);
	auto item = scene->items()[0];

	QGraphicsEllipseItem* graphic = qgraphicsitem_cast<QGraphicsEllipseItem*>(item);
	QVERIFY2(graphic != NULL, "Graphics item is not an ellipse item");

	QCOMPARE(graphic->rect(), expected);
}

void NotebookTest::testSingleLine(const QString& input, const QLineF& expected,
	const qreal& thickness) {
	auto ip = app.findChild<QWidget*>("input");
	auto op = app.findChild<QWidget*>("output");

	// Type in a simple expression
	QTest::keyClicks(ip, input);
	submitInput(ip);

	// There should only be one child in the output
	verifyNumberOfOutputGraphics(op, 1);

	auto scene = getScene(op);
	auto item = scene->items()[0];

	QGraphicsLineItem* graphic = qgraphicsitem_cast<QGraphicsLineItem*>(item);
	QVERIFY2(graphic != NULL, "Graphics item is not an line item");

	QCOMPARE(graphic->line(), expected);
	QCOMPARE(graphic->pen(), QPen(QBrush(Qt::black), thickness));
}

void NotebookTest::testSingleText(const QString& input, const QString& expected,
	const QPointF& pos, qreal rotation) {
	auto ip = app.findChild<QWidget*>("input");
	auto op = app.findChild<QWidget*>("output");

	// Type in a simple expression
	QTest::keyClicks(ip, input);
	submitInput(ip);

	// There should only be one child in the output
	verifyNumberOfOutputGraphics(op, 1);

	auto scene = getScene(op);
	auto items = scene->items();

	QVERIFY2(items.size() == 1, "Too many items in the scene");

	QGraphicsTextItem* graphic = qgraphicsitem_cast<QGraphicsTextItem*>(items[0]);
	QVERIFY2(graphic != NULL, "Graphics item is not an text item");

	QCOMPARE(graphic->toPlainText(), expected);
	QVERIFY2(graphic->pos() + graphic->boundingRect().center() == pos, "Text graphic not centered");
	QCOMPARE(graphic->rotation(), rotation);
}

// ************ END: Helper functions for repeated tests ************
void NotebookTest::findInputWidget() {
	auto ip = app.findChild<QWidget*>("input");

	QVERIFY2(ip, "Could not find widget with name: 'input'");
}

void NotebookTest::findOutputWidget() {
	auto op = app.findChild<QWidget*>("output");

	QVERIFY2(op, "Could not find widget with name: 'output'");
}

void NotebookTest::findOutputView() {
	auto op = app.findChild<QWidget*>("output");
	auto view = op->findChild<QGraphicsView*>();

	QVERIFY2(view, "Could not find a view in the output widget");
}

void NotebookTest::findOutputScene() {
	auto op = app.findChild<QWidget*>("output");
	auto view = op->findChild<QGraphicsView*>();
	auto scene = view->scene();

	QVERIFY2(scene, "Could not find a scene in the output widget");
}

void NotebookTest::simpleExpressions() {

	// List of inputs and outputs for simple expressions
	QList<QPair<QString, QString> > tests({
		qMakePair(QString("(1)"), QString("(1)")),
		qMakePair(QString("(\"test\")"), QString("(\"test\")")),
		qMakePair(QString("(+ 1 2)"), QString("(3)")),
		qMakePair(QString("(cos pi)"), QString("(-1)")),
		qMakePair(QString("(+ 1 (* 3 I))"), QString("(1,3)")),
		qMakePair(QString("(first (list (list 1) 2 3))"), QString("(1)")),
		qMakePair(QString("(begin (define inc (lambda (x) (+ 1 x))) (inc 1))"), QString("(2)")),
		qMakePair(QString("("), QString("Error")),
		qMakePair(QString("(+)"), QString("Error")),
		qMakePair(QString("(first (2))"), QString("Error"))
	});

	for (auto test = tests.cbegin(); test != tests.cend(); test++) {
		testSimpleExpression(test->first, test->second);
	}
}

void NotebookTest::testPoint() {
	testSinglePoint(QString("(make-point 0 0)"), QRectF(0, 0, 0, 0));
	testSinglePoint(QString("(make-point 10 15)"), QRectF(10, 15, 0, 0));
	testSinglePoint(QString("(set-property \"size\" 20 (make-point 10 10))"),
		QRectF(0, 0, 20, 20));

	// Error handling of point objects
	testSimpleExpression(QString("(set-property \"size\" -1 (make-point 0 0))"),
		QString("Error"));
	testSimpleExpression(QString("(make-point I 0)"),
		QString("Error"));
	testSimpleExpression(QString("(make-point 1 2 3)"),
		QString("Error"));
	testSimpleExpression(QString("(set-property \"object-name\" \"point\" 3"),
		QString("Error"));
}

void NotebookTest::testPointLists() {
	auto ip = app.findChild<QWidget*>("input");
	auto op = app.findChild<QWidget*>("output");

	// Type in a simple expression
	QTest::keyClicks(ip, "(list "
		"(set-property \"size\" 1 (make-point 0 0)) "
		"(set-property \"size\" 2 (make-point 4 0)) "
		"(set-property \"size\" 4 (make-point 8 0))"
		")");
	submitInput(ip);

	// There should only be one child in the output
	verifyNumberOfOutputGraphics(op, 3);

	auto scene = getScene(op);
	auto items = scene->items();
	QList<QGraphicsEllipseItem*> graphics;

	// Make sure the graphics aren't duplicated
	bool first = false,
		second = false,
		third = false;

	// verify the types of each object
	for (auto item = items.cbegin(); item != items.cend(); item++) {
		QGraphicsEllipseItem* graphic = qgraphicsitem_cast<QGraphicsEllipseItem*>(*item);
		QVERIFY2(graphic != NULL, "Graphics item is not an ellipse item");

		// Unsure of the order, so I use a conditional
		if (!first && graphic->rect() == QRectF(-0.5, -0.5, 1, 1)) {
			first = true;
		} else if (!second && graphic->rect() == QRectF(3, -1, 2, 2)) {
			second = true;
		} else if (!third && graphic->rect() == QRectF(6, -2, 4, 4)) {
			third = true;
		} else {
			QString msg = "Unknown point in list: \"" + QString(QTest::toString(graphic->rect())) + "\"";
			QFAIL(qPrintable(msg));
		}
	}
}

void NotebookTest::testLine() {
	testSingleLine(QString("(make-line (make-point 0 0) (make-point 20 20))"),
		QLineF(0, 0, 20, 20), (qreal)1);
	testSingleLine(QString("(set-property \"thickness\" 4 "
		"(make-line (make-point 15 5) (make-point 11 19)))"), QLineF(15, 5, 11, 19), (qreal)4);

	// Error handling of point objects
	testSimpleExpression(QString("(make-line 1 (make-point 20 20))"),
		QString("Error"));
	testSimpleExpression(QString("(set-property \"thickness\" -1 "
		"(make-line (make-point 0 0) (make-point 20 20)))"), QString("Error"));
}

void NotebookTest::testLineLists() {
	auto ip = app.findChild<QWidget*>("input");
	auto op = app.findChild<QWidget*>("output");

	// Type in a simple expression
	QTest::keyClicks(ip, "(list "
		"(set-property \"thickness\" 2 (make-line (make-point 0 0) (make-point 0 20))) "
		"(set-property \"thickness\" 4 (make-line (make-point 10 0) (make-point 10 20))) "
		"(set-property \"thickness\" 8 (make-line (make-point 20 0) (make-point 20 20)))"
		")");
	submitInput(ip);

	// There should only be one child in the output
	verifyNumberOfOutputGraphics(op, 3);

	auto scene = getScene(op);
	auto items = scene->items();
	QList<QGraphicsLineItem*> graphics;

	// Make sure the graphics aren't duplicated
	bool first = false,
		second = false,
		third = false;

	// verify the types of each object
	for (auto item = items.cbegin(); item != items.cend(); item++) {
		QGraphicsLineItem* graphic = qgraphicsitem_cast<QGraphicsLineItem*>(*item);
		QVERIFY2(graphic != NULL, "Graphics item is not a line item");

		// Unsure of the order, so I use a conditional
		if (!first && graphic->line() == QLineF(0, 0, 0, 20) &&
			graphic->pen() == QPen(QBrush(Qt::black), 2)) {
			first = true;
		} else if (!second && graphic->line() == QLineF(10, 0, 10, 20) &&
			graphic->pen() == QPen(QBrush(Qt::black), 4)) {
			second = true;
		} else if (!third && graphic->line() == QLineF(20, 0, 20, 20) &&
			graphic->pen() == QPen(QBrush(Qt::black), 8)) {
			third = true;
		} else {
			QString msg = "Unknown line in list";
			QFAIL(qPrintable(msg));
		}
	}
}

void NotebookTest::testText() {
	testSingleText(QString("(make-text \"Hi\")"), QString("Hi"), QPointF());
	testSingleText(QString("(set-property \"position\" (make-point 10 20) (make-text \"Hi\"))"),
		QString("Hi"), QPointF(10, 20));
	testSingleText(QString("(set-property \"text-rotation\" (/ pi 2) (make-text \"Hi\"))"),
		QString("Hi"), QPointF(), 90);

	// Error handling of point objects
	testSimpleExpression(QString("(make-text 7)"), QString("Error"));
	testSimpleExpression(QString("(set-property \"position\" e (make-text \"Hi\"))"),
		QString("Error"));
}

void NotebookTest::testTextLists() {
	auto ip = app.findChild<QWidget*>("input");
	auto op = app.findChild<QWidget*>("output");

	// Type in a simple expression
	QTest::keyClicks(ip, "(list "
		"(set-property \"position\" (make-point 0 10) (make-text \"Hi\")) "
		"(set-property \"position\" (make-point 0 0) (make-text \"Hi\")) "
		"(set-property \"position\" (make-point 10 0) (make-text \"Hi\"))"
		")");
	submitInput(ip);

	// There should only be one child in the output
	verifyNumberOfOutputGraphics(op, 3);

	auto scene = getScene(op);

	// verify the types of each object
	auto items = scene->items();
	for (auto item = items.cbegin(); item != items.cend(); item++) {
		QGraphicsTextItem* graphic = qgraphicsitem_cast<QGraphicsTextItem*>(*item);
		QVERIFY2(graphic != NULL, "Graphics item is not a line item");
	}

	auto item = qgraphicsitem_cast<QGraphicsTextItem*>(scene->itemAt(QPointF(0, 10), QTransform()));
	QCOMPARE(item->toPlainText(), QString("Hi"));

	item = qgraphicsitem_cast<QGraphicsTextItem*>(scene->itemAt(QPointF(0, 0), QTransform()));
	QCOMPARE(item->toPlainText(), QString("Hi"));

	item = qgraphicsitem_cast<QGraphicsTextItem*>(scene->itemAt(QPointF(10, 0), QTransform()));
	QCOMPARE(item->toPlainText(), QString("Hi"));
}

// **************** Plotting Tests ****************
/*
findLines - find lines in a scene contained within a bounding box with a small margin
 */
int findLines(QGraphicsScene * scene, QRectF bbox, qreal margin){

	QPainterPath selectPath;

	QMarginsF margins(margin, margin, margin, margin);
	selectPath.addRect(bbox.marginsAdded(margins));
	scene->setSelectionArea(selectPath, Qt::ContainsItemShape);

	int numlines(0);
	foreach(auto item, scene->selectedItems()){
		if(item->type() == QGraphicsLineItem::Type){
			numlines += 1;
		}
	}

	return numlines;
}

/*
findPoints - find points in a scene contained within a specified rectangle
 */
int findPoints(QGraphicsScene * scene, QPointF center, qreal radius){

	QPainterPath selectPath;
	selectPath.addRect(QRectF(center.x()-radius, center.y()-radius, 2*radius, 2*radius));
	scene->setSelectionArea(selectPath, Qt::ContainsItemShape);

	int numpoints(0);
	foreach(auto item, scene->selectedItems()){
		if(item->type() == QGraphicsEllipseItem::Type){
			numpoints += 1;
		}
	}

	return numpoints;
}

/*
findText - find text in a scene centered at a specified point with a given
					 rotation and string contents
 */
int findText(QGraphicsScene * scene, QPointF center, qreal rotation, QString contents){

	int numtext(0);
	foreach(auto item, scene->items(center)){
		if(item->type() == QGraphicsTextItem::Type){
			QGraphicsTextItem * text = static_cast<QGraphicsTextItem *>(item);
			if((text->toPlainText() == contents) &&
		 (text->rotation() == rotation) &&
		 (text->pos() + text->boundingRect().center() == center)){
		numtext += 1;
			}
		}
	}

	return numtext;
}

/*
intersectsLine - find lines in a scene that intersect a specified rectangle
 */
int intersectsLine(QGraphicsScene * scene, QPointF center, qreal radius){

	QPainterPath selectPath;
	selectPath.addRect(QRectF(center.x()-radius, center.y()-radius, 2*radius, 2*radius));
	scene->setSelectionArea(selectPath, Qt::IntersectsItemShape);

	int numlines(0);
	foreach(auto item, scene->selectedItems()){
		if(item->type() == QGraphicsLineItem::Type){
			numlines += 1;
		}
	}

	return numlines;
}

void NotebookTest::testDiscretePlotLayout() {
	auto inputWidget = app.findChild<QWidget*>("input");
	auto outputWidget = app.findChild<QWidget*>("output");

	QTest::keyClicks(inputWidget, "((discrete-plot (list (list -1 -1) (list 1 1)) "
		"(list (list \"title\" \"The Title\") "
		"(list \"abscissa-label\" \"X Label\") "
		"(list \"ordinate-label\" \"Y Label\")))");
	submitInput(inputWidget);

	// first check total number of items
	// 8 lines + 2 points + 7 text = 17
	verifyNumberOfOutputGraphics(outputWidget, 17);

	auto scene = getScene(outputWidget);
	auto items = scene->items();

	// make them all selectable
	foreach(auto item, items){
		item->setFlag(QGraphicsItem::ItemIsSelectable);
	}

	double scalex = 20.0/2.0;
	double scaley = 20.0/2.0;

	double xmin = scalex*-1;
	double xmax = scalex*1;
	double ymin = scaley*-1;
	double ymax = scaley*1;
	double xmiddle = (xmax+xmin)/2;
	double ymiddle = (ymax+ymin)/2;

	// check title
	QCOMPARE(findText(scene, QPointF(xmiddle, -(ymax+3)), 0, QString("The Title")), 1);

	// check abscissa label
	QCOMPARE(findText(scene, QPointF(xmiddle, -(ymin-3)), 0, QString("X Label")), 1);

	// check ordinate label
	QCOMPARE(findText(scene, QPointF(xmin-3, -ymiddle), -90, QString("Y Label")), 1);

	// check abscissa min label
	QCOMPARE(findText(scene, QPointF(xmin, -(ymin-2)), 0, QString("-1")), 1);

	// check abscissa max label
	QCOMPARE(findText(scene, QPointF(xmax, -(ymin-2)), 0, QString("1")), 1);

	// check ordinate min label
	QCOMPARE(findText(scene, QPointF(xmin-2, -ymin), 0, QString("-1")), 1);

	// check ordinate max label
	QCOMPARE(findText(scene, QPointF(xmin-2, -ymax), 0, QString("1")), 1);

	// check the bounding box bottom
	QCOMPARE(findLines(scene, QRectF(xmin, -ymin, 20, 0), 0.1), 1);

	// check the bounding box top
	QCOMPARE(findLines(scene, QRectF(xmin, -ymax, 20, 0), 0.1), 1);

	// check the bounding box left and (-1, -1) stem
	QCOMPARE(findLines(scene, QRectF(xmin, -ymax, 0, 20), 0.1), 2);

	// check the bounding box right and (1, 1) stem
	QCOMPARE(findLines(scene, QRectF(xmax, -ymax, 0, 20), 0.1), 2);

	// check the abscissa axis
	QCOMPARE(findLines(scene, QRectF(xmin, 0, 20, 0), 0.1), 1);

	// check the ordinate axis
	QCOMPARE(findLines(scene, QRectF(0, -ymax, 0, 20), 0.1), 1);

	// check the point at (-1,-1)
	QCOMPARE(findPoints(scene, QPointF(-10, 10), 0.6), 1);

	// check the point at (1,1)
	QCOMPARE(findPoints(scene, QPointF(10, -10), 0.6), 1);
}

void NotebookTest::testContinuousPlotSimple() {
	auto inputWidget = app.findChild<QWidget*>("input");
	auto outputWidget = app.findChild<QWidget*>("output");

	QTest::keyClicks(inputWidget, "(continuous-plot (lambda (x) (sin x)) (list (- pi) pi) "
		"(list (list \"title\" \"The Title\") "
		"(list \"abscissa-label\" \"X Label\") "
		"(list \"ordinate-label\" \"Y Label\")))");
	submitInput(inputWidget);

	// first check total number of items
	// 4 edge + 2 axis + 66 lines + 7 text = 79
	verifyNumberOfOutputGraphics(outputWidget, 79);
}

QTEST_MAIN(NotebookTest)
#include "notebook_test.moc"
