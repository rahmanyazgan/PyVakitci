#include "Dialogs.h"
#include "Localization.h"
#include <QHeaderView>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QIcon>
#include <QLabel>
#include <QTabWidget>
#include <QPushButton>
#include <QShortcut>
#include <QDesktopWidget>
#include <QApplication>

AylikVakitlerDialog::AylikVakitlerDialog(const QString& dbYolu, QWidget* parent)
    : QDialog(parent) {
    setObjectName("Vakitler");
    resize(720, 550);
    setMinimumSize(720, 550);
    setMaximumSize(720, 550);
    setWindowTitle(Localization::tr("aylik_vakitler"));
    setWindowIcon(QIcon(":/resimler/cami.png"));

    QFont font("Arial", 10);
    setFont(font);

    m_tableView = new QTableView(this);
    m_tableView->setGeometry(10, 10, 700, 530);
    m_tableView->setMinimumSize(700, 530);
    m_tableView->setMaximumSize(700, 530);
    m_tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_tableView->setSelectionMode(QAbstractItemView::SingleSelection);
    m_tableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_tableView->setObjectName("vakitler_tableView");

    QShortcut* esc = new QShortcut(QKeySequence(Qt::Key_Escape), this);
    connect(esc, &QShortcut::activated, this, &QDialog::close);

    QSqlDatabase db;
    if (QSqlDatabase::contains("PyVakitciConnection")) {
        db = QSqlDatabase::database("PyVakitciConnection");
    } else {
        db = QSqlDatabase::addDatabase("QSQLITE", "PyVakitciAylikConn");
        db.setDatabaseName(dbYolu);
        db.open();
    }

    QSqlQuery q(db);
    if (!q.exec("SELECT sehir, tarih, imsak, gunes, ogle, ikindi, aksam, yatsi FROM namazvakitleri")) {
        veritabaniVarmi = false;
        return;
    }

    QList<QStringList> rows;
    while (q.next()) {
        QStringList r;
        for (int i = 0; i < 8; ++i) {
            r.append(q.value(i).toString());
        }
        rows.append(r);
    }

    if (rows.isEmpty()) {
        veritabaniVarmi = false;
        return;
    }

    QStringList headers = {Localization::tr("sehir"), Localization::tr("tarih"), Localization::tr("imsak"), Localization::tr("gunes"), Localization::tr("ogle"), Localization::tr("ikindi"), Localization::tr("aksam"), Localization::tr("yatsi")};
    m_model = new QStandardItemModel(rows.size(), 8, this);
    m_model->setHorizontalHeaderLabels(headers);

    for (int r = 0; r < rows.size(); ++r) {
        for (int c = 0; c < 8; ++c) {
            QStandardItem* item = new QStandardItem(rows[r][c]);
            item->setTextAlignment(Qt::AlignCenter);
            m_model->setItem(r, c, item);
        }
    }

    m_tableView->setModel(m_model);
    QHeaderView* hh = m_tableView->horizontalHeader();
    QHeaderView* vh = m_tableView->verticalHeader();
    hh->setSectionResizeMode(QHeaderView::Stretch);
    vh->setSectionResizeMode(QHeaderView::Stretch);
    hh->setStretchLastSection(true);
    vh->setStretchLastSection(true);
    m_tableView->resizeColumnsToContents();
    m_tableView->resizeRowsToContents();

    QRect scr = QApplication::desktop()->screenGeometry();
    move((scr.width() - width()) / 2, (scr.height() - height()) / 2);
}

HakkindaDialog::HakkindaDialog(QWidget* parent)
    : QDialog(parent) {
    setObjectName("Dialog");
    resize(418, 231);
    setMinimumSize(418, 231);
    setMaximumSize(418, 231);
    setWindowTitle(Localization::tr("hakkinda"));
    setWindowIcon(QIcon(":/resimler/hakkinda.png"));

    QFont font("Arial", 10);
    setFont(font);

    QLabel* logoLabel = new QLabel(this);
    logoLabel->setGeometry(12, 8, 48, 48);
    logoLabel->setPixmap(QPixmap(":/resimler/cami.png"));
    logoLabel->setScaledContents(true);

    QLabel* baslikLabel = new QLabel("PyVakitci 1.8", this);
    baslikLabel->setGeometry(74, 21, 171, 31);
    QFont baslikFont("Arial", 16, QFont::Bold);
    baslikLabel->setFont(baslikFont);

    QTabWidget* tabWidget = new QTabWidget(this);
    tabWidget->setGeometry(9, 60, 401, 130);
    QFont tabFont("Arial", 11);
    tabWidget->setFont(tabFont);

    QWidget* tab1 = new QWidget();
    QLabel* lbl1 = new QLabel(tab1);
    lbl1->setGeometry(14, 15, 365, 81);
    lbl1->setWordWrap(true);
    lbl1->setText(
        "<!DOCTYPE HTML PUBLIC \"-//W3C//DTD HTML 4.0//EN\" \"http://www.w3.org/TR/REC-html40/strict.dtd\">\n"
        "<html><head><meta name=\"qrichtext\" content=\"1\" /></head><body style=\" font-family:'Arial'; font-size:10pt;\">\n"
        "<p style=\" margin:0px;\"><span style=\" font-size:9pt;\">" + Localization::tr("hakkinda_aciklama") + "</span></p>\n"
        "<p style=\" margin-top:8px; margin-bottom:0px;\"><span style=\" font-size:9pt; font-weight:600;\">Web:</span>"
        "<span style=\" font-size:9pt;\"> </span><a href=\"https://github.com/rahmanyazgan/PyVakitci\">"
        "<span style=\" font-size:9pt; text-decoration: underline; color:#0000ff;\">PyVakitci Projesi</span></a></p></body></html>"
    );
    lbl1->setOpenExternalLinks(true);
    tabWidget->addTab(tab1, Localization::tr("hakkinda"));

    QWidget* tab3 = new QWidget();
    QLabel* lbl3 = new QLabel(tab3);
    lbl3->setGeometry(14, 15, 365, 76);
    lbl3->setWordWrap(true);
    lbl3->setText(
        "<html><head/><body><p><span style=\" font-size:9pt;\">" + Localization::tr("oneri_sikayet") + "</span></p>"
        "<p><span style=\" font-size:9pt; font-weight:600;\">E-Posta:</span><span style=\" font-size:9pt;\"> </span>"
        "<a href=\"mailto:rahmanyazgan@gmail.com\"><span style=\" font-size:9pt; text-decoration: underline; color:#0000ff;\">rahmanyazgan@gmail.com</span></a></p></body></html>"
    );
    lbl3->setOpenExternalLinks(true);
    tabWidget->addTab(tab3, Localization::tr("bilgilendirme"));

    QPushButton* kapatBtn = new QPushButton(Localization::tr("cikis"), this);
    kapatBtn->setGeometry(330, 196, 80, 28);
    QFont btnFont("Arial", 11);
    kapatBtn->setFont(btnFont);
    kapatBtn->setIcon(QIcon(":/resimler/cikis.png"));
    connect(kapatBtn, &QPushButton::clicked, this, &QDialog::close);

    QShortcut* esc = new QShortcut(QKeySequence(Qt::Key_Escape), this);
    connect(esc, &QShortcut::activated, this, &QDialog::close);

    QRect scr = QApplication::desktop()->screenGeometry();
    move((scr.width() - width()) / 2, (scr.height() - height()) / 2);
}

