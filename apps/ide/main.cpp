#include <QtGui/QApplication>

#include "main.h"
#include "mainwindow.h"
#include "cpluginmanager.h"
#include "startupsplashdialog.h"
#include "environmentsettingsdialog.h"

#include "appeventfilter.h"

// Modeless dialog for Test Suite executive.
TestSuiteExecutiveDialog* testSuiteExecutive = NULL;

// Database of all known games.
CGameDatabaseHandler gameDatabase;

// The project container.
CNesicideProject* nesicideProject = (CNesicideProject*)NULL;

int main(int argc, char* argv[])
{
   // Main window of application.
   MainWindow* nesicideWindow;

   QApplication nesicideApplication(argc, argv);
   AppEventFilter nesicideEventFilter;
   nesicideApplication.installEventFilter(&nesicideEventFilter); // Installing the event filter

   QCoreApplication::setOrganizationName("CSPSoftware");
   QCoreApplication::setOrganizationDomain("nesicide.com");
   QCoreApplication::setApplicationName("NESICIDE");

   // Initialize Environment settings.
   EnvironmentSettingsDialog::readSettings();

   // Initialize the game database object...
   if ( EnvironmentSettingsDialog::useInternalGameDatabase() )
   {
      // Use internal resource.
      gameDatabase.initialize(":GameDatabase");
   }
   else
   {
      // Use named file resource.  Default to internal if it's not set.
      gameDatabase.initialize(EnvironmentSettingsDialog::getGameDatabase());
   }

   // Initialize the plugin manager
   pluginManager = new CPluginManager();

   // Run the startup splash
   StartupSplashDialog* splash = new StartupSplashDialog();
   //splash->exec();
   delete splash;

   // Set up default OpenGL format.
   QGLFormat fmt = QGLFormat::defaultFormat();

   // Disable VSYNC waiting.
   fmt.setSwapInterval(0);

   QGLFormat::setDefaultFormat(fmt);

   // Create, show, and execute the main window (UI) thread.
   nesicideWindow = new MainWindow();
   QObject::connect(&nesicideEventFilter,SIGNAL(applicationActivated()),nesicideWindow,SLOT(applicationActivated()));
   nesicideWindow->show();

   int result = nesicideApplication.exec();

   return result;
}