#include "mdichild.h"
#include "highlighter.h"
#include <QWheelEvent>
#include <QTextStream>
#include <QApplication>
#include <QScrollBar>
#include <QMessageBox>
#include <QFileInfo>
#include <QDebug>

MdiChild::MdiChild()
{    
    connect(document(),SIGNAL(contentsChanged()),this,SLOT(onContentsChanged()));
}

void MdiChild::wheelEvent(QWheelEvent *e)
{
    if(QApplication::keyboardModifiers() == Qt::ControlModifier){
        if(e->delta() > 0){
            zoomIn();
        }else{
            zoomOut();
        }
    }else{
        if(e->delta() > 0){
            verticalScrollBar()->setValue(verticalScrollBar()->value()-30);
        }else{
            verticalScrollBar()->setValue(verticalScrollBar()->value()+30);
        }
    }
}

bool MdiChild::loadFile(QString filename)
{
    path = filename;
    setWindowTitle(QFileInfo(filename).fileName() + "[*]");
    QFile *file = new QFile;
    file->setFileName(filename);
    bool ok = file->open(QIODevice::ReadOnly);
    if(ok){
        QPalette plt = palette();
        plt.setColor(QPalette::Text,QColor(Qt::white));
        plt.setBrush(QPalette::Base,QBrush(Qt::black));
        setPalette(plt);
        QTextStream ts(file);
        QString s = ts.readAll();
        setVisible(true);
        file->close();
        delete file;
        setPlainText(s);
        Highlighter *highlighter = new Highlighter(document());
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
        setWindowTitle(QFileInfo(path).fileName()+"[*]");
        setWindowModified(false);
        return true;
    }else{
        QMessageBox::warning(this,"错误", QString(" %1:\n%2").arg(path).arg(file.errorString()));
        return false;
    }
}

void MdiChild::onContentsChanged()
{
    setWindowModified(document()->isModified());
}
