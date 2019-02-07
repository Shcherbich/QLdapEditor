/*!
\file
\brief Header file for attributes editor delegates

File contains  declarations for attributes editor delegates
*/

#ifndef LDAPDATAEDITDELEGATE_H
#define LDAPDATAEDITDELEGATE_H

#include <QStyledItemDelegate>
#include "CLdapAttribute.h"
#include <QDateTime>

namespace ldapcore{
class CLdapEntry;
}

namespace ldapeditor
{
/*!
 * @ingroup ldapeditor
 * @brief Attribute editor delegate class
 *
 * Attribute editor delegate class
 */
class CLdapDataEditDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    /*!
     * @brief Constructor CLdapDataEditDelegate
     * @param parent pointer to parent QWidget object
     */
    explicit CLdapDataEditDelegate(QObject *parent = nullptr);

    /*!
     * @brief setLdapEntry
     * @param entry pointer to ldapcore::CLdapEntry
     */
    void setLdapEntry(ldapcore::CLdapEntry* entry);

    /*!
     * @brief Create Editor widget
     * @param parent pointer to parent widget
     * @param option style options
     * @param index index of item for edit
     * @return pointer to created edtitor QWidget or nullptr if error
     */
    QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const override;

    /*!
     * @brief Sets editor data
     * @param editor pointer to created editor
     * @param index index of item for edit
     */
    void setEditorData(QWidget *editor, const QModelIndex &index) const override;

    /*!
     * @brief Saves editor data into model after editing
     * @param editor pointer to editor widget
     * @param model pointer to data model
     * @param index index of item for edit
     */
    void setModelData(QWidget *editor, QAbstractItemModel *model,const QModelIndex &index) const override;

    /*!
     * @brief Updates editor's geometry
     * @param editor pointer to editor widget
     * @param option style options
     * @param index index of item for edit
     */
    void updateEditorGeometry(QWidget *editor,const QStyleOptionViewItem &option, const QModelIndex &index) const override;

    /*!
     * @brief Check is it possible to delete row
     * @param index index of item/row for deleting
     * @return true if it's possible to delete row, false - not possible
     */
    bool canDeleteRow(const QModelIndex &index) const;

    /*!
     * @brief show popup list of Combobox
     */
    void expandEditor() const;
signals:

public slots:

protected:
    /*!
     * @brief The GeneralizedTimeFormat enum
     * Set of possible formats for GeneralizedTime values
     */
    enum class GeneralizedTimeFormat{
        formatUnknown =0,      ///< unknown format
        formatTimeZoneWest,    ///< yyyymmddhhmmss.ffffff+hhmm UTC West offset
        formatTimeZoneEast,    ///< yyyymmddhhmmss.ffffff-hhmm UTC East offset
        formatTimeZoneUtc,     ///< yyyymmddhhmmss.ffffffZ UTC
        formatTimeZoneLocal    ///< yyyymmddhhmm[ss.ffffff] Local
    };

    /*!
      @brief Type of Structure with information about GeneralizedTime format
    */
    typedef struct {
        GeneralizedTimeFormat format{GeneralizedTimeFormat::formatUnknown}; ///< GeneralizedTimeFormat format
        QDateTime dateTime;       ///< QDateTime value
        QString   editFormat;     ///< Format string for QDateTimeEdit editor
    } GeneralizedTimeFormatInfo;

    /*!
     * @brief Method detects format of DataTime string and parses datetime value
     * @param strDateTime input dateTime string
     * @return GeneralizedTimeFormatInfo structure
     */
    GeneralizedTimeFormatInfo formatDataTimeFromString(const QString& strDateTime) const;

    /*!
     * \brief Methhod makes DateTime value based on source format of Datetime
     * \param formatInfo - saved information about datetime value
     * \return Qstring string with compsed DateTime value
     */
    QString makeDateTimeValue(const GeneralizedTimeFormatInfo &formatInfo) const;

protected:
    mutable ldapcore::AttrType m_attrType;                  ///< member type of attribute
    mutable ldapcore::CLdapEntry* m_entry{nullptr};         ///< member pointer to CLdapEntry class
    mutable QWidget* editor{nullptr};                       ///< memeber pointer to editor widget
    mutable GeneralizedTimeFormatInfo m_dateTimeFormatInfo; ///< memeber GeneralizedTimeFormatInfo
};


} //namespace ldapeditor

#endif // LDAPDATAEDITDELEGATE_H
