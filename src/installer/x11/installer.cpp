// _____________       ________            _____        ______ _____
// ___  __ \__(_)___  _____  _/_______________(_)______ ___  /___  /_
// __  /_/ /_  /__  |/_/__  /__  __ \_  ___/_  /__  __ `/_  __ \  __/
// _  ____/_  / __>  < __/ / _  / / /(__  )_  / _  /_/ /_  / / / /_
// /_/     /_/  /_/|_| /___/ /_/ /_//____/ /_/  _\__, / /_/ /_/\__/
//                                              /____/
// ----------------------------------------------------------------------------
// This file is part of PixInsight X11 UNIX/Linux Installer
// ----------------------------------------------------------------------------
// 2024/12/11 17:48:57 UTC
// installer.cpp
// ----------------------------------------------------------------------------
// Copyright (c) 2013-2024 Pleiades Astrophoto S.L.
// ----------------------------------------------------------------------------

#if !defined( __PCL_FREEBSD ) && !defined( __PCL_LINUX )
#error This program can only be built on FreeBSD and Linux platforms.
#endif

#include <unistd.h>
#include <cstdio> // fileno()
#include <signal.h> // kill()
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <iostream>

#include <pcl/Arguments.h>
#include <pcl/Exception.h>
#include <pcl/File.h>

/*
 * The installer version numbers identify the PixInsight core application
 * version being installed.
 */
#include "../../core/Components/Application/Version.h"

/*
 * Standard exit codes.
 */
#define EXIT_OK      0
#define EXIT_ERROR   1
#define EXIT_CANCEL  2

using namespace pcl;

// ----------------------------------------------------------------------------

/*
 * Install or uninstall PixInsight on X11 UNIX and Linux platforms according to
 * freedesktop.org standards and common practices on GNOME and KDE desktops.
 *
 * The relevant parts of this program are based on the following documents:
 *
 * https://specifications.freedesktop.org/desktop-entry-spec/latest/
 * https://specifications.freedesktop.org/menu-spec/latest/
 * https://specifications.freedesktop.org/icon-theme-spec/latest/
 * https://specifications.freedesktop.org/icon-theme-spec/latest/ar01s07.html
 * https://developer.gnome.org/integration-guide/stable/icons.html.en
 */
class PixInsightX11Installer
{
public:

   enum Task { NoTask, InstallTask, UninstallTask, ShowVersionTask, ShowHelpTask };

   PixInsightX11Installer( int argc, const char** argv );

   bool Perform();

private:

   /*
    * Install PixInsight by default.
    */
   Task m_task = InstallTask;

   /*
    * Installation source and target directories.
    */
   String m_executablePath;
   String m_baseDir;
   String m_sourceDir;
   String m_installDir;
   String m_installDesktopDir;
   String m_installMIMEDir;
   String m_installIconsDir;

   /*
    * We create a launcher script on /bin by default, so one can say just
    * 'PixInsight' from a terminal.
    */
   bool m_createBinLauncher = true;

   /*
    * We do not backup a previous installation by default.
    */
   bool m_removePrevious = true;

   /*
    * By default, ask for user confirmation with a 'yes/no' prompt.
    */
   bool m_forceYes = false;

   /*
    * The application launcher script on /bin
    */
   String m_binLauncherFile;

   /*
    * Desktop entry file:
    * https://specifications.freedesktop.org/desktop-entry-spec/latest/
    */
   String m_desktopEntryFile;

   /*
    * Shared MIME info XML file:
    * https://specifications.freedesktop.org/shared-mime-info-spec/shared-mime-info-spec-latest.html
    */
   String m_mimeDescriptionFile;

   /*
    * Application icons:
    * https://developer.gnome.org/integration-guide/stable/icons.html.en
    */
   String m_icon16x16Dir;
   String m_icon24x24Dir;
   String m_icon32x32Dir;
   String m_icon48x48Dir;
   String m_icon64x64Dir;
   String m_icon128x128Dir;
   String m_icon256x256Dir;
   String m_icon512x512Dir;
   String m_iconScalableDir;
   String m_icon16x16File;
   String m_icon24x24File;
   String m_icon32x32File;
   String m_icon48x48File;
   String m_icon64x64File;
   String m_icon128x128File;
   String m_icon256x256File;
   String m_icon512x512File;
   String m_iconScalableFile;
   String m_bundleIcon16x16File;
   String m_bundleIcon24x24File;
   String m_bundleIcon32x32File;
   String m_bundleIcon48x48File;
   String m_bundleIcon64x64File;
   String m_bundleIcon128x128File;
   String m_bundleIcon256x256File;
   String m_bundleIcon512x512File;
   String m_bundleIconScalableFile;

   /*
    * Source application icons. They must be on the rsc/icons folder of all
    * installation packages.
    */
   String m_sourceIcon16x16File;
   String m_sourceIcon24x24File;
   String m_sourceIcon32x32File;
   String m_sourceIcon48x48File;
   String m_sourceIcon64x64File;
   String m_sourceIcon128x128File;
   String m_sourceIcon256x256File;
   String m_sourceIcon512x512File;
   String m_sourceIconScalableFile;
   String m_sourceBundleIcon16x16File;
   String m_sourceBundleIcon24x24File;
   String m_sourceBundleIcon32x32File;
   String m_sourceBundleIcon48x48File;
   String m_sourceBundleIcon64x64File;
   String m_sourceBundleIcon128x128File;
   String m_sourceBundleIcon256x256File;
   String m_sourceBundleIcon512x512File;
   String m_sourceBundleIconScalableFile;

   /*
    * Installer task
    */
   bool DoInstall();

   /*
    * Uninstaller task
    */
   bool DoUninstall();

   /*
    * Show version task
    */
   bool DoShowVersion();

   /*
    * Show help task
    */
   bool DoShowHelp();

   /*
    * Readable version. Conventionally, the installer adopts the version of the
    * PixInsight core application that is being installed.
    */
   static String     VersionString();

   /*
    * Write program name and version information to stdout.
    */
   static void       ShowLogo();

   /*
    * Copy one file from source to target:
    * - Replaces the target file if exists.
    * - Does not follow symlinks.
    */
   static void       CopyFile( const String& targetFilePath, const String& sourceFilePath );

   /*
    * Clones a source directory tree on a target location.
    */
   static void       CopyFiles( const String& targetDir, const String& sourceDir );

   /*
    * Performs a recursive directory search and returns a list of absolute file
    * paths for all the files and directories found.
    *
    * To ensure that empty directories are also reported, directories end with
    * a slash character. For example, consider the following tree:
    *
    * /
    * |
    * + foo
    *    |
    *    +- bar.txt
    *    |
    *    +- foo
    *        |
    *        + foo
    *        |  |
    *        |  +
    *        |
    *        + bar.bin
    *
    * where the /foo/foo/foo directory is empty. A call to:
    *
    *    SearchDirectory( "/foo" )
    *
    * would return the following items:
    *
    *    /foo/foo/
    *    /foo/bar.txt
    *    /foo/foo/foo/
    *    /foo/foo/bar.bin
    *
    * Subdirectories are guaranteed to be reported before their contained files
    * in the returned list.
    */
   static StringList SearchDirectory( const String& dirPath );
   static void       SearchDirectory_Recursive( StringList& foundFiles, const String& dirPath, const String& baseDir );

   /*
    * Removes a directory tree recursively.
    */
   static void       RemoveDirectory( const String& dirPath );
   static void       RemoveDirectory_Recursive( const String& dirPath, const String& baseDir );

   /*
    * Removes an existing file. Does nothing if the file does not exist.
    */
   static void       RemoveExistingFile( const String& filePath );

   /*
    * If the specified directory exists, renames it to a unique name and
    * returns the new full path. If the directory doesn't exist, returns an
    * empty string.
    */
   static String     RenameOldInstallationDirectory( const String& targetDir );

   /*
    * Returns true if the directory looks like a valid PixInsight installation.
    */
   static bool       IsPixInsightInstallation( const String& dirPath );

   /*
    * Asks "Are you sure [yes|no]?" and returns true iff the user says "yes".
    */
   static bool       AskForConfirmation();

   /*
    * Throws an exception if you are not root.
    */
   static void       RootRequired();
};

// ----------------------------------------------------------------------------

PixInsightX11Installer::PixInsightX11Installer( int argc, const char** argv )
{
   // Assume that we get a path to the executable file (possibly a relative
   // path) in argv[0]. This is standard in all known unices.
   m_executablePath = String::UTF8ToUTF16( argv[0] ).Unquoted().Trimmed();

   // Base directory where we are running.
   m_baseDir = File::FullPath( File::ExtractDirectory( m_executablePath ) );
   if ( m_baseDir.EndsWith( '/' ) )
      m_baseDir.Delete( m_baseDir.UpperBound() );

   // Default source installation directory.
   m_sourceDir = m_baseDir + "/PixInsight";

   // The application installation directory.
   // ### N.B. Enforced for security since core 1.8.9-2 build 1603.
   m_installDir = "/opt/PixInsight";

   // Default desktop entry installation directory.
   m_installDesktopDir = "/usr/share/applications";
   if ( !File::DirectoryExists( m_installDesktopDir ) )
      m_installDesktopDir = "/usr/local/share/applications"; // FreeBSD

   // Default MIME installation directory.
   m_installMIMEDir = "/usr/share/mime";
   if ( !File::DirectoryExists( m_installMIMEDir ) )
      m_installMIMEDir = "/usr/local/share/mime"; // FreeBSD

   // Default application icons installation directory.
   m_installIconsDir = "/usr/share/icons/hicolor";
   if ( !File::DirectoryExists( m_installIconsDir ) )
      m_installIconsDir = "/usr/local/share/icons/hicolor"; // FreeBSD

   // Assume we are running on an UTF-8 POSIX-compliant system.
   StringList inputArgs;
   for ( int i = 1; i < argc; ++i )
      inputArgs.Add( IsoString( argv[i] ).UTF8ToUTF16() );

   // Parse the list of command-line arguments.
   ArgumentList arguments = ExtractArguments( inputArgs, ArgumentItemMode::NoItems );

   // Iterate and interpret command-line arguments.
   for ( const Argument& arg : arguments )
   {
      if ( arg.IsNumeric() )
      {
         throw Error( "Unknown numeric argument: " + arg.Token() );
      }
      else if ( arg.IsString() )
      {
         if ( arg.Id() == "-source-dir" || arg.Id() == "s" )
            m_sourceDir = arg.StringValue();
         else if ( arg.Id() == "-install-desktop-dir" )
            m_installDesktopDir = arg.StringValue();
         else if ( arg.Id() == "-install-mime-dir" )
            m_installMIMEDir = arg.StringValue();
         else if ( arg.Id() == "-install-icons-dir" )
            m_installIconsDir = arg.StringValue();
         else
            throw Error( "Unknown string argument: " + arg.Token() );
      }
      else if ( arg.IsSwitch() )
      {
         if ( arg.Id() == "-remove" || arg.Id() == "r" )
            m_removePrevious = arg.SwitchState();
         else if ( arg.Id() == "-bin-launcher" )
            m_createBinLauncher = arg.SwitchState();
         else
            throw Error( "Unknown switch argument: " + arg.Token() );
      }
      else if ( arg.IsLiteral() )
      {
         if ( arg.Id() == "-remove" || arg.Id() == "r" )
            m_removePrevious = true;
         else if ( arg.Id() == "-no-remove" || arg.Id() == "r" ) // also support the --no-xxx idiom
            m_removePrevious = false;
         else if ( arg.Id() == "-bin-launcher" )
            m_createBinLauncher = true;
         else if ( arg.Id() == "-no-bin-launcher" )
            m_createBinLauncher = false;
         else if ( arg.Id() == "-uninstall" || arg.Id() == "u" )
            m_task = UninstallTask;
         else if ( arg.Id() == "-version" )
            m_task = ShowVersionTask;
         else if ( arg.Id() == "-yes" || arg.Id() == "y" )
            m_forceYes = true;
         else if ( arg.Id() == "-help" )
            m_task = ShowHelpTask;
         else
            throw Error( "Unknown argument: " + arg.Token() );
      }
      else if ( arg.IsItemList() )
      {
         throw Error( "Unexpected non-parametric argument: " + arg.Token() );
      }
   }

   // Sanitize working directories.
   if ( m_sourceDir.EndsWith( '/' ) )
      m_sourceDir.Delete( m_sourceDir.UpperBound() );
   if ( m_installDir.EndsWith( '/' ) )
      m_installDir.Delete( m_installDir.UpperBound() );
   if ( m_installDesktopDir.EndsWith( '/' ) )
      m_installDesktopDir.Delete( m_installDesktopDir.UpperBound() );
   if ( m_installMIMEDir.EndsWith( '/' ) )
      m_installMIMEDir.Delete( m_installMIMEDir.UpperBound() );
   if ( m_installIconsDir.EndsWith( '/' ) )
      m_installIconsDir.Delete( m_installIconsDir.UpperBound() );

   // Launcher script
   m_binLauncherFile = "/bin/PixInsight";

   // Application desktop entry file
   m_desktopEntryFile = m_installDesktopDir + "/PixInsight.desktop";

   // MIME type description file
   m_mimeDescriptionFile = m_installMIMEDir + "/packages/PixInsight.xml";

   // Application icon directories
   m_icon16x16Dir    = m_installIconsDir + "/16x16/apps";
   m_icon24x24Dir    = m_installIconsDir + "/24x24/apps";
   m_icon32x32Dir    = m_installIconsDir + "/32x32/apps";
   m_icon48x48Dir    = m_installIconsDir + "/48x48/apps";
   m_icon64x64Dir    = m_installIconsDir + "/64x64/apps";
   m_icon128x128Dir  = m_installIconsDir + "/128x128/apps";
   m_icon256x256Dir  = m_installIconsDir + "/256x256/apps";
   m_icon512x512Dir  = m_installIconsDir + "/512x512/apps";
   m_iconScalableDir = m_installIconsDir + "/scalable/apps";

   // Application icon files
   m_icon16x16File          = m_icon16x16Dir    + "/PixInsight.png";
   m_icon24x24File          = m_icon24x24Dir    + "/PixInsight.png";
   m_icon32x32File          = m_icon32x32Dir    + "/PixInsight.png";
   m_icon48x48File          = m_icon48x48Dir    + "/PixInsight.png";
   m_icon64x64File          = m_icon64x64Dir    + "/PixInsight.png";
   m_icon128x128File        = m_icon128x128Dir  + "/PixInsight.png";
   m_icon256x256File        = m_icon256x256Dir  + "/PixInsight.png";
   m_icon512x512File        = m_icon512x512Dir  + "/PixInsight.png";
   m_iconScalableFile       = m_iconScalableDir + "/PixInsight.svg";
   m_bundleIcon16x16File    = m_icon16x16Dir    + "/PixInsightBundle.png";
   m_bundleIcon24x24File    = m_icon24x24Dir    + "/PixInsightBundle.png";
   m_bundleIcon32x32File    = m_icon32x32Dir    + "/PixInsightBundle.png";
   m_bundleIcon48x48File    = m_icon48x48Dir    + "/PixInsightBundle.png";
   m_bundleIcon64x64File    = m_icon64x64Dir    + "/PixInsightBundle.png";
   m_bundleIcon128x128File  = m_icon128x128Dir  + "/PixInsightBundle.png";
   m_bundleIcon256x256File  = m_icon256x256Dir  + "/PixInsightBundle.png";
   m_bundleIcon512x512File  = m_icon512x512Dir  + "/PixInsightBundle.png";
   m_bundleIconScalableFile = m_iconScalableDir + "/PixInsightBundle.svg";

   // Source icon files
   m_sourceIcon16x16File          = m_sourceDir + "/rsc/icons/pixinsight-icon.16.png";
   m_sourceIcon24x24File          = m_sourceDir + "/rsc/icons/pixinsight-icon.24.png";
   m_sourceIcon32x32File          = m_sourceDir + "/rsc/icons/pixinsight-icon.32.png";
   m_sourceIcon48x48File          = m_sourceDir + "/rsc/icons/pixinsight-icon.48.png";
   m_sourceIcon64x64File          = m_sourceDir + "/rsc/icons/pixinsight-icon.64.png";
   m_sourceIcon128x128File        = m_sourceDir + "/rsc/icons/pixinsight-icon.128.png";
   m_sourceIcon256x256File        = m_sourceDir + "/rsc/icons/pixinsight-icon.256.png";
   m_sourceIcon512x512File        = m_sourceDir + "/rsc/icons/pixinsight-icon.512.png";
   m_sourceIconScalableFile       = m_sourceDir + "/rsc/icons/pixinsight-icon.svg";
   m_sourceBundleIcon16x16File    = m_sourceDir + "/rsc/icons/pixinsight-bundle-icon.16.png";
   m_sourceBundleIcon24x24File    = m_sourceDir + "/rsc/icons/pixinsight-bundle-icon.24.png";
   m_sourceBundleIcon32x32File    = m_sourceDir + "/rsc/icons/pixinsight-bundle-icon.32.png";
   m_sourceBundleIcon48x48File    = m_sourceDir + "/rsc/icons/pixinsight-bundle-icon.48.png";
   m_sourceBundleIcon64x64File    = m_sourceDir + "/rsc/icons/pixinsight-bundle-icon.64.png";
   m_sourceBundleIcon128x128File  = m_sourceDir + "/rsc/icons/pixinsight-bundle-icon.128.png";
   m_sourceBundleIcon256x256File  = m_sourceDir + "/rsc/icons/pixinsight-bundle-icon.256.png";
   m_sourceBundleIcon512x512File  = m_sourceDir + "/rsc/icons/pixinsight-bundle-icon.512.png";
   m_sourceBundleIconScalableFile = m_sourceDir + "/rsc/icons/pixinsight-bundle-icon.svg";
}

// ----------------------------------------------------------------------------

bool PixInsightX11Installer::Perform()
{
   switch ( m_task )
   {
   case NoTask:          return true;
   case InstallTask:     return DoInstall();
   case UninstallTask:   return DoUninstall();
   case ShowVersionTask: return DoShowVersion();
   case ShowHelpTask:    return DoShowHelp();
   default:              throw Error( "Perform(): Internal error." );
   }
}

// ----------------------------------------------------------------------------

bool PixInsightX11Installer::DoShowVersion()
{
   std::cout << '\n' << VersionString() << '\n' << '\n';
   return true;
}

// ----------------------------------------------------------------------------

bool PixInsightX11Installer::DoInstall()
{
   ShowLogo();
   RootRequired();

   if ( !File::DirectoryExists( m_sourceDir ) )
      throw Error( "The source installation directory does not exist: " + m_sourceDir );
   if ( !File::DirectoryExists( m_installDesktopDir ) )
      throw Error( "The desktop entry installation directory does not exist: " + m_installDesktopDir );
   if ( !File::DirectoryExists( m_installIconsDir ) )
      throw Error( "The application icons installation directory does not exist: " + m_installIconsDir );

   if ( !File::Exists( m_sourceIcon16x16File          ) ||
        !File::Exists( m_sourceIcon24x24File          ) ||
        !File::Exists( m_sourceIcon32x32File          ) ||
        !File::Exists( m_sourceIcon48x48File          ) ||
        !File::Exists( m_sourceIcon64x64File          ) ||
        !File::Exists( m_sourceIcon128x128File        ) ||
        !File::Exists( m_sourceIcon256x256File        ) ||
        !File::Exists( m_sourceIcon512x512File        ) ||
        !File::Exists( m_sourceIconScalableFile       ) ||
        !File::Exists( m_sourceBundleIcon16x16File    ) ||
        !File::Exists( m_sourceBundleIcon24x24File    ) ||
        !File::Exists( m_sourceBundleIcon32x32File    ) ||
        !File::Exists( m_sourceBundleIcon48x48File    ) ||
        !File::Exists( m_sourceBundleIcon64x64File    ) ||
        !File::Exists( m_sourceBundleIcon128x128File  ) ||
        !File::Exists( m_sourceBundleIcon256x256File  ) ||
        !File::Exists( m_sourceBundleIcon512x512File  ) ||
        !File::Exists( m_sourceBundleIconScalableFile ) )
   {
      throw Error( "One or more application icons are missing in the source installation directory. "
                   "Unpack a valid installation archive and try again." );
   }

   std::cout <<
   "\nPixInsight will be installed with the following parameters:"
   "\n"
   "\nSource installation directory ... " << m_sourceDir <<
   "\nPixInsight core application ..... " << m_installDir <<
   "\nApplication desktop entry ....... " << m_desktopEntryFile <<
   "\nApplication icons directory ..... " << m_installIconsDir <<
   "\nMIME type description file ...... " << m_mimeDescriptionFile <<
   "\nCreate /bin launcher script ..... " << (m_createBinLauncher ? "yes" : "no") <<
   "\nRemove previous installation .... " << (m_removePrevious ? "yes" : "no") <<
   "\n";

   if ( !m_forceYes )
      if ( !AskForConfirmation() )
         return false;

   std::cout << "\nPlease wait while PixInsight is being installed...\n" << std::flush;

   // Backup an existing previous installation
   String oldDir = RenameOldInstallationDirectory( m_installDir );

   // Copy the PixInsight core application
   File::CreateDirectory( m_installDir );
   CopyFiles( m_installDir, m_sourceDir );

   // Application launcher script on /bin
   // Ensure that arguments with whitespaces are correctly received by the core
   // launcher script at <m_installDir>/bin/PixInsight.sh.
   if ( m_createBinLauncher )
   {
      File f = File::CreateFileForWriting( m_binLauncherFile );
#ifdef __PCL_FREEBSD
      f.OutTextLn( "#!/usr/local/bin/bash" );
#else
      f.OutTextLn( "#!/bin/bash" );
#endif
      f.OutTextLn( "args=\"\"" );
      f.OutTextLn( "for arg in \"$@\"" );
      f.OutTextLn( "   do" );
      f.OutTextLn( "      if printf '%s' \"$arg\" | grep -q \" \"; then" );
      f.OutTextLn( "         args=\"$args \\\"$arg\\\"\"" );
      f.OutTextLn( "      else" );
      f.OutTextLn( "         args=\"$args $arg\"" );
      f.OutTextLn( "      fi" );
      f.OutTextLn( "   done" );
      f.OutTextLn( "args=$(printf '%s' \"$args\" | sed 's/^ *//')" );
      f.OutTextLn( "eval \"" + m_installDir.ToUTF8() + "/bin/PixInsight.sh $args\"" );
      f.Close();

      // Make it executable
      IsoString binLauncherFile8 = m_binLauncherFile.ToUTF8();
      if ( chmod( binLauncherFile8.c_str(), S_IRUSR|S_IWUSR|S_IXUSR |
                                            S_IRGRP        |S_IXGRP |
                                            S_IROTH        |S_IXOTH ) != 0 )  // rwxr-xr-x
      throw Error( "Failed to chmod the /bin launcher script: " + m_binLauncherFile );
   }

   // Write the desktop entry file:
   // https://specifications.freedesktop.org/desktop-entry-spec/latest/
   // https://specifications.freedesktop.org/menu-spec/latest/
   //
   // For additional information on the MIME type for PixInsight projects:
   // https://unix.stackexchange.com/questions/689203/custom-mime-type-for-directories
   {
      File f = File::CreateFileForWriting( m_desktopEntryFile );
      f.OutTextLn( "[Desktop Entry]" );
      f.OutTextLn( "Encoding=UTF-8" );
      f.OutTextLn( "Name=PixInsight" );
      f.OutTextLn( "GenericName=Image Processing Software" );
      f.OutTextLn( "Comment=Advanced image processing platform" );
      f.OutTextLn( "Exec=" + m_installDir.ToUTF8() + "/bin/PixInsight.sh %F" );
      f.OutTextLn( "Terminal=false" );
      f.OutTextLn( "MultipleArgs=true" );
      f.OutTextLn( "Type=Application" );
      f.OutTextLn( "Icon=PixInsight" );
      f.OutTextLn( "X-KDE-StartupNotify=false" );
      f.OutTextLn( "Categories=ImageProcessing;Astronomy;RasterGraphics;Development;Science;Photography;Graphics" );
      f.OutTextLn( "MimeType=application/x-pixinsight-project;"
                            "application/x-xosm;"
                            "application/x-xpsm;application/x-psm;"
                            "image/bmp;application/bmp;"
                            "image/fits;application/fits;"
                            "image/gif;"
                            "image/jp2;image/jpc;"
                            "image/jpeg;application/jpeg;image/jpg;application/jpg;"
                            "image/png;application/png;application/x-png;"
                            "image/tiff;application/tiff;image/tif;application/tif;"
                            "image/svg+xml;"
                            "image/webp;"
                            "image/x-tga;"
                            "image/x-adobe-dng;"
                            "image/x-canon-cr2;"
                            "image/x-canon-cr3;"
                            "image/x-canon-crw;"
                            "image/x-fuji-raf;"
                            "image/x-kodak-dcr;image/x-kodak-k25;image/x-kodak-kdc;"
                            "image/x-minolta-mrw;"
                            "image/x-nikon-nef;"
                            "image/x-olympus-orf;"
                            "image/x-panasonic-raw;image/x-panasonic-raw2;"
                            "image/x-pentax-pef;"
                            "image/x-sigma-x3f;"
                            "image/x-sony-arw;image/x-sony-sr2;image/x-sony-srf;"
                            "image/x-xisf;"
                            "text/x-pidoc;" );
      f.Close();
   }

   // Copy application icons
   if ( File::DirectoryExists( m_icon16x16Dir ) )
   {
      CopyFile( m_icon16x16File, m_sourceIcon16x16File );
      CopyFile( m_bundleIcon16x16File, m_sourceBundleIcon16x16File );
   }
   if ( File::DirectoryExists( m_icon24x24Dir ) )
   {
      CopyFile( m_icon24x24File, m_sourceIcon24x24File );
      CopyFile( m_bundleIcon24x24File, m_sourceBundleIcon24x24File );
   }
   if ( File::DirectoryExists( m_icon32x32Dir ) )
   {
      CopyFile( m_icon32x32File, m_sourceIcon32x32File );
      CopyFile( m_bundleIcon32x32File, m_sourceBundleIcon32x32File );
   }
   if ( File::DirectoryExists( m_icon48x48Dir ) )
   {
      CopyFile( m_icon48x48File, m_sourceIcon48x48File );
      CopyFile( m_bundleIcon48x48File, m_sourceBundleIcon48x48File );
   }
   if ( File::DirectoryExists( m_icon64x64Dir ) )
   {
      CopyFile( m_icon64x64File, m_sourceIcon64x64File );
      CopyFile( m_bundleIcon64x64File, m_sourceBundleIcon64x64File );
   }
   if ( File::DirectoryExists( m_icon128x128Dir ) )
   {
      CopyFile( m_icon128x128File, m_sourceIcon128x128File );
      CopyFile( m_bundleIcon128x128File, m_sourceBundleIcon128x128File );
   }
   if ( File::DirectoryExists( m_icon256x256Dir ) )
   {
      CopyFile( m_icon256x256File, m_sourceIcon256x256File );
      CopyFile( m_bundleIcon256x256File, m_sourceBundleIcon256x256File );
   }
   if ( File::DirectoryExists( m_icon512x512Dir ) )
   {
      CopyFile( m_icon512x512File, m_sourceIcon512x512File );
      CopyFile( m_bundleIcon512x512File, m_sourceBundleIcon512x512File );
   }
   if ( File::DirectoryExists( m_iconScalableDir ) )
   {
      CopyFile( m_iconScalableFile, m_sourceIconScalableFile );
      CopyFile( m_bundleIconScalableFile, m_sourceBundleIconScalableFile );
   }

   // Write the shared MIME info XML file:
   // http://standards.freedesktop.org/shared-mime-info-spec/shared-mime-info-spec-latest.html
   {
      File f = File::CreateFileForWriting( m_mimeDescriptionFile );
      f.OutTextLn( "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" );
      f.OutTextLn( "<mime-info xmlns=\"http://www.freedesktop.org/standards/shared-mime-info\">" );
      f.OutTextLn( "   <mime-type type=\"application/x-pixinsight-project\">" );
      f.OutTextLn( "      <sub-class-of type=\"inode/directory\"/>" );
      f.OutTextLn( "      <icon name=\"PixInsightBundle\"/>" );
      f.OutTextLn( "      <comment>PixInsight project bundle</comment>" );
      f.OutTextLn( "      <glob-deleteall/>" );
      f.OutTextLn( "      <glob pattern=\"*.pxiproject\"/>" );
      f.OutTextLn( "   </mime-type>" );
      f.OutTextLn( "   <mime-type type=\"application/x-xosm\">" );
      f.OutTextLn( "      <sub-class-of type=\"application/xml\"/>" );
      f.OutTextLn( "      <icon name=\"PixInsight\"/>" );
      f.OutTextLn( "      <comment>PixInsight project file</comment>" );
      f.OutTextLn( "      <glob-deleteall/>" );
      f.OutTextLn( "      <glob pattern=\"*.xosm\"/>" );
      f.OutTextLn( "   </mime-type>" );
      f.OutTextLn( "   <mime-type type=\"application/x-xpsm\">" );
      f.OutTextLn( "      <sub-class-of type=\"application/xml\"/>" );
      f.OutTextLn( "      <icon name=\"PixInsight\"/>" );
      f.OutTextLn( "      <comment>PixInsight process icon file</comment>" );
      f.OutTextLn( "      <glob-deleteall/>" );
      f.OutTextLn( "      <glob pattern=\"*.xpsm\"/>" );
      f.OutTextLn( "   </mime-type>" );
      f.OutTextLn( "   <mime-type type=\"application/x-psm\">" );
      f.OutTextLn( "      <icon name=\"PixInsight\"/>" );
      f.OutTextLn( "      <comment>PixInsight process icon file</comment>" );
      f.OutTextLn( "      <glob-deleteall/>" );
      f.OutTextLn( "      <glob pattern=\"*.psm\"/>" );
      f.OutTextLn( "   </mime-type>" );
      f.OutTextLn( "   <mime-type type=\"image/x-xisf\">" );
      f.OutTextLn( "      <icon name=\"PixInsight\"/>" );
      f.OutTextLn( "      <comment>Extensible Image Serialization Format</comment>" );
      f.OutTextLn( "      <glob-deleteall/>" );
      f.OutTextLn( "      <glob pattern=\"*.xisf\"/>" );
      f.OutTextLn( "   </mime-type>" );
      f.OutTextLn( "   <mime-type type=\"text/x-pidoc\">" );
      f.OutTextLn( "      <sub-class-of type=\"text/plain\"/>" );
      f.OutTextLn( "      <icon name=\"PixInsight\"/>" );
      f.OutTextLn( "      <comment>PixInsight documentation source file</comment>" );
      f.OutTextLn( "      <glob-deleteall/>" );
      f.OutTextLn( "      <glob pattern=\"*.pidoc\"/>" );
      f.OutTextLn( "   </mime-type>" );
      f.OutTextLn( "</mime-info>" );
      f.Close();

      // Update the MIME information database.
      IsoString installMIMEDir8 = m_installMIMEDir.ToUTF8();
      IsoString command = "update-mime-database \"" + m_installMIMEDir.ToUTF8() + "\" > /dev/null 2>&1";
      int ret = system( command.c_str() );
      if ( ret != 0 )
         std::cerr << "\n** Warning: Failed to execute the update-mime-database command. Exit code = " << IsoString( ret ) << "\n";
   }

   // If requested, remove a previous installation.
   if ( !oldDir.IsEmpty() )
      if ( m_removePrevious )
      {
         RemoveDirectory( oldDir );
         std::cout << "\n* Previous PixInsight installation removed.\n";
      }
      else
      {
         std::cout <<
         "\n* A previous PixInsight installation has been left on the following directory:"
         "\n" << oldDir << "\n";
      }

   // Set the setuid and setgid bits of the main updater program. The updater
   // requires elevated privileges.
   String updaterFilePath = m_installDir + "/bin/PixInsightUpdater";
   IsoString updaterFilePath8 = updaterFilePath.ToUTF8();
   if ( chmod( updaterFilePath8.c_str(), S_IRUSR|S_IWUSR|S_IXUSR |
                                         S_IRGRP        |S_IXGRP |
                                         S_IROTH        |S_IXOTH |
                                         S_ISUID|S_ISGID ) != 0 )  // rwsr-xr-x
      throw Error( "Failed to set the setuid/setgid bits of the updater program: " + updaterFilePath );

   std::cout << "\n* PixInsight installation completed.\n\n";
   return true;
}

// ----------------------------------------------------------------------------

bool PixInsightX11Installer::DoUninstall()
{
   ShowLogo();
   RootRequired();

   if ( !File::DirectoryExists( m_installDir ) )
      throw Error( "The target directory does not exist: " + m_installDir );
   if ( !IsPixInsightInstallation( m_installDir ) )
      throw Error( "The target directory does not look like a valid PixInsight installation: " + m_installDir );

   std::cout <<
   "\n*** WARNING ***"
   "\nThis action is irreversible. All existing files in the following directory will be removed:"
   "\n"
   "\n" + m_installDir +
   "\n";

   if ( !m_forceYes )
      if ( !AskForConfirmation() )
         return false;

   std::cout << "\nPlease wait while PixInsight is being uninstalled...\n" << std::flush;

   RemoveDirectory( m_installDir );
   RemoveExistingFile( m_binLauncherFile );
   RemoveExistingFile( m_desktopEntryFile );
   RemoveExistingFile( m_mimeDescriptionFile );
   RemoveExistingFile( m_icon16x16File );
   RemoveExistingFile( m_icon24x24File );
   RemoveExistingFile( m_icon32x32File );
   RemoveExistingFile( m_icon48x48File );
   RemoveExistingFile( m_icon64x64File );
   RemoveExistingFile( m_icon128x128File );
   RemoveExistingFile( m_icon256x256File );
   RemoveExistingFile( m_icon512x512File );
   RemoveExistingFile( m_iconScalableFile );
   RemoveExistingFile( m_bundleIcon16x16File );
   RemoveExistingFile( m_bundleIcon24x24File );
   RemoveExistingFile( m_bundleIcon32x32File );
   RemoveExistingFile( m_bundleIcon48x48File );
   RemoveExistingFile( m_bundleIcon64x64File );
   RemoveExistingFile( m_bundleIcon128x128File );
   RemoveExistingFile( m_bundleIcon256x256File );
   RemoveExistingFile( m_bundleIcon512x512File );
   RemoveExistingFile( m_bundleIconScalableFile );

   std::cout << "\n* PixInsight has been successfully uninstalled.\n\n";
   return true;
}

// ----------------------------------------------------------------------------

bool PixInsightX11Installer::DoShowHelp()
{
   ShowLogo();

   bool haveUsrShare = File::DirectoryExists( "/usr/share" );
//             1         2         3         4         5         6         7
//    1234567890123456789012345678901234567890123456789012345678901234567890
   std::cout <<
   "\nInstalls or uninstalls PixInsight on X11 UNIX and Linux platforms,"
   "\naccording to freedesktop.org standards and common practices on current"
   "\nKDE and GNOME desktops."
   "\n"
   "\nUsage: installer [<arguments>]"
   "\n"
   "\nSupported arguments (default values in parentheses):"
   "\n"
   "\n-s=<dir> | --source-dir=<dir>"
   "\n"
   "\n      Specifies the source installation files directory."
   "\n      (./PixInsight)"
   "\n"
   "\n-y | --yes"
   "\n"
   "\n      Do not ask for confirmation to install/uninstall. Without this"
   "\n      argument the installer asks for confirmation with a 'yes/no' prompt."
   "\n      This argument is useful to perform automated"
   "\n      installations/uninstallations without user interaction."
   "\n"
   "\n--install-desktop-dir=<dir>"
   "\n"
   "\n      Specifies the system directory where application desktop entry"
   "\n      files will be generated."
<< (haveUsrShare ?
   "\n      (/usr/share/applications/)" :
   "\n      (/usr/local/share/applications)")
<< "\n"
   "\n--install-mime-dir=<dir>"
   "\n"
   "\n      Specifies the system directory where a MIME database file will"
   "\n      be installed."
<< (haveUsrShare ?
   "\n      (/usr/share/mime/)" :
   "\n      (/usr/local/share/mime)")
<< "\n"
   "\n--install-icons-dir=<dir>"
   "\n"
   "\n      Specifies the system directory where application and file type"
   "\n      icon files will be installed."
<< (haveUsrShare ?
   "\n      (/usr/share/icons/hicolor/)" :
   "\n      (/usr/local/share/icons/hicolor)")
<< "\n"
   "\n--bin-launcher[+|-]"
   "\n"
   "\n      Create a PixInsight launcher shell script on the /bin directory,"
   "\n      so you can say PixInsight from anywhere."
   "\n      (enabled)"
   "\n"
   "\n--no-bin-launcher"
   "\n"
   "\n      Do not create a launcher shell script. Same as --bin-launcher-"
   "\n"
   "\n-r[+|-] | --remove[+|-]"
   "\n"
   "\n      If a previous PixInsight installation exists, remove it before"
   "\n      carrying out a new one."
   "\n      (enabled)"
   "\n"
   "\n--no-remove"
   "\n"
   "\n      Do not remove a previous installation. Same as -r- or --remove-"
   "\n"
   "\n-u | --uninstall"
   "\n"
   "\n      Uninstalls PixInsight, if a previous installation exists."
   "\n"
   "\n--version"
   "\n"
   "\n      Prints the installer version and exits. The installer version is"
   "\n      the version number of the installed PixInsight core application."
   "\n"
   "\n--help"
   "\n"
   "\n      Shows this help text and exits."
   "\n"
   "\n** Security Notes **"
   "\n"
   "\nSince version 1.8.9-2 build 1603, the PixInsight core application can"
   "\nonly be installed on the /opt/PixInsight directory on Linux and FreeBSD"
   "\noperating systems."
   "\n"
   "\n** References **"
   "\n"
   "\nhttps://specifications.freedesktop.org/desktop-entry-spec/latest/"
   "\nhttps://specifications.freedesktop.org/menu-spec/latest/"
   "\nhttps://specifications.freedesktop.org/icon-theme-spec/latest/"
   "\nhttps://developer.gnome.org/integration-guide/stable/icons.html.en"
   "\n\n";

   return true;
}

// ----------------------------------------------------------------------------

#ifdef _S1_
# undef _S1_
#endif
#ifdef _S_
# undef _S_
#endif
#define _S1_( x ) #x
#define _S_( x ) _S1_( x )

String PixInsightX11Installer::VersionString()
{
   String version = _S_( PI_VERSION_MAJOR ) "." _S_( PI_VERSION_MINOR ) "." _S_( PI_VERSION_RELEASE );
#if ( PI_VERSION_REVISION > 0 )
      version << "-" _S_( PI_VERSION_REVISION );
#endif
   return version << " " PI_VERSION_CODENAME;
}

// ----------------------------------------------------------------------------

void PixInsightX11Installer::ShowLogo()
{
   std::cout <<
   "\n_____________       ________            _____        ______ _____"
   "\n___  __ \\__(_)___  _____  _/_______________(_)______ ___  /___  /_"
   "\n__  /_/ /_  /__  |/_/__  /__  __ \\_  ___/_  /__  __ `/_  __ \\  __/"
   "\n_  ____/_  / __>  < __/ / _  / / /(__  )_  / _  /_/ /_  / / / /_"
   "\n/_/     /_/  /_/|_| /___/ /_/ /_//____/ /_/  _\\__, / /_/ /_/\\__/"
   "\n                                             /____/"
   "\n----------------------------------------------------------------------"
   "\nPixInsight X11 UNIX/Linux installer / Core version " << VersionString() <<
   "\nCopyright (C) 2003-" PI_VERSION_YEAR " Pleiades Astrophoto. All Rights Reserved"
   "\n----------------------------------------------------------------------"
   "\n";
}

// ----------------------------------------------------------------------------

void PixInsightX11Installer::CopyFile( const String& targetFilePath, const String& sourceFilePath )
{
   if ( File::Exists( targetFilePath ) )
      File::Remove( targetFilePath );
   File::CopyFile( targetFilePath, sourceFilePath );
}

void PixInsightX11Installer::CopyFiles( const String& targetDir, const String& sourceDir )
{
   if ( !targetDir.StartsWith( '/' ) )
      throw Error( "CopyFiles(): Relative target directory." );
   if ( !sourceDir.StartsWith( '/' ) )
      throw Error( "CopyFiles(): Relative source directory." );
   if ( targetDir.EndsWith( '/' ) || sourceDir.EndsWith( '/' ) )
      throw Error( "CopyFiles(): Incorrectly terminated directories." );
   if ( !File::DirectoryExists( targetDir ) )
      throw Error( "CopyFiles(): Nonexistent target directory." );
   if ( !File::DirectoryExists( sourceDir ) )
      throw Error( "CopyFiles(): Nonexistent source directory." );

   StringList sourceItems = SearchDirectory( sourceDir );

   size_type sourceDirLen = sourceDir.Length();
   for ( const String& item : sourceItems )
   {
      String relSourcePath = item;
      relSourcePath.DeleteLeft( sourceDirLen );

      String targetPath = targetDir + relSourcePath;
      if ( targetPath.EndsWith( '/' ) )
      {
         /*
          * Create a subdirectory
          */
         targetPath.Delete( targetPath.UpperBound() );
         if ( !File::DirectoryExists( targetPath ) )
         {
            File::CreateDirectory( targetPath );
            String sourcePath = item;
            sourcePath.Delete( sourcePath.UpperBound() );
            File::CopyTimesAndPermissions( targetPath, sourcePath );
         }
      }
      else
      {
         /*
          * Copy a file
          */
         /*
          * ### N.B. SearchDirectory() promises that all subdirectories are
          * reported before their contained files. Therefore we don't have to
          * create subdirectories here becase they have already been created
          * before copying files.
          */
         /*
         String targetSubdir = File::ExtractDirectory( targetPath );
         if ( targetSubdir.EndsWith( '/' ) )
            targetSubdir.Delete( targetSubdir.UpperBound() );
         if ( !File::DirectoryExists( targetSubdir ) )
            File::CreateDirectory( targetSubdir );
         */
         File::CopyFile( targetPath, item );
      }
   }
}

// ----------------------------------------------------------------------------

StringList PixInsightX11Installer::SearchDirectory( const String& dirPath )
{
   if ( !dirPath.StartsWith( '/' ) )
      throw Error( "SearchDirectory(): Relative search directory." );
   if ( dirPath.EndsWith( '/' ) )
      throw Error( "SearchDirectory(): Incorrectly terminated directory." );

   StringList foundItems;
   SearchDirectory_Recursive( foundItems, dirPath, dirPath );
   return foundItems;
}

void PixInsightX11Installer::SearchDirectory_Recursive( StringList& foundItems, const String& dirPath, const String& baseDir )
{
   if ( dirPath.Contains( ".." ) )
      throw Error( "SearchDirectory(): Attempt to climb up the filesystem." );
   if ( !dirPath.StartsWith( baseDir ) )
      throw Error( "SearchDirectory(): Attempt to redirect outside the base directory." );
   if ( !File::DirectoryExists( dirPath ) )
      throw Error( "SearchDirectory(): Attempt to search a nonexistent directory." );

   String currentDir = dirPath;
   if ( !currentDir.EndsWith( '/' ) )
      currentDir << '/';

   StringList directories;
   FindFileInfo info;
   for ( File::Find f( currentDir + "*" ); f.NextItem( info ); )
      if ( info.IsDirectory() )
      {
         if ( info.name != "." && info.name != ".." )
         {
            directories << info.name;
            foundItems << currentDir + info.name + '/';
         }
      }
      else
         foundItems << currentDir + info.name;

   for ( const String& dir : directories )
      SearchDirectory_Recursive( foundItems, currentDir + dir, baseDir );
}

// ----------------------------------------------------------------------------

void PixInsightX11Installer::RemoveDirectory( const String& dirPath )
{
   if ( !dirPath.StartsWith( '/' ) )
      throw Error( "RemoveDirectory(): Relative directory." );
   if ( !File::DirectoryExists( dirPath ) )
      throw Error( "RemoveDirectory(): Nonexistent directory." );

   // Remove all files and subdirectories recursively
   RemoveDirectory_Recursive( dirPath, dirPath );

   File::RemoveDirectory( dirPath );
}

void PixInsightX11Installer::RemoveDirectory_Recursive( const String& dirPath, const String& baseDir )
{
   if ( dirPath.Contains( ".." ) )
      throw Error( "RemoveDirectory(): Attempt to climb up the filesystem." );
   if ( !dirPath.StartsWith( baseDir ) )
      throw Error( "RemoveDirectory(): Attempt to redirect outside the base directory." );
   if ( !File::DirectoryExists( dirPath ) )
      throw Error( "RemoveDirectory(): Attempt to remove a nonexistent directory." );

   String currentDir = dirPath;
   if ( !currentDir.EndsWith( '/' ) )
      currentDir << '/';

   FindFileInfo info;
   for ( File::Find f( currentDir + "*" ); f.NextItem( info ); )
   {
      String itemPath = currentDir + info.name;
      if ( info.IsDirectory() )
      {
         if ( info.name != "." && info.name != ".." )
         {
            RemoveDirectory_Recursive( itemPath, baseDir );
            File::RemoveDirectory( itemPath );
         }
      }
      else
      {
         File::Remove( itemPath );
      }
   }
}

// ----------------------------------------------------------------------------

void PixInsightX11Installer::RemoveExistingFile( const String& filePath )
{
   if ( File::Exists( filePath ) )
      File::Remove( filePath );
}

// ----------------------------------------------------------------------------

String PixInsightX11Installer::RenameOldInstallationDirectory( const String& targetDir )
{
   if ( !File::DirectoryExists( targetDir ) )
      return String();

   String oldDir;
   for ( unsigned u = 0; ; ++u )
   {
      oldDir = File::AppendToName( targetDir, "-old-" + String( u ) );
      if ( !File::DirectoryExists( oldDir ) )
         break;
   }
   File::Move( targetDir, oldDir ); // rename targetDir -> oldDir
   return oldDir;
}

// ----------------------------------------------------------------------------

bool PixInsightX11Installer::IsPixInsightInstallation( const String& dirPath )
{
   if ( !File::DirectoryExists( dirPath ) )
      return false;
   if ( !File::DirectoryExists( dirPath + "/bin" ) )
      return false;
   if ( !File::DirectoryExists( dirPath + "/bin/lib" ) )
      return false;
//    if ( !File::DirectoryExists( dirPath + "/color" ) )
//       return false;
   if ( !File::DirectoryExists( dirPath + "/doc" ) )
      return false;
   if ( !File::DirectoryExists( dirPath + "/etc" ) )
      return false;
   if ( !File::DirectoryExists( dirPath + "/include" ) )
      return false;
//    if ( !File::DirectoryExists( dirPath + "/lib" ) )
//       return false;
   if ( !File::DirectoryExists( dirPath + "/library" ) )
      return false;
   if ( !File::DirectoryExists( dirPath + "/rsc" ) )
      return false;
   if ( !File::DirectoryExists( dirPath + "/src" ) )
      return false;
   if ( !File::Exists( dirPath + "/bin/PixInsight" ) )
      return false;
//    if ( !File::Exists( dirPath + "/doc/licenses/EULA_en_US.txt" ) )
//       return false;
   return true;
}

// ----------------------------------------------------------------------------

bool PixInsightX11Installer::AskForConfirmation()
{
   for ( ;; )
   {
      std::cout << "\n==> Are you sure [yes|no] ? " << std::flush;
      IsoString answer( ' ', 8 );
      std::cin.getline( answer.Begin(), 8 );
      answer.ResizeToNullTerminated();
      answer.Trim();
      answer.ToCaseFolded();
      if ( answer == "yes" )
         return true;
      if ( answer == "no" )
      {
         std::cout << "\n** Canceled\n\n";
         return false;
      }
      std::cout << "\n*** Unrecognized answer. Expected either 'yes' or 'no'.\n\n";
   }
}

// ----------------------------------------------------------------------------

void PixInsightX11Installer::RootRequired()
{
   if ( getuid() != 0 )
      throw Error( "You must be root to run this program." );
}

// ----------------------------------------------------------------------------
// ----------------------------------------------------------------------------

int main( int argc, const char** argv )
{
   // Only run from a terminal
   if ( !isatty( fileno( stdin ) ) )
      return EXIT_ERROR;

   Exception::DisableGUIOutput();
   Exception::EnableConsoleOutput();

   try
   {
      return PixInsightX11Installer( argc, argv ).Perform() ? EXIT_OK : EXIT_CANCEL;
   }
   catch ( ... )
   {
      IsoString message;

      try
      {
         throw;
      }
      catch ( Exception& x )
      {
         message = x.Message().ToUTF8();
      }
      catch ( String& s )
      {
         message = s.ToUTF8();
      }
      catch ( std::bad_alloc& )
      {
         message = "Out of memory.";
      }
      catch ( ... )
      {
         message = "Unknown exception.";
      }

      std::cerr << "*** Error: " << message << "\n\n";
   }
   return EXIT_ERROR;
}

// ----------------------------------------------------------------------------
// 2024/12/11 17:48:57 UTC
// installer.cpp
