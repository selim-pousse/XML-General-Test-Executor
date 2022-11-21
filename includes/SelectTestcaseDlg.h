//=================================================================================================
// Copyright (C) 2016 Pousse Systems
//
// Name           CSelectTestcaseDlg
//
// Description    Dialog Class to select or unselect the Test-Cases.
//
// Author         Selim Pousse
//=================================================================================================
#ifndef SELECTTESTCASEDLG_H
#define SELECTTESTCASEDLG_H

#include <QDialog>

namespace Ui {
    class CSelectTestcaseDlg;
}

class CSelectTestcaseDlg : public QDialog
{
    Q_OBJECT

public:
    explicit CSelectTestcaseDlg(QWidget *parent = 0);
    ~CSelectTestcaseDlg();

public:
    Ui::CSelectTestcaseDlg *m_pxUi;
};

#endif // SELECTTESTCASEDLG_H
