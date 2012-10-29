/*
  The basis for creating an application
  (QMainwindow)

  Tray + translate + about dialog + mainwindow

  Author: Sapronov Alexander
  E-mail: sapronov.alexander92@gmail.com
  */

#include "ui_mainwindow.h" /// ui
#include "mainwindow.h" ///

#include <QUrl> /// open home site
#include <QCloseEvent>
#include <QMessageBox>
#include <QFileDialog> /// open module (test)
#include <QDesktopServices> /// tray
#include <QDesktopWidget> /// moved to center

#include <QTranslator>

#include "defines.h" /// defines
#include "config.h"
#include "filecommon.h"


MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    init();
    loadModulesFromFolder();
    loadDictFromFolder();

    //    debug();
}
//------------------------------------------------------------------------------
MainWindow::~MainWindow()
{
    GUI_About->deleteLater();
    GUI_Settings->deleteLater();
    GUI_RightPanel->deleteLater();
    GUI_LeftPanel->deleteLater();
    GUI_LeftPanel2->deleteLater();
    GUI_RightPanel->deleteLater();
    GUI_ModuleViewer->deleteLater();
    GUI_NoteEditor->deleteLater();
    GUI_ManagerModules->deleteLater();

    delete trayIconMenu;
    delete trIcon;
    delete minimizeAction;
    delete maximizeAction;
    delete restoreAction;
    delete quitAction;
    delete prModule;

    delete ui;
}
//------------------------------------------------------------------------------
void MainWindow::init()
{
    GUI_About = new AboutDialog(this);
    GUI_Settings = new Settings(this);

    GUI_ManagerModules = new ManagerModules(this);

    /// panel init
    GUI_RightPanel = new RightPanel();
    GUI_LeftPanel = new LeftPanel(this);
    GUI_LeftPanel2 = new LeftPanel2(this);
    //    GUI_BottomPanel = new BottomPanel(this);

    GUI_RightPanel->setMinimumWidth(200);
    GUI_LeftPanel->setMinimumWidth(200);
    //    GUI_BottomPanel->setMinimumHeight(100);
    addDockWidget(Qt::LeftDockWidgetArea, GUI_LeftPanel);
    addDockWidget(Qt::LeftDockWidgetArea, GUI_LeftPanel2);
    //    addDockWidget(Qt::RightDockWidgetArea, GUI_RightPanel);
    //    addDockWidget(Qt::BottomDockWidgetArea, GUI_BottomPanel);

    prModule = new ProcessModule();


    //    ui->centralWidget->setMouseTracking(true);
    GUI_ModuleViewer = new ModuleViewer(this);
    GUI_NoteEditor = new NoteEditor(this);

    //    GUI_ModuleViewer->setMouseTracking(true);
    this->setCentralWidget(GUI_ModuleViewer);
    //    centralWidget()->setAttribute(Qt::WA_TransparentForMouseEvents);
    //    setMouseTracking(true);

    /// load settings
    // replace to QString t_lang = GUI_Settings->getAppLang()
    // and switch t_lang?
    /// and load load module settings
    GUI_Settings->loadSettings();
    if (GUI_Settings->getAPPLang() == "Russian" )
    {
        setLangRu();
    }
    if (GUI_Settings->getAPPLang() == "English")
    {
        setLangEn();
    }
    if (GUI_Settings->getAPPLang() == "Deutsch")
    {
        setLangDe();
    }
    if (GUI_Settings->getAPPLang() == "Français")
    {
        setLangFr();
    }

    /// moved to center desktop
    //    QRect rect = QApplication::desktop()->availableGeometry(this);
    //    this->move(rect.width() / 2 - this->width() / 2,
    //               rect.height() / 2 - this->height() / 2);
    /// maximized
    this->showMaximized();

    createActions(); // create action for tray
    createTrayIcon(); // add actionts to tray menu
    createConnects(); // moved func
    trIcon->show();  //display tray


}
//------------------------------------------------------------------------------
void MainWindow::debug()
{
    QStringList fileName;

    //    fileName << "/home/files/Documents/Bible/unrar/Book_Spurgeon/bibleqt.ini";
    //    fileName << "/home/files/Documents/Bible/unrar/my/BIBLEQT.INI";
    //    fileName << "/home/files/Documents/Bible/unrar/NT_Russian_Kassian/Bibleqt.ini";
    //    fileName << "/home/files/Documents/Bible/unrar/Makarij/bibleqt.ini";

    QString fileStrong;
    fileStrong = "/home/files/Documents/Bible/oteh/Strongs/HEBREW.HTM";
    createListStrongs(fileStrong);
    fileStrong = "/home/files/Develop/git/projectQ/projectQ-build-desktop/build/bin/strongs/strong.xml";
    GUI_ModuleViewer->setStrongList(fileStrong);


    QHash<QString, QVector<int> > hash;

    QString str;
    str = "fsa2f fsaf asfas f4 f43 3 2 23d23 sfsd";
    myDebug() << getNextWord(str, 6);

    //    QString str = "";
    //    if (str.isEmpty())
    //    {
    //        myDebug() << "yes";
    //    }

    //    QString str;
    //    QString l1 = "module=\"Пятикнижие (перевод архим. Макария)\"";
    //    QString l2
    ////    str = "<note module=\"Пятикнижие (перевод архим. Макария)\" book=\"Левит\" chapter=\"3\" versebegin=\"0\" verseend=\"1\">gsadgsad";
    //    myDebug() << str;
    //    if (str.contains(l1) &&
    //            && )
    //    {
    //        myDebug() << "yes";
    //    }

    //    loadModulesFromFolder();
    //    for (int i = 0; i < fileName.size(); i++)
    //    {
    //        prModule->processing(fileName.at(i), OBVCore::Type_BibleQuoteModule);
    //    }

    // > 62
    // < 60
    //    // " " 127
    //        for (int i = 0; i < 255; i++)
    //        {
    //            myDebug() << QChar(i) << i;
    //        }
}
//------------------------------------------------------------------------------
void MainWindow::createConnects()
{
    //tray
    connect(trIcon,SIGNAL(activated(QSystemTrayIcon::ActivationReason)), SLOT(showHide(QSystemTrayIcon::ActivationReason)));

    // menu file
    connect(ui->action_File_Close, SIGNAL(triggered()), this, SLOT(close()));

    // menu settings
    connect(ui->action_Settings_General, SIGNAL(triggered()), SLOT(showSettings()));

    // manager module
    connect(ui->action_Settings_Module, SIGNAL(triggered()), SLOT(showModuleManager()));

    //menu settings -> lang
    connect(ui->action_Settings_Language_Russian, SIGNAL(triggered()), SLOT(setLangRu()) );
    connect(ui->action_Settings_Language_English, SIGNAL(triggered()), SLOT(setLangEn()) );
    connect(ui->action_Settings_Language_Deutsch, SIGNAL(triggered()), SLOT(setLangDe()) );
    connect(ui->action_Settings_Language_France, SIGNAL(triggered()),  SLOT(setLangFr()) );

    // menu search


    // toolbar
    connect(ui->actionAction_Other_Create_Note, SIGNAL(triggered()), SLOT(createNote()));
    connect(ui->actionAction_Other_Update_List_Module, SIGNAL(triggered()), SLOT(loadModulesFromFolder()));

    // menu about
    connect(ui->action_About_About, SIGNAL(triggered()), GUI_About, SLOT(show()));
    connect(ui->action_About_About_Qt, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
    connect(ui->action_About_Site, SIGNAL(triggered()), this, SLOT(aboutOpenSite()));
    connect(ui->action_About_Help, SIGNAL(triggered()), SLOT(showHelp()));

    /// other
    connect(prModule, SIGNAL(SIGNAL_ProcessOk()), SLOT(processFinish()));


    connect(GUI_ModuleViewer, SIGNAL(showNoteList(QString,QString,QString,QString,QString)),
            GUI_LeftPanel2, SLOT(showNoteList(QString,QString,QString,QString,QString)));

    connect(GUI_Settings, SIGNAL(SIGNAL_RetranslateGUI(QString)),
            SLOT(retranslate(QString)));
    connect(GUI_Settings, SIGNAL(SIGNAL_ReLoadModules()), SLOT(loadModulesFromFolder()));

    // connect fron left1 to left2 panels
    connect(GUI_LeftPanel, SIGNAL(SIGNAL_AddRecordToJournal(QString,QString,QString)),
            GUI_LeftPanel2, SLOT(addRecordToJournal(QString,QString,QString)));

    connect(GUI_LeftPanel2, SIGNAL(SIGNAL_ShowChapterFromJournal(QString,QString,QString)),
            GUI_LeftPanel, SLOT(showChapterFromJournal(QString,QString,QString)));

    // connect settings and module viewer
    connect(GUI_Settings, SIGNAL(SIGNAL_ReLoadFontSettings()), GUI_ModuleViewer,
            SLOT(updateFontSettings()));

}
//------------------------------------------------------------------------------
void MainWindow::showHide(QSystemTrayIcon::ActivationReason r)
{
    if (r == QSystemTrayIcon::Trigger)
    {
        if ( !this->isVisible() )
        {
            this->show();
        }
        else
        {
            this->hide();
        }
    }
}
//------------------------------------------------------------------------------
void MainWindow::createTrayIcon()
{
    trIcon = new QSystemTrayIcon();  //init
    trIcon->setIcon(QIcon(":/images/images/img.png"));  //set ico

    trayIconMenu = new QMenu(this);  // create menu
    trayIconMenu->addAction(minimizeAction);
    trayIconMenu->addAction(maximizeAction);
    trayIconMenu->addAction(restoreAction);
    trayIconMenu->addSeparator();
    trayIconMenu->addAction(quitAction);

    trIcon->setContextMenu(trayIconMenu); //set menu
}
//------------------------------------------------------------------------------
void MainWindow::createActions()
{
    minimizeAction = new QAction(tr("&Hide"), this);
    connect(minimizeAction, SIGNAL(triggered()), this, SLOT(hide()));

    maximizeAction = new QAction(tr("&Expand"), this);
    connect(maximizeAction, SIGNAL(triggered()), this, SLOT(showMaximized()));

    restoreAction = new QAction(tr("&Restore"), this);
    connect(restoreAction, SIGNAL(triggered()), this, SLOT(showNormal()));

    quitAction = new QAction(tr("Q&uit"), this);
    connect(quitAction, SIGNAL(triggered()), this, SLOT(close()));
}
//------------------------------------------------------------------------------
void MainWindow::setLangEn()
{
    ui->action_Settings_Language_Deutsch ->setChecked(false);
    ui->action_Settings_Language_Russian ->setChecked(false);
    ui->action_Settings_Language_France ->setChecked(false);
    ui->action_Settings_Language_English ->setChecked(true);
    QString t_lang = "English";
    if (Config::configuration()->getAppLang() != t_lang)
    {
        retranslate(t_lang);
    }
}

//------------------------------------------------------------------------------
void MainWindow::setLangRu()
{
    ui->action_Settings_Language_Deutsch ->setChecked(false);
    ui->action_Settings_Language_Russian ->setChecked(true);
    ui->action_Settings_Language_France ->setChecked(false);
    ui->action_Settings_Language_English ->setChecked(false);
    QString t_lang = "Russian";
    if (Config::configuration()->getAppLang() != t_lang)
    {
        retranslate(t_lang);
    }
}
//------------------------------------------------------------------------------
void MainWindow::setLangDe()
{
    ui->action_Settings_Language_Deutsch ->setChecked(true);
    ui->action_Settings_Language_Russian ->setChecked(false);
    ui->action_Settings_Language_France ->setChecked(false);
    ui->action_Settings_Language_English ->setChecked(false);
    QString t_lang = "Deutch";
    if (Config::configuration()->getAppLang() != t_lang)
    {
        retranslate(t_lang);
    }
}
//------------------------------------------------------------------------------
void MainWindow::setLangFr()
{
    ui->action_Settings_Language_Deutsch ->setChecked(false);
    ui->action_Settings_Language_Russian ->setChecked(false);
    ui->action_Settings_Language_France ->setChecked(true);
    ui->action_Settings_Language_English ->setChecked(false);

    QString t_lang = "Français";
    if (Config::configuration()->getAppLang() != t_lang)
    {
        retranslate(t_lang);
    }
}
//------------------------------------------------------------------------------
void MainWindow::aboutOpenSite()
{
    QDesktopServices::openUrl(QUrl(GL_WEB_SITE));
}
//------------------------------------------------------------------------------
void MainWindow::showSettings()
{
    //set sett
    GUI_Settings->show();
}
//------------------------------------------------------------------------------
void MainWindow::showModuleManager()
{
    //set sett
    GUI_ManagerModules->show();
}
//------------------------------------------------------------------------------
void MainWindow::closeEvent(QCloseEvent *e)
{
    //    myDebug()<< "closeEvent()";
    saveSettings();
    e -> accept();
}
//------------------------------------------------------------------------------
//void MainWindow::mouseMoveEvent(QMouseEvent *e)
//{
//    //    myDebug()<< "closeEvent()";
////    saveSettings();
////    e -> accept();
////    int x = e->pos().x() - GUI_NoteEditor->geometry().x();
////    int y = e->pos().y() - GUI_NoteEditor->geometry().y();
//////    myDebug() << e->pos() - GUI_NoteEditor->geometry().x;
////    myDebug() << e->pos();
////    myDebug() << x << y;
////    GUI_NoteEditor->mouseMoveEvent(e);
//}
//------------------------------------------------------------------------------
void MainWindow::saveSettings()
{
    Config *config = Config::configuration();
    //    myDebug() << "src = start to save settings";
    //    config -> setSideBarPage(helpDock -> tabWidget() -> currentIndex());
    //    config -> setWindowGeometry(saveGeometry());
    //    config -> setMainWindowState(saveState());

    //    // Create list of the tab urls
    //    QStringList lst;
    //    QList<HelpWindow*> browsers = tabs -> browsers();
    //    foreach (HelpWindow *browser, browsers){
    //        //        myDebug() << "src = " << browser -> source().toString();
    //        lst << relatifyFileName(browser -> source().toString(), config -> PrjDir());
    //        //        myDebug() << "lst = " << lst;
    //    }
    //    config -> setSource(lst);
    config -> saveSettings();
}
//------------------------------------------------------------------------------
void MainWindow::showHelp()
{
    //    HtmlHelp(NULL, "help.chm", HH_DISPLAY_TOPIC, 0);
}
//------------------------------------------------------------------------------
void MainWindow::processFinish()
{
    ProjectQModuleList* list = new ProjectQModuleList();
    list->refreshList();
    GUI_LeftPanel->refreshList(list);
}
//------------------------------------------------------------------------------
void MainWindow::loadModulesFromFolder()
{
    QStringList listModules = getListModulesFromPath(Config::configuration()->getBibleDir());
    for (int i = 0; i < listModules.size(); i++)
    {
        //        myDebug() << listModules.at(i);
        prModule->processing(listModules.at(i), OBVCore::Type_BibleQuoteModule);
    }
}
//------------------------------------------------------------------------------
void MainWindow::loadDictFromFolder()
{
    QStringList listModules = getListModulesFromPath(
                Config::configuration()->getDictDir()
                , ".idx");
    for (int i = 0; i < listModules.size(); i++)
    {
                myDebug() << listModules.at(i);
//        prModule->processing(listModules.at(i), OBVCore::Type_BibleQuoteModule);
    }
}
//------------------------------------------------------------------------------
void MainWindow::createNote()
{
    if (GUI_ModuleViewer->getModuleName().isEmpty())
        return;
    if (GUI_ModuleViewer->getChapterValue().isEmpty())
        return;

    GUI_NoteEditor->setPath(GUI_ModuleViewer->getPath());
    GUI_NoteEditor->setModuleName(GUI_ModuleViewer->getModuleName());
    GUI_NoteEditor->setBookName(GUI_ModuleViewer->getBookName());
    GUI_NoteEditor->setChapterValue(GUI_ModuleViewer->getChapterValue());
    GUI_NoteEditor->setFirstVerse(GUI_ModuleViewer->getLastSelectLineFirst());
    GUI_NoteEditor->setLastVerse(GUI_ModuleViewer->getLastSelectLineLast());
    GUI_NoteEditor->show();
}
//------------------------------------------------------------------------------
void MainWindow::retranslate(QString t_lang)
{
    /// remove translate
    QString lan = "projectQ_" + getShortLang(Config::configuration()->getAppLang());
    QTranslator translator2;
    translator2.load(lan,":lang/lang");

    qApp->removeTranslator(&translator2);
    ui->retranslateUi(this);

    /// set new translate
    Config::configuration()->setAppLang(t_lang);
    QTranslator translator;
    lan = "projectQ_" + getShortLang(t_lang);
    translator.load(lan,":lang/lang");
    qApp->installTranslator(&translator);

    ui->retranslateUi(this);

    GUI_About->retranslate();    // about dialog
    GUI_Settings->retranslate();
    GUI_RightPanel->retranslate();
    GUI_LeftPanel->retranslate();
    GUI_LeftPanel2->retranslate();
    //    GUI_BottomPanel->retranslate();
    GUI_ModuleViewer->retranslate();
    GUI_NoteEditor->retranslate();

}
//------------------------------------------------------------------------------
