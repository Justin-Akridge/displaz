// Copyright 2015, Christopher J. Foster and the other displaz contributors.
// Use of this code is governed by the BSD-style license found in LICENSE.txt

#pragma once

#include <vector>

#include <QAbstractListModel>
#include <QItemSelectionModel>

#include "Geometry.h"
#include "fileloader.h"
#include "GeometryMutator.h"

class QRegExp;

/// Collection of loaded data sets for use with Qt's model view architecture
///
/// Data sets can be points, lines or meshes.
class GeometryCollection : public QAbstractListModel
{
    Q_OBJECT
    public:
        typedef std::vector<std::shared_ptr<Geometry>> GeometryVec;

        GeometryCollection(QObject * parent = nullptr);

        /// Get current list of geometries
        const GeometryVec& get() const { return m_geometries; }

        /// Remove all geometries from the list
        void clear();
        /// Remove and unload all geometries whose filenames matched by given QRegExp object
        void unloadFiles(const QRegExp & filenameRegex);

        /// Find the first index to a geometry with label matching the given pattern
        QModelIndex findLabel(const QRegExp & labelPattern);

        // Specialisation of QAbstractListModel
        virtual int rowCount(const QModelIndex & parent = QModelIndex()) const;
        virtual QVariant data(const QModelIndex & index, int role = Qt::DisplayRole) const;
        virtual Qt::ItemFlags flags(const QModelIndex& index) const;
        virtual bool setData(const QModelIndex & index, const QVariant & value,
                             int role = Qt::EditRole);

        virtual bool removeRows(int row, int count, const QModelIndex& parent = QModelIndex());


    public slots:
        /// Add to the list of loaded geometries
        ///
        /// If `reloaded` is true, search existing geometry for
        /// `geom->fileName()` and if found, replace the existing geometry.
        ///
        /// If `replaceLabel` is true, search existing geometry for a matching
        /// `geom->label()` and replace the existing geometry if found.
        void addGeometry(std::shared_ptr<Geometry> geom, bool replaceLabel = false, bool reloaded = false);
        void mutateGeometry(std::shared_ptr<GeometryMutator> mutator);

    private:
        void loadPointFilesImpl(const QStringList& fileNames, bool removeAfterLoad);
        int findMatchingRow(const QRegExp & filenameRegex);

        GeometryVec m_geometries;
};
