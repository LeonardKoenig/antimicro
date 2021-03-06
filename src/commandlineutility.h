#ifndef COMMANDLINEPARSER_H
#define COMMANDLINEPARSER_H

#include <QObject>
#include <QStringList>
#include <QRegExp>

#include "logger.h"

class CommandLineUtility : public QObject
{
    Q_OBJECT
public:
    explicit CommandLineUtility(QObject *parent = 0);

    void parseArguments(QStringList& arguments);
    bool isLaunchInTrayEnabled();
    bool isHelpRequested();
    bool isVersionRequested();
    bool isTrayHidden();
    bool hasProfile();
    bool hasControllerNumber();
    bool hasControllerID();

    QString getProfileLocation();
    unsigned int getControllerNumber();
    QString getControllerID();
    bool isHiddenRequested();
    bool isUnloadRequested();
    bool shouldListControllers();
    bool shouldMapController();

    unsigned int getStartSetNumber();
    unsigned int getJoyStartSetNumber();

    QString getEventGenerator();

#ifdef Q_OS_UNIX
    bool launchAsDaemon();
    QString getDisplayString();
#endif

    void printHelp();
    void printVersionString();

    QString generateHelpString();
    QString generateVersionString();

    bool hasError();
    Logger::LogLevel getCurrentLogLevel();
    QString getErrorText();

protected:
    bool isPossibleCommand(QString temp);
    void setErrorMessage(QString temp);

    bool launchInTray;
    bool helpRequest;
    bool versionRequest;
    bool hideTrayIcon;
    QString profileLocation;
    unsigned int controllerNumber;
    QString controllerIDString;
    bool encounteredError;
    bool hiddenRequest;
    bool unloadProfile;
    unsigned int startSetNumber;
    bool daemonMode;
    QString displayString;
    bool listControllers;
    bool mappingController;
    QString eventGenerator;
    QString errorText;
    Logger::LogLevel currentLogLevel;

    static QRegExp trayRegexp;
    static QRegExp helpRegexp;
    static QRegExp versionRegexp;
    static QRegExp noTrayRegexp;
    static QRegExp loadProfileRegexp;
    static QRegExp loadProfileForControllerRegexp;
    static QRegExp hiddenRegexp;
    static QRegExp unloadRegexp;
    static QRegExp startSetRegexp;
    static QRegExp gamepadListRegexp;
    static QRegExp mappingRegexp;
    static QRegExp qtStyleRegexp;
    static QRegExp logLevelRegexp;
    static QRegExp eventgenRegexp;
    static QStringList eventGeneratorsList;

#ifdef Q_OS_UNIX
    static QRegExp daemonRegexp;
    static QRegExp displayRegexp;
#endif
    
signals:
    
public slots:
    
};

#endif // COMMANDLINEPARSER_H
