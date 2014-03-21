#ifndef BURNCOINSENTRY_H
#define BURNCOINSENTRY_H

#include <QFrame>

namespace Ui {
    class BurnCoinsEntry;
}

class WalletModel;
class SendCoinsRecipient;

/** A single entry in the dialog for sending bitcoins. */
class BurnCoinsEntry : public QFrame
{
    Q_OBJECT

public:
    explicit BurnCoinsEntry(QWidget *parent = 0);
    ~BurnCoinsEntry();

    void setModel(WalletModel *model);
    bool validate();
    SendCoinsRecipient getValue();

    /** Return whether the entry is still empty and unedited */
    bool isClear();

    void setValue(const qint64 &value);

    /** Set up the tab chain manually, as Qt messes up the tab chain by default in some cases (issue http://bugreports.qt.nokia.com/browse/QTBUG-10907).
     */
    //~ QWidget *setupTabChain(QWidget *prev);

    void setFocus();

public slots:
    //~ void setRemoveEnabled(bool enabled);
    void clear();

signals:
    void removeEntry(BurnCoinsEntry *entry);

private slots:
    //~ void on_deleteButton_clicked();
    //~ void on_payTo_textChanged(const QString &address);
    //~ void on_addressBookButton_clicked();
    //~ void on_pasteButton_clicked();

private:
    Ui::BurnCoinsEntry *ui;
    WalletModel *model;
};

#endif // BURNCOINSENTRY_H
