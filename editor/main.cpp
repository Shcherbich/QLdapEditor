#include "mainwindow.h"
#include "connectiondialog.h"
#include "ldapsettings.h"
#include "CLdapData.h"
#include <QApplication>
#include <QSettings>
#include <signal.h>
#include <unistd.h>
#include <cerrno>
#include <system_error>
#include <iostream>

static sigset_t theMask;
static int count = 0;

static void signalWrapper(int theSignalNumber, siginfo_t* theSignalDescription, void* theUserContext)
{
    Q_UNUSED(theSignalDescription);
    Q_UNUSED(theUserContext);

	// Do something here as reaction to you SIGPIPE
	// This is better way to react on such things

	if (SIGPIPE == theSignalNumber)
	{
		std::cout << "Got signal SIGPIPE" << std::endl;
	}
	else
	{
		std::cout << "Got signal " << theSignalNumber << std::endl;
	}


	// Reinstall handler
	struct ::sigaction sa;
	sa.sa_sigaction = &signalWrapper;
	sa.sa_mask = theMask;
	sa.sa_flags = SA_SIGINFO;

	try
	{
		if (::sigaction(theSignalNumber, &sa, NULL) == -1)
		{
			//throw std::error_code(errno, std::system_category());
		}
	}
	catch (const std::error_code& ec)
	{
		std::cerr << ec << std::endl;
	}

	count++;
}

void setupSignalHandlers()
{
	struct ::sigaction sa;

	// Prepare mask
	sigemptyset(&theMask);
	sigaddset(&theMask, SIGPIPE);
	// Add some more if you need it to process

	sa.sa_mask      = theMask;
	sa.sa_flags     = SA_SIGINFO;
	sa.sa_sigaction = &signalWrapper;

	// Perform setup
	try
	{
		if (::sigaction(SIGPIPE, &sa, NULL) == -1)
		{
			//throw std::error_code(errno, std::system_category());
		}
	}
	catch (const std::error_code& ec)
	{
		std::cerr << ec << std::endl;
	}
}


int main(int argc, char* argv[])
{
	setupSignalHandlers();

	QApplication a(argc, argv);


	ldapcore::CLdapData ldapData;
ldapeditor::CConnectionDialog connectionDlg(ldapData);
	if (connectionDlg.exec() == QDialog::Rejected)
	{
		return 0;
	}

    ldapeditor::CLdapSettings* settings = connectionDlg.settings();
    if(!settings)
    {
        return 0;
    }

    ldapeditor::MainWindow w(*settings, ldapData);
	w.show();
	return a.exec();
}
