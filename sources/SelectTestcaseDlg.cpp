//=================================================================================================
// Copyright (C) 2016 Pousse Systems
//
// Name           CSelectTestcaseDlg
//
// Description    Dialog Class to select or unselect the Test-Cases.
//
// Author         Selim Pousse
//=================================================================================================
#include "SelectTestcaseDlg.h"
#include "ui_SelectTestcaseDlg.h"

CSelectTestcaseDlg::CSelectTestcaseDlg(QWidget *parent) :
    QDialog(parent),
    m_pxUi(new Ui::CSelectTestcaseDlg)
{
    m_pxUi->setupUi(this);
}

CSelectTestcaseDlg::~CSelectTestcaseDlg()
{
    delete m_pxUi;
}
