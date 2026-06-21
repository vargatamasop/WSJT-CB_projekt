#include "about.h"

#include <QCoreApplication>
#include <QString>

#include "revision_utils.hpp"

#include "ui_about.h"

CAboutDlg::CAboutDlg(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::CAboutDlg)
{
  ui->setupUi(this);

  ui->labelTxt->setText ("<h2>" + product_versioned_name (revision ()) + "</h2>"

    "WSJT-11m V 1.0.0-crimson is an optimized version of the WSJT software for<br />"
    "weak-signal CB 27MHz communications.  <br /><br />"
    "&copy; 2026 by Varga Tamás 109HA2247,  <br />"
    "WSJT-11m is based on the WSJT-CB software <br />"
    "by Joe Taylor K1JT.<br /><br />"
    "The 11m community for the support.<br />");
}

CAboutDlg::~CAboutDlg()
{
}
