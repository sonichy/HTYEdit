#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    Ui::MainWindow *ui;
    void SyntaxHighlight();

private:
    void open(QString filename);

private slots:
    void on_action_new_triggered();
    void on_action_open_triggered();
    void on_action_close_triggered();
    void on_action_save_triggered();
    void on_action_saveas_triggered();
    void on_action_changelog_triggered();
    void on_action_aboutQt_triggered();
    void on_action_about_triggered();
    void on_action_subWindowView_triggered();
    void on_action_tabbedView_triggered();
    void on_action_cascade_triggered();
    void on_action_tile_triggered();
    void on_action_run_triggered();
    void on_action_zoomin_triggered();
    void on_action_zoomout_triggered();
    void on_action_undo_triggered();
    void on_action_redo_triggered();
    void on_action_find_triggered();
    void on_action_font_triggered();
    void find();
    void replace();
    void replaceAll();
    void onCursorPositionChanged();
};

#endif // MAINWINDOW_H
