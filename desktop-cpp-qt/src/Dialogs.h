#ifndef DIALOGS_H
#define DIALOGS_H

#include <QDialog>
#include <QTableView>
#include <QStandardItemModel>

class AylikVakitlerDialog : public QDialog {
    Q_OBJECT
public:
    explicit AylikVakitlerDialog(const QString& dbYolu, QWidget* parent = nullptr);
    bool veritabaniVarmi = true;

private:
    QTableView* m_tableView = nullptr;
    QStandardItemModel* m_model = nullptr;
};

class HakkindaDialog : public QDialog {
    Q_OBJECT
public:
    explicit HakkindaDialog(QWidget* parent = nullptr);
};

#endif // DIALOGS_H

