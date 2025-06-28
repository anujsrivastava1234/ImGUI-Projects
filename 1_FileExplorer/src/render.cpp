#include <filesystem>
#include <iostream>

#include <fmt/format.h>
#include <imgui.h>
#include <implot.h>

#include "render.hpp"

void WindowClass::Draw(std::string_view label)
{
    constexpr static auto window_flags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse ;
    
    constexpr static auto windowPos = ImVec2(0.0F, 0.0F);
    constexpr static auto windowSize = ImVec2(1280.0F, 720.0F);

    ImGui::SetNextWindowPos(windowPos);
    ImGui::SetNextWindowSize(windowSize);
    
    ImGui::Begin("File Explorer Tool", nullptr, window_flags);
    
    DrawMenu();
    ImGui::Separator();
    DrawContents();
    
    ImGui::SetCursorPosY(windowSize.y - 100);
    ImGui::Separator();
    DrawActions();
    ImGui::Separator();
    DrawFilters();
    
    ImGui::End();
}

void WindowClass::DrawMenu()
{
    if(ImGui::Button("Go Up"))
    {
        if(m_currentDirectoryPath.has_parent_path())
        {
            m_currentDirectoryPath = m_currentDirectoryPath.parent_path();
        }
    }

    ImGui::Text("Current Directory : %s", m_currentDirectoryPath.string().c_str());

}

void WindowClass::DrawContents()
{
    for(const auto &entry : std::filesystem::directory_iterator(m_currentDirectoryPath))
    {
        const auto is_selected = entry.path() == selectedEntry;
        const auto is_directory = entry.is_directory();
        const auto is_file = entry.is_regular_file();
        auto entry_name = entry.path().filename().string();

        if(is_directory)
        {
            entry_name = "\t[D]\t" + entry_name;
        }else if(is_file)
        {
            entry_name = "\t[F]\t" + entry_name;
        }

        if(ImGui::Selectable(entry_name.c_str(), is_selected))
        {
            if(is_directory)
            {
                m_currentDirectoryPath /= entry.path().filename();

            }

            selectedEntry = entry.path();
        }
    }
}

void WindowClass::DrawActions()
{
    if(fs::is_directory(selectedEntry))
    {
        ImGui::Text("Selected dir %s", selectedEntry.string().c_str());
    }else if(fs::is_regular_file(selectedEntry))
    {
        ImGui::Text("Selected file %s", selectedEntry.string().c_str());
    }else{
        ImGui::Text("Nothing selected");
    }

    if(fs::is_regular_file(selectedEntry) && ImGui::Button("Open File"))
    {
        openFileWithDefaultEditor();
    }

    ImGui::SameLine();

    if(ImGui::Button("Rename"))
    {
        ImGui::OpenPopup("Rename file");
        isRenameDialogOpen = true;
    }

    ImGui::SameLine();

    if(ImGui::Button("Delete"))
    {
        ImGui::OpenPopup("Delete file");
        isDeleteDialogOpen = true;
    }

    renameFilePopup();
    deleteFilePopup();
}

void WindowClass::DrawFilters()
{
    static char extension_filter[16] = {0};
    ImGui::Text("Filter by extension");
    ImGui::SameLine();
    ImGui::InputText("###inFilter ", extension_filter, sizeof(extension_filter));

    if(std::strlen(extension_filter) == 0)
    {
        return;
    }

    auto filtered_file_count = std::size_t{0};
    for(const auto &entry : fs::directory_iterator(m_currentDirectoryPath))
    {
        if(!fs::is_regular_file(entry))
            continue;
        
        if(entry.path().extension().string() == extension_filter)
        {
            ++filtered_file_count;
        }
    }

    ImGui::Text("Number of file : %u", filtered_file_count);
}

void WindowClass::openFileWithDefaultEditor()
{
    const auto command = "start \"\"\"" + selectedEntry.string() + "\"";

    std::system(command.c_str());
}

void WindowClass::renameFilePopup()
{
    if(ImGui::BeginPopupModal("Rename file", &isRenameDialogOpen ))
    {
        static char buffer_name[512] = {'\0'};

        ImGui::Text("Input Name");
        ImGui::InputText("###newName", buffer_name, sizeof(buffer_name));

        if(ImGui::Button("Rename"))
        {
            const auto new_path = selectedEntry.parent_path() / buffer_name;
            if(rename(selectedEntry, new_path))
            {
                ImGui::EndPopup();
                isRenameDialogOpen = false;
                selectedEntry = new_path;
                std::memset(buffer_name, 0 , sizeof(""));
                return;
            }
        }

        ImGui::SameLine();
        if(ImGui::Button("Cancel"))
        {
            isRenameDialogOpen = false;
        }
        ImGui::EndPopup();
    }
}

void WindowClass::deleteFilePopup()
{
    if(ImGui::BeginPopupModal("Delete file", &isDeleteDialogOpen))
    {
        ImGui::Text("Are you sure , you want to delete %s ? \t ", selectedEntry.filename().string().c_str());
        if(ImGui::Button("Yes"))
        {
            if(deleteFile(selectedEntry))
            {
                selectedEntry.clear();
                
            }
            isDeleteDialogOpen = false;
        }
        ImGui::SameLine();
        if( ImGui::Button("No"))
        {
            isDeleteDialogOpen = false;
        }
        ImGui::EndPopup();
    }
}

bool WindowClass::rename(const fs::path &old_path,const fs::path &new_path)
{
    try
    {
        fs::rename(old_path, new_path);
        return true;
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        return false;
    }
    
}

bool WindowClass::deleteFile(const fs::path &new_path)
{
    try
    {
        std::filesystem::remove(new_path);
        return true;    
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        return false;
    }

}

void render(WindowClass &window_obj)
{
    window_obj.Draw("Label");
}
