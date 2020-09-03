#include "mdichild.h"
#include "highlighter.h"
#include <QWheelEvent>
#include <QTextStream>
#include <QApplication>
#include <QScrollBar>
#include <QMessageBox>
#include <QFileInfo>
#include <QDebug>
#include <QTextCodec>
#include <QProcess>
#include <QPainter>
#include <QTextDocumentFragment>

MdiChild::MdiChild(QWidget *parent) :
    QPlainTextEdit(parent),
    settings(QCoreApplication::organizationName(), QCoreApplication::applicationName())
{
    setViewportMargins(50, 0, 0, 0);

    QString sfont = settings.value("Font").toString();
    if (sfont == "") {
        font = qApp->font();
    } else {
        QStringList SLFont = sfont.split(",");
        font = QFont(SLFont.at(0),SLFont.at(1).toInt(),SLFont.at(2).toInt(),SLFont.at(3).toInt());
    }
    ((QTextEdit*)(this))->setCurrentFont(font);

    QFontMetrics FM(font);
    int ts = 4;
    setTabStopWidth(ts * FM.boundingRect("*").width());
    connect(document(), SIGNAL(contentsChanged()), this, SLOT(contentsChange()));

    lineNumberArea = new LineNumberArea(this);
    connect(this, SIGNAL(updateRequest(QRect,int)), this, SLOT(updateLineNumberArea(QRect,int)));
    connect(this, SIGNAL(blockCountChanged(int)), this, SLOT(updateLineNumberAreaWidth(int)));

}

bool MdiChild::loadFile(QString filename)
{
    path = filename;
    setWindowTitle(QFileInfo(filename).fileName() + "[*]");
    // 检测编码
    QProcess *process = new QProcess;
    process->start("file --mime-encoding " + filename);
    process->waitForFinished();
    QString PO = process->readAllStandardOutput();
    scodec = PO.mid(PO.indexOf(": ") + 2).replace("\n","").toUpper();
    //qDebug() << scodec;

    QFile *file = new QFile;
    file->setFileName(filename);
    bool ok = file->open(QIODevice::ReadOnly);
    if(ok){
        QPalette plt = palette();
        plt.setColor(QPalette::Text,QColor(Qt::white));
        plt.setBrush(QPalette::Base,QBrush(Qt::black));
        setPalette(plt);
        QTextStream TS(file);
        // 还是乱码
        QTextCodec *codec = QTextCodec::codecForName(scodec.toLatin1());
        TS.setCodec(codec);
        QString s = TS.readAll();
        file->close();
        delete file;
        setPlainText(s);
        Highlighter *highlighter = new Highlighter(document());
        Q_UNUSED(highlighter);
        showMaximized();
        zoomIn(4);
        return true;
    }else{
        QMessageBox::warning(this,"错误", QString(" %1:\n%2").arg(path).arg(file->errorString()));
        return false;
    }
}

bool MdiChild::save()
{
    QFile file(path);
    if(file.open(QFile::WriteOnly))
    {
        QTextStream ts(&file);
        QString s = toPlainText();
        ts << s;
        //ui->mdiArea->currentSubWindow()->setWindowTitle(QFileInfo(filename).fileName());
        //LS1->setText("保存 "+filename);
        setWindowTitle(QFileInfo(path).fileName() + "[*]");
        setWindowModified(false);
        return true;
    }else{
        QMessageBox::warning(this,"错误", QString(" %1:\n%2").arg(path).arg(file.errorString()));
        return false;
    }
}

void MdiChild::contentsChange()
{
    setWindowModified(document()->isModified());
}

void MdiChild::wheelEvent(QWheelEvent *e)
{
    if(QApplication::keyboardModifiers() == Qt::ControlModifier){
        QTextCursor cursor = textCursor();
        selectAll();
        font = ((QTextEdit*)(this))->currentFont();
        qDebug() << ((QTextEdit*)(this))->fontPointSize();
        if (e->delta() > 0) {
            ((QTextEdit*)(this))->setFontPointSize(((QTextEdit*)(this))->fontPointSize() + 1);
        } else {
            if(((QTextEdit*)(this))->fontPointSize() > 1)
                ((QTextEdit*)(this))->setFontPointSize(((QTextEdit*)(this))->fontPointSize() - 1);
        }
        setTextCursor(cursor);
        lineNumberArea->update();
    } else {
        QPlainTextEdit::wheelEvent(e);
    }
}

void MdiChild::keyPressEvent(QKeyEvent *e)
{
    // 自动补全
    if (e->key() == Qt::Key_ParenLeft) {
        insertPlainText("()");
        moveCursor(QTextCursor::Left, QTextCursor::MoveAnchor);
    } else if (e->key() == Qt::Key_BraceLeft) {
        insertPlainText("{}");
        moveCursor(QTextCursor::Left, QTextCursor::MoveAnchor);
    } else if (e->key() == Qt::Key_BracketLeft) {
        insertPlainText("[]");
        moveCursor(QTextCursor::Left, QTextCursor::MoveAnchor);
    } else if ((e->modifiers() & Qt::ControlModifier) && e->key() == Qt::Key_Slash) {// Ctrl+/ 注释选中行
        QString s = textCursor().selection().toPlainText();
        QStringList SL = s.split("\n");
        for (int i=0; i<SL.length(); i++) {
            if (SL.at(i).trimmed().startsWith("//")) {
                s = SL.at(i);
                s.replace("//","");
            } else {
                s = "//" + SL.at(i);
            }
            if (i<SL.length()-1) s.append("\n");
            textCursor().insertText(s);
        }
    } else if (e->key() == Qt::Key_Enter || e->key() == Qt::Key_Return) {//auto indent
        QString s = textCursor().block().text();    //当前行字符串
        QString st = s.trimmed();   //去首尾空格后的字符串
        QString ws = s.mid(0, s.indexOf(st));   //字符串前面的空白
        qDebug() << ws;
        if(st.endsWith("{")){
            textCursor().insertText("\n\t" + ws);   //插入换行 + tab + 行首空白字符(缩进)
        }else{
            textCursor().insertText("\n" + ws);     //插入换行 + 行首空白字符(不缩进)
        }
    } else if (e->key() == Qt::Key_Tab) {   //选中每行前插入一个tab
        QString s = textCursor().selection().toPlainText();
        QStringList SL = s.split("\n");
        s = "";
        for (int i=0; i<SL.length(); i++) {
            QString st = "\t" + SL.at(i);
            if (i<SL.length()-1) st.append("\n");
            s += st;
        }
        textCursor().insertText(s);
    } else {
        return QPlainTextEdit::keyPressEvent(e);
    }
}

// 生成行号
void MdiChild::lineNumberAreaPaintEvent(QPaintEvent *event)
{
    QPainter painter(lineNumberArea);
    painter.fillRect(event->rect(), QBrush(QColor(33,33,33)));
    QTextBlock block = firstVisibleBlock();
    int blockNumber = block.blockNumber();
    int top = (int) blockBoundingGeometry(block).translated(contentOffset()).top();
    int bottom = top + (int) blockBoundingRect(block).height();
    while (block.isValid() && top <= event->rect().bottom()) {
        if (block.isVisible() && bottom >= event->rect().top()) {
            QString number = QString::number(blockNumber + 1);
            painter.setPen(Qt::white);
            painter.setFont(font);
            QFontMetrics FM(font);
            //painter.drawText(0, top, lineNumberArea->width(), fontMetrics().height(), Qt::AlignRight, number);
            painter.drawText(0, top, lineNumberArea->width(), FM.boundingRect(number).height(), Qt::AlignRight, number);
        }
        block = block.next();
        top = bottom;
        bottom = top + (int) blockBoundingRect(block).height();
        ++blockNumber;
    }
}

int MdiChild::lineNumberAreaWidth()
{
    int digits = 1;
    int max = qMax(1, blockCount());
    while (max >= 10) {
        max /= 10;
        ++digits;
    }
    //int space = 3 + fontMetrics().horizontalAdvance(QLatin1Char('9')) * digits;
    QFontMetrics FM(font);
    int space = 5 + FM.width(QLatin1Char('9')) * digits;
    return space;
}

void MdiChild::resizeEvent(QResizeEvent *e)
{
    QPlainTextEdit::resizeEvent(e);
    QRect cr = contentsRect();
    lineNumberArea->setGeometry(QRect(cr.left(), cr.top(), lineNumberAreaWidth(), cr.height()));
}

void MdiChild::updateLineNumberArea(const QRect &rect, int dy)
{
    if (dy)
        lineNumberArea->scroll(0, dy);
    else
        lineNumberArea->update(0, rect.y(), lineNumberArea->width(), rect.height());

    if (rect.contains(viewport()->rect()))
        updateLineNumberAreaWidth(0);
}

void MdiChild::updateLineNumberAreaWidth(int /* newBlockCount */)
{
    setViewportMargins(lineNumberAreaWidth(), 0, 0, 0);
}

void MdiChild::setReadOnlyA(bool b)
{
    setReadOnly(b);
}

void MdiChild::insertH(QString h)
{
    QString s = textCursor().selection().toPlainText(); //可以显示换行符
    QStringList SL = s.split("\n");
    s = "";
    for (int i=0; i<SL.length(); i++) {
        if (SL.at(i) != "") {
            s += "<" + h +">" + SL.at(i) + "</" + h + ">";
            if (i < SL.length()-1)
                s += "\n";
        }
    }
    textCursor().insertText(s);
}

void MdiChild::insertTR()
{
    QString s = textCursor().selection().toPlainText();
    QStringList SL = s.split("\n");
    s = "";
    for (int i=0; i<SL.length(); i++) {
        if (SL.at(i) != "") {
            s += "<tr>" + SL.at(i) + "</tr>";
            if (i < SL.length()-1)
                s += "\n";
        }
    }
    textCursor().insertText(s);
}

void MdiChild::insertTD()
{
    QString s = textCursor().selection().toPlainText();
    QStringList SL = s.split("\n");
    s = "";
    for (int i=0; i<SL.length(); i++) {
        if (SL.at(i) != "") {
            s += "<td>" + SL.at(i) + "</td>";
            if (i < SL.length()-1)
                s += "\n";
        }
    }
    textCursor().insertText(s);
}

void MdiChild::insertP()
{
    QString s = textCursor().selection().toPlainText();
    s = s.replace("<br>", "\n");
    QStringList SL = s.split("\n");
    s = "";
    for (int i=0; i<SL.length(); i++) {
        if (SL.at(i) != "") {
            s += "<p>" + SL.at(i) + "</p>";
            if (i < SL.length()-1)
                s += "\n";
        }
    }
    textCursor().insertText(s);
}

void MdiChild::insertDIV(QString s1)
{
    QString s = textCursor().selection().toPlainText();
    QStringList SL = s.split("\n");
    s = "";
    for (int i=0; i<SL.length(); i++) {
        if (SL.at(i) != "") {
            s += "<div" + s1 + ">" + SL.at(i) + "</div>";
            if (i < SL.length()-1)
                s += "\n";
        }
    }
    textCursor().insertText(s);
}

void MdiChild::insertA(QString s1)
{
    //正则去链接("/<a[^>]*>(.*?)<\/a>/is", "$1")
    QString s = textCursor().selection().toPlainText();
    QStringList SL = s.split("\n");
    s = "";
    for (int i=0; i<SL.length(); i++) {
        if (SL.at(i) != "") {
            s += "<a " + s1 + ">" + SL.at(i) + "</a>";
            if (i < SL.length()-1)
                s += "\n";
        }
    }
    textCursor().insertText(s);
}