#pragma once

#include <cstdint>
#include <filesystem>
#include <string_view>

namespace fs = std::filesystem;

class WindowClass
{
public:
    WindowClass() : m_currentDirectoryPath(fs::current_path()), selectedEntry(fs::path()){};
void Draw(std::string_view label);

private:
void DrawMenu();
void DrawContents();
void DrawActions();
void DrawFilters();

void openFileWithDefaultEditor();
void renameFilePopup();
void deleteFilePopup();
bool rename(const fs::path &old_path,const fs::path &new_path);
bool deleteFile(const fs::path &file_path);

private:
    fs::path m_currentDirectoryPath;
    fs::path selectedEntry;
    bool isRenameDialogOpen = false;
    bool isDeleteDialogOpen = false;
    
};

void render(WindowClass &window_obj);
