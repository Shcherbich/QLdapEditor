/*!
\file
\brief Header file for dialog 'Add/Edit LDAP Entity' class

File contains  declarations for dialog 'Add/EditLDPA Entity' class
*/

#ifndef CLdapNewEntryDialog_H
#define CLdapNewEntryDialog_H

#include <QString>
#include <QVector>
#include <QDialog>
#include <CLdapData.h>

namespace Ui {
class CLdapNewEntryDialog;
}

namespace ldapeditor
{
/*!
 * @ingroup ldapeditor
 * @brief The CLdapNewEntryDialog class
 *
 * The CLdapNewEntryDialogclass, used for adding new/editLDAP entity
 */
    class CLdapNewEntryDialog : public QDialog
    {
        Q_OBJECT

    public:
        /*!
         * \brief Constructor CLdapNewEntryDialog
         * \param parent pointer to parent QObject
         * \param parentDn parent DN string
         * \param ldapData reference to ldapcore::CLdapData
         *
         * Used for creating new LDAP entity
         */
        explicit CLdapNewEntryDialog(QWidget *parent, QString parentDn, ldapcore::CLdapData& ldapData);

        /*!
         * \brief Constructor CLdapNewEntryDialog
         * \param parent pointer to parent QObject
         * \param dn DN string
         * \param rdn RDN string
         * \param structuralClass structural class string
         * \param auxClasses array of auxiliary classes
         * \param ldapData reference to ldapcore::CLdapData
         *
         * Used for editing LDAP entity
         */
        explicit CLdapNewEntryDialog(QWidget *parent, QString dn, QString rdn, std::string& structuralClass,
                                     std::vector<std::string>& auxClasses,  ldapcore::CLdapData& ldapData);

        /*!
        * @brief Destructor of   CLdapNewEntryDialog class
        */
        ~CLdapNewEntryDialog();

        /*!
         * \brief returns RDN string of entity
         * \return string
         */
        QString rdn() const;

        /*!
         * \brief returns array of names for selected auxiliary claases
         * \return QStringList
         */
        QStringList selectedClasses() const;

        /*!
         * \brief returns name of structural class
         * \return string
         */
        QString structuralClass() const;

        /*!
         * \brief methods fills list of all auxiliary classes
         * \param structuralClass - name of structural class
         */
        void fillListAll(QString structuralClass);

    private slots:
        /*!
         * \brief Protected slot , called when user wants to close dialog
         */
        void onCloseClicked();

        /*!
         * \brief Protected slot, called when user clicked on 'Ok' button
         */
        void onOkClicked();

        /*!
         * \brief Protected slot, called when user changed structural class in combobox
         */
        void onStructuralComboChanged(const QString&);

    private:
        Ui::CLdapNewEntryDialog* ui;        ///< pointer to UI implementation
        QString          m_rdn;             ///< RDN string member
        QStringList vSelectedClasses;  ///< array of selected auxiliary classes
        ldapcore::CLdapData& m_LdapData;    ///< CLDapData reference member
        QString          m_structuralClass; ///< Structural class string
        bool m_editMode {false};            /// is dialof in Edit (true) ot Add New (false) entity mode
    };
}//namespace ldapeditor

#endif // CLdapNewEntryDialog_H
