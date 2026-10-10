/*
 * SPDX-License-Identifier: GPL-3.0-only
 * MuseScore-Studio-CLA-applies
 *
 * MuseScore Studio
 * Music Composition & Notation
 *
 * Copyright (C) 2021 MuseScore Limited and others
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#pragma once

#include "iprojectfilescontroller.h"

#include <QObject>
#include <QString>

#include "modularity/ioc.h"
#include "iinteractive.h"
#include "context/iglobalcontext.h"
#include "actions/actionable.h"
#include "actions/iactionsdispatcher.h"
#include "multiwindows/imultiwindowsprovider.h"
#include "multiwindows/iprojectprovider.h"
#include "cloud/musescorecom/imusescorecomservice.h"
#include "cloud/audiocom/iaudiocomservice.h"
#include "playback/iplaybackcontroller.h"
#include "print/iprintprovider.h"
#include "iexportprojectscenario.h"
#include "inotationreadersregister.h"
#include "iopensaveprojectscenario.h"
#include "imscmetareader.h"
#include "io/ifilesystem.h"
#include "notation/inotationconfiguration.h"
#include "musesounds/imusesoundscheckupdatescenario.h"
#include "musesounds/imusesamplercheckupdatescenario.h"
#include "extensions/iextensionsprovider.h"
#include "importexport/midi/imidiconfiguration.h"

#include "async/asyncable.h"

#include "iprojectconfiguration.h"
#include "iprojectcreator.h"
#include "irecentfilescontroller.h"
#include "iprojectautosaver.h"

namespace mu::project {
class ProjectActionsController : public IProjectFilesController, public muse::mi::IProjectProvider, public muse::Contextable,
    public muse::actions::Actionable, public muse::async::Asyncable
{
    muse::GlobalInject<IProjectConfiguration> configuration;
    muse::GlobalInject<muse::mi::IMultiWindowsProvider> multiwindowsProvider;
    muse::GlobalInject<notation::INotationConfiguration> notationConfiguration;
    muse::GlobalInject<muse::io::IFileSystem> fileSystem;
    muse::GlobalInject<IProjectCreator> projectCreator;
    muse::ContextInject<INotationReadersRegister> readers = { this };
    muse::ContextInject<IRecentFilesController> recentFilesController = { this };
    muse::ContextInject<IProjectAutoSaver> projectAutoSaver = { this };
    muse::ContextInject<IOpenSaveProjectScenario> openSaveProjectScenario = { this };
    muse::ContextInject<IExportProjectScenario> exportProjectScenario = { this };
    muse::ContextInject<muse::actions::IActionsDispatcher> dispatcher = { this };
    muse::ContextInject<muse::IInteractive> interactive = { this };
    muse::ContextInject<context::IGlobalContext> globalContext = { this };
    muse::ContextInject<playback::IPlaybackController> playbackController = { this };
    muse::ContextInject<print::IPrintProvider> printProvider = { this };
    muse::ContextInject<muse::extensions::IExtensionsProvider> extensionsProvider = { this };
    muse::GlobalInject<mu::iex::midi::IMidiImportExportConfiguration> midiImportExportConfiguration;

public:

    ProjectActionsController(const muse::modularity::ContextPtr& iocCtx)
        : muse::Contextable(iocCtx) {}

    void init();

    bool canReceiveAction(const muse::actions::ActionCode& code) const override;

    bool isUrlSupported(const QUrl& url) const override;
    bool isFileSupported(const muse::io::path_t& path) const override;
    muse::Ret openProject(const ProjectFile& file) override;
    bool closeOpenedProject(bool goToHome = true) override;
    bool saveProject(const muse::io::path_t& path = muse::io::path_t()) override;
    bool saveProjectLocally(
        const muse::io::path_t& path = muse::io::path_t(), SaveMode saveMode = SaveMode::Save, bool createBackup = true) override;

    // mi::IProjectProvider
    bool isProjectOpened(const muse::io::path_t& scorePath) const override;
    bool isAnyProjectOpened() const override;

    const ProjectBeingDownloaded& projectBeingDownloaded() const override;
    muse::async::Notification projectBeingDownloadedChanged() const override;

private:
    void setupConnections();

    project::INotationProjectPtr currentNotationProject() const;
    notation::IMasterNotationPtr currentMasterNotation() const;
    notation::INotationPtr currentNotation() const;
    notation::INotationInteractionPtr currentInteraction() const;
    notation::INotationSelectionPtr currentNotationSelection() const;

    void newProject();

    void openProject(const muse::actions::ActionData& args);
    muse::Ret openProject(const muse::io::path_t& path, const QString& displayNameOverride = QString());

    bool shouldRetryLoadAfterError(const muse::Ret& ret, const muse::io::path_t& filepath);
    bool askIfUserAgreesToOpenProjectWithIncompatibleVersion(const std::string& errorText);
    void warnFileTooNew(const muse::io::path_t& filepath);
    bool askIfUserAgreesToOpenCorruptedProject(const muse::String& projectName, const std::string& errorText);
    void warnProjectCriticallyCorrupted(const muse::String& projectName, const std::string& errorText);
    void warnProjectCannotBeOpened(const muse::Ret& ret, const muse::io::path_t& filepath);

    muse::IInteractive::Button askAboutSavingScore(INotationProjectPtr project);

    muse::Ret canSaveProject() const;
    bool saveProject(SaveMode saveMode, SaveLocationType saveLocationType = SaveLocationType::Undefined, bool force = false);
    void saveProjectAt(const muse::actions::ActionData& args);
    bool saveProjectAt(const SaveLocation& saveLocation, SaveMode saveMode = SaveMode::Save, bool force = false);

    bool askIfUserAgreesToSaveProjectWithErrors(const muse::Ret& ret, const SaveLocation& location);
    void warnScoreWithoutPartsCannotBeSaved();
    bool askIfUserAgreesToSaveCorruptedScore(const SaveLocation& location, const std::string& errorText, bool newlyCreated);
    bool askIfUserAgreesToSaveCorruptedScoreLocally(const std::string& errorText, bool canRevert);
    bool askIfUserAgreesToSaveCorruptedScoreUponOpenning(const SaveLocation& location, const std::string& errorText);
    void showErrCorruptedScoreCannotBeSaved(const SaveLocation& location, const std::string& errorText);

    void warnScoreCouldnotBeSaved(const muse::Ret& ret);
    void warnScoreCouldnotBeSaved(const std::string& errorText);
    int warnScoreHasBecomeCorruptedAfterSave(const muse::Ret& ret);

    void revertCorruptedScoreToLastSaved();

    RecentFile makeRecentFile(INotationProjectPtr project);


    void importPdf();
    void importAudioToScore();

    void clearRecentScores();

    void continueLastSession();

    void openProjectProperties();

    muse::async::Promise<muse::io::path_t> selectScoreOpeningFile() const;
    muse::io::path_t selectScoreSavingFile(const muse::io::path_t& defaultFilePath, const QString& saveTitle);

    muse::RetVal<INotationProjectPtr> loadProject(const muse::io::path_t& filePath);
    muse::Ret loadWithFallback(const std::shared_ptr<INotationProject>& project, const muse::io::path_t& loadPath,
                               const std::string& format);
    muse::Ret doOpenProject(const muse::io::path_t& filePath);

    muse::Ret doFinishOpenProject();
    muse::Ret openPageIfNeed(muse::Uri pageUri);

    void exportScore();
    void exportComposer();
    void exportPianomania();
    void exportPianomaniaAssets(const notation::INotationPtr& notation, const muse::io::path_t& basePath,
                                bool exportPdf, bool exportMidi, bool exportMei);
    void printScore();

    bool hasSelection() const;


    bool m_isProjectSaving = false;
    bool m_isProjectClosing = false;
    bool m_isProjectProcessing = false;



    ProjectBeingDownloaded m_projectBeingDownloaded;
    muse::async::Notification m_projectBeingDownloadedChanged;
};
}
