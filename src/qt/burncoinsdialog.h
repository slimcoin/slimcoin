#ifndef BURNCOINSDIALOG_H
#define BURNCOINSDIALOG_H

#include <QDialog>
#include "sendcoinsdialog.h"

namespace Ui {
    class BurnCoinsDialog;
}
class WalletModel;
class SendCoinsEntry;
class SendCoinsRecipient;

QT_BEGIN_NAMESPACE
class QUrl;
QT_END_NAMESPACE

/** Dialog for sending bitcoins */
class BurnCoinsDialog : public SendCoinsDialog
{
    Q_OBJECT

public:
    explicit BurnCoinsDialog(QWidget *parent = 0) : SendCoinsDialog(parent) {}
    
    //the base's deconstructor is automatically called
    ~BurnCoinsDialog()
    {
    }

};

#endif // BURNCOINSDIALOG_H
