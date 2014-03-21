#include "burncoinsentry.h"
#include "ui_burncoinsentry.h"
#include "guiutil.h"
#include "bitcoinunits.h"
#include "addressbookpage.h"
#include "walletmodel.h"
#include "optionsmodel.h"
#include "addresstablemodel.h"

//for fTestNet and burn addresses
#include "util.h"
#include "main.h"

#include <QApplication>
#include <QClipboard>

BurnCoinsEntry::BurnCoinsEntry(QWidget *parent) :
  QFrame(parent),
  ui(new Ui::BurnCoinsEntry),
  model(0)
{
  ui->setupUi(this);

  setFocusPolicy(Qt::TabFocus);
  setFocusProxy(ui->payAmount);

}

BurnCoinsEntry::~BurnCoinsEntry()
{
  delete ui;
}

//~ void BurnCoinsEntry::on_pasteButton_clicked()
//~ {
//~ // Paste text from clipboard into recipient field
//~ ui->payTo->setText(QApplication::clipboard()->text());
//~ }

//~ void BurnCoinsEntry::on_addressBookButton_clicked()
//~ {
//~ if(!model)
//~ return;
//~ AddressBookPage dlg(AddressBookPage::ForSending, AddressBookPage::SendingTab, this);
//~ dlg.setModel(model->getAddressTableModel());
//~ if(dlg.exec())
//~ {
//~ ui->payTo->setText(dlg.getReturnValue());
//~ ui->payAmount->setFocus();
//~ }
//~ }

//~ void BurnCoinsEntry::on_payTo_textChanged(const QString &address)
//~ {
//~ if(!model)
//~ return;
//~ // Fill in label from address book, if address has an associated label
//~ QString associatedLabel = model->getAddressTableModel()->labelForAddress(address);
//~ if(!associatedLabel.isEmpty())
//~ ui->addAsLabel->setText(associatedLabel);
//~ }

void BurnCoinsEntry::setModel(WalletModel *model)
{
  this->model = model;
  clear();
}

//~ void BurnCoinsEntry::setRemoveEnabled(bool enabled)
//~ {
//~ ui->deleteButton->setEnabled(enabled);
//~ }

void BurnCoinsEntry::clear()
{
  ui->payAmount->clear();
  ui->payAmount->setFocus();
  if(model && model->getOptionsModel())
  {
    ui->payAmount->setDisplayUnit(model->getOptionsModel()->getDisplayUnit());
  }
}

//~ void BurnCoinsEntry::on_deleteButton_clicked()
//~ {
//~ emit removeEntry(this);
//~ }

bool BurnCoinsEntry::validate()
{
  // Check input validity
  bool retval = true;

  if(!ui->payAmount->validate())
    retval = false;
  else
  {
    if(ui->payAmount->value() <= 0)
    {
      // Cannot send 0 coins or less
      ui->payAmount->setValid(false);
      retval = false;
    }
  }

  //~ if(!ui->payTo->hasAcceptableInput() ||
  //~ (model && !model->validateAddress(ui->payTo->text())))
  //~ {
  //~ ui->payTo->setValid(false);
  //~ retval = false;
  //~ }

  return retval;
}

SendCoinsRecipient BurnCoinsEntry::getValue()
{
  SendCoinsRecipient rv;

  const CBitcoinAddress address = fTestNet ? burnTestnetAddress : burnOfficialAddress;   

  rv.address = address.ToString().c_str();
  rv.label = "";
  rv.amount = ui->payAmount->value();

  return rv;
}

//~ QWidget *BurnCoinsEntry::setupTabChain(QWidget *prev)
//~ {
//~ QWidget::setTabOrder(prev, ui->payTo);
//~ QWidget::setTabOrder(ui->payTo, ui->addressBookButton);
//~ QWidget::setTabOrder(ui->addressBookButton, ui->pasteButton);
//~ QWidget::setTabOrder(ui->pasteButton, ui->deleteButton);
//~ QWidget::setTabOrder(ui->deleteButton, ui->addAsLabel);
//~ return ui->payAmount->setupTabChain(ui->addAsLabel);
//~ }

void BurnCoinsEntry::setValue(const qint64 &value)
{
  ui->payAmount->setValue(value);
}

bool BurnCoinsEntry::isClear()
{
  return ui->payAmount->validate();
}

void BurnCoinsEntry::setFocus()
{
  ui->payAmount->setFocus();
}
