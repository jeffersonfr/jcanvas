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
#ifndef J_FILESYSTEMDIALOG_H
#define J_FILESYSTEMDIALOG_H

#include "jcanvas/widgets/jdialog.h"
#include "jcanvas/widgets/jlistbox.h"
#include "jcanvas/widgets/jtext.h"
#include "jcanvas/widgets/jtextfield.h"
#include "jcanvas/widgets/jselectlistener.h"
#include "jcanvas/core/jbufferedimage.h"

#include <string>
#include <iostream>
#include <vector>
#include <algorithm>

#include <stdlib.h>

namespace jcanvas {

/**
 * \brief
 *
 */
enum class jfilechooser_type_t {
  OpenFile,
  SaveFile
};

/**
 * \brief
 *
 */
enum class jfilechooser_filter_t {
  None,
  File,
  Directory,
};

/**
 * \brief
 *
 * \author Jeff Ferr
 */
class FileChooserDialog : public Dialog, public SelectListener {

  private:
    /** \brief */
    std::vector<std::string> _extensions;
    /** \brief */
    std::shared_ptr<Text> _label;
    /** \brief */
    std::shared_ptr<ListBox> _list;
    /** \brief */
    std::shared_ptr<TextField> _file;
    /** \brief */
    std::shared_ptr<Image> _image_file;
    /** \brief */
    std::shared_ptr<Image> _image_folder;
    /** \brief */
    std::string _directory;
    /** \brief */
    jfilechooser_type_t _type;
    /** \brief */
    jfilechooser_filter_t _filter;
    /** \brief */
    bool _extension_ignorecase;

    /**
     * \brief
     *
     */
    virtual bool ShowFiles(std::string path);

    /**
     * \brief
     *
     */
    virtual bool ListFiles(std::string dirPath, std::vector<std::string> *files);

    /**
     * \brief
     *
     */
    virtual void ItemSelected(SelectEvent *event) override;

  public:

    /**
     * \brief
     *
     */
    FileChooserDialog(std::shared_ptr<Container> parent, std::string title, std::string directory, jfilechooser_type_t type = jfilechooser_type_t::OpenFile, jfilechooser_filter_t filter = jfilechooser_filter_t::None);

    /**
     * \brief
     *
     */
    virtual ~FileChooserDialog();

    /**
     * \brief
     *
     */
    virtual void Init();

    /**
     * \brief
     *
     */
    virtual std::string GetPath();

    /**
     * \brief
     *
     */
    virtual void AddExtension(std::string ext);

    /**
     * \brief
     *
     */
    virtual void SetExtensionIgnoreCase(bool b);

};

}

#endif 
