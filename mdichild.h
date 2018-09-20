#ifndef MDICHILD_H
#define MDICHILD_H

#include <QPlainTextEdit>

class LineNumberArea;

class MdiChild : public QPlainTextEdit
{
    Q_OBJECT
public:
    MdiChild(QWidget *parent);
    bool loadFile(QString);
    bool save();
    QString path, scodec;
    void lineNumberAreaPaintEvent(QPaintEvent *event);
    int lineNumberAreaWidth();

private:
    LineNumberArea *lineNumberArea;

protected:
    void wheelEvent(QWheelEvent*);
    void keyPressEvent(QKeyEvent *e);
    void resizeEvent(QResizeEvent* e);

private slots:
    void contentsChange();
    void updateLineNumberAreaWidth(int newBlockCount);
    void updateLineNumberArea(const QRect &, int);
    void setReadOnlyA(bool ro);
};


class LineNumberArea : public QWidget
{
public:
    LineNumberArea(MdiChild *editor) : QWidget(editor) {
        mdiChild = editor;
    }

    QSize sizeHint() const override {
        return QSize(mdiChild->lineNumberAreaWidth(), 0);
    }

protected:
    void paintEvent(QPaintEvent *event) override {
        mdiChild->lineNumberAreaPaintEvent(event);
    }

private:
    MdiChild *mdiChild;
};

#endif // MDICHILD_H
