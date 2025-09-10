/*******************************************************************
 *                     Peach Editor v0.0.7
 *             Created by Ranyodh Mandur - 🍑 2024
 *
 *             Licensed under the MIT License (MIT).
 *         For more details, see the LICENSE file or visit:
 *             https://opensource.org/licenses/MIT
 *
 *      Peach Editor is a free open source editor for Peach-E
********************************************************************/
#pragma once

#include "Managers/LogManager.h"
#include "../Editor/PeachProject.h"

namespace PeachEditor {

    struct PeachEditorSettings
    {

        SERIALIZABLE_FIELDS()
    };

    struct GlobalPeachEditorData
    {
        map<string, string> ListOfAllProjects;

        PeachEditorSettings EditorSettings; //global settings that persist across projects

        SERIALIZABLE_FIELDS(ListOfAllProjects, EditorSettings)
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
                PeachProject* fp_PeachProject //IDK HOW to indicate side effects better
            )
        {
            if (not fp_PeachProject)
            {

                return false;
            }
            //fp_PeachProject.
            const string f_FullProjectPath = fp_DesiredProjectPath + "/" + fp_ProjectName;

            //ensure there is no name clashes when a user tries to create a project w a name that is already being used by another project

            //if directory should not be created, check if the directory is valid/exists, if not inform user and stop project creation and prompt again
            if (not fp_ShouldCreateDirectory and not filesystem::exists(fp_DesiredProjectPath))
            {

                return false;
            }
            //create directories for project if directory creation UI item is checked and the user wants a new dir instead of using an existing dir
            else
            {
                error_code f_LastError;

                filesystem::create_directories(fp_DesiredProjectPath, f_LastError); //create hidden metadata folder for the project, like .vs or .godot


                filesystem::create_directories(f_FullProjectPath + "/.peach", f_LastError); //create hidden metadata folder for the project, like .vs or .godot
            }

            //write peach project settings -> JSON inside desired dir
            //scope declaration since serializer shouldnt live past here
            {
                PeachCore::Serializer f_Serializer;

                //f_Serializer.ToJSON(fp_PeachProject);

            }
        }

    };

}
