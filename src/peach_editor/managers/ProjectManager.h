/*******************************************************************
 *                     Peach Editor v0.0.1
 *         Created by Ranyodh Singh Mandur - 🍑 2024-2026
 *
 *             Licensed under the MIT License (MIT).
 *         For more details, see the LICENSE file or visit:
 *             https://opensource.org/licenses/MIT
 *
 *      Peach Editor is a free open source editor for Peach-E
********************************************************************/
#pragma once

#include <utils/Logger.h>

#include "../editor/PeachProject.h"

namespace PeachEditor {

    struct PeachEditorSettings
    {

        //SERIALIZABLE_FIELDS()
    };

    struct GlobalPeachEditorData
    {
        unordered_map<string, string> ListOfAllProjects;

        PeachEditorSettings EditorSettings; //global settings that persist across projects

        //SERIALIZABLE_FIELDS(ListOfAllProjects, EditorSettings)
    };

    struct ProjectManager
    {
    public:
        ProjectManager() = default; //created once and destroyed later on possibly creatde again but idk if i want the editor to have that option since its just a waste of space
        ~ProjectManager() = default;

        //prevent accidental copying
        ProjectManager(const ProjectManager&) = delete;
        ProjectManager& operator=(const ProjectManager&) = delete;

        GlobalPeachEditorData m_PeachEditorMetaData;

        unique_ptr<PeachCore::Logger> project_logger = nullptr;

    private:



    public:

        void
            Initialize(const string& logDirectory);

        bool
            StartProjectManager();

        bool
            CloseProjectManager();

        bool
            StartPeachEditor();

    private:
        /*XXX : used for keeping track of every project currently identified on the system, this is where default OS paths are handled
        * and we store persistent data relevant to the installed instance of the Peach Editor and Project Manager stuff. so we can keep track of
        * all currently created projects on the system,
        *
        * this is loaded at the very beginning of each time the peach editor is opened, if it cant be found it will generate a new one for first time installation
        * or reinstallation. similiar to the .conan2 or whatever folders ull find in the home/appdata folders
        *
        * Peach editor is built for use only on desktop OS' since  idk if ipadOS would be chill w development stuff
        */
        bool
            LoadGlobalProjectData()
        {

            return true;
        }

        bool
            CreateNewPeachProject
            (
                const string& fp_ProjectName,
                const string& fp_DesiredProjectPath,
                const bool fp_ShouldCreateDirectory,
                PeachProject& fp_PeachProject //IDK HOW to indicate side effects better
            );
    };
}
