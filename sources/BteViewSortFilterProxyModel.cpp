//=================================================================================================
// Copyright (C) 2016 Pousse Systems
//
// Name           CBteViewSortFilterProxyModel
//
// Description    Model used to sort and filter the data and log of the Main TableView.
//
// Author         Selim Pousse
//=================================================================================================
#include "BteViewSortFilterProxyModel.h"
#include <QColor>
#include <QDebug>

CBteViewSortFilterProxyModel::CBteViewSortFilterProxyModel()
{
}


void CBteViewSortFilterProxyModel::setFilterKeyColumns(const QList<qint32> &filterColumns)
{
    m_mapiColumn_xRegRole.clear();

    foreach(qint32 column, filterColumns)
    {
        m_mapiColumn_xRegRole.insert(column, SRegRole());
    }
}

void CBteViewSortFilterProxyModel::setFilter(qint32 column, const QRegExp &xPattern, qint32 eRole)
{
    if(!m_mapiColumn_xRegRole.contains(column))
    {
        return;
    }

    m_mapiColumn_xRegRole[column].xRegExp = xPattern;
    m_mapiColumn_xRegRole[column].eDataRole = eRole;
}

/**
 * @brief CBteViewSortFilterProxyModel::filterAcceptsRow
 * @param sourceRow row concerned of the source model
 * @param sourceParent source Parent of the TableView
 * @return true if the item in the row indicated by the given source_row and source_parent should be included in the model; otherwise returns false.
  The default implementation returns true if the value held by the relevant item matches the filter string, wildcard string or regular expression.
*/
bool CBteViewSortFilterProxyModel::filterAcceptsRow(int sourceRow, const QModelIndex &sourceParent) const
{
    if(m_mapiColumn_xRegRole.isEmpty())
    {
        return true;
    }

    bool ret = true;
    for(    QMap<qint32, SRegRole>::const_iterator iter = m_mapiColumn_xRegRole.constBegin();
            iter != m_mapiColumn_xRegRole.constEnd();
            ++iter
        )
    {
        /*QModelIndex xIdx = sourceModel()->index(sourceRow, iter.key(), sourceParent);
        if (xIdx.isValid())
        {
            const qint32& eDataRole = iter.value().eDataRole;
            const QRegExp& xRegExp = iter.value().xRegExp;
            const qint32& iCol = iter.key();

            if (!xRegExp.isEmpty() && xIdx.data(eDataRole).isValid())
            {
                ret = (xIdx.data(eDataRole).toString().contains(xRegExp));
                if(!ret)
                {
                    if (iCol!=0 &&
                        xIdx.data(Qt::BackgroundColorRole).isValid() &&
                        xIdx.data(Qt::BackgroundColorRole).value<QColor>()==Qt::yellow)
                    {
                        sourceModel()->setData(xIdx,QColor(Qt::white),Qt::BackgroundColorRole);
                    }
                    break;
                }
                if (iCol!=0)
                {
                    sourceModel()->setData(xIdx,QColor(Qt::yellow),Qt::BackgroundColorRole);
                }
            }
            else
            {
                if (iCol!=0 &&
                    xIdx.data(Qt::BackgroundColorRole).isValid() &&
                    xIdx.data(Qt::BackgroundColorRole).value<QColor>()==Qt::yellow)
                {
                    sourceModel()->setData(xIdx,QColor(Qt::white),Qt::BackgroundColorRole);
                }
                ret = true;
            }
        }*/

        QModelIndex xIdx = sourceModel()->index(sourceRow, iter.key(), sourceParent);
        const qint32& eDataRole = iter.value().eDataRole;
        const QRegExp& xRegExp = iter.value().xRegExp;
        const qint32& iCol = iter.key();

        //if (iCol!=0 && xIdx.data(Qt::BackgroundColorRole).value<QColor>()==Qt::yellow)
        {
            //sourceModel()->setData(xIdx,QColor(Qt::white),Qt::BackgroundColorRole);
        }

        if (!xRegExp.isEmpty())
        {

            ret = (xIdx.data(eDataRole).toString().contains(xRegExp));
            if(!ret)
            {
                return ret;
            }
            if (iCol!=0)
            {
               // sourceModel()->setData(xIdx,QColor(Qt::yellow),Qt::BackgroundColorRole);
            }
        }
    }

    return ret;
}

/**
 * @brief lessThan
 * @param left
 * @param right
 * @return
 */
bool CBteViewSortFilterProxyModel::lessThan(const QModelIndex &, const QModelIndex &) const
{
    return true;
}
