//=================================================================================================
// Copyright (C) 2016 Pousse Systems
//
// Name           CBteViewSortFilterProxyModel
//
// Description    Model used to sort and filter the data and log of the Main TableView.
//
// Author         Selim Pousse
//=================================================================================================
#ifndef BTEVIEWSORTFILTERPROXYMODEL_H
#define BTEVIEWSORTFILTERPROXYMODEL_H

#include <QSortFilterProxyModel>

class CBteViewSortFilterProxyModel : public QSortFilterProxyModel
{

    public:
        typedef struct
        {
            QRegExp xRegExp;
            qint32 eDataRole;
        }SRegRole;

        CBteViewSortFilterProxyModel();

        void setFilterKeyColumns(const QList<qint32> &filterColumns);
        void setFilter(qint32 column, const QRegExp &xPattern, qint32 eRole);

    protected:
        /**
         * @brief CBteViewSortFilterProxyModel::filterAcceptsRow
         * @param sourceRow row concerned of the source model
         * @param sourceParent source Parent of the TableView
         * @return true if the item in the row indicated by the given source_row and source_parent should be included in the model; otherwise returns false.
          The default implementation returns true if the value held by the relevant item matches the filter string, wildcard string or regular expression.
        */
        bool filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const;

        /**
         * @brief lessThan
         * @param left
         * @param right
         * @return
         */
        bool lessThan(const QModelIndex &, const QModelIndex &) const;

private:
    QMap<qint32, SRegRole> m_mapiColumn_xRegRole;
};

#endif // BTEVIEWSORTFILTERPROXYMODEL_H
