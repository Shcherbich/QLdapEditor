/*!
\file
\brief Header file for dialog 'Search' class

File contains  declarations for dialog 'Search' class
*/
#ifndef LDAPSEARCHDIALOG_H
#define LDAPSEARCHDIALOG_H

#include <QDialog>

namespace Ui {
class CLDapSearchDialog;
}

namespace ldapcore {
class CLdapData;
}

namespace ldapeditor
{
    /*!
     * @ingroup ldapeditor
     * @brief The CLDapSearchDialog class
     *
     * The CLDapSearchDialog, used for search entity
     */
    class CLDapSearchDialog : public QDialog
    {
        Q_OBJECT

    public:
        /*!
         * \brief Constructor CLDapSearchDialog
         * \param ldapData reference to ldapcore::CLdapData
         * \param parent pointer to parent QObject
         */
        explicit CLDapSearchDialog(ldapcore::CLdapData &ldapData, QWidget *parent = nullptr);

        /*!
         * \brief Destructor CLDapSearchDialog
         */
        ~CLDapSearchDialog();
    private slots:
        /*!
         * \brief Private slot, called by click on 'Close' button
         */
        void onCloseClicked();

        /*!
         * \brief Private slot, called by click on 'Search' button
         */
        void onSearchClicked();

    private:
        Ui::CLDapSearchDialog *ui;          ///< pointer to UI implementation memeber
        ldapcore::CLdapData& m_LdapData;    ///< referent to ldapcore::CLdapData  member
        /*!
         * \brief Private method enables UI controls
         */
        void enableSearch();

        /*!
         * \brief Private method prepare results list
         * \param dn - base DN string
         * \param headers - array of headers(attributes)
         * \return array of values for Row
         */
        QStringList prepareResultRow(const QString &dn, const QStringList &headers);
    };
}//namespace ldapeditor

#endif // LDAPSEARCHDIALOG_H
