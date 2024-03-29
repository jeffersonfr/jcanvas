/***************************************************************************
 *   Copyright (C) 2005 by Jeff Ferr                                       *
 *   root@sat                                                              *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#include "jcanvas/widgets/jfilechooserdialog.h"
#include "jcanvas/widgets/jbutton.h"
#include "jcanvas/core/jresources.h"

#include "jmixin/jstring.h"

#include <filesystem>

#include <unistd.h>

namespace jcanvas {

FileChooserDialog::FileChooserDialog(Container *parent, std::string title, std::string directory, jfilechooser_type_t type, jfilechooser_filter_t filter):
  Dialog(title, parent, {0, 0, 720, 480})
{
  _file = NULL;
  _directory = directory;
  _type = type;
  _filter = filter;
  _extension_ignorecase = true;

  _image_file = std::make_shared<BufferedImage>(Resources::Path() + "/images/file.png");
  _image_folder = std::make_shared<BufferedImage>(Resources::Path() + "/images/folder.png");
  
  if (_type == jfilechooser_type_t::SaveFile) {
    _list = new ListBox();
    _file = new Text();

    Add(_file, jborderlayout_align_t::North);
    Add(_list, jborderlayout_align_t::Center);
  } else {
    _list = new ListBox();
  
    Add(_list, jborderlayout_align_t::Center);
  }

  _list->RegisterSelectListener(this);
  _list->RequestFocus();

  ShowFiles(_directory);
}

FileChooserDialog::~FileChooserDialog()
{
  delete _list;
  delete _file;
}

std::string FileChooserDialog::GetPath()
{
  std::filesystem::path
    path {_directory + "/" + _list->GetSelectedItems()[0]->GetTextComponent()->GetText()};

  return path.string();
}

void FileChooserDialog::AddExtension(std::string ext)
{
  _extensions.push_back(ext);
}

void FileChooserDialog::SetExtensionIgnoreCase(bool b)
{
  _extension_ignorecase = b;
}

bool FileChooserDialog::ShowFiles(std::string directory)
{
  std::vector<std::string> files;
  
  if (ListFiles(directory, &files) == false) {
    return false;
  }

  _list->RemoveAll();
  _list->AddItem<Button>("..", _image_folder);
  _list->GetItemByIndex(0)->RequestFocus();

  std::sort(files.begin(), files.end(), 
      [&](auto &a, auto &b) {
        if (a < b) {
          return true;
        }

        return false;
      });


  if (_filter == jfilechooser_filter_t::Directory || _filter == jfilechooser_filter_t::None) {
    for (auto &item : files) {
      std::filesystem::path path(directory + "/" + item);

      if (std::filesystem::is_directory(std::filesystem::status(path)) == true) {
        _list->AddItem<Button>(path.filename(), _image_folder); 
      }
    }
  }

  if (_filter == jfilechooser_filter_t::File || _filter == jfilechooser_filter_t::None) {
    for (auto &item : files) {
      std::filesystem::path path(directory + "/" + item);
      
      if (std::filesystem::is_regular_file(std::filesystem::status(path)) == true) {
        if (_extensions.size() == 0) {
          _list->AddItem<Button>(path.filename(), _image_file);
        } else {
          for (auto &ext : _extensions) {
            std::string path_ext = path.extension();

            if (_extension_ignorecase == true) {
              ext = jmixin::String(ext).lower_case();
              path_ext = jmixin::String(path_ext).lower_case();
            }

            if (ext == path_ext) {
              _list->AddItem<Button>(path.filename(), _image_file);
            }
          }
        }
      }
    }
  }

  _list->Repaint();

  return true;
}

bool FileChooserDialog::ListFiles(std::string path, std::vector<std::string> *files)
{
  for (auto i = std::filesystem::directory_iterator(path); i != std::filesystem::directory_iterator(); i++) {
    files->push_back(i->path().filename().string());
  }

  return true;
}

void FileChooserDialog::ItemSelected(SelectEvent *)
{
  std::string item = _list->GetSelectedItems()[0]->GetTextComponent()->GetText();
  std::filesystem::path path(_directory + "/" + item);

  path = std::filesystem::canonical(path);

  if (std::filesystem::is_directory(std::filesystem::status(path)) == true) {
    if (ShowFiles(path.string()) == true) {
      _directory = path;
  
      _file->SetText("");
    }
  } else { 
    if (_type == jfilechooser_type_t::OpenFile) {
      Close();
    } else {
      _file->SetText(item);
    }
  }
}

}

