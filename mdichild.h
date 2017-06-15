#ifndef MDICHILD_H
#define MDICHILD_H

#include <QTextEdit>

class MdiChild : public QTextEdit
{
    Q_OBJECT
public:
    MdiChild();
    bool loadFile(QString);
    bool save();
    QString path;

protected:
    void wheelEvent(QWheelEvent*);

private slots:
    void onContentsChanged();

};

#endif // MDICHILD_H
