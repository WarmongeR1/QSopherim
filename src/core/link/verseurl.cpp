/***************************************************************************
openBibleViewer - Bible Study Tool
Copyright (C) 2009-2011 Paul Walger
This program is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the Free
Software Foundation; either version 3 of the License, or (at your option)
any later version.
This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.
You should have received a copy of the GNU General Public License along with
this program; if not, see <http://www.gnu.org/licenses/>.
*****************************************************************************/
#include "verseurl.h"
#include "debughelper.h"
#include <QtCore/QHashIterator>
#include <QtCore/QStringList>
VerseUrl::VerseUrl()
{
    m_isValid = true;
}
VerseUrl::VerseUrl(const VerseUrlRange &range)
{
    addRange(range);
}

void VerseUrl::setParam(const QString &name, const QString &value)
{
    //check if name or value contains "/" or "," or "="
    m_params.insert(name, value);
}

bool VerseUrl::hasParam(const QString &name) const
{
    //myDebug() << m_params;
    return m_params.contains(name);
}
QString VerseUrl::getParam(const QString &name) const
{
    return m_params.value(name, "");
}
void VerseUrl::addRange(const VerseUrlRange &range)
{
    m_ranges.append(range);
}
void VerseUrl::addRanges(const QList<VerseUrlRange> &ranges)
{
    m_ranges.append(ranges);
}

QString VerseUrl::toString() const
{
    QString ret = "";
    foreach(const VerseUrlRange range, m_ranges) {
        if(!ret.isEmpty())
            ret += "|";//seperator
        else
            ret += "verse:/";

        if(range.module() == VerseUrlRange::LoadModuleByID) {
            ret += QString::number(range.moduleID());
        } else if(range.module() == VerseUrlRange::LoadCurrentModule) {
            ret += "current";
        } else if(range.module() == VerseUrlRange::LoadModuleByUID) {
            ret += "uid=" + range.moduleUID();
        }
        ret += ",";
        //Book
        if(range.book() == VerseUrlRange::LoadBookByID) {
            ret += QString::number(range.bookID());
        } else if(range.book() == VerseUrlRange::LoadCurrentBook) {
            ret += "current";
        } else if(range.book() == VerseUrlRange::LoadFirstBook) {
            ret += "first";
        } else if(range.book() == VerseUrlRange::LoadLastBook) {
            ret += "last";
        }

        ret += ",";

        //Chapter
        if(range.chapter() == VerseUrlRange::LoadChapterByID) {
            ret += QString::number(range.chapterID());
        } else if(range.chapter() == VerseUrlRange::LoadCurrentChapter) {
            ret += "current";
        } else if(range.chapter() == VerseUrlRange::LoadFirstChapter) {
            ret += "first";
        } else if(range.chapter() == VerseUrlRange::LoadLastChapter) {
            ret += "last";
        }

        ret += ",";
        //Verse
        if(range.startVerse() == VerseUrlRange::LoadVerseByID) {
            ret += QString::number(range.startVerseID());
        } else if(range.startVerse() == VerseUrlRange::LoadCurrentVerse) {
            ret += "current";
        } else if(range.startVerse() == VerseUrlRange::LoadFirstVerse) {
            ret += "first";
        } else if(range.startVerse() == VerseUrlRange::LoadLastVerse) {
            ret += "last";
        }
        //todo: start verse should not be LoadAllVerse
        if((range.startVerse() != range.endVerse() && range.endVerse() != VerseUrlRange::LoadVerseNotSet) ||
                (range.startVerse() == range.endVerse() && range.startVerse() == VerseUrlRange::LoadVerseByID && range.startVerseID() != range.endVerseID() && range.endVerseID() != -1)) {
            ret += "-";
            if(range.endVerse() == VerseUrlRange::LoadVerseByID) {
                ret += QString::number(range.endVerseID());
            } else if(range.endVerse() == VerseUrlRange::LoadCurrentVerse) {
                ret += "current";
            } else if(range.endVerse() == VerseUrlRange::LoadFirstVerse) {
                ret += "first";
            } else if(range.endVerse() == VerseUrlRange::LoadLastVerse) {
                ret += "last";
            }
        }
        if(range.activeVerse() == VerseUrlRange::LoadVerseByID) {
            ret += ",active=" + QString::number(range.activeVerseID());
        }
        if(!m_params.isEmpty()) {
            QHashIterator<QString, QString> i(m_params);
            while(i.hasNext()) {
                i.next();
                ret += "," + i.key() + "=" + i.value();
            }
        }

    }

    return ret;
}
bool VerseUrl::fromString(QString url)
{
    m_ranges.clear();

    //verse:/moduleID,bookID,chapterID,verseID-verseID,otherStuf=otherValue
    if(!url.startsWith("verse:/")) {
        m_isValid = false;
        return false;
    }
    url.remove(0, 7); // remove verse:/
    QStringList urls = url.split("|");
    foreach(const QString nUrl, urls) {
        VerseUrlRange range;
        const QStringList params = nUrl.split(",");
        for(int i = 0; i < params.size(); ++i) {
            QString p = params.at(i);
            QString p2;
            if(i == 0) {//Bible
                if(p == "current") {
                    range.setModule(VerseUrlRange::LoadCurrentModule);
                } else if(p.startsWith("uid=")) {
                    range.setModule(p.remove(0, 4));
                } else {
                    range.setModule(p.toInt());
                }
            } else if(i == 1) {//Book
                if(p == "current") {
                    range.setBook(VerseUrlRange::LoadCurrentBook);
                } else if(p == "first") {
                    range.setBook(VerseUrlRange::LoadFirstBook);
                } else if(p == "last") {
                    range.setBook(VerseUrlRange::LoadLastBook);
                } else {
                    range.setBook(p.toInt());
                }

            } else if(i == 2) {

                if(p == "current") {
                    range.setChapter(VerseUrlRange::LoadCurrentChapter);
                } else if(p == "first") {
                    range.setChapter(VerseUrlRange::LoadFirstChapter);
                } else if(p == "last") {
                    range.setChapter(VerseUrlRange::LoadLastChapter);
                } else {
                    range.setChapter(p.toInt());
                }
            } else if(i == 3) {
                if(p.contains("-")) {
                    QStringList tmp = p.split("-");
                    p = tmp.first();
                    p2 = tmp.last();
                }
                if(p == "current") {
                    range.setStartVerse(VerseUrlRange::LoadCurrentVerse);
                } else if(p == "first") {
                    range.setStartVerse(VerseUrlRange::LoadFirstVerse);
                } else if(p == "last") {
                    range.setStartVerse(VerseUrlRange::LoadLastVerse);
                }  else {
                    range.setStartVerse(p.toInt());
                }

                if(p2 == "current") {
                    range.setEndVerse(VerseUrlRange::LoadCurrentVerse);
                } else if(p2 == "first") {
                    range.setEndVerse(VerseUrlRange::LoadFirstVerse);
                } else if(p2 == "last") {
                    range.setEndVerse(VerseUrlRange::LoadLastVerse);
                } else if(!p2.isEmpty()) {
                    range.setEndVerse(p2.toInt());
                }
            } else {
                if(p.contains("=")) {
                    const QStringList s = p.split("=");
                    const QString key = s.first();
                    const QString value = s.last();
                    if(key == "active") {
                        range.setActiveVerse(value.toInt());
                    } else {
                        m_params.insert(key, value);
                    }
                } else {
                    m_params.insert(p, "true");
                }
            }
        }
        m_ranges.append(range);
    }
    return true;
}
bool VerseUrl::fromMscope(const QString &url)
{
    const QStringList list = url.split(";");
    if(list.size() < 2)
        return false;
    const int bookID = list.at(0).toInt() - 1;
    const int chapterID = list.at(1).toInt() - 1;

    VerseUrlRange range;
    range.setModule(VerseUrlRange::LoadCurrentModule);
    range.setBook(bookID);
    range.setChapter(chapterID);

    if(list.size() == 3) {
        range.setStartVerse(list.at(2).toInt() - 1);
    } else {
        range.setWholeChapter();
    }
    clearRanges();
    m_ranges.append(range);
    return true;
}

QList<VerseUrlRange> VerseUrl::ranges() const
{
    return m_ranges;
}
void VerseUrl::clearRanges()
{
    m_ranges.clear();
}
void VerseUrl::unsetParam(const QString &name)
{
    m_params.remove(name);
}
bool VerseUrl::contains(int moduleID, int bookID, int chapterID, int verseID) const
{
    foreach(const VerseUrlRange & range, m_ranges) {
        if(range.moduleID() == moduleID && range.bookID() == bookID && range.containsChapter(chapterID) && range.containsVerse(verseID))
            return true;
    }
    return false;
}
bool VerseUrl::contains(int moduleID, int bookID, int chapterID) const
{
    foreach(const VerseUrlRange & range, m_ranges) {
        if(range.moduleID() == moduleID && range.bookID() == bookID && range.containsChapter(chapterID))
            return true;
    }
    return false;
}
bool VerseUrl::isValid() const
{
    return m_isValid && !m_ranges.isEmpty();
}
void VerseUrl::setOpenToTransformation(bool open)
{
    QMutableListIterator<VerseUrlRange> i(m_ranges);
    while(i.hasNext()) {
        i.next().setOpenToTransformation(open);
    }
}

VerseUrl VerseUrl::applyUrl(const VerseUrl &url)
{
    if(this->ranges().isEmpty())
        return url;

    VerseUrl newUrl = url;
    newUrl.clearRanges();
    for(int i = 0; i < url.ranges().size(); i++) {
        VerseUrlRange range = url.ranges().at(i);
        int id;
        if(this->ranges().size() < url.ranges().size()) {
            id = this->ranges().size() - 1;
        } else {
            id = i;
        }

        VerseUrlRange newRange = range;
        VerseUrlRange mRange = this->ranges().at(id);
        if(range.module() == VerseUrlRange::LoadCurrentModule) {
            newRange.setModule(this->ranges().at(id).module());
            if(mRange.module() == VerseUrlRange::LoadModuleByID) {
                newRange.setModule(mRange.moduleID());
            }
            if(mRange.module() == VerseUrlRange::LoadModuleByUID) {
                newRange.setModule(mRange.moduleUID());
            }
        }
        if(range.book() == VerseUrlRange::LoadCurrentBook) {
            newRange.setBook(mRange.book());
            if(mRange.book() == VerseUrlRange::LoadBookByID) {
                newRange.setBook(mRange.bookID());
            }
        }
        if(range.chapter() == VerseUrlRange::LoadCurrentChapter) {
            newRange.setChapter(mRange.chapter());
            if(mRange.chapter() == VerseUrlRange::LoadChapterByID) {
                newRange.setChapter(mRange.chapterID());
            }
        }

        if(range.startVerse() == VerseUrlRange::LoadCurrentVerse) {
            newRange.setStartVerse(mRange.startVerse());
            if(mRange.startVerse() == VerseUrlRange::LoadVerseByID) {
                newRange.setStartVerse(mRange.startVerseID());
            }
        }

        if(range.endVerse() == VerseUrlRange::LoadCurrentVerse) {
            newRange.setEndVerse(mRange.endVerse());
            if(mRange.endVerse() == VerseUrlRange::LoadVerseByID) {
                newRange.setEndVerse(mRange.endVerseID());
            }
        }
        newUrl.addRange(newRange);
        //todo: active Verse
    }
    return newUrl;
}
void VerseUrl::setModuleID(const int moduleID)
{
    QMutableListIterator<VerseUrlRange> i(m_ranges);
    while(i.hasNext()) {
        i.next().setModule(moduleID);
    }
}
bool VerseUrl::hasModuleID() const
{
    QListIterator<VerseUrlRange> i(m_ranges);
    while(i.hasNext()) {
        VerseUrlRange r = i.next();
        if(r.module() == VerseUrlRange::LoadModuleByID || r.module() == VerseUrlRange::LoadModuleByID)
            return true;
    }
    return false;
}
