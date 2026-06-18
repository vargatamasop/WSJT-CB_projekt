#include "qt_helpers.hpp"

#include <algorithm>

#include <QString>
#include <QCoreApplication>
#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QFont>
#include <QStandardPaths>
#include <QStringList>
#include <QWidget>
#include <QStyle>
#include <QVariant>
#include <QDateTime>

namespace
{
  char const * stable_application_name_property = "wsjtcb.settingsApplicationName";
  char const * stable_application_base_name = "WSJT-CB";

  bool copy_directory_missing_files (QString const& source_path, QString const& target_path)
  {
    QDir source_dir {source_path};
    if (!source_dir.exists ())
      {
        return false;
      }

    QDir target_dir;
    if (!target_dir.mkpath (target_path))
      {
        return false;
      }

    for (auto const& entry: source_dir.entryInfoList (QDir::NoDotAndDotDot | QDir::Files | QDir::Dirs))
      {
        auto target_file = QDir {target_path}.absoluteFilePath (entry.fileName ());
        if (entry.isDir ())
          {
            copy_directory_missing_files (entry.absoluteFilePath (), target_file);
          }
        else if (!QFileInfo::exists (target_file))
          {
            QFile::copy (entry.absoluteFilePath (), target_file);
          }
      }
    return true;
  }

  void migrate_legacy_writable_location (QString const& target_path)
  {
    QDir target_parent {QDir::cleanPath (QDir {target_path}.absoluteFilePath (".."))};
    QFileInfo target_info {target_path};
    auto legacy_dirs = target_parent.entryInfoList (QStringList {QString {stable_application_base_name} + "*"},
                                                    QDir::Dirs | QDir::NoDotAndDotDot);
    std::sort (legacy_dirs.begin (), legacy_dirs.end (), [] (QFileInfo const& lhs, QFileInfo const& rhs) {
        return lhs.lastModified () > rhs.lastModified ();
      });

    for (auto const& legacy_info: legacy_dirs)
      {
        if (legacy_info.absoluteFilePath () != target_info.absoluteFilePath ())
          {
            copy_directory_missing_files (legacy_info.absoluteFilePath (), target_path);
          }
      }
  }
}

QString font_as_stylesheet (QFont const& font)
{
  QString font_weight;
  switch (font.weight ())
    {
    case QFont::Light: font_weight = "light"; break;
    case QFont::Normal: font_weight = "normal"; break;
    case QFont::DemiBold: font_weight = "demibold"; break;
    case QFont::Bold: font_weight = "bold"; break;
    case QFont::Black: font_weight = "black"; break;
    }
  return QString {
      " font-family: %1;\n"
      " font-size: %2pt;\n"
      " font-style: %3;\n"
      " font-weight: %4;\n"}
  .arg (font.family ())
     .arg (font.pointSize ())
     .arg (font.styleName ())
     .arg (font_weight);
}

QString wsjtcb_stable_application_name ()
{
  auto app = QCoreApplication::instance ();
  auto name = app ? app->property (stable_application_name_property).toString ().trimmed () : QString {};
  return name.isEmpty () ? QCoreApplication::applicationName () : name;
}

QString wsjtcb_writable_location (QStandardPaths::StandardLocation location)
{
  auto path = QStandardPaths::writableLocation (location);
  auto stable_name = wsjtcb_stable_application_name ();
  auto visible_name = QCoreApplication::applicationName ();
  QFileInfo path_info {path};
  if (!stable_name.isEmpty () && !visible_name.isEmpty () && path_info.fileName () == visible_name)
    {
      path = path_info.dir ().absoluteFilePath (stable_name);
    }
  migrate_legacy_writable_location (path);
  return path;
}

void update_dynamic_property (QWidget * widget, char const * property, QVariant const& value)
{
  widget->setProperty (property, value);
  widget->style ()->unpolish (widget);
  widget->style ()->polish (widget);
  widget->update ();
}

QDateTime qt_round_date_time_to (QDateTime dt, int milliseconds)
{
  dt.setMSecsSinceEpoch (dt.addMSecs (milliseconds / 2).toMSecsSinceEpoch () / milliseconds * milliseconds);
  return dt;
}

QDateTime qt_truncate_date_time_to (QDateTime dt, int milliseconds)
{
  dt.setMSecsSinceEpoch (dt.toMSecsSinceEpoch () / milliseconds * milliseconds);
  return dt;
}
