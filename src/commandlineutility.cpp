//#include <QDebug>
#include <QStringListIterator>
#include <QFileInfo>
#include <QTextStream>

#ifdef Q_OS_UNIX
    #if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
#include <QApplication>
    #endif
#endif

#include "commandlineutility.h"
#include "common.h"

#include "eventhandlerfactory.h"

QRegExp CommandLineUtility::trayRegexp = QRegExp("--tray");
QRegExp CommandLineUtility::helpRegexp = QRegExp("(-h|--help)");
QRegExp CommandLineUtility::versionRegexp = QRegExp("(-v|--version)");
QRegExp CommandLineUtility::noTrayRegexp = QRegExp("--no-tray");
QRegExp CommandLineUtility::loadProfileRegexp = QRegExp("--profile");
QRegExp CommandLineUtility::loadProfileForControllerRegexp = QRegExp("--profile-controller");
QRegExp CommandLineUtility::hiddenRegexp = QRegExp("--hidden");
QRegExp CommandLineUtility::unloadRegexp = QRegExp("--unload");
QRegExp CommandLineUtility::startSetRegexp = QRegExp("--startSet");
QRegExp CommandLineUtility::gamepadListRegexp = QRegExp("(-l|--list)");
QRegExp CommandLineUtility::mappingRegexp = QRegExp("--map");
QRegExp CommandLineUtility::qtStyleRegexp = QRegExp("-style");
QRegExp CommandLineUtility::logLevelRegexp = QRegExp("--log-level");
QRegExp CommandLineUtility::eventgenRegexp = QRegExp("--eventgen");

#ifdef Q_OS_UNIX
QRegExp CommandLineUtility::daemonRegexp = QRegExp("--daemon|-d");

  #ifdef WITH_X11
    QRegExp CommandLineUtility::displayRegexp = QRegExp("--display");
  #endif
#endif

QStringList CommandLineUtility::eventGeneratorsList = EventHandlerFactory::buildEventGeneratorList();

CommandLineUtility::CommandLineUtility(QObject *parent) :
    QObject(parent)
{
    launchInTray = false;
    helpRequest = false;
    versionRequest = false;
    hideTrayIcon = false;
    profileLocation = "";
    controllerNumber = 0;
    encounteredError = false;
    hiddenRequest = false;
    unloadProfile = false;
    startSetNumber = 0;
    daemonMode = false;
    displayString = "";
    listControllers = false;
    mappingController = false;
    currentLogLevel = Logger::LOG_INFO;

    eventGenerator = EventHandlerFactory::fallBackIdentifier();
}

void CommandLineUtility::parseArguments(QStringList& arguments)
{
    QStringListIterator iter(arguments);
    //QTextStream out(stdout);
    //QTextStream errorsteam(stderr);

    while (iter.hasNext() && !encounteredError)
    {
        QString temp = iter.next();
        if (helpRegexp.exactMatch(temp))
        {
            helpRequest = true;
        }
        else if (versionRegexp.exactMatch(temp))
        {
            versionRequest = true;
        }
        else if (trayRegexp.exactMatch(temp))
        {
            launchInTray = true;
            hideTrayIcon = false;
        }
        else if (noTrayRegexp.exactMatch(temp))
        {
            hideTrayIcon = true;
            launchInTray = false;
        }
        else if (loadProfileRegexp.exactMatch(temp))
        {
            if (iter.hasNext())
            {
                temp = iter.next();
                QFileInfo fileInfo(temp);
                if (fileInfo.exists())
                {
                    if (fileInfo.suffix() != "amgp" && fileInfo.suffix() != "xml")
                    {
                        setErrorMessage(tr("Profile location %1 is not an XML file.").arg(temp));
                        //errorsteam << tr("Profile location %1 is not an XML file.").arg(temp) << endl;
                        //encounteredError = true;
                    }
                    else
                    {
                        profileLocation = fileInfo.absoluteFilePath();
                    }
                }
                else
                {
                    setErrorMessage(tr("Profile location %1 does not exist.").arg(temp));
                    //errorsteam << tr("Profile location %1 does not exist.").arg(temp) << endl;
                    //encounteredError = true;
                }
            }
        }
        else if (loadProfileForControllerRegexp.exactMatch(temp))
        {
            if (iter.hasNext())
            {
                temp = iter.next();

                bool validNumber = false;
                int tempNumber = temp.toInt(&validNumber);
                if (validNumber)
                {
                    controllerNumber = tempNumber;
                }
                else if (!temp.isEmpty())
                {
                    controllerIDString = temp;
                }
                else
                {
                    setErrorMessage(tr("Controller identifier is not a valid value."));
                    //errorsteam << tr("Controller identifier is not a valid value.") << endl;
                    //encounteredError = true;
                }
            }
        }
        else if (hiddenRegexp.exactMatch(temp))
        {
            hiddenRequest = true;
        }
        else if (unloadRegexp.exactMatch(temp))
        {
            unloadProfile = true;
            profileLocation = "";

            if (iter.hasNext())
            {
                temp = iter.next();

                if (!isPossibleCommand(temp))
                {
                    // A value has been passed. Attempt
                    // to validate the value.
                    bool validNumber = false;
                    int tempNumber = temp.toInt(&validNumber);
                    if (validNumber)
                    {
                        controllerNumber = tempNumber;
                    }
                    else if (!temp.isEmpty())
                    {
                        controllerIDString = temp;
                    }
                    else
                    {
                        setErrorMessage(tr("Controller identifier is not a valid value."));
                        //errorsteam << tr("Controller identifier is not a valid value.") << endl;
                        //encounteredError = true;
                    }
                }
                else
                {
                    // Grabbed a possible command-line option.
                    // Move iterator back to previous item.
                    iter.previous();
                }
            }
        }
        else if (startSetRegexp.exactMatch(temp))
        {
            if (iter.hasNext())
            {
                temp = iter.next();

                bool validNumber = false;
                int tempNumber = temp.toInt(&validNumber);
                if (validNumber && tempNumber >= 1 && tempNumber <= 8)
                {
                    startSetNumber = tempNumber;
                }
                else if (validNumber)
                {
                    setErrorMessage(tr("An invalid set number was specified."));
                    //errorsteam << tr("An invalid set number was specified.") << endl;
                    //encounteredError = true;
                }

                if (iter.hasNext())
                {
                    temp = iter.next();

                    if (!isPossibleCommand(temp))
                    {
                        if (validNumber)
                        {
                            controllerNumber = tempNumber;
                        }
                        else if (!temp.isEmpty())
                        {
                            controllerIDString = temp;
                        }
                        else
                        {
                            setErrorMessage(tr("Controller identifier is not a valid value."));
                            //errorsteam << tr("Controller identifier is not a valid value.") << endl;
                            //encounteredError = true;
                        }
                    }
                    else
                    {
                        // Grabbed a possible command-line option.
                        // Move iterator back to previous item.
                        iter.previous();
                    }
                }
            }
            else
            {
                setErrorMessage(tr("No set number was specified."));
                //errorsteam << tr("No set number was specified.") << endl;
                //encounteredError = true;
            }
        }
#ifdef USE_SDL_2
        else if (gamepadListRegexp.exactMatch(temp))
        {
            listControllers = true;
        }
        else if (mappingRegexp.exactMatch(temp))
        {
            if (iter.hasNext())
            {
                temp = iter.next();

                bool validNumber = false;
                int tempNumber = temp.toInt(&validNumber);
                if (validNumber)
                {
                    controllerNumber = tempNumber;
                    mappingController = true;
                }
                else if (!temp.isEmpty())
                {
                    controllerIDString = temp;
                    mappingController = true;
                }
                else
                {
                    setErrorMessage(tr("Controller identifier is not a valid value."));
                    //errorsteam << tr("Controller identifier is not a valid value.") << endl;
                    //encounteredError = true;
                }
            }
            else
            {
                setErrorMessage(tr("No controller was specified."));
                //errorsteam << tr("No controller was specified.") << endl;
                //encounteredError = true;
            }
        }
#endif

#ifdef Q_OS_UNIX
        else if (daemonRegexp.exactMatch(temp))
        {
            daemonMode = true;
        }
  #ifdef WITH_X11
        else if (displayRegexp.exactMatch(temp))
        {
            if (iter.hasNext())
            {
                displayString = iter.next();
            }
            else
            {
                setErrorMessage(tr("No display string was specified."));
                //errorsteam << tr("No display string was specified.") << endl;
                //encounteredError = true;
            }
        }
  #endif
#endif

#if (defined (Q_OS_UNIX) && defined(WITH_UINPUT) && defined(WITH_XTEST)) \
     || (defined(Q_OS_WIN) && defined(WITH_VMULTI))
        else if (eventgenRegexp.exactMatch(temp))
        {
            if (iter.hasNext())
            {
                QString temp = iter.next();

                if (!eventGeneratorsList.contains(temp))
                {
                    eventGenerator = "";
                    setErrorMessage(tr("An invalid event generator was specified."));
                    //errorsteam << tr("An invalid event generator was specified.") << endl;
                    //encounteredError = true;
                }
                else
                {
                    eventGenerator = temp;
                }
            }
            else
            {
                setErrorMessage(tr("No event generator string was specified."));
                //errorsteam << tr("No event generator string was specified.") << endl;
                //encounteredError = true;
            }
        }
#endif

        else if (qtStyleRegexp.exactMatch(temp))
        {
            if (iter.hasNext())
            {
                // Skip over argument
                iter.next();
            }
            else
            {
                setErrorMessage(tr("Qt style flag was detected but no style was specified."));
                //errorsteam << tr("Qt style flag was detected but no style was specified.") << endl;
                //encounteredError = true;
            }
        }
        else if (logLevelRegexp.exactMatch(temp))
        {
            if (iter.hasNext())
            {
                QString temp = iter.next();
                if (temp == "debug")
                {
                    currentLogLevel = Logger::LOG_DEBUG;
                }
                else if (temp == "info")
                {
                    currentLogLevel = Logger::LOG_INFO;
                }
                /*else if (temp == "warn")
                {
                    currentLogLevel = Logger::LOG_WARNING;
                }
                else if (temp == "error")
                {
                    currentLogLevel = Logger::LOG_ERROR;
                }
                */
            }
            else
            {
                setErrorMessage(tr("No log level specified."));
                //errorsteam << tr("No log level specified.") << endl;
                //encounteredError = true;
            }

        }
        else if (isPossibleCommand(temp))
        {
            // Flag is unrecognized. Assume that it is a Qt option.
            if (iter.hasNext())
            {
                // Check next argument
                QString nextarg = iter.next();
                if (isPossibleCommand(nextarg))
                {
                    // Flag likely didn't take an argument. Move iterator
                    // back.
                    iter.previous();
                }
            }
        }
        // Check if this is the last argument. If it is and no command line flag
        // is active, the final argument is likely a profile that has
        // been specified.
        else if (!temp.isEmpty() && !iter.hasNext())
        {
            // If the file exists and it is an xml file, assume that it is a
            // profile.
            QFileInfo fileInfo(temp);
            if (fileInfo.exists())
            {
                if (fileInfo.suffix() != "amgp" && fileInfo.suffix() != "xml")
                {
                    setErrorMessage(tr("Profile location %1 is not an XML file.").arg(temp));
                    //errorsteam << tr("Profile location %1 is not an XML file.").arg(temp) << endl;
                    //encounteredError = true;
                }
                else
                {
                    profileLocation = fileInfo.absoluteFilePath();
                }
            }
            else
            {
                setErrorMessage(tr("Profile location %1 does not exist.").arg(temp));
                //errorsteam << tr("Profile location %1 does not exist.").arg(temp) << endl;
                //encounteredError = true;
            }
        }
    }
}

bool CommandLineUtility::isLaunchInTrayEnabled()
{
    return launchInTray;
}

void CommandLineUtility::printHelp()
{
    QTextStream out(stdout);
    out << tr("antimicro version") << " " << PadderCommon::programVersion << endl;
    out << tr("Usage: antimicro [options] [profile]") << endl;
    out << endl;
    out << tr("Options") << ":" << endl;
    out << "-h, --help                    " << " " << tr("Print help text.") << endl;
    out << "-v, --version                 " << " " << tr("Print version information.") << endl;
    out << "--tray                        " << " " << tr("Launch program in system tray only.") << endl;
    out << "--no-tray                     " << " " << tr("Launch program with the tray menu disabled.") << endl;
    out << "--hidden                      " << " " << tr("Launch program without the main window\n                               displayed.") << endl;
    out << "--profile <location>          " << " " <<
           tr("Launch program with the configuration file\n                               selected as the default for selected\n                               controllers. Defaults to all controllers.")
        << endl;
    out << "--profile-controller <value>  " << " "
        << tr("Apply configuration file to a specific\n                               controller. Value can be a\n                               controller index, name, or GUID.")
        << endl;
    out << "--unload [<value>]            " << " " << tr("Unload currently enabled profile(s). \n                               Value can be a controller index, name, or GUID.")
        << endl;
    out << "--startSet <number> [<value>] " << " " << tr("Start joysticks on a specific set.   \n                               Value can be a controller index, name, or GUID.")
        << endl;
#ifdef Q_OS_UNIX
    out << "-d, --daemon                  " << " "
        << tr("Launch program as a daemon.") << endl;
    out << "--log-level (debug|info)      " << " " << tr("Enable logging.") << endl;

    #ifdef WITH_X11
        #if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    if (QApplication::platformName() == QStringLiteral("xcb"))
    {
        #endif
    out << "--display <value>             " << " "
        << tr("Use specified display for X11 calls.\n"
              "                               Useful for ssh.")
        << endl;
        #if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    }
        #endif
    #endif
#endif

#if defined(Q_OS_UNIX) && defined(WITH_UINPUT) && defined(WITH_XTEST)
    out << "--eventgen (xtest|uinput)     " << " "
        << tr("Choose between using XTest support and uinput\n"
           "                               support for event generation. Default: xtest.")
        << endl;
#elif defined(Q_OS_WIN) && defined(WITH_VMULTI)
    out << "--eventgen (sendinput|vmulti) " << " "
        << tr("Choose between using SendInput and vmulti\n"
           "                           support for event generation. Default: sendinput.")
        << endl;
#endif

#ifdef USE_SDL_2
    out << "-l, --list                    " << " "
        << tr("Print information about joysticks detected by \n"
              "                               SDL.") << endl;
    out << "--map <value>                 " << " "
        << tr("Open game controller mapping window of selected\n"
              "                               controller. Value can be a controller index or\n"
              "                               GUID.")
        << endl;
#endif

}

QString CommandLineUtility::generateHelpString()
{
    QString temp;
    QTextStream out(&temp);
    out << tr("antimicro version") << " " << PadderCommon::programVersion << endl;
    out << tr("Usage: antimicro [options] [profile]") << endl;
    out << endl;
    out << tr("Options") << ":" << endl;
    out << "-h, --help                    " << " " << tr("Print help text.") << endl;
    out << "-v, --version                 " << " " << tr("Print version information.") << endl;
    out << "--tray                        " << " " << tr("Launch program in system tray only.") << endl;
    out << "--no-tray                     " << " " << tr("Launch program with the tray menu disabled.") << endl;
    out << "--hidden                      " << " " << tr("Launch program without the main window\n                               displayed.") << endl;
    out << "--profile <location>          " << " " <<
           tr("Launch program with the configuration file\n                               selected as the default for selected\n                               controllers. Defaults to all controllers.")
        << endl;
    out << "--profile-controller <value>  " << " "
        << tr("Apply configuration file to a specific\n                               controller. Value can be a\n                               controller index, name, or GUID.")
        << endl;
    out << "--unload [<value>]            " << " " << tr("Unload currently enabled profile(s). \n                               Value can be a controller index, name, or GUID.")
        << endl;
    out << "--startSet <number> [<value>] " << " " << tr("Start joysticks on a specific set.   \n                               Value can be a controller index, name, or GUID.")
        << endl;
#ifdef Q_OS_UNIX
    out << "-d, --daemon                  " << " "
        << tr("Launch program as a daemon.") << endl;
    out << "--log-level (debug|info)      " << " " << tr("Enable logging.") << endl;
    #ifdef WITH_X11
        #if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    if (QApplication::platformName() == QStringLiteral("xcb"))
    {
        #endif
    out << "--display <value>             " << " "
        << tr("Use specified display for X11 calls.\n"
              "                               Useful for ssh.")
        << endl;
        #if (QT_VERSION >= QT_VERSION_CHECK(5, 0, 0))
    }
        #endif
    #endif
#endif

#if defined(Q_OS_UNIX) && defined(WITH_UINPUT) && defined(WITH_XTEST)
    out << "--eventgen (xtest|uinput)     " << " "
        << tr("Choose between using XTest support and uinput\n"
           "                               support for event generation. Default: xtest.")
        << endl;
#elif defined(Q_OS_WIN) && defined(WITH_VMULTI)
    out << "--eventgen (sendinput|vmulti) " << " "
        << tr("Choose between using SendInput and vmulti\n"
           "                           support for event generation. Default: sendinput.")
        << endl;
#endif

#ifdef USE_SDL_2
    out << "-l, --list                    " << " "
        << tr("Print information about joysticks detected by \n"
              "                               SDL.") << endl;
    out << "--map <value>                 " << " "
        << tr("Open game controller mapping window of selected\n"
              "                               controller. Value can be a controller index or\n"
              "                               GUID.")
        << endl;
#endif

    return temp;
}

bool CommandLineUtility::isHelpRequested()
{
    return helpRequest;
}

bool CommandLineUtility::isVersionRequested()
{
    return versionRequest;
}

void CommandLineUtility::printVersionString()
{
    QTextStream out(stdout);
    out << tr("antimicro version") << " " << PadderCommon::programVersion << endl;
}

QString CommandLineUtility::generateVersionString()
{
    QString temp;
    QTextStream out(&temp);
    out << tr("antimicro version") << " " << PadderCommon::programVersion;

    return temp;
}

bool CommandLineUtility::isTrayHidden()
{
    return hideTrayIcon;
}

bool CommandLineUtility::hasProfile()
{
    return !profileLocation.isEmpty();
}

bool CommandLineUtility::hasControllerNumber()
{
    return (controllerNumber > 0);
}

QString CommandLineUtility::getProfileLocation()
{
    return profileLocation;
}

unsigned int CommandLineUtility::getControllerNumber()
{
    return controllerNumber;
}

bool CommandLineUtility::hasError()
{
    return encounteredError;
}

bool CommandLineUtility::isHiddenRequested()
{
    return hiddenRequest;
}

bool CommandLineUtility::hasControllerID()
{
    return !controllerIDString.isEmpty();
}

QString CommandLineUtility::getControllerID()
{
    return controllerIDString;
}

bool CommandLineUtility::isUnloadRequested()
{
    return unloadProfile;
}

unsigned int CommandLineUtility::getStartSetNumber()
{
    return startSetNumber;
}

unsigned int CommandLineUtility::getJoyStartSetNumber()
{
    return startSetNumber-1;
}

bool CommandLineUtility::isPossibleCommand(QString temp)
{
    bool result = false;

    if (temp.startsWith("--") || temp.startsWith("-"))
    {
        result = true;
    }

    return result;
}

bool CommandLineUtility::shouldListControllers()
{
    return listControllers;
}

bool CommandLineUtility::shouldMapController()
{
    return mappingController;
}

QString CommandLineUtility::getEventGenerator()
{
    return eventGenerator;
}

#ifdef Q_OS_UNIX
bool CommandLineUtility::launchAsDaemon()
{
    return daemonMode;
}

QString CommandLineUtility::getDisplayString()
{
    return displayString;
}

#endif

Logger::LogLevel CommandLineUtility::getCurrentLogLevel()
{
    return currentLogLevel;
}

QString CommandLineUtility::getErrorText()
{
    return errorText;
}

void CommandLineUtility::setErrorMessage(QString temp)
{
    errorText = temp;
    encounteredError = true;
}
