/*
 * Copyright 2013 Canonical Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Authors:
 *      Michael Zanetti <michael.zanetti@canonical.com>
 */

#include "launcheritem.h"
#include "quicklistmodel.h"

#include <libintl.h>

LauncherItem::LauncherItem(const QString &appId, const QString &name, const QString &icon, QObject *parent) :
    LauncherItemInterface(parent),
    m_appId(appId),
    m_name(name),
    m_icon(icon),
    m_pinned(false),
    m_running(false),
    m_recent(false),
    m_progress(-1),
    m_count(0),
    m_countVisible(false),
    m_focused(false),
    m_alerting(false),
    m_quickList(new QuickListModel(this))
{
    QuickListEntry nameAction;
    nameAction.setActionId(QStringLiteral("launch_item"));
    nameAction.setText(m_name);
    nameAction.setHasSeparator(true);
    m_quickList->appendAction(nameAction);

    QuickListEntry pinningAction;
    pinningAction.setActionId(QStringLiteral("pin_item"));
    pinningAction.setText(gettext("Pin shortcut"));
    pinningAction.setIsPrivate(true);
    m_quickList->appendAction(pinningAction);

    m_quitAction.setActionId(QStringLiteral("stop_item"));
    m_quitAction.setIcon(QStringLiteral("application-exit"));
    m_quitAction.setText(gettext("Quit"));
    m_quitAction.setIsPrivate(true);
}

QString LauncherItem::appId() const
{
    return m_appId;
}

QString LauncherItem::name() const
{
    return m_name;
}

void LauncherItem::setName(const QString &name)
{
    if (m_name != name) {
        m_name = name;
        QuickListEntry entry;
        entry.setActionId(QStringLiteral("launch_item"));
        entry.setText(m_name);
        m_quickList->updateAction(entry);
        Q_EMIT nameChanged(name);
    }
}

QString LauncherItem::icon() const
{
    return m_icon;
}

void LauncherItem::setIcon(const QString &icon)
{
    if (m_icon != icon) {
        m_icon = icon;
        Q_EMIT iconChanged(icon);
    }
}

QStringList LauncherItem::keywords() const
{
    return m_keywords;
}

void LauncherItem::setKeywords(const QStringList &keywords)
{
    if (m_keywords != keywords) {
        m_keywords = keywords;
        Q_EMIT keywordsChanged(keywords);
    }
}

bool LauncherItem::pinned() const
{
    return m_pinned;
}

void LauncherItem::setPinned(bool pinned)
{
    if (m_pinned != pinned) {
        m_pinned = pinned;
        Q_EMIT pinnedChanged(pinned);
    }

    // Even if pinned status didn't change, we want to update text in case
    // the locale has changed since we last set pinned status.
    QuickListEntry entry;
    entry.setActionId(QStringLiteral("pin_item"));
    entry.setText(pinned ? gettext("Unpin shortcut") : gettext("Pin shortcut"));
    entry.setIsPrivate(true);
    m_quickList->updateAction(entry);
}

bool LauncherItem::running() const
{
    return m_running;
}

void LauncherItem::setRunning(bool running)
{
    if (m_running != running) {
        m_running = running;
        if (m_running) { // add the quit action
            m_quickList->appendAction(m_quitAction);
        } else { // remove the quit action
            m_quickList->removeAction(m_quitAction);
        }
        Q_EMIT runningChanged(running);
    }
}

bool LauncherItem::recent() const
{
    return m_recent;
}

void LauncherItem::setRecent(bool recent)
{
    if (m_recent != recent) {
        m_recent = recent;
        Q_EMIT recentChanged(recent);
    }
}

int LauncherItem::progress() const
{
    return m_progress;
}

void LauncherItem::setProgress(int progress)
{
    if (m_progress != progress) {
        m_progress = progress;
        Q_EMIT progressChanged(progress);
    }
}

int LauncherItem::count() const
{
    return m_count;
}

void LauncherItem::setCount(int count)
{
    if (m_count != count) {
        m_count = count;
        Q_EMIT countChanged(count);
    }
}

bool LauncherItem::countVisible() const
{
    return m_countVisible;
}

void LauncherItem::setCountVisible(bool countVisible)
{
    if (m_countVisible != countVisible) {
        m_countVisible = countVisible;
        Q_EMIT countVisibleChanged(countVisible);
    }
}

bool LauncherItem::focused() const
{
    return m_focused;
}

void LauncherItem::setFocused(bool focused)
{
    if (m_focused != focused) {
        m_focused = focused;
        Q_EMIT focusedChanged(focused);
    }
}

bool LauncherItem::alerting() const
{
    return m_alerting;
}

void LauncherItem::setAlerting(bool alerting)
{
    if (m_alerting != alerting) {
        m_alerting = alerting;
        Q_EMIT alertingChanged(alerting);
    }
}

int LauncherItem::surfaceCount() const
{
    return m_surfaces.count();
}

void LauncherItem::setSurfaces(const QList<QPair<QString, QString> > &surfaces)
{
    if (m_surfaces != surfaces) {
        m_surfaces = surfaces;

        QList<QuickListEntry> removedEntries;
        for (int i = 0; i < m_quickList->rowCount(); ++i) {
            QuickListEntry entry = m_quickList->get(i);
            if (entry.actionId().startsWith(QStringLiteral("surface_"))) {
                removedEntries.append(entry);
            }
        }
        Q_FOREACH (const QuickListEntry &entry, removedEntries) {
            m_quickList->removeAction(entry);
        }
        for (int i = 0; i < surfaces.count(); ++i) {
            QuickListEntry entry;
            entry.setActionId(QStringLiteral("surface_") + surfaces.at(i).first);
            entry.setText(surfaces.at(i).second);
            entry.setIsPrivate(true);
            if (i == surfaces.count() - 1) {
                entry.setHasSeparator(true);
            }
            m_quickList->insertAction(entry, i + 1);
        }

        Q_EMIT surfaceCountChanged(m_surfaces.count());
    }
}

uint LauncherItem::popularity() const
{
    return m_popularity;
}

void LauncherItem::setPopularity(uint popularity)
{
    if (m_popularity != popularity) {
        m_popularity = popularity;
        Q_EMIT popularityChanged(popularity);
    }
}

unity::shell::launcher::QuickListModelInterface *LauncherItem::quickList() const
{
    return m_quickList;
}
